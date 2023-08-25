# to generate assembler listing with LTO, add to LDFLAGS: -Wa,-adhln=$@.listing,--listing-rhs-width=200
# for better annotations add -dA -dP
# to generate assembler source with LTO, add to LDFLAGS: -save-temps=cwd

ifdef OS
	WINDOWS = 1
	SHELL = cmd.exe
endif

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

subdirs := $(wildcard Source/*)
VPATH = $(subdirs)
cpp_sources := $(call rwildcard,Source/,*.cpp)
cpp_objects := $(addprefix obj/,$(patsubst %.cpp,%.o,$(cpp_sources)))
c_sources := $(call rwildcard,Source/,*.c)
c_objects := $(addprefix obj/,$(patsubst %.c,%.o,$(c_sources)))
s_sources := $(call rwildcard,Source/,*.s)
s_objects := $(addprefix obj/,$(patsubst %.s,%.o,$(s_sources)))
vasm_sources := $(call rwildcard,Source/,*.asm)
vasm_objects := $(addprefix obj/, $(patsubst %.asm,%.o,$(vasm_sources)))
objects := $(cpp_objects) $(c_objects) $(s_objects) $(vasm_objects)

program = out/a
OUT = $(program)
CC = m68k-amiga-elf-gcc
VASM = vasmm68k_mot
KINGCON = Graphics/kingcon.exe

ifdef WINDOWS
	SDKDIR = $(abspath $(dir $(shell where $(CC)))..\m68k-amiga-elf\sys-include)
else
	SDKDIR = $(abspath $(dir $(shell which $(CC)))../m68k-amiga-elf/sys-include)
endif

CCFLAGS = -g -MP -MMD -m68000 -O0 -nostdlib -Wextra -Wno-unused-function -Wno-volatile-register-var -fomit-frame-pointer -fno-tree-loop-distribution -flto -fwhole-program -fno-exceptions
CPPFLAGS= $(CCFLAGS) -std=c++20 -fmodules-ts -fno-rtti -fcoroutines -fno-use-cxa-atexit
ASFLAGS = -Wa,-g,--register-prefix-optional,-I$(SDKDIR),-D
LDFLAGS = -Wl,--emit-relocs,-Ttext=0,-Map=$(OUT).map
VASMFLAGS = -m68000 -Felf -opt-fconst -nowarn=62 -dwarf=3 -quiet -x -I. -I$(SDKDIR)
ASSET_BITMAPS = \
	out/Graphics/Default.BPL \
	out/Graphics/coconut.BPL \
	out/Graphics/splitcoconut.BPL \
	out/Graphics/coconut-tree.BPL
SOUND_ASSETS = Sound/coconut.lsbank Sound/coconut.lsmusic

all: $(OUT).exe

.PHONY: bitmap_assets
bitmap_assets: out/graphics $(ASSET_BITMAPS)

out/graphics:
	-if not exist "$@" mkdir "$@"

.PHONY: sound_assets
sound_assets: $(SOUND_ASSETS)
# NOP, placeholder for future operations

out/Graphics/Default.BPL: Graphics/Default.png
	$(KINGCON) $^ $(basename $@) -Interleaved -Format=5 -RawPalette

out/%.BPL: %.png
	$(KINGCON) $^ $(basename $@) -Interleaved -Format=5 -Mask

$(OUT).exe: bitmap_assets sound_assets $(OUT).elf
	$(info Elf2Hunk $(program).exe)
	@elf2hunk $(OUT).elf $(OUT).exe -s

$(OUT).elf: $(objects)
	if not exist "$(@D)" mkdir "$(@D)"
	$(info Linking $(program).elf)
	@$(CC) $(CCFLAGS) $(LDFLAGS) $(objects) -o $@
	@m68k-amiga-elf-objdump --disassemble --no-show-raw-ins --visualize-jumps -S $@ >$(OUT).s

clean:
	$(info Cleaning...)
ifdef WINDOWS
	@rmdir /q /s obj out gcm.cache
else
	@$(RM) obj/* out/* gcm.cache/*
endif

-include $(objects:.o=.d)

$(cpp_objects) : obj/%.o : %.cpp
	$(info Compiling $<)
	if not exist "$(@D)" mkdir "$(@D)"
	@$(CC) $(CPPFLAGS) -c -o $@ $(CURDIR)/$<

$(c_objects) : obj/%.o : %.c
	$(info Compiling $<)
	if not exist "$(@D)" mkdir "$(@D)"
	@$(CC) $(CCFLAGS) -c -o $@ $<

$(s_objects): obj/%.o : %.s
	$(info Assembling $<)
	if not exist "$(@D)" mkdir "$(@D)"
	@$(CC) $(CCFLAGS) $(ASFLAGS) -c -o $@ $<

$(vasm_objects): obj/%.o : %.asm
	$(info Assembling $<)
	if not exist "$(@D)" mkdir "$(@D)"
	@$(VASM) $(VASMFLAGS) -o $@ $<

