#!/bin/bash

if [ -z "${gtDir:-""}" ]; then
  echo "This script is only useful when included into other scripts"
  exit 125
fi

export imageDir="${baseDir}/images/"
export fileDir="${baseDir}/files/"

export configDir="${gtDir}/Config/"
export makeDir="${gtDir}/Make/"


stdout="/dev/null"
stderr="/dev/null"

if [ "${verbosity:-""}" = "" ] ; then
  verbosity=4
fi

if [ "${verbosity}" -ge 6 ]; then
  stdout="/dev/fd/1"
  stderr="/dev/fd/2"
fi



debug() {
  if [ ${verbosity} -lt 5 ]; then
    return
  fi
  echo -n "debug  : "
  echo "$@"
}


message() {
  if [ ${verbosity} -lt 4 ]; then
    return
  fi
  echo "$@"
}


warning() {
  if [ ${verbosity} -lt 3 ]; then
    return
  fi
  echo -n "warning: "
  echo "$@"
}                                                                                                                                                                     


error() {
  echo -n "error  : "
  echo "$@"
}

fatal() {
  echo -n "fatal  : "
  echo "$@"
  exit 1
}


# checkApp
# return if $1 is found, else calls fatal
# $1: application to search for
# $2: optional comment
checkApp() {
  local app="${1:-""}"
  if ! which $app 2>"${stderr}" >"${stdout}" ; then
    if [ -z "${2:-""}" ]; then
      fatal "Can't find the command '$app' on your system!"
    else
      error "Can't find the command '$app' on your system!"
      fatal "${2}"
    fi
    return 1
  fi
}

# Check for root rights
checkRoot() {
  if [ "$(whoami)" != "root" ]; then
    fatal "you should be root to launch this program"
  fi
}

# checks for bash major version greater or equal than 4
checkBash() {
  if [ ${BASH_VERSINFO[0]} -ge 4 ]; then
    return 0
  else
    return 1
  fi
}
