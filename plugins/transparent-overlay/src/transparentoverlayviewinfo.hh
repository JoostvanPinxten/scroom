/*
 * Scroom - Generic viewer for 2D data
 * Copyright (C) 2009-2014 Kees-Jan Dijkzeul
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

#ifndef TRANSPARENTOVERLAYVIEWINFO_HH
#define TRANSPARENTOVERLAYVIEWINFO_HH

#include <list>
#include <map>
#include <vector>

#include <gtk/gtk.h>
#include <cairo.h>

#include <boost/shared_ptr.hpp>

#include <scroom/viewinterface.hh>
#include <scroom/presentationinterface.hh>
#include <scroom/utilities.hh>
#include <scroom/progressinterfacehelpers.hh>

#include "sizedeterminer.hh"

class TransparentOverlayViewInfo;

class ChildView : public ViewInterface, virtual public Scroom::Utils::Base
{
public:
  typedef boost::shared_ptr<ChildView> Ptr;

private:
  boost::shared_ptr<TransparentOverlayViewInfo> parent;
  ProgressInterface::Ptr progressInterface;
  
private:
  ChildView(boost::shared_ptr<TransparentOverlayViewInfo> const& parent);

public:
  static Ptr create(boost::shared_ptr<TransparentOverlayViewInfo> const& parent);

  // ViewInterface ///////////////////////////////////////////////////////
  virtual void invalidate();
  virtual ProgressInterface::Ptr getProgressInterface();
  virtual void addSideWidget(std::string title, GtkWidget* w);
  virtual void removeSideWidget(GtkWidget* w);
  virtual void addToToolbar(GtkToolItem* ti);
  virtual void removeFromToolbar(GtkToolItem* ti);
};

class TransparentOverlayViewInfo : virtual public Scroom::Utils::Base
{
public:
  typedef boost::shared_ptr<TransparentOverlayViewInfo> Ptr;

private:
  typedef std::map<PresentationInterface::Ptr, ChildView::Ptr> ChildMap;
  
private:
  ViewInterface::Ptr parentView;
  ChildMap childViews;
  Scroom::Utils::ProgressInterfaceMultiplexer::Ptr progressInterfaceMultiplexer;
  std::vector<GtkWidget*> buttons;
  std::vector<PresentationInterface::Ptr> children;
  SizeDeterminer::Ptr sizeDeterminer;

private:
  TransparentOverlayViewInfo(const ViewInterface::WeakPtr& vi, SizeDeterminer::Ptr const& sizeDeterminer);
  void createToggleToolButton();

public:
  static Ptr create(const ViewInterface::WeakPtr& vi, SizeDeterminer::Ptr const& sizeDeterminer);
  void addChildren(const std::list<PresentationInterface::Ptr>& children);
  void addChild(const PresentationInterface::Ptr& child);

  void close();

  void redraw(cairo_t* cr, GdkRectangle presentationArea, int zoom);

  void invalidate() { parentView->invalidate(); }
  ProgressInterface::Ptr getProgressInterface()
  { return progressInterfaceMultiplexer->createProgressInterface(); }

  // Helpers
  void toggled(GtkToggleButton* button);
};

#endif
