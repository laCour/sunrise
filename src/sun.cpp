/*
 * sunrise: https://git.io/sunrise
 * Copyright (C) 2016  Jonathan la Cour
 *
 * This file is part of sunrise.
 *
 * Sunrise is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sunrise is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sunrise.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <time.h>
#include <math.h>
#include "sun.h"

using namespace std;

double Sun::toDeg(double r) { return (180 / M_PI) * r; }

double Sun::toRad(double d) { return (M_PI / 180) * d; }

double Sun::normalise(double x, double s, double e) {
  double width = e - s;
  double offset = x - s;

  return (offset - (floor(offset / width) * width)) + s;
}

int Sun::day() {
  time_t t = time(0);
  struct tm * now = localtime(&t);

  return now->tm_yday;
}

long Sun::timestampFromHour(double hour, int offset) {
  time_t t = time(0);
  struct tm* now = localtime(&t);

  double s, h;
  modf(hour * 60 * 60, &s);

#ifndef AVR
  s += now->tm_gmtoff;
#endif

  now->tm_sec = int(s) % 60;
  now->tm_min = modf(s / 60 / 60, &h) * 60;
  now->tm_hour = h;

  if (h < 0) { now->tm_mday += 1; }

  return mktime(now) + (offset * 86400);
}

double Sun::lngHour() { return lng / ONE_HOUR_DEG; }

double Sun::trueLng(double approxTime) {
  double meanAnomaly = (EARTH_MEAN_MOTION * approxTime) - EARTH_MEAN_ORBITAL_DIFF;
  double eclipticLng = meanAnomaly + (EARTH_EQUATION_OF_CENTER * sin(toRad(meanAnomaly))) + (EARTH_ECCENTRICITY * sin(toRad(2 * meanAnomaly))) + SUN_MEAN_LONGITUDE;
  return normalise(eclipticLng, 0.0, 360.0);
}

double Sun::hourAngle(double trueLng, double zenith) {
  double rA = toDeg(atan(cos(EARTH_OBLIQUITY) * tan(toRad(trueLng))));
  rightAscension = normalise(rA, 0.0, 360.0);

  double lngQuadrant = floor(trueLng / 90) * 90;
  double rAQuadrant = floor(rightAscension / 90) * 90;

  rightAscension = (rightAscension + (lngQuadrant - rAQuadrant)) / ONE_HOUR_DEG;

  double sinDec = EARTH_OBLIQUITY * sin(toRad(trueLng));
  double cosDec = cos(toRad(toDeg(asin(sinDec))));

  return (cos(toRad(zenith)) - (sinDec * sin(toRad(lat)))) / (cosDec * cos(toRad(lat)));
}

double Sun::risingTime(int offset) { return day() + offset + ((6 - lngHour()) / 24); }
double Sun::settingTime(int offset) { return day() + offset + ((18 - lngHour()) / 24); }

double Sun::hour(double t, bool s, double zenith) {
  double l = trueLng(t);
  double hA = hourAngle(l, zenith);

  double hour = (s ? 360 - toDeg(acos(hA)) : toDeg(acos(hA))) / ONE_HOUR_DEG;
  double meanT = hour + rightAscension - (0.06571 * t) - 6.622;

  return normalise(meanT - lngHour(), 0.0, 24.0);
}

bool Sun::rises() {
  double l = trueLng(risingTime(0));
  return (hourAngle(l, ZENITH_OFFICIAL) < 1);
}

long Sun::sunrise(int offset, bool forceOffset, double zenith) {
  double h = hour(risingTime(offset), true, zenith);
  long timestamp = timestampFromHour(h, offset);
  if (timestamp < time(0) && offset < 10 && !forceOffset) { return sunrise(offset + 1, false, zenith); }
  return timestamp;
}

bool Sun::sets() {
  double l = trueLng(settingTime(0));
  return (hourAngle(l, ZENITH_OFFICIAL) > -1);
}

long Sun::sunset(int offset, bool forceOffset, double zenith) {
  double h = hour(settingTime(offset), false, zenith);
  long timestamp = timestampFromHour(h, offset);
  if (timestamp < time(0) && offset < 10 && !forceOffset) { return sunset(offset + 1, false, zenith); }
  return timestamp;
}

double Sun::brightness(double startZenith, double stopZenith) {
  time_t t = time(0);
  long sunriseStart = sunrise(0, true, stopZenith);
  long sunriseStop = sunrise(0, true, startZenith);
  long sunsetStart = sunset(0, true, startZenith);
  long sunsetStop = sunset(0, true, stopZenith);

  if (t > sunriseStart && t < sunriseStop) {
    return 100 - (((double) (sunriseStop - t) / (sunriseStop - sunriseStart)) * 100);
  } else if (t > sunsetStart && t < sunsetStop) {
    return 100 - (((double) (t - sunsetStart) / (sunsetStop - sunsetStart)) * 100);
  }

  if (t < sunsetStart) { return 100; }
  return 0;
}
