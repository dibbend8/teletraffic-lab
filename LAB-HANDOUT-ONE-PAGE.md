# Teletraffic Lab — One-Page Student Handout

## Goal

Study packet-level traffic generation and stochastic processes using ns-3:
- Poisson arrivals
- Markov-chain state evolution
- first-passage / hitting times
- absorption times and empirical stationary distributions

## Environment

Use WSL2 + Ubuntu on Windows. Keep the project in your home directory, for example:

```bash
~/teletraffic-lab
```

## Setup

```bash
sudo apt update
sudo apt install -y build-essential git python3 python3-dev python3-pip cmake pkg-config \
  libsqlite3-dev libxml2-dev libssl-dev libboost-all-dev qtbase5-dev qttools5-dev-tools \
  python3-venv tshark

sudo apt install -y python3.11 python3.11-dev python3.11-venv

git clone https://github.com/dibbend8/teletraffic-lab.git
cd teletraffic-lab
git submodule update --init --recursive
cd deps/ns-3-dev
python3.11 ./ns3 configure --enable-examples --enable-tests
python3.11 ./ns3 build
```

Open in VS Code on Windows using Remote - WSL:

```bash
cd ~/teletraffic-lab
code .
```

## First run

```bash
cd ~/teletraffic-lab/deps/ns-3-dev
python3.11 ./ns3 run scratch/poisson-demo
```

What to look for:
- packet timestamps in pcap files
- packet arrivals
- sender/receiver logs
- NetAnim visualization

## Markov experiment

```bash
cd ~/teletraffic-lab/deps/ns-3-dev
python3.11 ./ns3 run "scratch/markov/markov-demo --chainType=three_state --steps=10000 --stepInterval=0.001 --outPrefix=markov-three --run=1"
```

Analyze the result:

```bash
python3 ~/teletraffic-lab/labs/poisson/markov/analyze_markov.py markov-three_seq.txt markov-three
```

## Questions to answer

1. Does the empirical state distribution match the stationary distribution?
2. How does the hitting time change when transition probabilities change?
3. In an absorbing chain, what is the relationship between absorption time and probability of staying in transient states?
4. Is Poisson traffic a reasonable model for real Internet traffic? Why or why not?
5. Use Wireshark to inspect packet timing and compare to your numerical summary.

## Deliverables

- short summary of simulation settings
- plots for Poisson and Markov experiments
- interpretation of empirical stationary distributions / hitting times
- brief comparison to real traces (if used)
- screenshots from Wireshark and/or NetAnim

## Final reminder

Keep the repo in WSL under:

```bash
~/teletraffic-lab
```

Not under `/mnt/c/...`.
