import socket
import subprocess
import csv
import re
import sys

DOMAINS = ["google.com", "yandex.ru", "github.com", "openai.com"]
OUTPUT_FILE = "dns_traceroute.csv"


def resolve_ip(domain):
    try:
        return socket.gethostbyname(domain)
    except socket.gaierror:
        return None


def run_traceroute(ip):
    try:
        cmd = ["traceroute", "-n", "-m", "12", "-w", "2", ip]
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=40)
        return result.stdout.strip().split("\n")
    except Exception as e:
        return [f"Ошибка выполнения: {e}"]


def parse_traceroute(lines):
    hops = []
    for line in lines[1:]:  # пропускаем первую строку с информацией о маршруте
        if not line.strip():
            continue
        match = re.match(r"\s*(\d+)\s+(\S+)\s+(.*)", line)
        if match:
            hop_num = match.group(1)
            hop_ip = match.group(2)
            rtt_raw = match.group(3)
            rtt_match = re.search(r"([\d.]+)\s*ms", rtt_raw)
            rtt = rtt_match.group(1) if rtt_match else "*"
            hops.append({"hop": hop_num, "ip": hop_ip, "rtt": rtt})
    return hops


def main():
    rows = []
    for domain in DOMAINS:
        ip = resolve_ip(domain)
        if not ip:
            print(f"[!] Не удалось разрешить домен: {domain}")
            rows.append(
                {
                    "domain": domain,
                    "ip": "N/A",
                    "hop": "N/A",
                    "hop_ip": "N/A",
                    "rtt_ms": "N/A",
                }
            )
            continue

        print(f"[+] {domain} -> {ip}")
        lines = run_traceroute(ip)
        hops = parse_traceroute(lines)

        if not hops:
            rows.append(
                {
                    "domain": domain,
                    "ip": ip,
                    "hop": "0",
                    "hop_ip": ip,
                    "rtt_ms": "target",
                }
            )
        else:
            for h in hops:
                rows.append(
                    {
                        "domain": domain,
                        "ip": ip,
                        "hop": h["hop"],
                        "hop_ip": h["ip"],
                        "rtt_ms": h["rtt"],
                    }
                )

    with open(OUTPUT_FILE, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(
            f, fieldnames=["domain", "ip", "hop", "hop_ip", "rtt_ms"]
        )
        writer.writeheader()
        writer.writerows(rows)

    print(f"\n[+] Результаты сохранены в {OUTPUT_FILE}")


if __name__ == "__main__":
    main()
