#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <proto/exec.h>

#include "Copper.h"

struct TimestampedDome
{
    // A pointer to UWORD colors for each raster line
    const UWORD* dome;
    // The frame number when the dome shall be shown
    ULONG tick;
};

class MainView
{
public:
    MainView();
    ~MainView();

    void Update(ULONG frameNr);

    UWORD* GetCopperPtr();

private:
    void SetupCopper();
    void UpdateCopper(ULONG frameNr);

private:
	Copper m_copper;

    static const int NumScanLines = 283;
    /// A list of pointers into the copper where the background colors are defined for each raster line
    UWORD* m_copperColorPtr[NumScanLines];
    int m_currentDomeIndex = 0;
};


#endif
