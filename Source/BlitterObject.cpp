// See http://deadliners.net/BLTCONCheatSheet/index.html for calculation of minterms.

#include "BlitterObject.h"

#include <proto/graphics.h>

#include "FrameBuffer.h"

static volatile struct Custom *custom = (struct Custom*)0xdff000;

BlitterObject::BlitterObject(const void* pSourceData, UWORD width, UWORD height, UBYTE bitPlaneCount)
    : m_pSourceData(pSourceData),
      m_width(width),
      m_height(height),
      m_bitPlaneCount(bitPlaneCount),
      m_bltSize(((m_height * m_bitPlaneCount) << HSIZEBITS) | ((m_width + 16) / 16))
{
}

void BlitterObject::DrawObject(FrameBuffer *pFrameBuffer, UWORD x, UWORD y)
{
    APTR pDestinationMemory = (APTR)((UBYTE *)pFrameBuffer->m_pBufferData +
                                     (pFrameBuffer->m_width >> 3) * pFrameBuffer->m_bitPlaneCount * y + (x >> 3));
    UWORD destinationMemoryModulo = (pFrameBuffer->m_width - m_width - 16) >> 3;

    pFrameBuffer->RegisterDirtyRegion(pDestinationMemory, destinationMemoryModulo, m_bltSize);

    // Draw the image to the framebuffer
    WaitBlit();
    const UWORD minterm = 0x00ca; // B mask A | C mask !A
    custom->bltcon0 = minterm | SRCA | SRCB | SRCC | DEST | ((x & 15) << ASHIFTSHIFT); // A = mask, B = source, C = background, D = destination
    custom->bltcon1 = ((x & 15) << BSHIFTSHIFT);
    custom->bltbpt = (APTR)m_pSourceData;
    custom->bltamod = (m_width - 16) >> 3;
    custom->bltapt = (APTR)((UBYTE *)m_pSourceData + (m_width >> 3));
    custom->bltbmod = (m_width - 16) >> 3;
    custom->bltcpt = custom->bltdpt = pDestinationMemory;
    custom->bltcmod = custom->bltdmod = destinationMemoryModulo;
    custom->bltafwm = 0xffff;
    custom->bltalwm = 0;
    custom->bltsize = m_bltSize;
}

