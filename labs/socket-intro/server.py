#!/usr/bin/env python3
"""Simple sequential TCP server for teaching socket basics.

This version accepts one connection at a time and handles it before accepting the
next one. It is intentionally simple and easy to follow.
"""

import argparse
import socket


def handle_connection(conn, addr):
    print(f"[server] connected: {addr}")
    try:
        data = conn.recv(1024)
        if data:
            message = data.decode("utf-8", errors="replace").strip()
            print(f"[server] received from {addr}: {message}")
            reply = f"ACK: {message}"
            conn.sendall(reply.encode("utf-8"))
    finally:
        conn.close()
        print(f"[server] disconnected: {addr}")


def run_server(host: str, port: int):
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind((host, port))
    server.listen()

    print(f"[server] listening on {host}:{port}")

    try:
        while True:
            conn, addr = server.accept()
            handle_connection(conn, addr)
    except KeyboardInterrupt:
        print("\n[server] shutting down")
    finally:
        server.close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Simple sequential socket server")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=9001)
    args = parser.parse_args()

    run_server(args.host, args.port)
