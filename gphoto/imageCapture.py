from time import sleep
from datetime import datetime
from sh import gphoto2 as gp
import signal, os, subprocess

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


shot_date = datetime.now().strftime("%Y-%m-%d")
shot_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
picID = "PiShots"

triggerCommand = ["--trigger-capture"]


def captureImages():
    print("before triggerCommand.")
    gp(triggerCommand)
    #sleep(1)
    #sleep(2)

def renameFiles(ID):
    for filename in os.listdir("."):
        if len(filename) < 13:
            if filename.endswith(".JPG"):
                #os.rename(filename, (shot_time + ID + ".JPG"))
                os.rename(filename, "new.JPG")
                print("Renamed the JPG")
                

killgphoto2Process()
for x in range(10):
    shot_date = datetime.now().strftime("%Y-%m-%d")
    shot_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    #createSaveFolder()
    captureImages()
    #renameFiles(picID)
    #sleep(2)







    

































































































































































































































