#include "DX11ObjectManager.h"

DX11ObjectManager* DX11ObjectManager::ObjectManager = 0;

DX11ObjectManager* DX11ObjectManager::getInstance()
{
	if(DX11ObjectManager::ObjectManager == 0)
	{
		DX11ObjectManager::ObjectManager = new DX11ObjectManager;
	}

    return DX11ObjectManager::ObjectManager;
}

DX11ObjectManager::DX11ObjectManager()
{

}
