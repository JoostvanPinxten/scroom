/*
 * Scroom - Generic viewer for 2D data
 * Copyright (C) 2009-2017 Kees-Jan Dijkzeul
 *
 * SPDX-License-Identifier: LGPL-2.1
 */

#include "sizedeterminer.hh"

#include <algorithm>

#include <boost/foreach.hpp>

#include <scroom/gtk-helpers.hh>
#include <scroom/assertions.hh>

namespace
{
  GdkRectangle DetermineSize(std::list<PresentationInterface::Ptr> presentations)
  {
    int left = std::numeric_limits<int>::max();
    int top = std::numeric_limits<int>::max();
    int right = std::numeric_limits<int>::min();
    int bottom = std::numeric_limits<int>::min();

    BOOST_FOREACH(PresentationInterface::Ptr const& p, presentations)
    {
      GdkRectangle rect = p->getRect();
      left = std::min(left, rect.x);
      top = std::min(top, rect.y);
      right = std::max(right, rect.x + rect.width);
      bottom = std::max(bottom, rect.y + rect.height);
    }
    return Scroom::GtkHelpers::createGdkRectangle(left, top, right-left, bottom-top);
  }

  template<typename K, typename V>
  std::list<K> keys(std::map<K,V> const& m)
  {
    std::list<K> k;
    for(auto const& p: m)
      k.push_back(p.first);

    return k;
  }
}

////////////////////////////////////////////////////////////////////////

SizeDeterminer::PresentationData::PresentationData()
{
  // Can't conjure a ResizablePresentationInterface::Ptr out of thin
  // air. Hence calling this constructor is not valid. However,
  // std::map requires that it is present.
  defect();
}

SizeDeterminer::PresentationData::PresentationData(ResizablePresentationInterface::Ptr const& resizablePresentationInterface)
  : resizablePresentationInterface(resizablePresentationInterface)
{}

////////////////////////////////////////////////////////////////////////

SizeDeterminer::Ptr SizeDeterminer::create()
{
  return Ptr(new SizeDeterminer());
}

SizeDeterminer::SizeDeterminer()
{
}

void SizeDeterminer::add(PresentationInterface::Ptr const& p)
{
  ResizablePresentationInterface::Ptr r = boost::dynamic_pointer_cast<ResizablePresentationInterface>(p);
  if(r)
  {
    resizablePresentationData.insert(std::make_pair(p,PresentationData(r)));
  }
  else
    presentations.push_back(p);

  sendUpdates();
}

GdkRectangle SizeDeterminer::getRect() const
{
  if(!presentations.empty())
  {
    return DetermineSize(presentations);
  }
  if(!resizablePresentationData.empty())
  {
    return DetermineSize(keys(resizablePresentationData));
  }
  return Scroom::GtkHelpers::createGdkRectangle(0,0,0,0);
}

void SizeDeterminer::open(PresentationInterface::Ptr const& p, ViewInterface::WeakPtr const& vi)
{
  if(resizablePresentationData.count(p))
  {
    PresentationData& d = resizablePresentationData[p];
    require(!d.views.count(vi));
    d.views.insert(vi);

    d.resizablePresentationInterface->setRect(vi, getRect());
  }
  else
  {
    require(!boost::dynamic_pointer_cast<ResizablePresentationInterface>(p));
  }
}

void SizeDeterminer::close(PresentationInterface::Ptr const& p, ViewInterface::WeakPtr const& vi)
{
  if(resizablePresentationData.count(p))
  {
    PresentationData& d = resizablePresentationData[p];
    require(d.views.count(vi));
    d.views.erase(vi);
  }
  else
  {
    require(!boost::dynamic_pointer_cast<ResizablePresentationInterface>(p));
  }
}

void SizeDeterminer::sendUpdates()
{
  GdkRectangle rect = getRect();

  for(auto const& data: resizablePresentationData)
    for(auto const& view: data.second.views)
      data.second.resizablePresentationInterface->setRect(view, rect);
}