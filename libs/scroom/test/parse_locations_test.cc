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

#include <scroom/roi.hh>

#include <list>
#include <fstream>

#include <boost/test/unit_test.hpp>

#include <scroom/unused.hh>
#include <scroom/scroominterface.hh>

#include <roi-parser.hh>

namespace Roi = Scroom::Roi;

class PresentationStub : public PresentationBase
{
public:
  typedef boost::shared_ptr<PresentationStub> Ptr;

private:
  std::string name;

protected:
  PresentationStub(std::string const& name) :
      name(name)
  {
  }

public:
  static Ptr create(std::string const& name)
  {
    return Ptr(new PresentationStub(name));
  }

  // PresentationInterface
  virtual GdkRectangle getRect()
  {
    throw std::runtime_error("Operation not supported");
  }

  virtual void redraw(ViewInterface::Ptr const&, cairo_t*, GdkRectangle, int)
  {
    throw std::runtime_error("Operation not supported");
  }

  virtual bool getProperty(const std::string&, std::string&)
  {
    throw std::runtime_error("Operation not supported");
  }

  virtual bool isPropertyDefined(const std::string&)
  {
    throw std::runtime_error("Operation not supported");
  }

  virtual std::string getTitle()
  {
    return name;
  }

protected:
  // PresentationBase
  virtual void viewAdded(ViewInterface::WeakPtr)
  {
    throw std::runtime_error("Operation not supported");
  }

  virtual void viewRemoved(ViewInterface::WeakPtr)
  {
    throw std::runtime_error("Operation not supported");
  }

  virtual std::set<ViewInterface::WeakPtr> getViews()
  {
    throw std::runtime_error("Operation not supported");
  }
};

class AggregateStub : public PresentationStub, public Aggregate
{
public:
  typedef boost::shared_ptr<AggregateStub> Ptr;

private:
  std::vector<PresentationInterface::Ptr> children;

protected:
  AggregateStub(std::string const& name) :
    PresentationStub(name)
  {
  }

public:
  static Ptr create(std::string const& name)
  { return Ptr(new AggregateStub(name)); }

  // Aggregate
  virtual void addPresentation(PresentationInterface::Ptr const& presentation)
  { children.push_back(presentation); }

};

class ScroomInterfaceStub : public ScroomInterface
{
public:
  typedef boost::shared_ptr<ScroomInterfaceStub> Ptr;

public:
  std::list<std::string> newPresentations;
  std::set<std::string> openedFiles;
  std::list<std::string> newAggregates;
  std::list<PresentationInterface::Ptr> shownPresentations;

public:
  static Ptr create();

  // ScroomInterface
  virtual PresentationInterface::Ptr newPresentation(std::string const& name);
  virtual Aggregate::Ptr newAggregate(std::string const& name);
  virtual PresentationInterface::Ptr loadPresentation(std::string const& name, std::string const& relativeTo=std::string());

  virtual void showPresentation(PresentationInterface::Ptr const& presentation);
};

ScroomInterfaceStub::Ptr ScroomInterfaceStub::create()
{
  return Ptr(new ScroomInterfaceStub());
}

PresentationInterface::Ptr ScroomInterfaceStub::newPresentation(std::string const& name)
{
  newPresentations.push_back(name);
  return PresentationStub::create(name);
}

Aggregate::Ptr ScroomInterfaceStub::newAggregate(std::string const& name)
{
  newAggregates.push_back(name);
  return AggregateStub::create(name);
}

PresentationInterface::Ptr ScroomInterfaceStub::loadPresentation(std::string const& name, std::string const& /* relativeTo */ )
{
  openedFiles.insert(name);
  return PresentationStub::create(name);
}

void ScroomInterfaceStub::showPresentation(PresentationInterface::Ptr const& presentation)
{
  shownPresentations.push_back(presentation);
}


BOOST_AUTO_TEST_SUITE(Roi_Tests)

BOOST_AUTO_TEST_CASE(Parse_files)
{
  std::stringstream ss;
  ss << " * File: a.tif" << std::endl;
  ss << " * File: b.tif" << std::endl;
  ss << " * Aggregate: q" << std::endl;
  ss << "   * File: c.tif" << std::endl;
  ss << "   * File: d.tif" << std::endl;
  ss << "   * Aggregate: r" << std::endl;
  ss << "     * File: e.tif" << std::endl;
  ss << "   * File: f.tif" << std::endl;
  ss << " * File: g.tif" << std::endl;

  std::string input = ss.str();
  
  std::vector<Roi::Detail::Presentation> presentations = Roi::Detail::parse(input.begin(), input.end());

  // BOOST_FOREACH(Roi::Detail::Presentation const & p, presentations)
  //   std::cout << "Found presentation: " << p << std::endl;

  BOOST_CHECK_EQUAL(4, presentations.size());
}

BOOST_AUTO_TEST_CASE(Parse_files2)
{
  std::stringstream ss;
  ss << " * File: a.tif" << std::endl;
  ss << " * File: b.tif" << std::endl;
  ss << " * Aggregate: q" << std::endl;
  ss << "   * File: c.tif" << std::endl;
  ss << "   * File: d.tif" << std::endl;

  std::ofstream f("/tmp/testdata.txt");
  f << ss.str();
  
  Roi::List l = Roi::parse(ss);
  ScroomInterfaceStub::Ptr stub = ScroomInterfaceStub::create();

  std::set<ViewObservable::Ptr> presentations = l.instantiate(stub);

  BOOST_CHECK_EQUAL(3, presentations.size());
  BOOST_CHECK_EQUAL(4, stub->openedFiles.size());
}

BOOST_AUTO_TEST_SUITE_END()
