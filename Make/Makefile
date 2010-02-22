

ifeq ($(ComSpec)$(COMSPEC),)
PATH := ../Src/SimulatorQt/Util/zbuildgen/Linux/bin:$(PATH)
else
PATH := ../Src/SimulatorQt/Util/zbuildgen/Win32/bin:$(PATH)
endif



SOLUTIONDIR := $(shell pwd)
CONFIG ?= DebugWithReleaseLibs
CONFIGS := DebugWithReleaseLibs Debug Release ReleaseForProfiling Optimized 
PROJECTS := SimRobotCore SimRobotGUI Controller Simulator URC SpecialActions Behavior libbhuman Nao SimulatorDoc BehaviorDoc 
PROJECTCONFIGS := SimRobotCore_DebugWithReleaseLibs SimRobotGUI_DebugWithReleaseLibs Controller_DebugWithReleaseLibs Simulator_DebugWithReleaseLibs URC_DebugWithReleaseLibs SpecialActions_DebugWithReleaseLibs Behavior_DebugWithReleaseLibs libbhuman_DebugWithReleaseLibs Nao_DebugWithReleaseLibs SimulatorDoc_DebugWithReleaseLibs BehaviorDoc_DebugWithReleaseLibs SimRobotCore_Debug SimRobotGUI_Debug Controller_Debug Simulator_Debug URC_Debug SpecialActions_Debug Behavior_Debug libbhuman_Debug Nao_Debug SimulatorDoc_Debug BehaviorDoc_Debug SimRobotCore_Release SimRobotGUI_Release Controller_Release Simulator_Release URC_Release SpecialActions_Release Behavior_Release libbhuman_Release Nao_Release SimulatorDoc_Release BehaviorDoc_Release SimRobotCore_ReleaseForProfiling SimRobotGUI_ReleaseForProfiling Controller_ReleaseForProfiling Simulator_ReleaseForProfiling URC_ReleaseForProfiling SpecialActions_ReleaseForProfiling Behavior_ReleaseForProfiling libbhuman_ReleaseForProfiling Nao_ReleaseForProfiling SimulatorDoc_ReleaseForProfiling BehaviorDoc_ReleaseForProfiling SimRobotCore_Optimized SimRobotGUI_Optimized Controller_Optimized Simulator_Optimized URC_Optimized SpecialActions_Optimized Behavior_Optimized libbhuman_Optimized Nao_Optimized SimulatorDoc_Optimized BehaviorDoc_Optimized 
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
	-@$(ZBUILDGEN) . Behavior Documentation ../Src/SimulatorQt/Make/SimRobotCore.make.zbuild ../Src/SimulatorQt/Make/SimRobotGUI.make.zbuild ../Src/SimulatorQt/Make/SimRobotCore.vcproj.zbuild ../Src/SimulatorQt/Make/SimRobotGUI.vcproj.zbuild --platform linux > /dev/null

DebugWithReleaseLibs: SimRobotCore_DebugWithReleaseLibs SimRobotGUI_DebugWithReleaseLibs Controller_DebugWithReleaseLibs Simulator_DebugWithReleaseLibs URC_DebugWithReleaseLibs SpecialActions_DebugWithReleaseLibs Behavior_DebugWithReleaseLibs 

SimRobotCore_DebugWithReleaseLibs: prebuild 
	@$(MAKE) -j 2 --no-print-directory -C ../Src/SimulatorQt/Make/ -f SimRobotCore.make Release

SimRobotGUI_DebugWithReleaseLibs: prebuild SimRobotCore_DebugWithReleaseLibs 
	@$(MAKE) -j 2 --no-print-directory -C ../Src/SimulatorQt/Make/ -f SimRobotGUI.make Release

Controller_DebugWithReleaseLibs: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f Controller.make Release

Simulator_DebugWithReleaseLibs: prebuild SimRobotGUI_DebugWithReleaseLibs Controller_DebugWithReleaseLibs SpecialActions_DebugWithReleaseLibs Behavior_DebugWithReleaseLibs 
	@$(MAKE) -j 2 --no-print-directory  -f Simulator.make DebugWithReleaseLibs

URC_DebugWithReleaseLibs: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f URC.make Debug

SpecialActions_DebugWithReleaseLibs: prebuild URC_DebugWithReleaseLibs 
	@$(MAKE) -j 2 --no-print-directory  -f SpecialActions.make Release

Behavior_DebugWithReleaseLibs: prebuild URC_DebugWithReleaseLibs 
	@$(MAKE) -j 2 --no-print-directory  -f Behavior.make Release

Nao_DebugWithReleaseLibs: prebuild libbhuman_DebugWithReleaseLibs SpecialActions_DebugWithReleaseLibs Behavior_DebugWithReleaseLibs 
	@$(MAKE) -j 2 --no-print-directory  -f Nao.make Debug

SimulatorDoc_DebugWithReleaseLibs: prebuild 
	@$(MAKE) -j 2 --no-print-directory -C Documentation -f SimulatorDoc.make Release

BehaviorDoc_DebugWithReleaseLibs: prebuild 
	@$(MAKE) -j 1 --no-print-directory -C Documentation -f BehaviorDoc.make Release

libbhuman_DebugWithReleaseLibs: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f libbhuman.make Release

Debug: SimRobotCore_Debug SimRobotGUI_Debug Controller_Debug Simulator_Debug URC_Debug SpecialActions_Debug Behavior_Debug 

SimRobotCore_Debug: prebuild 
	@$(MAKE) -j 2 --no-print-directory -C ../Src/SimulatorQt/Make/ -f SimRobotCore.make Debug

SimRobotGUI_Debug: prebuild SimRobotCore_Debug 
	@$(MAKE) -j 2 --no-print-directory -C ../Src/SimulatorQt/Make/ -f SimRobotGUI.make Debug

Controller_Debug: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f Controller.make Debug

Simulator_Debug: prebuild SimRobotGUI_Debug Controller_Debug SpecialActions_Debug Behavior_Debug 
	@$(MAKE) -j 2 --no-print-directory  -f Simulator.make Debug

URC_Debug: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f URC.make Debug

SpecialActions_Debug: prebuild URC_Debug 
	@$(MAKE) -j 2 --no-print-directory  -f SpecialActions.make Release

Behavior_Debug: prebuild URC_Debug 
	@$(MAKE) -j 2 --no-print-directory  -f Behavior.make Release

Nao_Debug: prebuild libbhuman_Debug SpecialActions_Debug Behavior_Debug 
	@$(MAKE) -j 2 --no-print-directory  -f Nao.make Debug

SimulatorDoc_Debug: prebuild 
	@$(MAKE) -j 2 --no-print-directory -C Documentation -f SimulatorDoc.make Release

BehaviorDoc_Debug: prebuild 
	@$(MAKE) -j 1 --no-print-directory -C Documentation -f BehaviorDoc.make Release

libbhuman_Debug: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f libbhuman.make Release

Release: SimRobotCore_Release SimRobotGUI_Release Controller_Release Simulator_Release URC_Release SpecialActions_Release Behavior_Release 

SimRobotCore_Release: prebuild 
	@$(MAKE) -j 2 --no-print-directory -C ../Src/SimulatorQt/Make/ -f SimRobotCore.make Release

SimRobotGUI_Release: prebuild SimRobotCore_Release 
	@$(MAKE) -j 2 --no-print-directory -C ../Src/SimulatorQt/Make/ -f SimRobotGUI.make Release

Controller_Release: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f Controller.make Release

Simulator_Release: prebuild SimRobotGUI_Release Controller_Release SpecialActions_Release Behavior_Release 
	@$(MAKE) -j 2 --no-print-directory  -f Simulator.make Optimized

URC_Release: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f URC.make Debug

SpecialActions_Release: prebuild URC_Release 
	@$(MAKE) -j 2 --no-print-directory  -f SpecialActions.make Release

Behavior_Release: prebuild URC_Release 
	@$(MAKE) -j 2 --no-print-directory  -f Behavior.make Release

Nao_Release: prebuild libbhuman_Release SpecialActions_Release Behavior_Release 
	@$(MAKE) -j 2 --no-print-directory  -f Nao.make Release

SimulatorDoc_Release: prebuild 
	@$(MAKE) -j 2 --no-print-directory -C Documentation -f SimulatorDoc.make Release

BehaviorDoc_Release: prebuild 
	@$(MAKE) -j 1 --no-print-directory -C Documentation -f BehaviorDoc.make Release

libbhuman_Release: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f libbhuman.make Release

ReleaseForProfiling: SimRobotCore_ReleaseForProfiling SimRobotGUI_ReleaseForProfiling Controller_ReleaseForProfiling Simulator_ReleaseForProfiling URC_ReleaseForProfiling SpecialActions_ReleaseForProfiling Behavior_ReleaseForProfiling 

SimRobotCore_ReleaseForProfiling: prebuild 
	@$(MAKE) -j 2 --no-print-directory -C ../Src/SimulatorQt/Make/ -f SimRobotCore.make Release

SimRobotGUI_ReleaseForProfiling: prebuild SimRobotCore_ReleaseForProfiling 
	@$(MAKE) -j 2 --no-print-directory -C ../Src/SimulatorQt/Make/ -f SimRobotGUI.make Release

Controller_ReleaseForProfiling: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f Controller.make Release

Simulator_ReleaseForProfiling: prebuild SimRobotGUI_ReleaseForProfiling Controller_ReleaseForProfiling SpecialActions_ReleaseForProfiling Behavior_ReleaseForProfiling 
	@$(MAKE) -j 2 --no-print-directory  -f Simulator.make Optimized

URC_ReleaseForProfiling: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f URC.make Debug

SpecialActions_ReleaseForProfiling: prebuild URC_ReleaseForProfiling 
	@$(MAKE) -j 2 --no-print-directory  -f SpecialActions.make Release

Behavior_ReleaseForProfiling: prebuild URC_ReleaseForProfiling 
	@$(MAKE) -j 2 --no-print-directory  -f Behavior.make Release

Nao_ReleaseForProfiling: prebuild libbhuman_ReleaseForProfiling SpecialActions_ReleaseForProfiling Behavior_ReleaseForProfiling 
	@$(MAKE) -j 2 --no-print-directory  -f Nao.make ReleaseForProfiling

SimulatorDoc_ReleaseForProfiling: prebuild 
	@$(MAKE) -j 2 --no-print-directory -C Documentation -f SimulatorDoc.make Release

BehaviorDoc_ReleaseForProfiling: prebuild 
	@$(MAKE) -j 1 --no-print-directory -C Documentation -f BehaviorDoc.make Release

libbhuman_ReleaseForProfiling: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f libbhuman.make Release

Optimized: SimRobotCore_Optimized SimRobotGUI_Optimized Controller_Optimized Simulator_Optimized URC_Optimized SpecialActions_Optimized Behavior_Optimized 

SimRobotCore_Optimized: prebuild 
	@$(MAKE) -j 2 --no-print-directory -C ../Src/SimulatorQt/Make/ -f SimRobotCore.make Release

SimRobotGUI_Optimized: prebuild SimRobotCore_Optimized 
	@$(MAKE) -j 2 --no-print-directory -C ../Src/SimulatorQt/Make/ -f SimRobotGUI.make Release

Controller_Optimized: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f Controller.make Release

Simulator_Optimized: prebuild SimRobotGUI_Optimized Controller_Optimized SpecialActions_Optimized Behavior_Optimized 
	@$(MAKE) -j 2 --no-print-directory  -f Simulator.make Optimized

URC_Optimized: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f URC.make Debug

SpecialActions_Optimized: prebuild URC_Optimized 
	@$(MAKE) -j 2 --no-print-directory  -f SpecialActions.make Release

Behavior_Optimized: prebuild URC_Optimized 
	@$(MAKE) -j 2 --no-print-directory  -f Behavior.make Release

Nao_Optimized: prebuild libbhuman_Optimized SpecialActions_Optimized Behavior_Optimized 
	@$(MAKE) -j 2 --no-print-directory  -f Nao.make Optimized

SimulatorDoc_Optimized: prebuild 
	@$(MAKE) -j 2 --no-print-directory -C Documentation -f SimulatorDoc.make Release

BehaviorDoc_Optimized: prebuild 
	@$(MAKE) -j 1 --no-print-directory -C Documentation -f BehaviorDoc.make Release

libbhuman_Optimized: prebuild 
	@$(MAKE) -j 2 --no-print-directory  -f libbhuman.make Release


SimRobotCore: SimRobotCore_$(CONFIG)

SimRobotGUI: SimRobotGUI_$(CONFIG)

Controller: Controller_$(CONFIG)

Simulator: Simulator_$(CONFIG)

URC: URC_$(CONFIG)

SpecialActions: SpecialActions_$(CONFIG)

Behavior: Behavior_$(CONFIG)

libbhuman: libbhuman_$(CONFIG)

Nao: Nao_$(CONFIG)

SimulatorDoc: SimulatorDoc_$(CONFIG)

BehaviorDoc: BehaviorDoc_$(CONFIG)


clean: prebuild
	@echo "------ Cleaning ------"
	@$(MAKE) --no-print-directory -C ../Src/SimulatorQt/Make/ -f SimRobotCore.make clean
	@$(MAKE) --no-print-directory -C ../Src/SimulatorQt/Make/ -f SimRobotGUI.make clean
	@$(MAKE) --no-print-directory  -f Controller.make clean
	@$(MAKE) --no-print-directory  -f Simulator.make clean
	@$(MAKE) --no-print-directory  -f URC.make clean
	@$(MAKE) --no-print-directory  -f SpecialActions.make clean
	@$(MAKE) --no-print-directory  -f Behavior.make clean
	@$(MAKE) --no-print-directory  -f libbhuman.make clean
	@$(MAKE) --no-print-directory  -f Nao.make clean
	@$(MAKE) --no-print-directory -C Documentation -f SimulatorDoc.make clean
	@$(MAKE) --no-print-directory -C Documentation -f BehaviorDoc.make clean

dist: prebuild
	@$(MAKE) --no-print-directory -C ../Src/SimulatorQt/Make/ -f SimRobotCore.make dist
	@$(MAKE) --no-print-directory -C ../Src/SimulatorQt/Make/ -f SimRobotGUI.make dist
	@$(MAKE) --no-print-directory  -f Controller.make dist
	@$(MAKE) --no-print-directory  -f Simulator.make dist
	@$(MAKE) --no-print-directory  -f URC.make dist
	@$(MAKE) --no-print-directory  -f SpecialActions.make dist
	@$(MAKE) --no-print-directory  -f Behavior.make dist
	@$(MAKE) --no-print-directory  -f libbhuman.make dist
	@$(MAKE) --no-print-directory  -f Nao.make dist
	@$(MAKE) --no-print-directory -C Documentation -f SimulatorDoc.make dist
	@$(MAKE) --no-print-directory -C Documentation -f BehaviorDoc.make dist

