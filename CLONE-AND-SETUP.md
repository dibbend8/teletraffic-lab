# Clone and Setup Workflow

This document gives the exact steps to clone the course repo and set up the environment.

## 1) Clone the repo

```bash
git clone https://github.com/dibbend8/teletraffic-lab.git
cd teletraffic-lab
```

## 2) Initialize submodules

This repo includes the ns-3 dependency under `deps/ns-3-dev`.

```bash
git submodule update --init --recursive
```

## 3) Install WSL2 dependencies (Windows host)

Open PowerShell as Administrator and run:

```powershell
wsl --install -d ubuntu
```

Then open Ubuntu and install the required packages:

```bash
sudo apt update
sudo apt install -y build-essential git python3 python3-dev python3-pip cmake pkg-config \
  libsqlite3-dev libxml2-dev libssl-dev libboost-all-dev qtbase5-dev qttools5-dev-tools \
  python3-venv tshark

sudo apt install -y python3.11 python3.11-dev python3.11-venv
```

## 4) Open the repo in VS Code via WSL

From the Ubuntu terminal:

```bash
cd ~/teletraffic-lab
code .
```

Use the Remote - WSL extension in VS Code.

## 5) Configure and build ns-3

```bash
cd ~/teletraffic-lab/deps/ns-3-dev
python3.11 ./ns3 configure --enable-examples --enable-tests
python3.11 ./ns3 build
```

## 6) Run the lab examples

Poisson demo:

```bash
python3.11 ./ns3 run scratch/poisson-demo
```

Markov demo:

```bash
python3.11 ./ns3 run "scratch/markov/markov-demo --chainType=three_state --steps=10000 --stepInterval=0.001 --outPrefix=markov-three --run=1"
```

Analyze the output:

```bash
python3 /home/<your-user>/teletraffic-lab/labs/poisson/markov/analyze_markov.py markov-three_seq.txt markov-three
```

## 7) Troubleshooting

- Use Python 3.11 explicitly
- Keep the repo under the WSL home directory, e.g. `~/teletraffic-lab`
- Avoid working from `/mnt/c/...` for ns-3 builds
- If `tshark` is missing, install it with:

```bash
sudo apt install tshark
```
