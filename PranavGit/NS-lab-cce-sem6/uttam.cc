// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
 
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
 
NS_LOG_COMPONENT_DEFINE ("b2topo");
 
int
main (int argc, char *argv[])
{
  

  LogComponentEnable("b2topo",LOG_LEVEL_INFO);

  //uint32_t nCsma = 5;
  uint32_t maxBytes = 512;
  std::string datarate="0.5Mbps";
  std::string delay="2ms";

  CommandLine cmd (__FILE__);
  cmd.AddValue ("maxBytes",
                "Total number of bytes for application to send", maxBytes);
    cmd.AddValue ("datarate",
                "the datarate", datarate);
    cmd.AddValue ("delay",
                "the delay", delay);
  cmd.Parse (argc, argv);
  
 
 
  NodeContainer c;
  c.Create (10);
 
  NodeContainer c0 = NodeContainer (c.Get (0), c.Get (1), c.Get (2));
  NodeContainer c1 = NodeContainer (c.Get (3), c.Get (4), c.Get (5));
  NodeContainer c2 = NodeContainer (c.Get (6), c.Get (7), c.Get (8));

  NodeContainer p0 = NodeContainer (c.Get (2), c.Get (3));
  NodeContainer p1 = NodeContainer (c.Get (2), c.Get (6));
  NodeContainer p2 = NodeContainer (c.Get (8), c.Get (9));
  //NodeContainer p3 = NodeContainer (c.Get (5), c.Get (9));

  NS_LOG_INFO ("Build Topology.");
  PointToPointHelper pp;
  pp.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pp.SetChannelAttribute ("Delay", StringValue ("2ms"));

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
  
  
  NetDeviceContainer nd3, nd4, nd5, nd6;

  NetDeviceContainer nd0 = csma.Install (c0);
  NetDeviceContainer nd1 = csma.Install (c1);
  NetDeviceContainer nd2 = csma.Install (c2);  // First LAN
    

  nd3.Add( pp.Install (c.Get(2), c.Get(3)));
  nd4.Add( pp.Install (c.Get(2), c.Get(6)));
  nd5.Add( pp.Install (c.Get(8), c.Get(9)));
  nd6.Add( pp.Install (c.Get(5), c.Get(9)));


  InternetStackHelper stack;
  stack.Install (c);


  Ipv4AddressHelper address1, address2, address3, address4, address5, address6, address7;
  address1.SetBase ("10.1.1.0", "255.255.255.0");
  address2.SetBase ("10.1.2.0", "255.255.255.0");
  address3.SetBase ("10.1.3.0", "255.255.255.0");
  address4.SetBase ("10.1.4.0", "255.255.255.0");
  address5.SetBase ("10.1.5.0", "255.255.255.0");
  address6.SetBase ("10.1.6.0", "255.255.255.0");
  //address7.SetBase ("10.1.7.0", "255.255.255.0");
  

  Ipv4InterfaceContainer interfaces1 = address1.Assign (nd0);
  Ipv4InterfaceContainer interfaces2 = address2.Assign (nd1);
  Ipv4InterfaceContainer interfaces3 = address3.Assign (nd2);
  Ipv4InterfaceContainer interfaces4 = address4.Assign (nd3);
  Ipv4InterfaceContainer interfaces5 = address5.Assign (nd4);
  Ipv4InterfaceContainer interfaces6 = address6.Assign (nd5);
  //Ipv4InterfaceContainer interfaces7 = address7.Assign (nd6);




  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps1 = echoServer.Install (c.Get (9));
  serverApps1.Start (Seconds (4.8));
  serverApps1.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (interfaces6.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (6));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  

  ApplicationContainer clientApps1 = echoClient.Install (c.Get (0));
  clientApps1.Start (Seconds (4.8));
  clientApps1.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  NS_LOG_INFO ("Configure Tracing.");
 
  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("practice3.tr");
  pp.EnableAsciiAll (stream);
  csma.EnableAsciiAll(stream);


  NS_LOG_INFO ("Run Simulation.");
  AnimationInterface anim ("practice3.xml");
  anim.SetConstantPosition ( c.Get(0), 1, 20);
  anim.SetConstantPosition ( c.Get(1), 10, 20);
  anim.SetConstantPosition ( c.Get(2), 20, 20);
  anim.SetConstantPosition ( c.Get(3), 26, 10);
  anim.SetConstantPosition ( c.Get(4), 36, 10);
  anim.SetConstantPosition ( c.Get(5), 46, 10);
  anim.SetConstantPosition ( c.Get(9), 80, 20);
  anim.SetConstantPosition ( c.Get(6), 26, 60);
  anim.SetConstantPosition ( c.Get(7), 36, 60);
  anim.SetConstantPosition ( c.Get(8), 46, 60);
  

    

  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
 


  





 
  
}