#pragma once

#include "physics/keplerElements.h"

#include <glm/glm.hpp>

extern const double EPS;
extern const double G;
extern const double AU_TO_METER;


extern const double TIME_SCALE;

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

extern const double sunRadius;
extern const double mercuryRadius;
extern const double venusRadius;
extern const double earthRadius;
extern const double moonRadius;
extern const double marsRadius;
extern const double jupiterRadius;
extern const double saturnRadius;
extern const double uranusRadius;
extern const double neptuneRadius;

extern const glm::dvec3 sunPos;

extern const KeplerElements mercuryElements;
extern const KeplerElements venusElements;
extern const KeplerElements earthElements;
extern const KeplerElements moonElements;
extern const KeplerElements marsElements;
extern const KeplerElements jupiterElements;
extern const KeplerElements saturnElements;
extern const KeplerElements uranusElements;
extern const KeplerElements neptuneElements;

extern const double VISUAL_SCALE;
extern const double VISUAL_RADIUS_SCALE;

extern const double INNER_ASTEROID_BELT_EDGE;
extern const double OUTER_ASTEROID_BELT_EDGE;

extern const double MINIMUM_ASTEROID_RADIUS;
extern const double MAXIMUM_ASTEROID_RADIUS;