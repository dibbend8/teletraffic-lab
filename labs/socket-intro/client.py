#!/usr/bin/env python3
"""Simple client that opens multiple socket connections to a server.

This is meant to illustrate how multiple clients can send messages in parallel.
"""

import argparse
import socket
import threading


def send_messages(client_id: int, host: str, port: int, messages: int):
    with socket.create_connection((host, port), timeout=5) as sock:
        for i in range(messages):
            payload = f"client-{client_id}:msg-{i}"
            sock.sendall(payload.encode("utf-8"))
            reply = sock.recv(1024)
            print(f"[client {client_id}] sent: {payload}")
            print(f"[client {client_id}] received: {reply.decode('utf-8', errors='replace')}")


def run_clients(host: str, port: int, clients: int, messages: int):
    threads = []
    for i in range(clients):
        t = threading.Thread(target=send_messages, args=(i + 1, host, port, messages), daemon=True)
        threads.append(t)
        t.start()

    for t in threads:
        t.join()

    print("[client] all clients finished")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Socket client for the teaching demo")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=9001)
    parser.add_argument("--clients", type=int, default=3)
    parser.add_argument("--messages", type=int, default=2)
    args = parser.parse_args()

    run_clients(args.host, args.port, args.clients, args.messages)
