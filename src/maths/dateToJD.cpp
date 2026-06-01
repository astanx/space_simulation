#include "maths/dateToJD.h"
double dateToJD(const Date &date)
{
  int year = date.year;
  int month = date.month;

  if (month <= 2)
  {
    year -= 1;
    month += 12;
  }

  int A = year / 100;
  int B = 2 - A + (A / 4);

  double JD = std::floor(365.25 * (year + 4716)) + std::floor(30.6001 * (month + 1)) + date.day + B - 1524.5;
  JD += (date.hour + date.minute / 60.0 + date.second / 3600.0) / 24.0;

  return JD;
}
Date JDToDate(double JD)
{
  int Z = static_cast<int>(JD + 0.5);
  double F = (JD + 0.5) - Z;
  int A = Z;

  if (Z >= 2299161)
  {
    int alpha = static_cast<int>(((Z - 1867216.25) / 36524.25));
    A += 1 + alpha - (alpha / 4);
  }

  int B = A + 1524;
  int C = static_cast<int>((B - 122.1) / 365.25);
  int D = static_cast<int>(365.25 * C);
  int E = static_cast<int>((B - D) / 30.6001);

  int day = B - D - std::floor(30.6001 * E);
  int month = (E < 14) ? E - 1 : E - 13;
  int year = (month > 2) ? C - 4716 : C - 4715;

  double seconds = F * 86400.0;
  int hour = seconds / 3600;
  int minute = fmod(seconds, 3600) / 60;
  int second = fmod(seconds, 60);

  return Date(static_cast<int>(day), month, year, hour, minute, second);
}