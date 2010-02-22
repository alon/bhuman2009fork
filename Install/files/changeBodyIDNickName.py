import os
import sys
import time

path = `os.environ.get("AL_DIR")`
home = `os.environ.get("HOME")`

# import naoqi lib
if path == "None":
  print "the environnement variable AL_DIR is not set, aborting..."
  sys.exit(1)
else:
  alPath = path + "/extern/python/aldebaran"
  alPath = alPath.replace("~", home)
  alPath = alPath.replace("'", "")
  sys.path.append(alPath)
  import naoqi
  from naoqi import ALBroker
  from naoqi import ALModule
  from naoqi import ALProxy

# call method
try:
  dcm = ALProxy("DCM",sys.argv[2] ,9559)

# keep the original RobotID
  #robotId = "AL000000000000000002"
  robotName = sys.argv[1] # "Nao" + robotId[15,5].to_i.to_s
  #print robotId
  print robotName
	
  #ajout des clefs unique dans le DCM
  dcm.preferences("Add", "Chest","Device/DeviceList/ChestBoard/BodyNickName", robotName)
  #dcm.preferences("Add", "Chest", "Device/DeviceList/ChestBoard/BodyId",robotId)


  time.sleep(1)
  dcm.preferences("Save", "Chest", "", 0.0) # to save new calibration


  print "dcm ok"
  exit(0)

except RuntimeError,e:
  print "error dcm"
  exit(1)

