#!/usr/bin/env python3
"""
analyze_markov.py

Usage:
  python3 analyze_markov.py seq_file outprefix

seq_file format: whitespace-separated lines: <time> <state>

Produces:
 - outprefix_stationary.png  (bar chart of empirical distribution)
 - outprefix_hitting_hist.png (if hitting times file present)

"""
import sys
import numpy as np
import matplotlib.pyplot as plt
import os


def load_seq(path):
    times = []
    states = []
    with open(path) as f:
        for line in f:
            parts = line.split()
            if len(parts) >= 2:
                times.append(float(parts[0]))
                states.append(int(parts[1]))
    return np.array(times), np.array(states)

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print('Usage: analyze_markov.py seq_file outprefix')
        sys.exit(1)
    seq_file = sys.argv[1]
    outprefix = sys.argv[2]
    times, states = load_seq(seq_file)
    unique, counts = np.unique(states, return_counts=True)
    probs = counts / counts.sum()
    plt.figure(figsize=(6,4))
    plt.bar(unique.astype(str), probs)
    plt.xlabel('state')
    plt.ylabel('empirical probability')
    plt.title('Empirical stationary distribution (time-averaged)')
    plt.tight_layout()
    outpng = outprefix + '_stationary.png'
    plt.savefig(outpng, dpi=150)
    print('Wrote', outpng)

    # check for hitting file
    hitfile = outprefix + '_hitting.txt'
    if os.path.exists(hitfile):
        data = np.loadtxt(hitfile)
        plt.figure(figsize=(6,4))
        plt.hist(data, bins=40, alpha=0.7)
        plt.xlabel('hitting time (steps)')
        plt.ylabel('count')
        plt.title('Hitting time distribution')
        plt.tight_layout()
        outpng2 = outprefix + '_hitting_hist.png'
        plt.savefig(outpng2, dpi=150)
        print('Wrote', outpng2)
    else:
        print('No hitting file found at', hitfile)
