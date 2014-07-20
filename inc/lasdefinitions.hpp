/*
===============================================================================

  FILE:  lasdefinitions.hpp
  
  CONTENTS:
  
    Definitions of smaller helpers and data types used in the LASread library

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
#ifndef LAS_DEFINITIONS_HPP
#define LAS_DEFINITIONS_HPP

#define LASREAD_VERSION_MAJOR    1
#define LASREAD_VERSION_MINOR    0
#define LASREAD_BUILD_DATE  140720

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "mydefs.hpp"
#include "laszip.hpp"

#define LASREAD_FORMAT_DEFAULT 0
#define LASREAD_FORMAT_LAS     1
#define LASREAD_FORMAT_LAZ     2

class LASwavepacket
{
public:
  LASwavepacket() { zero(); };
  void zero() { memset(data, 0, 29); };
  inline U8 getIndex() const { return data[0]; };
  inline U64 getOffset() const { return ((U64*)&(data[1]))[0]; };
  inline U32 getSize() const { return ((U32*)&(data[9]))[0]; };
  inline F32 getLocation() const { return ((F32*)&(data[13]))[0]; };
  inline F32 getXt() const { return ((F32*)&(data[17]))[0]; };
  inline F32 getYt() const { return ((F32*)&(data[21]))[0]; };
  inline F32 getZt() const { return ((F32*)&(data[25]))[0]; };
private:
  U8 data[29];
};

class LASerror
{
public:
  LASerror()
  {
    memset((void*)this, 0, sizeof(LASerror));
  };

  ~LASerror()
  {
    clean();
  };

  void clean()
  {
    if (fail_num)
    {
      U32 i;
      for (i = 0; i < fail_num; i++)
      {
        free(fails[i]);
      }
      free(fails);
      fails = 0;
      fail_num = 0;
    }
    if (warning_num)
    {
      U32 i;
      for (i = 0; i < warning_num; i++)
      {
        free(warnings[i]);
      }
      free(warnings);
      warnings = 0;
      warning_num = 0;
    }
    memset((void*)this, 0, sizeof(LASerror));
  };

  void add_fail(const CHAR* fail, const CHAR* note)
  {
    if (fail)
    {
      fails = (CHAR**)realloc(fails, sizeof(CHAR*)*(fail_num+2));
      fails[fail_num] = strdup(fail);
      fail_num++;
      fails[fail_num] = strdup(note);
      fail_num++;
    }
  };

  void add_warning(const CHAR* warning, const CHAR* note)
  {
    if (warning)
    {
      warnings = (CHAR**)realloc(warnings, sizeof(CHAR*)*(warning_num+2));
      warnings[warning_num] = strdup(warning);
      warning_num++;
      warnings[warning_num] = strdup(note);
      warning_num++;
    }
  };

  // keep track of fails and warnings

  U32 fail_num;
  CHAR** fails;

  U32 warning_num;
  CHAR** warnings;
};

class LASquantizer
{
public:
  F64 x_scale_factor;
  F64 y_scale_factor;
  F64 z_scale_factor;
  F64 x_offset;
  F64 y_offset;
  F64 z_offset;

  inline F64 get_x(const I32 x) const { return x_scale_factor*x+x_offset; };
  inline F64 get_y(const I32 y) const { return y_scale_factor*y+y_offset; };
  inline F64 get_z(const I32 z) const { return z_scale_factor*z+z_offset; };

  inline I32 get_x(const F64 x) const { if (x >= x_offset) return (I32)((x-x_offset)/x_scale_factor+0.5); else return (I32)((x-x_offset)/x_scale_factor-0.5); };
  inline I32 get_y(const F64 y) const { if (y >= y_offset) return (I32)((y-y_offset)/y_scale_factor+0.5); else return (I32)((y-y_offset)/y_scale_factor-0.5); };
  inline I32 get_z(const F64 z) const { if (z >= z_offset) return (I32)((z-z_offset)/z_scale_factor+0.5); else return (I32)((z-z_offset)/z_scale_factor-0.5); };

  LASquantizer()
  {
    x_scale_factor = 0.01;
    y_scale_factor = 0.01;
    z_scale_factor = 0.01;
    x_offset = 0.0;
    y_offset = 0.0;
    z_offset = 0.0;
  };
};

#define LAS_ATTRIBUTE_U8  0
#define LAS_ATTRIBUTE_I8  1
#define LAS_ATTRIBUTE_U16 2
#define LAS_ATTRIBUTE_I16 3
#define LAS_ATTRIBUTE_U32 4
#define LAS_ATTRIBUTE_I32 5
#define LAS_ATTRIBUTE_U64 6
#define LAS_ATTRIBUTE_I64 7
#define LAS_ATTRIBUTE_F32 8
#define LAS_ATTRIBUTE_F64 9

class LASattribute
{
public:
  U8 reserved[2];           // 2 bytes
  U8 data_type;             // 1 byte
  U8 options;               // 1 byte
  CHAR name[32];            // 32 bytes
  U8 unused[4];             // 4 bytes
  U64I64F64 no_data[3];     // 24 = 3*8 bytes
  U64I64F64 min[3];         // 24 = 3*8 bytes
  U64I64F64 max[3];         // 24 = 3*8 bytes
  F64 scale[3];             // 24 = 3*8 bytes
  F64 offset[3];            // 24 = 3*8 bytes
  CHAR description[32];     // 32 bytes

  LASattribute(U8 size)
  {
    if (size == 0) throw;
    memset(this, 0, sizeof(LASattribute));
    scale[0] = scale[1] = scale[2] = 1.0;
    this->options = size;
  };

  LASattribute(U32 type, const char* name, const char* description=0, U32 dim=1)
  {
    if (type > LAS_ATTRIBUTE_F64) throw;
    if ((dim < 1) || (dim > 3)) throw;
    if (name == 0) throw;
    memset(this, 0, sizeof(LASattribute));
    scale[0] = scale[1] = scale[2] = 1.0;
    this->data_type = (dim-1)*10+type+1;
    strncpy(this->name, name, 32);
    if (description) strncpy(this->description, description, 32);
  }

  inline BOOL set_no_data(U8 no_data, I32 dim=0) { if ((0 == get_type()) && (dim < get_dim())) { this->no_data[dim].u64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(I8 no_data, I32 dim=0) { if ((1 == get_type()) && (dim < get_dim())) { this->no_data[dim].i64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(U16 no_data, I32 dim=0) { if ((2 == get_type()) && (dim < get_dim())) { this->no_data[dim].u64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(I16 no_data, I32 dim=0) { if ((3 == get_type()) && (dim < get_dim())) { this->no_data[dim].i64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(U32 no_data, I32 dim=0) { if ((4 == get_type()) && (dim < get_dim())) { this->no_data[dim].u64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(I32 no_data, I32 dim=0) { if ((5 == get_type()) && (dim < get_dim())) { this->no_data[dim].i64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(U64 no_data, I32 dim=0) { if ((6 == get_type()) && (dim < get_dim())) { this->no_data[dim].u64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(I64 no_data, I32 dim=0) { if ((7 == get_type()) && (dim < get_dim())) { this->no_data[dim].i64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(F32 no_data, I32 dim=0) { if ((8 == get_type()) && (dim < get_dim())) { this->no_data[dim].f64 = no_data; options |= 0x01; return TRUE; } return FALSE; }
  inline BOOL set_no_data(F64 no_data, I32 dim=0) { if ((9 == get_type()) && (dim < get_dim())) { this->no_data[dim].f64 = no_data; options |= 0x01; return TRUE; } return FALSE; }

  inline void set_min(U8* min, I32 dim=0) { this->min[dim] = cast(min); options |= 0x02; }
  inline void update_min(U8* min, I32 dim=0) { this->min[dim] = smallest(cast(min), this->min[dim]); }
  inline BOOL set_min(U8 min, I32 dim=0) { if ((0 == get_type()) && (dim < get_dim())) { this->min[dim].u64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(I8 min, I32 dim=0) { if ((1 == get_type()) && (dim < get_dim())) { this->min[dim].i64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(U16 min, I32 dim=0) { if ((2 == get_type()) && (dim < get_dim())) { this->min[dim].u64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(I16 min, I32 dim=0) { if ((3 == get_type()) && (dim < get_dim())) { this->min[dim].i64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(U32 min, I32 dim=0) { if ((4 == get_type()) && (dim < get_dim())) { this->min[dim].u64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(I32 min, I32 dim=0) { if ((5 == get_type()) && (dim < get_dim())) { this->min[dim].i64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(U64 min, I32 dim=0) { if ((6 == get_type()) && (dim < get_dim())) { this->min[dim].u64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(I64 min, I32 dim=0) { if ((7 == get_type()) && (dim < get_dim())) { this->min[dim].i64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(F32 min, I32 dim=0) { if ((8 == get_type()) && (dim < get_dim())) { this->min[dim].f64 = min; options |= 0x02; return TRUE; } return FALSE; }
  inline BOOL set_min(F64 min, I32 dim=0) { if ((9 == get_type()) && (dim < get_dim())) { this->min[dim].f64 = min; options |= 0x02; return TRUE; } return FALSE; }

  inline void set_max(U8* max, I32 dim=0) { this->max[dim] = cast(max); options |= 0x04; }
  inline void update_max(U8* max, I32 dim=0) { this->max[dim] = biggest(cast(max), this->max[dim]); }
  inline BOOL set_max(U8 max, I32 dim=0) { if ((0 == get_type()) && (dim < get_dim())) { this->max[dim].u64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(I8 max, I32 dim=0) { if ((1 == get_type()) && (dim < get_dim())) { this->max[dim].i64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(U16 max, I32 dim=0) { if ((2 == get_type()) && (dim < get_dim())) { this->max[dim].u64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(I16 max, I32 dim=0) { if ((3 == get_type()) && (dim < get_dim())) { this->max[dim].i64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(U32 max, I32 dim=0) { if ((4 == get_type()) && (dim < get_dim())) { this->max[dim].u64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(I32 max, I32 dim=0) { if ((5 == get_type()) && (dim < get_dim())) { this->max[dim].i64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(U64 max, I32 dim=0) { if ((6 == get_type()) && (dim < get_dim())) { this->max[dim].u64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(I64 max, I32 dim=0) { if ((7 == get_type()) && (dim < get_dim())) { this->max[dim].i64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(F32 max, I32 dim=0) { if ((8 == get_type()) && (dim < get_dim())) { this->max[dim].f64 = max; options |= 0x04; return TRUE; } return FALSE; }
  inline BOOL set_max(F64 max, I32 dim=0) { if ((9 == get_type()) && (dim < get_dim())) { this->max[dim].f64 = max; options |= 0x04; return TRUE; } return FALSE; }

  inline BOOL set_scale(F64 scale, I32 dim=0) { if (data_type) { this->scale[dim] = scale; options |= 0x08; return TRUE; } return FALSE; }
  inline BOOL set_offset(F64 offset, I32 dim=0) { if (data_type) { this->offset[dim] = offset; options |= 0x10; return TRUE; } return FALSE; }

  inline BOOL has_no_data() const { return options & 0x01; }
  inline BOOL has_min() const { return options & 0x02; }
  inline BOOL has_max() const { return options & 0x04; }
  inline BOOL has_scale() const { return options & 0x08; }
  inline BOOL has_offset() const { return options & 0x10; }

  inline U32 get_size() const
  {
    if (data_type)
    {
      const U32 size_table[10] = { 1, 1, 2, 2, 4, 4, 8, 8, 4, 8 };
      U32 type = get_type();
      U32 dim = get_dim();
      return size_table[type]*dim;
    }
    else
    {
      return options;
    }
  }

private:
  inline I32 get_type() const
  {
    return ((I32)data_type - 1)%10;
  }
  inline I32 get_dim() const
  {
    return 1 + ((I32)data_type - 1)/10;
  }
  inline U64I64F64 cast(U8* value) const
  {
    I32 type = get_type();
    U64I64F64 casted_value;
    if (type == 0)
      casted_value.u64 = *((U8*)value);
    else if (type == 1)
      casted_value.i64 = *((I8*)value);
    else if (type == 2)
      casted_value.u64 = *((U16*)value);
    else if (type == 3)
      casted_value.i64 = *((I16*)value);
    else if (type == 4)
      casted_value.u64 = *((U32*)value);
    else if (type == 5)
      casted_value.i64 = *((I32*)value);
    else if (type == 6)
      casted_value.u64 = *((U64*)value);
    else if (type == 7)
      casted_value.i64 = *((I64*)value);
    else if (type == 8)
      casted_value.f64 = *((F32*)value);
    else
      casted_value.f64 = *((F64*)value);
    return casted_value;
  }
  inline U64I64F64 smallest(U64I64F64 a, U64I64F64 b) const
  {
    I32 type = get_type();
    if (type >= 8) // float compare
    {
      if (a.f64 < b.f64) return a;
      else               return b;
    }
    if (type & 1) // int compare
    {
      if (a.i64 < b.i64) return a;
      else               return b;
    }
    if (a.u64 < b.u64) return a;
    else               return b;
  }
  inline U64I64F64 biggest(U64I64F64 a, U64I64F64 b) const
  {
    I32 type = get_type();
    if (type >= 8) // float compare
    {
      if (a.f64 > b.f64) return a;
      else               return b;
    }
    if (type & 1) // int compare
    {
      if (a.i64 > b.i64) return a;
      else               return b;
    }
    if (a.u64 > b.u64) return a;
    else               return b;
  }
};

class LASattributer
{
public:
  I32 number_extra_attributes;
  LASattribute* extra_attributes;
  I32* extra_attribute_array_offsets;
  I32* extra_attribute_sizes;

  LASattributer()
  {
    number_extra_attributes = 0;
    extra_attributes = 0;
    extra_attribute_array_offsets = 0;
    extra_attribute_sizes = 0;
  }

  ~LASattributer()
  {
    clean_extra_attributes();
  }

  void clean_extra_attributes()
  {
    if (number_extra_attributes)
    {
      number_extra_attributes = 0;
      free(extra_attributes); extra_attributes = 0;
      free(extra_attribute_array_offsets); extra_attribute_array_offsets = 0;
      free(extra_attribute_sizes); extra_attribute_sizes = 0;
    }
  }

  BOOL init_extra_attributes(U32 number_extra_attributes, LASattribute* extra_attributes)
  {
    U32 i;
    clean_extra_attributes();
    this->number_extra_attributes = number_extra_attributes;
    this->extra_attributes = (LASattribute*)malloc(sizeof(LASattribute)*number_extra_attributes);
    memcpy(this->extra_attributes, extra_attributes, sizeof(LASattribute)*number_extra_attributes);
    extra_attribute_array_offsets = (I32*)malloc(sizeof(I32)*number_extra_attributes);
    extra_attribute_sizes = (I32*)malloc(sizeof(I32)*number_extra_attributes);
    extra_attribute_array_offsets[0] = 0;
    extra_attribute_sizes[0] = extra_attributes[0].get_size();
    for (i = 1; i < number_extra_attributes; i++)
    {
      extra_attribute_array_offsets[i] = extra_attribute_array_offsets[i-1] + extra_attribute_sizes[i-1];
      extra_attribute_sizes[i] = extra_attributes[i].get_size();
    }
    return TRUE;
  }

  I32 add_extra_attribute(const LASattribute extra_attribute)
  {
    if (extra_attribute.get_size())
    {
      if (extra_attributes)
      {
        number_extra_attributes++;
        extra_attributes = (LASattribute*)realloc(extra_attributes, sizeof(LASattribute)*number_extra_attributes);
        extra_attribute_array_offsets = (I32*)realloc(extra_attribute_array_offsets, sizeof(I32)*number_extra_attributes);
        extra_attribute_sizes = (I32*)realloc(extra_attribute_sizes, sizeof(I32)*number_extra_attributes);
        extra_attributes[number_extra_attributes-1] = extra_attribute;
        extra_attribute_array_offsets[number_extra_attributes-1] = extra_attribute_array_offsets[number_extra_attributes-2] + extra_attribute_sizes[number_extra_attributes-2];
        extra_attribute_sizes[number_extra_attributes-1] = extra_attributes[number_extra_attributes-1].get_size();
      }
      else
      {
        number_extra_attributes = 1;
        extra_attributes = (LASattribute*)malloc(sizeof(LASattribute));
        extra_attribute_array_offsets = (I32*)malloc(sizeof(I32));
        extra_attribute_sizes = (I32*)malloc(sizeof(I32));
        extra_attributes[0] = extra_attribute;
        extra_attribute_array_offsets[0] = 0;
        extra_attribute_sizes[0] = extra_attributes[0].get_size();
      }
      return number_extra_attributes-1;
    }
    return -1;
  };

  inline I16 get_total_extra_attributes_size() const
  {
    return (extra_attributes ? extra_attribute_array_offsets[number_extra_attributes-1] + extra_attribute_sizes[number_extra_attributes-1] : 0);
  }

  I32 get_extra_attribute_index(const char* name) const
  {
    I32 i;
    for (i = 0; i < number_extra_attributes; i++)
    {
      if (strcmp(extra_attributes[i].name, name) == 0)
      {
        return i;
      }
    }
    return -1;
  }

  I32 get_extra_attribute_array_offset(const char* name) const
  {
    I32 i;
    for (i = 0; i < number_extra_attributes; i++)
    {
      if (strcmp(extra_attributes[i].name, name) == 0)
      {
        return extra_attribute_array_offsets[i];
      }
    }
    return -1;
  }

  I32 get_extra_attribute_array_offset(I32 index) const
  {
    if (index < number_extra_attributes)
    {
      return extra_attribute_array_offsets[index];
    }
    return -1;
  }

  BOOL remove_extra_attribute(I32 index)
  {
    if (index < 0 || index >= number_extra_attributes)
    {
      return FALSE;
    }
    for (index = index + 1; index < number_extra_attributes; index++)
    {
      extra_attributes[index-1] = extra_attributes[index];
      if (index > 1)
      {
        extra_attribute_array_offsets[index-1] = extra_attribute_array_offsets[index-2] + extra_attribute_sizes[index-2];
      }
      else
      {
        extra_attribute_array_offsets[index-1] = 0;
      }
      extra_attribute_sizes[index-1] = extra_attribute_sizes[index];
    }
    number_extra_attributes--;
    if (number_extra_attributes)
    {
      extra_attributes = (LASattribute*)realloc(extra_attributes, sizeof(LASattribute)*number_extra_attributes);
      extra_attribute_array_offsets = (I32*)realloc(extra_attribute_array_offsets, sizeof(I32)*number_extra_attributes);
      extra_attribute_sizes = (I32*)realloc(extra_attribute_sizes, sizeof(I32)*number_extra_attributes);
    }
    else
    {
      free(extra_attributes); extra_attributes = 0;
      free(extra_attribute_array_offsets); extra_attribute_array_offsets = 0;
      free(extra_attribute_sizes); extra_attribute_sizes = 0;
    }
    return TRUE;
  }

  BOOL remove_extra_attribute(const char* name)
  {
    I32 index = get_extra_attribute_index(name);
    if (index != -1)
    { 
      return remove_extra_attribute(index);
    }
    return FALSE;
  }
};

class LASvlr
{
public:
  U16 reserved;
  I8 user_id[16]; 
  U16 record_id;
  U16 record_length_after_header;
  CHAR description[32];
  U8* data;
};

class LASevlr
{
public:
  U16 reserved;
  I8 user_id[16]; 
  U16 record_id;
  I64 record_length_after_header;
  CHAR description[32];
  U8* data;
};

class LASgeokeys
{
public:
  U16 key_directory_version;
  U16 key_revision;
  U16 minor_revision;
  U16 number_of_keys;
};

class LASgeokey_entry
{
public:
  U16 key_id;
  U16 tiff_tag_location;
  U16 count;
  U16 value_offset;
};

class LASclassification
{
public:
  U8 class_number;
  CHAR description[15];
};

class LASwave_packet_descriptor
{
public:
  LASwave_packet_descriptor() {clean();};
  void clean() {memset(data, 0, 26);};
  inline U8 getBitsPerSample() const {return data[0];};
  inline U8 getCompressionType() const {return data[1];};
  inline U32 getNumberOfSamples() const {return ((U32*)&(data[2]))[0];};
  inline U32 getTemporalSpacing() const {return ((U32*)&(data[6]))[0];};
  inline F64 getDigitizerGain() const {return ((F64*)&(data[10]))[0];};
  inline F64 getDigitizerOffset() const {return ((F64*)&(data[18]))[0];};
  inline void setBitsPerSample(U8 bps) {data[0] = bps;};
  inline void setCompressionType(U8 compression) {data[1] = compression;};
  inline void setNumberOfSamples(U32 samples) {((U32*)&(data[2]))[0] = samples;};
  inline void setTemporalSpacing(U32 spacing) {((U32*)&(data[6]))[0] = spacing;};
  inline void setDigitizerGain(F64 gain) {((F64*)&(data[10]))[0] = gain;};
  inline void setDigitizerOffset(F64 offset) {((F64*)&(data[18]))[0] = offset;};
private:
  U8 data[26];
};

#endif
