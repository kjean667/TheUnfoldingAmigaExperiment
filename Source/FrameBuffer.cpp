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

    m_dirtyRegionFreeIndex = 0;
}

FrameBuffer::~FrameBuffer()
{
    FreeMem(m_pBufferData, m_bufferByteSize);
}

void FrameBuffer::RegisterDirtyRegion(APTR pMemoryStart, UWORD modulo, UWORD bltSize)
{
    m_dirtyRegions[m_dirtyRegionFreeIndex].m_pMemoryStart = pMemoryStart;
    m_dirtyRegions[m_dirtyRegionFreeIndex].m_modulo = modulo;
    m_dirtyRegions[m_dirtyRegionFreeIndex].m_bltSize = bltSize;
    if (m_dirtyRegionFreeIndex < MAX_DIRTY_REGIONS - 1) {
        m_dirtyRegionFreeIndex++;
    }
}

void FrameBuffer::Clear()
{
	WaitBlit();
    custom->bltcon0 = DEST;
    custom->bltcon1 = 0;
    custom->bltdpt = (APTR)m_pBufferData;
    custom->bltdmod = 0;
    const UWORD screenWordSize = (m_width * m_height * m_bitPlaneCount) / 16;
    custom->bltsize = screenWordSize;
}

void FrameBuffer::ClearDirtyRegions()
{
    //TODO: Make it smarter to not clear the same memory many times if regions overlap much.
    WaitBlit();
    custom->bltcon0 = DEST;
    custom->bltcon1 = 0;
    for (UWORD i = 0; i < m_dirtyRegionFreeIndex; i++)
    {
        WaitBlit();
        custom->bltdpt = m_dirtyRegions[i].m_pMemoryStart;
        custom->bltdmod = m_dirtyRegions[i].m_modulo;
        custom->bltsize = m_dirtyRegions[i].m_bltSize;
    }
    m_dirtyRegionFreeIndex = 0;
}