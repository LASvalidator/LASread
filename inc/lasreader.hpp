/*
===============================================================================

  FILE:  lasreader.hpp
  
  CONTENTS:
  
    Reads LIDAR points from LAS/LAZ formats versions 1.0 - 1.4.

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
#ifndef LAS_READER_HPP
#define LAS_READER_HPP

#include "lasheader.hpp"
#include "laspoint.hpp"

#include <stdio.h>

#ifdef LZ_WIN32_VC6
#include <fstream.h>
#else
#include <istream>
#include <fstream>
using namespace std;
#endif

class LASreadPoint;

class LASreader
{
public:
  LASheader header;
  LASpoint point;

  I64 npoints;
  I64 p_count;

  BOOL open(const char* file_name, U32 io_buffer_size=65536);
  BOOL open(FILE* file);
  BOOL open(istream& stream);

  BOOL is_compressed() const;

  inline F64 get_min_x() const { return header.min_x; };
  inline F64 get_min_y() const { return header.min_y; };
  inline F64 get_min_z() const { return header.min_z; };

  inline F64 get_max_x() const { return header.max_x; };
  inline F64 get_max_y() const { return header.max_y; };
  inline F64 get_max_z() const { return header.max_z; };

  BOOL seek(const I64 p_index);
  BOOL read_point();

  inline F64 get_x() const { return point.get_x(); };
  inline F64 get_y() const { return point.get_y(); };
  inline F64 get_z() const { return point.get_z(); };

  void close(BOOL close_stream=TRUE);

  LASreader();
  ~LASreader();

private:
  BOOL open(ByteStreamIn* stream);
  FILE* file;
  ByteStreamIn* stream;
  LASreadPoint* reader;
};

#endif
