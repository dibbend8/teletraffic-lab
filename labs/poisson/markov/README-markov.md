# Markov Chain Simulations — lab-poisson/markov

This folder contains a simple ns-3-based Markov chain simulator and analysis tools to explore:

- Empirical stationary (long-run) state distribution
- Hitting (first-passage) times to a target state
- Absorption time statistics for absorbing chains

Files:
- `markov-demo.cc` — ns-3 discrete-time Markov chain scratch program
- `analyze_markov.py` — Python analysis for sequences and hitting times

Quick run example (from ~/ns-3-dev):

# if you are using the copy in scratch/markov
./ns3 run "scratch/markov/markov-demo --chainType=three_state --steps=10000 --stepInterval=0.001 --outPrefix=markov-three --run=1"

This will produce `markov-three_seq.txt` and `markov-three_hitting.txt` in the current working directory. Analyze with:

python3 lab-poisson/markov/analyze_markov.py markov-three_seq.txt markov-three

See the script headers for more options.

Suggested exercises:
1. Compute empirical distribution for a symmetric 3-state chain and compare to analytic stationary distribution.
2. Create an absorbing chain with one absorbing state and measure absorption-time distribution for many runs.
3. Investigate mean hitting time vs transition probabilities and discuss intuition.

Notes:
- The simulator runs inside ns-3 but uses a simple event every `stepInterval` seconds to advance the discrete-time chain; it is primarily a teaching example to use ns-3 scheduling and RNGs for stochastic experiments.
