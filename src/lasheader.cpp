/*
===============================================================================

  FILE:  lasheader.cpp
  
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

#include "lasheader.hpp"

#include "bytestreamin.hpp"

void LASheader::clean()
{
  if (user_data_in_header)
  {
    delete [] user_data_in_header;
    user_data_in_header = 0;
    user_data_in_header_size = 0;
  }
  if (vlrs)
  {
    U32 i;
    for (i = 0; i < number_of_variable_length_records; i++)
    {
      if (vlrs[i].data && (vlrs[i].data != (U8*)extra_attributes))
      {
        delete [] vlrs[i].data;
      }
    }
    free(vlrs);
    vlrs = 0;
    geokeys = 0;
    geokey_entries = 0;
    geokey_double_params = 0;
    geokey_ascii_params = 0;
    ogc_wkt = 0;
    classification = 0;
    if (wave_packet_descriptor) delete [] wave_packet_descriptor;
    wave_packet_descriptor = 0;
    number_of_variable_length_records = 0;
  }
  if (evlrs)
  {
    U32 i;
    for (i = 0; i < number_of_extended_variable_length_records; i++)
    {
      if (evlrs[i].data && (evlrs[i].data != (U8*)extra_attributes))
      {
        delete [] evlrs[i].data;
      }
    }
    free(evlrs);
    evlrs = 0;
  }
  if (laszip)
  {
    delete laszip;
    laszip = 0;
  }
  if (user_data_after_header)
  {
    delete [] user_data_after_header;
    user_data_after_header = 0;
    user_data_after_header_size = 0;
  }
  if (number_extra_attributes)
  {
    number_extra_attributes = 0;
    free(extra_attributes); extra_attributes = 0;
    free(extra_attribute_array_offsets); extra_attribute_array_offsets = 0;
    free(extra_attribute_sizes); extra_attribute_sizes = 0;
  }
  memset((void*)this, 0, sizeof(LASheader));
};

BOOL LASheader::load_header(ByteStreamIn* stream)
{
  int i;

  // read the header variable after variable (to avoid alignment issues)

  try { stream->getBytes((U8*)file_signature, 4); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.file_signature\n");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&file_source_id); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.file_source_id\n");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&global_encoding); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.global_encoding\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&project_ID_GUID_data_1); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.project_ID_GUID_data_1\n");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&project_ID_GUID_data_2); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.project_ID_GUID_data_2\n");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&project_ID_GUID_data_3); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.project_ID_GUID_data_3\n");
    return FALSE;
  }
  try { stream->getBytes((U8*)project_ID_GUID_data_4, 8); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.project_ID_GUID_data_4\n");
    return FALSE;
  }
  try { stream->getBytes((U8*)&version_major, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.version_major\n");
    return FALSE;
  }
  try { stream->getBytes((U8*)&version_minor, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.version_minor\n");
    return FALSE;
  }
  try { stream->getBytes((U8*)system_identifier, 32); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.system_identifier\n");
    return FALSE;
  }
  try { stream->getBytes((U8*)generating_software, 32); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.generating_software\n");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&file_creation_day); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.file_creation_day\n");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&file_creation_year); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.file_creation_year\n");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&header_size); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.header_size\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&offset_to_point_data); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.offset_to_point_data\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&number_of_variable_length_records); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.number_of_variable_length_records\n");
    return FALSE;
  }
  try { stream->getBytes((U8*)&point_data_format, 1); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.point_data_format\n");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&point_data_record_length); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.point_data_record_length\n");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&legacy_number_of_point_records); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.legacy_number_of_point_records\n");
    return FALSE;
  }
  for (i = 0; i < 5; i++)
  {
    try { stream->get32bitsLE((U8*)&(legacy_number_of_points_by_return[i])); } catch(...)
    {
      fprintf(stderr,"ERROR: reading header.legacy_number_of_points_by_return %d\n", i);
      return FALSE;
    }
  }
  try { stream->get64bitsLE((U8*)&x_scale_factor); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.x_scale_factor\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&y_scale_factor); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.y_scale_factor\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&z_scale_factor); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.z_scale_factor\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&x_offset); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.x_offset\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&y_offset); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.y_offset\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&z_offset); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.z_offset\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&max_x); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.max_x\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&min_x); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.min_x\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&max_y); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.max_y\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&min_y); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.min_y\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&max_z); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.max_z\n");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&min_z); } catch(...)
  {
    fprintf(stderr,"ERROR: reading header.min_z\n");
    return FALSE;
  }

  // special handling for LAS 1.3
  if ((version_major == 1) && (version_minor >= 3))
  {
    if (header_size < 235)
    {
      fprintf(stderr,"ERROR: for LAS 1.%d header_size should at least be 235 but it is only %d\n", version_minor, header_size);
      return FALSE;
    }
    else
    {
      try { stream->get64bitsLE((U8*)&start_of_waveform_data_packet_record); } catch(...)
      {
        fprintf(stderr,"ERROR: reading header.start_of_waveform_data_packet_record\n");
        return FALSE;
      }
      user_data_in_header_size = header_size - 235;
    }
  }
  else
  {
    user_data_in_header_size = header_size - 227;
  }

  // special handling for LAS 1.4
  if ((version_major == 1) && (version_minor >= 4))
  {
    if (header_size < 375)
    {
      fprintf(stderr,"ERROR: for LAS 1.%d header_size should at least be 375 but it is only %d\n", version_minor, header_size);
      return FALSE;
    }
    else
    {
      try { stream->get64bitsLE((U8*)&start_of_first_extended_variable_length_record); } catch(...)
      {
        fprintf(stderr,"ERROR: reading header.start_of_first_extended_variable_length_record\n");
        return FALSE;
      }
      try { stream->get32bitsLE((U8*)&number_of_extended_variable_length_records); } catch(...)
      {
        fprintf(stderr,"ERROR: reading header.number_of_extended_variable_length_records\n");
        return FALSE;
      }
      try { stream->get64bitsLE((U8*)&number_of_point_records); } catch(...)
      {
        fprintf(stderr,"ERROR: reading header.number_of_point_records\n");
        return FALSE;
      }
      for (i = 0; i < 15; i++)
      {
        try { stream->get64bitsLE((U8*)&(number_of_points_by_return[i])); } catch(...)
        {
          fprintf(stderr,"ERROR: reading header.number_of_points_by_return[%d]\n", i);
          return FALSE;
        }
      }
      user_data_in_header_size = header_size - 375;
    }
  }

  // load any number of user-defined bytes that might have been added to the header
  if (user_data_in_header_size)
  {
    user_data_in_header = new U8[user_data_in_header_size];

    try { stream->getBytes((U8*)user_data_in_header, user_data_in_header_size); } catch(...)
    {
      fprintf(stderr,"ERROR: reading %d bytes of data into header.user_data_in_header\n", user_data_in_header_size);
      return FALSE;
    }
  }

  return TRUE;
};

BOOL LASheader::load_vlrs(ByteStreamIn* stream)
{
  BOOL success = TRUE;

  // read variable length records into the header

  U32 vlrs_size = 0;

  if (number_of_variable_length_records)
  {
    U32 i,j;

    vlrs = (LASvlr*)malloc(sizeof(LASvlr)*number_of_variable_length_records);
 
    for (i = 0; i < number_of_variable_length_records; i++)
    {
      // make sure there are enough bytes left to read a variable length record before the point block starts

      if (((int)offset_to_point_data - vlrs_size - header_size) < 54)
      {
        success = FALSE;
        fprintf(stderr,"WARNING: only %d bytes until point block after reading %d of %d vlrs. skipping remaining vlrs ...\n", (int)offset_to_point_data - vlrs_size - header_size, i, number_of_variable_length_records);
        number_of_variable_length_records = i;
        break;
      }

      // read variable length records variable after variable (to avoid alignment issues)

      try { stream->get16bitsLE((U8*)&(vlrs[i].reserved)); } catch(...)
      {
        fprintf(stderr,"ERROR: reading header.vlrs[%d].reserved\n", i);
        return FALSE;
      }

      try { stream->getBytes((U8*)vlrs[i].user_id, 16); } catch(...)
      {
        fprintf(stderr,"ERROR: reading header.vlrs[%d].user_id\n", i);
        return FALSE;
      }
      try { stream->get16bitsLE((U8*)&(vlrs[i].record_id)); } catch(...)
      {
        fprintf(stderr,"ERROR: reading header.vlrs[%d].record_id\n", i);
        return FALSE;
      }
      try { stream->get16bitsLE((U8*)&(vlrs[i].record_length_after_header)); } catch(...)
      {
        fprintf(stderr,"ERROR: reading header.vlrs[%d].record_length_after_header\n", i);
        return FALSE;
      }
      try { stream->getBytes((U8*)vlrs[i].description, 32); } catch(...)
      {
        fprintf(stderr,"ERROR: reading header.vlrs[%d].description\n", i);
        return FALSE;
      }

      // keep track on the number of bytes we have read so far

      vlrs_size += 54;

      // check variable length record contents

      if (vlrs[i].reserved != 0xAABB)
      {
//        fprintf(stderr,"WARNING: wrong header.vlrs[%d].reserved: %d != 0xAABB\n", i, vlrs[i].reserved);
      }

      // make sure there are enough bytes left to read the data of the variable length record before the point block starts

      if (((int)offset_to_point_data - vlrs_size - header_size) < vlrs[i].record_length_after_header)
      {
        success = FALSE;
        fprintf(stderr,"WARNING: only %d bytes until point block when trying to read %d bytes into header.vlrs[%d].data\n", (int)offset_to_point_data - vlrs_size - header_size, vlrs[i].record_length_after_header, i);
        vlrs[i].record_length_after_header = (int)offset_to_point_data - vlrs_size - header_size;
      }

      // load data following the header of the variable length record

      if (vlrs[i].record_length_after_header)
      {
        if (strcmp(vlrs[i].user_id, "laszip encoded") == 0)
        {
          laszip = new LASzip();

          // read this data following the header of the variable length record
          //     U16  compressor         2 bytes 
          //     U32  coder              2 bytes 
          //     U8   version_major      1 byte 
          //     U8   version_minor      1 byte
          //     U16  version_revision   2 bytes
          //     U32  options            4 bytes 
          //     I32  chunk_size         4 bytes
          //     I64  num_points         8 bytes
          //     I64  num_bytes          8 bytes
          //     U16  num_items          2 bytes
          //        U16 type                2 bytes * num_items
          //        U16 size                2 bytes * num_items
          //        U16 version             2 bytes * num_items
          // which totals 34+6*num_items

          try { stream->get16bitsLE((U8*)&(laszip->compressor)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading compressor %d\n", (I32)laszip->compressor);
            return FALSE;
          }
          try { stream->get16bitsLE((U8*)&(laszip->coder)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading coder %d\n", (I32)laszip->coder);
            return FALSE;
          }
          try { stream->getBytes((U8*)&(laszip->version_major), 1); } catch(...)
          {
            fprintf(stderr,"ERROR: reading version_major %d\n", laszip->version_major);
            return FALSE;
          }
          try { stream->getBytes((U8*)&(laszip->version_minor), 1); } catch(...)
          {
            fprintf(stderr,"ERROR: reading version_minor %d\n", laszip->version_minor);
            return FALSE;
          }
          try { stream->get16bitsLE((U8*)&(laszip->version_revision)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading version_revision %d\n", laszip->version_revision);
            return FALSE;
          }
          try { stream->get32bitsLE((U8*)&(laszip->options)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading options %d\n", (I32)laszip->options);
            return FALSE;
          }
          try { stream->get32bitsLE((U8*)&(laszip->chunk_size)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading chunk_size %d\n", laszip->chunk_size);
            return FALSE;
          }
          try { stream->get64bitsLE((U8*)&(laszip->num_points)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading num_points %d\n", (I32)laszip->num_points);
            return FALSE;
          }
          try { stream->get64bitsLE((U8*)&(laszip->num_bytes)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading num_bytes %d\n", (I32)laszip->num_bytes);
            return FALSE;
          }
          try { stream->get16bitsLE((U8*)&(laszip->num_items)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading num_items %d\n", laszip->num_items);
            return FALSE;
          }
          laszip->items = new LASitem[laszip->num_items];
          for (j = 0; j < laszip->num_items; j++)
          {
            U16 type, size, version;
            try { stream->get16bitsLE((U8*)&type); } catch(...)
            {
              fprintf(stderr,"ERROR: reading type %d of item %d\n", type, j);
              return FALSE;
            }
            try { stream->get16bitsLE((U8*)&size); } catch(...)
            {
              fprintf(stderr,"ERROR: reading size %d of item %d\n", size, j);
              return FALSE;
            }
            try { stream->get16bitsLE((U8*)&version); } catch(...)
            {
              fprintf(stderr,"ERROR: reading version %d of item %d\n", version, j);
              return FALSE;
            }
            laszip->items[j].type = (LASitem::Type)type;
            laszip->items[j].size = size;
            laszip->items[j].version = version;
          }
        }
        else
        {
          vlrs[i].data = new U8[vlrs[i].record_length_after_header];

          try { stream->getBytes(vlrs[i].data, vlrs[i].record_length_after_header); } catch(...)
          {
            fprintf(stderr,"ERROR: reading %d bytes of data into header.vlrs[%d].data\n", vlrs[i].record_length_after_header, i);
            return FALSE;
          }
        }
      }
      else
      {
        vlrs[i].data = 0;
      }

      // keep track on the number of bytes we have read so far

      vlrs_size += vlrs[i].record_length_after_header;

      // special handling for known variable header tags

      if (strcmp(vlrs[i].user_id, "LASF_Projection") == 0)
      {
        if (vlrs[i].data)
        {
          if (vlrs[i].record_id == 34735) // GeoKeyDirectoryTag
          {
            if (geokeys)
            {
              success = FALSE;
              fprintf(stderr,"WARNING: variable length records contain more than one GeoKeyDirectory\n");
            }
            geokeys = (LASgeokeys*)vlrs[i].data;

            // check variable header geo keys contents

            if (geokeys->key_directory_version != 1)
            {
              success = FALSE;
              fprintf(stderr,"WARNING: wrong geokeys->key_directory_version: %d != 1\n",geokeys->key_directory_version);
            }
            if (geokeys->key_revision != 1)
            {
              success = FALSE;
              fprintf(stderr,"WARNING: wrong geokeys->key_revision: %d != 1\n",geokeys->key_revision);
            }
            if (geokeys->minor_revision != 0)
            {
              success = FALSE;
              fprintf(stderr,"WARNING: wrong geokeys->minor_revision: %d != 0\n",geokeys->minor_revision);
            }
            geokey_entries = (LASgeokey_entry*)&geokeys[1];
            if (vlrs[i].record_length_after_header != ((geokeys->number_of_keys+1)*8))
            {
              success = FALSE;
              fprintf(stderr,"WARNING: payload size of %u for GeoKey VLR cannot hold %u + 1 geokey entries\n", (U32)vlrs[i].record_length_after_header, geokeys->number_of_keys);
            }
          }
          else if (vlrs[i].record_id == 34736) // GeoDoubleParamsTag
          {
            if (geokey_double_params)
            {
              success = FALSE;
              fprintf(stderr,"WARNING: variable length records contain more than one GeoDoubleParams\n");
            }
            geokey_double_params = (F64*)vlrs[i].data;
            if ((vlrs[i].record_length_after_header % 8) != 0)
            {
              success = FALSE;
              fprintf(stderr,"WARNING: payload size of %u for GeoDoubleParamsTag VLR is not a multiple of 8\n", (U32)vlrs[i].record_length_after_header);
            }
            geokey_double_params_num = vlrs[i].record_length_after_header / 8;
          }
          else if (vlrs[i].record_id == 34737) // GeoAsciiParamsTag
          {
            if (geokey_ascii_params)
            {
              success = FALSE;
              fprintf(stderr,"WARNING: variable length records contain more than one GeoAsciiParams\n");
            }
            geokey_ascii_params = (CHAR*)vlrs[i].data;
            geokey_ascii_params_num = vlrs[i].record_length_after_header;
          }
          else if (vlrs[i].record_id == 2112) // GeoOCGWKTParamsTag
          {
            if (ogc_wkt)
            {
              success = FALSE;
              fprintf(stderr,"WARNING: variable length records contain more than one OCG WKT\n");
            }
            ogc_wkt = (CHAR*)vlrs[i].data;
            ogc_wkt_num = vlrs[i].record_length_after_header;
          }
        }
        else
        {
          success = FALSE;
          fprintf(stderr,"WARNING: no payload for LASF_Projection (not specification-conform).\n");
        }
      }
      else if (strcmp(vlrs[i].user_id, "LASF_Spec") == 0)
      {
        if (vlrs[i].data)
        {
          if (vlrs[i].record_id == 0) // ClassificationLookup
          {
            if (classification)
            {
              success = FALSE;
              fprintf(stderr,"WARNING: variable length records contain more than one ClassificationLookup\n");
            }
            classification = (LASclassification*)vlrs[i].data;
          }
          else if (vlrs[i].record_id == 2) // Histogram
          {
          }
          else if (vlrs[i].record_id == 3) // TextAreaDescription
          {
          }
          else if (vlrs[i].record_id == 4) // ExtraBytes
          {
            init_extra_attributes(vlrs[i].record_length_after_header/sizeof(LASattribute), (LASattribute*)vlrs[i].data);
          }
          else if ((vlrs[i].record_id >= 100) && (vlrs[i].record_id < 355)) // WavePacketDescriptor
          {
            I32 idx = vlrs[i].record_id - 99;

            if (wave_packet_descriptor == 0)
            {
              wave_packet_descriptor = new LASwave_packet_descriptor*[256];
              for (j = 0; j < 256; j++) wave_packet_descriptor[j] = 0;
            }
            if (wave_packet_descriptor[idx])
            {
              success = FALSE;
              fprintf(stderr,"WARNING: variable length records defines wave packet descr %d more than once\n", idx);
            }
            wave_packet_descriptor[idx] = (LASwave_packet_descriptor*)vlrs[i].data;
          }
        }
        else
        {
          success = FALSE;
          fprintf(stderr,"WARNING: no payload for LASF_Spec (not specification-conform).\n");
        }
      }
      else if (strcmp(vlrs[i].user_id, "laszip encoded") == 0)
      {
        // we take our the VLR for LASzip away
        offset_to_point_data -= (54+vlrs[i].record_length_after_header);
        vlrs_size -= (54+vlrs[i].record_length_after_header);
        i--;
        number_of_variable_length_records--;
      }
    }
  }

  // load any number of user-defined bytes that might have been added after the header

  user_data_after_header_size = (int)offset_to_point_data - vlrs_size - header_size;
  if (user_data_after_header_size)
  {
    user_data_after_header = new U8[user_data_after_header_size];

    try { stream->getBytes((U8*)user_data_after_header, user_data_after_header_size); } catch(...)
    {
      fprintf(stderr,"ERROR: reading %d bytes of data into header.user_data_after_header\n", user_data_after_header_size);
      return FALSE;
    }
  }

  // check the compressor state

  if (laszip)
  {
    if (!laszip->check())
    {
      fprintf(stderr,"ERROR: %s\n", laszip->get_error());
      fprintf(stderr,"       please upgrade to the latest release of LAStools (with LASzip)\n");
      fprintf(stderr,"       or contact 'martin.isenburg@rapidlasso.com' for assistance.\n");
      return FALSE;
    }
  }

  // remove extra bits in point data type

  if ((point_data_format & 128) || (point_data_format & 64)) 
  {
    if (!laszip)
    {
      fprintf(stderr,"ERROR: this file was compressed with an experimental version of laszip\n");
      fprintf(stderr,"       please contact 'martin.isenburg@rapidlasso.com' for assistance.\n");
      return FALSE;
    }
    point_data_format &= 127;
  }

  return success;
};

BOOL LASheader::load_evlrs(ByteStreamIn* stream)
{
  BOOL success = TRUE;

  // special handling for LAS 1.4
  if ((version_major == 1) && (version_minor >= 4))
  {
    if (number_of_extended_variable_length_records)
    {
      if (!stream->isSeekable())
      {
        success = FALSE;
        fprintf(stderr,"WARNING: LAS %d.%d file has %d EVLRs but stream is seekable ...\n", version_major, version_minor, number_of_extended_variable_length_records);
      }
      else
      {
        U32 i,j;

        I64 here = stream->tell();
        stream->seek(start_of_first_extended_variable_length_record);

        evlrs = (LASevlr*)malloc(sizeof(LASevlr)*number_of_extended_variable_length_records);
  
        // read the extended variable length records into the header

        I64 evlrs_size = 0;

        for (i = 0; i < number_of_extended_variable_length_records; i++)
        {
          // read variable length records variable after variable (to avoid alignment issues)

          try { stream->get16bitsLE((U8*)&(evlrs[i].reserved)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading evlrs[%d].reserved\n", i);
            return FALSE;
          }
          try { stream->getBytes((U8*)evlrs[i].user_id, 16); } catch(...)
          {
            fprintf(stderr,"ERROR: reading evlrs[%d].user_id\n", i);
            return FALSE;
          }
          try { stream->get16bitsLE((U8*)&(evlrs[i].record_id)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading evlrs[%d].record_id\n", i);
            return FALSE;
          }
          try { stream->get64bitsLE((U8*)&(evlrs[i].record_length_after_header)); } catch(...)
          {
            fprintf(stderr,"ERROR: reading evlrs[%d].record_length_after_header\n", i);
            return FALSE;
          }
          try { stream->getBytes((U8*)evlrs[i].description, 32); } catch(...)
          {
            fprintf(stderr,"ERROR: reading evlrs[%d].description\n", i);
            return FALSE;
          }

          // keep track on the number of bytes we have read so far

          evlrs_size += 60;

          // check variable length record contents

          if (evlrs[i].reserved != 0xAABB)
          {
            // fprintf(stderr,"WARNING: wrong evlrs[%d].reserved: %d != 0xAABB\n", i, evlrs[i].reserved);
          }

          // load data following the header of the variable length record

          if (evlrs[i].record_length_after_header)
          {
            evlrs[i].data = new U8[(U32)evlrs[i].record_length_after_header];

            try { stream->getBytes(evlrs[i].data, (U32)evlrs[i].record_length_after_header); } catch(...)
            {
              fprintf(stderr,"ERROR: reading %d bytes of data into header.evlrs[%d].data\n", (I32)evlrs[i].record_length_after_header, i);
              return FALSE;
            }
          }
          else
          {
            evlrs[i].data = 0;
          }

          // keep track on the number of bytes we have read so far

          evlrs_size += evlrs[i].record_length_after_header;

          // special handling for known variable header tags

          if (strcmp(evlrs[i].user_id, "LASF_Projection") == 0)
          {
            if (evlrs[i].record_id == 34735) // GeoKeyDirectoryTag
            {
              if (geokeys)
              {
                success = FALSE;
                fprintf(stderr,"WARNING: variable length records contain more than one GeoKeyDirectoryTag\n");
              }
              geokeys = (LASgeokeys*)evlrs[i].data;

              // check variable header geo keys contents

              if (geokeys->key_directory_version != 1)
              {
                success = FALSE;
                fprintf(stderr,"WARNING: wrong geokeys->key_directory_version: %d != 1\n",geokeys->key_directory_version);
              }
              if (geokeys->key_revision != 1)
              {
                success = FALSE;
                fprintf(stderr,"WARNING: wrong geokeys->key_revision: %d != 1\n",geokeys->key_revision);
              }
              if (geokeys->minor_revision != 0)
              {
                success = FALSE;
                fprintf(stderr,"WARNING: wrong geokeys->minor_revision: %d != 0\n",geokeys->minor_revision);
              }
              geokey_entries = (LASgeokey_entry*)&geokeys[1];
            }
            else if (evlrs[i].record_id == 34736) // GeoDoubleParamsTag
            {
              if (geokey_double_params)
              {
                success = FALSE;
                fprintf(stderr,"WARNING: variable length records contain more than one GeoDoubleParamsTag\n");
              }
              geokey_double_params = (F64*)evlrs[i].data;
            }
            else if (evlrs[i].record_id == 34737) // GeoAsciiParamsTag
            {
              if (geokey_ascii_params)
              {
                success = FALSE;
                fprintf(stderr,"WARNING: variable length records contain more than one GeoAsciiParamsTag\n");
              }
              geokey_ascii_params = (CHAR*)evlrs[i].data;
            }
            else if (evlrs[i].record_id == 2112) // GeoOCGWKTParamsTag
            {
              if (ogc_wkt)
              {
                success = FALSE;
                fprintf(stderr,"WARNING: variable length records contain more than one GeoOCGWKTParamsTag\n");
              }
              ogc_wkt = (CHAR*)evlrs[i].data;
            }
          }
          else if (strcmp(evlrs[i].user_id, "LASF_Spec") == 0)
          {
            if (evlrs[i].record_id == 0) // ClassificationLookup
            {
              if (classification)
              {
                success = FALSE;
                fprintf(stderr,"WARNING: variable length records contain more than one ClassificationLookup\n");
              }
              classification = (LASclassification*)evlrs[i].data;
            }
            else if (evlrs[i].record_id == 2) // Histogram
            {
            }
            else if (evlrs[i].record_id == 3) // TextAreaDescription
            {
            }
            else if (evlrs[i].record_id == 4) // ExtraBytes
            {
              init_extra_attributes((U32)evlrs[i].record_length_after_header/sizeof(LASattribute), (LASattribute*)evlrs[i].data);
            }
            else if ((evlrs[i].record_id >= 100) && (evlrs[i].record_id < 355)) // WavePacketDescriptor
            {
              I32 idx = evlrs[i].record_id - 99;

              if (wave_packet_descriptor == 0)
              {
                wave_packet_descriptor = new LASwave_packet_descriptor*[256];
                for (j = 0; j < 256; j++) wave_packet_descriptor[j] = 0;
              }
              if (wave_packet_descriptor[idx])
              {
                success = FALSE;
                fprintf(stderr,"WARNING: extended variable length records defines wave packet descr %d more than once\n", idx);
              }
              wave_packet_descriptor[idx] = (LASwave_packet_descriptor*)evlrs[i].data;
            }
          }
        }
        stream->seek(here);
      }
    }
  }
  return success;
};

const LASvlr* LASheader::get_vlr(const char* user_id, U16 record_id) const
{
  U32 i = 0;
  for (i = 0; i < number_of_variable_length_records; i++)
  {
    if ((strncmp(vlrs[i].user_id, user_id, 16) == 0) && (vlrs[i].record_id == record_id))
    {
      return &(vlrs[i]);
    }
  }
  return 0;
};

LASheader::LASheader()
{
  memset((void*)this, 0, sizeof(LASheader));
};

LASheader::~LASheader()
{
  clean();
};
