#include "App.h"
#include <exception>

bool App::singletonFlag;
App* App::app;

App* App::getInstance()
{
	if(singletonFlag == false)
	{
		throw std::exception("No App setup");
	}
    return app;
}