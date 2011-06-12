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


#ifndef NCOORDS_NODES_H
#define  NCOORDS_NODES_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <deque>
#include <iterator>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "nodesmap.h"
#include "ncoords.h"

namespace NCOORDS{
  /*
  template <typename INPUTITER, typename OUTPUTCONTAINER>
  void steal_pointer(INPUTITER in, OUTPUTCONTAINER& out){
    out.push_back(*in);
    *in = NULL;
    }*/

  void steal_pointer(Nodedeq::iterator in, Nodedeq& out);
  
  
  class Nodes: public ncoords{
  public:
    class doorbitch;
    friend class doorbitch;
    class doorbitch: public std::binary_function< Nodedeq::iterator, unsigned int&, void>{
    protected:
      Nodes* owner;
    public:
      doorbitch(Nodes* the_club): owner(the_club) {}
      virtual ~doorbitch(){}

      virtual void operator()(Nodedeq::iterator suppliant, unsigned int& incrementor){
	if (owner->within_limits(*(*suppliant)) && value_is_unique(owner->nodes, *(*suppliant))){
	  steal_pointer(suppliant, owner->nodes);
	  ++incrementor;
	}
      }

    };
  public:
    typedef Nodedeq::iterator iterator;
    typedef Nodedeq::const_iterator const_iterator;
    typedef Nodedeq::size_type size_type;

    typedef deqC::iterator deqC_iter;
     protected:
    static unsigned int MINGRID;
    static unsigned int MAXGRID;
    Nodedeq cullable;
    Nodedeq nodes;
    //unsigned int - change
    int limit_x;
    int limit_y;

    unsigned int maxnodecount;

    static bool Nodes_is_verbose;
    
    unsigned int loopcount;
  public:
    Nodes(const coords& orig, const coords& second,  int point_no = 3,int lim_x =3000, int lim_y =2000, bool iterated = true);
    Nodes(const coords& orig, float rad, float ang, int lim_x =4000, int lim_y =3000, bool iterated = true);
    Nodes(const ncoords& src, int lim_x =4000, int lim_y =3000, bool iterated = true);
    
    Nodes(const Nodes& src);
    // 
    //Nodes(const Nodes& src, unsigned int count);
  virtual ~Nodes(){}
    void generate_nodes();
    void delete_centre_node();
    void fix_radial();

    inline double get_radius() const{
return radius;
    }
    /*
    void transform(const cairo_matrix_t& matrix){
      ncoords::transform(matrix);
      if(nodes.size()){
	nodes.transform(matrix);
      }
    }
    */
    size_type size()const{return nodes.size();}
    Nodedeq& get_nodes(){return nodes;}
    const  Nodedeq& get_nodes()const{return nodes;}

    void set_limits(const coords& centre, const coords& secondpt, unsigned int base, unsigned int limx,  unsigned int limy);
    virtual void set(const coords& centre, const coords& secondpt, unsigned int base);
    inline virtual coords& get_origin(){
      return (coords&)(*this);
    }
inline virtual const coords& get_origin()const {
      return (const coords&)(*this);
    }
    inline virtual void set_origin(coords& orig){
      coords::set(*this);
    }
    virtual void set_baseno(unsigned int n);
    void regenerate();
    inline int get_limit_x() const{
      return limit_x;
    }
    inline  void set_limit_x(int limx){
      limit_x = limx;
    }

inline  void set_limit_y(int limy){
      limit_y = limy;
    }

inline int get_limit_y() const{
      return limit_y;
    }
    //shouldn't need this
    inline  void set_loopcount(int count){
      loopcount =count;
    }

inline unsigned int get_loopcount()const{
return loopcount;
}
    
 void outputstring(const std::string& filename, const deqC& printlist) const;

 void outputstring(const std::string& filename);
    void iterate( unsigned int seedcount);
    
    void gen_cullable(unsigned int seedcount);
    
    unsigned int add_nodes();
    //void mark_centre();
    coords* mark_centre_from_limit(int& list_position, int& serial_no);
    coords* mark_centre_from_click(const coords& click, int& list_position, int& serial_no);

    virtual bool    base_good()const;

    const coords& find_closest(double ex, double why)const;
    const coords& find_closest(const coords* neighbour)const;

    std::pair<const coords&, double> 
    find_closest_and_distance(const coords* neighbour)const;


    void translate(Nodedeq& list, const coords* offset);
    void deliver(Nodedeq& delivery);
    void deliver(setC& delivery);
    void print(Nodedeq& printlist);
    void print(setC& printlist);
    void invert_nodestate(Nodedeq& coordlist);
    double get_radius(){ return ncoords::get_radius();}

    bool within_limits(const coords& candidate);
    
    void save_nodes(const std::string& filename);
  
    void increment_loopcount(){loopcount++;}
    
    friend std::ostream& operator<<(std::ostream& os, const Nodes& n){
      for(Nodedeq::const_iterator walker = n.nodes.begin();walker != n.nodes.end(); walker++){
	os << *(*walker);
      }
      return os;
    }  
  };

  class ProximityNodes: public Nodes{
  protected:
    distance_threshold<coords> separator;//mindistance between nodes
    //as for Nodes, but with a threshhold for separation between points,
    //(default: half the radius)
    //to create 'imperfect' grids of base 5 and so on
  public:
    ProximityNodes(const ncoords& src, int lim_x =4000, int lim_y =3000):
      Nodes(src, lim_x, lim_y, false),
      separator(src.get_radius()/ 2.0){
      nodes.push_back(new coords(get_centre()));
      this->to_iterator(std::back_inserter(cullable));
      iterate(cullable.size());
    }

    void iterate(unsigned int iters){
      //unsigned int base=get_base();
      if(Nodes::base_good()){
	Nodes::iterate(iters);
      }
      else{
	throw xn_Error("FIXME: overload to use separation threshold");
      }
    }

  };
}
  
  
#endif /* NCOORDS_NODES_H*/
