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
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1 ---  n2   n5  
//    point-to-point   |            |
//                      n3  n4   -- n6  

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SecondScriptExample");

int
main(int argc, char* argv[])
{
    bool verbose = true;
    uint32_t nCsma = 3;

    CommandLine cmd(__FILE__);
    cmd.AddValue("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);

    cmd.Parse(argc, argv);

    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    nCsma = nCsma == 0 ? 1 : nCsma;

    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    NodeContainer csmaNodes1;
    csmaNodes1.Add(p2pNodes.Get(1));
    csmaNodes1.Create(2);   

    NodeContainer csmaNodes2;
    csmaNodes2.Add(p2pNodes.Get(1));
    csmaNodes2.Create(2);

    NodeContainer p2pNodes1;
    p2pNodes1.Add(csmaNodes1.Get(2));
    p2pNodes1.Create(1);

    NodeContainer p2pNodes2;
    p2pNodes2.Add(csmaNodes2.Get(2));
    p2pNodes2.Add(p2pNodes1.Get(1));


    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices1;
    p2pDevices1 = pointToPoint.Install(p2pNodes.Get(0),p2pNodes.Get(1));

    NetDeviceContainer p2pDevices2;
    p2pDevices2 = pointToPoint.Install(p2pNodes1.Get(0),p2pNodes1.Get(1));

    NetDeviceContainer p2pDevices3;
    p2pDevices3 = pointToPoint.Install(p2pNodes2.Get(0),p2pNodes2.Get(1));

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    NetDeviceContainer csmaDevices1,csmaDevices2;
    csmaDevices1 = csma.Install(csmaNodes1);
    csmaDevices2 = csma.Install(csmaNodes2);

    InternetStackHelper stack;
    stack.Install(p2pNodes.Get(0));
    // stack.Install(p2pNodes.Get(1));
    stack.Install(csmaNodes1);
    stack.Install(csmaNodes2.Get(1));
    stack.Install(csmaNodes2.Get(2));
    stack.Install(p2pNodes1.Get(1));
    

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces1;
    p2pInterfaces1 = address.Assign(p2pDevices1);


    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces1;
    csmaInterfaces1 = address.Assign(csmaDevices1);

    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces2;
    csmaInterfaces2 = address.Assign(csmaDevices2); 

    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces2;
    p2pInterfaces2 = address.Assign(p2pDevices2); 

    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces3;
    p2pInterfaces3 = address.Assign(p2pDevices3);


    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(p2pNodes2.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(p2pInterfaces3.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(p2pNodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    uint16_t port = 9; // well-known echo port number
    uint32_t maxBytes = 512;
    BulkSendHelper source ("ns3::TcpSocketFactory",
    InetSocketAddress (csmaInterfaces1.GetAddress (1), port));
    source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
    ApplicationContainer sourceApps = source.Install (csmaNodes1.Get (0));
    sourceApps.Start (Seconds (0.0));
    sourceApps.Stop (Seconds (10.0));

    PacketSinkHelper sink ("ns3::TcpSocketFactory",
    InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sinkApps = sink.Install (csmaNodes1.Get (2));
    sinkApps.Start (Seconds (0.0));
    sinkApps.Stop (Seconds (10.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    pointToPoint.EnablePcapAll("second");
    csma.EnablePcap("second", csmaDevices1.Get(1), true);

    AnimationInterface anim("l2q1.xml");
    anim.SetConstantPosition(p2pNodes.Get(0),10,10);
    anim.SetConstantPosition(p2pNodes.Get(1),15,10);
    anim.SetConstantPosition(csmaNodes1.Get(1),20,5);
    anim.SetConstantPosition(csmaNodes1.Get(2),25,5);
    anim.SetConstantPosition(csmaNodes2.Get(1),20,15);
    anim.SetConstantPosition(csmaNodes2.Get(2),25,15);
    anim.SetConstantPosition(p2pNodes1.Get(1),30,10);

    AsciiTraceHelper ascii;
    pointToPoint.EnableAsciiAll(ascii.CreateFileStream("Second.tr"));
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO ("Done.");
    Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));
    std::cout << "Total Bytes Received: " << sink1->GetTotalRx () << std::endl;
    
    return 0;
}
