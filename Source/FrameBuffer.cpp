#include "FrameBuffer.h"

#include "support/gcc8_c_support.h"
#include <proto/graphics.h>

static volatile struct Custom *custom = (struct Custom*)0xdff000;

FrameBuffer::FrameBuffer(const char* name, UWORD width, UWORD height, UBYTE bitPlaneCount)
    : m_bufferByteSize(bitPlaneCount * height * (width >> 3)),
      m_width(width),
      m_height(height),
      m_bitPlaneCount(bitPlaneCount)
{
   	m_pBufferData = (UBYTE*)AllocMem(m_bufferByteSize, MEMF_CHIP);
	debug_register_bitmap(m_pBufferData, name, width, height, bitPlaneCount, debug_resource_bitmap_interleaved);

    // Pre-calculate plane pointers
    for (int i = 0; i < bitPlaneCount; i++) {
        m_pBufferDataPlane[i] = (UBYTE*)m_pBufferData + i * (m_width / 8);
    }
}

FrameBuffer::~FrameBuffer()
{
    FreeMem(m_pBufferData, m_bufferByteSize);
}

void FrameBuffer::Clear()
{
	WaitBlit();
    custom->bltcon0 = A_TO_D | DEST;
    custom->bltcon1 = 0;
    custom->bltadat = 0;
    custom->bltdpt = (APTR)m_pBufferData;
    custom->bltdmod = 0;
    custom->bltafwm = custom->bltalwm = 0xffff;
    const unsigned short screenWordSize = (m_width * m_height * m_bitPlaneCount) / 16;
    custom->bltsize = screenWordSize;
}