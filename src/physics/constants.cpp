#include "physics/constants.h"
#include <glm/glm.hpp>

// Precision
const double EPS = 1e-6;

// Gravitational constant 
const double G = 0.0000001; //  6.674e-11 - Scientific value

// Time step
const float TIME_SCALE = 3600.0f * 24.0f; // 1 tick = 1 day

// Masses (arbitrary units, relative to Sun)
const double sunMass = 100.0;
const double earthMass = 3.0;
const double moonMass = 0.04;

// Radii
const float sunRadius = 24.0f;
const float earthRadius = 5.0f;
const float moonRadius = 1.4f;

// Initial positions
const glm::vec3 sunPos = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 earthPos = glm::vec3(400.0f, 0.0f, 0.0f);  
const glm::vec3 moonPos = earthPos + glm::vec3(30.0f, 0.0f, 0.0f); 

// Orbital velocities
const float earthOrbitalPeriod = 365.25f * 24.f * 3600.f;
const float moonOrbitalPeriod = 27.321f * 24.f * 3600.f;
