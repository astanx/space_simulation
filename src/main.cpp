#include "core/app/application.h"

#include "parsers.h"

int main(int argc, char **argv)
{
	Application application(parseArgs(argc, argv));

	while (!application.shouldExit())
	{
		application.update();
		application.render();
	}
	return 0;
}