#ifndef BLITTER_OBJECT_H
#define BLITTER_OBJECT_H

#include <proto/exec.h>

class FrameBuffer;

class BlitterObject
{
public:
    BlitterObject(const void* pSourceData, UWORD width, UWORD height, UBYTE bitPlaneCount);
    void DrawObject(FrameBuffer *pFrameBuffer, UWORD x, UWORD y);

private:
    const void* m_pSourceData;
    const UWORD m_width;
    const UWORD m_height;
    const UBYTE m_bitPlaneCount;
};

#endif
