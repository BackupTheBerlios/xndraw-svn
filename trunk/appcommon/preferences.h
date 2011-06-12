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
 * @file preferences.h
 * classes to contain preferences of various types
 */

#ifndef SNARKY_APPCOMMON_PREFERENCES_H
#define SNARKY_APPCOMMON_PREFERENCES_H

#ifdef HAVE_CONFIG_H
#include "appcommonconfig.h"
#endif

#ifndef NAMESPACE_APPCOMMON
#define NAMESPACE_APPCOMMON namespace APPCOMMON
#define NAMESPACE APPCOMMON
#endif
//define SnarkyMap as some hash_map implementation
//, or if one can't be found, 
//fall back to std::map
#ifndef SnarkyMap
#define SnarkyMap XndrawMap
#endif
#if HAVE_HASHMAP 

// hash<SOMETYPE>::operator()(SOMETYPE)
///must be DEFINEd 
//to use SOMETYPE as a key_type in some hash map
//see wanthash.h for examples.
//Obviously keytype should make a fair crack at immutability
//lest the hash mutate also
#include "wanthash.h"

#if  HAVE_UNORDERED_MAP
#include <unordered_map>
#else
#if HAVE_TR1_UNORDERED_MAP
#include <tr1/unordered_map>
#else
#if HAVE_EXT_HASH_MAP
#include <ext/hash_map>
#else 
#error "HAVE_HASHMAP defined but no candidate indicated. config error"
#endif //HAVE_EXT_HASH_MAP
#endif //HAVE_TR1_UNORDERED_MAP
#endif //HAVE_UNORDERED_MAP
#else
#include <map>
#endif // HAVE_HASHMAP


//define SnarkySet as some hash_set implemantation
//, or if one can't be found, 
//fall back to std::set
#ifndef SnarkySet
#define SnarkySet XndrawSet
#endif

#if HAVE_HASHSET
#include "wanthash.h"
#if  HAVE_UNORDERED_SET
#include <unordered_set>
#else
#ifdef HAVE_TR1_UNORDERED_SET
#include <tr1/unordered_set>
#else
#ifdef  HAVE_EXT_HASH_SET
#include <ext/hash_set>
#else 
#error "HAVE_HASHSET defined but no candidate indicated. config error"
#endif //USE_EXT_HASH_SET
#endif //USE_TR1_UNORDERED_SET
#endif //USE_UNORDERED_SET
#else
#include <set>
#endif // HAVE_HASHSET

#include "exceptions.h"
#include "fileexists.h"

#include <locale>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cstdlib>
#include "stupidiorefs.h"

#ifdef DEBUG
#include <typeinfo>
#endif



//prefs - this is all over the place now
// the global preferences should load at startup, reading any
// files and command line arguments(so that the command line args override)
// and setting hint key/val pairs
//prefernces used to create App instances should then copy their prefs
//from the globals , modifying them with any options supplied in python etc constructors

NAMESPACE_APPCOMMON{

  enum{
    SNARKY_FATAL = -1,
    SNARKY_SUCCESS = 0,
    SNARKY_FAILURE = 1,
    SNARKY_EOF = 2
  };
  
  std::string get_user_prefsdirpath();
  std::string get_hometempdirpath();
  std::string get_sys_prefsdirpath();

  std::string get_prefsTypeHint(const std::string& key);
  void set_prefsTypeHint(const std::string& name, const std::string& typehint);

  std::vector<std::string> list_dir(DIR* d);

template <typename CHARTYPE>
    std::pair< typename std::basic_string<CHARTYPE>::const_iterator, typename std::basic_string<CHARTYPE>::const_iterator>
    snarky_strip_whitespace(typename std::basic_string<CHARTYPE>::const_iterator first, 
			    typename std::basic_string<CHARTYPE>::const_iterator last, 
			    const std::locale& loc){
    typedef typename std::basic_string<CHARTYPE>::const_iterator iterator;
    typedef std::basic_string<CHARTYPE> std_type;
    typedef std::pair<iterator, iterator> iterator_pair;
    while(std::isspace(*first, loc) && first != last){ first++; }
    while(std::isspace(*(last-1), loc) && first != last){ last--; }
    return iterator_pair(first, last);
  }

  ///valuetype
   template<typename T>
  std::string get_valueType(const T& t){
    return std::string();
  }

  template <>
  std::string get_valueType<unsigned int>(const unsigned int& i);
  
  template <>
  std::string get_valueType<std::string>(const std::string& i);
  
  template <>
  std::string get_valueType<int>(const int& i);
  
  //note : char* treated like std::string(and converted into one)
  template <>
  std::string get_valueType<char*>(char* const& i);
  
  template <>
  std::string get_valueType<const char*>(const char* const& i);

  template <>
  std::string get_valueType<float>(const float& i);
  
  template <>
  std::string get_valueType<double>(const double& d);
  
  template <>
  std::string get_valueType<size_t>(const size_t& d);
  
  template <>
  std::string get_valueType<ptrdiff_t>(const ptrdiff_t& d);
  
  template <>
  std::string get_valueType<long>(const long& d);
  
  template <>
  std::string get_valueType<unsigned long>(const unsigned long& d);

  template <>
  std::string get_valueType<char>(const char& d);
  
  template <>
  std::string get_valueType<unsigned char>(const unsigned char& d);
  
  template <>
  std::string get_valueType<short>(const short& d);
  
  template <>
  std::string get_valueType<unsigned short>(const unsigned short& d);
  
  class Preferences;
  
  struct Preferencebase{
    
    virtual Preferencebase* selfcopy();/// ?? attempt to copy derived type  
    
    Preferencebase(const char* s):name(s){}
       Preferencebase(const std::string& s):name(s){}
    virtual ~Preferencebase(){}
    
    virtual std::string get_typeHint()const;
    
    virtual void copyto(Preferences* prefs){
      /**
       *helper for Preferences::Preferences(const Preferences&).
       * override in derived class
       * \param prefs - the Preferences we are copying to
       */
}
  
    /// 
    const std::string name;
    
  };
  
  template <class VALUETYPE>
  class Preference: public Preferencebase{
    typedef VALUETYPE value_type;
    
  protected:
    VALUETYPE value;
    
  public:
    friend class Preferences;
    using Preferencebase::name;
    
    Preference(const char* name, VALUETYPE val):Preferencebase(name), value(val){}
    Preference(const std::string& name, VALUETYPE val):Preferencebase(name), value(val){}
    
    virtual ~Preference(){
#ifdef DEBUG
      std::cerr << "~Preference(): another preference<T> going doewn..." <<std::endl;
#endif // DEBUG
    }
    
    virtual Preferencebase* selfcopy(){
      return new Preference(name, value);
    }

    virtual  std::string get_typeHint()const{
      std::string basehint = Preferencebase::get_typeHint();//look up name in registry
      VALUETYPE v;
      std::string valtype = get_valueType(v);
      //if(valtype == basehint){return basehint;}
      if(valtype.size() && !basehint.size()){
	throw PreferenceError("prefhint not set on pref initialisation");
      }
      return basehint;
    }
    
    template <typename VAL2>
    Preference& operator=(VAL2 src){
      std::stringstream tmpios(std::ios_base::in|std::ios_base::out);
      tmpios << src;
      tmpios >> value; 
      value = src;
      
      return *this;
    }
    
    void get(VALUETYPE& outval)const{outval = value;}
    template <class VAL2>
    void get(VAL2& outval)const{
      std::stringstream tmpios(std::ios_base::in|std::ios_base::out);
      tmpios << value;
      tmpios >> outval; 
    }
    void set(VALUETYPE val){value = val;}
    
    template <class VAL2>
    void set(VAL2 val){ 
      std::stringstream tmpios(std::ios_base::in|std::ios_base::out);
      //if val not compatible type with value, no change
      tmpios << val;
      tmpios >> value; 
    }

    virtual void copyto(Preferences* prefs);
    
  };
  
  //template <class CHARTYPE>
  std::ostream& operator << (  std::ostream& os, const  Preferences& prefs);
  
  class Preferences{
  public:
    typedef std::pair<const std::string, Preferencebase*> pair_type;
    typedef std::map<const std::string, Preferencebase*> map_type;
    typedef map_type::iterator iterator;
    typedef map_type::const_iterator const_iterator;
    typedef std::pair<std::string, std::string> typehint_type;
    typedef SnarkyMap<std::string, std::string> typehint_map_type;
    typedef typehint_map_type::const_iterator typehint_const_iterator;
    
  protected:
    
    map_type m_map;
    static SnarkyMap<std::string, std::string> registry;//name, type
    
  public:

    iterator begin();
const_iterator begin() const;
iterator end();
const_iterator end() const;
    //friend class PreferenceBase;

    Preferences();
 
    virtual  ~Preferences();

  protected:

    Preferences(const Preferences& src);
    
    Preferences& operator=(const Preferences& src);

  public:
    
    /***
     * @ITERATOR iterator to some container of pair_type
     ***/
    template <class ITERATOR>
    Preferences(ITERATOR start, ITERATOR end){
      insert(start,end);
    }
    
    //    static Preferences create(const char* prefspath = NULL){}
    
    Preferencebase*& operator[](const std::string& name_);

    Preferencebase*& operator[](const char* name_);

    

    std::deque<std::string> keys();

    static SnarkyMap<std::string, std::string>& get_registrystatic();
    static const SnarkyMap<std::string, std::string>& get_registrystatic_const();

    SnarkyMap<std::string, std::string>& get_registry();
    const SnarkyMap<std::string, std::string>& get_registry()const;
 
    friend std::ostream& operator <<(  std::ostream& os, const  Preferences& prefs);
    
    unsigned int get_depth()const;

    void do_throw_error(const char* msg);

    bool have_pref(const std::string& s)const;
    
    virtual void set_defaults(){}

    void add_registryKey(const char* prefkey, const char* hint);
    static void add_registryKeyStatic(const char* prefkey, const char* hint);
       
    void failed_get_exception(const std::string& prefrequest)const;
    
    template <class ITERATOR>
      void insert(ITERATOR start, ITERATOR end){
      for(;start !=end; start++){
	//pair_type pr(*start);
	m_map.insert(*start);
      }
    }
    
    const Preferencebase& get_preference(const std::string& prefname)const;
    
    Preferencebase& get_preference(const std::string& prefname);
    
    template <class VALUETYPE>
    void get_value(const std::string& prefname, VALUETYPE& outval)const{
#ifdef DEBUG
      std::cerr <<"looking up pref: " << prefname<<std::endl;
#endif
      if(!have_pref(prefname)){

	std::cerr << "Preferences::get_value(preference, value): no such preference: "<< prefname 
		  << "value returned unchanged"<< std::endl;
	return;
      }
      const Preferencebase* v_pref = &get_preference(prefname);
      std::string valtypestring = get_valueType(outval);
      std::string prefhint =  v_pref->get_typeHint();
#ifdef DEBUG
      std::cerr << "preferences.get_value(): type of supplied value outval: " 
		<<valtypestring<< "hint in name registry: "<< prefhint
		<< std::endl;
#endif
      if (valtypestring == prefhint && valtypestring.size()){
	const Preference<VALUETYPE>* pref = dynamic_cast<const Preference<VALUETYPE>*>(v_pref);
	if(!pref){
	  std::ostringstream ostmp;
	  ostmp << "Fatal error: Preference valuetype/ typehint mismatch|npreference name: " << prefname <<"\nget_Valuetype(outval)"<< valtypestring;
	  throw PreferenceError(ostmp.str());
	}
	pref->get(outval);
      }
      else {
	get_value_coerced(v_pref, outval, prefhint);
      }
    }


  protected://internal

template <class VALUETYPE>
void get_value_coerced( const Preferencebase* p,VALUETYPE& v, const std::string& prefhint/*from the preference*/)const {
  if(prefhint == "string"){
    const Preference<std::string>* str_p= dynamic_cast<const Preference<std::string>*>(p);
    if(!str_p){throw PreferenceError("should be Preference<std::string>");}
    str_p->get(v);
  }
  else if(prefhint== "uint"){
    const Preference<unsigned int>* uint_p= dynamic_cast<const Preference<unsigned int>*>(p);
    if(!uint_p){throw PreferenceError("should be Preference<unsigned int>");}
    uint_p->get(v);
  }
  else if(prefhint== "int"){
    const Preference<int>* int_p= dynamic_cast<const Preference<int>*>(p);
    if(!int_p){throw PreferenceError("should be Preference<int>");}
    int_p->get(v);
  }
  else if(prefhint == "double"){
const Preference<double>* db_p= dynamic_cast<const Preference<double>*>(p);
    if(!db_p){throw PreferenceError("should be Preference<double>");}
    db_p->get(v);
  }
  else{
    std::cerr << "get_value_coerced<"<< prefhint<<"> unsupported\n";
  }

}
    public:
   
    std::deque<std::string> keys()const;
    
    
    
    void set_value(const std::string& prefname,const char* val);
    

    template <class VALUETYPE>
    void set_value(const std::string& prefname, const VALUETYPE& v){
      std::string typenow = registry[prefname];      
      Preferencebase* v_pref = NULL;
      if(have_pref(prefname)){
	v_pref = &get_preference(prefname);   
	Preference<VALUETYPE>* pref = dynamic_cast<Preference<VALUETYPE>*>(v_pref);
	if(!pref){
	  std::string prefhint = v_pref->get_typeHint();
	  if(prefhint.size() && prefhint != get_valueType(v)){
	    set_value_coerced(v_pref, v, prefhint);
	  }
	  else{std::cerr<< "set_value: operation ubsupported for pref "<<prefname<<std::endl;}
	}
	else{
	  pref->set(v);
	}
      }
      else{
#ifdef DEBUG_VERBOSE
	std::cerr << "\nCreating new preference...:\t'" << prefname << "' = " << v<< std::endl;
#endif
	
	//m_map[prefname] = new Preference<VALUETYPE>(prefname, v);
	create_preference(prefname, v);
      }
    }
    
    
  protected:
    template <class VALUETYPE>
    void set_value_coerced(Preferencebase* v_pref, VALUETYPE v, std::string prefhint ){
      std::string vtype = get_valueType(v);
      if(prefhint == "string"){
	//throw PreferenceError("set_value_coerced_numeric -string shouldn't be here");
	std::ostringstream ostmp;
	  Preference<std::string>* str_p= dynamic_cast<Preference<std::string>*>(v_pref);
	if(!str_p){throw PreferenceError("should be Preference<std::string>");}
	str_p->set(v);
      }
      else if(prefhint== "uint"){
	Preference<unsigned int>* uint_p= dynamic_cast<Preference<unsigned int>*>(v_pref);
	if(!uint_p){throw PreferenceError("should be Preference<unsigned int>");}
	uint_p->set(v);
      }
      else if(prefhint== "int"){
	Preference<int>* int_p= dynamic_cast<Preference<int>*>(v_pref);
	if(!int_p){throw PreferenceError("should be Preference<int>");}
	int_p->set(v);
      }
      else if(prefhint == "double"){
	Preference<double>* db_p= dynamic_cast<Preference<double>*>(v_pref);
	if(!db_p){throw PreferenceError("should be Preference<double>");}
	db_p->set(v);
      }
      else{
	if(!vtype.size()){vtype = "T";}
	std::cerr << "get_value_coerced<" << vtype<<"> unsupported with prefhint "<< prefhint<< std::endl;;
      }
    }
  public:
    
    template <typename VALUETYPE>
    void create_preference(const std::string& pname, VALUETYPE v){
      std::string valtype = get_valueType(v);
      std::string prefhint = get_typehint(pname);
      if(!prefhint.size()){registry[pname] = valtype;}
      m_map[pname] = new Preference<VALUETYPE>(pname, v);
    }

    std::locale getloc();
               
    void read_prefsfile(std::istream& fprefs);
    void set_valuefromstring(const std::string& option, const std::string& val, std::string typehint = std::string());

    static std::string get_typehint(const std::string& key);
    static void set_typehint(const std::string& name, const std::string& typehint);
    std::string get_typeHint(const char* pname)const;
    std::string get_typeHint(const std::string& pname)const;

  };

template <>
void Preferences::get_value<char*>(const std::string& pref, char*& v)const;
template <>
void Preferences::get_value<const char*>(const std::string& pref, const char*& v)const;

template <>
void Preferences::create_preference<char*>(const std::string& pname, char* v);

template <>
void Preferences::create_preference<const char*>(const std::string& pname, const char*  v);

template <>
void Preferences::create_preference<short>(const std::string& pname, short  v);

template <>
void Preferences::create_preference<char>(const std::string& pname, char  v);
  /*
template <>
void Preferences::create_preference<size_t>(const std::string& pname, size_t  v);

template <>
void Preferences::create_preference<ptrdiff_t>(const std::string& pname, ptrdiff_t  v);
  */
template <>
void Preferences::create_preference<unsigned short>(const std::string& pname, unsigned short  v);

template <>
void Preferences::create_preference<unsigned char>(const std::string& pname, unsigned char v);

    template <>
    void Preferences::create_preference<unsigned long>(const std::string& pname, unsigned long v);

template <>
void Preferences::create_preference<long>(const std::string& pname, long v);

template <>
void Preferences::create_preference<float>(const std::string& pname, float v);

template <class VALUETYPE>
  void Preference<VALUETYPE>::copyto(Preferences* prefs){
  VALUETYPE v;
  get(v);
  if(prefs->have_pref(name)){
    prefs->set_value(name, v);
  }
  else{
    prefs->create_preference(name, v);
  }

 }
}
#endif //SNARKY_APPCOMMON_PREFERENCES_H
