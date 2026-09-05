#include "core/application.h"

#include "debug/logger.h"

AppConfig parseArgs(int argc, char **argv)
{
	AppConfig cfg;

	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];

		if (arg == "--gpu")
		{
			cfg.backend = Backend::GPU;
		}
		else if (arg == "--cpu")
		{
			cfg.backend = Backend::CPU;
		}
		else if (arg == "--double")
		{
			cfg.precision = Precision::DOUBLE;
		}
		else if (arg == "--float")
		{
			cfg.precision = Precision::FLOAT;
		}
		else
		{
			Logger::logWarning("Main", "Unknow argument: " + arg);
		}
	}

	return cfg;
}

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