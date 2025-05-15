# ft_ping

A reimplementation of the classic `ping` utility in C, designed to closely mimic the behavior of the original system tool. This program sends ICMP Echo Request packets to a specified host and listens for Echo Replies to measure round-trip time and reachability.

## 🚀 Project Overview

`ft_ping` is a networking diagnostic tool that allows you to test the reachability of a host on an IP network. It operates using raw sockets and the ICMP protocol. This implementation manually constructs, sends, and receives ICMP packets and handles key functionalities such as:

- DNS resolution
- Round-trip time calculation
- Signal handling
- Non-blocking socket reads
- Display of statistics and errors

## ✅ Supported Features

- ICMP Echo Request/Reply handling
- TTL (Time-To-Live) management
- Real-time RTT measurements
- UNIX timestamp output
- Optional hostname or IP-only display
- Graceful termination with Ctrl+C
- Handling of ICMP error types (e.g., Time Exceeded)

## 🧩 Usage

```
./ft_ping [OPTIONS] HOST
```

## Example

```
./ft_ping google.com
```

## Output Example

```
PING google.com (142.250.178.142): 56 data bytes
64 bytes from par21s22-in-f14.1e100.net (142.250.178.142): icmp_seq=0 ttl=254 time=5.798 ms
64 bytes from par21s22-in-f14.1e100.net (142.250.178.142): icmp_seq=1 ttl=254 time=5.877 ms
--- google.com ping statistics ---
2 packets transmitted, 2 packets received, 0% packet loss
round-trip min/avg/max/stddev = 5.798/5.837/5.877/0.040 ms
```

## 🧵 Options

Usage: ft_ping [OPTION...] HOST ...
Send ICMP ECHO_REQUEST packets to network hosts.

    Options:
        <HOST>                DNS name or IPv4 address
        -?                    Show help
        -c <count>            Stop after <count> replies
        -D                    Print timestamp (UNIX format)
        -i <interval>         Seconds between each packet
        -h                    Show help
        -q                    Quiet output (summary only)
        -t <ttl>              Set time-to-live value
        -v                    Verbose output

## 🛑 Known Limitations

1. Only supports IPv4.

2. Requires root privileges (due to raw sockets).

3. Does not accept full URLs (https://domain.com/) — only hostnames or IPs are valid.

## 🔧 Build

To compile ft_ping, use the provided Makefile:

    make

Run with:

    sudo ./ft_ping google.com

🧠 Learning Focus

This project was created for educational purposes. It involves:

- Low-level network programming

- Use of raw sockets

- Manual parsing of IP and ICMP headers

- Signal and timeout management

- Performance statistics collection