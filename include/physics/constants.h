#pragma once

#include "physics/structs/keplerElements.h"
#include "physics/structs/materialProperties.h"
#include "physics/structs/radii.h"

#include <glm/glm.hpp>

extern const double EPS;
extern const double G;
extern const double AU_TO_METER;

// extern const double TIME_SCALE;

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

extern const KeplerElements mercuryElements;
extern const KeplerElements venusElements;
extern const KeplerElements earthElements;
extern const KeplerElements moonElements;
extern const KeplerElements marsElements;
extern const KeplerElements jupiterElements;
extern const KeplerElements saturnElements;
extern const KeplerElements uranusElements;
extern const KeplerElements neptuneElements;

extern const PhongMaterialProperties sunMaterial;
extern const PhongMaterialProperties mercuryMaterial;
extern const PhongMaterialProperties venusMaterial;
extern const PhongMaterialProperties earthMaterial;
extern const PhongMaterialProperties moonMaterial;
extern const PhongMaterialProperties marsMaterial;
extern const PhongMaterialProperties jupiterMaterial;
extern const PhongMaterialProperties saturnMaterial;
extern const PhongMaterialProperties uranusMaterial;
extern const PhongMaterialProperties neptuneMaterial;

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

extern const double VISUAL_SCALE;
extern const double VISUAL_RADIUS_SCALE;

// Asteroids
extern const double INNER_ASTEROID_BELT_EDGE;
extern const double OUTER_ASTEROID_BELT_EDGE;

extern const double MINIMUM_ASTEROID_RADIUS;
extern const double MAXIMUM_ASTEROID_RADIUS;

extern const double MINIMUM_ASTEROID_DENSITY;
extern const double MAXIMUM_ASTEROID_DENSITY;

extern const KeplerElements MINIMUM_ASTEROID_ELEMENTS;
extern const KeplerElements MAXIMUM_ASTEROID_ELEMENTS;