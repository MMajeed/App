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