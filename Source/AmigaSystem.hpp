#pragma once

#include <proto/exec.h>
#include <hardware/custom.h>

class AmigaSystem {
public:
    AmigaSystem();
    ~AmigaSystem();

    APTR GetInterruptHandler();
    void SetInterruptHandler(APTR interrupt);

    void WaitLine(USHORT line);

    static inline Custom * GetCustom() {
        return (Custom *)0xdff000;
    }

private:
    void FreeSystem();
    void TakeSystem();
    void WaitVbl();
    APTR GetVBR(void);

    volatile struct Custom *custom;

    // Backup
    UWORD SystemInts;
    UWORD SystemDMA;
    UWORD SystemADKCON;
    volatile APTR VBR;
    APTR SystemIrq;
    
    struct View *ActiView;
};
