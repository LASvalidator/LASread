/*
===============================================================================

  FILE:  lasitemreadraw.hpp
  
  CONTENTS:
  
    Implementation of LASitemReadRaw for *all* items that compose a point.

  PROGRAMMERS:

    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com

  COPYRIGHT:

    (c) 2007-2014, martin isenburg, rapidlasso - tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING.txt file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    28 August 2017 -- moving 'context' from global development hack to interface  
    22 April 2013 -- adapted from the LASzip library for the ASPRS LASvalidator
  
===============================================================================
*/
#ifndef LAS_READ_ITEM_RAW_HPP
#define LAS_READ_ITEM_RAW_HPP

#include "lasreaditem.hpp"

#include <assert.h>

typedef struct LASremainderPoint10
{
  U8 return_number : 3;
  U8 number_of_returns_of_given_pulse : 3;
  U8 scan_direction_flag : 1;
  U8 edge_of_flight_line : 1;
  U8 classification : 5;
  U8 classification_flags : 3;
  I8 scan_angle_rank;
  U8 user_data;
  U16 point_source_ID;
} LASremainderPoint10;

typedef struct LASremainderPoint14
{
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
} LASremainderPoint14;

class LASreadItemRaw_POINT10_LE : public LASreadItemRaw
{
public:
  LASreadItemRaw_POINT10_LE(){};
  inline void read(U8* item, U32& context)
  {
    // read the first 14 bytes that are identical between old and new point format 
    instream->getBytes(item, 14);
    // read the remaining 6 bytes of the old format
    instream->getBytes((U8*)&remainderPoint10, 6);
    // get a pointer to the remainder of the new point format
    LASremainderPoint14* remainderPoint14 = (LASremainderPoint14*)(item + 14);
    // copy from old to new
    remainderPoint14->return_number = remainderPoint10.return_number;
    remainderPoint14->number_of_returns_of_given_pulse = remainderPoint10.number_of_returns_of_given_pulse;
    remainderPoint14->classification_flags = remainderPoint10.classification_flags;
    remainderPoint14->scanner_channel = 0;
    remainderPoint14->scan_direction_flag = remainderPoint10.scan_direction_flag;
    remainderPoint14->edge_of_flight_line = remainderPoint10.edge_of_flight_line;
    remainderPoint14->classification = remainderPoint10.classification;
    remainderPoint14->user_data = remainderPoint10.user_data;
    remainderPoint14->scan_angle = I16_QUANTIZE(166.6666666f * remainderPoint10.scan_angle_rank);
    remainderPoint14->point_source_ID = remainderPoint10.point_source_ID;
    remainderPoint14->scan_angle_rank = remainderPoint10.scan_angle_rank;
  }
private:
  LASremainderPoint10 remainderPoint10;
};

class LASreadItemRaw_POINT10_BE : public LASreadItemRaw
{
public:
  LASreadItemRaw_POINT10_BE(){};
  inline void read(U8* item, U32& context)
  {
    instream->getBytes(swapped, 20);
    ENDIAN_SWAP_32(&swapped[ 0], &item[ 0]);    // x
    ENDIAN_SWAP_32(&swapped[ 4], &item[ 4]);    // y
    ENDIAN_SWAP_32(&swapped[ 8], &item[ 8]);    // z
    ENDIAN_SWAP_16(&swapped[12], &item[12]);    // intensity
    *((U32*)&item[14]) = *((U32*)&remainderPoint10); // bitfield, classification, scan_angle_rank, user_data
    ENDIAN_SWAP_16(&swapped[18], &item[20]);    // point_source_ID
    // get a pointer to the remainder of the new point format
    LASremainderPoint14* remainderPoint14 = (LASremainderPoint14*)(item + 14);
    // copy from old to new
    remainderPoint14->return_number = remainderPoint10.return_number;
    remainderPoint14->number_of_returns_of_given_pulse = remainderPoint10.number_of_returns_of_given_pulse;
    remainderPoint14->classification_flags = remainderPoint10.classification_flags;
    remainderPoint14->scanner_channel = 0;
    remainderPoint14->scan_direction_flag = remainderPoint10.scan_direction_flag;
    remainderPoint14->edge_of_flight_line = remainderPoint10.edge_of_flight_line;
    remainderPoint14->classification = remainderPoint10.classification;
    remainderPoint14->user_data = remainderPoint10.user_data;
    remainderPoint14->scan_angle = I16_QUANTIZE(166.6666666f * remainderPoint10.scan_angle_rank);
    remainderPoint14->scan_angle_rank = remainderPoint10.scan_angle_rank;
  };
private:
  U8 swapped[20];
  LASremainderPoint10 remainderPoint10;
};

class LASreadItemRaw_GPSTIME11_LE : public LASreadItemRaw
{
public:
  LASreadItemRaw_GPSTIME11_LE(){};
  inline void read(U8* item, U32& context)
  {
    instream->getBytes(item, 8);
  };
};

class LASreadItemRaw_GPSTIME11_BE : public LASreadItemRaw
{
public:
  LASreadItemRaw_GPSTIME11_BE(){};
  inline void read(U8* item, U32& context)
  {
    instream->getBytes(swapped, 8);
    ENDIAN_SWAP_64(swapped, item);
  };
private:
  U8 swapped[8];
};

class LASreadItemRaw_RGB12_LE : public LASreadItemRaw
{
public:
  LASreadItemRaw_RGB12_LE(){};
  inline void read(U8* item, U32& context)
  {
    instream->getBytes(item, 6);
  };
};

class LASreadItemRaw_RGB12_BE : public LASreadItemRaw
{
public:
  LASreadItemRaw_RGB12_BE(){};
  inline void read(U8* item, U32& context)
  {
    instream->getBytes(swapped, 6);
    ENDIAN_SWAP_32(&swapped[ 0], &item[ 0]); // R
    ENDIAN_SWAP_32(&swapped[ 2], &item[ 2]); // G
    ENDIAN_SWAP_32(&swapped[ 4], &item[ 4]); // B
  };
private:
  U8 swapped[6];
};

class LASreadItemRaw_WAVEPACKET13_LE : public LASreadItemRaw
{
public:
  LASreadItemRaw_WAVEPACKET13_LE(){}
  inline void read(U8* item, U32& context)
  {
    instream->getBytes(item, 29);
  };
};

class LASreadItemRaw_WAVEPACKET13_BE : public LASreadItemRaw
{
public:
  LASreadItemRaw_WAVEPACKET13_BE(){}
  inline void read(U8* item, U32& context)
  {
    instream->getBytes(swapped, 29);
    item[0] = swapped[0];                    // wavepacket descriptor index
    ENDIAN_SWAP_64(&swapped[ 1], &item[ 1]); // byte offset to waveform data
    ENDIAN_SWAP_32(&swapped[ 9], &item[ 9]); // waveform packet size in bytes
    ENDIAN_SWAP_32(&swapped[13], &item[13]); // return point waveform location
    ENDIAN_SWAP_32(&swapped[17], &item[17]); // X(t)
    ENDIAN_SWAP_32(&swapped[21], &item[21]); // Y(t)
    ENDIAN_SWAP_32(&swapped[25], &item[25]); // Z(t)
  };
private:
  U8 swapped[29];
};

class LASreadItemRaw_BYTE : public LASreadItemRaw
{
public:
  LASreadItemRaw_BYTE(U32 number)
  {
    this->number = number;
  }
  inline void read(U8* item, U32& context)
  {
    instream->getBytes(item, number);
  };
private:
  U32 number;
};

class LAStempReadPoint10
{
public:
  I32 x;
  I32 y;
  I32 z;
  U16 intensity;
  U8 return_number : 3;
  U8 number_of_returns_of_given_pulse : 3;
  U8 scan_direction_flag : 1;
  U8 edge_of_flight_line : 1;
  U8 classification;
  I8 scan_angle_rank;
  U8 user_data;
  U16 point_source_ID;
  // LAS 1.4 only
  U8 extended_point_type : 2;
  U8 extended_scanner_channel : 2;
  U8 extended_classification_flags : 4;
  U8 extended_classification;
  U8 extended_return_number : 4;
  U8 extended_number_of_returns_of_given_pulse : 4;
  I16 extended_scan_angle;
  F64 gps_time;
};

class LASreadItemRaw_POINT14_LE : public LASreadItemRaw
{
public:
  LASreadItemRaw_POINT14_LE(){};
  inline void read(U8* item, U32& context)
  {
    instream->getBytes(item, 22);
    instream->getBytes(item+24, 8);
  }
};

class LASreadItemRaw_POINT14_BE : public LASreadItemRaw
{
public:
  LASreadItemRaw_POINT14_BE(){};
  inline void read(U8* item, U32& context)
  {
    instream->getBytes(swapped, 30);
  }
private:
  U8 swapped[30];
};

class LASreadItemRaw_RGBNIR14_LE : public LASreadItemRaw
{
public:
  LASreadItemRaw_RGBNIR14_LE(){};
  inline void read(U8* item, U32& context)
  {
    instream->getBytes(item, 8);
  };
};

class LASreadItemRaw_RGBNIR14_BE : public LASreadItemRaw
{
public:
  LASreadItemRaw_RGBNIR14_BE(){};
  inline void read(U8* item, U32& context)
  {
    instream->getBytes(swapped, 8);
    ENDIAN_SWAP_32(&swapped[ 0], &item[ 0]); // R
    ENDIAN_SWAP_32(&swapped[ 2], &item[ 2]); // G
    ENDIAN_SWAP_32(&swapped[ 4], &item[ 4]); // B
    ENDIAN_SWAP_32(&swapped[ 6], &item[ 6]); // NIR
  };
private:
  U8 swapped[8];
};

#endif
