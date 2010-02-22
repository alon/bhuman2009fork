#!/usr/bin/env bash

set -u
set -o pipefail
set -e

scriptVersion="1.3.13"
imageName="opennao-robocup-1.3.13-nao-geode.ext3"
archivePath=""
imagePath=""
deviceName=""
devicePath=""
robotName=""
configFileSuffix=""
wlanConfigSuffix="template"

flashDevice="true"
installBHumanFiles="true"
forceDevice=""
wipeDevice=""
removeJournal="true"
unsafeDeviceSearch=""

rootMP=""
dataMP=""
phase2=""

export scriptPath=$(echo $0 | sed "s|^\.\./|`pwd`/../|" | sed "s|^\./|`pwd`/|")
export baseDir=$(dirname ${scriptPath})
export gtDir=$(dirname ${baseDir})
export includeDir="${baseDir}/include/"

source "${includeDir}/bhumanBase.sh"


usage() {
  echo "${0} ${scriptVersion}"
  echo ""
  echo "usage:"
  echo "${0} [-h]"
  echo "${0} [-r] [-u] [-s] [-w] [-i <image>] [-d|-D <dev>]"
  echo "${0} [-f] [-u] [-s] [-j] [-W <suffix>] [-w] [-i <image>] [-d|-D <dev>] name"
  echo ""
  echo " <image> : the image to flash on the drive as uncompressed oder bzipped file"
  echo " <dev>   : the name of the device (eg sdb or /dev/sdb)"
  echo " <suffix>: the suffix to select the wlan configuration."
  echo "           The file name must be 'wpa_supplicant.conf_<suffix>'"
  echo ""
  echo "  -i     : Use given image instead of default image"
  echo "  -d     : use the device <dev> and do some basic safety checks"
  echo "  -D     : use the device <dev> without any safety check."
  echo "  -f     : Do not flash the device. Conflicts with -r. Can result in an unusable stick"
  echo "  -j     : Do not delete the ext3 journal. Default with -r"
  echo "  -r     : Write original image without any modifications to device."
  echo "  -W     : use given suffix instead of default (${wlanConfigSuffix})"
  echo "  -w     : Wipe the device before writing image. With -r disable wiping device"
  echo "  -u     : accept unsafe dmesg based device detection results" 
  echo "  -h     : Display this help"
  exit 1
}


parseOptions() {
  local restore=""
  local noflash=""
  local wipe=""
  local keepJournal=""

  while getopts :urjwhfi:d:D:W: opt ; do
    case "$opt" in
      d)
        deviceName="$OPTARG"
        ;;
      D)
        deviceName="$OPTARG"
        forceDevice="true"
        ;;
      i)
        imageName="$OPTARG"
        ;;
      u)
        unsafeDeviceSearch="true"
        ;;
      f)
        noflash="true"
        ;;
      r)
        restore="true"
        ;;
      W)
        wlanConfigSuffix="${OPTARG}"
        ;;
      [?]|h)
        usage
        ;;
      w)
        wipe="true"
        ;;
      j)
        keepJournal="true"
        ;;
    esac
  done
  
  shift $(($OPTIND-1))
  
  if [ -z "${restore}" ]; then
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
  else
    if [ $# -eq 1 ] ; then
      usage
    fi
  fi

  if [ -n "${deviceName}" ]; then
    deviceName=$(echo "${deviceName}" | sed -e 's%.*/%%g' -e 's/[0-9]$//g')
  fi


  if [ -z "${restore}" ]; then
    installBHumanFiles="true"
    if [ -n "${wipe}" ]; then
      wipeDevice="true"
    fi
    if [ -n "${noflash}" ]; then
      flashDevice=""
    fi
    if [ -n "${keepJournal}" ]; then
      removeJournal=""
    fi
  fi

  if [ -n "${restore}" ]; then
    installBHumanFiles=""
    if [ -n "${wipe}" ]; then
      warning "Wiping device is default in restore mode"
      warning "-w will disable wiping"
      wipeDevice=""
    else
      wipeDevice="true"
    fi
    flashDevice="true"
    if [ -n "${noflash}" ]; then
      warning "-f conficts with restoring original stick."
      warning "Ignoring -f"
    fi
    removeJournal=""
  fi
  
  debug "robotName: " "${robotName}"
  debug "configFileSuffix: " "${configFileSuffix}"
  debug "flashDevice: " "${flashDevice}"
  debug "installBHumanFiles: " "${installBHumanFiles}"
  debug "wipeDevice: " "${wipeDevice}"
  debug "removeJournal: " "${removeJournal}"
  debug "unsafeDeviceSearch: " "${unsafeDeviceSearch}"
  debug "deviceName: " "${deviceName}"
  debug "forceDevice: " "${forceDevice}"
  debug "imageName: " "${imageName}"

  if [ -z "${imageName}" -o -z "${robotName}" ]; then
    error "Missing parameter"
    usage
  fi
}


searchImage() {
  if [ -r "${imageName}" -o -r "${imageName}.bz2" ]; then
    imagePath="${imageName}"
  elif [ -r "${imageDir}/${imageName}" -o -r "${imageDir}/${imageName}.bz2" ]; then
    imagePath="${imageDir}/${imageName}"
  elif [ -r "${scriptPath}/${imageName}" -o -r "${scriptPath}/${imageName}.bz2" ]; then
    imagePath="${scriptPath}/${imageName}"
  elif [ -r "${fileDir}/${imageName}" -o -r "${fileDir}/${imageName}.bz2" ]; then
    imagePath="${fileDir}/${imageName}"
  else
    fatal "No image found"
  fi
  STRIPPED_PATH=$(echo ${imagePath} | sed -e 's/.bz2$//g')
  if [ -r "${STRIPPED_PATH}.bz2" ]; then
    archivePath="${STRIPPED_PATH}.bz2"
    imagePath="$(mktemp 2>${stderr})"
  fi

  debug "imagePath: " "${imagePath}"
  debug "archivePath: " "${archivePath}"
}


checkSysFS() {
  if [ -z "${1:-""}" -o -z "${2:-""}" ]; then
    error "Missing parameter for checkSysFS"
    return 1
  fi
  local hasSysFS=""
  if [ ! -d "/sys/bus/scsi/devices" ]; then
    warning "sysfs is not accessible. This disables some safety checks"
  else
    debug "enabled sysfs check"
    hasSysFS="true"
  fi
  if [ -n "${hasSysFS}" ]; then
    if [ -e "/sys/bus/scsi/devices/${1}" ]; then
      debug "Found /sys/bus/scsi/devices/${1}"
      if [ -r "/sys/bus/scsi/devices/${1}/vendor" ]; then
        debug "Found /sys/bus/scsi/devices/${1}/vendor"
        if [ "${2}" != `cat /sys/bus/scsi/devices/${1}/vendor` ]; then
          debug "Inconsistent data for ${1}" 
          return 1
        else
          debug "Verified vendor name for device ${1}"
        fi
      else
        debug "/sys/bus/scsi/devices/${1}/vendor does not exist"
        return 1
      fi
    else
      debug "/sys/bus/scsi/devices/${1} does not exist"
      return 1
    fi
  fi
}


searchDeviceSysfsDeprecated() {
  declare -a devices
  declare -a scsiIDs
  declare -a vendors
  declare -i deviceCount=0
  
  if [ -d "/sys/bus/scsi/devices/" ]; then
    cd "/sys/bus/scsi/devices/"
  else
    debug "found unexpected sysfs directory structure"
    return
  fi
  for i in *; do
    if [ -r "$i/vendor" ]; then
      vendor=$(cat "${i}/vendor" | sed -e 's/ //g')
      if ! device=$(ls "${i}" | grep "block:" | sed -e 's/block://g'); then
        debug "found unexpected sysfs directory structure"
        return
      fi
      if [ ${vendor} = "Kingmax" ]; then
        debug "_${i}_${vendor}_${device}_"
        devices[${deviceCount}]="${device}"
        vendors[${deviceCount}]="${vendor}"
        scsiIDs[${deviceCount}]="${i}"
        deviceCount=$((${deviceCount}+1))
      else
        debug "Ignoring ${i}"
      fi
    fi
  done

  if [ ${deviceCount} -gt 0 ]; then
    for ((j=0; j<${deviceCount}; j++)); do
      message "Found ${vendors[${j}]} drive with device name ${devices[${j}]} and SCSI-ID ${scsiIDs[${j}]}"
    done
    if [ ${deviceCount} -gt 1 ]; then
      error "Detected more than one known drive"
      fatal "Select one using -d"
    else
      deviceName="${devices[0]}"
      debug "Only one known drive found"
    fi
    message "Press Ctrl-C within 5 seconds to abort"
    sleep 5
  else
   fatal "No known flash drive found"
  fi
  cd "${baseDir}"
}


searchDeviceSysfs() {
  declare -a devices
  declare -a scsiIDs
  declare -a vendors
  declare -i deviceCount=0
  declare -i blockCount=0
  
  if [ -d "/sys/bus/scsi/devices/" ]; then
    cd "/sys/bus/scsi/devices/"
  else
    debug "found unexpected sysfs directory structure"
    return
  fi
  for i in [0-9]*:[0:9]*:[0-9]*:[0-9]*; do
    echo "$i"
    blockCount=0
    if [ -r "$i/vendor" ]; then
      vendor=$(cat "${i}/vendor" | sed -e 's/ //g')
      if [ ! -d "${i}/block" ]; then
        continue
      fi
      blockCount=$(ls "${i}/block" | wc -l)
      device=$(ls "${i}/block" | head -1)
      if [ ${vendor} = "Kingmax" -a ${blockCount} -eq 1 ]; then
        debug "_${i}_${vendor}_${device}_"
        devices[${deviceCount}]="${device}"
        vendors[${deviceCount}]="${vendor}"
        scsiIDs[${deviceCount}]="${i}"
        deviceCount=$((${deviceCount}+1))
      else
        debug "Ignoring ${i}"
      fi
    fi
  done

  if [ ${deviceCount} -gt 0 ]; then
    for ((j=0; j<${deviceCount}; j++)); do
      message "Found ${vendors[${j}]} drive with device name ${devices[${j}]} and SCSI-ID ${scsiIDs[${j}]}"
    done
    if [ ${deviceCount} -gt 1 ]; then
      error "Detected more than one known drive"
      fatal "Select one using -d"
    else
      deviceName="${devices[0]}"
      debug "Only one known drive found"
    fi
    message "Press Ctrl-C within 5 seconds to abort"
    sleep 5
  else
   fatal "No known flash drive found"
  fi
  cd "${baseDir}"
}


searchDeviceDmesg() {
  local scsiId=$(dmesg |grep -e Kingmax |sed 's/^.*scsi \([0-9]*:[0-9]*:[0-9]*:[0-9]*:\).*$/\1/' | sed -e 's/:$//g' | tail -1)
  if [ -z "${scsiId}" ] ; then
    fatal "Error: no known flash drive found"
  else
    if [ -n "${scsiId}" ]; then
      debug "Found Kingmax flash drive with SCSI-ID ${scsiId}"
      if ! checkSysFS "${scsiId}" "Kingmax"; then
        debug "failed sysfs check for ${scsiId}"
        scsiId=""
      else
        debug "successful sysfs check for ${scsiId}"
      fi
    fi
  fi

  local device=""
  if [ -n "${scsiId}" ]; then
    local device=$(dmesg |grep "${scsiId}" | grep "removable disk"| sed 's/.*\[\([a-z]*\)\].*$/\1/' | tail -1)
  fi
  if [ -z "${device}" ] ; then
    fatal "Error: no known flash drive found"
  else
    debug "Found Kingmax flash drive with device name ${device}"
  fi

  if [ -n "${device}" ]; then
    if [ -z "${unsafeDeviceSearch}" ]; then
      error "dmesg based device detection found device ${device}"
      error "The result is ignored, because the dmseg based detection is known to be unsafe"
      fatal "Use -u to accept unsafe result or check the device name an call ${0} with -d ${device}"
    else
      warning "dmesg based device detection was used for ${device}"
      warning "This detection method is known to be unsafe"
      deviceName="${device}"
      warning "Press Ctrl-C within 5 seconds to abort"
      sleep 5
    fi
  fi
  debug "deviceName   : ${deviceName}" 
}

  
checkDevice() {
  if [ -z "${deviceName}" ]; then
    fatal "No device found"
  fi
  devicePath="/dev/${deviceName}"
  if [ ! -b "${devicePath}" ]; then
    fatal "Device ${deviceName} not found"
  else
    if [ $(($(cat /proc/partitions | grep ${deviceName} | wc -l)-1)) -ne 2 ]; then
      if [ "${forceDevice}" != "true" ]; then
        error "Given device has $(($(cat /proc/partitions | grep ${deviceName} | wc -l)-1)) partitions"
        error "Are you sure that ${deviceName} is the right device?"
        fatal "Use -D instead of -d to use this device"
      else
        warning "Given device has $(($(cat /proc/partitions | grep ${deviceName} | wc -l)-1)) partitions, expected 2"
      fi
    fi
    message "Using device ${deviceName}"
  fi
}


checkedUmount() {
  if [ -z ${1:-""} ]; then
    error "checkedUmount needs one device name as parameter"
    return 1
  fi
  if ! [ -b "${1}" ]; then
    debug "Device ${1} not found"
    return 0
  fi
  local abort=""
  while [ -z "${abort}" ]; do 
    if mount | grep "${1}" 2>"${stderr}" >"${stdout}"; then
      if ! umount "${1}" 2>"${stderr}" >"${stdout}"; then
        error "failed to unmount ${1}"
        return 1
      else
        debug "successful umount for ${1}"
      fi
    else
      debug "${1} is not mounted"
      abort="true"
    fi
  done
}


wipeAndFlashDevice() {
  if ! checkedUmount "${devicePath}1"; then
    fatal "Failed to unmount device ${devicePath}1"
  fi
  if ! checkedUmount "${devicePath}2"; then
    fatal "Failed to unmount device ${devicePath}2"
  fi
  if [ -n "${wipeDevice}" ]; then
    message "Wiping usb stick"
    dd bs=8192 if=/dev/zero of="${devicePath}" || echo -n ""
  fi
  if [ -n "${archivePath}" ]; then
    message "Extracting ${archivePath} to ${devicePath}"
    if ! bunzip2 -k -c -p ${archivePath} | dd bs=8192 of="${devicePath}" 2>"${stderr}" >"${stdout}"; then
     fatal "Failed to decompress and write"
    fi
  else
    message "Writing image"
    if ! dd bs=8192 if="${imagePath}" of="${devicePath}" 2>"${stderr}" >"${stdout}"; then
      if [ -n "${archivePath}" ]; then
        rm "${imagePath}"
      fi
      fatal "Failed to write image"
    fi
  fi

  sleep 1
  message "Syncing disks"
  sync 2>"${stderr}" >"${stdout}"
  sleep 1

  sfdisk -R "${devicePath}" 2>"${stderr}" >"${stdout}"
  if ps ax | grep hal[d] 2>"${stderr}" >"${stdout}"; then
    message "detected running hal daemon"
    message "Waiting for hal to react on partition table reload"
    sleep 20
  fi
  if ! checkedUmount "${devicePath}1"; then
    error "Failed to unmount device ${devicePath}1"
  else
    if [ -n "${removeJournal}" ]; then
      message "Removing journal from ${devicePath}1"
      tune2fs -O ^has_journal "${devicePath}1" 2>"${stderr}" >"${stdout}"
      message "Checking filesystem on ${devicePath}1"
      e2fsck -p -f "${devicePath}1" 2>"${stderr}" >"${stdout}" || echo -n ""
    else
      message "Removing ext3 journal disabled by -n or -r"
    fi
    tune2fs -L "system" "${devicePath}1" 2>"${stderr}" >"${stdout}"
  fi

  
  if ! checkedUmount "${devicePath}2" ; then
    error "Failed to unmount device ${devicePath}2"
  else
    message "Formating data partition"
    if ! mkfs.vfat -n "userdata" "${devicePath}2"  2>"${stderr}" >"${stdout}"; then
      fatal "Failed to format second partition"
    fi
  fi
}


createPhase2() {
  phase2=$(mktemp 2>"${stderr}")
  if [ -z "${phase2}" ]; then
    error "Failed to create temporary file"
    return 1
  fi

  cat ${fileDir}/phase2.sh | sed -e "s/%ROBOTNAME%/${robotName}/g" > "${phase2}"
}


checkedChown(){
  if [ -z "${1:-""}" ]; then
    error "Missing parameter for checkedChown"
    return 1
  fi
  if ! chown 0:0 "${1}" 2>"${stderr}" >"${stdout}"; then
    error "chown 0:0 ${1} failed"
    return 1
  else
    debug "successful chown 0:0 ${1}"
    return 0
  fi
}


checkedChmod() {
  if [ -z "${1:-""}" ]; then
    error "Missing parameter for checkedMkdir"
    return 1
  fi
  local mode="000"
  if [ -d "${1}" ]; then
    mode="${2:-"755"}"
  else
    mode="${2:-"644"}"
  fi
  if ! chmod "${mode}" "${1}" 2>"${stderr}" >"${stdout}"; then
    error "chmod ${mode} ${1} failed"
    return 1
  else
    debug "successful chmod ${mode} ${1}"
    return 0
  fi
}


checkedCopy(){
  if [ -z "${1:-""}" -o -z "${2:-""}" ]; then
    error "Missing parameter for checkedCopy"
    return 1
  fi
  if ! cp "${1}" "${2}" 2>"${stderr}" >"${stdout}"; then
    error "cp from ${1} to ${2} failed"
    return 1
  else
    checkedChown "${2}"
    if [ -z "${3:-""}" ]; then
      checkedChmod "${2}"
    else
      checkedChmod "${2}" "${3}"
    fi  
    debug "successful cp for ${1} to ${2}"
    return 0
  fi
}


checkedMove(){
  if [ -z "${1:-""}" -o -z "${2:-""}" ]; then
    error "Missing parameter for checkedMove"
    return 1
  fi
  if ! mv "${1}" "${2}" 2>"${stderr}" >"${stdout}"; then
    error "mv from ${1} to ${2} failed"
    return 1
  else
    checkedChown "${2}"
    if [ -z "${3:-""}" ]; then
      checkedChmod "${2}"
    else
      checkedChmod "${2}" "${3}"
    fi  
    debug "successful mv for ${1} to ${2}"
    return 0
  fi
}


checkedMkdir(){
  if [ -z "${1:-""}" ]; then
    error "Missing parameter for checkedMkdir"
    return 1
  fi
  if ! mkdir -p "${1}" 2>"${stderr}" >"${stdout}"; then
    error "mkdir -p ${1} failed"
    return 1
  else
    checkedChown "${1}"
    if [ -z "${3:-""}" ]; then
      checkedChmod "${1}"
    else
      checkedChmod "${1}" "${3}"
    fi  
    debug "successful mkdir -p ${1}"
    return 0
  fi
}


copyFiles() {
  if checkedMkdir "${rootMP}/home/root/"; then
    checkedMove "${phase2}" "${rootMP}/home/root/phase2.sh" "755"
    checkedCopy "${fileDir}/bhuman" "${rootMP}/home/root/bhuman" "755"
    checkedCopy "${fileDir}/save" "${rootMP}/home/root/save" "755"
    checkedCopy "${fileDir}/naoqi" "${rootMP}/home/root/naoqi" "755"
    checkedCopy "${fileDir}/stop" "${rootMP}/home/root/stop" "755"
    checkedCopy "${fileDir}/status" "${rootMP}/home/root/status" "755"
    checkedCopy "${fileDir}/qitobh" "${rootMP}/home/root/qitobh" "755"
    checkedCopy "${fileDir}/bhtoqi" "${rootMP}/home/root/bhtoqi" "755"
    ln -sf "/etc/init.d/bhumand" "${rootMP}/home/root/bhumand" 2>"${stderr}" >"${stdout}"
    ln -sf "/etc/init.d/naoqi" "${rootMP}/home/root/naoqid" 2>"${stderr}" >"${stdout}"
  fi
  if checkedMkdir "${rootMP}/etc/ssh/"; then
    checkedCopy "${fileDir}/ssh/ssh_host_key${configFileSuffix}" "${rootMP}/etc/ssh/ssh_host_key" "600"
    checkedCopy "${fileDir}/ssh/ssh_host_dsa_key${configFileSuffix}" "${rootMP}/etc/ssh/ssh_host_dsa_key" "600"
    checkedCopy "${fileDir}/ssh/ssh_host_rsa_key${configFileSuffix}" "${rootMP}/etc/ssh/ssh_host_rsa_key" "600"
    checkedCopy "${fileDir}/ssh/ssh_host_key${configFileSuffix}.pub" "${rootMP}/etc/ssh/ssh_host_key.pub" 
    checkedCopy "${fileDir}/ssh/ssh_host_dsa_key${configFileSuffix}.pub" "${rootMP}/etc/ssh/ssh_host_dsa_key.pub"
    checkedCopy "${fileDir}/ssh/ssh_host_rsa_key${configFileSuffix}.pub" "${rootMP}/etc/ssh/ssh_host_rsa_key.pub"
  fi
  if checkedMkdir "${rootMP}/home/root/files"; then
    checkedCopy "${fileDir}/menu.lst_sda" "${rootMP}/home/root/files/menu.lst_sda"
    checkedCopy "${fileDir}/menu.lst_sdb" "${rootMP}/home/root/files/menu.lst_sdb"
    checkedCopy "${fileDir}/fstab_label" "${rootMP}/home/root/files/fstab_label"
    checkedCopy "${fileDir}/changeBodyIDNickName.py" "${rootMP}/home/root/files/changeBodyIDNickName.py" "755"
  fi
  if checkedMkdir "${rootMP}/home/root/.ssh" "700" ; then
    checkedCopy "${fileDir}/ssh/authorized_keys" "${rootMP}/home/root/.ssh/authorized_keys"
  fi
  if checkedMkdir "${rootMP}/etc/network/"; then
    checkedCopy "${fileDir}/interfaces${configFileSuffix}" "${rootMP}/etc/network/interfaces"
  fi
  
  checkedCopy "${fileDir}/fstab_firstboot" "${rootMP}/etc/fstab"
  if checkedMkdir "${rootMP}/etc/init.d/"; then
    checkedCopy "${fileDir}/bhumand" "${rootMP}/etc/init.d/bhumand" "755"
  fi
  if checkedMkdir "${rootMP}/usr/bin/"; then
    checkedCopy "${fileDir}/rsync" "${rootMP}/usr/bin/rsync" "755"
  fi 
  if checkedMkdir "${rootMP}/etc/init.d/"; then
    checkedCopy "${fileDir}/libacl.so.1.1.0" "${rootMP}/lib/libacl.so.1.1.0" "644"
    checkedCopy "${fileDir}/libpopt.so.0.0.0" "${rootMP}/lib/libpopt.so.0.0.0" "644"
    checkedCopy "${fileDir}/libattr.so.1.1.0" "${rootMP}/lib/libattr.so.1.1.0" "644"
    cd "${rootMP}/lib/" 2>"${stderr}" >"${stdout}"
    ln -sf "libacl.so.1.1.0" "libacl.so.1" 2>"${stderr}" >"${stdout}"
    ln -sf "libpopt.so.0.0.0" "libpopt.so.0" 2>"${stderr}" >"${stdout}"
    ln -sf "libattr.so.1.1.0" "libattr.so.1" 2>"${stderr}" >"${stdout}"
    cd "${baseDir}" 2>"${stderr}" >"${stdout}"
  fi

  if checkedMkdir "${rootMP}/etc/udhcpc.d/"; then
    checkedCopy "${fileDir}/dhcp-sayip" "${rootMP}/etc/udhcpc.d/99sayip" "755"
  fi
 
  cd "${rootMP}/etc/" 2>"${stderr}" >"${stdout}"
  patch -p0 < "${fileDir}/rc.0.patch" 2>"${stderr}" >"${stdout}"
  patch -p0 < "${fileDir}/passwd.0.patch" 2>"${stderr}" >"${stdout}"
  cd "${baseDir}" 2>"${stderr}" >"${stdout}"

  cd "${rootMP}/home/root/" 2>"${stderr}" >"${stdout}"
  patch -p0 < "${fileDir}/profile.0.patch" 2>"${stderr}" >"${stdout}"
  cd "${baseDir}" 2>"${stderr}" >"${stdout}"

  checkedMkdir "${dataMP}/Config"
  if checkedMkdir "${rootMP}/opt/naoqi/modules/lib/"; then 
    ln -sf "/media/userdata/Config/libbhuman.so" "${rootMP}/opt/naoqi/modules/lib/" 2>"${stderr}" >"${stdout}"

    if [ ! -f "${rootMP}/opt/naoqi/modules/lib/autoload.org" ] ; then
      checkedCopy "${rootMP}/opt/naoqi/modules/lib/autoload.ini" "${rootMP}/opt/naoqi/modules/lib/autoload.org"
    fi
    checkedCopy "${fileDir}/autoload.ini" "${rootMP}/opt/naoqi/modules/lib/autoload.bh"
  fi
  
  if checkedMkdir "${dataMP}/system"; then
  
    if checkedMkdir "${dataMP}/system/wpa_supplicant.d/"; then
      cd "${fileDir}" 2>"${stderr}" >"${stdout}"
      for i in wpa_supplicant.conf*; do
        checkedCopy "${i}" "${dataMP}/system/wpa_supplicant.d/${i}"
      done
      if ! checkedCopy "${fileDir}/wpa_supplicant.conf_${wlanConfigSuffix}" "${dataMP}/system/wpa_supplicant.conf"; then
        error "Could not copy wpa_supplicant.conf"
        error "Copy correct file to /media/userdata/system/wpa_supplicant.conf to enable wireless lan"
      fi
      cd "${baseDir}" 2>"${stderr}" >"${stdout}"
      cd "${rootMP}/etc/" 2>"${stderr}" >"${stdout}"
      ln -sf "/media/userdata/system/wpa_supplicant.conf" "wpa_supplicant.conf" 2>"${stderr}" >"${stdout}"
      cd "${baseDir}" 2>"${stderr}" >"${stdout}"
    fi
    checkedCopy "${fileDir}/asound.state" "${dataMP}/system/asound.state"
    cd "${rootMP}/etc/" 2>"${stderr}" >"${stdout}"
    ln -sf "/media/userdata/system/asound.state" "asound.state" 2>"${stderr}" >"${stdout}"
    cd "${baseDir}" 2>"${stderr}" >"${stdout}"
  fi

  if [ ! -f "${rootMP}/etc/issue.org" ] ; then
    checkedMove "${rootMP}/etc/issue" "${rootMP}/etc/issue.org"
    echo "=== B-Human ===  ${robotName}" > "${rootMP}/etc/issue"
  fi
  if [ ! -f "${rootMP}/etc/issue.net.org" ] ; then
    checkedMove "${rootMP}/etc/issue.net" "${rootMP}/etc/issue.net.org"
    echo "=== B-Human ===  ${robotName}" > "${rootMP}/etc/issue.net"
  fi
  echo "${robotName}" > "${rootMP}/etc/hostname"
  if [ -z "$(grep "UseDNS no" "${rootMP}/etc/ssh/sshd_config")" ]; then 
    echo "UseDNS no" >> "${rootMP}/etc/ssh/sshd_config"
  fi          

  if checkedMkdir "${rootMP}/usr/local/bin/"; then
    cd "${rootMP}/usr/local/"
    tar -xjf "${fileDir}/tts.tar.bz2" 2>"${stderr}" >"${stdout}"
    ln -sf "/usr/local/tts/bin/say" "${rootMP}/usr/local/bin/say" 2>"${stderr}" >"${stdout}"
    cd "${baseDir}" 2>"${stderr}" >"${stdout}"
  fi
}



###############################################################################
##                                                                           ##
##  MAIN                                                                     ##
##                                                                           ##
###############################################################################


checkRoot

debug "baseDir: " "${baseDir}"
debug "imageDir: " "${imageDir}"
debug "fileDir: " "${imageDir}"

checkApp "sed" "stream editor"
checkApp "grep" "grep"
checkApp "mount" "mount file systems"
checkApp "umount" "unmount file systems"

if ! checkBash; then
  checkApp "tr" "tool for translating characters" 
fi

if [ $# -lt 1 ] ; then
  error "bad number of arguments"
  usage
else
  parseOptions "${@}"
fi

if [ -n "${flashDevice}" ]; then
  searchImage
fi
if [ -z "${deviceName}" ]; then
  if [ -e "/sys/bus/scsi/devices/" ]; then
    debug "Searching device with sysfs"
    searchDeviceSysfs
    if [ -e "${deviceName}" ]; then
      debug "Searching sysfs failed. Trying method for deprecated sysfs structure."
      searchDeviceSysfsDeprecated
    fi
  else
    debug "Searching device via dmesg"
    searchDeviceDmesg
  fi
fi

checkDevice

checkApp "mkfs.vfat" "tools to create fat32 partition"
checkApp "dd" "tools to do binary copy"
checkApp "mktemp" "tools to create temp files and directories"
checkApp "whoami" "know which user is running the current process"
checkApp "sfdisk" "reload the partition table"
checkApp "patch" "apply patch files"
checkApp "tar" "tool to extract tar archives"
checkApp "bunzip2" "tool to extract bzipped files"
if [ -n "${removeJournal}" ]; then
  checkApp "tune2fs" "tool to change ext2/3 file system settings"
  checkApp "e2fsck" "tool for checking an ext2/3 file system"
fi

if [ -n "${flashDevice}" ]; then
  wipeAndFlashDevice
fi


if [ -n "${installBHumanFiles}" ]; then

  rootMP=$(mktemp -d 2>"${stderr}" )
  dataMP=$(mktemp -d 2>"${stderr}" )

  debug "rootMP" "${rootMP}"
  debug "dataMP" "${dataMP}"

  if [ -z "${rootMP}" -o -z "${dataMP}" ]; then
    rm -Rf "${rootMP}" 2>"${stderr}" >"${stdout}"
    rm -Rf "${dataMP}" 2>"${stderr}" >"${stdout}"
    fatal "Could not create mount points"
  fi
 
  if ! mount -t ext2 "${devicePath}1" "${rootMP}" 2>"${stderr}" >"${stdout}"; then
    rm -Rf "${rootMP}" 2>"${stderr}" >"${stdout}" 
    rm -Rf "${dataMP}" 2>"${stderr}" >"${stdout}"
    fatal "Could not mount root filesystem"
  else
    if ! mount -t vfat "${devicePath}2" "${dataMP}" 2>"${stderr}" >"${stdout}"; then
      if ! umount "${rootMP}" 2>"${stderr}" >"${stdout}"; then
        error "Failed to unmount ${rootMP}"
      else
        rm -Rf "${rootMP}" 2>"${stderr}" >"${stdout}" 
      fi  
      rm -Rf "${dataMP}" 2>"${stderr}" >"${stdout}"
      fatal "Could not mount userdata filesystem"
    fi
  fi
  
  message "Copying files"
  if createPhase2; then
    if ! copyFiles "${rootMP}" "${dataMP}"; then
      error "failed to copy files"
    fi
  else
    error "Failed to create phase 2 script"
  fi
  
  message "Syncing disks"
  sync
  sleep 1

  if ! umount "${rootMP}" 2>"${stderr}" >"${stdout}"; then
    error "Failed to unmount ${rootMP}"
  else
    rm -Rf "${rootMP}" 2>"${stderr}" >"${stdout}" 
  fi  

  if ! umount "${dataMP}" 2>"${stderr}" >"${stdout}"; then
    error "Failed to unmount ${dataMP}"
  else
    rm -Rf "${dataMP}" 2>"${stderr}" >"${stdout}" 
  fi  
fi

message "Operation completed"
if [ -n "${installBHumanFiles}" ]; then
  message "Don't forget to run /home/root/phase2.sh on ${robotName}"
fi
