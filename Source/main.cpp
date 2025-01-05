#include "AmigaSystem.hpp"
#include "FrameBuffer.h"
#include "BlitterObject.h"
#include "Copper.h"
#include "MainView.h"
#include "Music.hpp"

#include "support/gcc8_c_support.h"
#include <hardware/dmabits.h>
#include <hardware/intbits.h>

__attribute__((always_inline)) inline short MouseLeft() {return !((*(volatile UBYTE*)0xbfe001)&64);}	
__attribute__((always_inline)) inline short MouseRight() {return !((*(volatile UWORD*)0xdff016)&(1<<10));}

volatile ULONG frameCounter = 0;
INCBIN(ColorPalette, "out/Graphics/Default.PAL")
INCBIN_CHIP(coconut, "out/Graphics/coconut.BPL")
INCBIN_CHIP(splitcoconut, "out/Graphics/splitcoconut.BPL")
INCBIN_CHIP(coconuttree, "out/Graphics/coconut-tree.BPL")

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
	auto c {AmigaSystem::GetCustom()};
	c->intreq = (1<<INTB_VERTB);
	c->intreq = (1<<INTB_VERTB); // Reset vbl req. twice for a4000 bug.

	LSP_MusicPlayTick();

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
	// Take system. This must be the very first thing
	AmigaSystem amigaSystem;

	warpmode(0);

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

	copPtr = screenScanDefault(copPtr);

	// Music
	*copPtr++ = offsetof(struct Custom, dmacon);
	USHORT *dmaConPatch = copPtr;
	*copPtr++ = DMAF_SETCLR;
	LSP_MusicInit(dmaConPatch);

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

	// jump to the scene's copper
	*copPtr++ = offsetof(struct Custom, copjmp2);
	*copPtr++ = 0x7fff;

	MainView mainView;

	auto c {AmigaSystem::GetCustom()};
	c->cop1lc = (ULONG)copper1;
	c->cop2lc = (ULONG)mainView.GetCopperPtr();
	c->dmacon = DMAF_BLITTER; // Disable blitter dma for copjmp bug
	c->copjmp1 = 0x7fff; // Start copper
	c->dmacon = DMAF_SETCLR | DMAF_MASTER | DMAF_RASTER | DMAF_COPPER | DMAF_BLITTER;

	amigaSystem.SetInterruptHandler((APTR)interruptHandler);

	warpmode(0);
	
	c->intena = INTF_SETCLR | INTF_INTEN | INTF_VERTB;
    c->intreq = (1<<INTB_VERTB); // Reset vbl req

	frameBuffer1.Clear();
	frameBuffer2.Clear();

	auto BOtree = BlitterObject(coconuttree, 32, 64, 5);
	auto BOcoconut = BlitterObject(coconut, 32, 32, 5);
	auto BOsplitcoconut = BlitterObject(splitcoconut, 32, 32, 5);

	short coconutPosX = 35;

	while (!MouseLeft()) {
		amigaSystem.WaitLine(300);

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

		mainView.Update(frameCounter);

#if 0
		// WinUAE debug overlay test
		int f = frameCounter & 255;
		debug_clear();
		debug_filled_rect(f + 100, 200*2, f + 400, 220*2, 0x0000ff00); // 0x00RRGGBB
		debug_rect(f + 90, 190*2, f + 400, 220*2, 0x000000ff); // 0x00RRGGBB
		debug_text(f+ 130, 209*2, "This is a WinUAE debug overlay", 0x00ff00ff);
#endif
	}
}
