/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h"
#include "ns3/animation-interface.h"
#include "ns3/energy-module.h"
#include "ns3/wifi-radio-energy-model-helper.h"
#include "ns3/constant-velocity-mobility-model.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0


using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("SecondScriptExample");

void TearDownLink (Ptr<Node> nodeA, Ptr<Node> nodeB, uint32_t interfaceA, uint32_t interfaceB)
{
  nodeA->GetObject<Ipv4> ()->SetDown (interfaceA);
  nodeB->GetObject<Ipv4> ()->SetDown (interfaceB);
}

void Stop_node(Ptr<ConstantVelocityMobilityModel> m)
{
  m->SetVelocity(Vector(0.0,0.0,0.0));
}

void Join_link (Ptr<Node> nodeA, Ptr<Node> nodeB, uint32_t interfaceA, uint32_t interfaceB)
{
  nodeA->GetObject<Ipv4> ()->SetUp(interfaceA);
  nodeB->GetObject<Ipv4> ()->SetUp (interfaceB);
}

int 
main (int argc, char *argv[])
{
  LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);
  //LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);

  uint32_t maxBytes = 0;

  NodeContainer nodes,csma_nodes1,csma_nodes2,p2p1,p2p2,wireless_sta,wireless_ap, wireless;
  nodes.Create(13);

  csma_nodes1.Add(nodes.Get(0));//N0
  csma_nodes1.Add(nodes.Get(1));//N1
  csma_nodes1.Add(nodes.Get(2));//N2

  p2p1.Add(nodes.Get(2));//N2
  p2p1.Add(nodes.Get(3));//N3

  p2p2.Add(nodes.Get(9));//N9
  p2p2.Add(nodes.Get(10));//N10

  csma_nodes2.Add(nodes.Get(10));//N10
  csma_nodes2.Add(nodes.Get(11));//N11
  csma_nodes2.Add(nodes.Get(12));//N12

  wireless_ap.Add(nodes.Get(3));//N5
  wireless_ap.Add(nodes.Get(9));//N5
  for(int i = 4; i < 9; i++)
  {
    wireless_sta.Add(nodes.Get(i));//N4-N8
  }
  //wifi
  WifiMacHelper wifiMac;
  WifiHelper wifiHelper;
  wifiHelper.SetStandard (WIFI_STANDARD_80211n);

  /* Set up Legacy Channel */
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));

  /* Setup Physical Layer */
  YansWifiPhyHelper wifiPhy;
  wifiPhy.SetChannel (wifiChannel.Create ());
  wifiPhy.SetErrorRateModel ("ns3::YansErrorRateModel");
  wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode", StringValue ("HtMcs7"),
                                      "ControlMode", StringValue ("HtMcs0"));

  PointToPointHelper pointToPoint;
  NetDeviceContainer p2pd1,p2pd2,csmad1,csmad2;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("2Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("5ms"));
  p2pd1 = pointToPoint.Install (p2p1);
  p2pd2 = pointToPoint.Install (p2p2);
  
  //droptail
  // pointToPoint.SetQueue ("ns3::DropTailQueue", "MaxSize",StringValue ("50p"));
  // p2pd3 = pointToPoint.Install (p2p1);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", StringValue ("10ms"));
  csmad1 = csma.Install (csma_nodes1);
  csmad2 = csma.Install (csma_nodes2);

  /* Configure AP */
  Ssid ssid = Ssid ("network");
  wifiMac.SetType ("ns3::ApWifiMac",
                   "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevice;
  apDevice = wifiHelper.Install (wifiPhy, wifiMac, wireless_ap);

  /* Configure STA */
  wifiMac.SetType ("ns3::StaWifiMac",
                   "Ssid", SsidValue (ssid));

  NetDeviceContainer staDevices, devices;
  staDevices = wifiHelper.Install (wifiPhy, wifiMac, wireless_sta);

  /* Mobility model */
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

  positionAlloc->Add (Vector (30, 60, 0.0));
  positionAlloc->Add (Vector (50.0, 60.0, 0.0));
  positionAlloc->Add (Vector (30, 80, 0.0));
  positionAlloc->Add (Vector (50, 80, 0.0));
  positionAlloc->Add (Vector (70, 70, 0.0));
  positionAlloc->Add (Vector (110, 20, 0.0));

  positionAlloc->Add (Vector (110, 70, 0.0));
  positionAlloc->Add (Vector (110, 120, 0.0));
  positionAlloc->Add (Vector (160, 70, 0.0));
  positionAlloc->Add (Vector (190, 80, 0.0));
  positionAlloc->Add (Vector (200, 80, 0.0));
  positionAlloc->Add (Vector (200, 20, 0.0));
  positionAlloc->Add (Vector (220, 20, 0.0));


  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  mobility.Install (nodes);
  cout << 1 << '\n';
  //movility with velocity
  
  Ptr<ConstantVelocityMobilityModel> m4 = DynamicCast<ConstantVelocityMobilityModel>(nodes.Get(4)->GetObject<MobilityModel> ());
  Ptr<ConstantVelocityMobilityModel> m8 = DynamicCast<ConstantVelocityMobilityModel>(nodes.Get(8)->GetObject<MobilityModel> ());
  // Ptr<ConstantVelocityMobilityModel> m6 = DynamicCast<ConstantVelocityMobilityModel>(nodes.Get(5)->GetObject<MobilityModel> ());
  m4->SetVelocity(Vector(15,0,0));
  m8->SetVelocity(Vector(-15,0,0));
  
  Simulator::Schedule (Seconds (1.2), &Stop_node, m4);
  Simulator::Schedule (Seconds (1.2), &Stop_node, m8);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  Ipv4InterfaceContainer p2pi1,p2pi2,csmai1,csmai2;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  csmai1 = address.Assign (csmad1);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  p2pi1 = address.Assign (p2pd1);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterface;
  apInterface = address.Assign (apDevice);
  Ipv4InterfaceContainer staInterface;
  staInterface = address.Assign (staDevices);

  address.SetBase ("10.1.4.0", "255.255.255.0");
  p2pi2 = address.Assign (p2pd2);

  address.SetBase ("10.1.5.0", "255.255.255.0");
  csmai2 = address.Assign (csmad2);

  Ipv4GlobalRoutingHelper routingHelper;
  routingHelper.PopulateRoutingTables();

  // UdpEchoServerHelper echoServer(9);
  
  // ApplicationContainer serverApps = echoServer.Install(wireless_sta.Get(2));
  // serverApps.Start(Seconds(1.0));
  // serverApps.Stop(Seconds(10.0));

  // UdpEchoClientHelper echoClient(staInterface.GetAddress(2), 9);
  // echoClient.SetAttribute("MaxPackets", UintegerValue(1));
  // echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
  // echoClient.SetAttribute("PacketSize", UintegerValue(1024));

  // ApplicationContainer clientApps = echoClient.Install(csma_nodes1.Get(0));
  // clientApps.Start(Seconds(0.0));
  // clientApps.Stop(Seconds(10.0));  

  // uint16_t port = 9;   // Discard port (RFC 863)
  // OnOffHelper TcpOnoff ("ns3::TcpSocketFactory", Address (InetSocketAddress (csmai1.GetAddress(0), port)));
  // TcpOnoff.SetConstantRate (DataRate ("448kb/s"));
  // ApplicationContainer TcpApps = TcpOnoff.Install (nodes.Get (12));//N9
  // TcpApps.Start (Seconds (1.0));
  // TcpApps.Stop (Seconds (5.0));

  // // Create a packet sink to receive these packets
  // PacketSinkHelper TcpSink ("ns3::TcpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  // TcpApps = TcpSink.Install (nodes.Get(0));//N1
  // TcpApps.Start (Seconds (0.5));
  // TcpApps.Stop (Seconds (6.0));
  
  

  /* Install TCP Receiver on the access point */
  PacketSinkHelper WsinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 9));
  ApplicationContainer WsinkApp = WsinkHelper.Install (nodes.Get(9));//N9
  

  // /* Install TCP/UDP Transmitter on the station */
  OnOffHelper Wserver ("ns3::TcpSocketFactory", (InetSocketAddress (staInterface.GetAddress(0), 9)));
  Wserver.SetAttribute ("PacketSize", UintegerValue (1024));
  Wserver.SetAttribute ("MaxBytes", UintegerValue(90*1024));
  Wserver.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  Wserver.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  Wserver.SetAttribute ("DataRate", StringValue("10240bps"));
  ApplicationContainer WserverApp = Wserver.Install (nodes.Get(3));//N11

  WsinkApp.Start (Seconds (5.0));
  WsinkApp.Stop (Seconds(16.0));
  WserverApp.Start (Seconds (6.0));
  WserverApp.Stop (Seconds(15.0));
  


  // Anim.SetConstantPosition(nodes.Get(0),30.0,60.0);
  // Anim.SetConstantPosition(nodes.Get(1),50.0,60.0);
  // Anim.SetConstantPosition(nodes.Get(2),30.0,80.0);
  // Anim.SetConstantPosition(nodes.Get(3),50.0,80.0);
  // Anim.SetConstantPosition(nodes.Get(4),70.0,70.0);
  // Anim.SetConstantPosition(nodes.Get(5),110.0,20.0);
  // Anim.SetConstantPosition(nodes.Get(6),110.0,70.0);
  // Anim.SetConstantPosition(nodes.Get(7),110.0,120.0);
  // Anim.SetConstantPosition(nodes.Get(8),160.0,70.0);
  // Anim.SetConstantPosition(nodes.Get(9),190.0,80.0);
  // Anim.SetConstantPosition(nodes.Get(10),200.0,80.0);
  // Anim.SetConstantPosition(nodes.Get(11),200.0,20.0);
  // Anim.SetConstantPosition(nodes.Get(12),220.0,20.0);
  AnimationInterface Anim("pract1.xml");
  AsciiTraceHelper ascii;
  Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ("mixed-global-routing.tr");
  pointToPoint.EnableAsciiAll (stream);
  csma.EnableAsciiAll (stream);
  wifiPhy.EnableAsciiAll (stream);
  
  Simulator::Stop (Seconds(17.0));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}