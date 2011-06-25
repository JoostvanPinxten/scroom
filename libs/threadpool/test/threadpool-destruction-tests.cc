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

#include <scroom/threadpool.hh>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>

#include <scroom/semaphore.hh>

#include "helpers.hh"
#include "function-additor.hh"


using namespace boost::posix_time;
using namespace Scroom;

const millisec short_timeout(250);
const millisec long_timeout(2000);

//////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(ThreadPool_destruction_Tests)

BOOST_AUTO_TEST_CASE(threads_terminate_on_destruction)
{
  ThreadPool::Ptr pool = ThreadPool::create(0);
  ThreadPool::ThreadPtr t = pool->add();
  BOOST_CHECK(!t->timed_join(short_timeout));
  pool.reset();

  bool success = boost::thread::id() == t->get_id();
  BOOST_CHECK(success);
  if(!success)
  {
    t->interrupt();
    t->timed_join(long_timeout);
    BOOST_REQUIRE(boost::thread::id() == t->get_id());
  }
}

BOOST_AUTO_TEST_CASE(threads_can_be_interrupted)
{
  ThreadPool pool(0);
  ThreadPool::ThreadPtr t = pool.add();
  boost::this_thread::sleep(millisec(50));
  t->interrupt();
  t->timed_join(short_timeout);
  BOOST_CHECK_EQUAL(boost::thread::id(), t->get_id());
}

BOOST_AUTO_TEST_CASE(destroy_threadpool_with_nonempty_queue)
{
  ThreadPool::Ptr pool = ThreadPool::create(1);
  Semaphore guard(0);
  Semaphore a(0);
  Semaphore b(0);
  Semaphore c(0);
  Semaphore d(0);

  pool->schedule(clear(&a)+pass(&b));
  pool->schedule(pass(&d)+clear(&c));

  // Give the thread some time to start the job
  a.P();
  
  boost::thread t(pass(&guard)+destroy(pool));
  pool.reset();
  guard.V();

  // Thread t blocks until the ThreadPool is destroyed, which is after
  // the pass(&b) completes
  BOOST_CHECK(!t.timed_join(short_timeout));
  b.V();
  BOOST_CHECK(t.timed_join(long_timeout));
  BOOST_REQUIRE(boost::thread::id() == t.get_id());
  BOOST_CHECK(!c.try_P());
}

BOOST_AUTO_TEST_CASE(destroy_threadpool_with_nonempty_queue_with_completeAllJobsBeforeDestruction_true)
{
  ThreadPool::Ptr pool = ThreadPool::create(1, true);
  Semaphore guard(0);
  Semaphore a(0);
  Semaphore b(0);
  Semaphore c(0);

  pool->schedule(clear(&a)+pass(&b));
  pool->schedule(clear(&c));

  // Give the thread some time to start the job
  a.P();
  
  boost::thread t(pass(&guard)+destroy(pool));
  pool.reset();
  guard.V();

  // Thread t blocks until the ThreadPool is destroyed, which is after
  // the pass(&b) completes
  BOOST_CHECK(!t.timed_join(short_timeout));
  b.V();
  BOOST_CHECK(t.timed_join(long_timeout));
  BOOST_REQUIRE(boost::thread::id() == t.get_id());
  BOOST_CHECK(c.try_P());
}


BOOST_AUTO_TEST_SUITE_END()