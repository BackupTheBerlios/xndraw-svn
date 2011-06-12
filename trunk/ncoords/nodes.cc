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




#include "nodes.h"
#include <sstream>
using namespace NCOORDS;

bool Nodes::Nodes_is_verbose = true;
//should properly be called Nodes_gen
unsigned int Nodes::MINGRID = 100u;
unsigned int Nodes::MAXGRID = 500u;
void Nodes::set_baseno(unsigned int n){
  ncoords::set_baseno(n);
  regenerate();
}
  void Nodes::regenerate(){
  nodes.clear();
  if(cullable.size()){cullable.clear();}
  loopcount=0;
  generate_nodes();
    }
void Nodes::delete_centre_node(){
  if (!nodes.size()){throw xn_Error("nodes empty");}
  coords centre = ncoords::get_centre();
  coords* beg= *(nodes.begin());
  if(!beg){throw xn_Error("NULL point to begin");}
  if(*beg == centre){
    nodes.erase(nodes.begin());
  }
  else{throw  xn_Error("centre node not there - already deleted");}
  }
    
Nodes::Nodes(const ncoords& src, int lim_x , int lim_y , bool iterated ):
  ncoords(src),
limit_x(lim_x),
  limit_y(lim_y),
  maxnodecount(MAX_NODE_COUNT),
  loopcount(0)
{
  std::cerr<<"Nodes(const ncoords& src, int lim_x , int lim_y , bool iterated"
	   <<std::endl;
  //if(limit_x <=0) ||(limit_y <=  0){ maxnodecount = MAX_NODE_COUNT; }
  fix_radial();
  if(iterated){
  this->generate_nodes();
  //FIXME -- if set b4 iterating, run out of memeory
  //limit_x=lim_x;
}
  std::cerr<<"Nodes ... done"<<std::endl;
}

Nodes::Nodes(const coords& orig, const coords& second,  
	     int point_no, 
	     int lim_x, int lim_y,
	     bool iterated)
  :
  ncoords(orig, second, point_no),
  
  limit_x(lim_x),
  limit_y(lim_y),
  maxnodecount(MAX_NODE_COUNT),
  loopcount(0)
{
  fix_radial();
  //if(limit_x <=0) ||(limit_y <=  0){ maxnodecount = MAX_NODE_COUNT; }
  if(iterated){
  this->generate_nodes();
}
}
void Nodes::generate_nodes(){
  if(nodes.size() || loopcount){
    throw xn_Error("clear nodes before generating");
  }
  coords centre=ncoords::get_centre();
  unsigned int sym= get_base();
  if((sym ==3) || (sym ==4)||( sym == 6)){
    nodes.load(centre);//   original centre  
    increment_loopcount();
    ncoords::to_iterator(std::back_inserter(nodes));
    increment_loopcount();
    iterate(sym);
    cullable.clear();
    if(sym % 2){
      //otherwise 3-grid has an idiosyncratic extra node
      delete_centre_node();
    }
  }
  else if (sym ==2){
    coords radd(*get_radial());
    Nodes tmp(centre, radd, 4, limit_x, limit_y);
    nodes= tmp.get_nodes();
    loopcount= tmp.loopcount;
  }
  else{
    std::ostringstream ostmp;
    ostmp<<"unsupported grid: "<<sym <<" radial symmetry";
    throw xn_Error(ostmp.str());
  }
}

//get rid of - unnecessary confusion
Nodes::Nodes(const Nodes& src):
  ncoords(src),
  nodes(src.nodes),
  limit_x(src.limit_x),
  limit_y(src.limit_y),
  maxnodecount(src.maxnodecount),
  loopcount(src.loopcount)
{
  if(!nodes.size()){
    fix_radial();
    generate_nodes();
  }
}

void Nodes::fix_radial(){
  unsigned int mingrid = MINGRID;
  if(this->get_distance(&radial) < mingrid){
    unsigned int maxgrid = MAXGRID;
    if(maxgrid <= mingrid){
      throw xn_Error("maxgrid less than min");
    }
    unsigned int gridrange = maxgrid - mingrid;
    double xportion = std::rand() % maxgrid;
    unsigned int rem = std::sqrt(((double)(maxgrid * maxgrid))- (xportion* xportion));
    double yportion(0.);
    if(rem){
      yportion = std::rand() % rem;
    }
    double dist = std::sqrt((xportion *xportion) + (yportion *yportion));
    double newx= xportion ;
    double newy = yportion ;
    if(dist < mingrid){
      double more = std::sqrt((dist *dist)/2);
      newx += more;
      newy += more;
    }
    else if (dist > maxgrid){
      throw xn_Error("miscalcualtion of grid - too alarge");
    }
    coords newcoords(newx, newy);
    radial += newcoords;
    radius= get_distance(&radial);
  }
}


void Nodes::set_limits(const coords& centre, const coords& secondpt, 
		       unsigned int base, 
		       unsigned int limx,  unsigned int limy){
  limit_x=limx;
  limit_y= limy;
  this->set(centre, secondpt, base);
}

void Nodes::set(const coords& centre, const coords& secondpt, 
		       unsigned int base){
  nodes.clear();
  cullable.clear();
  ncoords::set(centre, secondpt, base);
  fix_radial();
  loopcount=0;
  //int oldx =limit_x;
  //limit_x = 0;
  maxnodecount = MAX_NODE_COUNT;
  generate_nodes();
  //limit_x =oldx;
}



bool Nodes::within_limits(const coords& candidate){
  if(limit_x <= 0 || limit_y <=0){
    return nodes.size() <=  maxnodecount;
  }
    double exy = candidate.get_x();
    double wy = candidate.get_y();
#if DEBUG_NODES
    std::cerr<<"within_limits(): -> x: "<<exy<<", y: "<< wy<<"\nlimit_x: "<< limit_x
	     <<"\nlimit_y: "<< limit_y <<std::endl;
#endif
    if ((-limit_x / 2) <= exy){
      if (exy  <= limit_x){ 
	if ((-limit_y / 2) <= wy){
	  if (wy <= limit_y){ 
	    std::cerr <<"good"<<std::endl;
	    return true;
	  }
	}
      }
    }
    std::cerr <<"not good"<<std::endl;
	    
    return false;
  }

void NCOORDS::steal_pointer(Nodedeq::iterator in, Nodedeq& out){
    out.push_back(*in);
    *in = NULL;
  }

void Nodes::outputstring(const std::string& filename){
std::ofstream out(filename.c_str());
out.setf(std::ios::unitbuf);
 for (Nodedeq::const_iterator walk = nodes.begin();walk!= nodes.end(); walk++){
   out << *walk<< std::endl;
 }
      out.close();
}
  
void Nodes::outputstring(const std::string& filename, const deqC& printlist)const{
std::ofstream out(filename.c_str());
out.setf(std::ios::unitbuf);
 for (Nodedeq::const_iterator walk = printlist.begin();walk!= printlist.end(); walk++){
   out << *walk<< std::endl;
 }
      out.close();
}

void Nodes::gen_cullable(unsigned int seedcount){
  std::cerr <<"gen_cullable:\ncullable.size() -> "<<cullable.size()
	    <<"\nnodes.size() -> "<< nodes.size() 
	    <<"\nseedcount: "<< seedcount<< std::endl;
  if (seedcount > nodes.size()){
    throw xn_Error("seedcount greater than the number of seeds available");
  }
  double ncoordsangle = this->get_angle();
  float our_angle;
  if ((number_pts%2) ==0){
    our_angle = ncoordsangle;
  }
  else{
    //our_angle = angle;   
    if (ncoordsangle < M_PI){
      our_angle = ncoordsangle+ (M_PI * (loopcount%2)); 
    }
    else{
      our_angle = ncoordsangle- (M_PI * (loopcount%2));
      }
  }
  Nodedeq::iterator temp_walker = nodes.end() - seedcount;
  for(;temp_walker != nodes.end(); temp_walker++){
    coords* seed = *temp_walker;
    ncoords current_points(*seed, radius, our_angle, number_pts);   
    current_points.to_iterator(std::back_inserter(cullable));
  }
  std::cerr <<"exit gen_cullable() cullable.size():"<<cullable.size()
	    <<std::endl;
}

//typedef value_is_unique<Nodedeq, coords> predicate;

unsigned int Nodes::add_nodes(){
  std::cerr <<"add_nodes()"<<std::endl;
  unsigned int result(0);
  doorbitch sentry(this);
  Nodedeq::iterator d_walker = cullable.begin();
  for (; d_walker != cullable.end() ; d_walker++){
    sentry(d_walker, result);
  }
  cullable.clear();
  std::cerr <<"result"<<result<<std::endl;
  return result;
}  

void Nodes::iterate(unsigned int seedcount){
  if(!seedcount){
    return;
  }
  gen_cullable(seedcount);
  std::cerr <<"post gen_cullable: size:"<<cullable.size()<<std::endl;
  seedcount = add_nodes();
  increment_loopcount(); 
  if (loopcount < 18){
    iterate(seedcount);
  }
#ifdef PRINT_NODES
    std::cout << "\n\n\n\n\nfinal coordset list: \n" << nodes << std::endl;
#endif
}


void Nodes::print(Nodedeq& printlist){
  Nodedeq::iterator node_walker = printlist.begin();
  for(;node_walker != printlist.end(); node_walker++){
        std::cout << *(*node_walker);
  }
}

void Nodes::print(setC& printlist){
  for(setC::iterator node_walker = printlist.begin();node_walker != printlist.end();  node_walker++){
    std::cout << *(*node_walker);
  }
}

void Nodes::translate(Nodedeq& list, const coords* offset){
  for (Nodedeq::iterator walker = list.begin();walker!= list.end(); walker++){
    (*walker)->translate(offset);
  }
}

void Nodes::deliver(Nodedeq& delivery){
  delivery.swap(nodes);
}

const coords& Nodes::find_closest(double ex, double why)const {
  coords neighbour(ex,why);
  return find_closest(&neighbour);
}

const coords& Nodes::find_closest(const coords* neighbour)const{
if(!nodes.size()){
    throw xn_Error("Nodes::find_closest_and_distance():nodes empty: iterate() and then check for closest");
  }
  // you DON'T own the return , so don't delete
  Nodedeq::const_iterator walk = nodes.begin();
  const coords* closest =*walk;
  double min_distance = closest->get_distance(neighbour);
  for(;walk != nodes.end(); ++walk){
    const coords* tested = *walk;
    double distance= tested->get_distance(neighbour);
    if (distance < min_distance){
      min_distance = distance ;
      closest = tested;
    }
  }
  return *closest;
}

std::pair<const coords&, double> 
Nodes::find_closest_and_distance(const coords* neighbour)const{
  if(!nodes.size()){
    throw xn_Error("Nodes::find_closest_and_distance():nodes empty: iterate() and then check for closest");
  }
Nodedeq::const_iterator walk = nodes.begin();
  const coords* closest =*walk;
  double min_distance = closest->get_distance(neighbour);
  for(;walk != nodes.end(); ++walk){
    const coords* tested = *walk;
    double distance= tested->get_distance(neighbour);
    if (distance < min_distance){
      min_distance = distance ;
      closest = tested;
    }
  }
  return std::pair<const coords&, double> (*closest, min_distance);
}
bool    Nodes::base_good()const{
      unsigned int b = get_base();
      return (b == 2 || b== 3 || b==4 || b==6);
    }
/*
void Nodes::get_nodes_without_first_offset(Nodedeq& outnodes, unsigned int which){
  
   * @fn get_nodes_without_first_offset()
   * the first node generated 'shouldn't' be there, 
   * but that's where the mouse was, so 
   * offset the set of points so one of the second/third/fourth points
   is where the first was.
   * @param outnodes -> where the points will be copied to
   * @param which -> which of the first ncoords::number_pts points to be used to determine offset
  
  which = (which % number_pts) +1;
  if(nodes.size() <= number_pts){
    if(!nodes.size()){throw XNLogicError("no points generated when get_nodes_without_first_offset() called");}
    throw XNLogicError("not enough points when get_nodes_without_first_offset() called");
  }
  const coords& theoriginal = *(*(nodes.begin()));
  const coords& newmouse = *(*(nodes.begin()+which));
  const coords mapoffset =  theoriginal - newmouse;
#ifdef DEBUG
  std::cerr << "mouse click, and first point ++> "<<  theoriginal<< "\ncurrent location of newmouse: "
	    << newmouse <<"\noffset to be added to each point: "<<mapoffset<<std::endl;
#endif
  coords tmp;
  for(Nodedeq::iterator walk = nodes.begin() +1;walk != nodes.end(); ++walk){
    tmp = *(*walk) + mapoffset;
#ifdef DEBUG
    std::cerr << "before ==> " <<*(*walk)<<"\nafter ==> "<< tmp<<std::endl;
#endif
    outnodes.push_back(new coords(tmp));
  }
}
*/

coords* Nodes::mark_centre_from_limit(int& list_position, int& serial_no){
//caller responsible for deleting returned pointer
  coords centre(*(*(nodes.begin())));
  coords* offset = new coords(limit_x / 2.0 - centre.get_x(), limit_y / 2.0 - centre.get_x());
  translate(nodes, offset);
  return offset;
}

coords* Nodes::mark_centre_from_click(const coords& click, int& list_position, int& serial_no){
  //caller responsible for deleting returned pointer
 return new coords;
}



void Nodes::invert_nodestate(Nodedeq& coordlist){
  Nodedeq::iterator walk = coordlist.begin();
  Nodedeq::iterator stop = coordlist.end();
  for(;walk!=stop; walk++){
    coords* pt = *walk;
    pt->flip_stateflag(COORDS_UP);
  }
}

void Nodes::save_nodes(const std::string& filename){
  nodes.write_data(nodes,filename);
}

