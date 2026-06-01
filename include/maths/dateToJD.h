#pragma once

#include <string>

struct Date
{
  int day;
  int month;
  int year;
  int hour;
  int minute;
  int second;

  Date(int day, int month, int year, int hour = 0, int minute = 0, int second = 0) : day(day), month(month), year(year), hour(hour), minute(minute), second(second) {}

  std::string toString() const
  {
    return std::to_string(day) + "/" + std::to_string(month) + "/" + std::to_string(year) + " " +
           std::to_string(hour) + ":" + std::to_string(minute) + ":" + std::to_string(second);
  }
};

double dateToJD(const Date &date);
Date JDToDate(double JD);
