#!/bin/sh

. ./configuration.sh

rm -rf ${TEST_DIR2}
mkdir -p ${TEST_DIR2}

cp -a ${TEST_DIR1}/*.xtc ${TEST_DIR2}
${XTC2XABSL} -v ${TEST_DIR2}/*.xtc
