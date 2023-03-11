#include "BlitterObject.h"

#include <proto/graphics.h>

BlitterObject::BlitterObject(const void* pSourceData, UWORD width, UWORD height, UBYTE bitPlaneCount)
    : m_pSourceData(pSourceData),
      m_width(width),
      m_height(height),
      m_bitPlaneCount(bitPlaneCount)
{
}

void BlitterObject::DrawObject(const void* pDestinationData, UWORD x, UWORD y)
{
    WaitBlit();

    custom->bltcon0 = 0xca | SRCA | SRCB | SRCC | DEST | ((x & 15) << ASHIFTSHIFT); // A = source, B = mask, C = background, D = destination
    custom->bltcon1 = ((x & 15) << BSHIFTSHIFT);
    custom->bltapt = (APTR)m_pSourceData;
    custom->bltamod = (m_width - 16) >> 3;
    custom->bltbpt = (UBYTE *)m_pSourceData + (m_width >> 3);
    custom->bltbmod = (m_width - 16) >> 3;
    custom->bltcpt = custom->bltdpt = (APTR)((UBYTE *)pDestinationData + (320 >> 3) * m_bitPlaneCount * y + (x >> 3));
    custom->bltcmod = custom->bltdmod = (320 - m_width - 16) >> 3;
    custom->bltafwm = 0xffff;
    custom->bltalwm = 0;
    custom->bltsize = ((m_height * m_bitPlaneCount) << HSIZEBITS) | ((m_width + 16) / 16);
}
