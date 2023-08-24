#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <proto/exec.h>

#include "Copper.h"

#include "SkyDome.h"

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
    SkyDome m_skyDome;
};


#endif
