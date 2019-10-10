from canard.hw import cantact
from canard import can
from canard.utils.queue import CanQueue

import time

dev = cantact.CantactDev('/dev/tty.usbmodem14101')
dev.set_bitrate(500000)
dev.start()

cq = CanQueue(dev)
cq.start()

while True:

    # only print frames we are interested in
    print(cq.recv(filter=0x1E0A, timeout=0.5))
    print(cq.recv(filter=0x1016, timeout=0.5))
    print()
    time.sleep(1)
