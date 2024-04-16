// Network topology
//                     		   n10       n8---n7
//                                / |        |
//                               /  ==========
//            n5-------n0  n1  n2       |
//            | \      |   |   |        n9
//            |  \     ==========
//            n6  n4       |
//                         n3


#include <iostream>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("C2MID");

int 
main (int argc, char *argv[])
{
  Config::SetDefault ("ns3::CsmaNetDevice::EncapsulationMode", StringValue ("Dix"));
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);
  uint32_t maxBytes = 512;
  NS_LOG_INFO ("Create nodes.");
  NodeContainer c;
  c.Create (10);
  // We will later want two subcontainers of these nodes, for the two LANs
  NodeContainer c0 = NodeContainer (c.Get (0), c.Get (2), c.Get (1));
  //c0.Add(c.Get(3));
  
  Address serverAddress;

  NodeContainer c1 = NodeContainer (c.Get (1), c.Get (3), c.Get (4), c.Get (5));
  NodeContainer c2 = NodeContainer (c.Get (7), c.Get (8), c.Get (9));
  NodeContainer p0 = NodeContainer (c.Get (5), c.Get (6));
  NodeContainer p1 = NodeContainer (c.Get (6), c.Get (7));
  
  

  NS_LOG_INFO ("Build Topology.");
  NetDeviceContainer nd3, nd4;
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

  nd3.Add(p2p.Install(c.Get(5), c.Get(6)));
  nd4.Add(p2p.Install(c.Get(6), c.Get(7)));
  
  
  // We will use these NetDevice containers later, for IP addressing
  NetDeviceContainer nd0 = csma.Install (c0);  // First LAN
  NetDeviceContainer nd1 = csma.Install (c1);
  NetDeviceContainer nd2 = csma.Install (c2);  // Second LAN
  //NetDeviceContainer nd3 = p2p.Install (p1);
  NS_LOG_INFO ("Add IP Stack.");
  InternetStackHelper internet;
  internet.Install (c);

  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper address1, address2, address3, address4, address5, address6, address7, address8;
  address1.SetBase ("10.1.1.0", "255.255.255.0");
  address2.SetBase ("10.1.2.0", "255.255.255.0");
  address3.SetBase ("10.1.3.0", "255.255.255.0");
  address4.SetBase ("10.1.4.0", "255.255.255.0");
  address5.SetBase ("10.1.5.0", "255.255.255.0");
  //address6.SetBase ("10.1.6.0", "255.255.255.0");
  //address7.SetBase ("10.1.7.0", "255.255.255.0");
  //address8.SetBase ("10.1.8.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces1 = address1.Assign (nd0);
  Ipv4InterfaceContainer interfaces2 = address2.Assign (nd1);
  Ipv4InterfaceContainer interfaces3 = address3.Assign (nd2);
  Ipv4InterfaceContainer interfaces4 = address4.Assign (nd3);
  Ipv4InterfaceContainer interfaces5 = address5.Assign (nd4);
  

  //uint16_t port = 9; // well-known echo port number

  Ipv4Address multicastSource ("10.1.1.1");
  Ipv4Address multicastGroup ("225.1.2.4");

  Ipv4StaticRoutingHelper multicast;

  // 1) Configure a (static) multicast route on node n2 (multicastRouter)
  Ptr<Node> multicastRouter = c.Get (1);  // The node in question
  Ptr<NetDevice> inputIf = nd0.Get (2);  // The input NetDevice
  NetDeviceContainer outputDevices;  // A container of output NetDevices
  outputDevices.Add (nd1.Get (0));  // (we only need one NetDevice here)
  // outputDevices.Add (nd3.Get(0)); 

  multicast.AddMulticastRoute (multicastRouter, multicastSource, 
                               multicastGroup, inputIf, outputDevices);

  
  Ptr<Node> multicastRouter1 = c.Get (5);  // The node in question
  Ptr<NetDevice> inputIf1 = nd1.Get (3);  // The input NetDevice
  NetDeviceContainer outputDevices1;  // A container of output NetDevices
  outputDevices1.Add (nd3.Get (0));  // (we only need one NetDevice here)
  // outputDevices.Add (nd3.Get(0)); 

  multicast.AddMulticastRoute (multicastRouter1, multicastSource, 
                               multicastGroup, inputIf1, outputDevices1);

  
   Ptr<Node> multicastRouter2 = c.Get (6);  // The node in question
  Ptr<NetDevice> inputIf2 = nd3.Get (1);  // The input NetDevice
  NetDeviceContainer outputDevices2;  // A container of output NetDevices
  outputDevices2.Add (nd4.Get (0));  // (we only need one NetDevice here)
  // outputDevices.Add (nd2.Get(0)); 

  multicast.AddMulticastRoute (multicastRouter2, multicastSource, 
                               multicastGroup, inputIf2, outputDevices2);
  
  


    
  Ptr<Node> sender = c.Get (0);
  Ptr<NetDevice> senderIf = nd0.Get (0);
  multicast.SetDefaultMulticastRoute (sender, senderIf);



  uint16_t multicastPort = 9;

  OnOffHelper onoff ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (multicastGroup, multicastPort)));
  onoff.SetConstantRate (DataRate ("255b/s"));
  onoff.SetAttribute ("PacketSize", UintegerValue (128));

  ApplicationContainer srcC = onoff.Install (c0.Get (0));
  srcC.Start (Seconds (1.));
  srcC.Stop (Seconds (10.));


  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), multicastPort));

  ApplicationContainer sinkC = sink.Install (p1.Get (1)); 
  // Start the sink
  sinkC.Start (Seconds (1.0));
  sinkC.Stop (Seconds (10.0));


   uint16_t port = 9;  // well-known echo port number
  UdpEchoServerHelper server (port);
  ApplicationContainer apps = server.Install (c.Get (9));
  apps.Start (Seconds (20.0));
  apps.Stop (Seconds (30.0));

  serverAddress = Address(interfaces3.GetAddress (2));

  uint32_t packetSize = 1024;
  uint32_t maxPacketCount = 1;
  Time interPacketInterval = Seconds (1.);
  UdpEchoClientHelper client (serverAddress, port);
  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client.SetAttribute ("PacketSize", UintegerValue (packetSize));
  apps = client.Install (c.Get (7));
  apps.Start (Seconds (22.0));
  apps.Stop (Seconds (30.0));

 


  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  NS_LOG_INFO ("Configure Tracing.");
 
  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("mark2.tr");
  p2p.EnableAsciiAll (stream);
  csma.EnableAsciiAll(stream);


  NS_LOG_INFO ("Run Simulation.");
  AnimationInterface anim ("mark2.xml");
  anim.SetConstantPosition ( c.Get(0), 1, 20);
  anim.SetConstantPosition ( c.Get(2), 20, 20);
  anim.SetConstantPosition ( c.Get(1), 30, 20);
  anim.SetConstantPosition ( c.Get(3), 40, 40);
  anim.SetConstantPosition ( c.Get(4), 50, 40);
  anim.SetConstantPosition ( c.Get(5), 60, 40);
  anim.SetConstantPosition ( c.Get(6), 60, 60);
  anim.SetConstantPosition ( c.Get(7), 60, 70);
  anim.SetConstantPosition ( c.Get(8), 68, 70);
  anim.SetConstantPosition ( c.Get(9), 74, 70);
  

    

  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
