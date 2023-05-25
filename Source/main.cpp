#include "support/gcc8_c_support.h"
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <graphics/gfxbase.h>
#include <graphics/view.h>
#include <exec/execbase.h>
#include <graphics/gfxmacros.h>
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>

#include "FrameBuffer.h"
#include "BlitterObject.h"

#undef MUSIC

struct ExecBase *SysBase;
static volatile struct Custom *custom;
struct DosLibrary *DOSBase;
struct GfxBase *GfxBase;

// Backup
static UWORD SystemInts;
static UWORD SystemDMA;
static UWORD SystemADKCON;
static volatile APTR VBR = 0;
static APTR SystemIrq;
 
struct View *ActiView;


namespace std {
    using size_t = ULONG;
}

// C++ memory allocation for new/delete
using namespace std;

void * operator new (size_t sz) {
    void * memoryBlock = (void *) AllocMem((ULONG) sz + 4, MEMF_ANY);
    *((ULONG *) memoryBlock) = (ULONG) sz;
    return (void *) (((BYTE *) memoryBlock) + 4);
}

void * operator new [] (size_t sz) {
    void * memoryBlock = (void *) AllocMem((ULONG) sz + 4, MEMF_ANY);
    *((ULONG *) memoryBlock) = (ULONG) sz;
    return (void *) (((BYTE *) memoryBlock) + 4);
}

void operator delete (void * ptr) {
    void * memoryBlock = (void *) ((((BYTE *) ptr) - 4));
    ULONG byteSize = *((ULONG *) memoryBlock);
    FreeMem(memoryBlock, byteSize);
}

void operator delete (void * ptr, size_t sz) {
    void * memoryBlock = (void *) ((((BYTE *) ptr) - 4));
    ULONG byteSize = *((ULONG *) memoryBlock);
    FreeMem(memoryBlock, byteSize);
}


static APTR GetVBR(void)
{
	APTR vbr = 0;
	UWORD getvbr[] = { 0x4e7a, 0x0801, 0x4e73 }; // MOVEC.L VBR,D0 RTE

	if (SysBase->AttnFlags & AFF_68010) {
		vbr = (APTR)Supervisor((ULONG (*)())getvbr);
	}

	return vbr;
}

void SetInterruptHandler(APTR interrupt)
{
	*(volatile APTR*)(((UBYTE*)VBR)+0x6c) = interrupt;
}

APTR GetInterruptHandler()
{
	return *(volatile APTR*)(((UBYTE*)VBR)+0x6c);
}

// vblank begins at vpos 312 hpos 1 and ends at vpos 25 hpos 1
// vsync begins at line 2 hpos 132 and ends at vpos 5 hpos 18 
void WaitVbl()
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

void WaitLine(USHORT line)
{
	while (1) {
		volatile ULONG vpos=*(volatile ULONG*)0xDFF004;
		if (((vpos >> 8) & 511) == line) {
			break;
		}
	}
}

__attribute__((always_inline)) inline void WaitBlt()
{
	UWORD tst = *(volatile UWORD*)&custom->dmaconr; // for compatiblity a1000
	(void)tst;
	while (*(volatile UWORD*)&custom->dmaconr&(1<<14)) {} //blitter busy wait
}

void TakeSystem()
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
}

void FreeSystem()
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

__attribute__((always_inline)) inline short MouseLeft() {return !((*(volatile UBYTE*)0xbfe001)&64);}	
__attribute__((always_inline)) inline short MouseRight() {return !((*(volatile UWORD*)0xdff016)&(1<<10));}

volatile short frameCounter = 0;
INCBIN(ColorPalette, "out/Graphics/Default.PAL")
INCBIN_CHIP(coconut, "out/Graphics/coconut.BPL")
INCBIN_CHIP(splitcoconut, "out/Graphics/splitcoconut.BPL")
INCBIN_CHIP(coconuttree, "out/Graphics/coconut-tree.BPL")

// Put copper list into chip mem so we can use it without copying
const UWORD copper2[] __attribute__((section (".MEMF_CHIP"))) = {
	0xffff, 0xfffe // end copper list
};

void* doynaxdepack(const void* input, void* output) { // returns end of output data, input needs to be 16-bit aligned!
	register volatile const void* _a0 ASM("a0") = input;
	register volatile       void* _a1 ASM("a1") = output;
	__asm volatile (
		"movem.l %%d0-%%d7/%%a2-%%a6,-(%%sp)\n"
		"jsr _doynaxdepack_vasm\n"
		"movem.l (%%sp)+,%%d0-%%d7/%%a2-%%a6"
	: "+rf"(_a0), "+rf"(_a1)
	:
	: "cc", "memory");
	return (void*)_a1;
}

#ifdef MUSIC
	// Module Player - ThePlayer 6.1a: https://www.pouet.net/prod.php?which=19922
	// The Player® 6.1A: Copyright © 1992-95 Jarno Paananen
	INCBIN(player, "Sound/player610.6.no_cia.bin")
	INCBIN_CHIP(module, "Sound/coconut.p61")

	// Returns 0 if success, non-zero otherwise
	int p61Init(const void* module)
	{
		register volatile const void* _a0 ASM("a0") = module;
		register volatile const void* _a1 ASM("a1") = NULL;
		register volatile const void* _a2 ASM("a2") = NULL;
		register volatile const void* _a3 ASM("a3") = player;
		register                int   _d0 ASM("d0"); // return value
		__asm volatile (
			"movem.l %%d1-%%d7/%%a4-%%a6,-(%%sp)\n"
			"jsr 0(%%a3)\n"
			"movem.l (%%sp)+,%%d1-%%d7/%%a4-%%a6"
		: "=r" (_d0), "+rf"(_a0), "+rf"(_a1), "+rf"(_a2), "+rf"(_a3)
		:
		: "cc", "memory");
		return _d0;
	}

	void p61Music()
	{
		register volatile const void* _a3 ASM("a3") = player;
		register volatile const void* _a6 ASM("a6") = (void*)0xdff000;
		__asm volatile (
			"movem.l %%d0-%%d7/%%a0-%%a2/%%a4-%%a5,-(%%sp)\n"
			"jsr 4(%%a3)\n"
			"movem.l (%%sp)+,%%d0-%%d7/%%a0-%%a2/%%a4-%%a5"
		: "+rf"(_a3), "+rf"(_a6)
		:
		: "cc", "memory");
	}

	void p61End()
	{
		register volatile const void* _a3 ASM("a3") = player;
		register volatile const void* _a6 ASM("a6") = (void*)0xdff000;
		__asm volatile (
			"movem.l %%d0-%%d1/%%a0-%%a1,-(%%sp)\n"
			"jsr 8(%%a3)\n"
			"movem.l (%%sp)+,%%d0-%%d1/%%a0-%%a1"
		: "+rf"(_a3), "+rf"(_a6)
		:
		: "cc", "memory");
	}
#endif //MUSIC

__attribute__((always_inline)) inline USHORT* copSetPlanes(UBYTE bplPtrStart, USHORT* copListEnd, const UBYTE **planes, int numPlanes)
{
	for (USHORT i = 0; i < numPlanes; i++) {
		ULONG addr = (ULONG)planes[i];
		*copListEnd++ = offsetof(struct Custom, bplpt[0]) + (i + bplPtrStart) * sizeof(APTR);
		*copListEnd++ = (UWORD)(addr>>16);
		*copListEnd++ = offsetof(struct Custom, bplpt[0]) + (i + bplPtrStart) * sizeof(APTR) + 2;
		*copListEnd++ = (UWORD)addr;
	}
	return copListEnd;
}

__attribute__((always_inline)) inline USHORT* copWaitXY(USHORT *copListEnd, USHORT x, USHORT i)
{
	*copListEnd++ = (i<<8)|(x<<1)|1; // Bit 1 means wait. Waits for vertical position x<<8, first raster stop position outside the left 
	*copListEnd++ = 0xfffe;
	return copListEnd;
}

__attribute__((always_inline)) inline USHORT* copWaitY(USHORT* copListEnd, USHORT i)
{
	*copListEnd++ = (i<<8)|4|1;	// Bit 1 means wait. Waits for vertical position x<<8, first raster stop position outside the left 
	*copListEnd++ = 0xfffe;
	return copListEnd;
}

__attribute__((always_inline)) inline USHORT* copSetColor(USHORT* copListCurrent, USHORT index, USHORT color) {
	*copListCurrent++ = offsetof(struct Custom, color) + index * 2;
	*copListCurrent++ = color;
	return copListCurrent;
}

static const UBYTE sinus40[] = {
	20,22,24,26,28,30,31,33,
	34,36,37,38,39,39,40,40,
	40,40,39,39,38,37,36,35,
	34,32,30,29,27,25,23,21,
	19,17,15,13,11,10,8,6,
	5,4,3,2,1,1,0,0,
	0,0,1,1,2,3,4,6,
	7,9,10,12,14,16,18,20,
};

static __attribute__((interrupt)) void interruptHandler()
{
	custom->intreq = (1<<INTB_VERTB);
	custom->intreq = (1<<INTB_VERTB); // Reset vbl req. twice for a4000 bug.

#ifdef MUSIC
	p61Music();
#endif
	frameCounter += 1;
}

// Set up a 320x256 lowres display
__attribute__((always_inline)) inline USHORT* screenScanDefault(USHORT* copListEnd)
{
	const USHORT x = 129;
	const USHORT width = 320;
	const USHORT height = 256;
	const USHORT y = 44;
	const USHORT RES = 8; // 8 = lowres, 4 = hires
	USHORT xstop = x + width;
	USHORT ystop = y + height;
	USHORT fw = (x>>1) - RES;

	*copListEnd++ = offsetof(struct Custom, ddfstrt);
	*copListEnd++ = fw;
	*copListEnd++ = offsetof(struct Custom, ddfstop);
	*copListEnd++ = fw+(((width>>4)-1)<<3);
	*copListEnd++ = offsetof(struct Custom, diwstrt);
	*copListEnd++ = x+(y<<8);
	*copListEnd++ = offsetof(struct Custom, diwstop);
	*copListEnd++ = (xstop-256)+((ystop-256)<<8);
	return copListEnd;
}

int main()
{
	SysBase = *((struct ExecBase**)4UL);
	custom = (struct Custom*)0xdff000;

	// We will use the graphics library only to locate and restore the system copper list once we are through.
	GfxBase = (struct GfxBase *)OpenLibrary((CONST_STRPTR)"graphics.library",0);
	if (!GfxBase) {
		Exit(0);
	}

	// Used for printing
	DOSBase = (struct DosLibrary*)OpenLibrary((CONST_STRPTR)"dos.library", 0);
	if (!DOSBase)
		Exit(0);

	KPrintF("Hello debugger from Amiga!\n");
	Write(Output(), (APTR)"Hello console!\n", 15);
	Delay(50);

	warpmode(1);

#ifdef MUSIC
	if (p61Init(module) != 0) {
		KPrintF("p61Init failed!\n");
	}
#endif
	warpmode(0);

	TakeSystem();
	WaitVbl();

	const unsigned short planeCount = 5;

	USHORT* copper1 = (USHORT*)AllocMem(1024, MEMF_CHIP);
	USHORT* copPtr = copper1;
	USHORT* pCopFrameBuffer;

	FrameBuffer frameBuffer1("FrameBuffer1", 320, 256, planeCount);
	FrameBuffer frameBuffer2("FrameBuffer2", 320, 256, planeCount);
	FrameBuffer* frontBuffer = &frameBuffer1;
	FrameBuffer* backBuffer = &frameBuffer2;

	// Register graphics resources with WinUAE for nicer gfx debugger experience
	debug_register_bitmap(coconut, "coconut.BPL", 32, 32, 5, debug_resource_bitmap_interleaved | debug_resource_bitmap_masked);
	debug_register_bitmap(splitcoconut, "splitcoconut.BPL", 32, 32, 5, debug_resource_bitmap_interleaved | debug_resource_bitmap_masked);
	debug_register_bitmap(coconuttree, "coconut-tree.BPL", 32, 64, 5, debug_resource_bitmap_interleaved | debug_resource_bitmap_masked);
	debug_register_palette(ColorPalette, "Default.PAL", 1, 0);
	debug_register_copperlist(copper1, "copper1", 1024, 0);
	debug_register_copperlist(copper2, "copper2", sizeof(copper2), 0);

	copPtr = screenScanDefault(copPtr);

	// Enable bitplanes
	*copPtr++ = offsetof(struct Custom, bplcon0);
	*copPtr++ = (0<<10)/*dual pf*/|(1<<9)/*color*/|((5)<<12)/*num bitplanes*/;
	*copPtr++ = offsetof(struct Custom, bplcon1);
	*copPtr++ = 0;
	*copPtr++ = offsetof(struct Custom, bplcon2); // Playfied priority
	*copPtr++ = 1<<6; //0x24; // Sprites have priority over playfields

	const USHORT lineSize = 320/8;

	// Set bitplane modulo
	*copPtr++ = offsetof(struct Custom, bpl1mod); // Odd planes 1,3,5
	*copPtr++ = 4 * lineSize;
	*copPtr++ = offsetof(struct Custom, bpl2mod); // Even planes 2,4
	*copPtr++ = 4 * lineSize;

	// Set bitplane pointers
	pCopFrameBuffer = copPtr;
	const UBYTE* planes[planeCount];
	for (int a = 0; a < planeCount; a++) {
		planes[a] = (UBYTE*)(frontBuffer->m_pBufferDataPlane[a]);
	}
	copPtr = copSetPlanes(0, copPtr, planes, planeCount);

	// Set color palette
	for (int a = 0; a < 32; a++) {
		copPtr = copSetColor(copPtr, a, ((USHORT*)ColorPalette)[a]);
	}

	// jump to copper2
	*copPtr++ = offsetof(struct Custom, copjmp2);
	*copPtr++ = 0x7fff;

	custom->cop1lc = (ULONG)copper1;
	custom->cop2lc = (ULONG)copper2;
	custom->dmacon = DMAF_BLITTER; // Disable blitter dma for copjmp bug
	custom->copjmp1 = 0x7fff; // Start coppper
	custom->dmacon = DMAF_SETCLR | DMAF_MASTER | DMAF_RASTER | DMAF_COPPER | DMAF_BLITTER;

	SetInterruptHandler((APTR)interruptHandler);
#ifdef MUSIC
	custom->intena = INTF_SETCLR | INTF_EXTER; // ThePlayer needs INTF_EXTER
#else
	custom->intena = INTF_SETCLR | INTF_INTEN | INTF_VERTB;
#endif

	custom->intreq = (1<<INTB_VERTB); // Reset vbl req

	frameBuffer1.Clear();
	frameBuffer2.Clear();

	auto BOtree = BlitterObject(coconuttree, 32, 64, 5);
	auto BOcoconut = BlitterObject(coconut, 32, 32, 5);
	auto BOsplitcoconut = BlitterObject(splitcoconut, 32, 32, 5);

	short coconutPosX = 35;

	while (!MouseLeft()) {
		WaitLine(300);

		// Restore the backbuffer from previously drawn bobs
		backBuffer->ClearDirtyRegions();


		// (crude) animation

		BOtree.DrawObject(backBuffer, 30, 100);

		const short noofrebounds = 5;
		const short xmax = 200;
		auto sin40 = [](short x){ return sinus40[(x+sizeof(sinus40)/4)%sizeof(sinus40)];};

		if (coconutPosX > 200) {
			coconutPosX = 35;
		}

		const auto x = coconutPosX;
		const auto y = 100 + sin40(noofrebounds * coconutPosX);

		if (frameCounter % 3 == 0) {
			coconutPosX += 1;
		}

		if (coconutPosX < 100) {
			BOcoconut.DrawObject(backBuffer, x, y);
		} else {
			BOsplitcoconut.DrawObject(backBuffer, x, y);
		}


		// Swap frame buffers for double buffering
		FrameBuffer* tmp = frontBuffer;
		frontBuffer = backBuffer;
		backBuffer = tmp;

		// Set display to front buffer
		for (int a = 0; a < frontBuffer->m_bitPlaneCount; a++) {
			planes[a] = (UBYTE*)(frontBuffer->m_pBufferDataPlane[a]);
		}
		copSetPlanes(0, pCopFrameBuffer, planes, frontBuffer->m_bitPlaneCount);

		// WinUAE debug overlay test
		/*
		int f = frameCounter & 255;
		debug_clear();
		debug_filled_rect(f + 100, 200*2, f + 400, 220*2, 0x0000ff00); // 0x00RRGGBB
		debug_rect(f + 90, 190*2, f + 400, 220*2, 0x000000ff); // 0x00RRGGBB
		debug_text(f+ 130, 209*2, "This is a WinUAE debug overlay", 0x00ff00ff);
		*/
	}

#ifdef MUSIC
	p61End();
#endif

	// END
	FreeSystem();

	CloseLibrary((struct Library*)DOSBase);
	CloseLibrary((struct Library*)GfxBase);
}
