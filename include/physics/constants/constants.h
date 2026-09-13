#pragma once

#include "physics/structs/keplerElements.h"
#include "physics/structs/materialProperties.h"
#include "physics/structs/hapkeParameters.h"
#include "physics/structs/gravityField.h"
#include "physics/structs/tidalParameters.h"
#include "physics/structs/rotationalElements.h"
#include "physics/structs/radii.h"

#include <glm/glm.hpp>

extern const double sunMu;
extern const double mercuryMu;
extern const double venusMu;
extern const double earthMu;
extern const double moonMu;
extern const double marsMu;
extern const double jupiterMu;
extern const double saturnMu;
extern const double uranusMu;
extern const double neptuneMu;

extern const glm::dvec3 sunPos;
extern const double sunLuminosity;

extern const KeplerElements<double> mercuryElements;
extern const KeplerElements<double> venusElements;
extern const KeplerElements<double> earthElements;
extern const KeplerElements<double> moonElements;
extern const KeplerElements<double> marsElements;
extern const KeplerElements<double> jupiterElements;
extern const KeplerElements<double> saturnElements;
extern const KeplerElements<double> uranusElements;
extern const KeplerElements<double> neptuneElements;

extern const RotationalElements sunRotationalElements;
extern const RotationalElements mercuryRotationalElements;
extern const RotationalElements venusRotationalElements;
extern const RotationalElements earthRotationalElements;
extern const RotationalElements moonRotationalElements;
extern const RotationalElements marsRotationalElements;
extern const RotationalElements jupiterRotationalElements;
extern const RotationalElements saturnRotationalElements;
extern const RotationalElements uranusRotationalElements;
extern const RotationalElements neptuneRotationalElements;

extern const GravityField earthGravityField;
extern const GravityField moonGravityField;
extern const GravityField marsGravityField;

extern const TidalParameters earthTidalParameters;
extern const TidalParameters moonTidalParameters;

extern const Radii sunRadii;
extern const Radii mercuryRadii;
extern const Radii venusRadii;
extern const Radii earthRadii;
extern const Radii moonRadii;
extern const Radii marsRadii;
extern const Radii jupiterRadii;
extern const Radii saturnRadii;
extern const Radii uranusRadii;
extern const Radii neptuneRadii;

extern const HapkeParameters moonHapkeParameters;

// Asteroids
extern const double INNER_ASTEROID_BELT_EDGE;
extern const double OUTER_ASTEROID_BELT_EDGE;

extern const double MINIMUM_ASTEROID_RADIUS;
extern const double MAXIMUM_ASTEROID_RADIUS;

extern const double MINIMUM_ASTEROID_DENSITY;
extern const double MAXIMUM_ASTEROID_DENSITY;

extern const KeplerElements<double> MINIMUM_ASTEROID_ELEMENTS;
extern const KeplerElements<double> MAXIMUM_ASTEROID_ELEMENTS;

extern const RotationalElements MINIMUM_ASTEROID_ROTATIONAL_ELEMENTS;
extern const RotationalElements MAXIMUM_ASTEROID_ROTATIONAL_ELEMENTS;