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



/**
 * @file pathshape.h
 a shape bounded by multiple transformed copies
 of a path, possibly tileable
*/

#ifndef NCOORDS_PATHSHAPE_H
#define NCOORDS_PATHSHAPE_H
//generalise
#define DOUBLE_PATH_ROTATION ((2. * M_PI) / 3.)

#include "nodes.h"
#include "clickhistory.h"
#include "linesegment.h"

namespace NCOORDS{
  ///too kooky?
  class nPathShape; //FWD decl
  ////'DoublePath angle'
  //  2 => M_PI
  //  3 => 4 *M_PI / 3. ??
  //  4 => M_PI
  //  6 => M_PI ? 4 *M_PI / 3. ??
  ////////////
  //// 'Quadpath angle'
  //  2 => M_PI
  //  3 => 2 *M_PI / 3 ??
  //  4 => M_PI / 2 ?
  //  6 => 2 *M_PI / 3 ?
  ////////////////
 /** @class DoublePath
     * a path with its mirror clone
     joined at one end and rotated - construction element for
     tileable shape
     may instead just use this abstraction within nPathshape 
     via get_doublepath_angle() and get_quadpath_angle(), 
     and forget this class
    */

  class DoublePath{
  public:
    typedef  simple_click_history path_type;
   
    typedef simple_click_history::const_iterator const_iterator;
    typedef simple_click_history::iterator iterator;
    typedef simple_click_history::const_reverse_iterator const_reverse_iterator;
    typedef simple_click_history::reverse_iterator reverse_iterator;

  protected:
    // nPathShape* creator; //?
    simple_click_history* basepath;
    bool frontend;
    double rotangle;
    //coords offset; //later
  public:

    
    //DoublePath(nPathShape& shape, bool frontapex=true);
    DoublePath(simple_click_history* src,
	       bool frontapex=true, 
	       double rotation= DOUBLE_PATH_ROTATION);
    virtual ~DoublePath(){}
    coords get_apex()const;
    operator bool()const;
    
    size_t basesize()const;

    simple_click_history& get_basepath();
    const simple_click_history& get_basepath()const;

    void set_basepath(simple_click_history* p);

    //at which end is the clone attached and rotated?
    bool is_frontend()const;
    coords get_first_point()const;
    coords get_last_point()const;
    coords get_middle_point()const;

    //get a copy of basepath's clone, appropriately rotated
    simple_click_history get_doppelganger()const;

    virtual  simple_click_history as_path()const;
  };
  class QuadPath	{
    //where some copies of line skipped to create a larger shape		 
  public:
typedef simple_click_history::const_reverse_iterator const_reverse_iterator;
 typedef simple_click_history::const_iterator const_iterator;			  typedef simple_click_history::value_type point_type;
  protected:
    DoublePath* srcpath; 
    double rotangle;
    bool backend;
  public:
    QuadPath(DoublePath& p, double angle, bool use_backend_for_apex=true);

    coords get_apex()const;

    simple_click_history as_path() const;
  };
 

  class nPathShape: public ncoords {
  public:
    typedef std::pair<coords, int> point_type;
    typedef simple_click_history path_type;
    typedef path_type::iterator iterator;
    typedef path_type::const_iterator const_iterator;
    typedef path_type::reverse_iterator reverse_iterator;
    typedef path_type::const_reverse_iterator const_reverse_iterator;


  protected:
    //ncoords origin;
    DoublePath path;
    //simple_click_history basepath;

  public:
  nPathShape(const ncoords& origin, 
	     simple_click_history& basepath);

    bool is_tileable()const;

    const simple_click_history& get_basepath()const;
    simple_click_history& get_basepath();

    DoublePath create_doublepath(double angle = 0);

    DoublePath create_doublepath(path_type& basepath, double angle = 0);
    QuadPath create_quadpath(double quadangle =0, double doubleangle =0);

    void set_basepath(simple_click_history* p);
    simple_click_history angleshape(double firstangle=M_PI/3.0,
      double secondangle= 4.0* M_PI/3.0);

  simple_click_history as_path();
      double get_doublepath_angle()const;
      double get_quadpath_angle()const;
      double get_corner_angle(bool with_quadpath=true)const;
      simple_click_history draw_shape(bool with_quadpath=false);
    };

  }

#endif //NCOORDS_PATHSHAPE_H
