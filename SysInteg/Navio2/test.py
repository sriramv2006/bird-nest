#!usr/bin/env python

from PIL import Image
import select
import v4l2capture
import time

vid = v4l2capture.Video_device("/dev/video0")
#print vid.get_info()
size_x, size_y = vid.set_format(1920, 1080)
vid.create_buffers(1)
vid.queue_all_buffers()

vid.start()
last_time = time.time()

count = 0
while True:
        select.select((vid,), (), ())
	try:
		img = vid.read_and_queue()
                #time.sleep(0.050)
                image = Image.fromstring("RGB",(size_x, size_y), img)
		this_time = time.time()
		print "{}".format(1./(this_time-last_time))
		image.save("./output/tmp"+str(count)+".jpg")
                last_time = this_time
                count = count + 1
                if count > 10:
                        break
	except IOError as e:
		print e

vid.close()
