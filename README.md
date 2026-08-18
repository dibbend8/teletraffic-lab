# Teletraffic Lab

This repository contains the course materials for the ns-3 teletraffic/queueing lab and Markov chain experiments.

## Quick links

- Clone / checkout workflow: [CLONE-AND-SETUP.md](CLONE-AND-SETUP.md)
- One-page student handout: [LAB-HANDOUT-ONE-PAGE.md](LAB-HANDOUT-ONE-PAGE.md)
- WSL2 installation guide: [labs/poisson/STUDENT-SETUP-WSL2.md](labs/poisson/STUDENT-SETUP-WSL2.md)
- Poisson lab materials: [labs/poisson/README-lab-poisson.md](labs/poisson/README-lab-poisson.md)
- Markov chain lab: [labs/poisson/markov/README-markov.md](labs/poisson/markov/README-markov.md)
- Socket basics lab: [labs/socket-intro/README.md](labs/socket-intro/README.md)

## Repository structure

- `labs/poisson/` — Poisson traffic lab
- `labs/poisson/markov/` — Markov chain experiments
- `labs/socket-intro/` — Python socket tutorial for understanding client/server behavior
- `deps/ns-3-dev/` — upstream ns-3 source dependency

## Recommended student workflow

1. Install WSL2 + Ubuntu on Windows
2. Clone this repo
3. Run the setup steps in [CLONE-AND-SETUP.md](CLONE-AND-SETUP.md)
4. Build and run ns-3 from the `deps/ns-3-dev` directory
5. Use VS Code in Remote-WSL mode for editing, debugging, and Copilot

## Recommended environment

- Windows host: WSL2 + Ubuntu
- Python: 3.11
- Editor: VS Code with Remote - WSL and GitHub Copilot
- Build tool: `python3.11 ./ns3 build`

## Quick clone command

```bash
git clone https://github.com/dibbend8/teletraffic-lab.git
cd teletraffic-lab
```
