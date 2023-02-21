MKDIR := mkdir -p 
RM := rm -rf 

PROJECTNAME := prova
IMAGE_TYPE := debug
ifeq ($(IMAGE_TYPE), debug)
OUTPUT_SUFFIX := elf
else
OUTPUT_SUFFIX := hex
endif
OBJECTDIR := build/${IMAGE_TYPE}
CND_CONF := default
DISTDIR := dist/${IMAGE_TYPE}

MP_PROCESSOR_OPTION := 16F887

SOURCEFILES := $(wildcard src/*.c) $(wildcard src/driver/*/*.c)
OBJECTFILES := $(patsubst %.c, $(OBJECTDIR)/%.p1, $(SOURCEFILES))

OUTFILE := ${DISTDIR}/$(PROJECTNAME).${IMAGE_TYPE}.${OUTPUT_SUFFIX}

includePath := -Isrc/driver/ -Isrc/include/
MP_CC := "/opt/microchip/xc8/v2.36/bin/xc8-cc"
DFP_DIR := /opt/microchip/mplabx/v6.00/packs/Microchip/PIC16Fxxx_DFP/1.3.42

options := -D _XTAL_FREQ=8000000 $(includePath) -Wa,-a -mno-keep-startup -Wl -gdwarf-3 -mno-resetbits -mno-save-resetbits -O0 -fno-short-double -mno-stackcall -xassembler-with-cpp -DXPRJ_default=$(CND_CONF) -D__DEBUG=1 -mcpu=$(MP_PROCESSOR_OPTION) -mdfp="${DFP_DIR}/xc8" -msummary=-psect,-class,+mem,-hex,-file -Wl,--data-init -mno-osccal -mstack=compiled:auto:auto -fno-short-float -mdebugger=none -maddrqual=ignore -mno-download -mwarn=-3 -mdefault-config-bits -fasmfile -std=c99 -ginhx32 

IPPI := "192.168.1.10"

upload-to-pic: ${DISTDIR}/$(PROJECTNAME).${IMAGE_TYPE}.${OUTPUT_SUFFIX}
	@scp -P42000 ${DISTDIR}/$(PROJECTNAME).${IMAGE_TYPE}.hex pi@$(IPPI):/tmp
	@ssh -p42000 pi@$(IPPI) /opt/pickle/p14 program /tmp/$(PROJECTNAME).${IMAGE_TYPE}.hex
	@ssh -p42000 pi@$(IPPI) /opt/pickle/p14 verify /tmp/$(PROJECTNAME).${IMAGE_TYPE}.hex
	@echo Done uploading.

${DISTDIR}/$(PROJECTNAME).${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}
	@${MKDIR} ${DISTDIR}
	${MP_CC} $(options) -Wl,--defsym=__MPLAB_BUILD=1 -o $@ ${OBJECTFILES}
	@echo Done linking.

${OBJECTFILES}: $(SOURCEFILES)
	@${MKDIR} ${shell dirname $@}
	${MP_CC} $(options) -c -o $@ $(patsubst $(OBJECTDIR)/%.p1, %.c, $@)
	@echo Done compiling.

clean:
	${RM} $(shell dirname $(DISTDIR)) $(shell dirname $(OBJECTDIR))
	@echo Done cleaning.
