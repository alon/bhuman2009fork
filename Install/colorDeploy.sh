#! /bin/bash
set -eu

export scriptPath=$(echo $0 | sed "s|^\.\./|`pwd`/../|" | sed "s|^\./|`pwd`/|")
export baseDir=$(dirname ${scriptPath})
export gtDir=$(dirname ${baseDir})
export includeDir="${baseDir}/include/"

source "${includeDir}/bhumanBase.sh"
source "${includeDir}/robotAddresses.sh"

location=$(grep location ${configDir}/settings.cfg | tr -d "\n" | tr -d "\r" | awk '{print $2}')

if [ -z "${location}" ]; then
  fatal "location not set in settings.cfg"
fi

#FIXME for later use; see below
#targetDirPrefix="/media/userdata/"
#targetDirSuffix="/Locations/${location}/"

targetDir="/media/userdata/Config/Locations/${location}/"

function copyData()
{
     if scp -i "${configDir}/Keys/id_rsa_nao" -o StrictHostKeyChecking=no "${configDir}/Locations/${location}/camera.cfg" "${configDir}/Locations/${location}/coltable.c64" "root@${2}:/${1}" &> /dev/null; then
        message "Copied new camera.cfg and coltable.c64 to directory ${1} on robot ${2}"
     else
        error "Failed to copy new camera.cfg and coltable.c64 to robot ${2}"
     fi
}

for ((i=0; i<numOfRobotAddresses; i++)); do
# FIXME for later use
#  for j in Config Config_Optimized Config_Release; do
#     targetDir="${targetDirPrefix}${j}${targetDirSuffix}"
#     copyData "${targetDir}" "${robotAddresses[${i}]}" &
#  done
  copyData "${targetDir}" "${robotAddresses[${i}]}" &
done

wait
