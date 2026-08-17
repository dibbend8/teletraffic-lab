# Lab: ns-3 Poisson‑arrival Traffic

This folder contains the student lab materials for the Poisson‑arrival traffic exercise in ns‑3. The goal is not just to run a script, but to understand what is happening under the hood: how packet generation is scheduled, how a socket receives data, how the ns‑3 event loop advances simulation time, and how a Poisson process is represented in code.

This file is intentionally written as a beginner-friendly walkthrough. If you are new to ns‑3, treat it as a map: every line in this program is connected to a standard ns‑3 pattern.

## Files in this lab

- `poisson-demo.cc` — the main example program that creates a sender and a receiver
- `analyze_pcap_interarrival.py` — script to analyze packet arrival times from the PCAP
- `extract_times.sh` — helper to dump timestamps from a pcap using tshark
- `markov/` — optional follow-up Markov chain experiments

## Local run from VS Code (WSL2 recommended)

1. Open a WSL2 Ubuntu terminal.
2. Clone or open the repo.
3. Make sure the example is copied into the ns-3 scratch directory before building:

```bash
cd ~/teletraffic-lab
cp labs/poisson/poisson-demo.cc deps/ns-3-dev/scratch/poisson-demo.cc
cd deps/ns-3-dev
python3.11 ./ns3 configure --enable-examples --enable-tests
python3.11 ./ns3 build
python3.11 ./ns3 run scratch/poisson-demo
```

4. Open the repo in VS Code using Remote - WSL:

```bash
cd ~/teletraffic-lab
code .
```

5. In VS Code, use the integrated terminal or the provided tasks to build/run again.

The important part is this: the example must live in the ns‑3 `scratch/` directory before `./ns3 run` can see it.

---

## What this example is doing at a high level

This program creates a tiny network with two nodes:
- node 0 = sender
- node 1 = receiver

Then it:
- installs a point-to-point link between them
- assigns IP addresses
- enables PCAP capture
- creates a custom `Application` for the sender
- creates a custom `Application` for the receiver
- schedules packet transmissions according to an exponential random delay
- records output and packet traces

The sender is not sending packets at a fixed interval. Instead, it creates a Poisson process by drawing random inter‑arrival times from an exponential distribution.

That is the key idea: Poisson arrivals = exponential gaps.

---

## The important pieces in the C++ file

### 1) `using namespace ns3;`

This means the code is using the ns‑3 names directly, so you can write `NodeContainer`, `Application`, `Socket`, `Ptr<T>`, `Simulator`, and so on without prefixing everything with `ns3::`.

### 2) `Ptr<T>`

`Ptr<T>` is one of the most important ns‑3 concepts. It is a smart pointer with reference counting.

It is used like this:

```cpp
Ptr<Socket> m_socket;
Ptr<Packet> packet = Create<Packet>(m_packetSize);
```

Why is this important?
- ns‑3 objects are allocated dynamically
- they can be shared among multiple components
- `Ptr<T>` automatically manages object lifetime
- it prevents common raw-pointer bugs

In plain English: `Ptr<T>` is a safe handle to an ns‑3 object.

### 3) `CreateObject<ExponentialRandomVariable>()`

This creates a random variable object. The code sets:

```cpp
m_interArrival->SetAttribute("Mean", DoubleValue(1.0 / meanRateHz));
```

The mean of an exponential distribution is `1 / lambda`, where `lambda` is the arrival rate in packets per second.

So if `meanRateHz = 5.0`, then the mean gap is:

```text
1 / 5 = 0.2 seconds
```

This means the sender waits about 0.2s on average between packets, which is exactly the behavior of a Poisson process.

### 4) `Simulator::Schedule(...)`

This is the heart of ns‑3 simulation.

```cpp
m_sendEvent = Simulator::Schedule(next, &PoissonSender::SendPacket, this);
```

This means:
- wait for `next` seconds
- then call `PoissonSender::SendPacket()`
- on this object (`this`)

ns‑3 does not run code with normal wall-clock time. Instead, it advances a virtual simulation clock. Every event is scheduled in the simulator queue.

### 5) `Socket::CreateSocket(...)` and `Connect(...)`

The sender creates a UDP socket and connects it to the receiver address:

```cpp
m_socket = Socket::CreateSocket(GetNode(), UdpSocketFactory::GetTypeId());
m_socket->Connect(m_peerAddress);
```

This creates a socket on the current node and connects it to the remote endpoint.

### 6) `ReceiveCallback` / `SetRecvCallback`

The receiver sets a callback:

```cpp
m_socket->SetRecvCallback(MakeCallback(&PacketReceiver::HandleRead, this));
```

This means: when a packet arrives, call `HandleRead()` automatically. There is no polling loop in the main program. This is the ns‑3 equivalent of an event-driven callback.

### 7) `Packet` and `PacketSize`

```cpp
Ptr<Packet> packet = Create<Packet>(m_packetSize);
m_socket->Send(packet);
```

A packet is a container of bytes. The sender sends a UDP payload of size `m_packetSize` (in the example, 1024 bytes).

### 8) `Application` base class

Both sender and receiver inherit from `Application`.

That means they can be plugged into a node and controlled by timing methods:

```cpp
receiverApp->SetStartTime(Seconds(0.0));
receiverApp->SetStopTime(Seconds(10.0));
```

`StartApplication()` is called when the simulation starts and `StopApplication()` is called when it ends.

---

## Timeline: what happens when the program is run

### Step 1: build the network

In `main()`:

```cpp
NodeContainer nodes;
nodes.Create(2);
```

This creates two network nodes.

Then a point-to-point link is installed:

```cpp
PointToPointHelper p2p;
p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
p2p.SetChannelAttribute("Delay", StringValue("2ms"));
NetDeviceContainer devices = p2p.Install(nodes);
```

This creates the network link connecting the nodes.

Then the internet stack and IP addresses are installed:

```cpp
InternetStackHelper stack;
stack.Install(nodes);

Ipv4AddressHelper address;
address.SetBase("10.1.1.0", "255.255.255.0");
Ipv4InterfaceContainer interfaces = address.Assign(devices);
```

### Step 2: set up packet capture and animation

```cpp
p2p.EnablePcapAll("poisson-demo");
AnimationInterface anim("poisson-demo.xml");
```

This tells ns‑3 to write a PCAP trace and a NetAnim XML file.

### Step 3: create the receiver application

```cpp
Ptr<PacketReceiver> receiverApp = CreateObject<PacketReceiver>();
receiverApp->Setup(port);
nodes.Get(1)->AddApplication(receiverApp);
receiverApp->SetStartTime(Seconds(0.0));
receiverApp->SetStopTime(Seconds(10.0));
```

The receiver is attached to node 1 and binds to the UDP port. It then waits for packets.

### Step 4: create the sender application

```cpp
Ptr<PoissonSender> senderApp = CreateObject<PoissonSender>();
senderApp->Setup(InetSocketAddress(interfaces.GetAddress(1), port), 1024, 5.0);
nodes.Get(0)->AddApplication(senderApp);
senderApp->SetStartTime(Seconds(1.0));
senderApp->SetStopTime(Seconds(10.0));
```

This tells the sender:
- send to the receiver's IP address
- use UDP
- send 1024-byte packets
- use Poisson rate 5 packets/sec

### Step 5: the sender starts

When the simulation starts, `PoissonSender::StartApplication()` runs:

```cpp
m_socket = Socket::CreateSocket(GetNode(), UdpSocketFactory::GetTypeId());
m_socket->Connect(m_peerAddress);
ScheduleNextSend();
```

Then `ScheduleNextSend()` chooses the next delay from an exponential distribution:

```cpp
Time next = Seconds(m_interArrival->GetValue());
Simulator::Schedule(next, &PoissonSender::SendPacket, this);
```

So the first packet may be sent at 1.0s + 0.13s, or 1.0s + 0.7s, or any random delay. That is the Poisson process.

### Step 6: the sender sends a packet

When the scheduled event fires, `SendPacket()` runs:

```cpp
Ptr<Packet> packet = Create<Packet>(m_packetSize);
m_socket->Send(packet);
```

Then it schedules the next transmission:

```cpp
ScheduleNextSend();
```

That means the sender keeps sending packets forever until the stop time, with each gap drawn independently from an exponential random variable.

### Step 7: the receiver reacts

On the other side, the listener has a receive callback:

```cpp
m_socket->SetRecvCallback(MakeCallback(&PacketReceiver::HandleRead, this));
```

When a packet arrives, the callback runs:

```cpp
while ((packet = socket->RecvFrom(from)))
```

This loop reads every packet available at the socket.

Then the code does:

```cpp
Simulator::Schedule(MilliSeconds(1), &PacketReceiver::ProcessPacket, this, packet);
```

This schedules a follow-up event 1 millisecond later to represent processing or queueing. In other words, the receiver does not instantly print and stop; it schedules a follow-up simulation event before continuing.

### Step 8: the event loop ends

Finally:

```cpp
Simulator::Stop(Seconds(10.5));
Simulator::Run();
Simulator::Destroy();
```

This starts the event loop, processes all scheduled events, then cleans up.

The whole system is a deterministic scheduler running over a virtual clock, not a normal real-time loop.

---

## Why this is Poisson

The sender does not pick a fixed period. Instead it uses an exponential random variable for the waiting time between transmissions.

If the gap between events is exponential, then the arrival process is Poisson.

In the code:

```cpp
m_interArrival = CreateObject<ExponentialRandomVariable>();
m_interArrival->SetAttribute("Mean", DoubleValue(1.0 / meanRateHz));
```

This is the standard mathematical representation of Poisson packet arrivals.

---

## Suggested lab questions for students

1. What is the effect of changing `5.0` to `1.0` or `20.0`?
2. What happens if you increase the simulation time? Why do the statistics become more stable?
3. What is the difference between a fixed-rate sender and a Poisson sender?
4. If you double the packet size, what changes in the pcap? If you increase the data rate, what changes in flow behavior?
5. Why do we need `Ptr<T>` rather than raw pointers in ns‑3?
6. Why is event scheduling central to ns‑3?

---

## Practical debugging tips

- If the example does not run, make sure it is copied into the ns-3 `scratch/` directory:

```bash
cp labs/poisson/poisson-demo.cc deps/ns-3-dev/scratch/poisson-demo.cc
```

- If `./ns3 run ...` cannot find the file, check the filename and the working directory.
- If the build fails, check that you are using Python 3.11 and that you are inside the ns-3 source directory.
- If the pcap is empty, make sure the sender and receiver are started in time and that the simulation time is long enough.

---

## Optional extension: compare to real traffic

After understanding the Poisson example, compare it with real or captured network traces. The main question is: is real traffic really Poisson-like?

In practice, real traffic often shows:
- bursts
- correlations between packet arrivals
- heavy tails and non-uniform inter-arrival behavior

This is exactly why visualization and statistical tests are useful in later labs.

---

## Summary

This program is a tiny but complete example of how ns‑3 models stochastic network behavior.

The main ideas are:
- `Application` objects represent behavior attached to nodes
- `Ptr<T>` manages object lifetimes safely
- `Simulator::Schedule` drives all time-based behavior
- `ExponentialRandomVariable` creates Poisson inter-arrivals
- sockets and callbacks model packet transmission and reception
- PCAP and animation let you inspect the network behavior visually

Once you understand those ideas, the rest of ns‑3 becomes much easier to read.
