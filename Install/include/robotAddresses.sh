#!/bin/bash

if [ -z "${gtDir:-""}" ]; then
  echo "This script is only useful when included into other scripts"
  exit 125
fi

declare -a robotAddresses
declare -i numOfRobotAddresses=0

robotAddresses=($(cat ${fileDir}/interfaces_* 2> ${stderr} | grep address | awk '{print $2}' | sort -n )) 
numOfRobotAddresses=${#robotAddresses[@]}
