#replacing-clipboard-content

*Application created as part of the assignment for the Distributed Processing laboratory classes in the fourth semester (2023)*

This repository contains a simple client-server application that demonstrates basic network communication using Winsock2 in C. The server accepts connections from multiple clients and handles each client in a separate thread. Upon receiving a message from a client, the server modifies the clipboard content and sends an acknowledgment message back to the client.

server.c This file contains the server-side code. The server listens for incoming connections on port 27015, handles each connection in a separate thread, and modifies the clipboard content.

client.c This file contains the client-side code. The client connects to the server, sends a predefined message, and waits for a response from the server.

skrypt.bat This batch script compiles the server.c and client.c files using gcc, and then runs the server and client programs sequentially.
