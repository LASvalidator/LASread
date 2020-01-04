/*
===============================================================================

  FILE:  laspoint.cpp
  
  CONTENTS:
  
    see corresponding header file

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
  
    see corresponding header file
  
===============================================================================
*/

#include "laspoint.hpp"

BOOL LASpoint::init(const LASquantizer* quantizer, const U8 point_type, const U16 point_size, const LASattributer* attributer, LASerror* error)
{
  // clean the point

  clean();

  // switch over the point types we know

  if (!LASzip().setup(&num_items, &items, point_type, point_size))
  {
    if (error)
    {
      error->add_fail("point type or size", LASzip().get_error());
    }
    else
    {
      fprintf(stderr,"ERROR %s\n", LASzip().get_error());
    }
    return FALSE;
  }

  // create point's item pointers

  point = new U8*[num_items];

  if (point == 0)
  {
    if (error)
    {
      CHAR note[512];
      sprintf(note, "error allocating %lu bytes for %d item pointers", (I32)num_items*sizeof(U8*), (I32)num_items);
      error->add_fail("memory", note);
    }
    else
    {
      fprintf(stderr, "ERROR: allocating %lu bytes for %d item pointers\n", (I32)num_items*sizeof(U8*), (I32)num_items);
    }
    return FALSE;
  }

  U16 i;
  for (i = 0; i < num_items; i++)
  {
    total_point_size += items[i].size;
    switch (items[i].type)
    {
    case LASitem::POINT14:
      have_gps_time = TRUE;
      are_las14 = TRUE;
    case LASitem::POINT10:
      point[i] = (U8*)&X;
      break;
    case LASitem::GPSTIME11:
      have_gps_time = TRUE;
      point[i] = (U8*)&gps_time;
      break;
    case LASitem::RGBNIR14:
      have_nir = TRUE;
    case LASitem::RGB12:
    case LASitem::RGB14:
      have_rgb = TRUE;
      point[i] = (U8*)rgbi;
      break;
    case LASitem::WAVEPACKET13:
    case LASitem::WAVEPACKET14:
      have_wave_packet = TRUE;
      point[i] = (U8*)&wave_packet;
      break;
    case LASitem::BYTE:
    case LASitem::BYTE14:
      number_of_extra_bytes = items[i].size;
      extra_bytes = new U8[number_of_extra_bytes];
      if (extra_bytes == 0)
      {
        if (error)
        {
          CHAR note[512];
          sprintf(note, "error allocating %d bytes for item %d extra_bytes", (I32)number_of_extra_bytes, (I32)i);
          error->add_fail("memory", note);
        }
        else
        {
          fprintf(stderr, "ERROR: allocating %d bytes for item %d extra_bytes\n", (I32)number_of_extra_bytes, (I32)i);
        }
        return FALSE;
      }
      point[i] = extra_bytes;
      break;
    default:
      if (error)
      {
        CHAR note[512];
        sprintf(note, "error item %d has unknown type %d", (I32)i, (I32)items[i].type);
        error->add_fail("internal", note);
      }
      else
      {
        fprintf(stderr, "ERROR: item %d has unknown type %d", (I32)i, (I32)items[i].type);
      }
      return FALSE;
    }
  }
  this->quantizer = quantizer;
  this->attributer = attributer;
  return TRUE;
};

BOOL LASpoint::init(const LASquantizer* quantizer, const U32 num_items, const LASitem* items, const LASattributer* attributer, LASerror* error)
{
  U32 i,e;

  // clean the point

  clean();

  // create item description

  if (this->items) delete [] this->items;
  this->items = new LASitem[num_items];

  if (this->items == 0)
  {
    if (error)
    {
      CHAR note[512];
      sprintf(note, "error allocating %lu bytes for %d LASitems", (I32)num_items*sizeof(LASitem), (I32)num_items);
      error->add_fail("memory", note);
    }
    else
    {
      fprintf(stderr, "ERROR: allocating %lu bytes for %d item pointers\n", (I32)num_items*sizeof(U8*), (I32)num_items);
    }
    return FALSE;
  }

  if (this->point) delete [] this->point;
  this->point = new U8*[num_items];

  if (this->point == 0)
  {
    if (error)
    {
      CHAR note[512];
      sprintf(note, "error allocating %lu bytes for %d item pointers", (I32)num_items*sizeof(U8*), (I32)num_items);
      error->add_fail("memory", note);
    }
    else
    {
      fprintf(stderr, "ERROR: allocating %lu bytes for %d item pointers\n", (I32)num_items*sizeof(U8*), (I32)num_items);
    }
    return FALSE;
  }

  this->num_items = num_items;

  for (i = 0, e = 0; i < num_items; i++)
  {
    this->items[i] = items[i];
    total_point_size += items[i].size;
    switch (items[i].type)
    {
    case LASitem::POINT14:
      have_gps_time = TRUE;
      are_las14 = TRUE;
    case LASitem::POINT10:
      this->point[i] = (U8*)&(this->X);
      break;
    case LASitem::GPSTIME11:
      have_gps_time = TRUE;
      this->point[i] = (U8*)&(this->gps_time);
      break;
    case LASitem::RGBNIR14:
      have_nir = TRUE;
    case LASitem::RGB12:
    case LASitem::RGB14:
      have_rgb = TRUE;
      this->point[i] = (U8*)(this->rgbi);
      break;
    case LASitem::WAVEPACKET13:
    case LASitem::WAVEPACKET14:
      have_wave_packet = TRUE;
      this->point[i] = (U8*)&(this->wave_packet);
      break;
    case LASitem::BYTE:
    case LASitem::BYTE14:
      number_of_extra_bytes = items[i].size;
      extra_bytes = new U8[number_of_extra_bytes];
      if (extra_bytes == 0)
      {
        if (error)
        {
          CHAR note[512];
          sprintf(note, "error allocating %d bytes for item %d extra_bytes", (I32)number_of_extra_bytes, (I32)i);
          error->add_fail("memory", note);
        }
        else
        {
          fprintf(stderr, "ERROR: allocating %d bytes for item %d extra_bytes\n", (I32)number_of_extra_bytes, (I32)i);
        }
        return FALSE;
      }
      this->point[i] = extra_bytes;
      break;
    default:
      if (error)
      {
        CHAR note[512];
        sprintf(note, "error item %d has unknown type %d", (I32)i, (I32)items[i].type);
        error->add_fail("internal", note);
      }
      else
      {
        fprintf(stderr, "ERROR: item %d has unknown type %d", (I32)i, (I32)items[i].type);
      }
      return FALSE;
    }
  }
  this->quantizer = quantizer;
  this->attributer = attributer;
  return TRUE;
};

BOOL LASpoint::inside_bounding_box(const F64 min_x, const F64 min_y, const F64 min_z, const F64 max_x, const F64 max_y, const F64 max_z) const
{
  F64 xyz;
  xyz = get_x();
  if (xyz < min_x || xyz > max_x) return FALSE;
  xyz = get_y();
  if (xyz < min_y || xyz > max_y) return FALSE;
  xyz = get_z();
  if (xyz < min_z || xyz > max_z) return FALSE;
  return TRUE;
}

BOOL LASpoint::is_zero() const
{
  if (X || Y || Z || intensity || classification || user_data || scan_angle || point_source_ID)
  {
    return FALSE;
  }
  if (have_gps_time)
  {
    if (gps_time)
    {
      return FALSE;
    }
  }
  if (have_rgb)
  {
    if (rgbi[0] || rgbi[1] || rgbi[2])
    {
      return FALSE;
    }
    if (have_nir)
    {
      if (rgbi[3])
      {
        return FALSE;
      }
    }
  }
  return TRUE;
}

void LASpoint::zero()
{
  X = 0;
  Y = 0;
  Z = 0;
  intensity = 0;
  number_of_returns_of_given_pulse = 1;
  return_number = 1;
  classification_flags = 0;
  scanner_channel = 0;          // LAS 1.4, point types 6 - 10 only
  edge_of_flight_line = 0;
  scan_direction_flag = 0;
  classification = 0;
  user_data = 0;
  scan_angle = 0;
  point_source_ID = 0;
  scan_angle_rank = 0;
  gps_time_change = 0;          // LAS 1.4, point types 6 - 10 only
  gps_time = 0.0;
  rgbi[0] = rgbi[1] = rgbi[2] = rgbi[3] = 0;
  wave_packet.zero();
};

void LASpoint::clean()
{
  zero();

  if (extra_bytes)
  {
    delete [] extra_bytes;
    extra_bytes = 0;
  };

  if (point) delete [] point;
  point = 0;

  have_gps_time = FALSE;
  have_rgb = FALSE;
  have_wave_packet = FALSE;
  have_nir = FALSE;
  are_las14 = FALSE;
  number_of_extra_bytes = 0;
  total_point_size = 0;
  
  num_items = 0;
  if (items) delete [] items;
  items = 0;
};

LASpoint::LASpoint()
{
  extra_bytes = 0;
  point = 0;
  items = 0;
  clean();
};

LASpoint::~LASpoint()
{
  clean();
};
