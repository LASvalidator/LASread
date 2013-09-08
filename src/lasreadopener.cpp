/*
===============================================================================

  FILE:  lasreadopener.cpp
  
  CONTENTS:
  
    see corresponding header file
  
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
  
    see corresponding header file
  
===============================================================================
*/
#include "lasreadopener.hpp"

#include "lasreadopener.hpp"
#include "lasreader.hpp"
#include "laswaveformreader.hpp"

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define DIRECTORY_SLASH '\\'
#else
#define DIRECTORY_SLASH '/'
#endif

BOOL LASreadOpener::is_piped() const
{
  return (!file_names && use_stdin);
}

BOOL LASreadOpener::is_active() const
{
  return ((file_name_current < file_name_number) || use_stdin);
}

void LASreadOpener::reset()
{
  file_name_current = 0;
  file_name = 0;
}

LASreader* LASreadOpener::open()
{
  if (file_names)
  {
    use_stdin = FALSE;
    if (file_name_current == file_name_number)
    {
      return 0;
    }
    else
    {
      file_name = file_names[file_name_current];
      file_name_current++;
    }
#ifdef LASZIP_ENABLED
    if (strstr(file_name, ".las") || strstr(file_name, ".laz") || strstr(file_name, ".LAS") || strstr(file_name, ".LAZ"))
#else
    if (strstr(file_name, ".las") || strstr(file_name, ".LAS"))
#endif
    {
      LASreader* lasreader = new LASreader();
      if (!lasreader->open(file_name, io_ibuffer_size))
      {
        fprintf(stderr,"ERROR: cannot open lasreader with file name '%s'\n", file_name);
        delete lasreader;
        return 0;
      }
      return lasreader;
    }
#ifdef LASZIP_ENABLED
    else
#else
    else if (strstr(file_name, ".laz") || strstr(file_name, ".LAZ"))
    {
      fprintf(stderr,"ERROR: LASread not compiled with LASzip and therefore does not\n");
      fprintf(stderr,"       support LAZ files like '%s'\n", file_name);
      return 0;
    }
    else
#endif
    {
      fprintf(stderr,"ERROR: unknown file extension '%s'\n", file_name);
      return 0;
    }
  }
  else if (use_stdin)
  {
    use_stdin = FALSE;
    LASreader* lasreader = new LASreader();
    if (!lasreader->open(stdin))
    {
      fprintf(stderr,"ERROR: cannot open lasreader from stdin n");
      delete lasreader;
      return 0;
    }
    return lasreader;
  }
  return 0;
}

BOOL LASreadOpener::reopen(LASreader* lasreader)
{
  if (file_names)
  {
    if (!file_name) return FALSE;
    if (strstr(file_name, ".las") || strstr(file_name, ".laz") || strstr(file_name, ".LAS") || strstr(file_name, ".LAZ"))
    {
      if (!lasreader->open(file_name, io_ibuffer_size))
      {
        fprintf(stderr,"ERROR: cannot reopen lasreader with file name '%s'\n", file_name);
        return FALSE;
      }
      return TRUE;
    }
    else
    {
      fprintf(stderr,"ERROR: unknown file extension '%s'\n", file_name);
      return FALSE;
    }
  }
  else
  {
    fprintf(stderr,"ERROR: no lasreader input specified\n");
    return FALSE;
  }
}

LASwaveformreader* LASreadOpener::open_waveform(const LASheader* lasheader)
{
  if ((lasheader->point_data_format != 4) && (lasheader->point_data_format != 5) && (lasheader->point_data_format != 9) && (lasheader->point_data_format != 10)) return 0;
  if (lasheader->wave_packet_descriptor == 0) return 0;
  if (get_path() == 0) return 0;
  LASwaveformreader* waveformreader = new LASwaveformreader();
  if ((lasheader->global_encoding & 2) && (lasheader->start_of_waveform_data_packet_record > lasheader->offset_to_point_data))
  {
    if (waveformreader->open(get_path(), lasheader->start_of_waveform_data_packet_record, lasheader->wave_packet_descriptor))
    {
      return waveformreader;
    }
  }
  else
  {
    if (waveformreader->open(get_path(), 0, lasheader->wave_packet_descriptor))
    {
      return waveformreader;
    }
  }
  delete waveformreader;
  return 0;
}

void LASreadOpener::usage() const
{
  fprintf(stderr,"Supported Inputs:\n");
  fprintf(stderr,"  -i lidar.las\n");
  fprintf(stderr,"  -i lidar1.las lidar2.las lidar3.las\n");
  fprintf(stderr,"  -i *.las\n");
  fprintf(stderr,"  -i flight0??.las flight1??.las\n");
  fprintf(stderr,"  -lof file_list.txt\n");
}

BOOL LASreadOpener::parse(int argc, char* argv[])
{
  int i;
  for (i = 1; i < argc; i++)
  {
    if (argv[i][0] == '\0')
    {
      continue;
    }
    else if (strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"-help") == 0)
    {
      usage();
      return TRUE;
    }
    else if (strcmp(argv[i],"-i") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs at least 1 argument: file_name or wild_card\n", argv[i]);
        return FALSE;
      }
      *argv[i]='\0';
      i+=1;
      do
      {
        add_file_name(argv[i]);
        *argv[i]='\0';
        i+=1;
      } while (i < argc && *argv[i] != '-');
      i-=1;
    }
    else if (strcmp(argv[i],"-stdin") == 0)
    {
      use_stdin = TRUE;
      *argv[i]='\0';
    }
    else if (strcmp(argv[i],"-lof") == 0)
    {
      if ((i+1) >= argc)
      {
        fprintf(stderr,"ERROR: '%s' needs 1 argument: list_of_files\n", argv[i]);
        return FALSE;
      }
      FILE* file = fopen(argv[i+1], "r");
      if (file == 0)
      {
        fprintf(stderr, "ERROR: cannot open '%s'\n", argv[i+1]);
        return FALSE;
      }
      char line[1024];
      while (fgets(line, 1024, file))
      {
        // find end of line
        int len = strlen(line) - 1;
        // remove extra white spaces and line return at the end 
        while (len > 0 && ((line[len] == '\n') || (line[len] == ' ') || (line[len] == '\t') || (line[len] == '\012')))
        {
          line[len] = '\0';
          len--;
        }
        add_file_name(line);
      }
      fclose(file);
      *argv[i]='\0'; *argv[i+1]='\0'; i+=1;
    }
  }

  return TRUE;
}

const char* LASreadOpener::get_path() const
{
  if (file_name)
    return file_name;
  if (file_name_number)
    return file_names[0];
  return 0;
}

const char* LASreadOpener::get_file_name() const
{
  const char* file_name_only = get_path();
  if (file_name_only)
  {
    int len = strlen(file_name_only);
    while ((len >= 0) && (file_name_only[len] != DIRECTORY_SLASH))
    {
      len--;
    }
    if (len >= 0)
    {
      return (file_name_only + len + 1);
    }
    else
    {
      return file_name_only;
    }
  }
  return 0;
}

const char* LASreadOpener::get_directory()
{
  if (get_path())
  {
    const char* temp_path = get_path();
    const char* temp_file_name = get_file_name();
    if (directory)
    {
      free(directory);
      directory = 0;
    }
    directory = strdup(temp_file_name);
    int len = temp_file_name - temp_path;
    directory[len] = '\0';
    return directory;
  }
  return 0;
}

void LASreadOpener::set_io_ibuffer_size(I32 io_ibuffer_size)
{
  this->io_ibuffer_size = io_ibuffer_size;
}

void LASreadOpener::set_file_name(const char* file_name, BOOL unique)
{
  add_file_name(file_name, unique);
}

#ifdef _WIN32
#include <windows.h>
BOOL LASreadOpener::add_file_name(const char* file_name, BOOL unique)
{
  BOOL r = FALSE;
  HANDLE h;
  WIN32_FIND_DATA info;
  h = FindFirstFile(file_name, &info);
  if (h != INVALID_HANDLE_VALUE)
  {
    // find the path
    int len = strlen(file_name);
    while (len && file_name[len] != '\\') len--;
    if (len)
    {
      len++;
      char full_file_name[512];
      strncpy(full_file_name, file_name, len);
	    do
	    {
        sprintf(&full_file_name[len], "%s", info.cFileName);
        if (add_file_name_single(full_file_name, unique)) r = TRUE;
  	  } while (FindNextFile(h, &info));
    }
    else
    {
      do
      {
        if (add_file_name_single(info.cFileName, unique)) r = TRUE;
  	  } while (FindNextFile(h, &info));
    }
	  FindClose(h);
  }
  return r;
}
#endif

#ifdef _WIN32
BOOL LASreadOpener::add_file_name_single(const char* file_name, BOOL unique)
#else
BOOL LASreadOpener::add_file_name(const char* file_name, BOOL unique)
#endif
{
  if (unique)
  {
    U32 i;
    for (i = 0; i < file_name_number; i++)
    {
      if (strcmp(file_names[i], file_name) == 0)
      {
        return FALSE;
      }
    }
  }
  if (file_name_number == file_name_allocated)
  {
    if (file_names)
    {
      file_name_allocated *= 2;
      file_names = (char**)realloc(file_names, sizeof(char*)*file_name_allocated);
    }
    else
    {
      file_name_allocated = 16;
      file_names = (char**)malloc(sizeof(char*)*file_name_allocated);
    }
    if (file_names == 0)
    {
      fprintf(stderr, "ERROR: alloc for file_names pointer array failed at %d\n", file_name_allocated);
    }
  }
  file_names[file_name_number] = strdup(file_name);
  file_name_number++;
  return TRUE;
}

U32 LASreadOpener::get_file_name_number() const
{
  return file_name_number;
}

LASreadOpener::LASreadOpener()
{
  io_ibuffer_size = 65536;
  file_names = 0;
  file_name = 0;
  directory = 0;
  use_stdin = FALSE;
  file_name_number = 0;
  file_name_allocated = 0;
  file_name_current = 0;
}

LASreadOpener::~LASreadOpener()
{
  if (file_names)
  {
    U32 i;
    for (i = 0; i < file_name_number; i++) free(file_names[i]);
    free(file_names);
  }
}
