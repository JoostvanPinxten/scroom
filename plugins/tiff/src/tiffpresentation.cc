/*
 * Scroom - Generic viewer for 2D data
 * Copyright (C) 2009-2013 Kees-Jan Dijkzeul
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License, version 2, as published by the Free Software Foundation.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "tiffpresentation.hh"

#include <tiffio.h>
#include <string.h>

#include <scroom/unused.h>

#include <scroom/layeroperations.hh>

const TiffPresentation::Dummy TiffPresentation::dummy(0);

TiffPresentation::TiffPresentation()
    : fileName(), tif(NULL), height(0), width(0), tbi(), bpp(0)
{
  colormap = Colormap::createDefault(256);
}

TiffPresentation::~TiffPresentation()
{
  printf("TiffPresentation: Destructing...\n");

  if (tif != NULL)
  {
    TIFFClose(tif);
    tif = NULL;
  }

  tbi.reset();
  ls.clear();
}

bool TiffPresentation::load(std::string fileName)
{
  this->fileName = fileName;
  tif = TIFFOpen(fileName.c_str(), "r");
  if (!tif)
  {
    // Todo: report error
    printf("PANIC: Failed to open file %s\n", fileName.c_str());
    return false;
  }

  uint16 spp;
  TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
  if (spp != 1)
  {
    // Todo: Colour not yet supported
    printf("PANIC: Samples per pixel is not 1, but %d. Giving up\n", spp);
    return false;
  }

  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

  uint16 bpp;
  TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bpp);
  this->bpp = bpp;

  uint16 *r, *g, *b;
  int result = TIFFGetField(tif, TIFFTAG_COLORMAP, &r, &g, &b);
  if (result == 1)
  {
    originalColormap = Colormap::create();
    originalColormap->name = "Original";
    int count = 1 << bpp;
    originalColormap->colors.resize(count);

    for (int i = 0; i < count; i++)
    {
      originalColormap->colors[i] = Color(1.0 * r[i] / 0xFFFF,
          1.0 * g[i] / 0xFFFF, 1.0 * b[i] / 0xFFFF);
    }
  }
  else
  {
    colormap = Colormap::createDefault(1 << bpp);
  }

  uint16 photometric;
  TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric);
  switch (photometric)
  {
  case PHOTOMETRIC_MINISBLACK:
    if (originalColormap)
      printf(
          "WEIRD: Tiff contains a colormap, but photometric isn't palette\n");

    if (bpp == 1 || bpp == 8)
      originalColormap = Colormap::createDefault(2);
    else
      originalColormap = Colormap::createDefault(1 << bpp);

    break;

  case PHOTOMETRIC_MINISWHITE:
    if (originalColormap)
      printf(
          "WEIRD: Tiff contains a colormap, but photometric isn't palette\n");

    if (bpp == 1 || bpp == 8)
      originalColormap = Colormap::createDefaultInverted(2);
    else
      originalColormap = Colormap::createDefaultInverted(1 << bpp);

    break;

  case PHOTOMETRIC_PALETTE:
    if (!originalColormap)
    {
      printf(
          "WEIRD: Photometric is palette, but tiff doesn't contain a colormap\n");
      originalColormap = Colormap::createDefault(1 << bpp);
    }
    break;

  default:
    printf("PANIC: Unrecognized value for photometric\n");
    break;
  }

  colormap = originalColormap;

  printf("This bitmap has size %d*%d\n", width, height);
  ls.clear();

  if (bpp == 2 || bpp == 4 || photometric == PHOTOMETRIC_PALETTE)
  {
    ls.push_back(Operations::create(this, bpp));
    ls.push_back(OperationsColormapped::create(this, bpp));
    properties[COLORMAPPABLE_PROPERTY_NAME] = "";
  }
  else if (bpp == 1)
  {
    ls.push_back(Operations1bpp::create(this));
    ls.push_back(Operations8bpp::create(this));
  }
  else if (bpp == 8)
  {
    ls.push_back(Operations8bpp::create(this));
  }
  else
  {
    printf("PANIC: %d bits per pixel not supported\n", bpp);
    return false;
  }

  tbi = createTiledBitmap(width, height, ls);
  tbi->setSource(this);
  return true;
}

////////////////////////////////////////////////////////////////////////
// PresentationInterface
////////////////////////////////////////////////////////////////////////

GdkRectangle TiffPresentation::getRect()
{
  GdkRectangle rect;
  rect.x = 0;
  rect.y = 0;
  rect.width = width;
  rect.height = height;

  return rect;
}

void TiffPresentation::open(ViewInterface::WeakPtr viewInterface)
{
  views[viewInterface] = dummy;
  std::list<Viewable::Ptr> observers = getObservers();
  for (std::list<Viewable::Ptr>::iterator cur = observers.begin();
      cur != observers.end(); ++cur)
  {
    (*cur)->open(viewInterface);
  }

  if (tbi)
    tbi->open(viewInterface);
  else
  {
    printf(
        "ERROR: TiffPresentation::open(): No TiledBitmapInterface available!\n");
  }
}

void TiffPresentation::close(ViewInterface::WeakPtr vi)
{
  views.erase(vi);
  std::list<Viewable::Ptr> observers = getObservers();
  for (std::list<Viewable::Ptr>::iterator cur = observers.begin();
      cur != observers.end(); ++cur)
  {
    (*cur)->close(vi);
  }

  if (tbi)
    tbi->close(vi);
  else
  {
    printf(
        "ERROR: TiffPresentation::close(): No TiledBitmapInterface available!\n");
  }
}

void TiffPresentation::redraw(ViewInterface::Ptr vi, cairo_t* cr,
    GdkRectangle presentationArea, int zoom)
{
  if (tbi)
    tbi->redraw(vi, cr, presentationArea, zoom);
}

bool TiffPresentation::getProperty(const std::string& name, std::string& value)
{
  std::map<std::string, std::string>::iterator p = properties.find(name);
  bool found = false;
  if (p == properties.end())
  {
    found = false;
    value = "";
  }
  else
  {
    found = true;
    value = p->second;
  }

  return found;
}

bool TiffPresentation::isPropertyDefined(const std::string& name)
{
  return properties.end() != properties.find(name);
}

std::string TiffPresentation::getTitle()
{
  return fileName;
}

////////////////////////////////////////////////////////////////////////
// SourcePresentation
////////////////////////////////////////////////////////////////////////

void TiffPresentation::fillTiles(int startLine, int lineCount, int tileWidth,
    int firstTile, std::vector<Tile::Ptr>& tiles)
{
  // printf("Filling lines %d to %d, tile %d to %d (tileWidth = %d)\n",
  //        startLine, startLine+lineCount,
  //        firstTile, (int)(firstTile+tiles.size()),
  //        tileWidth);

  int pixelsPerByte = 8 / bpp;
  int dataLength = (width + pixelsPerByte - 1) / pixelsPerByte;
  byte row[dataLength];

  int tileCount = tiles.size();
  byte* dataPtr[tileCount];
  for (int tile = 0; tile < tileCount; tile++)
  {
    dataPtr[tile] = tiles[tile]->data.get();
  }

  for (int i = 0; i < lineCount; i++)
  {
    TIFFReadScanline(tif, (tdata_t) row, startLine + i);

    for (int tile = 0; tile < tileCount - 1; tile++)
    {
      memcpy(dataPtr[tile],
          row + (firstTile + tile) * tileWidth / pixelsPerByte,
          tileWidth / pixelsPerByte);
      dataPtr[tile] += tileWidth / pixelsPerByte;
    }
    memcpy(dataPtr[tileCount - 1],
        row + (firstTile + tileCount - 1) * tileWidth / pixelsPerByte,
        dataLength - (firstTile + tileCount - 1) * tileWidth / pixelsPerByte);
    dataPtr[tileCount - 1] += tileWidth / pixelsPerByte;
  }
}

void TiffPresentation::done()
{
  TIFFClose(tif);
  tif = NULL;
}

////////////////////////////////////////////////////////////////////////
// Colormappable
////////////////////////////////////////////////////////////////////////

void TiffPresentation::observerAdded(Viewable::Ptr observer,
    Scroom::Bookkeeping::Token)
{
  BOOST_FOREACH(const Views::value_type& p, views)
  {
    observer->open(p.first);
  }
}

void TiffPresentation::setColormap(Colormap::Ptr colormap)
{
  this->colormap = colormap;
  BOOST_FOREACH(const Views::value_type& p, views)
  {
    ViewInterface::Ptr v = p.first.lock();
    if(v)
    {
      if (tbi)
      {
        tbi->clearCaches(v);
      }
      v->invalidate();
    }
  }
}

Colormap::Ptr TiffPresentation::getColormap()
{
  return colormap;
}

Colormap::Ptr TiffPresentation::getOriginalColormap()
{
  return originalColormap;
}

int TiffPresentation::getNumberOfColors()
{
  return originalColormap->colors.size();
}

////////////////////////////////////////////////////////////////////////
// Helpers
////////////////////////////////////////////////////////////////////////
