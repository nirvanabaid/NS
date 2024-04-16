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

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ThirdScriptExample");

int
main(int argc, char* argv[])
{
    bool verbose = true;
    uint32_t nCsma = 3;
    uint32_t nWifi = 5;
    bool tracing = false;
    uint32_t maxBytes = 0;

    CommandLine cmd(_FILE_);
    cmd.AddValue("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
    cmd.AddValue("nWifi", "Number of wifi STA devices", nWifi);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue("tracing", "Enable pcap tracing", tracing);

    cmd.Parse(argc, argv);

    // The underlying restriction of 18 is due to the grid position
    // allocator's configuration; the grid layout will exceed the
    // bounding box if more than 18 nodes are provided.
    if (nWifi > 18)
    {
        std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box"
                  << std::endl;
        return 1;
    }

    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    // NodeContainer p2pNodes;
    // p2pNodes.Create(2);

    // PointToPointHelper pointToPoint;
    // pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    // pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    // NetDeviceContainer p2pDevices;
    // p2pDevices = pointToPoint.Install(p2pNodes);

    NodeContainer csmaNodes;
    csmaNodes.Create(nCsma);

    NodeContainer p2pNodes;
    p2pNodes.Add(csmaNodes.Get(2));
    p2pNodes.Create(1);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("50ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install(csmaNodes);

    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(nWifi);
    NodeContainer wifiApNode = p2pNodes.Get(1);
    

    NodeContainer p2pNodes1;
    p2pNodes1.Create(1);
    

    NodeContainer csmaNodes1;
    csmaNodes1.Create(nCsma);


    NetDeviceContainer csmaDevices1;
    csmaDevices1 = csma.Install(csmaNodes1);

    p2pNodes1.Add(csmaNodes1.Get(0));

    NetDeviceContainer p2pDevices1;
    p2pDevices1 = pointToPoint.Install(p2pNodes1);

    wifiApNode.Add(p2pNodes1.Get(0));

   
    
    
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;
    Ssid ssid = Ssid("ns-3-ssid");

    WifiHelper wifi;
    NetDeviceContainer staDevices;
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
    staDevices = wifi.Install(phy, mac, wifiStaNodes);

    NetDeviceContainer apDevices;
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    apDevices = wifi.Install(phy, mac, wifiApNode);
    

    MobilityHelper mobility;

    /*mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX",
                                  DoubleValue(0.0),
                                  "MinY",
                                  DoubleValue(0.0),
                                  "DeltaX",
                                  DoubleValue(5.0),
                                  "DeltaY",
                                  DoubleValue(10.0),
                                  "GridWidth",
                                  UintegerValue(3),
                                  "LayoutType",
                                  StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds",
                              RectangleValue(Rectangle(-50, 50, -50, 50)));
    mobility.Install(wifiStaNodes);

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");*/
    //mobility.Install(wifiApNode);
    
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (csmaNodes);
  mobility.Install (csmaNodes1);
   mobility.Install (p2pNodes);
    mobility.Install (p2pNodes1);
    mobility.Install (wifiStaNodes);
     
  Ptr<ConstantPositionMobilityModel> s1 = csmaNodes.Get (0)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s2 = csmaNodes.Get (1)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s3 = csmaNodes.Get (2)->GetObject<ConstantPositionMobilityModel> ();

 Ptr<ConstantPositionMobilityModel> s4 = csmaNodes1.Get (0)->GetObject<ConstantPositionMobilityModel> ();
  Ptr<ConstantPositionMobilityModel> s5 = csmaNodes1.Get (1)->GetObject<ConstantPositionMobilityModel> ();
   Ptr<ConstantPositionMobilityModel> s6 = csmaNodes1.Get (2)->GetObject<ConstantPositionMobilityModel> ();
   Ptr<ConstantPositionMobilityModel> s7 = p2pNodes.Get (1)->GetObject<ConstantPositionMobilityModel> ();
   Ptr<ConstantPositionMobilityModel> s8 = p2pNodes1.Get (0)->GetObject<ConstantPositionMobilityModel> ();
   
   Ptr<ConstantPositionMobilityModel> s9 = wifiStaNodes.Get (0)->GetObject<ConstantPositionMobilityModel> ();
   Ptr<ConstantPositionMobilityModel> s10 = wifiStaNodes.Get (1)->GetObject<ConstantPositionMobilityModel> ();
   Ptr<ConstantPositionMobilityModel> s11 = wifiStaNodes.Get (2)->GetObject<ConstantPositionMobilityModel> ();
   Ptr<ConstantPositionMobilityModel> s12 = wifiStaNodes.Get (3)->GetObject<ConstantPositionMobilityModel> ();
   Ptr<ConstantPositionMobilityModel> s13 = wifiStaNodes.Get (4)->GetObject<ConstantPositionMobilityModel> ();
   
   
   
 
  s1->SetPosition (Vector ( 0, 45, 0  ));
  s2->SetPosition (Vector ( 10, 45, 0  ));
  s3->SetPosition (Vector ( 20,45, 0 ));
  s4->SetPosition (Vector ( 70, 45, 0 ));
  s5->SetPosition (Vector ( 80, 45, 0 ));
  s6->SetPosition (Vector ( 90,45, 0 ));
  s7->SetPosition (Vector ( 20,22, 0 ));
  s8->SetPosition (Vector ( 70,22, 0 ));
  s9->SetPosition (Vector ( 30,30, 0 ));
  s10->SetPosition (Vector ( 40,20, 0 ));
  s11->SetPosition (Vector ( 40,30, 0 ));
  s12->SetPosition (Vector ( 40,40, 0 ));
  s13->SetPosition (Vector ( 50,30, 0 ));
    

    InternetStackHelper stack;
    stack.Install(csmaNodes);
    stack.Install(csmaNodes1);
    stack.Install(wifiApNode);
    stack.Install(wifiStaNodes);
    

    Ipv4AddressHelper address;

    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign(csmaDevices);

    address.SetBase("10.1.3.0", "255.255.255.0");
    address.Assign(staDevices);
    address.Assign(apDevices);
    // address.Assign(staDevices1);
    

     address.SetBase("10.1.4.0", "255.255.255.0");
     Ipv4InterfaceContainer p2pInterfaces1;
     p2pInterfaces1 = address.Assign(p2pDevices1);

    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces1;
    csmaInterfaces1 = address.Assign(csmaDevices1);

    // UdpEchoServerHelper echoServer(9);

    // ApplicationContainer serverApps = echoServer.Install(csmaNodes.Get(0));
    // serverApps.Start(Seconds(1.0));
    // serverApps.Stop(Seconds(10.0));

    // UdpEchoClientHelper echoClient(csmaInterfaces.GetAddress(0), 9);
    // echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    // echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    // echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    // ApplicationContainer clientApps = echoClient.Install(wifiApNode.Get(1));
    // clientApps.Start(Seconds(2.0));
    // clientApps.Stop(Seconds(10.0));

    
    
 
  /*
  //uint16_t multicastPort = 9;   // Discard port (RFC 863)
  // Configure a multicast packet generator that generates a packet
  // every few seconds
  OnOffHelper onoff ("ns3::UdpSocketFactory",Address());
  onoff.SetConstantRate (DataRate ("255b/s"));
  onoff.SetAttribute ("PacketSize", UintegerValue (128));

  ApplicationContainer srcC = onoff.Install (p2pNodes1.Get (0));
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
  
  */
  
  uint16_t port = 9; // well-known echo port number

    BulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(p2pInterfaces.GetAddress(1), port));
    // Set the amount of data to send in bytes.  Zero is unlimited.
    source.SetAttribute("MaxBytes", UintegerValue(maxBytes));
    ApplicationContainer sourceApps = source.Install(p2pNodes.Get(0));
    sourceApps.Start(Seconds(3.0));
    sourceApps.Stop(Seconds(11.0));

    //
    // Create a PacketSinkApplication and install it on node 1
    //
    PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
    ApplicationContainer sinkApps = sink.Install(p2pNodes.Get(1));
    sinkApps.Start(Seconds(2.0));
    sinkApps.Stop(Seconds(12.0));


    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Stop(Seconds(10.0));

    if (tracing)
    {
        phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        pointToPoint.EnablePcapAll("third");
        phy.EnablePcap("third", apDevices.Get(0));
        csma.EnablePcap("third", csmaDevices.Get(0), true);
    }
    AnimationInterface anim("exam7.xml");
    Simulator::Run();
    Simulator::Destroy();

    Ptr<PacketSink> sink1 = DynamicCast<PacketSink>(sinkApps.Get(0));
    std::cout << "Total Bytes Received: " << sink1->GetTotalRx() << std::endl;
    return 0;
}