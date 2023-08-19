#include "MainView.h"

#include "support/gcc8_c_support.h"

INCBIN(SkyDome, "Graphics/SkyDome/SkyDome_05.00.bin")

MainView::MainView()
: m_copper("MainViewCopper", 1024)
{
    SetupCopper();
}

MainView::~MainView()
{
    
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
		m_copper.SetColor(0, ((USHORT*)SkyDome)[di++]);
	}
	m_copper.WaitBottomScreenPart();
	for (USHORT ci = 0; ci <= 55; ci++) {
		m_copper.WaitY(ci);
		m_copper.SetColor(0, ((USHORT*)SkyDome)[di++]);
	}
}