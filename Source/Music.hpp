#pragma once

#include <proto/exec.h>

struct LSP_MusicInit_Ret {
    UWORD bpm;
    unsigned int musicLenTickCount;
};

LSP_MusicInit_Ret LSP_MusicInit(volatile const void *dmacon);
void LSP_MusicPlayTick();
void LSP_MusicSetPos(int pos);
int LSP_MusicGetPos();
