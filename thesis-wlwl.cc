//
//          +------+                                                   +------+
//          |  T0  |                                                   |  T1  |
//          +------+                                                   +------+
//                  \                                                 /
//                   \ 1Gbps                                         / 1Gbps
//                    \                                             /
//                     \                                           /
//                      +------+           /\              +------+
//                      |  R0  |-----------  \  -----------|  R1  |
//                      +------+              \/           +------+
//                     /                                           \ 
//                    / 1Gbps                                       \ 1Gbps
//                   /                                               \ 
//           +------+                                               +------+
//           |  N0  |                                               |  N1  |
//           +------+                                               +------+
//                   \                                               /
//                    \ 1Gbps                                       / 1Gbps
//                     \                                           /
//                      +------+           10Mbps          +------+
//                      |  R2  |---------------------------|  R3  |
//                      +------+                           +------+
//                     /                                           \ 
//                    /                                             \ 
//                   / 5Mbps                                         \ 5Mbps
//                  /                                                 \ 
//          +------+                                                   +------+
//          |  T2  |                                                   |  T3  |
//          +------+                                                   +------+
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

int main()
{
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

        // SETTING UP ROUTERS

        pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
        pointToPoint.SetChannelAttribute("Delay", StringValue("10ms"));

        // connecting R0 <-> R1
        NetDeviceContainer r0r1 = pointToPoint.Install(routers.Get(0), routers.Get(1));
        Ipv4InterfaceContainer r0r1Ip = r0r1addr.Assign(r0r1);
        LinuxStackHelper::RunIp(routers.Get(0), Seconds(0.2), "route add 10.1.0.0/24 via 11.0.0.1 dev sim0"); // outbound - R1 network
        LinuxStackHelper::RunIp(routers.Get(0), Seconds(0.2), "route add 10.0.0.1/32 via 10.0.0.2 dev sim1"); // inbound - N0
        LinuxStackHelper::RunIp(routers.Get(0), Seconds(0.2), "route add 10.0.0.3/32 via 10.0.0.4 dev sim2"); // inbound - T0

        LinuxStackHelper::RunIp(routers.Get(1), Seconds(0.2), "route add 10.0.0.0/24 via 11.0.0.2 dev sim0"); // outbound - R0 network
        LinuxStackHelper::RunIp(routers.Get(1), Seconds(0.2), "route add 10.1.0.1/32 via 10.1.0.2 dev sim1"); // inbound - N1
        LinuxStackHelper::RunIp(routers.Get(1), Seconds(0.2), "route add 10.1.0.3/32 via 10.1.0.4 dev sim2"); // inbound - T1

        // connectin R2 <-> R3
        NetDeviceContainer r2r3 = pointToPoint.Install(routers.Get(2), routers.Get(3));
        Ipv4InterfaceContainer r2r3Ip = r2r3addr.Assign(r2r3);
        LinuxStackHelper::RunIp(routers.Get(2), Seconds(0.2), "route add 10.3.0.0/24 via 11.1.0.1 dev sim0"); // outbound to R3 network
        LinuxStackHelper::RunIp(routers.Get(2), Seconds(0.2), "route add 10.2.0.1/32 via 10.2.0.2 dev sim1"); // inbound - N0
        LinuxStackHelper::RunIp(routers.Get(2), Seconds(0.2), "route add 10.2.0.3/32 via 10.2.0.4 dev sim2"); // inbound - T2

        LinuxStackHelper::RunIp(routers.Get(3), Seconds(0.2), "route add 10.2.0.0/24 via 11.1.0.2 dev sim0"); // outbound to R2 network
        LinuxStackHelper::RunIp(routers.Get(3), Seconds(0.2), "route add 10.3.0.1/32 via 10.3.0.2 dev sim1"); // inbound - N1
        LinuxStackHelper::RunIp(routers.Get(3), Seconds(0.2), "route add 10.3.0.3/32 via 10.3.0.4 dev sim2"); // inbound - T3

        // SETTING UP MPTCP NODES

        pointToPoint.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
        pointToPoint.SetChannelAttribute("Delay", StringValue("1ns"));

        // connecting N0 <-> R0
        NetDeviceContainer n0r0 = pointToPoint.Install(nodes.Get(0), routers.Get(0));
        Ipv4InterfaceContainer n0r0Ip = r0addr.Assign(n0r0);
        LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "rule add from 10.0.0.1/24 table 1");
        LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "route add 10.0.0.0/24 dev sim0 scope link table 1");
        LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "route add default via 10.0.0.2 dev sim0 table 1");

        // connecting N0 <-> R2
        NetDeviceContainer n0r2 = pointToPoint.Install(nodes.Get(0), routers.Get(2));
        Ipv4InterfaceContainer n0r2Ip = r2addr.Assign(n0r2);
        LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "rule add from 10.2.0.1/24 table 2");
        LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "route add 10.2.0.0/24 dev sim0 scope link table 2");
        LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "route add default via 10.2.0.2 dev sim0 table 2");

        // setting default route
        LinuxStackHelper::RunIp(nodes.Get(0), Seconds(0.1), "route add default via 10.2.0.2 dev sim1");

        // connecting N1 <-> R1
        NetDeviceContainer n1r1 = pointToPoint.Install(nodes.Get(1), routers.Get(1));
        Ipv4InterfaceContainer n1r1Ip = r1addr.Assign(n1r1);
        LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "rule add from 10.1.0.1/24 table 1");
        LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "route add 10.1.0.0/24 dev sim0 scope link table 1");
        LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "route add default via 10.1.0.2 dev sim0 table 1");

        // connecting N1 <-> R3
        NetDeviceContainer n1r3 = pointToPoint.Install(nodes.Get(1), routers.Get(3));
        Ipv4InterfaceContainer n1r3Ip = r3addr.Assign(n1r3);
        LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "rule add from 10.3.0.1/24 table 2");
        LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "route add 10.3.0.0/24 dev sim0 scope link table 2");
        LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "route add default via 10.3.0.2 dev sim0 table 2");

        // setting default route
        LinuxStackHelper::RunIp(nodes.Get(1), Seconds(0.1), "route add default via 10.3.0.2 dev sim1");

        // SETTING UP TRAFFIC NODES

        pointToPoint.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
        pointToPoint.SetChannelAttribute("Delay", StringValue("1ns"));

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

        DceApplicationHelper dce;
        ApplicationContainer apps;

        dce.SetStackSize(1 << 20);

        // MPTCP APPS

        // Launch iperf client on node 0 - MPTCP traffic starts at 5 - 35 sec
        dce.SetBinary("iperf");
        dce.ResetArguments();
        dce.ResetEnvironment();
        dce.ParseArguments("-c 10.3.0.1 -i 1 --time 60");
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
        dce.ParseArguments("-u -c 10.1.0.3 -b 2M --time 20");
        apps = dce.Install(traffic.Get(0));
        apps.Start(Seconds(15.0));
        apps.Stop(Seconds(40));

        dce.SetBinary("iperf");
        dce.ResetArguments();
        dce.ResetEnvironment();
        dce.ParseArguments("-u -c 10.3.0.3 -b 2M --time 20");
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
