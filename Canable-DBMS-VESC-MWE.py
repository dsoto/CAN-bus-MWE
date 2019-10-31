# this reads CAN bus messages from a connection VESC/DBMS combo
# VESC CAN connector and DBMS CAN connector are connected by all four wires
# DBMS and computer are connected by CANL and CANH only through Canable.io adapter
# This example uses the canard python CAN library for message filtering

# in this example, the BMS has CAN device ID 0x0A
# the VESC has CAN device ID 0x16

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
    # reverses list of 4 bytes and interprets as 32 bit integer and scales
    values.reverse()
    return np.frombuffer(bytes(values), dtype=np.uint8).view(dtype=np.int32)[0]/scale

def get_16(values, scale):
    # reverses list of 2 bytes and interprets as 16 bit integer and scales
    values.reverse()
    return np.frombuffer(bytes(values), dtype=np.uint8).view(dtype=np.int16)[0]/scale

while True:

    # assemble all data in a dictionary
    data = {}

    # grab pack voltage and current from battery
    # message id is CAN_PACKET_BMS_STATUS_MAIN_IV = 0x1E
    # device id is 0x0A
    frame = cq.recv(filter=0x1E0A, timeout=0.5)
    print(frame)
    data['pack_voltage'] = get_32(frame.data[0:4], 1e5)
    data['pack_current'] = get_32(frame.data[4:8], 1e5)

    # grab pack temperature
    # CAN_PACKET_BMS_STATUS_TEMPERATURES = 0x21
    frame = cq.recv(filter=0x210A, timeout=0.5)
    print(frame)
    data['battery_temp'] = get_16(frame.data[6:8], 1e2)

    # grab high and low cell voltage
    # CAN_PACKET_BMS_STATUS_CELLVOLTAGE = 0x1F
    frame = cq.recv(filter=0x1F0A, timeout=0.5)
    print(frame)
    data['cell_voltage_low'] = get_32(frame.data[0:4], 1e5)
    data['cell_voltage_high'] = get_32(frame.data[4:8], 1e5)

    # grab RPM and current
    # CAN_PACKET_STATUS = 0x09
    # device id is 0x16
    frame = cq.recv(filter=0x0916, timeout=0.5)
    print(frame)
    data['rpm'] = get_32(frame.data[0:4], 1)
    data['motor_current'] = get_16(frame.data[4:6], 1e1)

    # grab fet temp
    # CAN_PACKET_STATUS_4 = 0x10
    frame = cq.recv(filter=0x1016, timeout=0.5)
    print(frame)
    data['fet_temp'] = get_16(frame.data[0:2], 1e1)

    # print out the dictionary
    print(data)
    time.sleep(1)
