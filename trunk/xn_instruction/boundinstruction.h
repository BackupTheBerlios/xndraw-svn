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


//file: boundinstruction.h
// TODO: make the sources into .in files and create 
//an extra sub-configure script in some somedir - sy 'create_src'
// to add all repetitive boilerplate

#ifndef XN_INSTRUCTION_BOUNDINSTRUCTION_H
#define XN_INSTRUCTION_BOUNDINSTRUCTION_H

#include "coords.h"
#include <sigc++/sigc++.h>
#include <cairomm/context.h>

namespace xNDRAW{

  //sigc::slots seem useless for non-void returns...

  using NCOORDS::coords; 
  struct Boundmethodbase:public std::unary_function<void, void>{
    virtual ~Boundmethodbase(){}
    void operator()(){	
      sigc::slot<void> slot= get_slot();
      slot();
    }
    virtual sigc::slot<void> get_slot(){return sigc::slot<void>();}
  };
  

  template<typename TP>
  class BoundOpBase: public Boundmethodbase{
  protected:
    TP* methodowner;

  public:
    BoundOpBase(TP* meth_ow=NULL): methodowner(meth_ow){}
    virtual ~BoundOpBase(){}
    TP* get_object(){return  methodowner;}
    void set_object( TP*  o){methodowner = o;}

    virtual sigc::slot<void> create_slot(TP* tp){
      if(!tp){throw xn_Error("NULL");}
      return sigc::slot<void>();
    }

    void operator()(){Boundmethodbase::operator()(); }
    void operator()(TP* tp){
      sigc::slot<void> slot = create_slot(tp);
      slot();
    }
  };

  template<typename TP>
  class BoundOpBase<Cairo::RefPtr<TP> >: public Boundmethodbase{
  protected:
    Cairo::RefPtr<TP>  methodowner;

  public:
    BoundOpBase(TP* meth_ow=NULL): methodowner(meth_ow){
      if(methodowner){methodowner->reference();}
    }
    BoundOpBase(Cairo::RefPtr<TP> tp):  methodowner(tp){
      if(methodowner){methodowner->reference();}
    }

    virtual ~BoundOpBase(){}
    TP* get_object(){return  methodowner.operator->();}

    virtual sigc::slot<void> create_slot(TP* tp){
      if(!tp){throw xn_Error("NULL");}
      return sigc::slot<void>();
    }
    void set_object( TP*  o){
      if(o){o->reference();}
      methodowner = Cairo::RefPtr<TP>(o);//dtor unreferences()
    }
    void operator()(){Boundmethodbase::operator()(); }
    void operator()(TP* tp){
      sigc::slot<void> slot = create_slot(tp);
      slot();
    }
  };

  typedef BoundOpBase<Cairo::RefPtr<Cairo::Context> >  wrapped_context_type;
  //TODO: pointer and reference specialisations: just those we need,not all 40 or so
  //to copy their arg, not address, to avoid segfault
  //template<typename RV, typename TP>
  template<typename RV, typename TP, typename ARG1, typename ARG2,typename ARG3>
  class BoundOp: public BoundOpBase<TP>{
  public:
    typedef RV (TP::*method_type)(ARG1, ARG2,ARG3);
    typedef BoundOpBase<TP> base_type;
    using base_type::methodowner;
    using base_type::get_object;
  protected:
    method_type method;
    ARG1 arg1;
    ARG2 arg2;
    ARG3 arg3;

  public:
    BoundOp(method_type  meth, TP* o, ARG1 a1, ARG2 a2,ARG3 a3):
      base_type(o),
      method(meth),
      arg1(a1),
      arg2(a2),
      arg3(a3){}

    virtual ~BoundOp(){}

    virtual sigc::slot<void> create_slot(TP* tp){
      if(!tp){throw xn_Error("NULL");}
      return sigc::bind_return(sigc::bind(sigc::mem_fun(*tp, method),arg1, arg2, arg3));  
    }

    virtual sigc::slot<void> get_slot(){
      if(!methodowner){
	throw xn_Error("NULL object");
      }
      return sigc::bind_return(sigc::bind(sigc::mem_fun(*(get_object()), method),arg1, arg2, arg3));      
    }
  };

  template<typename TP, typename ARG1, typename ARG2,typename ARG3>
  class BoundOp<void, TP, ARG1, ARG2,ARG3>: public BoundOpBase<TP>{
  public:
    typedef void (TP::*method_type)(ARG1, ARG2,ARG3);
    typedef BoundOpBase<TP> base_type;
    using base_type::methodowner;
    using base_type::get_object;

  protected:
    method_type method;
    ARG1 arg1;
    ARG2 arg2;
    ARG3 arg3;
  public:
    BoundOp(method_type  meth, TP* o, ARG1 a1, ARG2 a2,ARG3 a3):
      base_type(o), 
      method(meth),
      arg1(a1), arg2(a2), arg3(a3){}

    virtual ~BoundOp(){}
    virtual sigc::slot<void> create_slot(TP* tp){
      if(!tp){throw xn_Error("NULL");}
      return sigc::bind(sigc::mem_fun(*tp, method),arg1, arg2, arg3);  
    }

    sigc::slot<void> get_slot(){
      if(!methodowner){
	throw xn_Error("NULL object");
      }
      return sigc::bind(sigc::mem_fun(*(get_object()), method), arg1, arg2, arg3);
      
    }
  };
  //////////////
  //<void, TP, arg1&, arg2, arg3>
  template< typename TP, 
	   typename ARG1, typename ARG2, typename ARG3>
class BoundOp<void, TP, ARG1&, ARG2,ARG3>: public BoundOpBase<TP>{
 public:
    typedef void (TP::*method_type)(ARG1&, ARG2, ARG3);
    typedef BoundOpBase<TP> base_type;
    using base_type::methodowner;
    using base_type::get_object;

  protected:
    method_type method;
    ARG1 arg1;
    ARG2 arg2;
 ARG3 arg3;
  public:
    BoundOp(method_type  meth, TP* o, ARG1& a1, ARG2 a2,ARG3 a3):
      base_type(o),
      method(meth), arg1(a1), arg2(a2), arg3(a3){}

    virtual ~BoundOp(){}
    virtual sigc::slot<void> create_slot(TP* tp){
      if(!tp){throw xn_Error("NULL");}
      return sigc::bind(sigc::mem_fun(*tp, method),arg1, arg2, arg3); 
    }

    sigc::slot<void> get_slot(){
      if(!methodowner){
	throw xn_Error("NULL object");
      }
      return sigc::bind(sigc::mem_fun(*(get_object()), method), arg1, arg2, arg3);
      
    }
  };
  /////////
  
  ////////
  //template<typename RV, typename TP>
  template<typename RV, typename TP, typename ARG1, typename ARG2>
  class BoundOp<RV, TP, ARG1, ARG2,void>: public BoundOpBase<TP>{
  public:
    typedef RV (TP::*method_type)(ARG1, ARG2);
    typedef BoundOpBase<TP> base_type;
    using base_type::methodowner;
    using base_type::get_object;

  protected:
    method_type method;
    ARG1 arg1;
    ARG2 arg2;
 
  public:
    BoundOp(method_type  meth, TP* o, ARG1 a1, ARG2 a2):
      base_type(o),
      method(meth), arg1(a1), arg2(a2){}

    virtual ~BoundOp(){}
    virtual sigc::slot<void> create_slot(TP* tp){
      if(!tp){throw xn_Error("NULL");}
      return sigc::bind_return(sigc::bind(sigc::mem_fun(*tp, method),arg1, arg2)); 
    }

    sigc::slot<void> get_slot(){
      if(!methodowner){
	throw xn_Error("NULL object");
      }
      return sigc::bind_return(sigc::bind(sigc::mem_fun(*(get_object()), method), arg1, arg2));
      
    }
  };

  //////////


 

  template<typename TP, typename ARG1, typename ARG2>
  class BoundOp<void, TP, ARG1, ARG2, void>: public BoundOpBase<TP>{
  public:
    typedef void (TP::*method_type)(ARG1, ARG2);
    typedef BoundOpBase<TP> base_type;
    using base_type::methodowner;
    using base_type::get_object;

  protected:
    method_type method;
    ARG1 arg1;
    ARG2 arg2;
    
  public:
    BoundOp(method_type  meth, TP* o, ARG1 a1, ARG2 a2):
      base_type(o),
      method(meth),
      arg1(a1), 
      arg2(a2){}

    virtual ~BoundOp(){}
    virtual sigc::slot<void> create_slot(TP* tp){
      if(!tp){throw xn_Error("NULL");}
      return sigc::bind(sigc::mem_fun(*tp, method),arg1, arg2);  
    }

    virtual sigc::slot<void> get_slot(){
      if(!methodowner){
	throw xn_Error("NULL object");
      }
      return sigc::bind(sigc::mem_fun(*(get_object()), method),arg1, arg2);
      
    }
  };
  ///////////////////

template<typename TP, typename ARG1, typename ARG2>
  class BoundOp<void, TP, ARG1&, ARG2, void>: public BoundOpBase<TP>{
  public:
    typedef void (TP::*method_type)(ARG1&, ARG2);
    typedef BoundOpBase<TP> base_type;
    using base_type::methodowner;
    using base_type::get_object;

  protected:
    method_type method;
    ARG1 arg1;
    ARG2 arg2;
    
  public:
    BoundOp(method_type  meth, TP* o, ARG1 a1, ARG2 a2):
      base_type(o),
      method(meth),
      arg1(a1), 
      arg2(a2){}

    virtual ~BoundOp(){}
    virtual sigc::slot<void> create_slot(TP* tp){
      if(!tp){throw xn_Error("NULL");}
      return sigc::bind(sigc::mem_fun(*tp, method),arg1, arg2);  
    }

    virtual sigc::slot<void> get_slot(){
      if(!methodowner){
	throw xn_Error("NULL object");
      }
      return sigc::bind(sigc::mem_fun(*(get_object()), method),arg1, arg2);
      
    }
  };

  ////////////////////////
  template<typename TP, typename ARG1>
  class BoundOp<void, TP, ARG1, void, void>: public BoundOpBase<TP>{
  public:
    typedef void (TP::*method_type)(ARG1);
    typedef BoundOpBase<TP> base_type;
    using base_type::methodowner;
    using base_type::get_object;
  protected:
    method_type method;
    ARG1 arg1;
    
  public:

    virtual ~BoundOp(){}

    BoundOp(method_type  meth, TP* o, ARG1 a1):
      base_type(o), 
      method(meth),arg1(a1){}

    virtual sigc::slot<void> create_slot(TP* tp){
      if(!tp){throw xn_Error("NULL");}
      return sigc::bind(sigc::mem_fun(*tp, method),arg1);  
    }
    sigc::slot<void> get_slot(){
      if(!methodowner){
	throw xn_Error("NULL object");
      }
      return sigc::bind(sigc::mem_fun(*(get_object()), method), arg1);
      
    }
  };
  //////////////specialisation for reference at first arg
  template<typename TP, typename ARG1>
  class BoundOp<void, TP, ARG1&, void, void>: public BoundOpBase<TP>{
  public:
    typedef void (TP::*method_type)(ARG1&);
    typedef BoundOpBase<TP> base_type;
    using base_type::methodowner;
    using base_type::get_object;
  protected:
    method_type method;
    ARG1 arg1;//not a reference
    
  public:

    virtual ~BoundOp(){}

    BoundOp(method_type  meth, TP* o, ARG1& a1):
      base_type(o), 
      method(meth),arg1(a1){}

    virtual sigc::slot<void> create_slot(TP* tp){
      if(!tp){throw xn_Error("NULL");}
      return sigc::bind(sigc::mem_fun(*tp, method),arg1);  
    }
    sigc::slot<void> get_slot(){
      if(!methodowner){
	throw xn_Error("NULL object");
      }
      return sigc::bind(sigc::mem_fun(*(get_object()), method), arg1);
      
    }
  };
  ///// +//
 template<typename TP, typename ARG1>
  class BoundOp<void, TP, ARG1*, void, void>: public BoundOpBase<TP>{
  public:
    typedef void (TP::*method_type)(ARG1*);
    typedef BoundOpBase<TP> base_type;
    using base_type::methodowner;
    using base_type::get_object;
  protected:
    method_type method;
    ARG1 arg1;//not a pointer
    
  public:

    virtual ~BoundOp(){}

    BoundOp(method_type  meth, TP* o, ARG1* a1):
      base_type(o), 
      method(meth),arg1(*a1){}

    virtual sigc::slot<void> create_slot(TP* tp){
      if(!tp){throw xn_Error("NULL");}
      return sigc::bind(sigc::mem_fun(*tp, method),&arg1);  
    }
    sigc::slot<void> get_slot(){
      if(!methodowner){
	throw xn_Error("NULL object");
      }
      return sigc::bind(sigc::mem_fun(*(get_object()), method), &arg1);
      
    }
  };

  //template<typename RV, typename TP>
  template<typename RV, typename TP>
  class BoundOp<RV, TP, void, void,void>: public BoundOpBase<TP>{
  public:
    typedef RV (TP::*method_type)();
    typedef BoundOpBase<TP> base_type;
    using base_type::methodowner;
    using base_type::get_object;
  protected:
    method_type method;
 
  public:

    virtual ~BoundOp(){}

    BoundOp(method_type  meth, TP* o):
      base_type(o),
      method(meth){}

    virtual sigc::slot<void> create_slot(TP* tp){
      if(!tp){throw xn_Error("NULL");}
      return sigc::bind_return(sigc::mem_fun(*tp, method), true);  
    }
    sigc::slot<void> get_slot(){
      if(!methodowner){
	throw xn_Error("NULL object");
      }
      return sigc::bind_return(sigc::mem_fun(*(get_object()), method), true);
      
    }
  };

  template<typename TP>
  class BoundOp<void, TP, void, void, void>: public BoundOpBase<TP>{
  public:
    typedef void (TP::*method_type)();
    typedef BoundOpBase<TP> base_type;
    using base_type::methodowner;
    using base_type::get_object;
  protected:
    method_type method;
    
  public:

    virtual ~BoundOp(){}

    BoundOp(method_type  meth, TP* o):
      base_type(o),
      method(meth){}
    virtual sigc::slot<void> create_slot(TP* tp){
      if(!tp){throw xn_Error("NULL");}
      return sigc::mem_fun(*tp, method);  
    }
  
    sigc::slot<void> get_slot(){
      if(!methodowner){
	throw xn_Error("NULL object");
      }
      return sigc::mem_fun(*(get_object()), method);
      
    }
  };

  template<typename RV, typename TP, typename ARG1, typename ARG2,typename ARG3>
  BoundOpBase<TP>* create_boundop(RV (TP::*method)( ARG1, ARG2, ARG3),
				  TP* tp,
				  ARG1 a1, ARG2 a2, ARG3 a3){
    return new BoundOp<RV,TP, ARG1, ARG2, ARG3>(method, tp,a1,a2,a3);
  }

  template<typename TP, typename ARG1, typename ARG2,typename ARG3>
  BoundOpBase<TP>* create_boundop(void (TP::*method)( ARG1, ARG2, ARG3),
				  TP* tp,
				  ARG1 a1, ARG2 a2, ARG3 a3){
    return new BoundOp<void,TP, ARG1, ARG2, ARG3>(method, tp,a1,a2,a3);
  }
  template< typename TP, typename ARG1, typename ARG2,typename ARG3>
  BoundOpBase<Cairo::RefPtr<TP> >* create_boundop(void (TP::*method)(ARG1, ARG2, ARG3),
						  Cairo::RefPtr<TP> tp,
						  ARG1 a1, ARG2 a2, ARG3 a3){
    return new BoundOp<void,Cairo::RefPtr<TP>,ARG1 , ARG2,ARG3>(method, tp, a1,a2, a3);
  }

  template<typename RV, typename TP, typename ARG1, typename ARG2>
  BoundOpBase<TP>* create_boundop(RV (TP::*method)( ARG1, ARG2),
				  TP* tp,
				  ARG1 a1, ARG2 a2){
    return new BoundOp<RV, TP, ARG1, ARG2, void>(method, tp,a1,a2);
  }
  template<typename TP, typename ARG1, typename ARG2>
  BoundOpBase<TP>* create_boundop(void (TP::*method)( ARG1, ARG2),
				  TP* tp,
				  ARG1 a1, ARG2 a2){
    return new BoundOp<void, TP, ARG1, ARG2, void>(method, tp,a1,a2);
  }

  template< typename TP, typename ARG1, typename ARG2>
  BoundOpBase<Cairo::RefPtr<TP> >* create_boundop(void (TP::*method)(ARG1, ARG2),
						  Cairo::RefPtr<TP> tp,
						  ARG1 a1, ARG2 a2){
    return new BoundOp<void,Cairo::RefPtr<TP>,ARG1 , ARG2,void>(method, tp, a1,a2);
  }


  template<typename RV, typename TP, typename ARG1>
  BoundOpBase<TP>* create_boundop(RV (TP::*method)( ARG1),
				  TP* tp,
				  ARG1 a1){
    return new BoundOp<RV, TP, ARG1, void, void>(method, tp,a1);
  }

  template<typename TP, typename ARG1>
  BoundOpBase<TP>* create_boundop(void (TP::*method)( ARG1),
				  TP* tp,
				  ARG1 a1){
    return new BoundOp<void, TP, ARG1, void, void>(method, tp,a1);
  }
  /*
template<typename TP, typename ARG1>
  BoundOpBase<TP>* create_boundop(void (TP::*method)( ARG1&),
				  TP* tp,
				  ARG1& a1){
    return new BoundOp<void, TP, ARG1&, void, void>(method, tp,a1);
  }
template<typename TP, typename ARG1>
  BoundOpBase<TP>* create_boundop(void (TP::*method)( const ARG1&),
				  TP* tp,
				  const ARG1& a1){
    return new BoundOp<void, TP, const ARG1&, void, void>(method, tp,a1);
  }
  */
  template< typename TP>
  BoundOpBase<Cairo::RefPtr<TP> >* create_boundop(void (TP::*method)(Cairo::Matrix const&),
						  Cairo::RefPtr<TP> tp,
						  const Cairo::Matrix& m){				 return new BoundOp<void,Cairo::RefPtr<TP>,const Cairo::Matrix&, void, void>(	method, tp, m);
  }
template< typename TP>
  BoundOpBase<TP>* create_boundop(void (TP::*method)( Cairo::Matrix const&),
				  TP* tp,
						  const Cairo::Matrix& m){				 return new BoundOp<void,TP,const Cairo::Matrix&, void, void>(	method, tp, m);
  }

  template< typename TP, typename ARG1>
  BoundOpBase<Cairo::RefPtr<TP> >* create_boundop(void (TP::*method)(ARG1),
						  Cairo::RefPtr<TP> tp,
						  ARG1 a1){
    return new BoundOp<void,Cairo::RefPtr<TP>,ARG1 , void,void>(method, tp, a1);
  }
  template< typename TP>
  BoundOpBase<TP>* create_boundop(void (TP::*method)(),
				  TP* tp){
    return new BoundOp<void,TP, void, void,void>(method, tp);
  }

  template< typename TP>
  BoundOpBase<Cairo::RefPtr<TP> >* create_boundop(void (TP::*method)(),
						  Cairo::RefPtr<TP> tp){
    return new BoundOp<void,Cairo::RefPtr<TP>, void, void,void>(method, tp);
  }

  template<typename RV, typename TP>
  BoundOpBase<TP>* create_boundop(RV (TP::*method)(),
				  TP* tp){
    return new BoundOp<RV,TP, void, void,void>(method, tp);
  } 

  //TODO - for 0,2, 3, 4, 6(curve_t0()-use 3 arg version) arguments
  // and for freestanding functions
  //create sigc slots instead?
  template <class TYPE,
	    class BOUNDARG = std::pair<coords*, int> >
  class boundMethod: 
    public Boundmethodbase{

  public:
    typedef BOUNDARG value_type;
    typedef void (TYPE::*method_type)(BOUNDARG);

  protected:
    TYPE* instance;
    BOUNDARG arg;
  
    std::mem_fun1_t<void, TYPE, BOUNDARG> method;
  public:
    boundMethod(TYPE* obj,method_type meth, BOUNDARG argument):
      instance(obj),
      arg(argument),// provide a special;isation to create and delete a coords
      method(meth)  {}
    virtual ~boundMethod(){}

    void operator()(){
      method(instance, arg);
    }
  };

  template <class TYPE,
	    class BOUNDARG>
  boundMethod<TYPE, BOUNDARG> bind_method(TYPE* obj,void (TYPE::*method_t)(BOUNDARG), BOUNDARG argument){
    return boundMethod<TYPE, BOUNDARG>(obj, method_t, argument);
  }

  template <class TYPE,
	    class BOUNDARG>
  boundMethod<TYPE, BOUNDARG>* bind_method_ptr(TYPE* obj,void (TYPE::*method_t)(BOUNDARG), BOUNDARG argument){
    return new boundMethod<TYPE, BOUNDARG>(obj, method_t, argument);
  }

  //template <typename TYPE, >


  struct TestA{
    virtual void test(int n){
      std::cout << "TestA::test "<< std::endl;
    }
    virtual ~TestA(){}
  };

  struct TestB: public TestA{
    virtual ~TestB(){}
    virtual void test(int n){
      std::cout << "TestB::test "<< std::endl;
    }
  };

  struct TestC: public TestB{
    virtual ~TestC(){}
    virtual void test(int n){
      std::cout << "TestC::test "<< std::endl;
    }
  };
} // namespace xNDRAW

#endif // XN_INSTRUCTION_BOUNDINSTRUCTION_H
