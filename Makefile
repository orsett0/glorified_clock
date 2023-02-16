CONFIG := default.config
include ${CONFIG}

MKDIR := mkdir -p 
RM := rm -rf 

PROJECTNAME := orologio_lidia
IMAGE_TYPE := prod
ifeq ($(IMAGE_TYPE), debug)
OUTPUT_SUFFIX := elf
else
OUTPUT_SUFFIX := hex
endif
OBJECTDIR := build/${IMAGE_TYPE}
CND_CONF := default
DISTDIR := dist/${IMAGE_TYPE}
MP_PROCESSOR_OPTION := 16F887

SOURCEFILES := $(wildcard src/*.c)
OBJECTFILES := $(patsubst %.c, $(OBJECTDIR)/%.p1, $(SOURCEFILES))

OUTFILE := ${DISTDIR}/$(PROJECTNAME).${IMAGE_TYPE}.${OUTPUT_SUFFIX}

options := -Wa,-a -mno-keep-startup -Wl -gdwarf-3 -mno-resetbits -mno-save-resetbits -O0 -fno-short-double -mno-stackcall -xassembler-with-cpp -DXPRJ_default=$(CND_CONF) -D__DEBUG=1 -mcpu=$(MP_PROCESSOR_OPTION) -mdfp="${DFP_DIR}/xc8" -msummary=-psect,-class,+mem,-hex,-file -Wl,--data-init -mno-osccal -mstack=compiled:auto:auto -fno-short-float -mdebugger=none -maddrqual=ignore -mno-download -mwarn=-3 -mdefault-config-bits -fasmfile -std=c99 -ginhx32 

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

upload-to-pic: ${DISTDIR}/$(PROJECTNAME).${IMAGE_TYPE}.${OUTPUT_SUFFIX}
	@scp -P42000 ${DISTDIR}/$(PROJECTNAME).${IMAGE_TYPE}.hex pi@$(IPPI):/tmp
	@ssh -p42000 pi@$(IPPI) /opt/pickle/p14 program /tmp/$(PROJECTNAME).${IMAGE_TYPE}.hex
	@ssh -p42000 pi@$(IPPI) /opt/pickle/p14 verify /tmp/$(PROJECTNAME).${IMAGE_TYPE}.hex
	@echo Done uploading.