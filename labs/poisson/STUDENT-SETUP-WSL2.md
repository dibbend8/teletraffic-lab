# ns-3 + VS Code + Copilot Setup (WSL2 on Windows)

This guide is for students running ns-3 on a Windows PC.

We strongly recommend using WSL2 with Ubuntu instead of a native Windows install, because ns-3 is Linux-first and builds much more reliably in WSL.

## 1) Install WSL2 + Ubuntu

Open PowerShell as Administrator and run:

```powershell
wsl --install -d ubuntu
```

Then reboot if prompted.

Open the Ubuntu app from the Start menu and create your Linux user account.

## 2) Install required packages in Ubuntu

Run the following commands in the Ubuntu terminal:

```bash
sudo apt update
sudo apt install -y build-essential git python3 python3-dev python3-pip cmake pkg-config \
  libsqlite3-dev libxml2-dev libssl-dev libboost-all-dev qtbase5-dev qttools5-dev-tools \
  python3-venv tshark
```

Install a Python version that works well with ns-3 in this course:

```bash
sudo apt install -y python3.11 python3.11-dev python3.11-venv
```

## 3) Clone the course repository

```bash
cd ~
git clone <your-repo-url> ns-3-dev
cd ns-3-dev
```

## 4) Configure and build ns-3

Use Python 3.11 explicitly:

```bash
python3.11 ./ns3 configure --enable-examples --enable-tests
python3.11 ./ns3 build
```

This will build the ns-3 libraries and examples in the `build/` directory.

## 5) Run the provided Poisson demo

```bash
python3.11 ./ns3 run scratch/poisson-demo
```

This will create pcap files and a NetAnim trace XML file for analysis.

## 6) Run the Markov chain demo

```bash
python3.11 ./ns3 run "scratch/markov/markov-demo --chainType=three_state --steps=10000 --stepInterval=0.001 --outPrefix=markov-three --run=1"
```

This simulates a discrete-time Markov chain and writes:
- `markov-three_seq.txt`
- `markov-three_hitting.txt`

## 7) Analyze the Markov outputs

From the repo root, run:

```bash
python3 lab-poisson/markov/analyze_markov.py markov-three_seq.txt markov-three
```

This generates plots for the empirical stationary distribution and hitting-time histogram.

## 8) Install VS Code on Windows

Download and install VS Code from:

https://code.visualstudio.com/

Then install these extensions in VS Code:
- Remote - WSL
- C/C++
- GitHub Copilot

## 9) Open the repo in VS Code using WSL

In the Ubuntu terminal:

```bash
cd ~/ns-3-dev
code .
```

This opens the project in a Remote - WSL window.

You can now edit source code, build, run simulations, and use Copilot from within VS Code while still working inside the Linux environment.

## 10) Why WSL2 instead of native Windows?

ns-3 is designed for Linux-like systems. WSL2 gives students:
- reliable builds
- easier package installation
- better compatibility with ns-3 tooling
- smooth VS Code integration with Copilot

## 11) Common troubleshooting

If a build or run fails:

- make sure you are using `python3.11` instead of the system default Python
- make sure the repo is in your WSL home directory, e.g. `~/ns-3-dev`
- rebuild cleanly if needed:

```bash
python3.11 ./ns3 configure --enable-examples --enable-tests
python3.11 ./ns3 build
```

If `tshark` is missing or not found:

```bash
sudo apt install tshark
```

## 12) Recommended workflow for the course

Use this pattern throughout the course:

```bash
cd ~/ns-3-dev
python3.11 ./ns3 configure --enable-examples --enable-tests
python3.11 ./ns3 build
python3.11 ./ns3 run "scratch/markov/markov-demo --chainType=three_state --steps=10000 --stepInterval=0.001 --outPrefix=markov-three --run=1"
```

Then open the generated files in Wireshark / Python / VS Code as needed.

## 13) Final reminder

Keep the project under the WSL home directory, e.g.:

```bash
~/ns-3-dev
```

Do not work primarily from the Windows-mounted directory like:

```bash
/mnt/c/...
```

This is slower and can cause more build issues.
