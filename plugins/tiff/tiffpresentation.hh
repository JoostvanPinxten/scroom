/*
 * Scroom - Generic viewer for 2D data
 * Copyright (C) 2009-2011 Kees-Jan Dijkzeul
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

#ifndef _TIFFPRESENTATION_HH
#define _TIFFPRESENTATION_HH

#include <string>
#include <map>
#include <list>

#include <scroom/scroominterface.hh>
#include <scroom/tiledbitmapinterface.hh>
#include <scroom/presentationinterface.hh>
#include <scroom/colormappable.hh>
#include <scroom/observable.hh>

typedef struct tiff TIFF;

class TiffPresentation : public PresentationInterface, public SourcePresentation, public Colormappable
{
public:
  typedef boost::shared_ptr<TiffPresentation> Ptr;

private:
  std::string fileName;
  TIFF* tif;
  int height;
  int width;
  TiledBitmapInterface* tbi;
  LayerSpec ls;
  int bpp;
  std::map<std::string, std::string> properties;
  std::list<ViewInterface*> views;
  Colormap::Ptr originalColormap;
  Colormap::Ptr colormap;
  
public:

  TiffPresentation();
  virtual ~TiffPresentation();

  bool load(std::string fileName, FileOperationObserver::Ptr observer);
  
  ////////////////////////////////////////////////////////////////////////
  // PresentationInterface
  ////////////////////////////////////////////////////////////////////////

  virtual GdkRectangle getRect();
  virtual void open(ViewInterface* viewInterface);
  virtual void redraw(ViewInterface* vi, cairo_t* cr, GdkRectangle presentationArea, int zoom);
  virtual void close(ViewInterface* vi);
  virtual bool getProperty(const std::string& name, std::string& value);
  virtual bool isPropertyDefined(const std::string& name);
  virtual std::string getTitle();

  ////////////////////////////////////////////////////////////////////////
  // SourcePresentation
  ////////////////////////////////////////////////////////////////////////
private:
  virtual void fillTiles(int startLine, int lineCount, int tileWidth, int firstTile, std::vector<Tile::Ptr>& tiles);
 
  ////////////////////////////////////////////////////////////////////////
  // Colormappable
  ////////////////////////////////////////////////////////////////////////

public:
  Scroom::Utils::Registration registerObserver(Viewable::WeakPtr observer);
  Scroom::Utils::Registration registerStrongObserver(Viewable::Ptr observer);
  void setColormap(Colormap::Ptr colormap);
  Colormap::Ptr getOriginalColormap();
  int getNumberOfColors();

  ////////////////////////////////////////////////////////////////////////
  // Helpers
  ////////////////////////////////////////////////////////////////////////
public:
  Colormap::Ptr getColormap();
  
};

#endif
