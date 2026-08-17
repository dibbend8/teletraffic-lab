//
// Minimal walkthrough example: Poisson packet arrivals from one node to another.
//
// Build/run:   ./ns3 run scratch/poisson-demo
//
// Two custom Application subclasses:
//   - PoissonSender:   draws exponential inter-arrival times (Poisson process)
//                       and schedules itself to send a UDP packet at each draw.
//   - PacketReceiver:  registers a socket receive callback and reacts to each
//                       arriving packet (the "handleMessage" equivalent).
//

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("PoissonDemo");

// ---------------------------------------------------------------------------
// Sender: generates Poisson-arrival traffic.
// ---------------------------------------------------------------------------
class PoissonSender : public Application
{
  public:
    static TypeId GetTypeId()
    {
        static TypeId tid =
            TypeId("PoissonSender")
                .SetParent<Application>()
                .AddConstructor<PoissonSender>();
        return tid;
    }

    // Configure destination, packet size, and mean arrival rate (packets/sec).
    void Setup(Address peerAddress, uint32_t packetSize, double meanRateHz)
    {
        m_peerAddress = peerAddress;
        m_packetSize = packetSize;
        // ExponentialRandomVariable's "Mean" is the mean of the distribution,
        // i.e. 1/rate -- this is what gives you Poisson-process arrivals.
        m_interArrival = CreateObject<ExponentialRandomVariable>();
        m_interArrival->SetAttribute("Mean", DoubleValue(1.0 / meanRateHz));
    }

  private:
    void StartApplication() override
    {
        m_socket = Socket::CreateSocket(GetNode(), UdpSocketFactory::GetTypeId());
        m_socket->Connect(m_peerAddress);

        // Schedule the very first send. This is the ns-3 equivalent of
        // scheduleAt(simTime() + draw, selfMsg) in OMNeT++: bind a method
        // and a delay, no message object required.
        ScheduleNextSend();
    }

    void StopApplication() override
    {
        Simulator::Cancel(m_sendEvent);
        if (m_socket)
        {
            m_socket->Close();
        }
    }

    void ScheduleNextSend()
    {
        Time next = Seconds(m_interArrival->GetValue());
        m_sendEvent = Simulator::Schedule(next, &PoissonSender::SendPacket, this);
    }

    void SendPacket()
    {
        Ptr<Packet> packet = Create<Packet>(m_packetSize);
        m_socket->Send(packet);
        NS_LOG_INFO("t=" << Simulator::Now().GetSeconds() << "s sender sent " << m_packetSize
                          << " bytes");

        // Chain the next event -- this is what keeps the Poisson process going.
        ScheduleNextSend();
    }

    Ptr<Socket> m_socket;
    Address m_peerAddress;
    uint32_t m_packetSize = 0;
    Ptr<ExponentialRandomVariable> m_interArrival;
    EventId m_sendEvent;
};

// ---------------------------------------------------------------------------
// Receiver: reacts to each arriving packet.
// ---------------------------------------------------------------------------
class PacketReceiver : public Application
{
  public:
    static TypeId GetTypeId()
    {
        static TypeId tid =
            TypeId("PacketReceiver")
                .SetParent<Application>()
                .AddConstructor<PacketReceiver>();
        return tid;
    }

    void Setup(uint16_t port)
    {
        m_port = port;
    }

  private:
    void StartApplication() override
    {
        m_socket = Socket::CreateSocket(GetNode(), UdpSocketFactory::GetTypeId());
        InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), m_port);
        m_socket->Bind(local);

        // This is the "handleMessage" equivalent for network arrivals: the
        // socket invokes HandleRead whenever a packet shows up, instead of
        // you polling or receiving via a shared dispatch method.
        m_socket->SetRecvCallback(MakeCallback(&PacketReceiver::HandleRead, this));
    }

    void StopApplication() override
    {
        if (m_socket)
        {
            m_socket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
            m_socket->Close();
        }
    }

    void HandleRead(Ptr<Socket> socket)
    {
        Ptr<Packet> packet;
        Address from;
        while ((packet = socket->RecvFrom(from)))
        {
            NS_LOG_INFO("t=" << Simulator::Now().GetSeconds() << "s receiver got "
                              << packet->GetSize() << " bytes from "
                              << InetSocketAddress::ConvertFrom(from).GetIpv4());

            // Event-based reaction to the message: here we just schedule a
            // follow-up "processing done" event 1ms later, to show how you'd
            // chain further simulated behaviour off of a received packet --
            // e.g. queueing, a reply, updating node state, etc.
            Simulator::Schedule(MilliSeconds(1), &PacketReceiver::ProcessPacket, this, packet);
        }
    }

    void ProcessPacket(Ptr<Packet> packet)
    {
        NS_LOG_INFO("t=" << Simulator::Now().GetSeconds() << "s finished processing packet of "
                          << packet->GetSize() << " bytes");
    }

    Ptr<Socket> m_socket;
    uint16_t m_port = 0;
};

// ---------------------------------------------------------------------------
int
main(int argc, char* argv[])
{
    LogComponentEnable("PoissonDemo", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(2);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    NetDeviceContainer devices = p2p.Install(nodes);

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // Dump every packet on this link to poisson-demo-0-0.pcap / -1-0.pcap so
    // you can open them in Wireshark and watch the Poisson arrivals visually.
    p2p.EnablePcapAll("poisson-demo");

    // NetAnim needs node positions to draw the topology. Place sender (0) on
    // the left and receiver (1) on the right.
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positions = CreateObject<ListPositionAllocator>();
    positions->Add(Vector(0.0, 0.0, 0.0));  // node 0: sender
    positions->Add(Vector(50.0, 0.0, 0.0)); // node 1: receiver
    mobility.SetPositionAllocator(positions);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    uint16_t port = 9;

    Ptr<PacketReceiver> receiverApp = CreateObject<PacketReceiver>();
    receiverApp->Setup(port);
    nodes.Get(1)->AddApplication(receiverApp);
    receiverApp->SetStartTime(Seconds(0.0));
    receiverApp->SetStopTime(Seconds(10.0));

    Ptr<PoissonSender> senderApp = CreateObject<PoissonSender>();
    senderApp->Setup(InetSocketAddress(interfaces.GetAddress(1), port),
                      1024,  // packet size (bytes)
                      5.0);  // mean rate: 5 packets/sec (Poisson)
    nodes.Get(0)->AddApplication(senderApp);
    senderApp->SetStartTime(Seconds(1.0));
    senderApp->SetStopTime(Seconds(10.0));

    // Write a NetAnim trace: open poisson-demo.xml in the NetAnim GUI to
    // watch packets fly from sender to receiver as the simulation runs.
    AnimationInterface anim("poisson-demo.xml");
    anim.SetConstantPosition(nodes.Get(0), 0.0, 0.0);
    anim.SetConstantPosition(nodes.Get(1), 50.0, 0.0);
    anim.UpdateNodeDescription(nodes.Get(0), "Sender (Poisson)");
    anim.UpdateNodeDescription(nodes.Get(1), "Receiver");
    anim.UpdateNodeColor(nodes.Get(0), 0, 200, 0);
    anim.UpdateNodeColor(nodes.Get(1), 0, 0, 200);
    anim.EnablePacketMetadata(true);

    Simulator::Stop(Seconds(10.5));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
