#include "core/application.h"

int main()
{
	Application application("Space Simulation", 800, 600, 4, 1, GL_TRUE);

	while (!application.getWindowShouldClose())
	{
		application.update();
		application.render();
	}
	return 0;
}