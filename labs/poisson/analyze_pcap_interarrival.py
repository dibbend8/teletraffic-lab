#!/usr/bin/env python3
"""
analyze_pcap_interarrival.py

Usage: python3 analyze_pcap_interarrival.py times.txt outprefix

Takes a newline-separated file of epoch timestamps (seconds) extracted from a pcap
and computes interarrival statistics, plots histogram+fit and ECDF, and runs a KS test
against an exponential distribution fitted by the sample mean.
"""

import sys
import numpy as np
import matplotlib.pyplot as plt
from scipy import stats


def load_times(path):
    with open(path) as f:
        times = [float(line.strip()) for line in f if line.strip()]
    return np.array(times)


def analyze(times, outprefix, configured_rate=None):
    if len(times) < 2:
        raise SystemExit("Need at least 2 timestamps to compute interarrival times")

    dt = np.diff(times)
    mean_dt = dt.mean()
    std_dt = dt.std(ddof=1)
    cv = std_dt / mean_dt
    print(f"n_packets={len(times)}, mean_dt={mean_dt:.6f}s, std_dt={std_dt:.6f}s, CV={cv:.3f}")

    # Fit exponential: scale parameter = mean
    loc, scale = 0.0, mean_dt
    print("Fitted exponential scale (mean):", scale)

    # KS test
    ks_stat, p_value = stats.kstest(dt, 'expon', args=(loc, scale))
    print(f"KS statistic={ks_stat:.4f}, p-value={p_value:.4f}")

    # Plot histogram & fitted PDF
    plt.figure(figsize=(6,4))
    bins = np.linspace(0, np.percentile(dt, 99), 60)
    plt.hist(dt, bins=bins, density=True, alpha=0.6, label='empirical')
    xs = np.linspace(0, bins[-1], 200)
    pdf = (1/scale) * np.exp(-xs/scale)
    plt.plot(xs, pdf, 'r-', lw=2, label=f'expon(scale={scale:.3f}s)')
    plt.xlabel('interarrival time (s)')
    plt.ylabel('density')
    plt.legend()
    plt.title(f'Interarrival times (n={len(dt)}), CV={cv:.2f}')
    plt.tight_layout()
    plt.savefig(outprefix + "_hist.png", dpi=150)
    plt.close()

    # ECDF plot
    sorted_dt = np.sort(dt)
    ecdf = np.arange(1, len(sorted_dt) + 1) / len(sorted_dt)
    plt.figure(figsize=(6,4))
    plt.step(sorted_dt, ecdf, label='empirical ECDF')
    plt.plot(xs, 1 - np.exp(-xs/scale), 'r--', label='expon CDF')
    plt.xlabel('interarrival time (s)')
    plt.ylabel('ECDF')
    plt.legend()
    plt.title('ECDF vs Exponential CDF')
    plt.tight_layout()
    plt.savefig(outprefix + "_ecdf.png", dpi=150)
    plt.close()

    return dict(n=len(times), mean_dt=mean_dt, std_dt=std_dt, cv=cv, ks_stat=ks_stat, p_value=p_value)


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: analyze_pcap_interarrival.py times.txt outprefix")
        sys.exit(1)
    times = load_times(sys.argv[1])
    r = analyze(times, sys.argv[2])
    print(r)
