#!/bin/bash

set -eu

export scriptPath=$(echo $0 | sed "s|^\.\./|`pwd`/../|" | sed "s|^\./|`pwd`/|")
export baseDir=$(dirname ${scriptPath})
export gtDir=$(dirname ${baseDir})
export includeDir="${baseDir}/include/"

source "${includeDir}/bhumanBase.sh"

function usage()
{
  echo "usage:"
  echo "${0} <name>"
  echo ""
  echo " <name>     : name of the robot to delete"
  exit 1
}

if [ $# -ne 1 ]; then
  usage
fi

${baseDir}/createNewRobot.sh -D ${1:-""}

