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

#include <boost/test/unit_test.hpp>

#include <scroom/progressinterfacehelpers.hh>
#include <scroom/assertions.hh>

#include "progressstateinterfacestub.hh"

using namespace Scroom::Utils;

BOOST_AUTO_TEST_SUITE(test_converters)

BOOST_AUTO_TEST_CASE(test_ProgressInterfaceFromProgressStateInterface)
{
  ProgressStateInterfaceStub::Ptr stub = ProgressStateInterfaceStub::create();
  BOOST_REQUIRE(stub);
  BOOST_CHECK_EQUAL(ProgressStateInterface::IDLE, stub->state);

  ProgressInterface::Ptr pi = ProgressInterfaceFromProgressStateInterfaceForwarder::create(stub);
  BOOST_REQUIRE(pi);
  BOOST_CHECK_EQUAL(ProgressStateInterface::IDLE, stub->state);

  pi->setWaiting();
  BOOST_CHECK_EQUAL(ProgressStateInterface::WAITING, stub->state);
  BOOST_CHECK_EQUAL(0.0, stub->progress);
  
  pi->setWorking(0.33);
  BOOST_CHECK_EQUAL(ProgressStateInterface::WORKING, stub->state);
  BOOST_CHECK_EQUAL(0.33, stub->progress);

  pi->setWaiting(0.25);
  BOOST_CHECK_EQUAL(ProgressStateInterface::WAITING, stub->state);
  BOOST_CHECK_EQUAL(0.25, stub->progress);
  
  pi->setFinished();
  BOOST_CHECK_EQUAL(ProgressStateInterface::FINISHED, stub->state);
  BOOST_CHECK_EQUAL(1.0, stub->progress);
  
  pi->setWorking(3,4);
  BOOST_CHECK_EQUAL(ProgressStateInterface::WORKING, stub->state);
  BOOST_CHECK_EQUAL(0.75, stub->progress);
  
  pi->setIdle();
  BOOST_CHECK_EQUAL(ProgressStateInterface::IDLE, stub->state);
}

BOOST_AUTO_TEST_CASE(test_ProgressStateInterfaceFromProgressInterface)
{
  ProgressStateInterfaceStub::Ptr stub = ProgressStateInterfaceStub::create();
  BOOST_REQUIRE(stub);
  BOOST_CHECK_EQUAL(ProgressStateInterface::IDLE, stub->state);

  ProgressInterface::Ptr pi = ProgressInterfaceFromProgressStateInterfaceForwarder::create(stub);
  BOOST_REQUIRE(pi);
  BOOST_CHECK_EQUAL(ProgressStateInterface::IDLE, stub->state);

  ProgressStateInterface::Ptr ps = ProgressStateInterfaceFromProgressInterfaceForwarder::create(pi);
  BOOST_REQUIRE(ps);
  BOOST_CHECK_EQUAL(ProgressStateInterface::IDLE, stub->state);

  stub->progress = 0.33;

  ps->setState(ProgressStateInterface::WAITING);
  BOOST_CHECK_EQUAL(ProgressStateInterface::WAITING, stub->state);
  BOOST_CHECK_EQUAL(0.0, stub->progress);
  
  ps->setProgress(0.27);
  BOOST_CHECK_EQUAL(ProgressStateInterface::WORKING, stub->state);
  BOOST_CHECK_EQUAL(0.27, stub->progress);

  ps->setState(ProgressStateInterface::WAITING);
  BOOST_CHECK_EQUAL(ProgressStateInterface::WAITING, stub->state);
  BOOST_CHECK_EQUAL(0.27, stub->progress);
  
  ps->setState(ProgressStateInterface::FINISHED);
  BOOST_CHECK_EQUAL(ProgressStateInterface::FINISHED, stub->state);
  BOOST_CHECK_EQUAL(1.0, stub->progress);
  
  ps->setProgress(3,4);
  BOOST_CHECK_EQUAL(ProgressStateInterface::WORKING, stub->state);
  BOOST_CHECK_EQUAL(0.75, stub->progress);
  
  ps->setState(ProgressStateInterface::IDLE);
  BOOST_CHECK_EQUAL(ProgressStateInterface::IDLE, stub->state);
}

BOOST_AUTO_TEST_SUITE_END()