#include "bouncenav.h"

namespace NCOORDS{

  double Bounceparms::clamp_angle_value(double inangle)const{
      if(inangle > (M_PI-anglelimit)){return M_PI-anglelimit;}
      else if(inangle < anglelimit){return anglelimit;}
      return inangle;
    }

  Bounceparms::Bounceparms(size_t max_recurs,size_t keep_tries, 
		size_t launches,double howrand, 
		double anglelmt/**<15 degrees*/):
      max_recursion(max_recurs),
      maxlaunches(launches), keeplaunches(keep_tries),
      randification (howrand),anglelimit(anglelmt)
    {}

  ///Bounce::Bounce(; )
  Bounce::Bounce( const Segment<coords*>& bouncefrom, 
	  int camefrom):grandparent(this),
      parent(NULL),
      bouncepad(bouncefrom),
      bouncedirection(camefrom), 
      recursions(0){
      initialise_launch_count();
    }

 Bounce::Bounce(Bounce* prent, 
	   const Segment<coords*>& bouncefrom):  
      grandparent(get_valid_granny(prent)), 
	parent(prent),
	params(grandparent->params),
	bouncepad(bouncefrom),
	bouncedirection(POINTSIDE_ERROR), 
	recursions(parent->recursions+1){	
      initialise_launch_count();
      coords srccoords=parent->bouncepoint();
	bouncedirection =bouncepad.pointside(srccoords);
      }

void Bounce::initialise_launch_count(){
	for(size_t i=0;i< params.maxlaunches;i++){
	  launch_count[i]=false;
	}
      }

 bool Bounce::launches_exhausted()const{
	for (const_launchcount_iterator w = launch_count.begin(); w!= launch_count.end();w++){
	  if(!((*w).second)){return false;}
	}
	return true;
      }

      Bounce* Bounce::get_valid_granny(Bounce* src){
	if (!src){throw xn_Error("NULL argument");}
	if(!(src->grandparent)){throw xn_Error("NULL grandparent in arg");}
	return src->grandparent;
      }

double Bounce::get_distance(const Bounce& bounce)const{
	  coords mpt(bouncepoint()), upt(bounce.bouncepoint());
	  return mpt.get_distance(&upt);
	}
  ///currently ranker is irrelevant
  bool Bounce::rank(const Bounce& left, const Bounce& right)const{
      if(left.launches_exhausted()){return false;}
      else if (right.launches_exhausted()){return true;}
      if(left.is_grandparent()){return true;}
      if(right.is_grandparent()){return false;}
      double last_jumpleft=left.last_hop_gained();
      double last_jumpright=right.last_hop_gained();
      if(last_jumpleft > (2* last_jumpright)){return true;}
      else if(last_jumpright > (2* last_jumpleft)){return false;}
      double avgleft=left.average_bounce();
      double avgright=right.average_bounce();
      if(avgleft > (2* avgright)){return true;}
      else if(avgright > (2* avgleft)){return false;}
      return left.bounce_aggregate()>= right.bounce_aggregate();
      }

Linear_function<double>  Bounce::generate_bouncefn(size_t launchno){
	  double rangeamt = M_PI- (2*params.anglelimit);
	  if((rangeamt <0)||(rangeamt >M_PI)){throw xn_Error("invalid value: (rangeamt <0)||(rangeamt >M_PI)");}
	  double launchrange = rangeamt/launch_count.size();
	  double midrange= (launchno * launchrange) + (launchrange/2.);
	  double randval = (((std::rand()%20000)-10000)/10000.0) *params.randification * (launchrange/2.0);
	  midrange= clamp_angle_value(midrange + randval);
	  if(bouncedirection ==POINTSIDE_RIGHT){midrange += M_PI;}
	  return bouncepad.get_function_from_midpoint(midrange);
	}

  double Bounce::bounce_aggregate()const{
	  if(!parent){return 0.0;}
	  coords ours(bouncepoint()), theirs(grandparent->bouncepoint());
	  return ours.get_distance(&theirs);
	}

    double Bounce::last_hop_distance()const{
      if(!parent){return -1;}
      return get_distance(*parent);
    }

double Bounce::last_hop_gained()const{
  if(!parent){throw xn_Error("last_hop_gained(): grandparent hasn't gone anywhere!");}
  return bounce_aggregate()-parent->bounce_aggregate();
    }
}
