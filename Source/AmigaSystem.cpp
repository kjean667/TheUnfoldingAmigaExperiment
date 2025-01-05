#include "AmigaSystem.hpp"

#include "support/gcc8_c_support.h"
#include <proto/dos.h>
#include <proto/graphics.h>
#include <exec/execbase.h>
#include <graphics/gfxmacros.h>

struct ExecBase *SysBase {*((struct ExecBase**)4UL)};
struct DosLibrary *DOSBase;
struct GfxBase *GfxBase;

AmigaSystem::AmigaSystem()
: VBR{GetVBR()}
, custom(GetCustom())
{
    // We will use the graphics library only to locate and restore the system copper list once we are through.
	GfxBase = (struct GfxBase *)OpenLibrary((CONST_STRPTR)"graphics.library",0);
	if (!GfxBase) {
		Exit(0);
	}

	// Used for printing
	DOSBase = (struct DosLibrary*)OpenLibrary((CONST_STRPTR)"dos.library", 0);
	if (!DOSBase) {
		Exit(0);
	}

    TakeSystem();
}

AmigaSystem::~AmigaSystem()
{
    FreeSystem();
    CloseLibrary((struct Library*)DOSBase);
	CloseLibrary((struct Library*)GfxBase);
}

APTR AmigaSystem::GetVBR(void)
{
	APTR vbr = 0;
	UWORD getvbr[] = { 0x4e7a, 0x0801, 0x4e73 }; // MOVEC.L VBR,D0 RTE

	if (SysBase->AttnFlags & AFF_68010) {
		vbr = (APTR)Supervisor((ULONG (*)())getvbr);
	}

	return vbr;
}

void AmigaSystem::SetInterruptHandler(APTR interrupt)
{
	*(volatile APTR*)(((UBYTE*)VBR)+0x6c) = interrupt;
}

APTR AmigaSystem::GetInterruptHandler()
{
	return *(volatile APTR*)(((UBYTE*)VBR)+0x6c);
}

// vblank begins at vpos 312 hpos 1 and ends at vpos 25 hpos 1
// vsync begins at line 2 hpos 132 and ends at vpos 5 hpos 18 
void AmigaSystem::WaitVbl()
{
	debug_start_idle();
	while (1) {
		volatile ULONG vpos = *(volatile ULONG*)0xDFF004;
		vpos &= 0x1ff00;
		if (vpos != (311<<8)) {
			break;
		}
	}
	while (1) {
		volatile ULONG vpos = *(volatile ULONG*)0xDFF004;
		vpos &= 0x1ff00;
		if (vpos == (311<<8)) {
			break;
		}
	}
	debug_stop_idle();
}

void AmigaSystem::WaitLine(USHORT line)
{
	while (1) {
		volatile ULONG vpos=*(volatile ULONG*)0xDFF004;
		if (((vpos >> 8) & 511) == line) {
			break;
		}
	}
}

void AmigaSystem::TakeSystem()
{
	Forbid();

	// Save current interrupts and DMA settings so we can restore them upon exit. 
	SystemADKCON = custom->adkconr;
	SystemInts = custom->intenar;
	SystemDMA = custom->dmaconr;
	ActiView = GfxBase->ActiView; //store current view

	LoadView(0);
	WaitTOF();
	WaitTOF();

	WaitVbl();
	WaitVbl();

	OwnBlitter();
	WaitBlit();	
	Disable();
	
	custom->intena = 0x7fff; // Disable all interrupts
	custom->intreq = 0x7fff; // Clear any interrupts that were pending
	custom->dmacon = 0x7fff; // Clear all DMA channels

	// Set all colors black
	for (int a = 0; a < 32; a++) {
		custom->color[a] = 0;
	}

	WaitVbl();
	WaitVbl();

	VBR = GetVBR();
	SystemIrq = GetInterruptHandler(); // Store interrupt register

	WaitVbl();
}

void AmigaSystem::FreeSystem()
{
	WaitVbl();
	WaitBlit();

	custom->intena = 0x7fff; // Disable all interrupts
	custom->intreq = 0x7fff; // Clear any interrupts that were pending
	custom->dmacon = 0x7fff; // Clear all DMA channels

	// Restore interrupts
	SetInterruptHandler(SystemIrq);

	// Restore system copper lists.
	custom->cop1lc = (ULONG)GfxBase->copinit;
	custom->cop2lc = (ULONG)GfxBase->LOFlist;
	custom->copjmp1 = 0x7fff; // Start coppper

	// Restore all interrupts and DMA settings.
	custom->intena  = SystemInts | 0x8000;
	custom->dmacon = SystemDMA | 0x8000;
	custom->adkcon = SystemADKCON | 0x8000;

	WaitBlit();	
	DisownBlitter();
	Enable();

	LoadView(ActiView);
	WaitTOF();
	WaitTOF();

	Permit();
}
