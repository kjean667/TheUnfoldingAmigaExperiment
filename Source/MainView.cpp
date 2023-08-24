#include "MainView.h"

#include "support/gcc8_c_support.h"

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
	UWORD i = 27;
	for (USHORT ci = 27; ci <= 255; ci++) {
		m_copper.WaitY(ci);
        m_skyDome.GenerateCopper(i++, &m_copper);
	}
	m_copper.WaitBottomScreenPart();
	for (USHORT ci = 0; ci <= 54; ci++) {
		m_copper.WaitY(ci);
        m_skyDome.GenerateCopper(i++, &m_copper);
	}
}

void MainView::UpdateCopper(ULONG frameNr)
{
    m_skyDome.UpdateCopper(frameNr);
}