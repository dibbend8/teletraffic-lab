# Socket basics for students

This folder contains a small Python socket example to help students understand the basic ideas behind sockets and callback-like behavior without needing to build a full ns-3 network simulation.

The goal is to show:

- a socket is an endpoint for communication
- a server listens on a port
- a client connects to that port
- the server handles one connection at a time in a simple sequential way
- the server reacts when a message arrives

This mirrors the way ns-3 uses sockets and callbacks, but it is much simpler and easier to inspect in plain Python.

## Files

- `server.py` — a simple sequential TCP server that handles one connection at a time
- `client.py` — a simple client that opens sequential connections and sends messages

## Run it

Open two terminals in WSL/Ubuntu.

Terminal 1:

```bash
cd ~/course-ns3/labs/socket-intro
python3 server.py --host 127.0.0.1 --port 9001
```

Terminal 2:

```bash
cd ~/course-ns3/labs/socket-intro
python3 client.py --host 127.0.0.1 --port 9001 --clients 3 --messages 2
```

You will see the server print incoming messages and the client print the replies.

## What this is teaching

### 1. Socket = communication endpoint

A socket is like a mailbox or port for a program. The server creates a listening socket and waits for incoming connections. The client creates a socket and connects to the server.

### 2. Bind, listen, accept, recv, send

The usual sequence is:

```python
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((host, port))
server_socket.listen()
conn, addr = server_socket.accept()
data = conn.recv(1024)
conn.sendall(response)
```

These are the same ideas used in ns-3 socket applications:

- bind to an IP:port
- wait for packets/messages
- receive them
- react to them

### 3. Sequential handling

This version accepts a connection, handles one message, closes it, and then waits for the next connection. That keeps the example easy to understand and easy to debug.

This is the simplest possible socket model before students are ready for the more complex event-driven callbacks used in ns-3.

### 4. Callback-like behavior

In ns-3, a socket may register a callback such as:

```cpp
m_socket->SetRecvCallback(MakeCallback(&PacketReceiver::HandleRead, this));
```

That means: when a packet arrives, call `HandleRead`.

In Python, the server's `handle_client` function plays a similar role: whenever the socket receives data, the server reacts immediately.

## Why this is useful before ns-3

This Python example is intentionally simpler than ns-3:

- no packet scheduling
- no nodes or links
- no IP routing
- no simulated time

But it teaches the core structure that appears again in ns-3:

- send from one endpoint to another
- receive on the other side
- trigger a handler when data arrives
- process the message

## Suggested student exercises

1. Modify the client to send different message types.
2. Add a simple server log with timestamps.
3. Change the client to send 100 messages and measure round-trip latency.
4. Compare this to the ns-3 demo where packets are scheduled with `Simulator::Schedule`.
5. Discuss how the same pattern becomes event-driven in ns-3 instead of a plain socket loop in Python.

## Important note

This is not a network simulator. It is a teaching example for understanding the socket logic behind packet-based communication.

The real ns-3 flow adds:

- nodes
- links
- channels
- sockets
- packets
- callbacks
- simulated time

That is why ns-3 is the better tool for networking experiments, while Python sockets are the better tool for understanding the core pattern.
