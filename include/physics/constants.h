#pragma once

#include "physics/keplerElements.h"

#include <glm/glm.hpp>

extern const double EPS;
extern const double G;

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

// extern const double mercuryOrbitalPeriod;
// extern const double venusOrbitalPeriod;
// extern const double earthOrbitalPeriod;
// extern const double moonOrbitalPeriod;
// extern const double marsOrbitalPeriod;
// extern const double jupiterOrbitalPeriod;
// extern const double saturnOrbitalPeriod;
// extern const double uranusOrbitalPeriod;
// extern const double neptuneOrbitalPeriod;

// extern const double mercuryLongitude;
// extern const double venusLongitude;
// extern const double earthLongitude;
// extern const double moonLongitude;
// extern const double marsLongitude;
// extern const double jupiterLongitude;
// extern const double saturnLongitude;
// extern const double uranusLongitude;
// extern const double neptuneLongitude;

// extern const double mercuryInclination;
// extern const double venusInclination;
// extern const double earthInclination;
// extern const double moonInclination;
// extern const double marsInclination;
// extern const double jupiterInclination;
// extern const double saturnInclination;
// extern const double uranusInclination;
// extern const double neptuneInclination;

extern const glm::dvec3 sunPos;
// extern const glm::dvec3 mercuryPos;
// extern const glm::dvec3 venusPos;
// extern const glm::dvec3 earthPos;
// extern const glm::dvec3 moonPos;
// extern const glm::dvec3 marsPos;
// extern const glm::dvec3 jupiterPos;
// extern const glm::dvec3 saturnPos;
// extern const glm::dvec3 uranusPos;
// extern const glm::dvec3 neptunePos;


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
