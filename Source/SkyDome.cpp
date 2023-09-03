#include "SkyDome.h"
#include "Copper.h"

#include "support/gcc8_c_support.h"

struct TimestampedDome
{
    // A pointer to UWORD colors for each raster line
    const UWORD* dome;
    // The frame number when the dome shall be shown
    ULONG tick;
};

INCBIN(SkyDome0000, "Graphics/SkyDome/SkyDome_00.00.bin")
INCBIN(SkyDome0100, "Graphics/SkyDome/SkyDome_01.00.bin")
INCBIN(SkyDome0200, "Graphics/SkyDome/SkyDome_02.00.bin")
INCBIN(SkyDome0300, "Graphics/SkyDome/SkyDome_03.00.bin")
INCBIN(SkyDome0400, "Graphics/SkyDome/SkyDome_04.00.bin")
INCBIN(SkyDome0405, "Graphics/SkyDome/SkyDome_04.05.bin")
INCBIN(SkyDome0410, "Graphics/SkyDome/SkyDome_04.10.bin")
INCBIN(SkyDome0415, "Graphics/SkyDome/SkyDome_04.15.bin")
INCBIN(SkyDome0420, "Graphics/SkyDome/SkyDome_04.20.bin")
INCBIN(SkyDome0425, "Graphics/SkyDome/SkyDome_04.25.bin")
INCBIN(SkyDome0430, "Graphics/SkyDome/SkyDome_04.30.bin")
INCBIN(SkyDome0435, "Graphics/SkyDome/SkyDome_04.35.bin")
INCBIN(SkyDome0440, "Graphics/SkyDome/SkyDome_04.40.bin")
INCBIN(SkyDome0445, "Graphics/SkyDome/SkyDome_04.45.bin")
INCBIN(SkyDome0450, "Graphics/SkyDome/SkyDome_04.50.bin")
INCBIN(SkyDome0455, "Graphics/SkyDome/SkyDome_04.55.bin")
INCBIN(SkyDome0500, "Graphics/SkyDome/SkyDome_05.00.bin")
INCBIN(SkyDome0505, "Graphics/SkyDome/SkyDome_05.05.bin")
INCBIN(SkyDome0510, "Graphics/SkyDome/SkyDome_05.10.bin")
INCBIN(SkyDome0515, "Graphics/SkyDome/SkyDome_05.15.bin")
INCBIN(SkyDome0520, "Graphics/SkyDome/SkyDome_05.20.bin")
INCBIN(SkyDome0525, "Graphics/SkyDome/SkyDome_05.25.bin")
INCBIN(SkyDome0530, "Graphics/SkyDome/SkyDome_05.30.bin")
INCBIN(SkyDome0545, "Graphics/SkyDome/SkyDome_05.45.bin")
INCBIN(SkyDome0600, "Graphics/SkyDome/SkyDome_06.00.bin")
INCBIN(SkyDome0700, "Graphics/SkyDome/SkyDome_07.00.bin")
INCBIN(SkyDome0800, "Graphics/SkyDome/SkyDome_08.00.bin")
INCBIN(SkyDome0900, "Graphics/SkyDome/SkyDome_09.00.bin")
INCBIN(SkyDome1000, "Graphics/SkyDome/SkyDome_10.00.bin")
INCBIN(SkyDome1100, "Graphics/SkyDome/SkyDome_11.00.bin")
INCBIN(SkyDome1200, "Graphics/SkyDome/SkyDome_12.00.bin")
INCBIN(SkyDome1300, "Graphics/SkyDome/SkyDome_13.00.bin")
INCBIN(SkyDome1400, "Graphics/SkyDome/SkyDome_14.00.bin")
INCBIN(SkyDome1500, "Graphics/SkyDome/SkyDome_15.00.bin")
INCBIN(SkyDome1600, "Graphics/SkyDome/SkyDome_16.00.bin")
INCBIN(SkyDome1700, "Graphics/SkyDome/SkyDome_17.00.bin")
INCBIN(SkyDome1800, "Graphics/SkyDome/SkyDome_18.00.bin")
INCBIN(SkyDome1815, "Graphics/SkyDome/SkyDome_18.15.bin")
INCBIN(SkyDome1830, "Graphics/SkyDome/SkyDome_18.30.bin")
INCBIN(SkyDome1845, "Graphics/SkyDome/SkyDome_18.45.bin")
INCBIN(SkyDome1900, "Graphics/SkyDome/SkyDome_19.00.bin")
INCBIN(SkyDome1905, "Graphics/SkyDome/SkyDome_19.05.bin")
INCBIN(SkyDome1910, "Graphics/SkyDome/SkyDome_19.10.bin")
INCBIN(SkyDome1915, "Graphics/SkyDome/SkyDome_19.15.bin")
INCBIN(SkyDome1920, "Graphics/SkyDome/SkyDome_19.20.bin")
INCBIN(SkyDome1925, "Graphics/SkyDome/SkyDome_19.25.bin")
INCBIN(SkyDome1930, "Graphics/SkyDome/SkyDome_19.30.bin")
INCBIN(SkyDome1935, "Graphics/SkyDome/SkyDome_19.35.bin")
INCBIN(SkyDome1940, "Graphics/SkyDome/SkyDome_19.40.bin")
INCBIN(SkyDome1945, "Graphics/SkyDome/SkyDome_19.45.bin")
INCBIN(SkyDome1950, "Graphics/SkyDome/SkyDome_19.50.bin")
INCBIN(SkyDome1955, "Graphics/SkyDome/SkyDome_19.55.bin")
INCBIN(SkyDome2000, "Graphics/SkyDome/SkyDome_20.00.bin")
INCBIN(SkyDome2005, "Graphics/SkyDome/SkyDome_20.05.bin")
INCBIN(SkyDome2010, "Graphics/SkyDome/SkyDome_20.10.bin")
INCBIN(SkyDome2015, "Graphics/SkyDome/SkyDome_20.15.bin")
INCBIN(SkyDome2030, "Graphics/SkyDome/SkyDome_20.30.bin")
INCBIN(SkyDome2200, "Graphics/SkyDome/SkyDome_22.00.bin")
INCBIN(SkyDome2300, "Graphics/SkyDome/SkyDome_23.00.bin")

static const int FPS = 50;
static const int AnimationLength = 4*60*FPS; // Real-time length, in frames, that the day-night cycles takes.
static const int AnimationScale = 24*60*60*FPS / AnimationLength;
static const struct TimestampedDome g_domes[] = {
    {(UWORD*)SkyDome0000, 0},
    {(UWORD*)SkyDome0100, 1*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome0200, 2*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome0300, 3*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome0400, 4*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome0405, (4*60*60*FPS + 5*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0410, (4*60*60*FPS + 10*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0415, (4*60*60*FPS + 15*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0420, (4*60*60*FPS + 20*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0425, (4*60*60*FPS + 25*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0430, (4*60*60*FPS + 30*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0435, (4*60*60*FPS + 35*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0440, (4*60*60*FPS + 40*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0445, (4*60*60*FPS + 45*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0450, (4*60*60*FPS + 50*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0455, (4*60*60*FPS + 55*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0500, 5*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome0505, (5*60*60*FPS + 5*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0510, (5*60*60*FPS + 10*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0515, (5*60*60*FPS + 15*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0520, (5*60*60*FPS + 20*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0525, (5*60*60*FPS + 25*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0530, (5*60*60*FPS + 30*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0545, (5*60*60*FPS + 45*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0600, 6*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome0700, 7*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome0800, 8*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome0900, 9*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome1000, 10*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome1100, 11*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome1200, 12*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome1300, 13*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome1400, 14*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome1500, 15*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome1600, 16*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome1700, 17*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome1800, 18*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome1815, (18*60*60*FPS + 15*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome1830, (18*60*60*FPS + 30*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome1845, (18*60*60*FPS + 45*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome1900, 19*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome1905, (19*60*60*FPS + 5*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome1910, (19*60*60*FPS + 10*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome1915, (19*60*60*FPS + 15*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome1920, (19*60*60*FPS + 20*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome1925, (19*60*60*FPS + 25*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome1930, (19*60*60*FPS + 30*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome1935, (19*60*60*FPS + 35*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome1940, (19*60*60*FPS + 40*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome1945, (19*60*60*FPS + 45*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome1950, (19*60*60*FPS + 50*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome1955, (19*60*60*FPS + 55*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome2000, 20*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome2005, (20*60*60*FPS + 5*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome2010, (20*60*60*FPS + 10*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome2015, (20*60*60*FPS + 15*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome2030, (20*60*60*FPS + 30*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome2200, 22*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome2300, 23*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome0000, 24*60*60*FPS/AnimationScale}
};

SkyDome::SkyDome()
{
}

void SkyDome::GenerateCopper(UWORD line, Copper* copper)
{
    if (line >= 27 && line < NumScanLines + 27)
    {
        copper->SetColor(0, 0x0000);
        // Save pointer to where the color is in the copper
        m_copperColorPtr[line-27] = copper->GetCopperWritePosPtr() - 1;
    }
}

static const UBYTE sinus0_5[] = {
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
static const UBYTE sinus1[] = {
    1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1
};
static const UBYTE sinus2[] = {
    2,2,2,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,3,3,3,3,3,3,2,2,2,2,2,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,2,2
};
static const UBYTE sinus3[] = {
    3,3,4,4,4,4,5,5,5,5,5,6,6,6,6,6,6,6,6,6,6,6,5,5,5,5,5,4,4,4,4,3,3,3,2,2,2,2,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,3
};
static const UBYTE sinus4[] = {
    4,4,5,5,6,6,6,7,7,7,7,8,8,8,8,8,8,8,8,8,8,8,7,7,7,7,6,6,6,5,5,4,4,4,3,3,2,2,2,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,3,3,4
};
static const UBYTE sinus5[] = {
    5,5,6,6,7,7,8,8,9,9,9,9,10,10,10,10,10,10,10,10,10,9,9,9,9,8,8,7,7,6,6,5,5,5,4,4,3,3,2,2,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,3,3,4,4,5
};
static const UBYTE sinus6[] = {
    6,7,7,8,8,9,9,10,10,11,11,11,12,12,12,12,12,12,12,12,12,11,11,11,10,10,9,9,8,8,7,7,6,5,5,4,4,3,3,2,2,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,2,2,3,3,4,4,5,5
};
static const UBYTE sinus7[] = {
    7,8,8,9,10,10,11,11,12,12,13,13,13,14,14,14,14,14,14,14,13,13,13,12,12,11,11,10,10,9,8,8,7,6,6,5,4,4,3,3,2,2,1,1,1,0,0,0,0,0,0,0,1,1,1,2,2,3,3,4,4,5,6,6
};
static const UBYTE sinus8[] = {
    8,9,10,10,11,12,12,13,14,14,15,15,15,16,16,16,16,16,16,16,15,15,15,14,14,13,12,12,11,10,10,9,8,7,6,6,5,4,4,3,2,2,1,1,1,0,0,0,0,0,0,0,1,1,1,2,2,3,4,4,5,6,6,7
};
static const UBYTE sinus9[] = {
    9,10,11,12,12,13,14,15,15,16,16,17,17,18,18,18,18,18,18,18,17,17,16,16,15,15,14,13,12,12,11,10,9,8,7,6,6,5,4,3,3,2,2,1,1,0,0,0,0,0,0,0,1,1,2,2,3,3,4,5,6,6,7,8
};
static const UBYTE sinus10[] = {
    10,11,12,13,14,15,16,16,17,18,18,19,19,20,20,20,20,20,20,20,19,19,18,18,17,16,16,15,14,13,12,11,10,9,8,7,6,5,4,4,3,2,2,1,1,0,0,0,0,0,0,0,1,1,2,2,3,4,4,5,6,7,8,9
};

// Distance table for water animation for each raster line below the horizon
static const UBYTE* waveSinuses[] = {
sinus0_5,
sinus0_5,
sinus0_5,
sinus0_5,
sinus0_5,
sinus0_5,
sinus0_5,
sinus0_5,
sinus0_5,
sinus1,
sinus1,
sinus1,
sinus1,
sinus1,
sinus1,
sinus1,
sinus1,
sinus1,
sinus1,
sinus1,
sinus1,
sinus2,
sinus2,
sinus2,
sinus2,
sinus2,
sinus2,
sinus2,
sinus2,
sinus2,
sinus2,
sinus2,
sinus2,
sinus2,
sinus2,
sinus2,
sinus2,
sinus2,
sinus3,
sinus3,
sinus3,
sinus3,
sinus3,
sinus3,
sinus3,
sinus3,
sinus3,
sinus3,
sinus3,
sinus3,
sinus3,
sinus3,
sinus3,
sinus4,
sinus4,
sinus4,
sinus4,
sinus4,
sinus4,
sinus4,
sinus4,
sinus4,
sinus4,
sinus4,
sinus4,
sinus4,
sinus4,
sinus5,
sinus5,
sinus5,
sinus5,
sinus5,
sinus5,
sinus5,
sinus5,
sinus5,
sinus5,
sinus5,
sinus5,
sinus5,
sinus6,
sinus6,
sinus6,
sinus6,
sinus6,
sinus6,
sinus6,
sinus6,
sinus6,
sinus6,
sinus6,
sinus6,
sinus7,
sinus7,
sinus7,
sinus7,
sinus7,
sinus7,
sinus7,
sinus7,
sinus7,
sinus7,
sinus7,
sinus7,
sinus8,
sinus8,
sinus8,
sinus8,
sinus8,
sinus8,
sinus8,
sinus8,
sinus8,
sinus8,
sinus8,
sinus9,
sinus9,
sinus9,
sinus9,
sinus9,
sinus9,
sinus9,
sinus9,
sinus9,
sinus9,
sinus9,
sinus10,
sinus10,
sinus10,
sinus10,
sinus10,
sinus10,
sinus10,
sinus10
};

void SkyDome::UpdateCopper(ULONG frameNr)
{
    // Animate which dome to show
    const int domeCount = sizeof(g_domes) / sizeof(struct TimestampedDome);
    int nextDomeIndex = m_currentDomeIndex + 1;
    if (nextDomeIndex >= domeCount) {
        nextDomeIndex = 0;
    }
    const struct TimestampedDome& nextDome = g_domes[nextDomeIndex];
    if (g_domes[nextDomeIndex].tick <= (frameNr % (AnimationLength + 1)))
    {
        m_currentDomeIndex = nextDomeIndex;
    }

    // Update copper colors for the dome
    const UWORD* domeColors = g_domes[m_currentDomeIndex].dome;
    UWORD* domeColorPtr = (UWORD*)domeColors;
    UWORD** copperPtr = m_copperColorPtr; 
    int y = 0;
    for (; y < 149; y++)
    {
        **copperPtr++ = *domeColorPtr++;
    }
    // Apply water wave animation on the bottom half of the screen
    UBYTE** waveSinusPtr = (UBYTE**)waveSinuses;
    for (; y < NumScanLines; y++)
    {
        UBYTE sin = (*waveSinusPtr)[(frameNr + y) % sizeof(sinus1)];
        waveSinusPtr++;
        **copperPtr++ = domeColors[y - sin];
    }
}
