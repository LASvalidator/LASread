/*
===============================================================================

  FILE:  lasheader.hpp
  
  CONTENTS:
  
    Contains the header definitions for LAS formats 1.0 to 1.4

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
  
    27 April 2013 -- adapted from the LASlib library for the ASPRS LASvalidator
  
===============================================================================
*/
#ifndef LAS_HEADER_HPP
#define LAS_HEADER_HPP

#include "lasdefinitions.hpp"

class ByteStreamIn;

class LASheader : public LASquantizer, public LASattributer
{
public:
  CHAR file_signature[4];
  U16 file_source_id;
  U16 global_encoding;
  U32 project_ID_GUID_data_1;
  U16 project_ID_GUID_data_2;
  U16 project_ID_GUID_data_3;
  CHAR project_ID_GUID_data_4[8];
  U8 version_major;
  U8 version_minor;
  CHAR system_identifier[32];
  CHAR generating_software[32];
  U16 file_creation_day;
  U16 file_creation_year;
  U16 header_size;
  U32 offset_to_point_data;
  U32 number_of_variable_length_records;
  U8 point_data_format;
  U16 point_data_record_length;
  U32 legacy_number_of_point_records;
  U32 legacy_number_of_points_by_return[5];
  F64 max_x;
  F64 min_x;
  F64 max_y;
  F64 min_y;
  F64 max_z;
  F64 min_z;

  // LAS 1.3 and higher only
  U64 start_of_waveform_data_packet_record;

  // LAS 1.4 and higer only
  U64 start_of_first_extended_variable_length_record;
  U32 number_of_extended_variable_length_records;
  U64 number_of_point_records;
  U64 number_of_points_by_return[15];

  U32 user_data_in_header_size;
  U8* user_data_in_header;

  LASvlr* vlrs;
  LASevlr* evlrs;
  LASvlr_geo_keys* vlr_geo_keys;
  LASvlr_key_entry* vlr_geo_key_entries;
  F64* vlr_geo_double_params;
  CHAR* vlr_geo_ascii_params;
  CHAR* vlr_geo_ogc_wkt;
  LASvlr_classification* vlr_classification;
  LASvlr_wave_packet_descr** vlr_wave_packet_descr;

//#ifdef LASZIP_ENABLED
  LASzip* laszip;
//#endif

  U32 user_data_after_header_size;
  U8* user_data_after_header;

  // load one after the other

  BOOL load_header(ByteStreamIn* stream);
  BOOL load_vlrs(ByteStreamIn* stream);
  BOOL load_evlrs(ByteStreamIn* stream);

  // convenience function

  const LASvlr* get_vlr(const char* user_id, U16 record_id) const;

  // housekeeping

  void clean();
  LASheader();
  ~LASheader();
};

#endif
