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
  LASerror::clean();
  LASattributer::clean_extra_attributes();
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
  CHAR note[512];

  // read the header variable after variable (to avoid alignment issues)

  try { stream->getBytes((U8*)file_signature, 4); } catch(...)
  {
    add_fail("file IO", "error reading header.file_signature");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&file_source_ID); } catch(...)
  {
    add_fail("file IO", "error reading header.file_source_ID");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&global_encoding); } catch(...)
  {
    add_fail("file IO", "error reading header.global_encoding");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&project_ID_GUID_data_1); } catch(...)
  {
    add_fail("file IO", "error reading header.project_ID_GUID_data_1");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&project_ID_GUID_data_2); } catch(...)
  {
    add_fail("file IO", "error reading header.project_ID_GUID_data_2");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&project_ID_GUID_data_3); } catch(...)
  {
    add_fail("file IO", "error reading header.project_ID_GUID_data_3");
    return FALSE;
  }
  try { stream->getBytes((U8*)project_ID_GUID_data_4, 8); } catch(...)
  {
    add_fail("file IO", "error reading header.project_ID_GUID_data_4");
    return FALSE;
  }
  try { stream->getBytes((U8*)&version_major, 1); } catch(...)
  {
    add_fail("file IO", "error reading header.version_major");
    return FALSE;
  }
  try { stream->getBytes((U8*)&version_minor, 1); } catch(...)
  {
    add_fail("file IO", "error reading header.version_minor");
    return FALSE;
  }
  try { stream->getBytes((U8*)system_identifier, 32); } catch(...)
  {
    add_fail("file IO", "error reading header.system_identifier");
    return FALSE;
  }
  try { stream->getBytes((U8*)generating_software, 32); } catch(...)
  {
    add_fail("file IO", "error reading header.generating_software");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&file_creation_day); } catch(...)
  {
    add_fail("file IO", "error reading header.file_creation_day");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&file_creation_year); } catch(...)
  {
    add_fail("file IO", "error reading header.file_creation_year");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&header_size); } catch(...)
  {
    add_fail("file IO", "error reading header.header_size");
    return FALSE;
  }
  if (header_size < 227)
  {
    sprintf(note, "the header_size of any LAS file is at least 227 but here it is only %d", (I32)header_size);
    add_fail("header size", note);
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&offset_to_point_data); } catch(...)
  {
    add_fail("file IO", "error reading header.offset_to_point_data");
    return FALSE;
  }
  if (offset_to_point_data < header_size)
  {
    sprintf(note, "the offset_to_point_data %u must be equal or larger than the header_size %d", offset_to_point_data, (I32)header_size);
    add_fail("offset to point data", note);
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&number_of_variable_length_records); } catch(...)
  {
    add_fail("file IO", "error reading header.number_of_variable_length_records");
    return FALSE;
  }
  try { stream->getBytes((U8*)&point_data_format, 1); } catch(...)
  {
    add_fail("file IO", "error reading header.point_data_format");
    return FALSE;
  }
  try { stream->get16bitsLE((U8*)&point_data_record_length); } catch(...)
  {
    add_fail("file IO", "error reading header.point_data_record_length");
    return FALSE;
  }
  try { stream->get32bitsLE((U8*)&legacy_number_of_point_records); } catch(...)
  {
    add_fail("file IO", "error reading header.legacy_number_of_point_records");
    return FALSE;
  }
  for (i = 0; i < 5; i++)
  {
    try { stream->get32bitsLE((U8*)&(legacy_number_of_points_by_return[i])); } catch(...)
    {
      sprintf(note, "error reading header.legacy_number_of_points_by_return %d", i);
      add_fail("file IO", note);
      return FALSE;
    }
  }
  try { stream->get64bitsLE((U8*)&x_scale_factor); } catch(...)
  {
    add_fail("file IO", "error reading header.x_scale_factor");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&y_scale_factor); } catch(...)
  {
    add_fail("file IO", "error reading header.y_scale_factor");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&z_scale_factor); } catch(...)
  {
    add_fail("file IO", "error reading header.z_scale_factor");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&x_offset); } catch(...)
  {
    add_fail("file IO", "error reading header.x_offset");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&y_offset); } catch(...)
  {
    add_fail("file IO", "error reading header.y_offset");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&z_offset); } catch(...)
  {
    add_fail("file IO", "error reading header.z_offset");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&max_x); } catch(...)
  {
    add_fail("file IO", "error reading header.max_x");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&min_x); } catch(...)
  {
    add_fail("file IO", "error reading header.min_x");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&max_y); } catch(...)
  {
    add_fail("file IO", "error reading header.max_y");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&min_y); } catch(...)
  {
    add_fail("file IO", "error reading header.min_y");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&max_z); } catch(...)
  {
    add_fail("file IO", "error reading header.max_z");
    return FALSE;
  }
  try { stream->get64bitsLE((U8*)&min_z); } catch(...)
  {
    add_fail("file IO", "error reading header.min_z");
    return FALSE;
  }

  user_data_in_header_size = header_size - 227;

  // special handling for LAS 1.3
  if ((version_major == 1) && (version_minor >= 3))
  {
    if (header_size < 235)
    {
      sprintf(note, "for LAS 1.%d header_size should at least be 235 but it is only %d", (I32)version_minor, (I32)header_size);
      add_fail("header size", note);
      return FALSE;
    }
    else
    {
      try { stream->get64bitsLE((U8*)&start_of_waveform_data_packet_record); } catch(...)
      {
        add_fail("file IO", "error reading header.start_of_waveform_data_packet_record");
        return FALSE;
      }
      user_data_in_header_size = header_size - 235;
    }
  }

  // special handling for LAS 1.4
  if ((version_major == 1) && (version_minor >= 4))
  {
    if (header_size < 375)
    {
      sprintf(note, "for LAS 1.%d header_size should at least be 375 but it is only %d", (I32)version_minor, (I32)header_size);
      add_fail("header size", note);
      return FALSE;
    }
    else
    {
      try { stream->get64bitsLE((U8*)&start_of_first_extended_variable_length_record); } catch(...)
      {
        add_fail("file IO", "error reading header.start_of_first_extended_variable_length_record");
        return FALSE;
      }
      try { stream->get32bitsLE((U8*)&number_of_extended_variable_length_records); } catch(...)
      {
        add_fail("file IO", "error reading header.number_of_extended_variable_length_records");
        return FALSE;
      }
      try { stream->get64bitsLE((U8*)&number_of_point_records); } catch(...)
      {
        add_fail("file IO", "error reading header.number_of_point_records");
        return FALSE;
      }
      for (i = 0; i < 15; i++)
      {
        try { stream->get64bitsLE((U8*)&(number_of_points_by_return[i])); } catch(...)
        {
          sprintf(note, "error reading header.number_of_points_by_return[%d]", i);
          add_fail("file IO", note);
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

    if (user_data_in_header == 0)
    {
      sprintf(note, "error allocating %u bytes for header.user_data_in_header", user_data_in_header_size);
      add_fail("memory", note);
      return FALSE;
    }

    try { stream->getBytes((U8*)user_data_in_header, user_data_in_header_size); } catch(...)
    {
      sprintf(note, "error reading %u bytes of data into header.user_data_in_header", user_data_in_header_size);
      add_fail("file IO", note);
      return FALSE;
    }
  }
  return TRUE;
};

BOOL LASheader::load_vlrs(ByteStreamIn* stream)
{
  BOOL success = TRUE;
  CHAR note[512];

  U32 vlrs_size = 0;

  if (number_of_variable_length_records)
  {
    U32 i,j;

    // read variable length records into the header

    vlrs = (LASvlr*)malloc(sizeof(LASvlr)*number_of_variable_length_records);
    if (vlrs == 0)
    {
      sprintf(note, "error allocating %lu bytes for %u VLRs", sizeof(LASvlr)*number_of_variable_length_records, number_of_variable_length_records);
      add_fail("memory", note);
      return FALSE;
    }
 
    for (i = 0; i < number_of_variable_length_records; i++)
    {
      // make sure there are enough bytes left to read a variable length record before the point block starts

      if (((I32)offset_to_point_data - vlrs_size - header_size) < 54)
      {
        sprintf(note, "only %d bytes until point block after reading %u of %u VLRs ... skipping", (I32)offset_to_point_data - vlrs_size - header_size, i, number_of_variable_length_records);
        add_fail("VLR", note);
        success = FALSE;
        number_of_variable_length_records = i;
        break;
      }

      // read variable length records variable after variable (to avoid alignment issues)

      try { stream->get16bitsLE((U8*)&(vlrs[i].reserved)); } catch(...)
      {
        sprintf(note, "error reading header.vlrs[%u].reserved", i);
        add_fail("file IO", note);
        return FALSE;
      }

      try { stream->getBytes((U8*)vlrs[i].user_id, 16); } catch(...)
      {
        sprintf(note, "error reading header.vlrs[%u].user_id", i);
        add_fail("file IO", note);
        return FALSE;
      }
      try { stream->get16bitsLE((U8*)&(vlrs[i].record_id)); } catch(...)
      {
        sprintf(note, "error reading header.vlrs[%u].record_id", i);
        add_fail("file IO", note);
        return FALSE;
      }
      try { stream->get16bitsLE((U8*)&(vlrs[i].record_length_after_header)); } catch(...)
      {
        sprintf(note, "error reading header.vlrs[%u].record_length_after_header", i);
        add_fail("file IO", note);
        return FALSE;
      }
      try { stream->getBytes((U8*)vlrs[i].description, 32); } catch(...)
      {
        sprintf(note, "error reading header.vlrs[%u].description", i);
        add_fail("file IO", note);
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

      if (U32_CLAMP(offset_to_point_data - vlrs_size - header_size) < vlrs[i].record_length_after_header)
      {
        sprintf(note, "only %d bytes left until point block when reading payload of %d bytes of header.vlrs[%u] ... truncating", (I32)offset_to_point_data - vlrs_size - header_size, (I32)vlrs[i].record_length_after_header, i);
        add_fail("VLR", note);
        success = FALSE;
        vlrs[i].record_length_after_header = U16_CLAMP(offset_to_point_data - vlrs_size - header_size);
      }

      // load data following the header of the variable length record

      if (vlrs[i].record_length_after_header)
      {
        if (strcmp(vlrs[i].user_id, "laszip encoded") == 0)
        {
          laszip = new LASzip();

          // read this data following the header of the variable length record
          //     U16  compressor                2 bytes 
          //     U32  coder                     2 bytes 
          //     U8   version_major             1 byte 
          //     U8   version_minor             1 byte
          //     U16  version_revision          2 bytes
          //     U32  options                   4 bytes 
          //     I32  chunk_size                4 bytes
          //     I64  number_of_special_evlrs   8 bytes
          //     I64  offset_to_special_evlrs   8 bytes
          //     U16  num_items                 2 bytes
          //        U16 type                2 bytes * num_items
          //        U16 size                2 bytes * num_items
          //        U16 version             2 bytes * num_items
          // which totals 34+6*num_items

          try { stream->get16bitsLE((U8*)&(laszip->compressor)); } catch(...)
          {
            add_fail("file IO", "error reading VLR laszip->compressor");
            return FALSE;
          }
          try { stream->get16bitsLE((U8*)&(laszip->coder)); } catch(...)
          {
            add_fail("file IO", "error reading VLR laszip->coder");
            return FALSE;
          }
          try { stream->getBytes((U8*)&(laszip->version_major), 1); } catch(...)
          {
            add_fail("file IO", "error reading VLR laszip->version_major");
            return FALSE;
          }
          try { stream->getBytes((U8*)&(laszip->version_minor), 1); } catch(...)
          {
            add_fail("file IO", "error reading VLR laszip->version_minor");
            return FALSE;
          }
          try { stream->get16bitsLE((U8*)&(laszip->version_revision)); } catch(...)
          {
            add_fail("file IO", "error reading VLR laszip->version_revision");
            return FALSE;
          }
          try { stream->get32bitsLE((U8*)&(laszip->options)); } catch(...)
          {
            add_fail("file IO", "error reading VLR laszip->options");
            return FALSE;
          }
          try { stream->get32bitsLE((U8*)&(laszip->chunk_size)); } catch(...)
          {
            add_fail("file IO", "error reading VLR laszip->chunk_size");
            return FALSE;
          }
          try { stream->get64bitsLE((U8*)&(laszip->number_of_special_evlrs)); } catch(...)
          {
            add_fail("file IO", "error reading VLR laszip->number_of_special_evlrs");
            return FALSE;
          }
          try { stream->get64bitsLE((U8*)&(laszip->offset_to_special_evlrs)); } catch(...)
          {
            add_fail("file IO", "error reading VLR laszip->offset_to_special_evlrs");
            return FALSE;
          }
          try { stream->get16bitsLE((U8*)&(laszip->num_items)); } catch(...)
          {
            add_fail("file IO", "error reading VLR laszip->num_items");
            return FALSE;
          }
          laszip->items = new LASitem[laszip->num_items];
          for (j = 0; j < laszip->num_items; j++)
          {
            U16 type, size, version;
            try { stream->get16bitsLE((U8*)&type); } catch(...)
            {
              sprintf(note, "error reading VLR laszip->items[%u].type", j);
              add_fail("file IO", note);
              return FALSE;
            }
            try { stream->get16bitsLE((U8*)&size); } catch(...)
            {
              sprintf(note, "error reading VLR laszip->items[%u].size", j);
              add_fail("file IO", note);
              return FALSE;
            }
            try { stream->get16bitsLE((U8*)&version); } catch(...)
            {
              sprintf(note, "error reading VLR laszip->items[%u].version", j);
              add_fail("file IO", note);
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

          if (vlrs[i].data == 0)
          {
            sprintf(note, "error allocating %d bytes for header.vlrs[%u].data", (I32)vlrs[i].record_length_after_header, i);
            add_fail("memory", note);
            return FALSE;
          }

          try { stream->getBytes(vlrs[i].data, vlrs[i].record_length_after_header); } catch(...)
          {
            sprintf(note, "reading %d bytes of data into header.vlrs[%u].data", (I32)vlrs[i].record_length_after_header, i);
            add_fail("file IO", note);
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
              add_fail("VLR", "variable length records contain more than one GeoKeyDirectory");
              success = FALSE;
            }
            geokeys = (LASgeokeys*)vlrs[i].data;

            // check variable header geo keys contents

            if (geokeys->key_directory_version != 1)
            {
              sprintf(note, "wrong geokeys->key_directory_version: %d != 1", (I32)geokeys->key_directory_version);
              add_fail("VLR", note);
              success = FALSE;
            }
            if (geokeys->key_revision != 1)
            {
              sprintf(note, "wrong geokeys->key_revision: %d != 1", (I32)geokeys->key_revision);
              add_fail("VLR", note);
              success = FALSE;
            }
            if (geokeys->minor_revision != 0)
            {
              sprintf(note, "wrong geokeys->minor_revision: %d != 1", (I32)geokeys->minor_revision);
              add_fail("VLR", note);
              success = FALSE;
            }
            geokey_entries = (LASgeokey_entry*)&geokeys[1];
            if (vlrs[i].record_length_after_header != ((geokeys->number_of_keys+1)*8))
            {
              sprintf(note, "payload size of %d for geokey VLR cannot hold %u + 1 geokey entries", (I32)vlrs[i].record_length_after_header, geokeys->number_of_keys);
              add_fail("VLR", note);
              success = FALSE;
            }
          }
          else if (vlrs[i].record_id == 34736) // GeoDoubleParamsTag
          {
            if (geokey_double_params)
            {
              add_fail("VLR", "variable length records contain more than one GeoDoubleParamsTag");
              success = FALSE;
            }
            geokey_double_params = (F64*)vlrs[i].data;
            if ((vlrs[i].record_length_after_header % 8) != 0)
            {
              sprintf(note, "payload size of %d for GeokeyDoubleParams VLR is not a multiple of 8", (I32)vlrs[i].record_length_after_header);
              add_fail("VLR", note);
              success = FALSE;
            }
            geokey_double_params_num = vlrs[i].record_length_after_header / 8;
          }
          else if (vlrs[i].record_id == 34737) // GeoAsciiParamsTag
          {
            if (geokey_ascii_params)
            {
              add_fail("VLR", "variable length records contain more than one GeoAsciiParamsTag");
              success = FALSE;
            }
            geokey_ascii_params = (CHAR*)vlrs[i].data;
            geokey_ascii_params_num = vlrs[i].record_length_after_header;
          }
          else if (vlrs[i].record_id == 2112) // GeoOCGWKTParamsTag
          {
            if (ogc_wkt)
            {
              add_fail("VLR", "variable length records contain more than one GeoOCGWKTParamsTag");
              success = FALSE;
            }
            ogc_wkt = (CHAR*)vlrs[i].data;
            ogc_wkt_num = vlrs[i].record_length_after_header;
          }
        }
        else
        {
          add_fail("VLR", "no payload for LASF_Projection VLR");
          success = FALSE;
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
              add_fail("VLR", "variable length records contain more than one ClassificationLookup");
              success = FALSE;
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
              sprintf(note, "variable length records defines wave packet descriptor %d more than once", idx);
              add_fail("VLR", note);
              success = FALSE;
            }
            if (vlrs[i].record_length_after_header != 26)
            {
              sprintf(note, "variable length record payload for wave packet descr %d is %d instead of 26 bytes\n", idx, (I32)vlrs[i].record_length_after_header);
              add_fail("VLR", note);
              success = FALSE;
            }

            wave_packet_descriptor[idx] = (LASwave_packet_descriptor*)vlrs[i].data;
            if ((wave_packet_descriptor[idx]->getBitsPerSample() != 8) && (wave_packet_descriptor[idx]->getBitsPerSample() != 16))
            {
              sprintf(note, "bits per sample for descriptor %d is %d instead of 8 or 16\n", idx, (I32)wave_packet_descriptor[idx]->getBitsPerSample());
              add_warning("wave packet descriptor", note);
            }
            if (wave_packet_descriptor[idx]->getNumberOfSamples() == 0)
            {
              sprintf(note, "number of samples for descriptor %d is zero", idx);
              add_warning("wave packet descriptor", note);
            }
            if (wave_packet_descriptor[idx]->getTemporalSpacing() == 0)
            {
              sprintf(note, "temporal spacing for descriptor %d is zero", idx);
              add_warning("wave packet descriptor", note);
            }
          }
          else
          {
            sprintf(note, "variable length records define LASF_Spec with unknown record ID %d", vlrs[i].record_id);
            success = FALSE;
          }
        }
        else
        {
          add_fail("VLR", "no payload for LASF_Spec VLR");
          success = FALSE;
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

  user_data_after_header_size = U32_CLAMP(offset_to_point_data - vlrs_size - header_size);
  if (user_data_after_header_size)
  {
    user_data_after_header = new U8[user_data_after_header_size];

    if (user_data_after_header == 0)
    {
      sprintf(note, "error allocating %u bytes for header.user_data_after_header", user_data_after_header_size);
      add_fail("memory", note);
      return FALSE;
    }

    try { stream->getBytes((U8*)user_data_after_header, user_data_after_header_size); } catch(...)
    {
      sprintf(note, "reading %u bytes of data into header.user_data_after_header", user_data_after_header_size);
      add_fail("loading_header", note);
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
  CHAR note[512];

  // special handling for LAS 1.4

  if ((version_major == 1) && (version_minor >= 4))
  {
    if (number_of_extended_variable_length_records)
    {
      if (!stream->isSeekable())
      {
        sprintf(note, "LAS %d.%d file has %u EVLRs but stream is not seekable", (I32)version_major, (I32)version_minor, number_of_extended_variable_length_records);
        add_warning("file IO", note);
      }
      else
      {
        U32 i,j;

        I64 here = stream->tell();
        stream->seek(start_of_first_extended_variable_length_record);

        // read the extended variable length records into the header

        evlrs = (LASevlr*)malloc(sizeof(LASevlr)*number_of_extended_variable_length_records);
        if (evlrs == 0)
        {
          sprintf(note, "error allocating %lu bytes for %u EVLRs", sizeof(LASevlr)*number_of_extended_variable_length_records, number_of_extended_variable_length_records);
          add_fail("memory", note);
          return FALSE;
        }

        I64 evlrs_size = 0;

        for (i = 0; i < number_of_extended_variable_length_records; i++)
        {
          // read variable length records variable after variable (to avoid alignment issues)

          try { stream->get16bitsLE((U8*)&(evlrs[i].reserved)); } catch(...)
          {
            sprintf(note, "error reading evlrs[%u].reserved", i);
            add_fail("file IO", note);
            return FALSE;
          }
          try { stream->getBytes((U8*)evlrs[i].user_id, 16); } catch(...)
          {
            sprintf(note, "error reading evlrs[%u].user_id", i);
            add_fail("file IO", note);
            return FALSE;
          }
          try { stream->get16bitsLE((U8*)&(evlrs[i].record_id)); } catch(...)
          {
            sprintf(note, "error reading evlrs[%u].record_id", i);
            add_fail("file IO", note);
            return FALSE;
          }
          try { stream->get64bitsLE((U8*)&(evlrs[i].record_length_after_header)); } catch(...)
          {
            sprintf(note, "error reading evlrs[%u].record_length_after_header", i);
            add_fail("file IO", note);
            return FALSE;
          }
          try { stream->getBytes((U8*)evlrs[i].description, 32); } catch(...)
          {
            sprintf(note, "error reading evlrs[%u].description", i);
            add_fail("file IO", note);
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
            if (evlrs[i].record_length_after_header > U32_MAX)
            {
#ifdef _WIN32
              sprintf(note, "gigantic payload of %I64d bytes for header.evlrs[%u]", evlrs[i].record_length_after_header, i);
#else
              sprintf(note, "gigantic payload of %lld bytes for header.evlrs[%u]", evlrs[i].record_length_after_header, i);
#endif
              add_warning("EVLR", note);
            }

            evlrs[i].data = new U8[(U32)evlrs[i].record_length_after_header];

            if (evlrs[i].data == 0)
            {
              sprintf(note, "error allocating %u bytes of data for header.evlrs[%u].data", (U32)evlrs[i].record_length_after_header, i);
              add_fail("memory", note);
              return FALSE;
            }

            try { stream->getBytes(evlrs[i].data, (U32)evlrs[i].record_length_after_header); } catch(...)
            {
              sprintf(note, "reading %u bytes of data into header.evlrs[%u].data", (U32)evlrs[i].record_length_after_header, i);
              add_fail("file IO", note);
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
                add_fail("EVLR", "extended variable length records contain more than one GeoKeyDirectory");
                success = FALSE;
              }
              geokeys = (LASgeokeys*)evlrs[i].data;

              // check variable header geo keys contents

              if (geokeys->key_directory_version != 1)
              {
                sprintf(note, "wrong geokeys->key_directory_version: %d != 1", (I32)geokeys->key_directory_version);
                add_fail("EVLR", note);
                success = FALSE;
              }
              if (geokeys->key_revision != 1)
              {
                sprintf(note, "wrong geokeys->key_revision: %d != 1", (I32)geokeys->key_revision);
                add_fail("EVLR", note);
                success = FALSE;
              }
              if (geokeys->minor_revision != 0)
              {
                sprintf(note, "wrong geokeys->minor_revision: %d != 1", (I32)geokeys->minor_revision);
                add_fail("EVLR", note);
                success = FALSE;
              }
              geokey_entries = (LASgeokey_entry*)&geokeys[1];
              if (evlrs[i].record_length_after_header != ((geokeys->number_of_keys+1)*8))
              {
                sprintf(note, "payload size of %d for geokey EVLR cannot hold %u + 1 geokey entries", (I32)evlrs[i].record_length_after_header, geokeys->number_of_keys);
                add_fail("EVLR", note);
                success = FALSE;
              }
            }
            else if (evlrs[i].record_id == 34736) // GeoDoubleParamsTag
            {
              if (geokey_double_params)
              {
                add_fail("EVLR", "extended variable length records contain more than one GeoDoubleParamsTag");
                success = FALSE;
              }
              geokey_double_params = (F64*)evlrs[i].data;
              if (((U32)evlrs[i].record_length_after_header % 8) != 0)
              {
                sprintf(note, "payload size of %d for GeokeyDoubleParams EVLR is not a multiple of 8", (I32)evlrs[i].record_length_after_header);
                add_fail("EVLR", note);
                success = FALSE;
              }
              geokey_double_params_num = (U32)evlrs[i].record_length_after_header / 8;
            }
            else if (evlrs[i].record_id == 34737) // GeoAsciiParamsTag
            {
              if (geokey_ascii_params)
              {
                add_fail("EVLR", "extended variable length records contain more than one GeoAsciiParamsTag");
                success = FALSE;
              }
              geokey_ascii_params = (CHAR*)evlrs[i].data;
            }
            else if (evlrs[i].record_id == 2112) // GeoOCGWKTParamsTag
            {
              if (ogc_wkt)
              {
                add_fail("EVLR", "extended variable length records contain more than one GeoOCGWKTParamsTag");
                success = FALSE;
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
                add_fail("EVLR", "extended variable length records contain more than one ClassificationLookup");
                success = FALSE;
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
                sprintf(note, "extended variable length records defines wave packet descriptor %d more than once", idx);
                add_fail("EVLR", note);
                success = FALSE;
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
