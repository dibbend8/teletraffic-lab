#!/usr/bin/env python3
"""Simple client that opens sequential socket connections to a server.

This is intentionally easier to read than a parallel/threaded client.
"""

import argparse
import socket


def send_one_message(client_id: int, host: str, port: int, message: str):
    with socket.create_connection((host, port), timeout=5) as sock:
        sock.sendall(message.encode("utf-8"))
        reply = sock.recv(1024)
        print(f"[client {client_id}] sent: {message}")
        print(f"[client {client_id}] received: {reply.decode('utf-8', errors='replace')}")


def run_clients(host: str, port: int, clients: int, messages: int):
    for client_id in range(1, clients + 1):
        for message_index in range(messages):
            payload = f"client-{client_id}:msg-{message_index}"
            send_one_message(client_id, host, port, payload)

    print("[client] all clients finished")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Sequential socket client for the teaching demo")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=9001)
    parser.add_argument("--clients", type=int, default=3)
    parser.add_argument("--messages", type=int, default=2)
    args = parser.parse_args()

    run_clients(args.host, args.port, args.clients, args.messages)
