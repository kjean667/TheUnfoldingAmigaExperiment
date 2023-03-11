#include "BlitterObject.h"

#include <proto/graphics.h>

#include "FrameBuffer.h"

static volatile struct Custom *custom = (struct Custom*)0xdff000;

BlitterObject::BlitterObject(const void* pSourceData, UWORD width, UWORD height, UBYTE bitPlaneCount)
    : m_pSourceData(pSourceData),
      m_width(width),
      m_height(height),
      m_bitPlaneCount(bitPlaneCount)
{
}

void BlitterObject::DrawObject(FrameBuffer *pFrameBuffer, UWORD x, UWORD y)
{
    WaitBlit();

    custom->bltcon0 = 0xca | SRCA | SRCB | SRCC | DEST | ((x & 15) << ASHIFTSHIFT); // A = source, B = mask, C = background, D = destination
    custom->bltcon1 = ((x & 15) << BSHIFTSHIFT);
    custom->bltapt = (APTR)m_pSourceData;
    custom->bltamod = (m_width - 16) >> 3;
    custom->bltbpt = (UBYTE *)m_pSourceData + (m_width >> 3);
    custom->bltbmod = (m_width - 16) >> 3;
    custom->bltcpt = custom->bltdpt = (APTR)((UBYTE *)pFrameBuffer->m_pBufferData +
                                             (pFrameBuffer->m_width >> 3) * pFrameBuffer->m_bitPlaneCount * y + (x >> 3));
    custom->bltcmod = custom->bltdmod = (pFrameBuffer->m_width - m_width - 16) >> 3;
    custom->bltafwm = 0xffff;
    custom->bltalwm = 0;
    custom->bltsize = ((m_height * m_bitPlaneCount) << HSIZEBITS) | ((m_width + 16) / 16);
}
