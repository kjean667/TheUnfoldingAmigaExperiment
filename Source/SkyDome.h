#ifndef SKY_DOME_H
#define SKY_DOME_H

#include <proto/exec.h>

class Copper;

class SkyDome
{
public:
    SkyDome();

    void GenerateCopper(UWORD line, Copper* copper);
    void UpdateCopper(ULONG frameNr);

private:
    static const int NumScanLines = 283;
    /// A list of pointers into the copper where the background colors are defined for each raster line
    UWORD* m_copperColorPtr[NumScanLines];
    int m_currentDomeIndex = 0;
};

#endif
