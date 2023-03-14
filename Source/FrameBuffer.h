#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <proto/exec.h>

class FrameBuffer
{
public:
    FrameBuffer(const char* name, UWORD width, UWORD height, UBYTE bitPlaneCount);
    ~FrameBuffer();

    void RegisterDirtyRegion(APTR pMemoryStart, UWORD modulo, UWORD bltSize);

    void Clear();
    void ClearDirtyRegions();

public:
    void* m_pBufferData;
    void* m_pBufferDataPlane[5];
    const ULONG m_bufferByteSize;
    const UWORD m_width;
    const UWORD m_height;
    const UBYTE m_bitPlaneCount;

    struct DirtyRegion
    {
        APTR m_pMemoryStart;
        UWORD m_modulo;
        UWORD m_bltSize;
    };

    static const UWORD MAX_DIRTY_REGIONS = 50;
    DirtyRegion m_dirtyRegions[MAX_DIRTY_REGIONS];
    UWORD m_dirtyRegionFreeIndex;
};

#endif
