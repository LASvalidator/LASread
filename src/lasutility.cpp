/*
===============================================================================

  FILE:  lasutility.cpp
  
  CONTENTS:
  
    see corresponding header file
  
  PROGRAMMERS:

    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com

  COPYRIGHT:

    (c) 2007-2013, martin isenburg, rapidlasso - fast tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING.txt file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    see corresponding header file
  
===============================================================================
*/
#include "lasutility.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LASinventory::LASinventory()
{
  U32 i;
  number_of_point_records = 0;
  for (i = 0; i < 16; i++) number_of_points_by_return[i] = 0;
  for (i = 0; i < 16; i++) number_of_returns_of_given_pulse[i] = 0;
  max_X = min_X = 0;
  max_Y = min_Y = 0;
  max_Z = min_Z = 0;
  min_gps_time = max_gps_time = 0.0;
  max_R = min_R = 0;
  max_G = min_G = 0;
  max_B = min_B = 0;
  max_I = min_I = 0;
  first = TRUE;
}

BOOL LASinventory::add(const LASpoint* point)
{
  number_of_point_records++;
  number_of_points_by_return[point->get_return_number()]++;
  number_of_returns_of_given_pulse[point->get_number_of_returns_of_given_pulse()]++;
  if (first)
  {
    min_X = max_X = point->get_X();
    min_Y = max_Y = point->get_Y();
    min_Z = max_Z = point->get_Z();
    min_gps_time = max_gps_time = point->get_gps_time();
    max_R = min_R = point->get_R();
    max_G = min_G = point->get_G();
    max_B = min_B = point->get_B();
    max_I = min_I = point->get_I();
    first = FALSE;
  }
  else
  {
    if (point->get_X() < min_X) min_X = point->get_X();
    else if (point->get_X() > max_X) max_X = point->get_X();
    if (point->get_Y() < min_Y) min_Y = point->get_Y();
    else if (point->get_Y() > max_Y) max_Y = point->get_Y();
    if (point->get_Z() < min_Z) min_Z = point->get_Z();
    else if (point->get_Z() > max_Z) max_Z = point->get_Z();
    if (point->get_gps_time() < min_gps_time) min_gps_time = point->get_gps_time();
    else if (point->get_gps_time() > max_gps_time) max_gps_time = point->get_gps_time();
    if (point->get_R() < min_R) min_R = point->get_R();
    else if (point->get_R() > max_R) max_R = point->get_R();
    if (point->get_G() < min_G) min_G = point->get_G();
    else if (point->get_G() > max_G) max_G = point->get_G();
    if (point->get_B() < min_B) min_B = point->get_B();
    else if (point->get_B() > max_B) max_B = point->get_B();
    if (point->get_I() < min_I) min_I = point->get_I();
    else if (point->get_I() > max_I) max_I = point->get_I();
  }
  return TRUE;
}
