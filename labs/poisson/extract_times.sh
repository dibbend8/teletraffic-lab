#!/usr/bin/env bash
# extract_times.sh
# Usage: ./extract_times.sh input.pcap output_times.txt

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 input.pcap output_times.txt"
  exit 1
fi

PCAP=$1
OUT=$2

# Use tshark to extract epoch timestamps (frame.time_epoch)
# Requires: sudo apt install tshark

tshark -r "$PCAP" -T fields -e frame.time_epoch | sed '/^$/d' > "$OUT"

echo "Wrote timestamps to $OUT"