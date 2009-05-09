# -----------------------------
# Makefile for Robot Odyssey DS
# -----------------------------

############################################
# Tools and Directories
#

.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

# Python executable
PYTHON      := python

TARGET      := robot-odyssey-ds
BUILDDIR    := build
SOURCEDIR   := source
SCRIPTDIR   := scripts
INCLUDEDIR  := include
DATADIR     := data
TOPDIR      := $(shell pwd)

LIBDIRS     := $(LIBNDS)

# NDS file banner
BANNER_ICON := $(DATADIR)/icon.bmp
BANNER_TEXT := "Robot Odyssey DS"


############################################
# Source Files.
#
# There are separate lists for normal ARM9
# sources, ARM9 sources that run out of ITCM
# memory, SBT86 scripts, and ARM7 sources.
#

SOURCES_A9   := main.cpp sbtProcess.cpp roData.cpp \
                hwCommon.cpp hwMain.cpp hwSub.cpp hwSpriteScraper.cpp \
                mSprite.cpp textRenderer.cpp spriteDraw.cpp \
                uiBase.cpp uiEffects.cpp uiSubScreen.cpp

SOURCES_ITCM := videoConvert.cpp
SOURCES_A7   := arm7.cpp soundEngine.cpp
SOURCES_BT   := bt_lab.py bt_menu.py bt_game.py bt_tutorial.py bt_renderer.py

SOURCES_GRIT := gfx_background.grit gfx_button_remote.grit \
                gfx_button_toolbox.grit gfx_button_solder.grit \
                gfx_battery.grit


############################################
# Build flags.
#
# We use separate build flags for normal ARM9
# sources, sources produced by SBT86, and ARM7
# sources.
#
# These are the normal ARM7/ARM9 build flags for
# producing Nintendo DS binaries, plus specific
# flags that are intended to help optimize the
# output of SBT86.
#
# XXX: I also want to use -frtl-abstract-sequences
#      in CFLAGS_BT, but it's currently causing
#      an internal gcc error. Oops.
#

LIB_LDFLAGS      := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)
LIB_CFLAGS       := $(foreach dir,$(LIBDIRS),-I$(dir)/include)

CFLAGS_COMMON    := -g -Werror -fomit-frame-pointer -ffast-math \
                    -I$(INCLUDEDIR) -I$(BUILDDIR) $(LIB_CFLAGS) \
                    -fno-rtti -fno-exceptions

ARCH_A9          := -mthumb -mthumb-interwork
ARM_A9           := -march=armv5te -mtune=arm946e-s -DARM9
CFLAGS_COMMON_A9 := $(CFLAGS_COMMON) $(ARCH_A9) $(ARM_A9)
CFLAGS_A9        := $(CFLAGS_COMMON_A9) -Wall -O2
LDFLAGS_A9       := -specs=ds_arm9.specs $(ARCH_A9) $(LIB_LDFLAGS)
LIBS_A9          := -lnds9

ARCH_ITCM        := -marm -mlong-calls
CFLAGS_ITCM      := $(CFLAGS_COMMON) $(ARCH_ITCM) \
                    $(ARM_A9) -Wall -O3

CFLAGS_BT        := $(CFLAGS_COMMON_A9) -Os -fweb

ARCH_A7          := -mthumb-interwork
CFLAGS_COMMON_A7 := $(CFLAGS_COMMON) $(ARCH_A7) -mcpu=arm7tdmi -mtune=arm7tdmi
CFLAGS_A7        := $(CFLAGS_COMMON_A7) -Wall -O2 -DARM7
LDFLAGS_A7       := -specs=ds_arm7.specs $(ARCH_A7) $(LIB_LDFLAGS)
LIBS_A7          := -lnds7

############################################
# Local Variables
#

# Binary names
NDSFILE  := $(TARGET).nds
ARM7BIN  := $(BUILDDIR)/$(TARGET).arm7
ARM7ELF  := $(BUILDDIR)/$(TARGET).arm7.elf
ARM9BIN  := $(BUILDDIR)/$(TARGET).arm9
ARM9ELF  := $(BUILDDIR)/$(TARGET).arm9.elf

# SBT86 sources
GENERATED_BT := $(addprefix $(BUILDDIR)/,$(subst .py,.cpp,$(SOURCES_BT)))
OBJS_BT      := $(subst .cpp,.o,$(GENERATED_BT))
SBT86_PYDEPS := $(SCRIPTDIR)/sbt86.py $(SCRIPTDIR)/bt_common.py $(BUILDDIR)/original
SBT86_CDEPS  := $(INCLUDEDIR)/sbt86.h

# Other sources
OBJS_A7      := $(addprefix $(BUILDDIR)/,$(subst .cpp,.o,$(SOURCES_A7)))
OBJS_A9      := $(addprefix $(BUILDDIR)/,$(subst .cpp,.o,$(SOURCES_A9)))
OBJS_ITCM    := $(addprefix $(BUILDDIR)/,$(subst .cpp,.o,$(SOURCES_ITCM)))

# We have a GBFS filesystem to hold all game datafiles
GBFS_ROOT    := $(BUILDDIR)/fs
GBFS_FILE    := $(BUILDDIR)/data.gbfs
GBFS_OBJ     := $(BUILDDIR)/data.gbfs.o

# GRIT output files, in our build directory
GRIT_ASM     := $(addprefix $(BUILDDIR)/,$(subst .grit,.s,$(SOURCES_GRIT)))
GRIT_OBJS    := $(addprefix $(BUILDDIR)/,$(subst .grit,.o,$(SOURCES_GRIT)))

# Default dependencies
# (Must include GRIT_OBJS, to get grit-generated header files.)
CDEPS := $(addprefix $(INCLUDEDIR)/,$(notdir $(wildcard $(INCLUDEDIR)/*.h))) $(GRIT_OBJS)

# All ARM9 objects, including non-normal files like GBFS and SBT86 output.
OBJS_A9_ALL  := $(OBJS_A9) $(OBJS_BT) $(OBJS_ITCM) $(GBFS_OBJ) $(GRIT_OBJS)

############################################
# Build targets.
#
# There are separate compile targets for normal ARM9 code,
# binary translated ARM9 code, and ARM7 code.
#

.PHONY: all
all: $(NDSFILE)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)/* $(NDSFILE)

# Simple size profiler
.PHONY: sizeprof
sizeprof: $(ARM9ELF)
	@arm-eabi-nm --size-sort -S $< | egrep -v " [bBsS] "

$(NDSFILE): $(ARM7BIN) $(ARM9BIN)
	@echo "[NDSTOOL]" $@
	@ndstool -c $(NDSFILE) -7 $(ARM7BIN) -9 $(ARM9BIN) \
		-o $(BANNER_ICON) -b $(BANNER_ICON) $(BANNER_TEXT)

$(ARM7BIN): $(ARM7ELF)
	@echo "[OBJCOPY]" $@
	@$(OBJCOPY) -O binary $< $@

$(ARM9BIN): $(ARM9ELF)
	@echo "[OBJCOPY]" $@
	@$(OBJCOPY) -O binary $< $@

$(ARM7ELF): $(OBJS_A7)
	@echo "[LD-ARM7]" $@
	@$(CXX) $(LDFLAGS_A7) $(OBJS_A7) $(LIBS_A7) -o $@

$(ARM9ELF): $(OBJS_A9_ALL)
	@echo "[LD-ARM9]" $@
	@$(CXX) $(LDFLAGS_A9) $(OBJS_A9_ALL) $(LIBS_A9) -o $@

$(OBJS_A7): $(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp $(CDEPS)
	@echo "[CC-ARM7]" $@
	@$(CXX) $(CFLAGS_A7) -c -o $@ $<

$(OBJS_A9): $(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp $(CDEPS)
	@echo "[CC-ARM9]" $@
	@$(CXX) $(CFLAGS_A9) -c -o $@ $<

$(OBJS_ITCM): $(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp $(CDEPS)
	@echo "[CC-ITCM]" $@
	@$(CXX) $(CFLAGS_ITCM) -c -o $@ $<

$(OBJS_BT): $(BUILDDIR)/%.o: $(BUILDDIR)/%.cpp $(SBT86_CDEPS)
	@echo "[CC-SBT ]" $@
	@$(CXX) $(CFLAGS_BT) -c -o $@ $<

$(GENERATED_BT): $(BUILDDIR)/%.cpp: $(SCRIPTDIR)/%.py $(SBT86_PYDEPS)
	@echo "[SBT86  ]" $@
	@$(PYTHON) $<

# Pseudo-target to clean up and extract original Robot Odyssey data
$(BUILDDIR)/original:
	@echo "[UNPACK ] Unpacking original Robot Odyssey data..."
	@$(PYTHON) $(SCRIPTDIR)/check-originals.py
	@touch $@

# Pseudo-target to package up the game data files with GBFS
$(GBFS_FILE): $(BUILDDIR)/original $(GRIT_OBJS)
	@echo "[GBFS   ] Packing game data files"
	@cd $(GBFS_ROOT); gbfs $(TOPDIR)/$(GBFS_FILE) *

$(GBFS_OBJ): $(GBFS_FILE)
	@echo "[OBJCOPY]" $@
	@$(OBJCOPY) -I binary -O elf32-littlearm -B arm \
		--rename-section .data=.rodata,readonly,data,contents,alloc \
		--redefine-sym _binary_build_data_gbfs_start=data_gbfs \
		--redefine-sym _binary_build_data_gbfs_end=data_gbfs_end \
		--redefine-sym _binary_build_data_gbfs_size=data_gbfs_size \
		$< $@

# Convert graphics with GRIT, generating assembly source which is
# compiled for ARM9.

$(GRIT_OBJS): $(BUILDDIR)/%.o: $(BUILDDIR)/%.s
	@echo "[GRIT-AS]" $@
	@$(CXX) $(CFLAGS_A9) -c -o $@ $<

$(GRIT_ASM): $(BUILDDIR)/%.s: $(DATADIR)/%.grit $(DATADIR)/%.png
	@echo "[GRIT   ]" $@
	@grit $(DATADIR)/$*.png -fts -ff $< -o $@

### The End ###
