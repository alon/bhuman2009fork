#!/bin/bash
# updates the remotecache and copies a bhuman distribution to a robot

usage()
{
  echo "usage: copyfiles.sh [Debug|Optimized|Release] [<ipaddress>|(-m n <ipaddress>)*] {options}"
  echo "  options:"
  echo "    -l <location> set location"
  echo "    -t <color>    set team color to blue or red"
  echo "    -p <number>   set player number"
  echo "    -d            delete remotecache first"
  echo "    -m n <ip>     copy to <ip>, and set playernumber to n (one -m per robot)"
  echo "  examples:"
  echo "    ./copyfiles.sh Optimized 134.102.204.229 -p 1"
  echo "    ./copyfiles.sh Release -d -m 1 10.0.0.1 -m 3 10.0.0.2"
  exit 1
}

copy()
{
  REMOTE=$1
  
  echo "------ Deploying $COMPONENT ($CONFIG) on $REMOTE ------"
  if [ $DELETE -eq 0 ]; then
    SOURCE=`cat ../Build/remotecache/source.cfg`
    REMOTESOURCE=`ssh -i ../Config/Keys/id_rsa_nao -o StrictHostKeyChecking=no root@$REMOTE cat /media/userdata/Config/source.cfg 2> /dev/null`
    [ "$SOURCE" != "$REMOTESOURCE" ] && DELETE=1
  fi
  if [ $DELETE -eq 0 ]; then
    echo "Updating..."
  else
    echo "Sending all..."
    ssh -i ../Config/Keys/id_rsa_nao -o StrictHostKeyChecking=no root@$REMOTE rm -rf /media/userdata/Config
  fi
  pushd ../Build/remotecache > /dev/null
  rsync --delete-during -rcve "ssh -i ../../Config/Keys/id_rsa_nao -o StrictHostKeyChecking=no" * root@$REMOTE:/media/userdata/Config  | sed -e '/sent.*bytes.*received.*bytes.*bytes.*sec/d' -e '/total size is.*speedup/d' -e '/^$/d'
  [ $? -ne 0 ] && exit 1
  popd > /dev/null
}

# init variables
[ -z $CONFIG ] && CONFIG="Debug"
COMPONENT="Nao"
PLAYER=
DELETE=0
TEAM=
LOCATION=
REMOTE=
NUMMULTIPLE=0
MULTIPLEDATA=

# parse options
[ "x" = "x$*" ] && usage
while true; do
  case $1 in
    "")
      break
      ;;
    "-p" | "/p")
      shift
      PLAYER=$1
      ;;
    "-l" | "/l")
      shift
      LOCATION=$1
      ;;
    "-t" | "/t")
      shift
      TEAM=$1
      ;;
    "-d" | "/d")
      DELETE=1
      ;;
    "-h" | "/h" | "/?")
      usage
      ;;
    "-m" | "/m")
      shift
      MULTIPLEDATA[$NUMMULTIPLE]=$1
      shift         
      MULTIPLEDATA[$NUMMULTIPLE + 1]=$1
      echo "Found -m, num=$NUMMULTIPLE with [$NUMMULTIPLE]=${MULTIPLEDATA[$NUMMULTIPLE]}, [$NUMMULTIPLE+1]=${MULTIPLEDATA[$NUMMULTIPLE + 1]}" 
      let "NUMMULTIPLE+=2"
      ;;
    "Nao")
      COMPONENT="Nao"
      ;;
    "Debug")
      CONFIG="Debug"
      ;;
    "Optimized")
      CONFIG="Optimized"
      ;;
    "Release")
      CONFIG="Release"
      ;;
    -*)
      echo "unknown parameter: $1"
      usage
      ;;
    *)
      REMOTE=$1
      ;;
  esac
  shift
done

if [ -z "${ComSpec}${COMSPEC}" ]; then
# on linux
  make $COMPONENT CONFIG=$CONFIG
  [ $? -ne 0 ] && exit 1
else
# on windows
  ../Src/SimulatorQt/Util/zbuildgen/Win32/bin/zbuildgen.exe Remotecache.make.zbuild
  [ $? -ne 0 ] && exit 1
fi

if [ $DELETE -eq 1 ]; then
  make -j 1 -f Remotecache.make clean 
  [ $? -ne 0 ] && exit 1
fi

make -j 1 -f Remotecache.make CONFIG=$CONFIG COMPONENT=$COMPONENT PLAYER=$PLAYER TEAM=$TEAM LOCATION=$LOCATION REMOTE=$REMOTE
[ $? -ne 0 ] && exit 1


if [ ! -z $REMOTE ]; then
  copy $REMOTE
  
else # try to deploy to multiple targets
  if [ "$NUMMULTIPLE" -ne 0 ]; then
    for ((i=0; i < NUMMULTIPLE; i+=2))
    do      
      copy ${MULTIPLEDATA[i+1]}
      # set playernumber on robot
      ssh -i ../Config/Keys/id_rsa_nao -o StrictHostKeyChecking=no root@${MULTIPLEDATA[i+1]} "sed -e 's/playerNumber.*/playerNumber ${MULTIPLEDATA[i]}/' < /media/userdata/Config/settings.cfg > /media/userdata/Config/settingsTEMP.cfg; mv /media/userdata/Config/settingsTEMP.cfg /media/userdata/Config/settings.cfg"
    done
  fi
fi
