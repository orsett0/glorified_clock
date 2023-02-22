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

SOURCEFILES := $(wildcard src/*.c) $(wildcard src/driver/*/*.c)
OBJECTFILES := $(patsubst %.c, $(OBJECTDIR)/%.p1, $(SOURCEFILES))

OUTFILE := ${DISTDIR}/$(PROJECTNAME).${IMAGE_TYPE}.${OUTPUT_SUFFIX}

includePath := -Isrc/driver/ -Isrc/include/

options := -D _XTAL_FREQ=8000000 $(includePath) -Wa,-a -mno-keep-startup -Wl -gdwarf-3 -mno-resetbits -mno-save-resetbits -O0 -fno-short-double -mno-stackcall -xassembler-with-cpp -DXPRJ_default=$(CND_CONF) -D__DEBUG=1 -mcpu=$(MP_PROCESSOR_OPTION) -mdfp="${DFP_DIR}/xc8" -msummary=-psect,-class,+mem,-hex,-file -Wl,--data-init -mno-osccal -mstack=compiled:auto:auto -fno-short-float -mdebugger=none -maddrqual=ignore -mno-download -mwarn=-3 -mdefault-config-bits -fasmfile -std=c99 -ginhx32 

upload-to-pic: ${DISTDIR}/$(PROJECTNAME).${IMAGE_TYPE}.${OUTPUT_SUFFIX}
	@./upload.sh ${DISTDIR}/$(PROJECTNAME).${IMAGE_TYPE}.${OUTPUT_SUFFIX} $(IPPI)
	@echo Done uploading.

clean-upload: clean upload-to-pic

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
