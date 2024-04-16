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

NS_LOG_COMPONENT_DEFINE ("CsmaMulticastExample");

int 
main (int argc, char *argv[])
{
  //CommandLine cmd (__FILE__);
  //cmd.Parse (argc, argv);
  uint32_t maxBytes = 512;
  NS_LOG_INFO ("Create nodes.");
  NodeContainer c;
  c.Create (12);
  // We will later want two subcontainers of these nodes, for the two LANs
  NodeContainer c0 = NodeContainer (c.Get (0), c.Get (1), c.Get (2));
  c0.Add(c.Get(3));

  NodeContainer c1 = NodeContainer (c.Get (10), c.Get (9), c.Get (8));
  NodeContainer p0 = NodeContainer (c.Get (5), c.Get (6), c.Get (4));
  p0.Add(c.Get(0));
  p0.Add(c.Get(2));
  p0.Add(c.Get(11));
  p0.Add(c.Get(10));
  p0.Add(c.Get(8));
  p0.Add(c.Get(7));


  NS_LOG_INFO ("Build Topology.");
  NetDeviceContainer nd3, nd4, nd5, nd6, nd7, nd8;
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

  nd3.Add(p2p.Install(c.Get(5), c.Get(6)));
  nd4.Add(p2p.Install(c.Get(5), c.Get(4)));
  nd5.Add(p2p.Install(c.Get(5), c.Get(0)));
  nd6.Add(p2p.Install(c.Get(2), c.Get(11)));
  nd7.Add(p2p.Install(c.Get(2), c.Get(10)));
  nd8.Add(p2p.Install(c.Get(8), c.Get(7)));
  
  // We will use these NetDevice containers later, for IP addressing
  NetDeviceContainer nd0 = csma.Install (c0);  // First LAN
  NetDeviceContainer nd1 = csma.Install (c1);  // Second LAN
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
  address6.SetBase ("10.1.6.0", "255.255.255.0");
  address7.SetBase ("10.1.7.0", "255.255.255.0");
  address8.SetBase ("10.1.8.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces1 = address1.Assign (nd0);
  Ipv4InterfaceContainer interfaces2 = address2.Assign (nd1);
  Ipv4InterfaceContainer interfaces3 = address3.Assign (nd3);
  Ipv4InterfaceContainer interfaces4 = address4.Assign (nd4);
  Ipv4InterfaceContainer interfaces5 = address5.Assign (nd5);
  Ipv4InterfaceContainer interfaces6 = address6.Assign (nd6);
  Ipv4InterfaceContainer interfaces7 = address7.Assign (nd7);
  Ipv4InterfaceContainer interfaces8 = address8.Assign (nd8);

  uint16_t port = 9; // well-known echo port number
BulkSendHelper source ("ns3::TcpSocketFactory",
InetSocketAddress (interfaces4.GetAddress (1), port));   //PUT HERE DESTINATION KA  INTERFACE 
// Set the amount of data to send in bytes. Zero is unlimited.
source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
ApplicationContainer sourceApps = source.Install (c.Get (11));   //SOURCE NODE  
sourceApps.Start (Seconds (2.0));
sourceApps.Stop (Seconds (5.3));

PacketSinkHelper sink ("ns3::TcpSocketFactory",
InetSocketAddress (Ipv4Address::GetAny (), port));
ApplicationContainer sinkApps = sink.Install (c.Get (4));  // DESTINATION NODE
sinkApps.Start (Seconds (2.0));
sinkApps.Stop (Seconds (5.3));

UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps1 = echoServer.Install (c.Get (7));
  serverApps1.Start (Seconds (4.8));
  serverApps1.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (interfaces8.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (6));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps1 = echoClient.Install (c.Get (6));
  clientApps1.Start (Seconds (4.8));
  clientApps1.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  NS_LOG_INFO ("Configure Tracing.");
 
  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("practice.tr");
  p2p.EnableAsciiAll (stream);
  csma.EnableAsciiAll(stream);


  NS_LOG_INFO ("Run Simulation.");
  AnimationInterface anim ("practice1.xml");
  anim.SetConstantPosition ( c.Get(5), 1, 20);
  anim.SetConstantPosition ( c.Get(0), 20, 20);
  anim.SetConstantPosition ( c.Get(1), 30, 20);
  anim.SetConstantPosition ( c.Get(2), 40, 20);
  anim.SetConstantPosition ( c.Get(6), 1, 40);
  anim.SetConstantPosition ( c.Get(4), 15, 40);
  anim.SetConstantPosition ( c.Get(3), 30, 60);
  anim.SetConstantPosition ( c.Get(10), 60, 5);
  anim.SetConstantPosition ( c.Get(11), 50, 50);
  anim.SetConstantPosition(c.Get(8), 70, 5);
  anim.SetConstantPosition ( c.Get(7), 80, 5);
  anim.SetConstantPosition ( c.Get(9), 65, 50);

    

  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
