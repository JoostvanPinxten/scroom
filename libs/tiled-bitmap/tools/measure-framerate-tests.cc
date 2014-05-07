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

#include "measure-framerate-tests.hh"

#include <time.h>

#include <string>

#include <boost/shared_ptr.hpp>

#include <scroom/unused.hh>

#include "measure-framerate-callbacks.hh"
#include "measure-framerate-stubs.hh"
#include "test-helpers.hh"

////////////////////////////////////////////////////////////////////////

class Invalidator
{
private:
  unsigned int secs;
  bool started;
  struct timespec t;

public:
  Invalidator(unsigned int secs);

  bool operator()();
};

class BaseCounter
{
private:
  std::string name;
  unsigned int secs;
  bool started;
  unsigned int count;
  struct timespec t;

  static unsigned int columnWidth;
  
public:
  BaseCounter(const std::string& name, unsigned int secs);

  bool operator()();
};

class InvalidatingCounter : public BaseCounter
{
public:
  InvalidatingCounter(const std::string& name, unsigned int secs);
  bool operator()();
};

////////////////////////////////////////////////////////////////////////


static bool logSizes()
{
  printf("Canvas size: %dx%d\n", drawingAreaWidth, drawingAreaHeight);
  return false;
}

////////////////////////////////////////////////////////////////////////

Invalidator::Invalidator(unsigned int secs)
  : secs(secs), started(false)
{
}

bool Invalidator::operator()()
{
  invalidate();
  
  if(!started && 0==clock_gettime(CLOCK_REALTIME, &t))
  {
    started = true;
    return true;
  }

  struct timespec now;
  if(0==clock_gettime(CLOCK_REALTIME, &now))
  {
    if(now.tv_sec > t.tv_sec + secs)
      return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////

unsigned int BaseCounter::columnWidth=0;

BaseCounter::BaseCounter(const std::string& name, unsigned int secs)
  : name(name), secs(secs), started(false), count(0)
{
  columnWidth = std::max(columnWidth, (unsigned int)name.length());
}

bool BaseCounter::operator()()
{
  if(!started && 0==clock_gettime(CLOCK_REALTIME, &t))
  {
    started = true;
    return true;
  }

  count++;
  
  struct timespec now;
  if(0==clock_gettime(CLOCK_REALTIME, &now))
  {
    if(now.tv_sec > t.tv_sec + secs)
    {
      // We're done. Compute frequency.
      double elapsed = (now.tv_nsec - t.tv_nsec)*1e-9;
      elapsed += now.tv_sec - t.tv_sec;
      printf("%-*s: %10.2f Hz\n", columnWidth, name.c_str(), count/elapsed);
      
      return false;
    }
  }

  return true;
}

////////////////////////////////////////////////////////////////////////

InvalidatingCounter::InvalidatingCounter(const std::string& name, unsigned int secs)
  : BaseCounter(name, secs)
{
}

bool InvalidatingCounter::operator()()
{
  invalidate();
  return BaseCounter::operator()();
}

////////////////////////////////////////////////////////////////////////

void init_tests()
{
  const int width = 2*4096;
  const int height = 2*4096;
  const unsigned int testDuration = 15;
  const unsigned int sleepDuration = 2;

  functions.push_back(Sleep(sleepDuration));
  functions.push_back(logSizes);
  functions.push_back(BaseCounter("Baseline (no invalidate)", testDuration));

  functions.push_back(Invalidator(sleepDuration));
  functions.push_back(InvalidatingCounter("Baseline (no redraw)", testDuration));

  functions.push_back(boost::bind(setupTest1bpp, -2, width, height));
  functions.push_back(wait);
  functions.push_back(Invalidator(sleepDuration));
  functions.push_back(InvalidatingCounter("1bpp, 1:4 zoom", testDuration));

  functions.push_back(boost::bind(setupTest1bpp, 4, width, height));
  functions.push_back(wait);
  functions.push_back(Invalidator(sleepDuration));
  functions.push_back(InvalidatingCounter("1bpp, 16:1 zoom", testDuration));

  functions.push_back(boost::bind(setupTest4bpp, -2, width, height));
  functions.push_back(wait);
  functions.push_back(Invalidator(sleepDuration));
  functions.push_back(InvalidatingCounter("4bpp, 1:4 zoom, colormapped", testDuration));

  functions.push_back(boost::bind(setupTest4bpp, 4, width, height));
  functions.push_back(wait);
  functions.push_back(Invalidator(sleepDuration));
  functions.push_back(InvalidatingCounter("4bpp, 16:1 zoom, colormapped", testDuration));

  functions.push_back(boost::bind(setupTest8bpp, -2, width, height));
  functions.push_back(wait);
  functions.push_back(Invalidator(sleepDuration));
  functions.push_back(InvalidatingCounter("8bpp, 1:4 zoom", testDuration));

  functions.push_back(boost::bind(setupTest8bpp, 4, width, height));
  functions.push_back(wait);
  functions.push_back(Invalidator(sleepDuration));
  functions.push_back(InvalidatingCounter("8bpp, 16:1 zoom", testDuration));

  functions.push_back(boost::bind(setupTest8bppColormapped, -2, width, height));
  functions.push_back(wait);
  functions.push_back(Invalidator(sleepDuration));
  functions.push_back(InvalidatingCounter("8bpp, 1:4 zoom, colormapped", testDuration));

  functions.push_back(boost::bind(setupTest8bppColormapped, 4, width, height));
  functions.push_back(wait);
  functions.push_back(Invalidator(sleepDuration));
  functions.push_back(InvalidatingCounter("8bpp, 16:1 zoom, colormapped", testDuration));

  functions.push_back(reset);
  functions.push_back(quit);
}
