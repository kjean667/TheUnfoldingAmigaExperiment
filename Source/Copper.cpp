#include "Copper.h"

#include "support/gcc8_c_support.h"

#include <proto/graphics.h>

Copper::Copper(const char* name, ULONG memorySize)
: m_copperSize(memorySize)
{
	m_pCopper = (USHORT*)AllocMem(memorySize, MEMF_CHIP);
    m_pCopperWritePos = (USHORT*)m_pCopper;

	debug_register_copperlist(m_pCopper, name, memorySize, 0);
}

Copper::~Copper()
{
    FreeMem((APTR)m_pCopper, m_copperSize);
}

UWORD* Copper::GetCopperPtr()
{
    return (USHORT*)m_pCopper;
}

void Copper::WaitY(USHORT y)
{
	*m_pCopperWritePos++ = (y<<8)|6|1; // Bit 1 means wait. 6 means is an offset to make next copper instruction happend exactly at the start of the scanline, not before. @see https://jvaltane.kapsi.fi/amiga/howtocode/copper.html
	*m_pCopperWritePos++ = 0xfffe;
}

/**
 * Cross over line 255 so we can wait for lines on the lower part of the screen.
 * The Y counter will restart with 0 after this.
 */
void Copper::WaitBottomScreenPart()
{
	*m_pCopperWritePos++ = 0xffdf;
	*m_pCopperWritePos++ = 0xfffe;
}

void Copper::SetColor(USHORT index, USHORT color)
{
	*m_pCopperWritePos++ = offsetof(struct Custom, color) + index * 2;
	*m_pCopperWritePos++ = color;
}
