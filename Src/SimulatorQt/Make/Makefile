

ifeq ($(ComSpec)$(COMSPEC),)
PATH := ../Util/zbuildgen/Linux/bin:$(PATH)
else
PATH := ../Util/zbuildgen/Win32/bin:$(PATH)
endif



SOLUTIONDIR := $(shell pwd)
CONFIG ?= Debug
CONFIGS := Debug Release 
PROJECTS := SimRobotCore SimRobotGUI SimRobotDemo 
PROJECTCONFIGS := SimRobotCore_Debug SimRobotGUI_Debug SimRobotDemo_Debug SimRobotCore_Release SimRobotGUI_Release SimRobotDemo_Release 
ZBUILDGEN ?= zbuildgen

export PATH
export SOLUTIONDIR
export CFLAGS
export CXXFLAGS
export LDFLAGS
export CC
export CXX
export AR
export ZBUILDGEN

.PHONY: all clean $(CONFIGS) $(PACKAGES) $(PACKAGECONFIGS) dist

all: $(CONFIG)

prebuild:
#	@echo "------ Regenerating Makefiles ------"
	-@$(ZBUILDGEN)  --platform linux > /dev/null

Debug: SimRobotCore_Debug SimRobotGUI_Debug SimRobotDemo_Debug 

SimRobotCore_Debug: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f SimRobotCore.make Debug

SimRobotGUI_Debug: prebuild SimRobotCore_Debug 
	@$(MAKE) -j 2 --no-print-directory  -f SimRobotGUI.make Debug

SimRobotDemo_Debug: prebuild SimRobotGUI_Debug 
	@$(MAKE) -j 2 --no-print-directory  -f SimRobotDemo.make Debug

Release: SimRobotCore_Release SimRobotGUI_Release SimRobotDemo_Release 

SimRobotCore_Release: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f SimRobotCore.make Release

SimRobotGUI_Release: prebuild SimRobotCore_Release 
	@$(MAKE) -j 2 --no-print-directory  -f SimRobotGUI.make Release

SimRobotDemo_Release: prebuild SimRobotGUI_Release 
	@$(MAKE) -j 2 --no-print-directory  -f SimRobotDemo.make Release


SimRobotCore: SimRobotCore_$(CONFIG)

SimRobotGUI: SimRobotGUI_$(CONFIG)

SimRobotDemo: SimRobotDemo_$(CONFIG)


clean: prebuild
	@echo "------ Cleaning ------"
	@$(MAKE) --no-print-directory  -f SimRobotCore.make clean
	@$(MAKE) --no-print-directory  -f SimRobotGUI.make clean
	@$(MAKE) --no-print-directory  -f SimRobotDemo.make clean

dist: prebuild
	@$(MAKE) --no-print-directory  -f SimRobotCore.make dist
	@$(MAKE) --no-print-directory  -f SimRobotGUI.make dist
	@$(MAKE) --no-print-directory  -f SimRobotDemo.make dist

