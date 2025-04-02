//
//          +------+                                                                        +------+
//          |  T0  |                                                                        |  T1  |
//          +------+                                                                        +------+
//                  \                                                                      /
//                   \ 10Mbps, 1ms                                                        / 10Mbps, 1ms
//                    \                                                                  /
//                     \                                                                /
//                      +------+                                                +------+
//                      |  R0  |------------------------------------------------|  R1  |
//                      +------+                  5Mbps, 40ms                   +------+
//                     /                                                                \ 
//                    / 100Mbps, 5ms                                                     \ 100Mbps, 5ms
//                   /                                                                    \ 
//           +------+                                                                      +------+
//           |  N0  |                                                                      |  N1  |
//           +------+                                                                      +------+
//                   \                                                                    /
//                    \ 100Mbps, 5ms                                                     / 100Mbps, 5ms
//                     \                                                                /
//                      +------+                 10Mbps, 40ms                   +------+
//                      |  R2  |------------------------------------------------|  R3  |
//                      +------+                                                +------+
//                     /                                                                \ 
//                    /                                                                  \ 
//                   / 10Mbps, 1ms                                                        \ 10Mbps, 1ms
//                  /                                                                      \ 
//          +------+                                                                        +------+
//          |  T2  |                                                                        |  T3  |
//          +------+                                                                        +------+
//

#include "ns3/applications-module.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/core-module.h"
#include "ns3/dce-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

void printTcpFlags(std::string key, std::string value)
{
    std::cout << key << " = " << value << std::endl;
}

void setPos(Ptr<Node> n, int x, int y, int z)
{
    Ptr<ConstantPositionMobilityModel> loc =
        CreateObject<ConstantPositionMobilityModel>();
    n->AggregateObject(loc);
    Vector locVec2(x, y, z);
    loc->SetPosition(locVec2);
}

int main(int argc, char *argv[])
{
    // choosing congestion control algorithm
    std::string congestionControl = "lia";
    std::string scheduler = "default";
    bool sack = true;
    bool wl = false;
    CommandLine cmd;
    cmd.AddValue("cc", "congestion control algo. LIA default",
                 congestionControl);

    cmd.AddValue("sch", "schedular for mptcp. minRTT default",
                 scheduler);

    cmd.AddValue("sack", "tcp sack on",
                 sack);

    cmd.AddValue("wl", "tcp sack on",
                 wl);

    cmd.Parse(argc, argv);

    std::cout << std::endl
              << "Using '" << congestionControl
              << "' congestion control algorithm." << std::endl;
    std::cout << std::endl
              << "Using '" << scheduler
              << "' scheduler algorithm." << std::endl;

    DceManagerHelper dceManager;
    dceManager.SetTaskManagerAttribute("FiberManagerType",
                                       StringValue("UcontextFiberManager"));

    dceManager.SetNetworkStack("ns3::LinuxSocketFdFactory", "Library",
                               StringValue("liblinux.so"));

    NodeContainer nodes, routers, traffic;
    nodes.Create(2);
    routers.Create(4);
    traffic.Create(4);

    LinuxStackHelper stack;
    stack.Install(nodes);
    stack.Install(routers);
    stack.Install(traffic);

    dceManager.Install(nodes);
    dceManager.Install(routers);
    dceManager.Install(traffic);

    PointToPointHelper pointToPoint;
    Ipv4AddressHelper address1, address2;
    Ipv4AddressHelper r0addr, r1addr, r2addr, r3addr, r0r1addr, r2r3addr;
    std::ostringstream cmd_oss;

    r0addr.SetBase("10.0.0.0", "255.255.255.0");
    r1addr.SetBase("10.1.0.0", "255.255.255.0");
    r2addr.SetBase("10.2.0.0", "255.255.255.0");
    r3addr.SetBase("10.3.0.0", "255.255.255.0");
    r0r1addr.SetBase("11.0.0.0", "255.255.255.0");
    r2r3addr.SetBase("11.1.0.0", "255.255.255.0");

    Ptr<RateErrorModel> wifiErrorModel =
        CreateObjectWithAttributes<RateErrorModel>(
            "RanVar", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
            "ErrorRate", DoubleValue(0.1), "ErrorUnit",
            EnumValue(RateErrorModel::ERROR_UNIT_PACKET));

    // SETTING UP ROUTERS

    pointToPoint.SetQueue("ns3::DropTailQueue<Packet>", "MaxSize", StringValue("50p"));
    // connecting R0 <-> R1
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("40ms"));

    NetDeviceContainer r0r1 = pointToPoint.Install(routers.Get(0), routers.Get(1));
    Ipv4InterfaceContainer r0r1Ip = r0r1addr.Assign(r0r1);
    LinuxStackHelper::RunIp(routers.Get(0), Seconds(0.2), "route add 10.1.0.0/24 via 11.0.0.1 dev sim0"); // outbound - R1 network
    LinuxStackHelper::RunIp(routers.Get(0), Seconds(0.2), "route add 10.0.0.1/32 via 10.0.0.2 dev sim1"); // inbound - N0
    LinuxStackHelper::RunIp(routers.Get(0), Seconds(0.2), "route add 10.0.0.3/32 via 10.0.0.4 dev sim2"); // inbound - T0

    LinuxStackHelper::RunIp(routers.Get(1), Seconds(0.2), "route add 10.0.0.0/24 via 11.0.0.2 dev sim0"); // outbound - R0 network
    LinuxStackHelper::RunIp(routers.Get(1), Seconds(0.2), "route add 10.1.0.1/32 via 10.1.0.2 dev sim1"); // inbound - N1
    LinuxStackHelper::RunIp(routers.Get(1), Seconds(0.2), "route add 10.1.0.3/32 via 10.1.0.4 dev sim2"); // inbound - T1

    // connectin R2 <-> R3
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("40ms"));

    NetDeviceContainer r2r3 = pointToPoint.Install(routers.Get(2), routers.Get(3));
    Ipv4InterfaceContainer r2r3Ip = r2r3addr.Assign(r2r3);
    LinuxStackHelper::RunIp(routers.Get(2), Seconds(0.2), "route add 10.3.0.0/24 via 11.1.0.1 dev sim0"); // outbound to R3 network
    LinuxStackHelper::RunIp(routers.Get(2), Seconds(0.2), "route add 10.2.0.1/32 via 10.2.0.2 dev sim1"); // inbound - N0
    LinuxStackHelper::RunIp(routers.Get(2), Seconds(0.2), "route add 10.2.0.3/32 via 10.2.0.4 dev sim2"); // inbound - T2

    LinuxStackHelper::RunIp(routers.Get(3), Seconds(0.2), "route add 10.2.0.0/24 via 11.1.0.2 dev sim0"); // outbound to R2 network
    LinuxStackHelper::RunIp(routers.Get(3), Seconds(0.2), "route add 10.3.0.1/32 via 10.3.0.2 dev sim1"); // inbound - N1
    LinuxStackHelper::RunIp(routers.Get(3), Seconds(0.2), "route add 10.3.0.3/32 via 10.3.0.4 dev sim2"); // inbound - T3

    // SETTING UP MPTCP NODES

    pointToPoint.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("5ms"));

    // connecting N0 <-> R0
    NetDeviceContainer n0r0 = pointToPoint.Install(nodes.Get(0), routers.Get(0));
    Ipv4InterfaceContainer n0r0Ip = r0addr.Assign(n0r0);
    LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "route add 10.1.0.1 via 10.0.0.2 dev sim0");

    // connecting N0 <-> R2
    NetDeviceContainer n0r2 = pointToPoint.Install(nodes.Get(0), routers.Get(2));
    Ipv4InterfaceContainer n0r2Ip = r2addr.Assign(n0r2);
    LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "route add 10.3.0.1 via 10.2.0.2 dev sim1");

    // connecting N1 <-> R1
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("20ms"));
    NetDeviceContainer n1r1 = pointToPoint.Install(nodes.Get(1), routers.Get(1));
    Ipv4InterfaceContainer n1r1Ip = r1addr.Assign(n1r1);
    LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "route add 10.0.0.1 via 10.1.0.2 dev sim0");
    if (wl)
    {
        n1r1.Get(0)->SetAttribute("ReceiveErrorModel",
                                  PointerValue(wifiErrorModel));
        n1r1.Get(1)->SetAttribute("ReceiveErrorModel",
                                  PointerValue(wifiErrorModel));
    }

    // connecting N1 <-> R3
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("5ms"));
    NetDeviceContainer n1r3 = pointToPoint.Install(nodes.Get(1), routers.Get(3));
    Ipv4InterfaceContainer n1r3Ip = r3addr.Assign(n1r3);
    LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "route add 10.2.0.1 via 10.3.0.2 dev sim1");

    // SETTING UP TRAFFIC NODES

    pointToPoint.SetDeviceAttribute("DataRate", StringValue("20Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("1ms"));

    // connecting T0 <-> R0
    NetDeviceContainer t0r0 = pointToPoint.Install(traffic.Get(0), routers.Get(0));
    Ipv4InterfaceContainer t0r0Ip = r0addr.Assign(t0r0);
    LinuxStackHelper::RunIp(traffic.Get(0), Seconds(0.1), "route add default via 10.0.0.4 dev sim0");

    // connecting T1 <-> R1
    NetDeviceContainer t1r1 = pointToPoint.Install(traffic.Get(1), routers.Get(1));
    Ipv4InterfaceContainer t1r1Ip = r1addr.Assign(t1r1);
    LinuxStackHelper::RunIp(traffic.Get(1), Seconds(0.1), "route add default via 10.1.0.4 dev sim0");

    // connecting T2 <-> R2
    NetDeviceContainer t2r2 = pointToPoint.Install(traffic.Get(2), routers.Get(2));
    Ipv4InterfaceContainer t2r2Ip = r2addr.Assign(t2r2);
    LinuxStackHelper::RunIp(traffic.Get(2), Seconds(0.1), "route add default via 10.2.0.4 dev sim0");

    // connecting T3 <-> R3
    NetDeviceContainer t3r3 = pointToPoint.Install(traffic.Get(3), routers.Get(3));
    Ipv4InterfaceContainer t3r3Ip = r3addr.Assign(t3r3);
    LinuxStackHelper::RunIp(traffic.Get(3), Seconds(0.1), "route add default via 10.3.0.4 dev sim0");

    // debug
    stack.SysctlSet(nodes, ".net.mptcp.mptcp_debug", "1");
    stack.SysctlSet(nodes, ".net.ipv4.tcp_congestion_control", congestionControl);
    stack.SysctlSet(nodes, ".net.mptcp.mptcp_scheduler", scheduler);
    stack.SysctlSet(nodes, ".net.ipv4.tcp_sack", sack ? "1" : "0");

    LinuxStackHelper::SysctlGet(nodes.Get(0), Seconds(1),
                                ".net.ipv4.tcp_available_congestion_control",
                                &printTcpFlags);

    LinuxStackHelper::SysctlGet(nodes.Get(0), Seconds(1),
                                ".net.ipv4.tcp_congestion_control",
                                &printTcpFlags);

    LinuxStackHelper::SysctlGet(nodes.Get(0), Seconds(1),
                                ".net.mptcp.mptcp_scheduler",
                                &printTcpFlags);

    LinuxStackHelper::SysctlGet(nodes.Get(0), Seconds(1),
                                ".net.ipv4.tcp_sack",
                                &printTcpFlags);

    DceApplicationHelper dce;
    ApplicationContainer apps;

    dce.SetStackSize(1 << 24);

    //------------
    // MPTCP APPS
    //------------

    // source on N0
    BulkSendHelper bulkSend = BulkSendHelper("ns3::LinuxTcpSocketFactory",
                                             InetSocketAddress("10.1.0.1", 9)); // 5mbps link
    int dataToSendMb = 100000;
    bulkSend.SetAttribute("MaxBytes", UintegerValue(dataToSendMb * 1000000));
    apps = bulkSend.Install(nodes.Get(0));
    apps.Start(Seconds(20));

    // sink on N1
    PacketSinkHelper sink =
        PacketSinkHelper("ns3::LinuxTcpSocketFactory",
                         InetSocketAddress(Ipv4Address::GetAny(), 9));
    apps = sink.Install(nodes.Get(1));
    apps.Start(Seconds(5));

    //----------
    // UDP APPS
    //----------
    int mptcp_start = 20;
    //  sink on T1 and T0
    //  -----------------
    PacketSinkHelper UdpTrafficSink =
        PacketSinkHelper("ns3::LinuxUdpSocketFactory",
                         InetSocketAddress(Ipv4Address::GetAny(), 6000));
    apps = UdpTrafficSink.Install(traffic.Get(0));
    apps.Start(Seconds(2));
    apps = UdpTrafficSink.Install(traffic.Get(1));
    apps.Start(Seconds(2));

    // forward traffic from 50 - 100
    OnOffHelper forward = OnOffHelper(
        "ns3::LinuxUdpSocketFactory", InetSocketAddress("10.1.0.3", 6000));
    forward.SetAttribute("DataRate", StringValue("11Mbps"));
    forward.SetAttribute("PacketSize", UintegerValue(1500));
    forward.SetAttribute(
        "OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    forward.SetAttribute(
        "OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    apps = forward.Install(traffic.Get(0));
    apps.Start(Seconds(mptcp_start + 50));
    apps.Stop(Seconds(mptcp_start + 100));

    // reverse traffic from 100 - 200
    OnOffHelper reverse = OnOffHelper(
        "ns3::LinuxUdpSocketFactory", InetSocketAddress("10.0.0.3", 6000));
    reverse.SetAttribute("DataRate", StringValue("11Mbps"));
    reverse.SetAttribute("PacketSize", UintegerValue(1500));
    reverse.SetAttribute(
        "OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    reverse.SetAttribute(
        "OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    apps = reverse.Install(traffic.Get(1));
    apps.Start(Seconds(mptcp_start + 100));
    apps.Stop(Seconds(mptcp_start + 200));

    // forward traffic from 150 - 200
    // OnOffHelper forward = OnOffHelper(
    //     "ns3::LinuxUdpSocketFactory", InetSocketAddress("10.1.0.3", 6000));
    forward.SetAttribute("DataRate", StringValue("11Mbps"));
    forward.SetAttribute("PacketSize", UintegerValue(1500));
    forward.SetAttribute(
        "OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    forward.SetAttribute(
        "OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    apps = forward.Install(traffic.Get(0));
    apps.Start(Seconds(mptcp_start + 150)); //
    apps.Stop(Seconds(mptcp_start + 200));  //

    int yOffSet = 50;
    int xOffSet = 0;
    setPos(nodes.Get(0), 0 + xOffSet, 0 + yOffSet, 0);
    setPos(nodes.Get(1), 100 + xOffSet, 0 + yOffSet, 0);
    setPos(routers.Get(0), 25 + xOffSet, -25 + yOffSet, 0);
    setPos(routers.Get(1), 75 + xOffSet, -25 + yOffSet, 0);
    setPos(routers.Get(2), 25 + xOffSet, 25 + yOffSet, 0);
    setPos(routers.Get(3), 75 + xOffSet, 25 + yOffSet, 0);
    setPos(traffic.Get(0), 25 - 5 + xOffSet, -10 - 25 + yOffSet, 0);
    setPos(traffic.Get(1), 75 + 5 + xOffSet, -10 - 25 + yOffSet, 0);
    setPos(traffic.Get(2), 25 - 5 + xOffSet, -10 + 25 + yOffSet, 0);
    setPos(traffic.Get(3), 75 + 5 + xOffSet, -10 + 25 + yOffSet, 0);

    AnimationInterface anim("thesis-anim.xml");
    anim.SetMaxPktsPerTraceFile(1 * 10000000);
    anim.EnablePacketMetadata(true);

    anim.UpdateNodeColor(nodes.Get(0)->GetId(), 0, 255, 0);
    anim.UpdateNodeSize(nodes.Get(0)->GetId(), 5.0, 5.0);
    anim.UpdateNodeColor(nodes.Get(1)->GetId(), 0, 255, 0);
    anim.UpdateNodeSize(nodes.Get(1)->GetId(), 5.0, 5.0);

    anim.UpdateNodeColor(routers.Get(0)->GetId(), 255, 0, 0);
    anim.UpdateNodeSize(routers.Get(0)->GetId(), 4.0, 4.0);
    anim.UpdateNodeColor(routers.Get(1)->GetId(), 255, 0, 0);
    anim.UpdateNodeSize(routers.Get(1)->GetId(), 4.0, 4.0);
    anim.UpdateNodeColor(routers.Get(2)->GetId(), 255, 0, 0);
    anim.UpdateNodeSize(routers.Get(2)->GetId(), 4.0, 4.0);
    anim.UpdateNodeColor(routers.Get(3)->GetId(), 255, 0, 0);
    anim.UpdateNodeSize(routers.Get(3)->GetId(), 4.0, 4.0);

    anim.UpdateNodeColor(traffic.Get(0)->GetId(), 0, 0, 255);
    anim.UpdateNodeSize(traffic.Get(0)->GetId(), 3.0, 3.0);
    anim.UpdateNodeColor(traffic.Get(1)->GetId(), 0, 0, 255);
    anim.UpdateNodeSize(traffic.Get(1)->GetId(), 3.0, 3.0);
    anim.UpdateNodeColor(traffic.Get(2)->GetId(), 0, 0, 255);
    anim.UpdateNodeSize(traffic.Get(2)->GetId(), 3.0, 3.0);
    anim.UpdateNodeColor(traffic.Get(3)->GetId(), 0, 0, 255);
    anim.UpdateNodeSize(traffic.Get(3)->GetId(), 3.0, 3.0);

    pointToPoint.EnablePcapAll("thesis-ww", false);

    Simulator::Stop(Seconds(mptcp_start + 405));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
