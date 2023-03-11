#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <proto/exec.h>

class FrameBuffer
{
public:
    FrameBuffer(const char* name, UWORD width, UWORD height, UBYTE bitPlaneCount);
    ~FrameBuffer();

    void Clear();

public:
    void* m_pBufferData;
    void* m_pBufferDataPlane[5];
    const ULONG m_bufferByteSize;
    const UWORD m_width;
    const UWORD m_height;
    const UBYTE m_bitPlaneCount;
};

#endif
