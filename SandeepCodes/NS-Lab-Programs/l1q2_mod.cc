
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"

// Default Network Topology
//
//       10.1.1.0
// n0 ------->------ n2 -------------- n1
//    point-to-point |
//                   ^
//                   |
//                   n3

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(4);

    PointToPointHelper pointToPoint1, pointToPoint2, pointToPoint3;
    pointToPoint1.SetDeviceAttribute("DataRate", StringValue("10Mbps")); // n0 to n2
    pointToPoint1.SetChannelAttribute("Delay", StringValue("2ms"));

    pointToPoint2.SetDeviceAttribute("DataRate", StringValue("8Mbps")); // n2 to n1
    pointToPoint2.SetChannelAttribute("Delay", StringValue("3ms"));

    pointToPoint3.SetDeviceAttribute("DataRate", StringValue("10Mbps")); // n2 to n3
    pointToPoint3.SetChannelAttribute("Delay", StringValue("4ms"));


    NetDeviceContainer device1, device2, device3;
    device1 = pointToPoint1.Install(nodes.Get(0), nodes.Get(2));
    device2 = pointToPoint2.Install(nodes.Get(1), nodes.Get(2));
    device3 = pointToPoint3.Install(nodes.Get(3), nodes.Get(2));


    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address1, address2, address3;
    address1.SetBase("10.1.1.0", "255.255.255.0");
    address2.SetBase("10.1.2.0", "255.255.255.0");
    address3.SetBase("10.1.3.0", "255.255.255.0");


    Ipv4InterfaceContainer interface1 = address1.Assign(device1);
    Ipv4InterfaceContainer interface2 = address2.Assign(device2);
    Ipv4InterfaceContainer interface3 = address3.Assign(device3);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    
    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(3));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(interface3.GetAddress(0), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApp1 = echoClient.Install(nodes.Get(0));
    clientApp1.Start(Seconds(2.0));
    clientApp1.Stop(Seconds(10.0));

    

    AnimationInterface anim("l1q2.xml");

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
