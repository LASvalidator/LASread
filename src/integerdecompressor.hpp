/*
===============================================================================

  FILE:  integerdecompressor.hpp (adapted from integercompressor.hpp)
  
  CONTENTS:
 
    This version can only decompress. It has been stripped of the parts
    of the code that compress for integration into the ASPRS LASvalidator. 

    This compressor provides three different contexts for encoding integer
    numbers whose range may lie anywhere between 1 and 31 bits, which is
    specified with the SetPrecision function.

    The compressor encodes two things:

      (1) the number k of miss-predicted low-order bits and
      (2) the k-bit number that corrects the missprediction

    The k-bit number is usually coded broken in two chunks. The highest
    bits are compressed using an arithmetic range table. The lower bits
    are stored raw without predicive coding. How many of the higher bits
    are compressed can be specified with bits_high. The default is 8.

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
  
    27 April 2013 -- adapted from the LASzip library for the ASPRS LASvalidator
  
===============================================================================
*/
#ifndef INTEGER_COMPRESSOR_HPP
#define INTEGER_COMPRESSOR_HPP

#include "arithmeticdecoder.hpp"

class IntegerCompressor
{
public:

  // Constructor & Deconstructor
  IntegerCompressor(ArithmeticDecoder* dec, U32 bits=16, U32 contexts=1, U32 bits_high=8, U32 range=0);
  ~IntegerCompressor();

  // Manage Decompressor
  void initDecompressor();
  I32 decompress(I32 iPred, U32 context=0);

  // Get the k corrector bits from the last compress/decompress call
  U32 getK() const {return k;};

private:
  I32 readCorrector(ArithmeticModel* model);

  U32 k;

  U32 contexts;
  U32 bits_high;

  U32 bits;
  U32 range;

  U32 corr_bits;
  U32 corr_range;
  I32 corr_min;
  I32 corr_max;

  ArithmeticDecoder* dec;

  ArithmeticModel** mBits;

  ArithmeticModel** mCorrector;

  int** corr_histogram;
};

#endif
