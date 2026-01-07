#include "physics/constants.h"
#include "physics/keplerElements.h"
#include <glm/glm.hpp>

const double AU_TO_METER = 1.49597870700e11;
// VISUAL SCALING
const double VISUAL_SCALE = 1e-9;
const double VISUAL_RADIUS_SCALE = VISUAL_SCALE * 50.0;

// Precision
const double EPS = 1e-6;

// Gravitational constant
const double G = 6.674e-11;

// Time step
const double TIME_SCALE = 3600.0 * 24 * 365;

// SUN
const double sunMu = 1.3271244004210e20;
const double sunRadius = 6.957e8;
const glm::dvec3 sunPos = glm::dvec3(0.0f, 0.0f, 0.0f);

// MERCURY
const double mercuryMu = 2.2031870799860e13;
const double mercuryRadius = 2.4397e6;
const KeplerElements mercuryElements(
    0.38709927 * AU_TO_METER,
    0.20563593,
    glm::radians(7.00497902),
    glm::radians(48.33076593),
    glm::radians(77.45779628),
    glm::radians(174.796));

// VENUS
const double venusMu = 3.248585926e14;
const double venusRadius = 6.0518e6;
const KeplerElements venusElements(
    0.72333566 * AU_TO_METER,
    0.00677672,
    glm::radians(3.39467605),
    glm::radians(76.67984255),
    glm::radians(131.60246718),
    glm::radians(50.115));

// EARTH
const double earthMu = 3.9860044188e14;
const double earthRadius = 6.371e6;
const KeplerElements earthElements(
    1.000003 * AU_TO_METER,
    0.01671,
    glm::radians(7.155),
    glm::radians(-11.26064),
    glm::radians(114.20783),
    glm::radians(358.617));

// MOON
const double moonMu = 4.902800118e12;
const double moonRadius = 1.7374e6;
const KeplerElements moonElements(
    (1.0 / 389.0) * AU_TO_METER,
    0.0549,
    glm::radians(5.145),
    glm::radians(125.08),
    glm::radians(318.15),
    glm::radians(115.3684));