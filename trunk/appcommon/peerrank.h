// -*- C++ -*-

/* Copyright (C)2006-2011 John O'Driscoll - biggerbumme@ yahoo.co.in
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef APPCOMMON_PEERRANK_H 
#define APPCOMMON_PEERRANK_H

#if HAVE_CONFIG_H
#include "appcommonconfig.h"
#endif

#include <functional>
NAMESPACE_APPCOMMON{
template <typename RETVAL, typename RANKABLE>
  class PeerRankBase;//public std::binary_function<const RANKABLE&, const RANKABLE&, RETVAL>;


  template <typename RETVAL, typename RANKABLE>
    class PeerRanker:public std::binary_function<const RANKABLE&, const RANKABLE&, RETVAL>{

  public:
    typedef RETVAL (RANKABLE::*rank_function)(const RANKABLE& , const RANKABLE&)const;
  protected:
    const RANKABLE& ranker;
    rank_function rank;
  public:
    PeerRanker(const RANKABLE& r, rank_function fn):ranker(r), rank(fn){}
    RETVAL operator()(const RANKABLE& left, const RANKABLE& right)const{
      return ranker.*rank(left, right);
    }
  };

template <typename RETVAL, typename RANKABLE>
    class UnConstPeerRanker:public std::binary_function<const RANKABLE&, const RANKABLE&, RETVAL>{

  public:
    typedef RETVAL (RANKABLE::*rank_function)(const RANKABLE& , const RANKABLE&);
  protected:
    RANKABLE& ranker;
    rank_function rank;
  public:
    UnConstPeerRanker(RANKABLE& r, rank_function fn):ranker(r), rank(fn){}
    RETVAL operator()(const RANKABLE& left, const RANKABLE& right){
      return ranker.*rank(left, right);
    }
  };

template <typename RANKABLE>
  class PeerSorter : public PeerRanker<bool, RANKABLE>{
public:
  typedef PeerRanker<bool, RANKABLE> base_type;
  typedef typename base_type::rank_function rank_function;

protected:
  using base_type::ranker;
  using base_type::rank;
public:
  PeerSorter(const RANKABLE& r, rank_function fn):base_type(r, fn){}

};

} //NAMESPACE_APPCOMMON

#endif

