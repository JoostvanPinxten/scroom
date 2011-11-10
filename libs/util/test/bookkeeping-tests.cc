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

#include <boost/test/unit_test.hpp>

#include <scroom/bookkeeping.hh>

using namespace Scroom::Bookkeeping;

//////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(Bookkeeping_Tests)

BOOST_AUTO_TEST_CASE(basic_usage)
{
  Map<int, int>::Ptr map = Map<int, int>::create();
  BOOST_REQUIRE(map);

  Token a = map->add(1,1);
  Token b = map->add(2,2);

  BOOST_CHECK(a);
  BOOST_CHECK(b);
  BOOST_CHECK_EQUAL(1, map->get(1));
  BOOST_CHECK_EQUAL(2, map->get(2));
  BOOST_CHECK_EQUAL(2, map->keys().size());
  BOOST_CHECK_EQUAL(2, map->values().size());
  BOOST_CHECK_THROW(map->get(3), std::invalid_argument);
  BOOST_CHECK_THROW(map->set(3,3), std::invalid_argument);
  BOOST_CHECK_THROW(map->add(2,4), std::invalid_argument);
  BOOST_CHECK_EQUAL(2, map->get(2));
  BOOST_FOREACH(int k, map->keys())
  {
    map->get(k); // should not throw an exception
  }
  map->set(2,5);
  BOOST_CHECK_EQUAL(5, map->get(2));
  BOOST_CHECK_EQUAL(2, map->keys().size());
  BOOST_CHECK_EQUAL(2, map->values().size());
  b.reset();
  BOOST_CHECK_THROW(map->get(2), std::invalid_argument);
  BOOST_CHECK_EQUAL(1, map->get(1));
  BOOST_CHECK_EQUAL(1, map->keys().size());
  BOOST_CHECK_EQUAL(1, map->values().size());
  map.reset();
  BOOST_CHECK(a);
}

BOOST_AUTO_TEST_CASE(weak_ptr)
{
  Map<WeakToken, int>::Ptr map = Map<WeakToken, int>::create();
  BOOST_REQUIRE(map);

  Token a = map->add(1);
  BOOST_CHECK(a);
  BOOST_CHECK_THROW(map->add(a,3), std::invalid_argument);
  BOOST_CHECK_THROW(map->addMe(a,3), std::invalid_argument);
  BOOST_CHECK_EQUAL(1, map->get(a));

  Token b = Detail::Token::create();
  BOOST_CHECK(b);
  BOOST_CHECK_THROW(map->get(b), std::invalid_argument);
  Token c = map->add(b, 3);
  BOOST_CHECK_EQUAL(3, map->get(b));
  BOOST_CHECK_THROW(map->get(c), std::invalid_argument);
  c.reset();
  BOOST_CHECK_THROW(map->get(b), std::invalid_argument);

  map->addMe(b, 5);
  BOOST_CHECK_EQUAL(5, map->get(b));
  BOOST_CHECK_EQUAL(2, map->keys().size());
  b.reset();
  BOOST_CHECK_EQUAL(1, map->keys().size());
}

BOOST_AUTO_TEST_CASE(shared_ptr)
{
  Map<Token, int>::Ptr map = Map<Token, int>::create();
  BOOST_REQUIRE(map);

  Token a = map->add(1);
  BOOST_CHECK(a);
  BOOST_CHECK_THROW(map->add(a,3), std::invalid_argument);
  BOOST_CHECK_THROW(map->addMe(a,3), std::invalid_argument);
  BOOST_CHECK_EQUAL(1, map->get(a));
  BOOST_CHECK_EQUAL(1, map->keys().size());
  WeakToken aa = a;
  a.reset();
  BOOST_CHECK_EQUAL(1, map->keys().size());
  BOOST_CHECK_EQUAL(aa.lock(), map->keys().front());
}

BOOST_AUTO_TEST_SUITE_END()
