#pragma once

struct RotationalElements
{
  double alpha; // Right Ascension of north pole
  double delta; // Declination of north pole
  double alphaDot;
  double deltaDot;
  double W;
  double WDot;

  void advanceFromJD2000(double secondsSinceJD2000)
  {
    double d = secondsSinceJD2000 / 86400.0;
    double T = d / 36525.0;

    this->alpha += this->alphaDot * T;
    this->delta += this->deltaDot * T;
    this->W += this->WDot * d;
  }

  glm::dmat3 calculateOrientation()
  {
    double ca = std::cos(this->alpha);
    double sa = std::sin(this->alpha);
    double cd = std::cos(this->delta);
    double sd = std::sin(this->delta);
    double cw = std::cos(this->W);
    double sw = std::sin(this->W);

    return glm::dmat3{
        -sa * cw - ca * sd * sw, ca * cw - sa * sd * sw, cd * sw,
        sa * sw - ca * sd * cw, -ca * sw - sa * sd * cw, cd * cw,
        ca * cd, sa * cd, sd};
  }

  glm::dvec3 calculateAngularVelocity()
  {
    glm::dvec3 pole(
        cos(this->delta) * cos(this->alpha),
        cos(this->delta) * sin(this->alpha),
        sin(this->delta));

    double omega_mag = this->WDot / 86400.0;
    return pole * omega_mag;
  }

  RotationalElements(double alpha, double delta, double alphaDot, double deltaDot, double W, double WDot)
      : alpha(alpha), delta(delta), alphaDot(alphaDot), deltaDot(deltaDot), W(W), WDot(WDot) {}
};