# Event-driven models only

This folder contains examples that do not use sockets, IP, nodes, or packet transmission. These are pure event-driven simulations for teaching:

- state transitions
- queueing dynamics
- Markov chains
- random event scheduling
- stochastic processes without packet-level networking

## Included examples

- `binary-symmetric-channel.cc` — a 2-state Markov/BSC model driven by scheduled state transitions
- `markov-demo.cc` — a discrete-time Markov chain example with a target state and state sequence output
- `mm1-queue.cc` — an M/M/1 queue model driven by arrival and service events

## Why this folder exists

This is the simulation layer below real network traffic.

These examples model what happens when:

- a timer expires
- a state changes
- an arrival occurs
- a service completes
- the next event is scheduled

This is a good way to understand how `Simulator::Schedule()` and callbacks work before adding sockets and packet transmission.

## Run them in ns-3

Copy the example into the ns-3 scratch directory, then run it from the ns-3 repo root.

```bash
cp labs/events/binary-symmetric-channel.cc deps/ns-3-dev/scratch/
cd deps/ns-3-dev
python3.11 ./ns3 build
python3.11 ./ns3 run "scratch/binary-symmetric-channel --p=0.2 --steps=10"
```

For the queue:

```bash
cp labs/events/mm1-queue.cc deps/ns-3-dev/scratch/
cd deps/ns-3-dev
python3.11 ./ns3 build
python3.11 ./ns3 run "scratch/mm1-queue --lambda=2.0 --mu=3.0 --events=10"
```

For the Markov demo:

```bash
cp labs/events/markov-demo.cc deps/ns-3-dev/scratch/
cd deps/ns-3-dev
python3.11 ./ns3 build
python3.11 ./ns3 run "scratch/markov-demo --chainType=three_state --steps=10000 --stepInterval=0.001 --outPrefix=markov-three"
```

## Conceptual distinction

- Event-driven model: state + time + scheduled transitions only
- Packet-level model: sockets + packets + links + IP + callbacks + pcap capture

The packet-based demos are in the packet-simulation lab, not here.
