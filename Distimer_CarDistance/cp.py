
import sys
import time
#sys.path.append('/usr/local/lib/python2.7/site-packages')
import cv2

# Camera 0 is the integrated web cam on my netbook
camera_port = 0

#Number of frames to throw away while the camera adjusts to light levels
ramp_frames = 30

# Now we can initialize the camera capture object with the cv2.VideoCapture class.
# All it needs is the index to a camera port.
camera = cv2.VideoCapture(camera_port)

# Captures a single image from the camera and returns it in PIL format
def get_image():
    # read is the easiest way to get a full image out of a VideoCapture object.
    retval, im = camera.read()
    return im

# Ramp the camera - these frames will be discarded and are only used to allow v4l2
# to adjust light levels, if necessary
for i in xrange(ramp_frames):
    temp = get_image()
print("Taking image...")
# Take the actual image we want to keep
camera_capture = get_image()
file = "/Volumes/Work/STU/IoT/FPTDrive/trunk/Distimer_CarDistance/test_image.png"
# A nice feature of the imwrite method is that it will automatically choose the
# correct format based on the file extension you provide. Convenient!
cv2.imwrite(file, camera_capture)

file2 = "/Volumes/Work/STU/IoT/FPTDrive/trunk/Distimer_CarDistance/test_image2.png"
camera_capture = get_image()
cv2.imwrite(file2, camera_capture)
# You'll want to release the camera, otherwise you won't be able to create a new
# capture object until your script exits

for x in range(0, 3):
    print("Taking image... %s" %(x))
    file2 = "/Volumes/Work/STU/IoT/FPTDrive/trunk/Distimer_CarDistance/test_imageaa%s.png" %(x)
    camera_capture = get_image()
    cv2.imwrite(file2, camera_capture)
    time.sleep(15)

del(camera)