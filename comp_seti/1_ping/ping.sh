#!/bin/bash

CSV_FILE="ping_results.csv"

echo "hostname, ip, average rtt, icmp_seq, ttl" > "$CSV_FILE"

for host in "$@"; do
    ping_out=$(ping -c 1 "$host")
    if [ $? -eq 0 ]; then
        ip=$(echo "$ping_out" | grep -oP 'PING \S+ \(\K[^)]+')
        rtt=$(echo "$ping_out" | grep -oP 'rtt min/avg/max/mdev\s*=\s*[\d.]+\/[\d.]+\/\K[\d.]+')
        icmp_seq=$(echo "$ping_out" | grep -oP 'icmp_seq=\K\d+')
        ttl=$(echo "$ping_out" | grep -oP 'ttl=\K\d+')
        echo "$host, $ip, $rtt, $icmp_seq, $ttl" >> "$CSV_FILE"
    fi
done
