#pragma once

struct KeplerElements
{
    double a; // semi-major axis (meters)
    double e; // eccentricity (unitless)

    double i;     // inclination (radians)
    double Omega; // longitude of ascending node (radians)
    double omega; // argument of periapsis (radians)

    double m; // mean anomaly (radians)

    KeplerElements(
        double a,
        double e,
        double i,
        double Omega,
        double omega,
        double m)
        : a(a), e(e), i(i), Omega(Omega), omega(omega), m(m) {};
};
