/*
 * Scroom - Generic viewer for 2D data
 * Copyright (C) 2009-2020 Kees-Jan Dijkzeul
 *
 * SPDX-License-Identifier: LGPL-2.1
 */

#pragma once

#include <map>
#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <scroom/utilities.hh>
#include <scroom/stuff.hh>

namespace Scroom
{
  namespace Bookkeeping
  {
    using Scroom::Utils::Stuff;
    using Scroom::Utils::StuffList;

    namespace Detail
    {
      class TokenImpl;
      class TokenAddition;

      template<typename V>
      class ValueType;

      template<typename V>
      class LValue;
    }

    class Token : public boost::shared_ptr<Detail::TokenImpl>
    {
    public:
      Token(const boost::shared_ptr<Detail::TokenImpl>& t);
      Token(const boost::weak_ptr<Detail::TokenImpl>& t);
      Token();
      Token(const Stuff& s);
      Token(const StuffList& l);

      void add(const Stuff& s) const;
      void add(const StuffList& l) const;
      void merge(Token& rhs) const;
      void merge(StuffList& l) const;

    public:
      Detail::TokenAddition operator+(const Stuff& rhs) const;
      Token const& operator+=(const Stuff& rhs) const;
    };

    typedef boost::weak_ptr<Detail::TokenImpl> WeakToken;

    template<typename K, typename V>
    class MapBase : public virtual Scroom::Utils::Base, public boost::noncopyable
    {
    private:
      typedef typename std::map<K,boost::weak_ptr<Detail::ValueType<V> > > MapType;

      MapType map;
      mutable boost::mutex mut;

    public:
      Token reserve(const K& k);
      Token reReserve(const K& k);
      void remove(const K& k);
      void remove(const K& k, WeakToken t);
      Detail::LValue<V> at(const K& k);
      void set(const K& k, const V& v);
      V get(const K& k);
      std::list<K> keys() const;
      std::list<V> values() const;
    };

    template<typename K, typename V>
    class Map : public MapBase<K,V>
    {
    public:
      typedef boost::shared_ptr<Map<K, V> > Ptr;

    public:
      static Ptr create();
    };

//    template<typename V>
//    class Map<Token, V> : public MapBase<Token,V>
//    {
//    public:
//      typedef boost::shared_ptr<Map<Token, V> > Ptr;
//
//    public:
//      static Ptr create();
//
//    public:
//      void addMe(const Token& k, const V& v);
//      Token add(const V& v);
//      Token add(const Token& k, const V& v);
//    };
//
//    template<typename V>
//    class Map<WeakToken, V> : public MapBase<WeakToken,V>
//    {
//    public:
//      typedef boost::shared_ptr<Map<WeakToken, V> > Ptr;
//
//    public:
//      static Ptr create();
//
//    public:
//      void addMe(const WeakToken& k, const V& v);
//      Token add(const V& v);
//      Token add(const WeakToken& k, const V& v);
//    };

  }
}

#include <scroom/impl/bookkeepingimpl.hh>

