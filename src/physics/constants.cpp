#include "physics/constants.h"
#include <glm/glm.hpp>

// VISUAL SCALING
const double VISUAL_SCALE = 1e-9;
const double VISUAL_RADIUS_SCALE = VISUAL_SCALE * 25.0;

// Precision
const double EPS = 1e-6;

// Gravitational constant 
const double G = 6.674e-11;

// Time step
const double TIME_SCALE = 3600.0 * 24;

// SUN
const double sunMass = 1.989e30;
const double sunRadius = 6.957e8;
const glm::dvec3 sunPos = glm::dvec3(0.0f, 0.0f, 0.0f);

// MERCURY
const double mercuryMass = 3.3011e23;        
const double mercuryRadius = 2.4397e6;     
const glm::dvec3 mercuryPos = glm::vec3(4.6e10, 0.0, 0.0);
const double mercuryOrbitalPeriod = 87.969 * 24.0 * 3600.0; 
const double mercuryLongitude = 77.456;
const double mercuryInclination = 7.005;

// VENUS
const double venusMass = 4.86732e24;        
const double venusRadius = 6.0518e6;     
const glm::dvec3 venusPos = glm::vec3(1.082e11, 0.0, 0.0);
const double venusOrbitalPeriod = 224.7 * 24.0 * 3600.0; 
const double venusLongitude = 131.602;
const double venusInclination = 3.395;

// EARTH
const double earthMass = 5.97219e24;
const double earthRadius = 6.371e6;
const glm::dvec3 earthPos = glm::dvec3(1.4709845e11, 0.0, 0.0);  
const double earthOrbitalPeriod = 365.25 * 24 * 3600;
const double earthLongitude = 102.937;
const double earthInclination = 0.0001;

// MOON
const double moonMass = 7.346e22;
const double moonRadius = 1.7374e6;
const glm::dvec3 moonPos = earthPos + glm::dvec3(3.626e8, 0.0f, 0.0f); 
const double moonOrbitalPeriod = 27.321 * 24 * 3600;
const double moonLongitude = 18.6;
const double moonInclination = 5.145;