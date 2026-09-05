#include "core/application.h"

#include "parsers.h"

int main(int argc, char **argv)
{
	Application application(parseArgs(argc, argv));

	while (!application.getWindowShouldClose())
	{
		application.update();
		application.render();
	}
	return 0;
}