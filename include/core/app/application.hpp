#include "debug/logger.h"

#include "debug/validators/energyValidator.h"
#include "debug/validators/dataValidator.h"

template <typename Real, typename WorldReal>
void Application::initValidator()
{
  if (this->cfg.validatorCfg.type == ValidatorType::Energy)
    this->validator = std::make_unique<EnergyValidator<Real, WorldReal>>(this->threadPool);
  else if (this->cfg.validatorCfg.type == ValidatorType::Data)
    this->validator = std::make_unique<DataValidator<Real, WorldReal>>(this->threadPool);
  else
    Logger::logFatal("Application", "This validator type is not supported");
}