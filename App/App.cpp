#include "App.h"
#include <exception>

App* App::app = 0;

App* App::getInstance()
{
	if(App::app == 0)
	{
		throw std::exception("No App setup");
	}
    return app;
}

App::App()
{
	this->timer._absoluteTime          = 0.0;
	this->timer._frameTime             = 0.0;
	this->timer._frameCount            = 0;
	this->timer._frameRate             = 0.0;
	this->timer._sinceLastDraw         = 0.0;
	this->timer._sinceLastPhysicUpdate = 0.0;
	this->timer._sinceLastInputUpdate  = 0.0;
}