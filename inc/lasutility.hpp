/*
===============================================================================

  FILE:  lasutility.hpp
  
  CONTENTS:
  
    Simple utilities in the LASread library needed for the ASPRS LASvalidator

  PROGRAMMERS:

    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com
  
  COPYRIGHT:
  
    (c) 2007-2013, martin isenburg, rapidlasso - tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING.txt file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    27 April 2013 -- adapted from the LASzip library for the ASPRS LASvalidator
  
===============================================================================
*/
#ifndef LAS_UTILITY_HPP
#define LAS_UTILITY_HPP

#include "laspoint.hpp"

class LASinventory
{
public:
  BOOL is_active() const { return (first == FALSE); }; 
  U64 number_of_point_records;
  U64 number_of_points_by_return[16];
  U64 number_of_returns_of_given_pulse[16];
  U64 return_count_for_return_number[16][16];
  I32 max_X;
  I32 min_X;
  I32 max_Y;
  I32 min_Y;
  I32 max_Z;
  I32 min_Z;
  U16 min_intensity;
  U16 max_intensity;
  U16 min_point_source_ID;
  U16 max_point_source_ID;
  I8 min_scan_angle_rank;
  I8 max_scan_angle_rank;
  I16 min_scan_angle;
  I16 max_scan_angle;
  F64 max_gps_time;
  F64 min_gps_time;
  U16 max_R;
  U16 min_R;
  U16 max_G;
  U16 min_G;
  U16 max_B;
  U16 min_B;
  U16 max_I;
  U16 min_I;
  BOOL add(const LASpoint* point);
  LASinventory();
private:
  BOOL first;
};

#endif
