import zmq
import subprocess
import threading

context = zmq.Context()
socket = context.socket(zmq.ROUTER)
socket.connect("tcp://localhost:5555")

while True:
    command = input("Print the command (create/exec/kill): ").strip().split()

    