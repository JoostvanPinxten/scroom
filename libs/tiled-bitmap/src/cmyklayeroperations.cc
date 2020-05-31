/*
 * Scroom - Generic viewer for 2D data
 * Copyright (C) 2009-2020 Kees-Jan Dijkzeul
 *
 * SPDX-License-Identifier: LGPL-2.1
 */

#include <scroom/bitmap-helpers.hh>
#include <scroom/layeroperations.hh>
#include <scroom/bitmap-helpers.hh>

////////////////////////////////////////////////////////////////////////

namespace
{
  boost::shared_ptr<unsigned char> shared_malloc(size_t size)
  {
    return boost::shared_ptr<unsigned char>(static_cast<unsigned char*>(malloc(size)), free);
  }
}

////////////////////////////////////////////////////////////////////////
// OperationsCMYK

LayerOperations::Ptr OperationsCMYK::create(int bps)
{
  return Ptr(new OperationsCMYK(bps));
}

OperationsCMYK::OperationsCMYK(int bps_)
: bps(bps_)
{
}

int OperationsCMYK::getBpp()
{
  // CMYK has 4 channels -> 4 samples per pixel.
  return 4*bps;
}

/**
 * Cache the given tile
 */
Scroom::Utils::Stuff OperationsCMYK::cache(const ConstTile::Ptr tile)
{
  // Allocate the space for the cache - stride is the height of one row
  const int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, tile->width);
  boost::shared_ptr<uint8_t> data = shared_malloc(static_cast<size_t>(stride * tile->height));

  // Row is a pointer to a row of pixels (destination)
  uint32_t* row = reinterpret_cast<uint32_t*>(data.get());
  Scroom::Bitmap::SampleIterator<const uint8_t> si(tile->data.get(), 0, bps);
  const unsigned int max = si.pixelMask;

  for (int i = 0; i < tile->height * tile->width; i++)
  {
    // Convert CMYK to ARGB, because cairo doesn't know how to render CMYK.
    unsigned int C_i = *si++;
    unsigned int M_i = *si++;
    unsigned int Y_i = *si++;
    unsigned int K_i = *si++;
    
    unsigned int R = (max - C_i)*(max - K_i) * 255 / max / max;
    unsigned int G = (max - M_i)*(max - K_i) * 255 / max / max;
    unsigned int B = (max - Y_i)*(max - K_i) * 255 / max / max;

    // Write 255 as alpha (fully opaque)
    row[i] = 255u << 24 | R << 16 | G << 8 | B;
  }

  return Scroom::Bitmap::BitmapSurface::create(tile->width, tile->height, CAIRO_FORMAT_ARGB32, stride, data);
}

void OperationsCMYK::reduce(Tile::Ptr target, const ConstTile::Ptr source, int top_left_x, int top_left_y)
{
  // Reducing by a factor 8
  const int sourceStride = 4*source->width; // 4 samples per pixel
  Scroom::Bitmap::SampleIterator<const uint8_t> sourceBase(source->data.get(), 0, bps);
  const unsigned int sourceMax = sourceBase.pixelMask;

  const int targetStride = 4 * target->width; // 4 samples per pixel
  Scroom::Bitmap::SampleIterator<uint8_t> targetBase(target->data.get() +
      (target->height * top_left_y + top_left_x ) * targetStride / 8, 0, 8);
  const unsigned int targetMax = targetBase.pixelMask;

  for (int y = 0; y < source->height / 8; y++)
  {
    auto targetSample = targetBase;

    for (int x = 0; x < source->width / 8; x++)
    {
      // We want to store the average colour of the 8*8 pixel image
      // with (x, y) as its top-left corner into targetSample.
      auto sourceRow = sourceBase + 8*4*x;//8 pixels of 4 samples times x

      int sum_c = 0;
      int sum_m = 0;
      int sum_y = 0;
      int sum_k = 0;
      for (size_t row=0; row<8; row++, sourceRow += sourceStride)
      {
        auto sourceSample = sourceRow;
        for (size_t current = 0; current < 8; current++)
        {
          sum_c += *sourceSample++;
          sum_m += *sourceSample++;
          sum_y += *sourceSample++;
          sum_k += *sourceSample++;
        }
      }

      (targetSample++).set(sum_c * targetMax / sourceMax / 64);
      (targetSample++).set(sum_m * targetMax / sourceMax / 64);
      (targetSample++).set(sum_y * targetMax / sourceMax / 64);
      (targetSample++).set(sum_k * targetMax / sourceMax / 64);
    }

    targetBase += targetStride; // Advance 1 row
    sourceBase += sourceStride * 8; // Advance 8 rows
  }
}
