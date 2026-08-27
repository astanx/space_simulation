#pragma once

#include "physics/structs/keplerMaths.h"

template <typename real>
struct KeplerElements
{
	real a; // semi-major axis (meters)
	real e; // eccentricity (unitless)

	real i;			// inclination (radians)
	real Omega; // longitude of ascending node (radians)
	real omega; // argument of periapsis (radians)

	real m; // mean anomaly (radians)

	real n; // mean motion (radians per second)

	KeplerElements() = default;
	KeplerElements(
			real a,
			real e,
			real i,
			real Omega,
			real omega,
			real m)
			: a(a), e(e), i(i), Omega(Omega), omega(omega), m(m) {};

	template <typename other>
	KeplerElements(const KeplerElements<other> &el)
			: a(static_cast<real>(el.a)),
				e(static_cast<real>(el.e)),
				i(static_cast<real>(el.i)),
				Omega(static_cast<real>(el.Omega)),
				omega(static_cast<real>(el.omega)),
				m(static_cast<real>(el.m)),
				n(static_cast<real>(el.n))
	{
	}

	void calculateMeanMotion(real mu)
	{
		this->n = ::calculateMeanMotion<real>(mu, this->a);
	}

	void advanceMeanAnomaly(real dt)
	{
		this->m = ::advanceMeanAnomaly<real>(this->m, this->n, dt);
	}
};
