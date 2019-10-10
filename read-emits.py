# this reads CAN bus messages from a connection VESC/DBMS combo
# VESC and DBMS are connected by all four wires
# DBMS and computer are connected by CANL and CANH only through Canable.io adapter


from canard.hw import cantact
from canard import can
from canard.utils.queue import CanQueue

import time
import numpy as np

dev = cantact.CantactDev('/dev/tty.usbmodem14101')
dev.set_bitrate(500000)
dev.start()

cq = CanQueue(dev)
cq.start()

def get_32(values, scale):
    values.reverse()
    return np.frombuffer(bytes(values), dtype=np.uint8).view(dtype=np.int32)[0]/scale

while True:

    data = {}
    # TODO: associate values we want with specific frames to read from
    # TODO: associate values with bytes within those specific frames
    # TODO: clean way to gang up stuff
    # TODO: document frame ids and stuff in them in table in thing

    # grab pack voltage and current from battery
    frame = cq.recv(filter=0x1E0A, timeout=0.5)
    data['pack_voltage'] = get_32(frame.data[0:4], 1e5)
    data['pack_current'] = get_32(frame.data[4:8], 1e5)

    # grab RPM
    # TODO: document and determine frame ID
    # frame = cq.recv(filter=?, timeout=0.5)
    # data['ERPM'] = get_32(frame.data[0:4], 1)

    print(data)
    time.sleep(1)
