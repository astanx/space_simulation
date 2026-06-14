#include "physics/constants.h"

#include "physics/structs/keplerElements.h"
#include "physics/structs/materialProperties.h"
#include "physics/structs/radii.h"

#include <glm/glm.hpp>

const double AU_TO_METER = 1.49597870700e11;

const double INNER_ASTEROID_BELT_EDGE = 2.1 * AU_TO_METER;
const double OUTER_ASTEROID_BELT_EDGE = 3.3 * AU_TO_METER;

const double JD_2000 = 2451545.0;

// Meters
const double MINIMUM_ASTEROID_RADIUS = 1.0;
const double MAXIMUM_ASTEROID_RADIUS = 500000.0;

// kg/m^3
const double MINIMUM_ASTEROID_DENSITY = 1000.0;
const double MAXIMUM_ASTEROID_DENSITY = 8000.0;

const KeplerElements MINIMUM_ASTEROID_ELEMENTS(
		INNER_ASTEROID_BELT_EDGE, // Should not be used
		0.0,
		glm::radians(0.0),
		glm::radians(0.0),
		glm::radians(0.0),
		glm::radians(0.0));

const KeplerElements MAXIMUM_ASTEROID_ELEMENTS(
		OUTER_ASTEROID_BELT_EDGE, // Should not be used
		0.4,
		glm::radians(30.0),
		glm::radians(360.0),
		glm::radians(360.0),
		glm::radians(360.0));

const RotationalElements MINIMUM_ASTEROID_ROTATIONAL_ELEMENTS(
		glm::radians(0.0),
		glm::radians(-90.0),
		glm::radians(-0.1),
		glm::radians(-0.1),
		glm::radians(0.0),
		glm::radians(-3000.0));

const RotationalElements MAXIMUM_ASTEROID_ROTATIONAL_ELEMENTS(
		glm::radians(360.0),
		glm::radians(90.0),
		glm::radians(0.1),
		glm::radians(0.1),
		glm::radians(360.0),
		glm::radians(3000.0));

// VISUAL SCALING
const double VISUAL_SCALE = 1;
const double VISUAL_RADIUS_SCALE = 1;
const double VISUAL_ASTEROID_SCALE = 1;
// const double VISUAL_SCALE = 1e-10;
// const double VISUAL_RADIUS_SCALE = 1e-9;
// const double VISUAL_ASTEROID_SCALE = 3e-3;

// Precision
const double EPS = 1e-6;

// Gravitational constant
const double G = 6.674e-11;

// Time step
// const double TIME_SCALE = 3600.0 * 24;

// SUN
const double sunMu = 1.3271244004210e20;
const Radii sunRadii(
		6.957e8,
		6.957e8,
		6.957e8);
const glm::dvec3 sunPos = glm::dvec3(0.0f, 0.0f, 0.0f);
const double sunLuminosity = 3.826e26f;
const RotationalElements sunRotationalElements{
		glm::radians(286.13),
		glm::radians(63.87),
		glm::radians(0.0),
		glm::radians(0.0),
		glm::radians(84.176),
		glm::radians(14.1844)};

// MERCURY
const double mercuryMu = 2.2031870799860e13;
const Radii mercuryRadii(
		2.4397e6,
		2.4397e6,
		2.4397e6);
const KeplerElements mercuryElements(
		0.38709927 * AU_TO_METER,
		0.20563593,
		glm::radians(7.00497902),
		glm::radians(48.33076593),
		glm::radians(77.45779628),
		glm::radians(174.796));
const RotationalElements mercuryRotationalElements{
		glm::radians(281.0103),
		glm::radians(61.4155),
		glm::radians(-0.0328),
		glm::radians(-0.0049),
		glm::radians(329.59992624081),
		glm::radians(6.1385108)};

// VENUS
const double venusMu = 3.248585926e14;
const Radii venusRadii(
		6.0518e6,
		6.0518e6,
		6.0518e6);
const KeplerElements venusElements(
		0.72333566 * AU_TO_METER,
		0.00677672,
		glm::radians(3.39467605),
		glm::radians(76.67984255),
		glm::radians(131.60246718),
		glm::radians(50.115));
const RotationalElements venusRotationalElements{
		glm::radians(272.76),
		glm::radians(67.16),
		glm::radians(0.0),
		glm::radians(0.0),
		glm::radians(160.2),
		glm::radians(-1.481359)};

// EARTH
const double earthMu = 3.9860044188e14;
const Radii earthRadii(
		6.378137e6,
		6.356752e6,
		6.371e6);
const KeplerElements earthElements(
		1.000003 * AU_TO_METER,
		0.01671,
		glm::radians(7.155),
		glm::radians(-11.26064),
		glm::radians(114.20783),
		glm::radians(358.617));
const RotationalElements earthRotationalElements{
		glm::radians(0.0),
		glm::radians(90.0),
		glm::radians(-0.641),
		glm::radians(-0.557),
		glm::radians(190.147),
		glm::radians(360.9856235)};
const GravityField earthGravityField(
		-4.841693174e-4,
		2.4393836e-6,
		0.3308);

// MOON
const double moonMu = 4.902800118e12;
const Radii moonRadii(
		1.7381e6,
		1.736e6,
		1.7374e6);
const KeplerElements moonElements(
		(1.0 / 389.0) * AU_TO_METER,
		0.0549,
		glm::radians(5.145),
		glm::radians(125.08),
		glm::radians(318.15),
		glm::radians(115.3684));
const RotationalElements moonRotationalElements{
		glm::radians(269.9949),
		glm::radians(66.5392),
		glm::radians(0.0031),
		glm::radians(0.013),
		glm::radians(38.3213),
		glm::radians(13.17635815)};
const GravityField moonGravityField(
		-2.032e-4,
		2.277e-5,
		0.393);

const HapkeParameters moonHapkeParameters(
		0.1f,
		glm::radians(20.f),
		0.05f,
		0.8f,
		0.01f,
		0.2f,
		-0.35f,
		0.5f);

// MARS
const double marsMu = 4.2828372e13;
const Radii marsRadii(
		3.3962e6,
		3.3762e6,
		3.3896e6);
const KeplerElements marsElements(
		1.52368055 * AU_TO_METER,
		0.0934,
		glm::radians(1.85),
		glm::radians(49.57854),
		glm::radians(286.5),
		glm::radians(19.412));
const RotationalElements marsRotationalElements{
		glm::radians(317.68085440731),
		glm::radians(52.88643927513),
		glm::radians(-0.10927547),
		glm::radians(-0.05827105),
		glm::radians(176.63205973192),
		glm::radians(350.891982443297)};
const GravityField marsGravityField(
		-8.745e-4,
		5.086e-5,
		0.366);

// JUPITER
const double jupiterMu = 1.266865349e17;
const Radii jupiterRadii(
		7.1492e7,
		6.684e7,
		6.9911e7);
const KeplerElements jupiterElements(
		5.2038 * AU_TO_METER,
		0.0489,
		glm::radians(1.303),
		glm::radians(100.464),
		glm::radians(273.867),
		glm::radians(20.02));
const RotationalElements jupiterRotationalElements{
		glm::radians(268.0572040427),
		glm::radians(64.4958099534),
		glm::radians(-0.006499),
		glm::radians(0.002413),
		glm::radians(284.95),
		glm::radians(870.536)};
