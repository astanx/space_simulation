#pragma once

#include <cmath>

struct KeplerElements
{
	double a; // semi-major axis (meters)
	double e; // eccentricity (unitless)

	double i;			// inclination (radians)
	double Omega; // longitude of ascending node (radians)
	double omega; // argument of periapsis (radians)

	double m; // mean anomaly (radians)

	double n; // mean motion (radians per second)

	KeplerElements(
			double a,
			double e,
			double i,
			double Omega,
			double omega,
			double m)
			: a(a), e(e), i(i), Omega(Omega), omega(omega), m(m) {};

	void calculateMeanMotion(double mu)
	{
		this->n = sqrt(mu / (this->a * this->a * this->a));
	}

	void advanceMeanAnomaly(double dt)
	{
		this->m = fmod(this->m + this->n * dt, 2 * M_PI);
		if (this->m < 0)
			this->m += 2 * M_PI;
	}
};
