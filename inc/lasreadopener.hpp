/*
===============================================================================

  FILE:  lasreadopener.hpp
  
  CONTENTS:
  
    Utility to iterate over and open multiple files in the LAS / LAZ format.

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
  
    27 April 2013 -- adapted from the LASlib library for the ASPRS LASvalidator
  
===============================================================================
*/
#ifndef LAS_READ_OPENER_HPP
#define LAS_READ_OPENER_HPP

#include "lasheader.hpp"
#include "lasreader.hpp"

class ByteStreamIn;
class LASwaveformreader;

class LASreadOpener
{
public:
  void set_io_ibuffer_size(I32 io_ibuffer_size);
  const char* get_path() const;
  const char* get_file_name() const;
  const char* get_directory();
  void set_file_name(const char* file_name, BOOL unique=FALSE);
  BOOL add_file_name(const char* file_name, BOOL unique=FALSE);
  U32 get_file_name_number() const;
  void usage() const;
  BOOL parse(const int argc, char * const argv[]);
  void set_piped(BOOL piped) { use_stdin = piped; };
  BOOL is_piped() const;
  BOOL is_active() const;
  LASreader* open();
  BOOL reopen(LASreader* lasreader);
  LASwaveformreader* open_waveform(const LASheader* lasheader);
  void reset();
  LASreadOpener();
  ~LASreadOpener();
private:
#ifdef _WIN32
  BOOL add_file_name_single(const char* file_name, BOOL unique=FALSE);
#endif
  I32 io_ibuffer_size;
  char** file_names;
  char* file_name;
  char* directory;
  U32 file_name_number;
  U32 file_name_allocated;
  U32 file_name_current;
  BOOL use_stdin;
};

#endif
