#!/bin/bash

set -eu

export scriptPath=$(echo $0 | sed "s|^\.\./|`pwd`/../|" | sed "s|^\./|`pwd`/|")
export baseDir=$(dirname ${scriptPath})
export gtDir=$(dirname ${baseDir})
export includeDir="${baseDir}/include/"

source "${includeDir}/bhumanBase.sh"

robotName=""
configFileSuffix=""
teamID=""
robotPart=""
deleteBeforeWrite=""
deleteOnly=""

usage() {
  echo "usage:"
  echo "${0} [-h]"
  echo "${0} [-d] -t <teamID> -r <robotPart> <name>"
  echo "${0} -D <name>"
  echo ""
  echo " <name>     : name of the robot"
  echo " <teamID>   : the third part of the IPv4 address of the robot"
  echo "              Needs to be the same on all robots of your team"
  echo " <robotPart>: the last part of the IPv4 address of the robot"
  echo "              Needs to be unique for the chosen teamID"
  echo ""
  echo " -d         : Delete existing files for the given robot before creating new files"
  echo " -D         : Only delete alle files for the given robot"
  echo " -h         : Display this help"
  exit 1
}


parseOptions() {
  while getopts :ht:r:Dd opt ; do
    case "$opt" in
      t)
        teamID="$OPTARG"
        ;;
      r)
        robotPart="$OPTARG"
        ;;
      d)
        deleteBeforeWrite="true"
        ;;
      D)
        deleteOnly="true"
        ;;
      [?]|h)
        usage
        ;;
    esac
  done
  
  debug $#
  debug $OPTIND
  shift $(($OPTIND-1))
  
  if ! [ $# -eq 1 ] ; then
    error "bad number of arguments"
    usage
  fi
  if checkBash; then
    robotName=${1,,}
    robotName=${robotName^}
    configFileSuffix="_${1,,}"
  else
    robotName="$(echo "${1}" | cut -c -1 | tr [:lower:] [:upper:])$(echo "${1}" | cut -c 2- | tr [:upper:] [:lower:])"
    configFileSuffix="_$(echo "${1}" | tr [:upper:] [:lower:])"
  fi
  
  if [ -z ${deleteOnly} ]; then
    if [ -z "${teamID}" -o -z "${robotPart}" -o -z "${robotName}" ]; then
      usage
    fi
  else
    if [ -n "${teamID}" -o -n "${robotPart}" -o -z "${robotName}" -o -n "${deleteBeforeWrite}" ]; then
      usage
    fi
    deleteBeforeWrite="true"
  fi

  debug "robotName: " "${robotName}"
  debug "configFileSuffix: " "${configFileSuffix}"
  debug "teamID: " "${teamID}"
  debug "robotPart: " "${robotPart}"
  debug "deleteOnly: " "${deleteOnly}"
  debug "deleteBeforeWrite: " "${deleteBeforeWrite}"
}

function checkFile()
{
  if [ -z ${1:-""} ]; then
    debug "No parameter given"
    return 0
  fi
  if [ -e "${1}" ]; then
    debug "${1} found"
    if [ -z "${deleteBeforeWrite}" ]; then
      return 1
    else
      debug "Deleting ${1}"
      rm -rf "${1}"
      return 0
    fi
  else
    debug "${1} not found"
  fi
}

function checkFiles()
{
  foundFiles=""
  checkFile "${fileDir}/interfaces${configFileSuffix}"               || foundFiles="true"
  checkFile "${fileDir}/ssh/ssh_host_dsa_key${configFileSuffix}"     || foundFiles="true"
  checkFile "${fileDir}/ssh/ssh_host_dsa_key${configFileSuffix}.pub" || foundFiles="true"
  checkFile "${fileDir}/ssh/ssh_host_key${configFileSuffix}"         || foundFiles="true"
  checkFile "${fileDir}/ssh/ssh_host_key${configFileSuffix}.pub"     || foundFiles="true"
  checkFile "${fileDir}/ssh/ssh_host_rsa_key${configFileSuffix}"     || foundFiles="true"
  checkFile "${fileDir}/ssh/ssh_host_rsa_key${configFileSuffix}.pub" || foundFiles="true"
  checkFile "${configDir}/Robots/${robotName}"                       || foundFiles="true"
  debug "foundFiles: ${foundFiles}"

  if [ -n "${foundFiles}" ]; then
    error "Found files for robot ${robotName}"
    fatal "Use -d to cleanup before writing new files"
  fi  
}

function generateSSHKeys()
{
  message "Generating Hostkey..."
  /usr/bin/ssh-keygen -t rsa1 -f "${fileDir}/ssh/ssh_host_key${configFileSuffix}" -N '' > "${stdout}" 2> "${stderr}" || return 1
  message "Generating DSA-Hostkey..."
  /usr/bin/ssh-keygen -d -f "${fileDir}/ssh/ssh_host_dsa_key${configFileSuffix}" -N '' > "${stdout}" 2> "${stderr}" || return 1
  message "Generating RSA-Hostkey..."
  /usr/bin/ssh-keygen -t rsa -f "${fileDir}/ssh/ssh_host_rsa_key${configFileSuffix}" -N '' > "${stdout}" 2> "${stderr}" || return 1
  return 0
}

function createInterfaces()
{
  message "Creating network configuration"
  sed -e s/%teamID%/${teamID}/g -e s/%robotPart%/${robotPart}/g "${fileDir}/_interfaces_template_" > "${fileDir}/interfaces${configFileSuffix}" || return 1
  return 0
}

function copyCalibrationDirectory()
{
  message "Creating calibration directory"
  cp -r "${configDir}/Robots/_template_" "${configDir}/Robots/${robotName}" || return 1
  return 0
}

###############################################################################
##                                                                           ##
##  MAIN                                                                     ##
##                                                                           ##
###############################################################################


checkApp "sed" "stream editor"
checkApp "grep" "grep"
checkApp "awk" "awk"

parseOptions "$@"
checkFiles

if [ -n "${deleteOnly}" ]; then
  message "Successfully deleted all files for ${robotName}"
else
  if ! generateSSHKeys; then
    error "Failed to create SSH keys"
    fatal "Rerun ${0} with -d"
  fi
  if ! createInterfaces; then
    error "Failed to create network configuration"
    fatal "Rerun ${0} with -d"
  fi
  if ! copyCalibrationDirectory; then
    error "Failed to create calibration directory"
    fatal "Rerun ${0} with -d"
  fi
  message "Created needed files for robot ${robotName}"
  message "Don't forget to calibrate ${robotName}"
fi

