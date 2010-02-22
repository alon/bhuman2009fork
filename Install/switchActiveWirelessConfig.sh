#!/bin/bash

set -eu

export scriptPath=$(echo $0 | sed "s|^\.\./|`pwd`/../|" | sed "s|^\./|`pwd`/|")
export baseDir=$(dirname ${scriptPath})
export gtDir=$(dirname ${baseDir})
export includeDir="${baseDir}/include/"

source "${includeDir}/bhumanBase.sh"
source "${includeDir}/robotAddresses.sh"

wlanConfigSuffix=""
runUpdateBeforeActivation=""

function switchConfig()
{
     if ssh -i "${configDir}/Keys/id_rsa_nao" -o StrictHostKeyChecking=no root@${1} cp "/media/userdata/system/wpa_supplicant.d/wpa_supplicant.conf_${wlanConfigSuffix}" "/media/userdata/system/wpa_supplicant.conf" > "${stdout}" 2> "${stderr}" ; then
        message "Switching wireless configuration to ${wlanConfigSuffix} on ${1}"
        ssh -i "${configDir}/Keys/id_rsa_nao" -o StrictHostKeyChecking=no root@${1} /etc/init.d/wireless restart >"${stdout}" 2> "${stderr}" 
     else
        error "Failed to switch wireless configuration to ${wlanConfigSuffix} on ${1}"
     fi
}

function usage()
{
  echo "usage:"
  echo "${0} [-h]"
  echo "${0} [-u] <suffix>"
  echo ""
  echo " <suffix>: the suffix to select the wlan configuration."
  echo "           The file name must be 'wpa_supplicant.conf_<suffix>'"
  echo ""
  echo "  -u     : run updateWirelessConfig.sh before switching to the new configuration" 
  echo "  -h     : Display this help"
  exit 1
}


function parseOptions()
{
  while getopts :uh opt ; do
    case "$opt" in
      u)
        runUpdateBeforeActivation="true"
        ;;
      [?]|h)
        usage
        ;;
    esac
  done
  
  shift $(($OPTIND-1))
  
  if ! [ $# -eq 1 ] ; then
    error "bad number of arguments"
    usage
  fi
  wlanConfigSuffix="${1}"

  debug "runUpdateBeforeActivatione: " "${runUpdateBeforeActivation}"
  debug "wlanConfigSuffix: " "${wlanConfigSuffix}"
}

if [ $# -lt 1 ] ; then
  error "bad number of arguments"
  usage
else
  parseOptions "${@}"
fi


if [ -n "${runUpdateBeforeActivation}" ]; then
  ${baseDir}/updateWirelessConfig.sh
fi


for ((i=0; i<numOfRobotAddresses; i++)); do
  switchConfig "${robotAddresses[${i}]}" &
done

wait
