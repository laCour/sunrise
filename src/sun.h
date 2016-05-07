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

#ifndef SUN_H
#define SUN_H

#define _USE_MATH_DEFINES

#define ONE_HOUR 15 // 360° / 24 hrs
#define EARTH_ECCENTRICITY 0.01671022
#define EARTH_OBLIQUITY 0.40911 // 23.44°
#define EARTH_MEAN_MOTION 0.98560912 // 360° / 365.25636 days
#define EARTH_MEAN_ORBITAL_DIFF 3.39384 // ((aphelion / perihelion) - 1) * 100
#define EARTH_EQUATION_OF_CENTER 1.9147
#define SUN_MEAN_LONGITUDE  282.634 // Modified mean lng, unknown equation ≈ mean lng + 0.7%
#define ZENITH_OFFICIAL 90.83333
#define ZENITH_CIVIL 96.0

class Sun {
public:
  Sun(double lat, double lng) : lat(lat), lng(lng) { }
  bool rises();
  long sunrise(int offset=0, bool forceOffset=false, double zenith=ZENITH_OFFICIAL);
  bool sets();
  long sunset(int offset=0, bool forceOffset=false, double zenith=ZENITH_OFFICIAL);
  double brightness(double startZenith=ZENITH_OFFICIAL, double stopZenith=ZENITH_CIVIL);
private:
  double lat, lng, rightAscension;
  double toDeg(double r);
  double toRad(double d);
  double normalise(double x, double s, double e);
  int day();
  long timestampFromHour(double hour, int offset=0);
  double lngHour();
  double trueLng(double approxTime);
  double hourAngle(double trueLng, double zenith);
  double risingTime(int offset);
  double settingTime(int offset);
  double hour(double t, bool s, double zenith);
};

#endif
