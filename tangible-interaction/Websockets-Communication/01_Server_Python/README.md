# Python Websocket Server Example

This script runs a websocket server in a local laptop.

## Python Installation

1. Install python 3: https://www.python.org/ 
2. In a terminal, install: `pip install websockets`
   - If on Mac, use a virtual environment:
   - Open the terminal in the directory of the python server script
   - In the terminal write `python3 -m venv venv` or `python -m venv venv`
   - In the terminal write `source venv/bin/activate`. You should now see (venv) followed by your directory
   - In the terminal write `pip install websockets`
3. Run the python script with `python WebsocketServerExample.py` or `python3 WebsocketServerExample.py`
   * Note! On **Mac** you will always need to run "source venv/bin/activate" every time you open a new terminal window before running the server python script
4. You should now see **"WebSocket server running on IP ( ** Your IP address ** ) port ( ** Your port ** )"**
5. Use this IP address in your configuration of Websocket Clients
