#include "BlitterObject.h"

#include <proto/graphics.h>

#include "FrameBuffer.h"

static volatile struct Custom *custom = (struct Custom*)0xdff000;

BlitterObject::BlitterObject(const void* pSourceData, UWORD width, UWORD height, UBYTE bitPlaneCount, bool enableBackgroundRestore)
    : m_pSourceData(pSourceData),
      m_width(width),
      m_height(height),
      m_bitPlaneCount(bitPlaneCount)
{
    if (enableBackgroundRestore) {
        m_restoreData[0].m_pRestoreBuffer = new FrameBuffer("BlitterObjectRestore1", width, height, bitPlaneCount);
        m_restoreData[1].m_pRestoreBuffer = new FrameBuffer("BlitterObjectRestore2", width, height, bitPlaneCount);
    } else {
        m_restoreData[0].m_pRestoreBuffer = nullptr;
        m_restoreData[1].m_pRestoreBuffer = nullptr;
    }
    m_restoreData[0].m_pDestinationMemory = nullptr;
    m_restoreData[1].m_pDestinationMemory = nullptr;

    m_bltSize = ((m_height * m_bitPlaneCount) << HSIZEBITS) | ((m_width + 16) / 16); 
}

BlitterObject::~BlitterObject()
{
    if (m_restoreData[0].m_pRestoreBuffer != nullptr) {
        delete m_restoreData[0].m_pRestoreBuffer;
    }
    if (m_restoreData[1].m_pRestoreBuffer != nullptr) {
        delete m_restoreData[1].m_pRestoreBuffer;
    }
}

void BlitterObject::DrawObject(FrameBuffer *pFrameBuffer, UWORD x, UWORD y)
{
    APTR pDestinationMemory = (APTR)((UBYTE *)pFrameBuffer->m_pBufferData +
                                     (pFrameBuffer->m_width >> 3) * pFrameBuffer->m_bitPlaneCount * y + (x >> 3));
    UWORD destinationMemoryModulo = (pFrameBuffer->m_width - m_width - 16) >> 3;

    if (m_restoreData[0].m_pRestoreBuffer != nullptr)
    {
        // Save the framebuffer background behind the image so that it can be restored by the RestoreBackground method
        WaitBlit();
        custom->bltcon0 = 0xca | SRCA | DEST;
        custom->bltcon1 = 0;
        custom->bltapt = pDestinationMemory;
        custom->bltamod = destinationMemoryModulo;
        custom->bltdpt = (APTR)m_restoreData[0].m_pRestoreBuffer->m_pBufferData;
        custom->bltdmod = m_restoreData[0].m_pRestoreBuffer->m_width >> 3;
        custom->bltafwm = 0xffff;
        custom->bltalwm = 0xffff;
        custom->bltsize = m_bltSize;

        m_restoreData[0].m_pDestinationMemory = pDestinationMemory;
        m_restoreData[0].m_destinationMemoryModulo = destinationMemoryModulo;
    }

    // Draw the image to the framebuffer
    WaitBlit();
    custom->bltcon0 = 0xca | SRCA | SRCB | SRCC | DEST | ((x & 15) << ASHIFTSHIFT); // A = source, B = mask, C = background, D = destination
    custom->bltcon1 = ((x & 15) << BSHIFTSHIFT);
    custom->bltapt = (APTR)m_pSourceData;
    custom->bltamod = (m_width - 16) >> 3;
    custom->bltbpt = (UBYTE *)m_pSourceData + (m_width >> 3);
    custom->bltbmod = (m_width - 16) >> 3;
    custom->bltcpt = custom->bltdpt = pDestinationMemory;
    custom->bltcmod = custom->bltdmod = destinationMemoryModulo;
    custom->bltafwm = 0xffff;
    custom->bltalwm = 0;
    custom->bltsize = m_bltSize;
}

void BlitterObject::RestoreBackground()
{
    // Shift double buffer restore information
    auto tmp = m_restoreData[0];
    m_restoreData[0] = m_restoreData[1];
    m_restoreData[1] = tmp;

    if (m_restoreData[0].m_pDestinationMemory == nullptr) {
        return;
    }

    // Restore background where object was previously drawn
    WaitBlit();
    custom->bltcon0 = 0xca | SRCA | DEST;
    custom->bltcon1 = 0;
    custom->bltapt = (APTR)m_restoreData[0].m_pRestoreBuffer->m_pBufferData;
    custom->bltamod = (m_width - 16) >> 3;
    custom->bltdpt = m_restoreData[0].m_pDestinationMemory;
    custom->bltdmod = m_restoreData[0].m_destinationMemoryModulo;
    custom->bltafwm = 0xffff;
    custom->bltalwm = 0xffff;
    custom->bltsize = m_bltSize;
}
