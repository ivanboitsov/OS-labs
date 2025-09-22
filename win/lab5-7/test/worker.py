import os
import zmq
import time

context = zmq.Context()
socket = context.socket(zmq.DEALER)
socket.bind("tcp://*:5555")

print("Worker node is waiting command...")

while True:
    message = socket.recv_json()
    command = message["command"]

    if command == "exec":
        params = message["params"]
        result = sum(params)
        time.sleep(2)
        socket.send_json({"status" : "Ok", "result" : result})
        print("Something")