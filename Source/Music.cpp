#include "Music.hpp"

#include "support/gcc8_c_support.h"

INCBIN_CHIP(player, "Sound/lightspeedplayer-mod.bin")
INCBIN_CHIP(playerbank, "Sound/coconut.lsbank")
INCBIN_CHIP(playermusic, "Sound/coconut.lsmusic")

// For __asm syntax, see https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html

LSP_MusicInit_Ret LSP_MusicInit(volatile const void *dmacon)
{
    register volatile const void* _a0 ASM("a0") = playermusic;
    register volatile const void* _a1 ASM("a1") = playerbank;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-arith"
    register volatile const void* _a2 ASM("a2") = dmacon+1; // low byte address (odd)
#pragma GCC diagnostic pop
    register volatile const void* _a3 ASM("a3") = player;
    register                int   _d0 ASM("d0"); // return value
    __asm volatile (
        "movem.l %%d1-%%d7/%%a4-%%a6,-(%%sp)\n"
        "jsr 0(%%a3)\n"
        "movem.l (%%sp)+,%%d1-%%d7/%%a4-%%a6"
    : "=rd" (_d0), "+ra"(_a0)
    : 
    : "memory");
    return {
        .bpm = (UWORD)( *(UWORD*)_a0 & 0xffff),
        .musicLenTickCount = (unsigned int)_d0
    };
}

void LSP_MusicPlayTick()
{
    register volatile const void* _a5 ASM("a5") = player;
    register volatile const void* _a6 ASM("a6") = (void*)0xdff0a0;
    __asm volatile (
        "movem.l %%d0-%%d2/%%a0-%%a4,-(%%sp)\n"
        "jsr 4(%%a5)\n"
        "movem.l (%%sp)+,%%d0-%%d2/%%a0-%%a4"
    : 
    :
    : "memory");
}

void LSP_MusicSetPos(int pos)
{
    register volatile const void* _a4 ASM("a4") = player;
    register volatile const void* _d0 ASM("d0") = (void*)pos;
    __asm volatile (
        "movem.l %%d1/%%a0-%%a3,-(%%sp)\n"
        "jsr 8(%%a4)\n"
        "movem.l (%%sp)+,%%d1/%%a0-%%a3"
    : 
    :
    : "memory");
}

int LSP_MusicGetPos()
{
    register volatile const void* _a3 ASM("a3") = player;
    register                int   _d0 ASM("d0"); // return value
    __asm volatile (
        "jsr 12(%%a3)"
    : "=rd"(_d0)
    :
    : "memory");
    return _d0;
}
