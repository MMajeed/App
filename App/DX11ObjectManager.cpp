#include "DX11ObjectManager.h"

bool DX11ObjectManager::singletonFlag;
DX11ObjectManager* DX11ObjectManager::ObjectManager;

DX11ObjectManager* DX11ObjectManager::getInstance()
{
	if(DX11ObjectManager::singletonFlag == false)
	{
		DX11ObjectManager::ObjectManager = new DX11ObjectManager;
		DX11ObjectManager::singletonFlag = true;
	}

    return DX11ObjectManager::ObjectManager;
}

DX11ObjectManager::DX11ObjectManager()
{

}
