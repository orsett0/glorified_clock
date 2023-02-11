MKDIR=mkdir -p 
RM=rm -rf 

PROJECTNAME=prova
IMAGE_TYPE=debug
ifeq ($(IMAGE_TYPE), debug)
OUTPUT_SUFFIX=elf
else
OUTPUT_SUFFIX=hex
endif
OBJECTDIR=build/${IMAGE_TYPE}
CND_CONF=default
DISTDIR=dist/${IMAGE_TYPE}
MP_PROCESSOR_OPTION=16F887

SOURCEFILES=$(wildcard src/*.c) $(wildcard driver/*/*.c)
OBJECTFILES:=$(patsubst %.c, $(OBJECTDIR)/%.p1, $(SOURCEFILES))

MP_CC="/opt/microchip/xc8/v2.36/bin/xc8-cc"
DFP_DIR=/opt/microchip/mplabx/v6.00/packs/Microchip/PIC16Fxxx_DFP/1.3.42

options := -Wa,-a -mno-keep-startup -Wl -gdwarf-3 -mno-resetbits -mno-save-resetbits -O0 -fno-short-double -mno-stackcall -xassembler-with-cpp -DXPRJ_default=$(CND_CONF) -D__DEBUG=1 -mcpu=$(MP_PROCESSOR_OPTION) -mdfp="${DFP_DIR}/xc8" -msummary=-psect,-class,+mem,-hex,-file -Wl,--data-init -mno-osccal -mstack=compiled:auto:auto -fno-short-float -mdebugger=none -maddrqual=ignore -mno-download -mwarn=-3 -mdefault-config-bits -fasmfile -std=c99 -ginhx32 

# ${DISTDIR}/prova.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES} Makefile
# 	@${MKDIR} ${DISTDIR}
# 	${MP_CC} -mcpu=$(MP_PROCESSOR_OPTION) -Wl -D__DEBUG=1 -mdebugger=none -DXPRJ_default=$(CND_CONF) -Wl,--defsym=__MPLAB_BUILD=1 -mdfp="${DFP_DIR}/xc8" -fno-short-double -fno-short-float -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -mwarn=-3 -Wa,-a -msummary=-psect,-class,+mem,-hex,-file -ginhx32 -Wl,--data-init -mno-keep-startup -mno-osccal -mno-resetbits -mno-save-resetbits -mno-download -mno-stackcall -mdefault-config-bits -std=c99 -gdwarf-3 -mstack=compiled:auto:auto -Wl,--memorysummary -o ${DISTDIR}/prova.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES}

# ${OBJECTFILES}: $(SOURCEFILES) Makefile
# 	@${MKDIR} "${OBJECTDIR}"
# 	@${RM} $@
# 	${MP_CC} -mcpu=$(MP_PROCESSOR_OPTION) -c -D__DEBUG=1 -mdebugger=none -mdfp="${DFP_DIR}/xc8" -fno-short-double -fno-short-float -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF) -msummary=-psect,-class,+mem,-hex,-file -ginhx32 -Wl,--data-init -mno-keep-startup -mno-osccal -mno-resetbits -mno-save-resetbits -mno-download -mno-stackcall -mdefault-config-bits -std=c99 -gdwarf-3 -mstack=compiled:auto:auto -o ${OBJECTDIR}/newmain.p1 newmain.c

link: ${DISTDIR}/$(PROJECTNAME).${IMAGE_TYPE}.${OUTPUT_SUFFIX}
	@echo Done linking.

compile: ${OBJECTFILES}
	@echo Done compiling.

${DISTDIR}/$(PROJECTNAME).${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}
	@${MKDIR} ${DISTDIR}
	${MP_CC} $(options) -Wl,--defsym=__MPLAB_BUILD=1 -o $@ ${OBJECTFILES}
	${RM} $(OBJECTDIR)

${OBJECTFILES}: $(SOURCEFILES)
	@${MKDIR} ${shell dirname $@}
	@${RM} $@
	${MP_CC} $(options) -c -o $@ $(patsubst $(OBJECTDIR)/%.p1, %.c, $@)