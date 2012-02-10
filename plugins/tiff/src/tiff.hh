/*
 * Scroom - Generic viewer for 2D data
 * Copyright (C) 2009-2012 Kees-Jan Dijkzeul
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

#ifndef _TIFF_HH
#define _TIFF_HH

#include <scroom/plugininformationinterface.hh>
#include <scroom/presentationinterface.hh>

class Tiff : public PluginInformationInterface, public OpenInterface
{
public:
  ////////////////////////////////////////////////////////////////////////
  // PluginInformationInterface
  
  virtual std::string getPluginName();
  virtual std::string getPluginVersion();
  virtual void registerCapabilities(ScroomInterface* host);
  virtual void unregisterCapabilities(ScroomInterface* host);

  ////////////////////////////////////////////////////////////////////////
  // OpenInterface
  
  virtual std::list<GtkFileFilter*> getFilters();
  virtual PresentationInterface::Ptr open(const std::string& fileName);

  ////////////////////////////////////////////////////////////////////////
  
  virtual ~Tiff();
};

#endif