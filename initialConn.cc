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

#include <fstream>
#include <iostream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "zlib.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("UdpEchoExample");

int 
main (int argc, char *argv[]) 
{

        CommandLine cmd;
        cmd.Parse (argc, argv);
  
        Time::SetResolution (Time::NS);

        LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
        //LogComponentEnable ("SimpleGlobalRoutingExample", LOG_LEVEL_INFO);
        //LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

        //NS_LOG_INFO ("Create nodes.");
        //Creating nodes
        NodeContainer hosts;
        //NodeContainer routers;

        hosts.Create(4);
        //routers.Create(2);

        InternetStackHelper stack;
        stack.Install(hosts);
        //stack1.Install(routers);
        //stack.Install(n3);

        PointToPointHelper p2p;

        Ipv4AddressHelper address;

        NodeContainer n1;
        n1.Add(hosts.Get(0));
        n1.Add(hosts.Get(1));

        NodeContainer n2;
        n2.Add(hosts.Get(1));
        n2.Add(hosts.Get(2));
 
        NodeContainer n3;
        n3.Add(hosts.Get(2));
        n3.Add(hosts.Get(3));

        //NS_LOG_INFO ("Create channels.");

        p2p.SetDeviceAttribute ("DataRate", StringValue ("8Mbps"));
        p2p.SetChannelAttribute ("Delay", StringValue ("100ms"));
        NetDeviceContainer subnet1Devices = p2p.Install(n1);
        NetDeviceContainer subnet2Devices = p2p.Install(n2);
        NetDeviceContainer subnet3Devices = p2p.Install(n3);

        Ptr<NetDevice> p = subnet1Devices.Get(1);

        //p->GetQueue() 
        //InternetStackHelper stack;
        //stack.Install(n1);
        //stack.Install(n2);
        //stack.Install(n3);
        //NS_LOG_INFO ("Assign IP Addresses.");

        address.SetBase("10.1.1.0","255.255.255.0");

        Ipv4InterfaceContainer subnet1Interfaces = address.Assign(subnet1Devices);

        address.SetBase("10.1.2.0","255.255.255.0");

        Ipv4InterfaceContainer subnet2Interfaces = address.Assign(subnet2Devices);

        address.SetBase("10.1.3.0","255.255.255.0");

        Ipv4InterfaceContainer subnet3Interfaces = address.Assign(subnet3Devices);

        //NS_LOG_INFO ("Create Applications.");

        Ipv4GlobalRoutingHelper::PopulateRoutingTables();

        UdpEchoServerHelper server(9);
        ApplicationContainer serverApp;
        serverApp = server.Install(hosts.Get(3));
        serverApp.Start(Seconds(1.0));
        serverApp.Stop(Seconds(10.0));

        uint32_t packetSize = 1100;
        uint32_t maxPacketCount = 10;
        Time interPacketInterval = Seconds (1.);
        UdpEchoClientHelper client(subnet3Interfaces.GetAddress(1),9);
        ApplicationContainer clientApp;
        client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client.SetAttribute ("PacketSize", UintegerValue (packetSize));
        clientApp=client.Install(hosts.Get(0)); 
        clientApp.Start(Seconds(2.0));
        clientApp.Stop(Seconds(9.0));       
                
        //NS_LOG_INFO ("Run Simulation.");
        Simulator::Run();
        Simulator::Destroy();
        //NS_LOG_INFO ("Done.");
        return 0;
}
