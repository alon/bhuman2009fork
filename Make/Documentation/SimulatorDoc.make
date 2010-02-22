
ifeq ($(ComSpec)$(COMSPEC),)
DOXYGEN = doxygen
else
DOXYGEN = ../../Util/doxygen/doxygen.exe
endif

.PHONY: all Release prebuild

all: Release

prebuild:
	@echo "------ Building SimulatorDoc (Release) ------"

Release: prebuild
	-@mkdir -p ../../Doc/Reference/Simulator
	@$(DOXYGEN) Simulator_Documentation.cfg 2> .dtmp
ifeq ($(ComSpec)$(COMSPEC),)
	@cat .dtmp
else
	@cat .dtmp  \
  | sed -e "s%\(.*\):\([0-9][0-9]*\) Warning: %\1(\2) : warning: %" \
        -e "s%\(.*\):\([0-9][0-9]*\) Error: %\1(\2) : error: %"
endif
	-@rm -f .dtmp

clean:
	@echo "Cleaning SimulatorDoc..."
	-@rm -rf ../../Doc/Reference/Simulator
