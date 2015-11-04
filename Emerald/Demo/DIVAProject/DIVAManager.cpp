#include "DIVAManager.h"
#include "DIVAMenu.h"


//----------------------------------------------------------------------------------------------------
DIVAManager::DIVAManager()
	:
	m_state(DIVA_MENU)
{
	Initialize();
}

//----------------------------------------------------------------------------------------------------
DIVAManager::~DIVAManager()
{

}

//----------------------------------------------------------------------------------------------------
bool DIVAManager::Process()
{
	EEDesc desc;
	desc.applicationName = L"Emerald";
	desc.isFullScreen = false;
	desc.width = 800;
	desc.height = 450;
	desc.isSSAA = false;
	desc.isVsync = true;
	EEInitialize(desc);

	while (m_state != DIVA_END)
	{
		switch (m_state)
		{
		case DIVA_MENU:
		{
			DIVAMenu menu;
			m_state = menu.Process();
			break;
		}
		default:
			m_state = DIVA_END;
			break;
		}
	}

	EEShutdown();
	return 0;
}

//----------------------------------------------------------------------------------------------------
bool DIVAManager::Initialize()
{

	return true;
}