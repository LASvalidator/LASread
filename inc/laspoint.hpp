/*
===============================================================================

  FILE:  laspoint.hpp
  
  CONTENTS:
  
    Uses the layout of a LAS 1.4 point to store the superset of all the point
    attributes defined in the ASPRS formats LAS 1.0 through LAS 1.4

      Version 1.4,   Nov 14, 2011.
      Version 1.3,   Oct 24, 2010.
      Version 1.2, April 29, 2008.
      Version 1.1, March 07, 2005.
      Version 1.0,   May 09, 2003

  PROGRAMMERS:

    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com

  COPYRIGHT:

    (c) 2005-2013, martin isenburg, rapidlasso - tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING.txt file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    27 April 2013 -- adapted from the LASzip library for the ASPRS LASvalidator
  
===============================================================================
*/
#ifndef LAS_POINT_HPP
#define LAS_POINT_HPP

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "lasheader.hpp"

class LASpoint
{
public:

  // these funtions provide access to the point attributes

  inline I32 get_X() const { return X; }; // (raw = neither scaled nor translated)
  inline I32 get_Y() const { return Y; }; // (raw = neither scaled nor translated)
  inline I32 get_Z() const { return Z; }; // (raw = neither scaled nor translated)
  inline F64 get_x() const { return quantizer->get_x(X); };
  inline F64 get_y() const { return quantizer->get_y(Y); };
  inline F64 get_z() const { return quantizer->get_z(Z); };
  inline U16 get_intensity() const { return intensity; };
  inline U8 get_return_number() const { return return_number; };
  inline U8 get_number_of_returns_of_given_pulse() const { return number_of_returns_of_given_pulse; };
  inline U8 get_classification_flags() const { return classification_flags; };
  inline U8 get_scanner_channel() const { return scanner_channel; };
  inline U8 get_scan_direction_flag() const { return scan_direction_flag; };
  inline U8 get_edge_of_flight_line() const { return edge_of_flight_line; };
  inline U8 get_classification() const { return classification; };
  inline U8 get_user_data() const { return user_data; };
  inline I16 get_scan_angle() const { return scan_angle; }; // (raw = not scaled by 0.006)
  inline U16 get_point_source_ID() const { return point_source_ID; };
  inline I8 get_scan_angle_rank() const { return scan_angle_rank; };
  inline F64 get_gps_time() const { return gps_time; };
  inline U16 get_R() const { return rgbi[0]; };
  inline U16 get_G() const { return rgbi[1]; };
  inline U16 get_B() const { return rgbi[2]; };
  inline U16 get_I() const { return rgbi[3]; };

  // these funtions provide access to the wave packet attributes

  inline U8 get_wave_packet_index() const { return wave_packet.getIndex(); };
  inline U64 get_wave_packet_offset() const { return wave_packet.getOffset(); };
  inline U32 get_wave_packet_size() const { return wave_packet.getSize(); };
  inline F32 get_wave_packet_location() const { return wave_packet.getLocation(); };
  inline F32 get_wave_packet_Xt() const { return wave_packet.getXt(); };
  inline F32 get_wave_packet_Yt() const { return wave_packet.getYt(); };
  inline F32 get_wave_packet_Zt() const { return wave_packet.getZt(); };

  // generic read access to entire extra bytes array

  inline const U8* get_extra_bytes() const { return extra_bytes; };

  // generic read access to extra bytes attributes

  inline BOOL get_extra_bytes_attribute(I32 index, U8* data) const { if (extra_bytes == 0) return FALSE; memcpy(data, extra_bytes + attributer->extra_attribute_array_offsets[index], attributer->extra_attribute_sizes[index]); return TRUE; };

  // more efficient typed and offset read access to common extra bytes attributes

  inline void get_extra_bytes_attribute(I32 offset, U8 &data) const { data = extra_bytes[offset]; };
  inline void get_extra_bytes_attribute(I32 offset, I8 &data) const { data = (I8)(extra_bytes[offset]); };
  inline void get_extra_bytes_attribute(I32 offset, U16 &data) const { data = *((U16*)(extra_bytes + offset)); };
  inline void get_extra_bytes_attribute(I32 offset, I16 &data) const { data = *((I16*)(extra_bytes + offset)); };
  inline void get_extra_bytes_attribute(I32 offset, U32 &data) const { data = *((U32*)(extra_bytes + offset)); };
  inline void get_extra_bytes_attribute(I32 offset, I32 &data) const { data = *((I32*)(extra_bytes + offset)); };
  inline void get_extra_bytes_attribute(I32 offset, U64 &data) const { data = *((U64*)(extra_bytes + offset)); };
  inline void get_extra_bytes_attribute(I32 offset, I64 &data) const { data = *((I64*)(extra_bytes + offset)); };
  inline void get_extra_bytes_attribute(I32 offset, F32 &data) const { data = *((F32*)(extra_bytes + offset)); };
  inline void get_extra_bytes_attribute(I32 offset, F64 &data) const { data = *((F64*)(extra_bytes + offset)); };

  // these functions describe the point specifics

  inline BOOL has_gps_time() const { return have_gps_time; };
  inline BOOL has_rgb() const { return have_rgb; };
  inline BOOL has_wave_packet() const { return have_wave_packet; };
  inline BOOL has_nir() const { return have_nir; };
  inline BOOL is_las14() const { return are_las14; };
  inline BOOL has_extra_bytes() const { return (BOOL)(number_of_extra_bytes != 0); };
  inline I32 get_number_of_extra_bytes() const { return number_of_extra_bytes; };
  inline U32 get_total_point_size() const { return total_point_size; };

  // these functions set the desired point format

  BOOL init(const LASquantizer* quantizer, const U8 point_type, const U16 point_size, const LASattributer* attributer=0, LASerror* error=0);
  BOOL init(const LASquantizer* quantizer, const U32 num_items, const LASitem* items, const LASattributer* attributer=0, LASerror* error=0);

  // convenience function

  BOOL inside_bounding_box(const F64 min_x, const F64 min_y, const F64 min_z, const F64 max_x, const F64 max_y, const F64 max_z) const;
  BOOL is_zero() const;

  // housekeeping

  void zero();
  void clean();

  LASpoint();
  ~LASpoint();

  // this field provides generic access to the point data

  U8** point;

  // these fields describe the point format terms of generic items

  U16 num_items;
  LASitem* items;

protected:

  // these fields store the point attributes

  I32 X;
  I32 Y;
  I32 Z;
  U16 intensity;
  U8 return_number : 4;
  U8 number_of_returns_of_given_pulse : 4;
  U8 classification_flags : 4;
  U8 scanner_channel : 2;
  U8 scan_direction_flag : 1;
  U8 edge_of_flight_line : 1;
  U8 classification;
  U8 user_data;
  I16 scan_angle;
  U16 point_source_ID;
  I8 scan_angle_rank;
  U8 dummy_for_eight_byte_alignment;

  F64 gps_time;

  U16 rgbi[4];

  LASwavepacket wave_packet;

  U8* extra_bytes;

// these fields describe the point format LAS specific

  BOOL have_gps_time;
  BOOL have_rgb;
  BOOL have_wave_packet;
  BOOL have_nir;
  BOOL are_las14;
  I32 number_of_extra_bytes;
  U32 total_point_size;

// for converting between raw X,Y,Z integer coordinates and scaled/translated x,y,z double coordinates

  const LASquantizer* quantizer;

// for attributed access to the extra bytes

  const LASattributer* attributer;
};

#endif
