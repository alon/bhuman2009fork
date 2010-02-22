@echo off
pushd "%~dp0"
bash ./switchActiveWirelessConfig.sh %*
popd 
