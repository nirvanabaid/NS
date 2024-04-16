#include <iostream>
#include <fstream>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-module.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("CsmaMulticastExample");

int 
main (int argc, char *argv[])
{
  //
  // Users may find it convenient to turn on explicit debugging
  // for selected modules; the below lines suggest how to do this
  //
  LogComponentEnable ("CsmaMulticastExample", LOG_LEVEL_INFO);
LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);
  //
  // Set up default values for the simulation.
  //
  // Select DIX/Ethernet II-style encapsulation (no LLC/Snap header)
  Config::SetDefault ("ns3::CsmaNetDevice::EncapsulationMode", StringValue ("Dix"));

  // Allow the user to override any of the defaults at
  // run-time, via command-line arguments
  //CommandLine cmd (_FILE_);
  //cmd.Parse (argc, argv);

  NS_LOG_INFO ("Create nodes.");
  NodeContainer c;
  c.Create (10);
  // We will later want two subcontainers of these nodes, for the two LANs
  NodeContainer c0 = NodeContainer (c.Get (0), c.Get (1), c.Get (2));
  NodeContainer c1 = NodeContainer (c.Get (7), c.Get (3), c.Get (4));
  NodeContainer c2 = NodeContainer (c.Get (8), c.Get (5), c.Get (6));
  NodeContainer p1 = NodeContainer (c.Get (9), c.Get(4));
  NodeContainer p2 = NodeContainer (c.Get (9), c.Get(6));
  NodeContainer p3 = NodeContainer (c.Get (2), c.Get(7));
  NodeContainer p4 = NodeContainer (c.Get (2), c.Get(8));


  NS_LOG_INFO ("Build Topology.");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  // We will use these NetDevice containers later, for IP addressing
  NetDeviceContainer nd0 = csma.Install (c0);  // First LAN
  NetDeviceContainer nd1 = csma.Install (c1);  // Second LAN
  NetDeviceContainer nd2 = csma.Install (c2);
  NetDeviceContainer nd3 = p2p.Install (p1);
   NetDeviceContainer nd4 = p2p.Install (p2);
    NetDeviceContainer nd5 = p2p.Install (p3);
     NetDeviceContainer nd6 = p2p.Install (p4);
  NS_LOG_INFO ("Add IP Stack.");
  InternetStackHelper internet;
  internet.Install (c);

  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4Addr;
  ipv4Addr.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4Addr.Assign (nd0);
  ipv4Addr.SetBase ("10.1.2.0", "255.255.255.0");
  ipv4Addr.Assign (nd1);
  ipv4Addr.SetBase ("10.1.3.0", "255.255.255.0");
  ipv4Addr.Assign (nd2);
  ipv4Addr.SetBase ("10.1.4.0", "255.255.255.0");
  ipv4Addr.Assign (nd3);
  ipv4Addr.SetBase ("10.1.5.0", "255.255.255.0");
  ipv4Addr.Assign (nd4);
  ipv4Addr.SetBase ("10.1.6.0", "255.255.255.0");
  ipv4Addr.Assign (nd5);
  ipv4Addr.SetBase ("10.1.7.0", "255.255.255.0");
  ipv4Addr.Assign (nd6);


  NS_LOG_INFO ("Configure multicasting.");
  //
  // Now we can configure multicasting.  As described above, the multicast 
  // source is at node zero, which we assigned the IP address of 10.1.1.1 
  // earlier.  We need to define a multicast group to send packets to.  This
  // can be any multicast address from 224.0.0.0 through 239.255.255.255
  // (avoiding the reserved routing protocol addresses).
  //

  Ipv4Address multicastSource ("10.1.3.1");
  Ipv4Address multicastGroup ("225.1.2.4");
NS_LOG_INFO ("Configure multicasting.");
  // Now, we will set up multicast routing.  We need to do three things:
  // 1) Configure a (static) multicast route on node n2
  // 2) Set up a default multicast route on the sender n0 
  // 3) Have node n4 join the multicast group
  // We have a helper that can help us with static multicast
  Ipv4StaticRoutingHelper multicast;

  // 1) Configure a (static) multicast route on node n2 (multicastRouter)
  Ptr<Node> multicastRouter = c.Get (8);  // The node in question
  Ptr<NetDevice> inputIf = nd2.Get (0);  // The input NetDevice
  NetDeviceContainer outputDevices;  // A container of output NetDevices
  outputDevices.Add (nd6.Get (1));  // (we only need one NetDevice here)
  // outputDevices.Add (nd3.Get(0)); 
NS_LOG_INFO ("Configure multicasting.");
  multicast.AddMulticastRoute (multicastRouter, multicastSource, 
                               multicastGroup, inputIf, outputDevices);

  Ptr<Node> multicastRouter1 = c.Get (2);  // The node in question
  Ptr<NetDevice> inputIf1 = nd6.Get (0);  // The input NetDevice
  NetDeviceContainer outputDevices1;  // A container of output NetDevices
  outputDevices1.Add (nd0.Get (2));  // (we only need one NetDevice here)
  multicast.AddMulticastRoute (multicastRouter1, multicastSource, 
                             multicastGroup, inputIf1, outputDevices1);

     Ptr<Node> sender = c.Get (5);
  Ptr<NetDevice> senderIf = nd2.Get (1);
  multicast.SetDefaultMulticastRoute (sender, senderIf);
NS_LOG_INFO ("Configure multicasting.");

    Ipv4Address multicastSource1 ("10.1.4.1");
  Ipv4Address multicastGroup1 ("225.2.3.4");
                             
  
  Ptr<Node> multicastRouter2 = c.Get (4);  // The node in question
  Ptr<NetDevice> inputIf2 = nd3.Get (1);  // The input NetDevice
  NetDeviceContainer outputDevices2;  // A container of output NetDevices
  outputDevices2.Add (nd1.Get (2));  // (we only need one NetDevice here)
  multicast.AddMulticastRoute (multicastRouter2, multicastSource1, 
                          multicastGroup1, inputIf2, outputDevices2);
  
    Ptr<Node> multicastRouter3 = c.Get (7);  // The node in question
  Ptr<NetDevice> inputIf3 = nd1.Get (0);  // The input NetDevice
  NetDeviceContainer outputDevices3;  // A container of output NetDevices
  outputDevices3.Add (nd5.Get (1));  // (we only need one NetDevice here)
  multicast.AddMulticastRoute (multicastRouter3, multicastSource1, 
                          multicastGroup1, inputIf3, outputDevices3);
     Ptr<Node> multicastRouter4 = c.Get (2);  // The node in question
  Ptr<NetDevice> inputIf4 = nd5.Get (0);  // The input NetDevice
  NetDeviceContainer outputDevices4;  // A container of output NetDevices
  outputDevices4.Add (nd0.Get (2));  // (we only need one NetDevice here)
  multicast.AddMulticastRoute (multicastRouter4, multicastSource1, 
                          multicastGroup1, inputIf4, outputDevices4);


  // 2) Set up a default multicast route on the sender n0 


  Ptr<Node> sender1 = c.Get (9);
  Ptr<NetDevice> senderIf1 = nd3.Get (0);
  multicast.SetDefaultMulticastRoute (sender1, senderIf1);

  //
  // Create an OnOff application to send UDP datagrams from node zero to the
  // multicast group (node four will be listening).
  //
  NS_LOG_INFO ("Create Applications.");

  uint16_t port = 9; // well-known echo port number

BulkSendHelper source ("ns3::TcpSocketFactory",
InetSocketAddress (multicastGroup1, port));
// Set the amount of data to send in bytes. Zero is unlimited.
source.SetAttribute ("MaxBytes", UintegerValue (1024));
ApplicationContainer sourceApps = source.Install (c.Get (9));
sourceApps.Start (Seconds (0.0));
sourceApps.Stop (Seconds (10.0));
//
// Create a PacketSinkApplication and install it on node 1
//
PacketSinkHelper sink1 ("ns3::TcpSocketFactory",
InetSocketAddress (Ipv4Address::GetAny (), port));
ApplicationContainer sinkApps1 = sink1.Install (c.Get (0));
sinkApps1.Start (Seconds (0.0));
sinkApps1.Stop (Seconds (10.0));
//
// Set up tracing if enabled
//


uint16_t multicastPort = 9;   // Discard port (RFC 863)
  // Configure a multicast packet generator that generates a packet
  // every few seconds
  OnOffHelper onoff ("ns3::UdpSocketFactory",Address(InetSocketAddress(multicastGroup, multicastPort)));
  onoff.SetConstantRate (DataRate ("255b/s"));
  onoff.SetAttribute ("PacketSize", UintegerValue (128));

  ApplicationContainer srcC = onoff.Install (c.Get (5));
  //ApplicationContainer srcC1 = onoff.Install (p1.Get (0));
  //UdpEchoClientHelper echoClient (multicastGroup, multicastPort);
  //echoClient.SetAttribute ("MaxPackets", UintegerValue (6));
  //echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  //echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  NS_LOG_INFO ("Configure multicasting.");
  // Tell the application when to start and stop.
  //
  srcC.Start (Seconds (1.));
  srcC.Stop (Seconds (10.));
  //srcC1.Start (Seconds (1.));
  //srcC1.Stop (Seconds (10.));
  //ApplicationContainer clientApps = echoClient.Install (c.Get (9));
  //clientApps.Start (Seconds (2.0));
  //clientApps.Stop (Seconds (10.0));

  //UdpEchoServerHelper echoServer (9);

  //ApplicationContainer serverApps = echoServer.Install (c.Get(0));
  //serverApps.Start (Seconds (1.0));
  //serverApps.Stop (Seconds (10.0));

  // Create an optional packet sink to receive these packets
  PacketSinkHelper sink ("ns3::UdpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), multicastPort));

  ApplicationContainer sinkC = sink.Install (c0.Get (1)); // Node n0
  // Start the sink
  sinkC.Start (Seconds (1.0));
  sinkC.Stop (Seconds (10.0));

  NS_LOG_INFO ("Configure Tracing.");
  //
  // Configure tracing of all enqueue, dequeue, and NetDevice receive events.
  // Ascii trace output will be sent to the file "csma-multicast.tr"
  //
  AsciiTraceHelper ascii;
  csma.EnableAsciiAll (ascii.CreateFileStream ("prepkc1.tr"));
  p2p.EnableAsciiAll (ascii.CreateFileStream ("prepkc1.tr"));

  // Also configure some tcpdump traces; each interface will be traced.
  // The output files will be named:
  //     csma-multicast-<nodeId>-<interfaceId>.pcap
  // and can be read by the "tcpdump -r" command (use "-tt" option to
  // display timestamps correctly)
  // csma.EnablePcapAll ("csma-multicast", false);

  //
  // Now, do the actual simulation.
  //
  NS_LOG_INFO ("Run Simulation.");
  AnimationInterface anim ("prepkc1.xml");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}