#ifndef COPPER_H
#define COPPER_H

#include <proto/exec.h>

class Copper
{
public:
    Copper(const char* name, ULONG memorySize);
    ~Copper();

    UWORD* GetCopperPtr();
    UWORD* GetCopperWritePosPtr();

    void WaitXY(USHORT x, USHORT y);
    void WaitY(USHORT y);
    void SetColor(USHORT index, USHORT color);
    void WaitBottomScreenPart();

private:
    const ULONG m_copperSize;

    const UWORD* m_pCopper;
    UWORD* m_pCopperWritePos;
};

#endif
