#include "core/app/application.h"

#include "debug/logger.h"

#include "scene/scene.h"

#include "resources/resources.h"
#include "resources/resourceInitializer.h"

#include "graphics/primitives/ellipsoid.h"
#include "graphics/primitives/quad.h"
#include "graphics/primitives/asteroidShape.h"

#include "render/state/renderState.h"

#include "physics/constants/constants.h"

#include "debug/validators/energyValidator.h"

#include <iostream>
#include <filesystem>

// Private functions
void Application::initWorld()
{
  bool enableRender = this->cfg.mode == Mode::Simulation;
  this->scene.init(this->renderCtx, this->resourceManager, this->threadPool, this->cfg.precision, this->timeManager.getStartTime(), enableRender);

  if (this->cfg.backend == Backend::GPU)
    this->scene.initGPUWorld(this->resourceManager);
  else if (this->cfg.backend == Backend::CPU)
    this->scene.initCPUWorld(this->threadPool);
  else
    Logger::logFatal("Application", "Backend type is not supported");
}
void Application::initRenderer()
{
  this->renderer.init(this->renderCtx);

  if (this->cfg.backend == Backend::GPU)
    this->renderer.initGPUBackend(this->scene);
  else if (this->cfg.backend == Backend::CPU)
    this->renderer.initCPUBackend(this->scene);
  else
    Logger::logFatal("Application", "Backend type is not supported");
}

void Application::initWindow()
{
  if (this->window)
    Logger::logError("Application", "Window initialized twice");

  if (this->cfg.mode == Mode::Simulation)
  {
    this->window = std::make_unique<Window>(this->cfg.windowConfig);
    this->window->init();

    this->renderCtx.frameCtx = this->window->getFrameContext();

    this->window->setMouseCallback([this](float x, float y)
                                   { scene.processMouseMovement(x, y); });

    this->window->setScrollCallback([this](float y)
                                    { scene.processMouseScroll(y); });

    this->window->setFramebufferResizeCallback([this](FrameContext ctx)
                                               { renderer.resize(ctx); });
  }
}
void Application::initResources()
{
  if (this->cfg.mode == Mode::Simulation)
  {
    ResourceInitializer::loadShaders(this->resourceManager, this->cfg.windowConfig);
    ResourceInitializer::loadModels(this->resourceManager);
    ResourceInitializer::loadAsteroids(this->resourceManager);
    ResourceInitializer::loadFullscreenQuad(this->resourceManager);
  }

  if (this->cfg.backend == Backend::GPU)
  {
    Context &ctx = this->resourceManager.LoadContext(Res::MAIN_CONTEXT);
    if (this->cfg.precision == Precision::DOUBLE && !ctx.getSupportsDouble())
      Logger::logFatal("Application", "Double precision is not supported on this GPU, use --precision float argument");
    ResourceInitializer::loadKernels(this->resourceManager);
  }
}
void Application::initMode()
{
  if (this->cfg.mode == Mode::Simulation)
    this->initRenderer();
  else if (this->cfg.mode == Mode::EnergyValidation)
  {
    if (this->cfg.validatorCfg.precision == Precision::DOUBLE)
    {
      if (this->cfg.precision == Precision::DOUBLE)
        this->validator = std::make_unique<EnergyValidator<double, double>>(this->threadPool);
      else if (this->cfg.precision == Precision::FLOAT)
        this->validator = std::make_unique<EnergyValidator<double, float>>(this->threadPool);
    }
    else if (this->cfg.validatorCfg.precision == Precision::FLOAT)
    {
      if (this->cfg.precision == Precision::DOUBLE)
        this->validator = std::make_unique<EnergyValidator<float, double>>(this->threadPool);
      else if (this->cfg.precision == Precision::FLOAT)
        this->validator = std::make_unique<EnergyValidator<float, float>>(this->threadPool);
    }
    else
      Logger::logFatal("Application", "This precision is not supported for validator");

    this->validator->init(this->scene, this->timeManager.getElapsedTime(), this->cfg.validatorCfg.steps);
  }
}

void Application::processInput()
{
  double dt = this->timeManager.getFrameDeltaTime();
  if (this->input.isActionPressed(Action::Exit))
    this->window->setWindowShouldClose();

  if (this->input.isActionHold(Action::MoveForward))
    this->scene.processKeyboard(FORWARD, dt);

  if (this->input.isActionHold(Action::MoveBackward))
    this->scene.processKeyboard(BACKWARD, dt);

  if (this->input.isActionHold(Action::MoveLeft))
    this->scene.processKeyboard(LEFT, dt);

  if (this->input.isActionHold(Action::MoveRight))
    this->scene.processKeyboard(RIGHT, dt);

  if (this->input.isActionHold(Action::MoveUp))
    this->scene.processKeyboard(UP, dt);

  if (this->input.isActionHold(Action::MoveDown))
    this->scene.processKeyboard(DOWN, dt);

  if (this->input.isActionPressed(Action::ToggleBloom))
    this->renderCtx.settings.useBloom = !this->renderCtx.settings.useBloom;

  if (this->input.isActionPressed(Action::ToggleHDR))
    this->renderCtx.settings.useHDR = !this->renderCtx.settings.useHDR;

  if (this->input.isActionPressed(Action::HideText))
    this->isTextShown = !this->isTextShown;

  if (this->input.isActionPressed(Action::LogPosition))
  {
    const glm::vec3 position = this->scene.getActiveCameraPosition();
    Logger::logInfo("Application", "Camera position: " +
                                       std::to_string(position.x) + ", " +
                                       std::to_string(position.y) + ", " +
                                       std::to_string(position.z));
  }

  if (this->input.isActionPressed(Action::Pause))
    this->renderCtx.settings.paused = !this->renderCtx.settings.paused;

  if (this->input.isActionPressed(Action::DecreaseCameraSpeed))
    this->scene.decreaseCameraSpeed();

  if (this->input.isActionPressed(Action::IncreaseCameraSpeed))
    this->scene.increaseCameraSpeed();

  if (this->input.isActionPressed(Action::DoubleTimestep))
  {
    if (this->cfg.timeCfg.timestep > 0)
      this->cfg.timeCfg.timestep *= 2;
    else
      this->cfg.timeCfg.timestep *= .5;
  }

  if (this->input.isActionPressed(Action::HalfTimestep))
  {
    if (this->cfg.timeCfg.timestep > 0)
      this->cfg.timeCfg.timestep *= .5;
    else
      this->cfg.timeCfg.timestep *= 2;
  }

  if (this->input.isActionHold(Action::DecreaseTimestep))
    this->cfg.timeCfg.timestep -= 2;

  if (this->input.isActionHold(Action::IncreaseTimestep))
    this->cfg.timeCfg.timestep += 2;
}

void Application::updateValidator()
{
  this->validator->update(this->scene, this->timeManager.getElapsedTime());
  if (this->validator->isFinished())
  {
    this->validator->sendTable();
    if (this->cfg.validatorCfg.pathSpecified)
      this->validator->saveTable(this->scene, this->cfg.validatorCfg.savePath);

    this->isFinished = true;
  }
}

// Constructor / Destructor
Application::Application(const AppConfig &config) : cfg(config),
                                                    resourceManager(),
                                                    threadPool(),
                                                    scene(),
                                                    input(),
                                                    timeManager(this->cfg.timeCfg),
                                                    renderer(resourceManager)
{
  this->initWindow();

  // Init variables
  this->renderCtx.deltaTime = 0.0;
  this->renderCtx.settings.paused = false;
  this->renderCtx.settings.useBloom = true;
  this->renderCtx.settings.useHDR = true;
  this->renderCtx.settings.exposure = 5e-4;
  this->renderCtx.settings.bloomPower = 0.5;

  this->isTextShown = true;

  this->initResources();
  this->initWorld();
  this->initMode();
}

Application::~Application() = default;

// Accessors
int Application::shouldExit()
{
  if (this->window)
    return this->window->getWindowShouldClose();
  return this->isFinished;
}

// Public functions
void Application::update()
{
  // Update context
  this->timeManager.update(this->renderCtx.settings.paused);
  this->renderCtx.deltaTime = this->timeManager.getDeltaTime();

  if (!this->renderCtx.settings.paused)
    this->scene.updatePhysicsWorld(this->renderCtx.deltaTime);

  if (this->cfg.mode == Mode::Simulation)
    this->renderer.update(this->scene, this->renderCtx);
  else if (this->cfg.mode == Mode::EnergyValidation)
    this->updateValidator();

  // Poll events
  if (this->cfg.mode == Mode::Simulation)
  {
    glfwPollEvents();

    this->input.update(this->window->get());

    this->processInput();
  }
}

void Application::render()
{
  if (this->cfg.mode != Mode::Simulation)
    return;

  this->renderer.render(this->scene, this->renderCtx);

  if (this->isTextShown)
  {
    int height = this->window->getHeight();
    int width = this->window->getWidth();
    this->renderer.renderText("FPS: " + std::to_string(this->timeManager.getFPS()),
                              25.f, height - 100.f, .5f, glm::vec3(0.5, 0.8f, 0.2f));
    this->renderer.renderText("Time scale: " + std::to_string(int(this->cfg.timeCfg.timestep)) + " seconds per real second",
                              25.f, height - 150.f, .5f, glm::vec3(0.5, 0.8f, 0.2f));
    this->renderer.renderText("Date: " + this->timeManager.getDate().toString(),
                              25.f, height - 200.f, .5f, glm::vec3(0.5, 0.8f, 0.2f));

    if (this->renderCtx.settings.paused)
      this->renderer.renderText("Paused", width / 2 - 50.f, height - 100.f, .5f, glm::vec3(1.f, 0.8f, 0.2f));
  }

  // Swap buffers
  if (this->cfg.mode == Mode::Simulation)
    this->window->swapBuffers();
}
