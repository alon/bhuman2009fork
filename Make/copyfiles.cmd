@echo off
pushd "%~dp0"
bash ./copyfiles.sh %*
popd 
