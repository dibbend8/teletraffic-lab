#!/usr/bin/env python3
"""Simple threaded TCP server for teaching socket basics.

This script listens for incoming connections, handles each client in a separate
thread, and echoes back a message. It is intentionally simple and readable.
"""

import argparse
import socket
import threading


def handle_client(conn, addr):
    print(f"[server] connected: {addr}")
    try:
        while True:
            data = conn.recv(1024)
            if not data:
                break
            message = data.decode("utf-8", errors="replace").strip()
            print(f"[server] received from {addr}: {message}")
            reply = f"ACK: {message}"
            conn.sendall(reply.encode("utf-8"))
    except Exception as exc:
        print(f"[server] error with {addr}: {exc}")
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
            thread = threading.Thread(target=handle_client, args=(conn, addr), daemon=True)
            thread.start()
    except KeyboardInterrupt:
        print("\n[server] shutting down")
    finally:
        server.close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Simple threaded socket server")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=9001)
    args = parser.parse_args()

    run_server(args.host, args.port)
