# Lab: ns-3 Poisson‑arrival Traffic

This folder contains the student lab materials for the Poisson‑arrival traffic exercise using ns‑3. It is designed for Windows students using WSL2/Ubuntu and VS Code (Remote‑WSL). The materials include:

- README (this file) — step‑by‑step installation, annotated walkthrough, lab tasks, analysis recipe
- `analyze_pcap_interarrival.py` — Python script to analyze interarrival times extracted from pcaps
- `extract_times.sh` — small helper to extract timestamps from a pcap using tshark
- `.vscode/` — VS Code task and launch configurations for the lab (useful when opening the lab folder directly in VS Code)

Link to example used in the lab: [poisson-demo.cc](../../deps/ns-3-dev/scratch/poisson-demo.cc)

---

Last updated: 2026-08-17

## Quick start (paste these commands into WSL/Ubuntu)

# Update + install core packages
sudo apt update && sudo apt upgrade -y
sudo apt install -y build-essential git python3 python3-dev python3-pip cmake pkg-config \
  libsqlite3-dev libxml2-dev libssl-dev libboost-all-dev qtbase5-dev qttools5-dev-tools \
  python3-venv tshark

# Clone (if students don't already have the repo)
cd ~
# Use your class repo or the upstream ns-3 dev tree
git clone https://gitlab.com/nsnam/ns-3-dev.git ns-3-dev
cd ns-3-dev

# Configure & build (ns-3 uses ./ns3, not ./waf)
./ns3 configure --enable-examples --enable-tests
./ns3 build

# Run the provided scratch example (produces pcap and xml files)
./ns3 run scratch/poisson-demo

---

## Annotated walkthrough (important parts to inspect in poisson-demo.cc)

Open: `deps/ns-3-dev/scratch/poisson-demo.cc`

- PoissonSender::Setup(Address peerAddress, uint32_t packetSize, double meanRateHz)
  - Uses `ExponentialRandomVariable` with `Mean = 1.0 / meanRateHz` — this produces exponential interarrival times (Poisson process arrivals).
- ScheduleNextSend/SendPacket
  - Each send schedules the next send by sampling the exponential RV and calling `Simulator::Schedule`.
- Receiver uses socket callback and schedules processing events on arrivals.
- `p2p.EnablePcapAll("poisson-demo")` writes `.pcap` files (open with Wireshark).
- `AnimationInterface anim("poisson-demo.xml")` writes NetAnim trace for visualization.

Key parameters to vary:
- `meanRateHz` (packets/sec) — third argument to `senderApp->Setup(..., 5.0)`
- Simulation `StopTime` and start times — increase duration for better statistics
- `packetSize` (bytes)

---

## Lab tasks (deliverables go into `lab-poisson/<your-id>/`)

See the full lab handout in the class README for details. Short version:

1. Build & run the example; screenshot Wireshark and NetAnim outputs.
2. Run simulations for three rates (e.g., 1, 5, 20 pkt/s) for 120s each; collect pcaps.
3. Extract arrival timestamps and compute interarrival times; produce histogram + ECDF.
4. Perform KS test against exponential fit and report p-values.
5. Compare to a short real Internet pcap (download or capture) and comment on differences.
6. Submit: scripts, plots, times.txt, report (Markdown/PDF), and screenshots.

---

## Tools & scripts in this folder

- `analyze_pcap_interarrival.py` — Python analysis script (see usage below)
- `extract_times.sh` — run `./extract_times.sh input.pcap output_times.txt`
- `.vscode/tasks.json` & `.vscode/launch.json` — task/launch for easy build/run within VS Code when opening this folder

---

## Python analysis usage

1. Extract timestamps from a pcap:

```bash
./extract_times.sh poisson-demo-1-0.pcap times.txt
```

2. Run the analysis script:

```bash
python3 analyze_pcap_interarrival.py times.txt poisson-rate-5s-120s
```

This writes `poisson-rate-5s-120s_hist.png` and `_ecdf.png` and prints KS test results.

---

## VS Code: open the `lab-poisson` folder directly in Remote‑WSL to use the provided tasks/launch configs

Open WSL terminal and run:

```bash
cd ~/ns-3-dev/lab-poisson
code .
```

Then use Terminal → Run Task → `waf: build` or the `Run Poisson Demo` task.

---

## New: Markov chain simulations

A new subfolder `markov/` accompanies this lab. It contains a simple ns‑3-based discrete-time Markov chain simulator and an analysis script to demonstrate:

- empirical (long-run) state distribution (stationary distribution)
- hitting time / first-passage time statistics to a target state
- absorption times for absorbing chains

Files under `lab-poisson/markov/`:
- `markov-demo.cc` — ns‑3 scratch program that simulates discrete-time Markov chains using an event scheduled at a fixed step interval. It records the visited state sequence and per-run hitting/absorption times to output files.
- `analyze_markov.py` — Python script to compute empirical stationary distribution, plot bar charts, and analyze hitting/absorption time histograms.
- `README-markov.md` — instructions and tasks for the Markov exercises.

How to run (from the lab folder in WSL):

```bash
# build ns-3 if not already built
cd ~/ns-3-dev
./waf build -j$(nproc)

# run the Markov demo (example):
./waf --run "scratch/markov/markov-demo --chainType=three_state --steps=10000 --stepInterval=0.001 --outPrefix=markov-three"

# extract outputs are written as text files (state sequence and hitting times). Analyze with:
python3 markov/analyze_markov.py markov-three_seq.txt markov-three
```

See `lab-poisson/markov/README-markov.md` for detailed tasks and examples.

If you want me to also create a ZIP of this `lab-poisson` folder for distribution, say so and I will add a zip under `lab-poisson/`.
