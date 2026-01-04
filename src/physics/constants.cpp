#include "physics/constants.h"

const double EPS = 1e-6;
const double G = 4.985e5; // Scientific value 6.67430e-11

const double sunMass = 1.988e6;
const double earthMass = 5.972;
const double moonMass = 0.0736;

const float sunRadius = 696.34f;
const float earthRadius = 6.371f;
const float moonRadius = 1.737f;

const float earthOrbitalPeriod = 365.25 * 24 * 60 * 60;
const float moonOrbitalPeriod = 27.321 * 24 * 60 * 60;

const glm::vec3 sunPos(0.f, 0.f, 0.f);
const glm::vec3 earthPos(149600.f, 0.f, 0.f);
const glm::vec3 moonPos = earthPos + glm::vec3(384.4f, 0.f, 0.f);
