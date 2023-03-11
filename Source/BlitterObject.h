#ifndef BLITTER_OBJECT_H
#define BLITTER_OBJECT_H

#include <proto/exec.h>

class BlitterObject
{
public:
    BlitterObject(const void* pSourceData, UWORD width, UWORD height, UBYTE bitPlaneCount);
    void DrawObject(const void* pDestinationData, UWORD x, UWORD y);

private:
    const void* m_pSourceData;
    const UWORD m_width;
    const UWORD m_height;
    const UBYTE m_bitPlaneCount;

    volatile struct Custom *custom = (struct Custom*)0xdff000;
};

#endif
