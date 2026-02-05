###############################################################################
###############################################################################
#Extrality Lab 2026 - Stockholm University
#Questions? Contact antonio.braga@dsv.su.se or luis.quintero@dsv.su.se
###############################################################################
#This file should be ran together with either the Unity file or the Arduino file. Check the README file for more information about this
###############################################################################
#To run this file, you need to have the 'websockets' library installed. You can install it using pip: "pip install websockets"
###############################################################################
###############################################################################

import asyncio
import websockets
import socket

PORT = 8081
clients = set()


def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # Doesn't have to be reachable
        s.connect(("8.8.8.8", 80))
        ip = s.getsockname()[0]
    finally:
        s.close()
    return ip

async def handler(websocket):
    print("Client connected")
    clients.add(websocket)

    try:
        async for message in websocket:
            print("Received:", message)

            for client in clients.copy():
                if client != websocket:
                    try:
                        await client.send(message)
                    except websockets.exceptions.ConnectionClosed:
                        clients.remove(client)

    except websockets.exceptions.ConnectionClosed:
        print("Client disconnected unexpectedly")

    finally:
        clients.discard(websocket)
        print("Client cleaned up")

async def main():
    print(f"WebSocket server running on IP {get_local_ip()} port {PORT}")
    async with websockets.serve(handler, "0.0.0.0", PORT):
        await asyncio.Future()

asyncio.run(main())
