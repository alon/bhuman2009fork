#!/bin/sh

. ./configuration.sh

rm -rf ${TEST_DIR1}
mkdir -p ${TEST_DIR1}

cp -a ${OPTIONS_DIR}/*.xml ${TEST_DIR1}
find ${TEST_DIR1} -iname "*.xml" | while read line; do echo $line; ${XABSL2XTC} ${line}; done
