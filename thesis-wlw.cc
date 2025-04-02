//
//          +------+                                                   +------+
//          |  T0  |                                                   |  T1  |
//          +------+                                                   +------+
//           (0,20) \                                                 / (15,20)
//                   \ 1Gbps                                         / 1Gbps
//                    \                                             /
//                     \                                           /
//                      +------+           /\              +------+
//                      |  R0  |-----------  \  -----------|  R1  |
//                      +------+              \/           +------+
//                     / (5,15)                             (10,15)\ 
//                    / 1Gbps                                       \ 1Gbps
//                   /                                               \ 
//           +------+                                               +------+
//           |  N0  |                                               |  N1  |
//           +------+                                               +------+
//            (0,10) \                                               / (15,10)
//                    \ 1Gbps                                       / 1Gbps
//                     \                                           /
//                      +------+           10Mbps          +------+
//                      |  R2  |---------------------------|  R3  |
//                      +------+                           +------+
//                     /  (5,5)                             (10,5) \ 
//                    /                                             \ 
//                   / 5Mbps                                         \ 5Mbps
//                  /                                                 \ 
//          +------+                                                   +------+
//          |  T2  |                                                   |  T3  |
//          +------+                                                   +------+
//           (0,0)                                                      (15,0)

#include "ns3/applications-module.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/core-module.h"
#include "ns3/dce-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h"
#include "ns3/packet-sink.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/tcp-westwood.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("thesis");

void setPos(Ptr<Node> n, int x, int y, int z)
{
        Ptr<ConstantPositionMobilityModel> loc =
            CreateObject<ConstantPositionMobilityModel>();
        Vector locVec2(x, y, z);
        loc->SetPosition(locVec2);
        n->AggregateObject(loc);
}

int main()
{
        DceManagerHelper dceManager;
        dceManager.SetTaskManagerAttribute("FiberManagerType",
                                           StringValue("UcontextFiberManager"));

        dceManager.SetNetworkStack("ns3::LinuxSocketFdFactory", "Library",
                                   StringValue("liblinux.so"));

        NodeContainer nodes, wirelessRouters, wiredRouters, traffic;
        nodes.Create(2);
        wirelessRouters.Create(2);
        wiredRouters.Create(2);
        traffic.Create(4);

        LinuxStackHelper stack;
        stack.Install(nodes);
        stack.Install(wirelessRouters);
        stack.Install(wiredRouters);
        stack.Install(traffic);

        dceManager.Install(nodes);
        dceManager.Install(wirelessRouters);
        dceManager.Install(wiredRouters);
        dceManager.Install(traffic);

        PointToPointHelper pointToPoint;
        Ipv4AddressHelper r0addr, r1addr, r2addr, r3addr, r0r1addr, r2r3addr;
        std::ostringstream cmd_oss;

        r0addr.SetBase("10.0.0.0", "255.255.255.0");
        r1addr.SetBase("10.1.0.0", "255.255.255.0");
        r2addr.SetBase("10.2.0.0", "255.255.255.0");
        r3addr.SetBase("10.3.0.0", "255.255.255.0");
        r0r1addr.SetBase("11.0.0.0", "255.255.255.0");
        r2r3addr.SetBase("11.1.0.0", "255.255.255.0");

        // SETTING UP POSITIONS

        setPos(wirelessRouters.Get(0), 5, 15, 0);
        setPos(wirelessRouters.Get(1), 10, 15, 0);
        setPos(nodes.Get(0), 0, 10, 0);
        setPos(nodes.Get(1), 15, 10, 0);

        // SETTING UP ROUTERS

        pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
        pointToPoint.SetChannelAttribute("Delay", StringValue("10ms"));

        // connecting R0 <-> R1

        WifiHelper wifi;
        YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
        YansWifiPhyHelper phy;
        phy.SetChannel(channel.Create());
        WifiMacHelper mac;
        Ssid ssid = Ssid("thesis");

        mac.SetType("ns3::AdhocWifiMac", "Ssid", SsidValue(ssid));
        NetDeviceContainer r0r1 = wifi.Install(phy, mac, wirelessRouters);
        MobilityHelper mobility;
        mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
        mobility.Install(wirelessRouters);

        r0r1addr.Assign(r0r1);

        // routing
        LinuxStackHelper::RunIp(wirelessRouters.Get(0), Seconds(0.2), "route add 10.1.0.0/24 via 11.0.0.2 dev sim0"); // outbound - R1 network
        LinuxStackHelper::RunIp(wirelessRouters.Get(0), Seconds(0.2), "route add 10.0.0.1/32 via 10.0.0.1 dev sim1"); // inbound - N0
        LinuxStackHelper::RunIp(wirelessRouters.Get(0), Seconds(0.2), "route add 10.0.0.3/32 via 10.0.0.3 dev sim2"); // inbound - T0
        //
        LinuxStackHelper::RunIp(wirelessRouters.Get(1), Seconds(0.2), "route add 10.0.0.0/24 via 11.0.0.1 dev sim0"); // outbound - R0 network
        LinuxStackHelper::RunIp(wirelessRouters.Get(1), Seconds(0.2), "route add 10.1.0.1/32 via 10.1.0.1 dev sim1"); // inbound - N1
        LinuxStackHelper::RunIp(wirelessRouters.Get(1), Seconds(0.2), "route add 10.1.0.3/32 via 10.1.0.3 dev sim2"); // inbound - T1

        //

        // connectin R2 <-> R3
        NetDeviceContainer r2r3 = pointToPoint.Install(wiredRouters.Get(0), wiredRouters.Get(1));
        Ipv4InterfaceContainer r2r3Ip = r2r3addr.Assign(r2r3);
        LinuxStackHelper::RunIp(wiredRouters.Get(0), Seconds(0.2), "route add 10.3.0.0/24 via 11.1.0.2 dev sim0"); // outbound to R3 network
        LinuxStackHelper::RunIp(wiredRouters.Get(0), Seconds(0.2), "route add 10.2.0.1/32 via 10.2.0.1 dev sim1"); // inbound - N0
        LinuxStackHelper::RunIp(wiredRouters.Get(0), Seconds(0.2), "route add 10.2.0.3/32 via 10.2.0.3 dev sim2"); // inbound - T2

        LinuxStackHelper::RunIp(wiredRouters.Get(1), Seconds(0.2), "route add 10.2.0.0/24 via 11.1.0.1 dev sim0"); // outbound to R2 network
        LinuxStackHelper::RunIp(wiredRouters.Get(1), Seconds(0.2), "route add 10.3.0.1/32 via 10.3.0.1 dev sim1"); // inbound - N1
        LinuxStackHelper::RunIp(wiredRouters.Get(1), Seconds(0.2), "route add 10.3.0.3/32 via 10.3.0.3 dev sim2"); // inbound - T3

        // SETTING UP MPTCP NODES

        pointToPoint.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
        pointToPoint.SetChannelAttribute("Delay", StringValue("1ns"));

        // connecting N0 <-> R0
        NetDeviceContainer n0r0 = pointToPoint.Install(nodes.Get(0), wirelessRouters.Get(0));
        Ipv4InterfaceContainer n0r0Ip = r0addr.Assign(n0r0);
        LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "rule add from 10.0.0.1/24 table 1");
        LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "route add 10.0.0.0/24 dev sim0 scope link table 1");
        LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "route add default via 10.0.0.2 dev sim0 table 1");

        // connecting N0 <-> R2
        NetDeviceContainer n0r2 = pointToPoint.Install(nodes.Get(0), wiredRouters.Get(0));
        Ipv4InterfaceContainer n0r2Ip = r2addr.Assign(n0r2);
        LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "rule add from 10.2.0.1/24 table 2");
        LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "route add 10.2.0.0/24 dev sim0 scope link table 2");
        LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "route add default via 10.2.0.2 dev sim0 table 2");

        // setting default route
        LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "route add default via 10.2.0.2 dev sim1");

        // connecting N1 <-> R1
        // NetDeviceContainer n1r1 = pointToPoint.Install(nodes.Get(1), wirelessRouters.Get(1));
        // Ipv4InterfaceContainer n1r1Ip = r1addr.Assign(n1r1);
        // LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "rule add from 10.1.0.1/24 table 1");
        // LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "route add 10.1.0.0/24 dev sim0 scope link table 1");
        // LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "route add default via 10.1.0.2 dev sim0 table 1");

        // connecting N1 <-> R3
        NetDeviceContainer n1r3 = pointToPoint.Install(nodes.Get(1), wiredRouters.Get(1));
        Ipv4InterfaceContainer n1r3Ip = r3addr.Assign(n1r3);
        LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "rule add from 10.3.0.1/24 table 2");
        LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "route add 10.3.0.0/24 dev sim0 scope link table 2");
        LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "route add default via 10.3.0.2 dev sim0 table 2");

        // setting default route
        LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "route add default via 10.3.0.2 dev sim1");

        // SETTING UP TRAFFIC NODES

        pointToPoint.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
        pointToPoint.SetChannelAttribute("Delay", StringValue("1ns"));

        // // connecting T0 <-> R0
        NetDeviceContainer t0r0 = pointToPoint.Install(traffic.Get(0), wirelessRouters.Get(0));
        Ipv4InterfaceContainer t0r0Ip = r0addr.Assign(t0r0);
        LinuxStackHelper::RunIp(traffic.Get(0), Seconds(0.1), "route add default via 10.0.0.4 dev sim0");

        // // connecting T1 <-> R1
        NetDeviceContainer t1r1 = pointToPoint.Install(traffic.Get(1), wirelessRouters.Get(1));
        Ipv4InterfaceContainer t1r1Ip = r1addr.Assign(t1r1);
        LinuxStackHelper::RunIp(traffic.Get(1), Seconds(0.1), "route add default via 10.1.0.4 dev sim0");

        // // connecting T2 <-> R2
        NetDeviceContainer t2r2 = pointToPoint.Install(traffic.Get(2), wiredRouters.Get(0));
        Ipv4InterfaceContainer t2r2Ip = r2addr.Assign(t2r2);
        LinuxStackHelper::RunIp(traffic.Get(2), Seconds(0.1), "route add default via 10.2.0.4 dev sim0");

        // // connecting T3 <-> R3
        NetDeviceContainer t3r3 = pointToPoint.Install(traffic.Get(3), wiredRouters.Get(1));
        Ipv4InterfaceContainer t3r3Ip = r3addr.Assign(t3r3);
        LinuxStackHelper::RunIp(traffic.Get(3), Seconds(0.1), "route add default via 10.3.0.4 dev sim0");

        // debug
        stack.SysctlSet(nodes, ".net.mptcp.mptcp_debug", "1");
        // LinuxStackHelper::RunIp(wirelessRouters.Get(0), Seconds(4), "addr show");
        // LinuxStackHelper::RunIp(wirelessRouters.Get(1), Seconds(4), "addr show");

        DceApplicationHelper dce;
        ApplicationContainer apps;

        dce.SetStackSize(1 << 20);

        // wifi testing apps

        // MPTCP APPS

        // Launch iperf client on node 0 - MPTCP traffic
        dce.SetBinary("iperf");
        dce.ResetArguments();
        dce.ResetEnvironment();
        dce.ParseArguments("-c 10.1.0.1 -i 1 --time 60");
        apps = dce.Install(nodes.Get(0));
        apps.Start(Seconds(5.0));
        apps.Stop(Seconds(66));

        // Launch iperf server on node 1
        dce.SetBinary("iperf");
        dce.ResetArguments();
        dce.ResetEnvironment();
        dce.ParseArguments("-s");
        apps = dce.Install(nodes.Get(1));
        apps.Start(Seconds(2));
        apps.Stop(Seconds(70));

        // UDP APPS
        dce.SetBinary("iperf");
        dce.ResetArguments();
        dce.ResetEnvironment();
        dce.ParseArguments("-u -c 10.1.0.3 -b 7M --time 20");
        apps = dce.Install(traffic.Get(0));
        apps.Start(Seconds(15.0));
        apps.Stop(Seconds(40));

        dce.SetBinary("iperf");
        dce.ResetArguments();
        dce.ResetEnvironment();
        dce.ParseArguments("-u -c 10.3.0.3 -b 7M --time 20");
        apps = dce.Install(traffic.Get(2));
        apps.Start(Seconds(25.0));
        apps.Stop(Seconds(50));

        dce.SetBinary("iperf");
        dce.ResetArguments();
        dce.ResetEnvironment();
        dce.ParseArguments("-u -s");
        apps = dce.Install(traffic.Get(1));
        apps.Start(Seconds(2));
        apps.Stop(Seconds(45));

        dce.SetBinary("iperf");
        dce.ResetArguments();
        dce.ResetEnvironment();
        dce.ParseArguments("-u -s");
        apps = dce.Install(traffic.Get(3));
        apps.Start(Seconds(2));
        apps.Stop(Seconds(55));

        pointToPoint.EnablePcapAll("thesis-wlw", false);

        Simulator::Stop(Seconds(80.0));
        Simulator::Run();
        Simulator::Destroy();

        return 0;
}
