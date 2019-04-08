#Put the link to the reference here


from time import sleep
from datetime import datetime
from sh import gphoto2 as gp
import signal, os, subprocess

#For the hardware activation:
import RPi.GPIO as GPIO

#kill gphoto2 process that starts whenever we connect the camera
def killgphoto2Process():
    p = subprocess.Popen(['ps', '-A'], stdout=subprocess.PIPE)
    out, err = p.communicate()

    #Search for the line that has the process we want to kill
    for line in out.splitlines():
        if b'gvfsd-gphoto2' in line:
            #kill the process
            pid = int(line.split(None,1)[0])
            os.kill(pid,signal.SIGKILL)


def captureImages():
    print("before triggerCommand.")
    gp(triggerCommand)
    

def renameFiles(ID):
    for filename in os.listdir("."):
        if len(filename) < 13:
            if filename.endswith(".JPG"):
                #os.rename(filename, (shot_time + ID + ".JPG"))
                os.rename(filename, "new.JPG")
                print("Renamed the JPG")

triggerCommand = ["--trigger-capture"]


shot_date = datetime.now().strftime("%Y-%m-%d")
shot_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
picID = "Image"


#Willy: We can still take 10 photo with commenting killgphoto2Process(). Why? What is killgphoto2Process is for? 
killgphoto2Process()




#For hardware activation
GPIO.setmode(GPIO.BCM)

GPIO.setup(2, GPIO.IN)

while True:
    if GPIO.input(2) == 0:
        shot_date = datetime.now().strftime("%Y-%m-%d")
        shot_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        #createSaveFolder()
        captureImages()
        #renameFiles(picID)
    else:
        print("else")
        sleep(2)
    




    


















































































































































































































































