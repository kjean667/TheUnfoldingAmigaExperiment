#ifndef BLITTER_OBJECT_H
#define BLITTER_OBJECT_H

#include <proto/exec.h>

class FrameBuffer;

class BlitterObject
{
public:
    BlitterObject(const void* pSourceData, UWORD width, UWORD height, UBYTE bitPlaneCount, bool enableBackgroundRestore = false);
    ~BlitterObject();

    void DrawObject(FrameBuffer *pFrameBuffer, UWORD x, UWORD y);
    void RestoreBackground();

private:
    const void* m_pSourceData;
    const UWORD m_width;
    const UWORD m_height;
    const UBYTE m_bitPlaneCount;

    UWORD m_bltSize;

    struct RestoreData {
        FrameBuffer *m_pRestoreBuffer;
        APTR m_pDestinationMemory;
        UWORD m_destinationMemoryModulo;
    };

    struct RestoreData m_restoreData[2];
};

#endif
