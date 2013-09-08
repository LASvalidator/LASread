/*
===============================================================================

  FILE:  laswaveformreader.hpp
  
  CONTENTS:
  
    Interface to read the Waveform Data Packets that are associated with points
    of type 4 and 5 in LAS 1.3 and of type 9 and 10 in LAS 1.4.

  PROGRAMMERS:

    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com

  COPYRIGHT:

    (c) 2005-2013, martin isenburg, rapidlasso - fast tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING.txt file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    27 April 2013 -- adapted from the LASzip library for the ASPRS LASvalidator
  
===============================================================================
*/
#ifndef LAS_WAVEFORM_READER_HPP
#define LAS_WAVEFORM_READER_HPP

#include "laspoint.hpp"

class ByteStreamIn;
class LASwaveformDescription;
class ArithmeticDecoder;
class IntegerCompressor;

class LASwaveformreader
{
public:
  U32 nbits;
  U32 nsamples;
  U32 temporal;
  F32 location;
  F32 XYZt[3];
  F64 XYZreturn[3];

  F64 XYZsample[3];

  U32 s_count;
  U32 sample;

  U32 sampleMin;
  U32 sampleMax;

  U8* samples;

  BOOL open(const char* file_name, I64 start_of_waveform_data_packet_record, const LASwave_packet_descriptor * const * wave_packet_descriptor);
  BOOL is_compressed() const;

  BOOL read_waveform(const LASpoint* point);

  BOOL get_samples();
  BOOL has_samples();

  BOOL get_samples_xyz();
  BOOL has_samples_xyz();

  void close();

  LASwaveformreader();
  ~LASwaveformreader();

private:
  BOOL compressed;
  U32 size;
  const LASwave_packet_descriptor * const * wave_packet_descriptor;
  FILE* file;
  ByteStreamIn* stream;
  I64 start_of_waveform_data_packet_record;
  I64 last_position;
  ArithmeticDecoder* dec;
  IntegerCompressor* ic8;
  IntegerCompressor* ic16;
};

#endif
