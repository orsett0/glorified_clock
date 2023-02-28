CONFIG := default.config
include ${CONFIG}

MKDIR := mkdir -p 
RM := rm -rf 

BUILDDIR := build/
OUTFILE := $(BUILDDIR)/orologio_lidia
CND_CONF := default

MP_PROCESSOR_OPTION := 16F887

OBJECTFILES := $(patsubst %.c, $(BUILDDIR)/%.p1, $(wildcard src/*.c) $(wildcard src/driver/*/*.c))

includePath := -Isrc/driver/ -Isrc/include/

options := -D _XTAL_FREQ=8000000 -D SEC_SINCE_EPOCH=$(shell date +"%s") $(includePath) -Wa,-a -mno-keep-startup -Wl -gdwarf-3 -mno-resetbits -mno-save-resetbits -O0 -fno-short-double -mno-stackcall -xassembler-with-cpp -DXPRJ_default=$(CND_CONF) -D__DEBUG=1 -mcpu=$(MP_PROCESSOR_OPTION) -mdfp="${DFP_DIR}/xc8" -msummary=-psect,-class,+mem,-hex,-file -Wl,--data-init -mno-osccal -mstack=compiled:auto:auto -fno-short-float -mdebugger=none -maddrqual=ignore -mno-download -mwarn=-3 -mdefault-config-bits -fasmfile -std=c99 -ginhx32 

default: upload-main
test: upload-test

upload%: $(OUTFILE)%
	@./upload.sh $^.hex $(IPPI)

clean-default: clean default
clean-test: clean test
clean:
	${RM} $(BUILDDIR)

$(OUTFILE)-main: ${filter-out $(BUILDDIR)/src/test.p1, $(OBJECTFILES)}
	${MP_CC} $(options) -Wl,--defsym=__MPLAB_BUILD=1 -o $@ $^

$(OUTFILE)-test: ${filter-out $(BUILDDIR)/src/main.p1, $(OBJECTFILES)}
	@echo linking in $@
	${MP_CC} $(options) -Wl,--defsym=__MPLAB_BUILD=1 -o $@ $^

$(OBJECTFILES):
	@${MKDIR} ${shell dirname $@}
	${MP_CC} $(options) -c -o $@ $(patsubst $(BUILDDIR)/%.p1, %.c, $@)