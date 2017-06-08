from __future__ import print_function, division

from ardupilot import ArduPilot
from PIL import Image
import select
import v4l2capture
import time


def initGPS( port, baud ):
    AP = ArduPilot( port, baud, True)
    print("Finished instantiating AP")
    print(AP.getMode())
    print("Finished getting mode")
    AP.arm()
    AP.setDataStreams()
    return AP


def initCam( dev, sizeX, sizeY ):
    vid = v4l2capture.Video_device("/dev/video0")
    #print vid.get_info()
    #size_x, size_y = vid.set_format(1920, 1080)
    #size_x, size_y = vid.set_format(4208,3120)
    size_x, size_y = vid.set_format(3840,2160)
    print(size_x,size_y)
    vid.create_buffers(1)
    vid.queue_all_buffers()

    vid.start()
    return vid


def saveGPSData(AP, filename):
    with open( filename, "w+" ) as f:
        try:
            alt = AP.getAltitude()
            print("altitude: {:+.5f}".format(alt), end=", ", file=f)
        except Exception:
            print("altitude: ", end=", ", file=f)

        try:
            heading = AP.getHeading()
            print("heading: {:+.5f}".format(heading), end=", ", file=f)
        except Exception:
            print("heading: ", end=",", file=f)

        try:
            speed = AP.getSpeed()
            print("speed: {:+.5f}".format(speed), end=", ", file=f)
        except Exception:
            print("speed: ", end=", ", file=f)

        try:
            loc = AP.getLocation()
            print("location: {}".format(loc), end=", ", file=f)
        except Exception:
            print("location: ", end=", ", file=f)

        try:
            att = AP.getAttitude()
            print("attitude: {}".format(att), file=f)
        except Exception:
            print("attitude: ", file=f)


def saveImage( vid, filename, size_x, size_y ):
    last_time = time.time()
    print( "Before select" )
    select.select((vid,), (), ())
    print("After select")
    try:
        print( "Qing" )
        img = vid.read_and_queue()
        #time.sleep(0.050)
        print( "saving Image" )
        image = Image.fromstring("RGB",(size_x, size_y), img)
        this_time = time.time()
        print("{}".format((this_time-last_time)) )
        image.save(filename)
        last_time = this_time
    except IOError as e:
        print(e)


if __name__ == '__main__':
    ap = initGPS( 'udp:localhost:14550', 15200 )
    vid = initCam( "/dev/video0", 1920, 1080 )

    for i in xrange(20):
        # sleep to wait for data
        time.sleep(0.7)
        filename = "testgps/" + time.strftime("%Y_%m_%d_%H_%M_%S")
        saveGPSData( ap, filename + ".data" )
        saveImage( vid, filename + ".jpg", 3840, 2160 )
        # get image and save to the same filename, different folder
    vid.close()
