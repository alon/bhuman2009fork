#!/bin/bash

set -eu

export scriptPath=$(echo $0 | sed "s|^\.\./|`pwd`/../|" | sed "s|^\./|`pwd`/|")
export baseDir=$(dirname ${scriptPath})
export gtDir=$(dirname ${baseDir})
export includeDir="${baseDir}/include/"

source "${includeDir}/bhumanBase.sh"
source "${includeDir}/robotAddresses.sh"


targetDir="/media/userdata/system/wpa_supplicant.d/"

function copyData()
{
     if scp -i "${configDir}/Keys/id_rsa_nao" -o StrictHostKeyChecking=no ${fileDir}/wpa_supplicant.conf_* "root@${2}:/${1}" &> /dev/null; then
        message "Copied all existing wpa_supplicant.conf files to director ${1} on robot ${2}"
     else
        error "Failed to copy all existing wpa_suplicant.conf files to robot ${2}"
     fi
}

for ((i=0; i<numOfRobotAddresses; i++)); do
    copyData "${targetDir}" "${robotAddresses[${i}]}" &
done

wait
