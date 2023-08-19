#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <proto/exec.h>

#include "Copper.h"

class MainView
{
public:
    MainView();
    ~MainView();

    UWORD* GetCopperPtr();

private:
    void SetupCopper();

private:
	Copper m_copper;
};

#endif
