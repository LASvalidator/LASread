/*
===============================================================================

  FILE:  lasreader.cpp
  
  CONTENTS:
  
    see corresponding header file
  
  PROGRAMMERS:
  
    martin.isenburg@rapidlasso.com  -  http://rapidlasso.com

  COPYRIGHT:

    (c) 2007-2013, martin isenburg, rapidlasso - fast tools to catch reality

    This is free software; you can redistribute and/or modify it under the
    terms of the GNU Lesser General Licence as published by the Free Software
    Foundation. See the COPYING.txt file for more information.

    This software is distributed WITHOUT ANY WARRANTY and without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    see corresponding header file
  
===============================================================================
*/
#include "lasreader.hpp"

#include "bytestreamin.hpp"
#include "bytestreamin_file.hpp"
#include "bytestreamin_istream.hpp"
#include "lasreadpoint.hpp"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include <stdlib.h>
#include <string.h>

BOOL LASreader::open(const char* file_name, U32 io_buffer_size)
{
  if (file_name == 0)
  {
    fprintf(stderr,"ERROR: fine name pointer is zero\n");
    return FALSE;
  }

  file = fopen(file_name, "rb");
  if (file == 0)
  {
    fprintf(stderr, "ERROR: cannot open file '%s'\n", file_name);
    return FALSE;
  }

  if (setvbuf(file, NULL, _IOFBF, io_buffer_size) != 0)
  {
    fprintf(stderr, "WARNING: setvbuf() failed with buffer size %u\n", io_buffer_size);
  }

  // create input
  ByteStreamIn* in;
  if (IS_LITTLE_ENDIAN())
    in = new ByteStreamInFileLE(file);
  else
    in = new ByteStreamInFileBE(file);

  return open(in);
}

BOOL LASreader::open(FILE* file)
{
  if (file == 0)
  {
    fprintf(stderr,"ERROR: file pointer is zero\n");
    return FALSE;
  }

#ifdef _WIN32
  if (file == stdin)
  {
    if(_setmode( _fileno( stdin ), _O_BINARY ) == -1 )
    {
      fprintf(stderr, "ERROR: cannot set stdin to binary (untranslated) mode\n");
      return FALSE;
    }
  }
#endif

  // create input
  ByteStreamIn* in;
  if (IS_LITTLE_ENDIAN())
    in = new ByteStreamInFileLE(file);
  else
    in = new ByteStreamInFileBE(file);

  return open(in);
}

BOOL LASreader::open(istream& stream)
{
  // create input
  ByteStreamIn* in;
  if (IS_LITTLE_ENDIAN())
    in = new ByteStreamInIstreamLE(stream);
  else
    in = new ByteStreamInIstreamBE(stream);

  return open(in);
}

BOOL LASreader::open(ByteStreamIn* stream)
{
  if (stream == 0)
  {
    fprintf(stderr,"ERROR: ByteStreamIn* pointer is zero\n");
    return FALSE;
  }

  this->stream = stream;

  // clean the header

  header.clean();

  // load the header (without VLRs or EVLRs)

  if (!header.load_header(stream))
  {
    return FALSE;
  }

  // load the header VLRs

  if (!header.load_vlrs(stream))
  {
    return FALSE;
  }

  // load the header EVLRs

  if (!header.load_evlrs(stream))
  {
    return FALSE;
  }

  // create the point reader

  reader = new LASreadPoint();

  // initialize point and the reader

  if (header.laszip)
  {
    if (!point.init(&header, header.laszip->num_items, header.laszip->items)) return FALSE;
    if (!reader->setup(header.laszip->num_items, header.laszip->items, header.laszip)) return FALSE;
  }
  else
  {
    if (!point.init(&header, header.point_data_format, header.point_data_record_length)) return FALSE;
    if (!reader->setup(point.num_items, point.items)) return FALSE;
  }
  if (!reader->init(stream)) return FALSE;

  npoints = (header.legacy_number_of_point_records ? header.legacy_number_of_point_records : header.number_of_point_records);
  p_count = 0;

  return TRUE;
}

BOOL LASreader::is_compressed() const
{
  if (header.laszip)
  {
    return (header.laszip->compressor == LASZIP_COMPRESSOR_NONE ? FALSE : TRUE);
  }
  return FALSE;
}

BOOL LASreader::seek(const I64 p_index)
{
  if (p_index < npoints)
  {
    if (reader->seek((U32)p_count, (U32)p_index))
    {
      p_count = p_index;
      return TRUE;
    }
  }
  return FALSE;
}

BOOL LASreader::read_point()
{
  if (p_count < npoints)
  {
    if (reader->read(point.point) == FALSE)
    {
      fprintf(stderr,"WARNING: end-of-file after %u of %u points\n", (U32)p_count, (U32)npoints);
      return FALSE;
    }
    p_count++;
    return TRUE;
  }
  return FALSE;
}

void LASreader::close(BOOL close_stream)
{
  if (reader) 
  {
    reader->done();
    delete reader;
    reader = 0;
  }
  if (close_stream)
  {
    if (stream)
    {
      delete stream;
      stream = 0;
    }
    if (file)
    {
      fclose(file);
      file = 0;
    }
  }
}

LASreader::LASreader()
{
  npoints = 0;
  p_count = 0;
  file = 0;
  stream = 0;
  reader = 0;
}

LASreader::~LASreader()
{
  if (reader || stream) close();
}
