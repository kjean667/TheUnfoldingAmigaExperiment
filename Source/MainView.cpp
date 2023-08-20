#include "MainView.h"

#include "support/gcc8_c_support.h"

INCBIN(SkyDome0000, "Graphics/SkyDome/SkyDome_00.00.bin")
INCBIN(SkyDome0100, "Graphics/SkyDome/SkyDome_01.00.bin")
INCBIN(SkyDome0200, "Graphics/SkyDome/SkyDome_02.00.bin")
INCBIN(SkyDome0300, "Graphics/SkyDome/SkyDome_03.00.bin")
INCBIN(SkyDome0400, "Graphics/SkyDome/SkyDome_04.00.bin")
INCBIN(SkyDome0415, "Graphics/SkyDome/SkyDome_04.15.bin")
INCBIN(SkyDome0430, "Graphics/SkyDome/SkyDome_04.30.bin")
INCBIN(SkyDome0445, "Graphics/SkyDome/SkyDome_04.45.bin")
INCBIN(SkyDome0500, "Graphics/SkyDome/SkyDome_05.00.bin")
INCBIN(SkyDome0515, "Graphics/SkyDome/SkyDome_05.15.bin")
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
INCBIN(SkyDome1915, "Graphics/SkyDome/SkyDome_19.15.bin")
INCBIN(SkyDome1930, "Graphics/SkyDome/SkyDome_19.30.bin")
INCBIN(SkyDome1945, "Graphics/SkyDome/SkyDome_19.45.bin")
INCBIN(SkyDome2000, "Graphics/SkyDome/SkyDome_20.00.bin")
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
    {(UWORD*)SkyDome0415, (4*60*60*FPS + 15*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0430, (4*60*60*FPS + 30*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0445, (4*60*60*FPS + 45*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome0500, 5*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome0515, (5*60*60*FPS + 15*60*FPS)/AnimationScale},
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
    {(UWORD*)SkyDome1915, (19*60*60*FPS + 15*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome1930, (19*60*60*FPS + 30*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome1945, (19*60*60*FPS + 45*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome2000, 20*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome2015, (20*60*60*FPS + 15*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome2030, (20*60*60*FPS + 30*60*FPS)/AnimationScale},
    {(UWORD*)SkyDome2200, 22*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome2300, 23*60*60*FPS/AnimationScale},
    {(UWORD*)SkyDome0000, 24*60*60*FPS/AnimationScale}
};

MainView::MainView()
: m_copper("MainViewCopper", NumScanLines*8+4+4)
{
    SetupCopper();
}

MainView::~MainView()
{
    
}

void MainView::Update(ULONG frameNr)
{
    UpdateCopper(frameNr);
}

UWORD* MainView::GetCopperPtr()
{
    return m_copper.GetCopperPtr();
}

void MainView::SetupCopper()
{
	UWORD di = 0;
	for (USHORT ci = 27; ci <= 255; ci++) {
		m_copper.WaitY(ci);
		m_copper.SetColor(0, 0x0000);
        // Save pointer to where the color is in the copper
        m_copperColorPtr[di++] = m_copper.GetCopperWritePosPtr() - 1;
	}
	m_copper.WaitBottomScreenPart();
	for (USHORT ci = 0; ci <= 54; ci++) {
		m_copper.WaitY(ci);
		m_copper.SetColor(0, 0x0000);
        // Save pointer to where the color is in the copper
        m_copperColorPtr[di++] = m_copper.GetCopperWritePosPtr() - 1;
	}
}

void MainView::UpdateCopper(ULONG frameNr)
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
    UWORD** copperPtr = m_copperColorPtr; 
    for (int y = 0; y < NumScanLines; y++)
    {
        **copperPtr++ = *domeColors++;
    }
}