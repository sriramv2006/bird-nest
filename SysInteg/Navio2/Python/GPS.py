import time

import navio.util
from navio.ublox import Ublox
from navio.ublox import NavStatusMsg
from navio.ublox import NavPosllhMsg


if __name__ == "__main__":
    ubl = Ublox()
    navstat = NavStatusMsg()
    #navpos = NavPosllhMsg()
    #print navpos

    navio.util.check_apm()

    for ind in range(0, 20):
        ubl.enable_posllh()

    print("Starting service")

    while(1):
        init = time.time()
        buffer = ubl.bus.xfer2([10000])
        for byt in buffer:
            ubl.scan_ubx(byt)
            if not ubl.mess_queue.empty():
                msg = ubl.parse_ubx()
                if (msg != None):
                    print("\nMESSAGE INCOMING!")
                    print(msg)
                    print( "{:7.4f}".format( (time.time() - init)*1000 ) )
