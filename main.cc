#include "ns3/applications-module.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/core-module.h"
#include "ns3/dce-application-helper.h"
#include "ns3/dce-module.h"
#include "ns3/error-model.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/flow-monitor.h"
#include "ns3/internet-module.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/trace-helper.h"
#include "ns3/wifi-module.h"
#include "ns3/wimax-module.h"
// #include

using namespace ns3;

void printTcpFlags(std::string key, std::string value) {
  std::cout << key << " = " << value << std::endl;
}

void setPos(Ptr<Node> n, int x, int y, int z) {
  Ptr<ConstantPositionMobilityModel> loc =
      CreateObject<ConstantPositionMobilityModel>();
  n->AggregateObject(loc);
  Vector locVec2(x, y, z);
  loc->SetPosition(locVec2);
}

<<<<<<< HEAD
int main(int argc, char *argv[])
{

    std::string congestionControl = "lia";
    bool t = false;
    CommandLine cmd;
    cmd.AddValue("cc", "Congestion Control algorithm to use. Default is LIA",
                 congestionControl);
    cmd.AddValue("t", "traffic", t);
    cmd.Parse(argc, argv);
    std::cout << "Using '" << congestionControl
              << "' congestion control algorithm." << std::endl;

    DceManagerHelper dceManager;
    dceManager.SetTaskManagerAttribute("FiberManagerType",
                                       StringValue("UcontextFiberManager"));

    dceManager.SetNetworkStack("ns3::LinuxSocketFdFactory", "Library",
                               StringValue("liblinux.so"));

    Ptr<RateErrorModel> lteErrorModel =
        CreateObjectWithAttributes<RateErrorModel>(
            "RanVar", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
            "ErrorRate", DoubleValue(0.02), "ErrorUnit",
            EnumValue(RateErrorModel::ERROR_UNIT_PACKET));
    Ptr<RateErrorModel> wifiErrorModel =
        CreateObjectWithAttributes<RateErrorModel>(
            "RanVar", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
            "ErrorRate", DoubleValue(0.1), "ErrorUnit",
            EnumValue(RateErrorModel::ERROR_UNIT_PACKET));
    Ptr<RateErrorModel> wimaxErrorModel =
        CreateObjectWithAttributes<RateErrorModel>(
            "RanVar", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
            "ErrorRate", DoubleValue(0.2), "ErrorUnit",
            EnumValue(RateErrorModel::ERROR_UNIT_PACKET));

    NodeContainer nodes, routers, lte, wifi, wimax, traffic;
    nodes.Create(2);
    routers.Create(6);
    lte.Create(1);
    wifi.Create(1);
    wimax.Create(1);
    traffic.Create(6);

    LinuxStackHelper stack;
    stack.Install(nodes);
    stack.Install(routers);
    stack.Install(wifi);
    stack.Install(lte);
    stack.Install(traffic);
    stack.Install(wimax);

    dceManager.Install(nodes);
    dceManager.Install(routers);
    dceManager.Install(wifi);
    dceManager.Install(wimax);
    dceManager.Install(traffic);
    dceManager.Install(lte);

    Ipv4AddressHelper node0, routerAddr, node1, trafficSrc, trafficSink;

    node0.SetBase("10.0.0.0", "255.255.255.0");
    routerAddr.SetBase("11.0.0.0", "255.255.255.0");
    node1.SetBase("12.0.0.0", "255.255.255.0");
    trafficSrc.SetBase("13.0.0.0", "255.255.255.0");
    trafficSink.SetBase("14.0.0.0", "255.255.255.0");

    PointToPointHelper p2pHelper;
    p2pHelper.SetQueue("ns3::DropTailQueue<Packet>", "MaxSize",
                       StringValue("50p"));

    // +--------------------+
    // | MAKING CONNECTIONS |
    // +--------------------+

    //        T0             T1
    //        |              |
    // N0 <-> R0 <-> R1 <-> LTE <-> N1
    // -------------------------------

    // N0 <-> R0
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
    NetDeviceContainer n0r0 = p2pHelper.Install(nodes.Get(0), routers.Get(0));
    node0.Assign(n0r0);

    // T0 <-> R0
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
    NetDeviceContainer t0r0 = p2pHelper.Install(traffic.Get(0), routers.Get(0));
    trafficSrc.Assign(t0r0);

    // R0 <-> R1
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    // p2pHelper.SetChannelAttribute("Delay", StringValue("2ms"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("200ms"));
    NetDeviceContainer r0r1 = p2pHelper.Install(routers.Get(0), routers.Get(1));
    routerAddr.Assign(r0r1);

    // R1 <-> LTE
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
    NetDeviceContainer r1LteIp = p2pHelper.Install(routers.Get(1), lte.Get(0));
    routerAddr.Assign(r1LteIp);

    // LTE <-> N1
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("4Mbps"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("20ms"));
    NetDeviceContainer n1LteIp = p2pHelper.Install(nodes.Get(1), lte.Get(0));
    // n1LteIp.Get(0)->SetAttribute("ReceiveErrorModel",
    //  PointerValue(lteErrorModel));
    // n1LteIp.Get(1)->SetAttribute("ReceiveErrorModel",
    //  PointerValue(lteErrorModel));
    node1.Assign(n1LteIp);

    // LTE <-> T1
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
    NetDeviceContainer t1LteIp = p2pHelper.Install(traffic.Get(1), lte.Get(0));
    // t1LteIp.Get(0)->SetAttribute("ReceiveErrorModel",
    //  PointerValue(lteErrorModel));
    // t1LteIp.Get(1)->SetAttribute("ReceiveErrorModel",
    //  PointerValue(lteErrorModel));
    trafficSink.Assign(t1LteIp);

    //        T2             T3
    //        |              |
    // N0 <-> R2 <-> R3 <-> WiFi <-> N1
    // --------------------------------
    routerAddr.NewNetwork();
    node0.NewNetwork();
    node1.NewNetwork();
    trafficSrc.NewNetwork();
    trafficSink.NewNetwork();

    // N0 <-> R2
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
    NetDeviceContainer n0r2 = p2pHelper.Install(nodes.Get(0), routers.Get(2));
    node0.Assign(n0r2);

    // T2 <-> R2
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
    NetDeviceContainer t2r2 = p2pHelper.Install(traffic.Get(2), routers.Get(2));
    trafficSrc.Assign(t2r2);

    // R2 <-> R3
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    // p2pHelper.SetChannelAttribute("Delay", StringValue("1ms"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("100ms"));
    NetDeviceContainer r2r3 = p2pHelper.Install(routers.Get(2), routers.Get(3));
    routerAddr.Assign(r2r3);

    // R3 <-> WiFi
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
    NetDeviceContainer r3WifiIp = p2pHelper.Install(routers.Get(3), wifi.Get(0));
    routerAddr.Assign(r3WifiIp);

    // WiFi <-> N1
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("20ms"));
    NetDeviceContainer n1WifiIp = p2pHelper.Install(nodes.Get(1), wifi.Get(0));
    // n1WifiIp.Get(0)->SetAttribute("ReceiveErrorModel",
    //   PointerValue(wifiErrorModel));
    // n1WifiIp.Get(1)->SetAttribute("ReceiveErrorModel",
    //   PointerValue(wifiErrorModel));
    node1.Assign(n1WifiIp);

    // WiFi <-> T3
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("5ms"));
    NetDeviceContainer t3WifiIp = p2pHelper.Install(traffic.Get(3), wifi.Get(0));
    // t3WifiIp.Get(0)->SetAttribute("ReceiveErrorModel",
    //   PointerValue(wifiErrorModel));
    // t3WifiIp.Get(1)->SetAttribute("ReceiveErrorModel",
    //   PointerValue(wifiErrorModel));
    trafficSink.Assign(t3WifiIp);

    //        T4             T5
    //        |              |
    // N0 <-> R4 <-> R5 <-> WiMax <-> N1
    // ---------------------------------
    routerAddr.NewNetwork();
    node0.NewNetwork();
    node1.NewNetwork();
    trafficSrc.NewNetwork();
    trafficSink.NewNetwork();

    // N0 <-> R4
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
    NetDeviceContainer n0r4 = p2pHelper.Install(nodes.Get(0), routers.Get(4));
    node0.Assign(n0r4);

    // T2 <-> R2
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
    NetDeviceContainer t4r4 = p2pHelper.Install(traffic.Get(4), routers.Get(4));
    trafficSrc.Assign(t4r4);

    // R4 <-> R5
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    // p2pHelper.SetChannelAttribute("Delay", StringValue("5ms"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("50ms"));
    NetDeviceContainer r4r5 = p2pHelper.Install(routers.Get(4), routers.Get(5));
    routerAddr.Assign(r4r5);

    // R5 <-> WiMax
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
    NetDeviceContainer r5WimaxIp =
        p2pHelper.Install(routers.Get(5), wimax.Get(0));
    routerAddr.Assign(r5WimaxIp);

    // WiMax <-> N1
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("8Mbps"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("20ms"));
    NetDeviceContainer n1WimaxIp = p2pHelper.Install(nodes.Get(1), wimax.Get(0));
    // n1WimaxIp.Get(0)->SetAttribute("ReceiveErrorModel",
    //    PointerValue(wimaxErrorModel));
    // n1WimaxIp.Get(1)->SetAttribute("ReceiveErrorModel",
    //    PointerValue(wimaxErrorModel));
    node1.Assign(n1WimaxIp);

    // WiMax <-> T5
    p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2pHelper.SetChannelAttribute("Delay", StringValue("5ms"));
    NetDeviceContainer t5WimaxIp =
        p2pHelper.Install(traffic.Get(5), wimax.Get(0));
    // t5WimaxIp.Get(0)->SetAttribute("ReceiveErrorModel",
    //    PointerValue(wimaxErrorModel));
    // t5WimaxIp.Get(1)->SetAttribute("ReceiveErrorModel",
    //    PointerValue(wimaxErrorModel));
    trafficSink.Assign(t5WimaxIp);

    // +---------------------------+
    // | POPULATING ROUTING TABLES |
    // +---------------------------+

    // N0
    LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.5), "route add 12.0.0.1/32 via 10.0.0.2 dev sim0");
    LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.5), "route add 12.0.1.1/32 via 10.0.1.2 dev sim1");
    LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.5), "route add 12.0.2.1/32 via 10.0.2.2 dev sim2");

    // N1
    LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.5), "route add 10.0.0.1/32 via 12.0.0.2 dev sim0");
    LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.5), "route add 10.0.1.1/32 via 12.0.1.2 dev sim1");
    LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.5), "route add 10.0.2.1/32 via 12.0.2.2 dev sim2");

    // R0
    LinuxStackHelper::RunIp(routers.Get(0), Seconds(0.5), "route add 10.0.0.0/16 via 10.0.0.1 dev sim0");
    LinuxStackHelper::RunIp(routers.Get(0), Seconds(0.5), "route add 12.0.0.0/16 via 11.0.0.2 dev sim2");
    LinuxStackHelper::RunIp(routers.Get(0), Seconds(0.5), "route add 14.0.0.0/16 via 11.0.0.2 dev sim2");
    LinuxStackHelper::RunIp(routers.Get(0), Seconds(0.5), "route add 13.0.0.0/16 via 13.0.0.1 dev sim1");
    // R1
    LinuxStackHelper::RunIp(routers.Get(1), Seconds(0.5), "route add 10.0.0.0/16 via 11.0.0.1 dev sim0");
    LinuxStackHelper::RunIp(routers.Get(1), Seconds(0.5), "route add 12.0.0.0/16 via 11.0.0.4 dev sim1");
    LinuxStackHelper::RunIp(routers.Get(1), Seconds(0.5), "route add 13.0.0.0/16 via 11.0.0.1 dev sim0");
    LinuxStackHelper::RunIp(routers.Get(1), Seconds(0.5), "route add 14.0.0.0/16 via 11.0.0.4 dev sim1");
    // R2
    LinuxStackHelper::RunIp(routers.Get(2), Seconds(0.5), "route add 10.0.0.0/16 via 10.0.1.1 dev sim0");
    LinuxStackHelper::RunIp(routers.Get(2), Seconds(0.5), "route add 12.0.0.0/16 via 11.0.1.2 dev sim2");
    LinuxStackHelper::RunIp(routers.Get(2), Seconds(0.5), "route add 14.0.0.0/16 via 11.0.1.2 dev sim2");
    LinuxStackHelper::RunIp(routers.Get(2), Seconds(0.5), "route add 13.0.0.0/16 via 13.0.1.1 dev sim1");
    // R3
    LinuxStackHelper::RunIp(routers.Get(3), Seconds(0.5), "route add 10.0.0.0/16 via 11.0.1.1 dev sim0");
    LinuxStackHelper::RunIp(routers.Get(3), Seconds(0.5), "route add 12.0.0.0/16 via 11.0.1.4 dev sim1");
    LinuxStackHelper::RunIp(routers.Get(3), Seconds(0.5), "route add 13.0.0.0/16 via 11.0.1.1 dev sim0");
    LinuxStackHelper::RunIp(routers.Get(3), Seconds(0.5), "route add 14.0.0.0/16 via 11.0.1.4 dev sim1");
    // R4
    LinuxStackHelper::RunIp(routers.Get(4), Seconds(0.5), "route add 10.0.0.0/16 via 10.0.2.1 dev sim0");
    LinuxStackHelper::RunIp(routers.Get(4), Seconds(0.5), "route add 12.0.0.0/16 via 11.0.2.2 dev sim2");
    LinuxStackHelper::RunIp(routers.Get(4), Seconds(0.5), "route add 14.0.0.0/16 via 11.0.2.2 dev sim2");
    LinuxStackHelper::RunIp(routers.Get(4), Seconds(0.5), "route add 13.0.0.0/16 via 13.0.2.1 dev sim1");
    // R5
    LinuxStackHelper::RunIp(routers.Get(5), Seconds(0.5), "route add 10.0.0.0/16 via 11.0.2.1 dev sim0");
    LinuxStackHelper::RunIp(routers.Get(5), Seconds(0.5), "route add 12.0.0.0/16 via 11.0.2.4 dev sim1");
    LinuxStackHelper::RunIp(routers.Get(5), Seconds(0.5), "route add 13.0.0.0/16 via 11.0.2.1 dev sim0");
    LinuxStackHelper::RunIp(routers.Get(5), Seconds(0.5), "route add 14.0.0.0/16 via 11.0.2.4 dev sim1");
    // lte
    LinuxStackHelper::RunIp(lte.Get(0), Seconds(0.5), "route add 10.0.0.0/16 via 11.0.0.4 dev sim0");
    LinuxStackHelper::RunIp(lte.Get(0), Seconds(0.5), "route add 12.0.0.0/16 via 12.0.0.1 dev sim1");
    LinuxStackHelper::RunIp(lte.Get(0), Seconds(0.5), "route add 14.0.0.0/16 via 14.0.0.1 dev sim2");
    LinuxStackHelper::RunIp(lte.Get(0), Seconds(0.5), "route add 13.0.0.0/16 via 11.0.0.4 dev sim0");
    // wifi router
    LinuxStackHelper::RunIp(wifi.Get(0), Seconds(0.5), "route add 10.0.0.0/16 via 11.0.1.4 dev sim0");
    LinuxStackHelper::RunIp(wifi.Get(0), Seconds(0.5), "route add 12.0.0.0/16 via 12.0.1.1 dev sim1");
    LinuxStackHelper::RunIp(wifi.Get(0), Seconds(0.5), "route add 14.0.0.0/16 via 14.0.1.1 dev sim2");
    LinuxStackHelper::RunIp(wifi.Get(0), Seconds(0.5), "route add 13.0.0.0/16 via 11.0.1.4 dev sim0");
    // wimax router
    LinuxStackHelper::RunIp(wimax.Get(0), Seconds(0.5), "route add 10.0.0.0/16 via 11.0.2.4 dev sim0");
    LinuxStackHelper::RunIp(wimax.Get(0), Seconds(0.5), "route add 12.0.0.0/16 via 12.0.2.1 dev sim1");
    LinuxStackHelper::RunIp(wimax.Get(0), Seconds(0.5), "route add 14.0.0.0/16 via 14.0.2.1 dev sim2");
    LinuxStackHelper::RunIp(wimax.Get(0), Seconds(0.5), "route add 13.0.0.0/16 via 11.0.2.4 dev sim0");
    // T0
    LinuxStackHelper::RunIp(traffic.Get(0), Seconds(0.5), "route add 14.0.0.0/24 via 13.0.0.2 dev sim0");
    // T1
    LinuxStackHelper::RunIp(traffic.Get(1), Seconds(0.5), "route add 13.0.0.0/24 via 14.0.0.2 dev sim0");
    // T2
    LinuxStackHelper::RunIp(traffic.Get(2), Seconds(0.5), "route add 14.0.1.0/24 via 13.0.1.2 dev sim0");
    // T3
    LinuxStackHelper::RunIp(traffic.Get(3), Seconds(0.5), "route add 13.0.1.0/24 via 14.0.1.2 dev sim0");
    // T4
    LinuxStackHelper::RunIp(traffic.Get(4), Seconds(0.5), "route add 14.0.2.0/24 via 13.0.2.2 dev sim0");
    // T5
    LinuxStackHelper::RunIp(traffic.Get(5), Seconds(0.5), "route add 13.0.2.0/24 via 14.0.2.2 dev sim0");

    // +------------------------------+
    // | MPTCP RELATED CONFIGURATIONS |
    // +------------------------------+

    stack.SysctlSet(nodes, ".net.mptcp.mptcp_debug", "1");
    stack.SysctlSet(nodes, ".net.ipv4.tcp_congestion_control", congestionControl);

    LinuxStackHelper::SysctlGet(nodes.Get(0), Seconds(1),
                                ".net.ipv4.tcp_available_congestion_control",
                                &printTcpFlags);

    LinuxStackHelper::SysctlGet(nodes.Get(0), Seconds(1),
                                ".net.ipv4.tcp_congestion_control",
                                &printTcpFlags);

    // +---------------------+
    // | CRADLE APPLICATIONS |
    // +---------------------+

    ApplicationContainer apps;

    // MPTCP data
    // ----------

    // source on N0
    BulkSendHelper bulkSend = BulkSendHelper("ns3::LinuxTcpSocketFactory",
                                             InetSocketAddress("12.0.2.1", 9));
    int dataToSendMb = 100000;
    bulkSend.SetAttribute("MaxBytes", UintegerValue(dataToSendMb * 1000000));
    apps = bulkSend.Install(nodes.Get(0));
    apps.Start(Seconds(20));

    // sink on N1
    PacketSinkHelper sink =
        PacketSinkHelper("ns3::LinuxTcpSocketFactory",
                         InetSocketAddress(Ipv4Address::GetAny(), 9));
    apps = sink.Install(nodes.Get(1));
    apps.Start(Seconds(2));

    // UDP source

    // Forward Traffic
    // ---------------

    int mptcp_start = 20;

    // 50 - 100

    if (t)
    {
        OnOffHelper forward = OnOffHelper(
            "ns3::LinuxUdpSocketFactory", InetSocketAddress("14.0.0.1", 6000));
        forward.SetAttribute("DataRate", StringValue("101Mbps"));
        forward.SetAttribute("PacketSize", UintegerValue(15000));
        forward.SetAttribute(
            "OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        forward.SetAttribute(
            "OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        apps = forward.Install(traffic.Get(0));
        apps.Start(Seconds(mptcp_start + 50));
        apps.Stop(Seconds(mptcp_start + 100));
    }
    if (t)
    {
        // 100 - 150
        // OnOffHelper forward = OnOffHelper(
        //     "ns3::LinuxUdpSocketFactory", InetSocketAddress("14.0.1.1", 6000));
        // forward.SetAttribute("DataRate", StringValue("101Mbps"));
        // forward.SetAttribute("PacketSize", UintegerValue(15000));
        // forward.SetAttribute(
        //     "OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        // forward.SetAttribute(
        //     "OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        // apps = forward.Install(traffic.Get(2));
        // apps.Start(Seconds(mptcp_start + 100));
        // apps.Stop(Seconds(mptcp_start + 150));
    }
    if (t)
    {
        // 150 - 200
        // OnOffHelper forward = OnOffHelper(
        //     "ns3::LinuxUdpSocketFactory", InetSocketAddress("14.0.2.1", 6000));
        // forward.SetAttribute("DataRate", StringValue("101Mbps"));
        // forward.SetAttribute("PacketSize", UintegerValue(15000));
        // forward.SetAttribute(
        //     "OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        // forward.SetAttribute(
        //     "OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        // apps = forward.Install(traffic.Get(4));
        // apps.Start(Seconds(mptcp_start + 150));
        // apps.Stop(Seconds(mptcp_start + 200));
    }
    if (t)
    {
        // 200 - 400
        // OnOffHelper forward = OnOffHelper(
        //     "ns3::LinuxUdpSocketFactory", InetSocketAddress("13.0.0.1", 6000));
        // forward.SetAttribute("DataRate", StringValue("101Mbps"));
        // forward.SetAttribute("PacketSize", UintegerValue(15000));
        // forward.SetAttribute(
        //     "OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        // forward.SetAttribute(
        //     "OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        // apps = forward.Install(traffic.Get(1));
        // apps.Start(Seconds(mptcp_start + 200));
        // apps.Stop(Seconds(mptcp_start + 400));
    }
    if (t)
    {
        // OnOffHelper forward = OnOffHelper(
        //     "ns3::LinuxUdpSocketFactory", InetSocketAddress("13.0.1.1", 6000));
        // forward.SetAttribute("DataRate", StringValue("101Mbps"));
        // forward.SetAttribute("PacketSize", UintegerValue(15000));
        // forward.SetAttribute(
        //     "OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        // forward.SetAttribute(
        //     "OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        // apps = forward.Install(traffic.Get(3));
        // apps.Start(Seconds(mptcp_start + 200));
        // apps.Stop(Seconds(mptcp_start + 400));
    }
    if (t)
    {
        // OnOffHelper forward = OnOffHelper(
        //     "ns3::LinuxUdpSocketFactory", InetSocketAddress("13.0.2.1", 6000));
        // forward.SetAttribute("DataRate", StringValue("101Mbps"));
        // forward.SetAttribute("PacketSize", UintegerValue(15000));
        // forward.SetAttribute(
        //     "OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        // forward.SetAttribute(
        //     "OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        // apps = forward.Install(traffic.Get(5));
        // apps.Start(Seconds(mptcp_start + 200));
        // apps.Stop(Seconds(mptcp_start + 400));
    }

    // UDP sink
    PacketSinkHelper UdpTrafficSink =
        PacketSinkHelper("ns3::LinuxUdpSocketFactory",
                         InetSocketAddress(Ipv4Address::GetAny(), 6000));
    apps = UdpTrafficSink.Install(traffic.Get(0));
    apps.Start(Seconds(2));
    apps = UdpTrafficSink.Install(traffic.Get(1));
    apps.Start(Seconds(2));
    apps = UdpTrafficSink.Install(traffic.Get(2));
    apps.Start(Seconds(2));
    apps = UdpTrafficSink.Install(traffic.Get(3));
    apps.Start(Seconds(2));
    apps = UdpTrafficSink.Install(traffic.Get(4));
    apps.Start(Seconds(2));
    apps = UdpTrafficSink.Install(traffic.Get(5));
    apps.Start(Seconds(2));
    // }
    // +------------------------------------------------+
    // | ASSIGNING POSINTIONS AND CONFIGURING ANIMATION |
    // +------------------------------------------------+

    int yOffSet = 50;
    int xOffSet = 0;
    setPos(nodes.Get(0), 0 + xOffSet, 0 + yOffSet, 0);
    setPos(nodes.Get(1), 100 + xOffSet, 0 + yOffSet, 0);
    setPos(routers.Get(0), 25 + xOffSet, 25 + yOffSet, 0);
    setPos(routers.Get(1), 50 + xOffSet, 25 + yOffSet, 0);
    setPos(routers.Get(2), 25 + xOffSet, 0 + yOffSet, 0);
    setPos(routers.Get(3), 50 + xOffSet, 0 + yOffSet, 0);
    setPos(routers.Get(4), 25 + xOffSet, -25 + yOffSet, 0);
    setPos(routers.Get(5), 50 + xOffSet, -25 + yOffSet, 0);
    setPos(lte.Get(0), 75 + xOffSet, 25 + yOffSet, 0);
    setPos(wifi.Get(0), 75 + xOffSet, 0 + yOffSet, 0);
    setPos(wimax.Get(0), 75 + xOffSet, -25 + yOffSet, 0);
    setPos(traffic.Get(0), 25 - 5 + xOffSet, -10 + 25 + yOffSet, 0);
    setPos(traffic.Get(1), 75 + 5 + xOffSet, -10 + 25 + yOffSet, 0);
    setPos(traffic.Get(2), 25 - 5 + xOffSet, -10 + 0 + yOffSet, 0);
    setPos(traffic.Get(3), 75 + 5 + xOffSet, -10 + 0 + yOffSet, 0);
    setPos(traffic.Get(4), 25 - 5 + xOffSet, -10 - 25 + yOffSet, 0);
    setPos(traffic.Get(5), 75 + 5 + xOffSet, -10 - 25 + yOffSet, 0);

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
    anim.UpdateNodeColor(routers.Get(4)->GetId(), 255, 0, 0);
    anim.UpdateNodeSize(routers.Get(4)->GetId(), 4.0, 4.0);
    anim.UpdateNodeColor(routers.Get(5)->GetId(), 255, 0, 0);
    anim.UpdateNodeSize(routers.Get(5)->GetId(), 4.0, 4.0);

    anim.UpdateNodeColor(lte.Get(0)->GetId(), 255, 255, 0);
    anim.UpdateNodeSize(lte.Get(0)->GetId(), 4.0, 4.0);

    anim.UpdateNodeColor(wifi.Get(0)->GetId(), 255, 255, 0);
    anim.UpdateNodeSize(wifi.Get(0)->GetId(), 4.0, 4.0);

    anim.UpdateNodeColor(wimax.Get(0)->GetId(), 255, 255, 0);
    anim.UpdateNodeSize(wimax.Get(0)->GetId(), 4.0, 4.0);

    anim.UpdateNodeColor(traffic.Get(0)->GetId(), 0, 0, 255);
    anim.UpdateNodeSize(traffic.Get(0)->GetId(), 3.0, 3.0);
    anim.UpdateNodeColor(traffic.Get(1)->GetId(), 0, 0, 255);
    anim.UpdateNodeSize(traffic.Get(1)->GetId(), 3.0, 3.0);
    anim.UpdateNodeColor(traffic.Get(2)->GetId(), 0, 0, 255);
    anim.UpdateNodeSize(traffic.Get(2)->GetId(), 3.0, 3.0);
    anim.UpdateNodeColor(traffic.Get(3)->GetId(), 0, 0, 255);
    anim.UpdateNodeSize(traffic.Get(3)->GetId(), 3.0, 3.0);
    anim.UpdateNodeColor(traffic.Get(4)->GetId(), 0, 0, 255);
    anim.UpdateNodeSize(traffic.Get(4)->GetId(), 3.0, 3.0);
    anim.UpdateNodeColor(traffic.Get(5)->GetId(), 0, 0, 255);
    anim.UpdateNodeSize(traffic.Get(5)->GetId(), 3.0, 3.0);

    // +------------------------------------+
    // | PCAP AND FLOW MONITORING FOR STATS |
    // +------------------------------------+

    p2pHelper.EnablePcapAll("thesis-main-" + congestionControl, false);

    Simulator::Stop(Seconds(600));
    Simulator::Run();

    Simulator::Destroy();
=======
int main(int argc, char *argv[]) {

  std::string congestionControl = "lia";
  CommandLine cmd;
  cmd.AddValue("cc", "Congestion Control algorithm to use. Default is LIA",
               congestionControl);
  cmd.Parse(argc, argv);
  std::cout << "Using '" << congestionControl
            << "' congestion control algorithm." << std::endl;

  DceManagerHelper dceManager;
  dceManager.SetTaskManagerAttribute("FiberManagerType",
                                     StringValue("UcontextFiberManager"));

  dceManager.SetNetworkStack("ns3::LinuxSocketFdFactory", "Library",
                             StringValue("liblinux.so"));

  Ptr<RateErrorModel> lteErrorModel =
      CreateObjectWithAttributes<RateErrorModel>(
          "RanVar", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
          "ErrorRate", DoubleValue(0.02), "ErrorUnit",
          EnumValue(RateErrorModel::ERROR_UNIT_PACKET));
  Ptr<RateErrorModel> wifiErrorModel =
      CreateObjectWithAttributes<RateErrorModel>(
          "RanVar", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
          "ErrorRate", DoubleValue(0.1), "ErrorUnit",
          EnumValue(RateErrorModel::ERROR_UNIT_PACKET));
  Ptr<RateErrorModel> wimaxErrorModel =
      CreateObjectWithAttributes<RateErrorModel>(
          "RanVar", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
          "ErrorRate", DoubleValue(0.2), "ErrorUnit",
          EnumValue(RateErrorModel::ERROR_UNIT_PACKET));

  NodeContainer nodes, routers, lte, wifi, wimax, traffic;
  nodes.Create(2);
  routers.Create(6);
  lte.Create(1);
  wifi.Create(1);
  wimax.Create(1);
  traffic.Create(6);

  LinuxStackHelper stack;
  stack.Install(nodes);
  stack.Install(routers);
  stack.Install(wifi);
  stack.Install(lte);
  stack.Install(traffic);
  stack.Install(wimax);

  dceManager.Install(nodes);
  dceManager.Install(routers);
  dceManager.Install(wifi);
  dceManager.Install(wimax);
  dceManager.Install(traffic);
  dceManager.Install(lte);

  Ipv4AddressHelper node0, routerAddr, node1, trafficSrc, trafficSink;

  node0.SetBase("10.0.0.0", "255.255.255.0");
  routerAddr.SetBase("11.0.0.0", "255.255.255.0");
  node1.SetBase("12.0.0.0", "255.255.255.0");
  trafficSrc.SetBase("13.0.0.0", "255.255.255.0");
  trafficSink.SetBase("14.0.0.0", "255.255.255.0");

  PointToPointHelper p2pHelper;
  p2pHelper.SetQueue("ns3::DropTailQueue<Packet>", "MaxSize",
                     StringValue("50p"));

  // +--------------------+
  // | MAKING CONNECTIONS |
  // +--------------------+

  //        T0             T1
  //        |              |
  // N0 <-> R0 <-> R1 <-> LTE <-> N1
  // -------------------------------

  // N0 <-> R0
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
  NetDeviceContainer n0r0 = p2pHelper.Install(nodes.Get(0), routers.Get(0));
  node0.Assign(n0r0);

  // T0 <-> R0
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
  NetDeviceContainer t0r0 = p2pHelper.Install(traffic.Get(0), routers.Get(0));
  trafficSrc.Assign(t0r0);

  // R0 <-> R1
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("200ms"));
  NetDeviceContainer r0r1 = p2pHelper.Install(routers.Get(0), routers.Get(1));
  routerAddr.Assign(r0r1);

  // R1 <-> LTE
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
  NetDeviceContainer r1LteIp = p2pHelper.Install(routers.Get(1), lte.Get(0));
  routerAddr.Assign(r1LteIp);

  // LTE <-> N1
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("4Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("20ms"));
  NetDeviceContainer n1LteIp = p2pHelper.Install(nodes.Get(1), lte.Get(0));
  n1LteIp.Get(0)->SetAttribute("ReceiveErrorModel",
                               PointerValue(lteErrorModel));
  n1LteIp.Get(1)->SetAttribute("ReceiveErrorModel",
                               PointerValue(lteErrorModel));
  node1.Assign(n1LteIp);

  // LTE <-> T1
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("20ms"));
  NetDeviceContainer t1LteIp = p2pHelper.Install(traffic.Get(1), lte.Get(0));
  t1LteIp.Get(0)->SetAttribute("ReceiveErrorModel",
                               PointerValue(lteErrorModel));
  t1LteIp.Get(1)->SetAttribute("ReceiveErrorModel",
                               PointerValue(lteErrorModel));
  trafficSink.Assign(t1LteIp);

  //        T2             T3
  //        |              |
  // N0 <-> R2 <-> R3 <-> WiFi <-> N1
  // --------------------------------
  routerAddr.NewNetwork();
  node0.NewNetwork();
  node1.NewNetwork();
  trafficSrc.NewNetwork();
  trafficSink.NewNetwork();

  // N0 <-> R2
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
  NetDeviceContainer n0r2 = p2pHelper.Install(nodes.Get(0), routers.Get(2));
  node0.Assign(n0r2);

  // T2 <-> R2
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
  NetDeviceContainer t2r2 = p2pHelper.Install(traffic.Get(2), routers.Get(2));
  trafficSrc.Assign(t2r2);

  // R2 <-> R3
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("100ms"));
  NetDeviceContainer r2r3 = p2pHelper.Install(routers.Get(2), routers.Get(3));
  routerAddr.Assign(r2r3);

  // R3 <-> WiFi
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
  NetDeviceContainer r3WifiIp = p2pHelper.Install(routers.Get(3), wifi.Get(0));
  routerAddr.Assign(r3WifiIp);

  // WiFi <-> N1
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("20ms"));
  NetDeviceContainer n1WifiIp = p2pHelper.Install(nodes.Get(1), wifi.Get(0));
  n1WifiIp.Get(0)->SetAttribute("ReceiveErrorModel",
                                PointerValue(wifiErrorModel));
  n1WifiIp.Get(1)->SetAttribute("ReceiveErrorModel",
                                PointerValue(wifiErrorModel));
  node1.Assign(n1WifiIp);

  // WiFi <-> T3
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("20ms"));
  NetDeviceContainer t3WifiIp = p2pHelper.Install(traffic.Get(3), wifi.Get(0));
  t3WifiIp.Get(0)->SetAttribute("ReceiveErrorModel",
                                PointerValue(wifiErrorModel));
  t3WifiIp.Get(1)->SetAttribute("ReceiveErrorModel",
                                PointerValue(wifiErrorModel));
  trafficSink.Assign(t3WifiIp);

  //        T4             T5
  //        |              |
  // N0 <-> R4 <-> R5 <-> WiMax <-> N1
  // ---------------------------------
  routerAddr.NewNetwork();
  node0.NewNetwork();
  node1.NewNetwork();
  trafficSrc.NewNetwork();
  trafficSink.NewNetwork();

  // N0 <-> R4
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
  NetDeviceContainer n0r4 = p2pHelper.Install(nodes.Get(0), routers.Get(4));
  node0.Assign(n0r4);

  // T2 <-> R2
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
  NetDeviceContainer t4r4 = p2pHelper.Install(traffic.Get(4), routers.Get(4));
  trafficSrc.Assign(t4r4);

  // R4 <-> R5
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("50ms"));
  NetDeviceContainer r4r5 = p2pHelper.Install(routers.Get(4), routers.Get(5));
  routerAddr.Assign(r4r5);

  // R5 <-> WiMax
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("10ms"));
  NetDeviceContainer r5WimaxIp =
      p2pHelper.Install(routers.Get(5), wimax.Get(0));
  routerAddr.Assign(r5WimaxIp);

  // WiMax <-> N1
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("8Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("20ms"));
  NetDeviceContainer n1WimaxIp = p2pHelper.Install(nodes.Get(1), wimax.Get(0));
  n1WimaxIp.Get(0)->SetAttribute("ReceiveErrorModel",
                                 PointerValue(wimaxErrorModel));
  n1WimaxIp.Get(1)->SetAttribute("ReceiveErrorModel",
                                 PointerValue(wimaxErrorModel));
  node1.Assign(n1WimaxIp);

  // WiMax <-> T5
  p2pHelper.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  p2pHelper.SetChannelAttribute("Delay", StringValue("20ms"));
  NetDeviceContainer t5WimaxIp =
      p2pHelper.Install(traffic.Get(5), wimax.Get(0));
  t5WimaxIp.Get(0)->SetAttribute("ReceiveErrorModel",
                                 PointerValue(wimaxErrorModel));
  t5WimaxIp.Get(1)->SetAttribute("ReceiveErrorModel",
                                 PointerValue(wimaxErrorModel));
  trafficSink.Assign(t5WimaxIp);

  // +---------------------------+
  // | POPULATING ROUTING TABLES |
  // +---------------------------+

  // N0
  LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.5),
                          "rule add from 10.0.0.1/32 table 1");
  LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.5),
                          "rule add from 10.0.1.1/32 table 2");
  LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.5),
                          "rule add from 10.0.2.1/32 table 3");

  LinuxStackHelper::RunIp(
      nodes.Get(0), Seconds(0.5),
      "route add 12.0.0.0/24 via 10.0.0.2 dev sim0 scope link table 1");
  LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.5),
                          "route add default via 10.0.0.2 dev sim0 table 1");

  LinuxStackHelper::RunIp(
      nodes.Get(0), Seconds(0.5),
      "route add 12.0.1.0/24 via 10.0.1.2 dev sim1 scope link table 1");
  LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.5),
                          "route add default via 10.0.1.2 dev sim1 table 2");

  LinuxStackHelper::RunIp(
      nodes.Get(0), Seconds(0.5),
      "route add 12.0.2.0/24 via 10.0.2.2 dev sim2 scope link table 2");
  LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.5),
                          "route add default via 10.0.2.2 dev sim2 table 3");

  LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.5),
                          "route add default via 10.0.0.2 dev sim0");
  // N1
  LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.5),
                          "rule add from 12.0.0.1/32 table 1");
  LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.5),
                          "rule add from 12.0.1.1/32 table 2");
  LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.5),
                          "rule add from 12.0.2.1/32 table 3");

  LinuxStackHelper::RunIp(
      nodes.Get(1), Seconds(0.5),
      "route add 10.0.0.0/24 via 12.0.0.2 dev sim0 scope link table 1");
  LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.5),
                          "route add default via 12.0.0.2 dev sim0 table 1");

  LinuxStackHelper::RunIp(
      nodes.Get(1), Seconds(0.5),
      "route add 10.0.1.0/24 via 12.0.1.2 dev sim1 scope link table 2");
  LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.5),
                          "route add default via 12.0.1.2 dev sim0 table 2");

  LinuxStackHelper::RunIp(
      nodes.Get(1), Seconds(0.5),
      "route add 10.0.2.0/24 via 12.0.2.2 dev sim2 scope link table 3");
  LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.5),
                          "route add default via 12.0.2.2 dev sim0 table 3");

  LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.5),
                          "route add default via 12.0.0.2 dev sim0");
  // R0
  LinuxStackHelper::RunIp(routers.Get(0), Seconds(0.5),
                          "route add 10.0.0.0/16 via 10.0.0.1 dev sim0");
  LinuxStackHelper::RunIp(routers.Get(0), Seconds(0.5),
                          "route add 12.0.0.0/16 via 11.0.0.2 dev sim2");
  LinuxStackHelper::RunIp(routers.Get(0), Seconds(0.5),
                          "route add 14.0.0.0/16 via 11.0.0.2 dev sim2");
  LinuxStackHelper::RunIp(routers.Get(0), Seconds(0.5),
                          "route add 13.0.0.0/16 via 13.0.0.1 dev sim1");
  // R1
  LinuxStackHelper::RunIp(routers.Get(1), Seconds(0.5),
                          "route add 10.0.0.0/16 via 11.0.0.1 dev sim0");
  LinuxStackHelper::RunIp(routers.Get(1), Seconds(0.5),
                          "route add 12.0.0.0/16 via 11.0.0.4 dev sim1");
  LinuxStackHelper::RunIp(routers.Get(1), Seconds(0.5),
                          "route add 13.0.0.0/16 via 11.0.0.1 dev sim0");
  LinuxStackHelper::RunIp(routers.Get(1), Seconds(0.5),
                          "route add 14.0.0.0/16 via 11.0.0.4 dev sim1");
  // R2
  LinuxStackHelper::RunIp(routers.Get(2), Seconds(0.5),
                          "route add 10.0.0.0/16 via 10.0.1.1 dev sim0");
  LinuxStackHelper::RunIp(routers.Get(2), Seconds(0.5),
                          "route add 12.0.0.0/16 via 11.0.1.2 dev sim2");
  LinuxStackHelper::RunIp(routers.Get(2), Seconds(0.5),
                          "route add 14.0.0.0/16 via 11.0.1.2 dev sim2");
  LinuxStackHelper::RunIp(routers.Get(2), Seconds(0.5),
                          "route add 13.0.0.0/16 via 13.0.1.1 dev sim1");
  // R3
  LinuxStackHelper::RunIp(routers.Get(3), Seconds(0.5),
                          "route add 10.0.0.0/16 via 11.0.1.1 dev sim0");
  LinuxStackHelper::RunIp(routers.Get(3), Seconds(0.5),
                          "route add 12.0.0.0/16 via 11.0.1.4 dev sim1");
  LinuxStackHelper::RunIp(routers.Get(3), Seconds(0.5),
                          "route add 13.0.0.0/16 via 11.0.1.1 dev sim0");
  LinuxStackHelper::RunIp(routers.Get(3), Seconds(0.5),
                          "route add 14.0.0.0/16 via 11.0.1.4 dev sim1");
  // R4
  LinuxStackHelper::RunIp(routers.Get(4), Seconds(0.5),
                          "route add 10.0.0.0/16 via 10.0.2.1 dev sim0");
  LinuxStackHelper::RunIp(routers.Get(4), Seconds(0.5),
                          "route add 12.0.0.0/16 via 11.0.2.2 dev sim2");
  LinuxStackHelper::RunIp(routers.Get(4), Seconds(0.5),
                          "route add 14.0.0.0/16 via 11.0.2.2 dev sim2");
  LinuxStackHelper::RunIp(routers.Get(4), Seconds(0.5),
                          "route add 13.0.0.0/16 via 13.0.2.1 dev sim1");
  // R5
  LinuxStackHelper::RunIp(routers.Get(5), Seconds(0.5),
                          "route add 10.0.0.0/16 via 11.0.2.1 dev sim0");
  LinuxStackHelper::RunIp(routers.Get(5), Seconds(0.5),
                          "route add 12.0.0.0/16 via 11.0.2.4 dev sim1");
  LinuxStackHelper::RunIp(routers.Get(5), Seconds(0.5),
                          "route add 13.0.0.0/16 via 11.0.2.1 dev sim0");
  LinuxStackHelper::RunIp(routers.Get(5), Seconds(0.5),
                          "route add 14.0.0.0/16 via 11.0.2.4 dev sim1");
  // lte
  LinuxStackHelper::RunIp(lte.Get(0), Seconds(0.5),
                          "route add 10.0.0.0/16 via 11.0.0.4 dev sim0");
  LinuxStackHelper::RunIp(lte.Get(0), Seconds(0.5),
                          "route add 12.0.0.0/16 via 12.0.0.1 dev sim1");
  LinuxStackHelper::RunIp(lte.Get(0), Seconds(0.5),
                          "route add 14.0.0.0/16 via 14.0.0.1 dev sim2");
  LinuxStackHelper::RunIp(lte.Get(0), Seconds(0.5),
                          "route add 13.0.0.0/16 via 11.0.0.4 dev sim0");
  // wifi router
  LinuxStackHelper::RunIp(wifi.Get(0), Seconds(0.5),
                          "route add 10.0.0.0/16 via 11.0.1.4 dev sim0");
  LinuxStackHelper::RunIp(wifi.Get(0), Seconds(0.5),
                          "route add 12.0.0.0/16 via 12.0.1.1 dev sim1");
  LinuxStackHelper::RunIp(wifi.Get(0), Seconds(0.5),
                          "route add 14.0.0.0/16 via 14.0.1.1 dev sim2");
  LinuxStackHelper::RunIp(wifi.Get(0), Seconds(0.5),
                          "route add 13.0.0.0/16 via 11.0.1.4 dev sim0");
  // wimax router
  LinuxStackHelper::RunIp(wimax.Get(0), Seconds(0.5),
                          "route add 10.0.0.0/16 via 11.0.2.4 dev sim0");
  LinuxStackHelper::RunIp(wimax.Get(0), Seconds(0.5),
                          "route add 12.0.0.0/16 via 12.0.2.1 dev sim1");
  LinuxStackHelper::RunIp(wimax.Get(0), Seconds(0.5),
                          "route add 14.0.0.0/16 via 14.0.2.1 dev sim2");
  LinuxStackHelper::RunIp(wimax.Get(0), Seconds(0.5),
                          "route add 13.0.0.0/16 via 11.0.2.4 dev sim0");
  // T0
  LinuxStackHelper::RunIp(traffic.Get(0), Seconds(0.5),
                          "route add 14.0.0.0/24 via 13.0.0.2 dev sim0");
  // T1
  LinuxStackHelper::RunIp(traffic.Get(1), Seconds(0.5),
                          "route add 13.0.0.0/24 via 14.0.0.2 dev sim0");
  // T2
  LinuxStackHelper::RunIp(traffic.Get(2), Seconds(0.5),
                          "route add 14.0.1.0/24 via 13.0.1.2 dev sim0");
  // T3
  LinuxStackHelper::RunIp(traffic.Get(3), Seconds(0.5),
                          "route add 13.0.1.0/24 via 14.0.1.2 dev sim0");
  // T4
  LinuxStackHelper::RunIp(traffic.Get(4), Seconds(0.5),
                          "route add 14.0.2.0/24 via 13.0.2.2 dev sim0");
  // T5
  LinuxStackHelper::RunIp(traffic.Get(5), Seconds(0.5),
                          "route add 13.0.2.0/24 via 14.0.2.2 dev sim0");

  // +------------------------------+
  // | MPTCP RELATED CONFIGURATIONS |
  // +------------------------------+

  stack.SysctlSet(nodes, ".net.mptcp.mptcp_debug", "1");
  stack.SysctlSet(nodes, ".net.ipv4.tcp_congestion_control", congestionControl);

  LinuxStackHelper::SysctlGet(nodes.Get(0), Seconds(1),
                              ".net.ipv4.tcp_available_congestion_control",
                              &printTcpFlags);

  LinuxStackHelper::SysctlGet(nodes.Get(0), Seconds(1),
                              ".net.ipv4.tcp_congestion_control",
                              &printTcpFlags);

  // +---------------------+
  // | CRADLE APPLICATIONS |
  // +---------------------+

  ApplicationContainer apps;

  // MPTCP data
  // ----------

  // source on N0
  BulkSendHelper bulkSend = BulkSendHelper("ns3::LinuxTcpSocketFactory",
                                           InetSocketAddress("12.0.2.1", 9));
  int dataToSendMb = 100000;
  bulkSend.SetAttribute("MaxBytes", UintegerValue(dataToSendMb * 1000000));
  apps = bulkSend.Install(nodes.Get(0));
  apps.Start(Seconds(20));

  // sink on N1
  PacketSinkHelper sink =
      PacketSinkHelper("ns3::LinuxTcpSocketFactory",
                       InetSocketAddress(Ipv4Address::GetAny(), 9));
  apps = sink.Install(nodes.Get(1));
  apps.Start(Seconds(2));

  // Traffic Data
  // ------------

  // UDP source

// // T0
// OnOffHelper onOffUdpT0_1500 = OnOffHelper(
//     "ns3::LinuxUdpSocketFactory", InetSocketAddress("14.0.0.1", 6000));
// onOffUdpT0_1500.SetAttribute("DataRate", StringValue("50Mbps"));
// onOffUdpT0_1500.SetAttribute("PacketSize", UintegerValue(1500));
// onOffUdpT0_1500.SetAttribute(
//     "OnTime", StringValue("ns3::ConstantRandomVariable[Constant=01]"));
// onOffUdpT0_1500.SetAttribute(
//     "OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
// apps = onOffUdpT0_1500.Install(traffic.Get(0));
// apps.Start(Seconds(60));
// apps.Stop(Seconds(110));
//
// // T2
// OnOffHelper onOffUdpT2_1500 = OnOffHelper(
//     "ns3::LinuxUdpSocketFactory", InetSocketAddress("14.0.1.1", 6000));
// onOffUdpT2_1500.SetAttribute("DataRate", StringValue("50Mbps"));
// onOffUdpT2_1500.SetAttribute("PacketSize", UintegerValue(1500));
// onOffUdpT2_1500.SetAttribute(
//     "OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
// onOffUdpT2_1500.SetAttribute(
//     "OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
// apps = onOffUdpT2_1500.Install(traffic.Get(2));
// apps.Start(Seconds(60));
// apps.Stop(Seconds(110));
//
// // T4
// OnOffHelper onOffUdpT4_1500 = OnOffHelper(
//     "ns3::LinuxUdpSocketFactory", InetSocketAddress("14.0.2.1", 6000));
// onOffUdpT4_1500.SetAttribute("DataRate", StringValue("50Mbps"));
// onOffUdpT4_1500.SetAttribute("PacketSize", UintegerValue(1500));
// onOffUdpT4_1500.SetAttribute(
//     "OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
// onOffUdpT4_1500.SetAttribute(
//     "OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
// apps = onOffUdpT4_1500.Install(traffic.Get(4));
// apps.Start(Seconds(60));
// apps.Stop(Seconds(110));

  // TCP sink
  // PacketSinkHelper TcpTrafficSink =
  //     PacketSinkHelper("ns3::LinuxTcpSocketFactory",
  //                      InetSocketAddress(Ipv4Address::GetAny(), 5000));
  // apps = TcpTrafficSink.Install(traffic.Get(1));
  // apps.Start(Seconds(2));
  // apps = TcpTrafficSink.Install(traffic.Get(3));
  // apps.Start(Seconds(2));
  // apps = TcpTrafficSink.Install(traffic.Get(5));
  // apps.Start(Seconds(2));

  // UDP sink
  PacketSinkHelper UdpTrafficSink =
      PacketSinkHelper("ns3::LinuxUdpSocketFactory",
                       InetSocketAddress(Ipv4Address::GetAny(), 6000));
  apps = UdpTrafficSink.Install(traffic.Get(1));
  apps.Start(Seconds(2));
  apps = UdpTrafficSink.Install(traffic.Get(3));
  apps.Start(Seconds(2));
  apps = UdpTrafficSink.Install(traffic.Get(5));
  apps.Start(Seconds(2));

  // +------------------------------------------------+
  // | ASSIGNING POSINTIONS AND CONFIGURING ANIMATION |
  // +------------------------------------------------+

  int yOffSet = 50;
  int xOffSet = 0;
  setPos(nodes.Get(0), 0 + xOffSet, 0 + yOffSet, 0);
  setPos(nodes.Get(1), 100 + xOffSet, 0 + yOffSet, 0);
  setPos(routers.Get(0), 25 + xOffSet, 25 + yOffSet, 0);
  setPos(routers.Get(1), 50 + xOffSet, 25 + yOffSet, 0);
  setPos(routers.Get(2), 25 + xOffSet, 0 + yOffSet, 0);
  setPos(routers.Get(3), 50 + xOffSet, 0 + yOffSet, 0);
  setPos(routers.Get(4), 25 + xOffSet, -25 + yOffSet, 0);
  setPos(routers.Get(5), 50 + xOffSet, -25 + yOffSet, 0);
  setPos(lte.Get(0), 75 + xOffSet, 25 + yOffSet, 0);
  setPos(wifi.Get(0), 75 + xOffSet, 0 + yOffSet, 0);
  setPos(wimax.Get(0), 75 + xOffSet, -25 + yOffSet, 0);
  setPos(traffic.Get(0), 25 - 5 + xOffSet, -10 + 25 + yOffSet, 0);
  setPos(traffic.Get(1), 75 + 5 + xOffSet, -10 + 25 + yOffSet, 0);
  setPos(traffic.Get(2), 25 - 5 + xOffSet, -10 + 0 + yOffSet, 0);
  setPos(traffic.Get(3), 75 + 5 + xOffSet, -10 + 0 + yOffSet, 0);
  setPos(traffic.Get(4), 25 - 5 + xOffSet, -10 - 25 + yOffSet, 0);
  setPos(traffic.Get(5), 75 + 5 + xOffSet, -10 - 25 + yOffSet, 0);

  AnimationInterface anim("thesis-anim.xml");
  anim.SetMaxPktsPerTraceFile(1 * 1000000);
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
  anim.UpdateNodeColor(routers.Get(4)->GetId(), 255, 0, 0);
  anim.UpdateNodeSize(routers.Get(4)->GetId(), 4.0, 4.0);
  anim.UpdateNodeColor(routers.Get(5)->GetId(), 255, 0, 0);
  anim.UpdateNodeSize(routers.Get(5)->GetId(), 4.0, 4.0);

  anim.UpdateNodeColor(lte.Get(0)->GetId(), 255, 255, 0);
  anim.UpdateNodeSize(lte.Get(0)->GetId(), 4.0, 4.0);

  anim.UpdateNodeColor(wifi.Get(0)->GetId(), 255, 255, 0);
  anim.UpdateNodeSize(wifi.Get(0)->GetId(), 4.0, 4.0);

  anim.UpdateNodeColor(wimax.Get(0)->GetId(), 255, 255, 0);
  anim.UpdateNodeSize(wimax.Get(0)->GetId(), 4.0, 4.0);

  anim.UpdateNodeColor(traffic.Get(0)->GetId(), 0, 0, 255);
  anim.UpdateNodeSize(traffic.Get(0)->GetId(), 3.0, 3.0);
  anim.UpdateNodeColor(traffic.Get(1)->GetId(), 0, 0, 255);
  anim.UpdateNodeSize(traffic.Get(1)->GetId(), 3.0, 3.0);
  anim.UpdateNodeColor(traffic.Get(2)->GetId(), 0, 0, 255);
  anim.UpdateNodeSize(traffic.Get(2)->GetId(), 3.0, 3.0);
  anim.UpdateNodeColor(traffic.Get(3)->GetId(), 0, 0, 255);
  anim.UpdateNodeSize(traffic.Get(3)->GetId(), 3.0, 3.0);
  anim.UpdateNodeColor(traffic.Get(4)->GetId(), 0, 0, 255);
  anim.UpdateNodeSize(traffic.Get(4)->GetId(), 3.0, 3.0);
  anim.UpdateNodeColor(traffic.Get(5)->GetId(), 0, 0, 255);
  anim.UpdateNodeSize(traffic.Get(5)->GetId(), 3.0, 3.0);

  // +------------------------------------+
  // | PCAP AND FLOW MONITORING FOR STATS |
  // +------------------------------------+

  p2pHelper.EnablePcapAll("thesis-main-" + congestionControl, false);
  // p2pHelper.EnableAsciiAll("thesis-main");

  Simulator::Stop(Seconds(160));
  Simulator::Run();

  Simulator::Destroy();
>>>>>>> parent of 035b741... updated routing
}
