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


#define COMPILING_STUPIDAPP 1

#include "preferences.h"
#include "fileexists.h"
#include <stdexcept>
#include <cstdlib>
#include <sstream>
//#include "userapp.h"

using NAMESPACE_APPCOMMON;

SnarkyMap<std::string, std::string> Preferences::registry;//call default ctor

void Preferences::add_registryKey(const char* prefkey, const char* hint){
  registry[prefkey] = hint;
}
void Preferences::add_registryKeyStatic(const char* prefkey, const char* hint){
  registry[prefkey] = hint;
}

void  Preferences::do_throw_error(const char* msg){
  ///this function doesn't return
  throw PreferenceError(msg);
}

Preferences::Preferences(){
  //todo copy from globals - in derived class
}

Preferences::~Preferences(){
  for(map_type::iterator start = m_map.begin(); start != m_map.end(); start++){
    if ((*start).second){
      //delete (*start).second;//Preference missing virtual dTor  causing segfault?
      //(*start).second = NULL;
    }
    else
      {
	std::cerr << "NULL Preference in map!" <<std::endl;
      }
  }
}

Preferences::iterator Preferences::begin(){
  return m_map.begin();
}
Preferences::const_iterator Preferences::begin() const{
  return m_map.begin();
}
Preferences::iterator Preferences::end(){
  return m_map.end();
}
Preferences::const_iterator Preferences::end() const{
  return m_map.end();
}

Preferences::Preferences(const Preferences& src){
  for(const_iterator i= src.m_map.begin();i != src.m_map.end(); i++){
    ((*i).second)->copyto(this);
  }
}

Preferences& Preferences::operator=(const Preferences& src){
  if(&src != this){
    m_map.clear();//TODO: fix this leak
    for(const_iterator i= src.m_map.begin();i != src.m_map.end(); i++){
      ((*i).second)->copyto(this);
    }
  }
  return *this;
}

Preferencebase*& Preferences::operator[](const std::string& name_){
  return m_map.operator[](name_);
}

Preferencebase*& Preferences::operator[](const char* name_){
  std::string n(name_);
  return m_map.operator[](n);
}


std::deque<std::string> Preferences::keys(){
  std::deque<std::string> result;
  for(const_iterator iter = m_map.begin();iter != m_map.end();iter++){
    result.push_back((*iter).first);
  }
  return result;
}


SnarkyMap<std::string, std::string>& Preferences::get_registrystatic(){
  return registry;
}

const SnarkyMap<std::string, std::string>& Preferences::get_registrystatic_const(){
  return registry;
}

    SnarkyMap<std::string, std::string>& Preferences::get_registry(){
  return registry;
}

    const SnarkyMap<std::string, std::string>& Preferences::get_registry()const{
  return registry;
}

template <>
std::string NAMESPACE::get_valueType<unsigned int>(const unsigned int& i){
  return std::string("uint");
}
template <>
std::string NAMESPACE::get_valueType<std::string>(const std::string& i){
  return std::string("string");
}
template <>
std::string NAMESPACE::get_valueType<int>(const int& i){
  return std::string("int");
}

  //note : char* treated like std::string(and converted into one)
//but will need to overload prefs.get_value_coerced<char*>(p,v,s)
template <>
std::string NAMESPACE::get_valueType<char*>(char* const& i){
  return std::string("string");
}

template <>
std::string NAMESPACE::get_valueType<const char*>(const char* const& i){
  return std::string("string");
}

template <>
std::string NAMESPACE::get_valueType<float>(const float& i){
  return std::string("float");
}

template <>
std::string NAMESPACE::get_valueType<double>(const double& d){
return std::string("double");
}
/*
template <>
std::string NAMESPACE::get_valueType<size_t>(const size_t& d){
return std::string("size_t");
}
template <>
std::string NAMESPACE::get_valueType<ptrdiff_t>(const ptrdiff_t& d){
return std::string("ptrdiff_t");
}
*/
template <>
std::string NAMESPACE::get_valueType<long>(const long& d){
return std::string("long");
}

template <>
std::string NAMESPACE::get_valueType<unsigned long>(const unsigned long& d){
return std::string("unsigned long");
}
template <>
std::string NAMESPACE::get_valueType<char>(const char& d){
return std::string("char");
}

template <>
std::string NAMESPACE::get_valueType<unsigned char>(const unsigned char& d){
return std::string("unsigned char");
}

template <>
std::string NAMESPACE::get_valueType<short>(const short& d){
return std::string("short");
}

template <>
std::string NAMESPACE::get_valueType<unsigned short>(const unsigned short& d){
return std::string("unsigned short");
}

template <>
void Preferences::get_value<char*>(const std::string& pref, char*& v)const{
  std::cerr <<"get_value<char*> nsupported\n";
}
template <>
void Preferences::get_value<const char*>(const std::string& pref, const char*& v)const{
  std::cerr <<"get_value<const char*> unsupported\n";
}

template <>
void Preferences::create_preference<char*>(const std::string& pname, char* v){
  std::string prefhint = get_typehint(pname);
  if(!prefhint.size()){registry[pname] = "string";}
  std::string actualv(v);
  m_map[pname] = new Preference<std::string>(pname, actualv);
}

template <>
void Preferences::create_preference<const char*>(const std::string& pname, const char*  v){
  std::string prefhint = get_typehint(pname);
  if(!prefhint.size()){registry[pname] = "string";}
  std::string actualv(v);
  m_map[pname] = new Preference<std::string>(pname, actualv);
}

template <>
void Preferences::create_preference<short>(const std::string& pname, short  v){
  std::string prefhint = get_typehint(pname);
  if(!prefhint.size()){registry[pname] = "int";}
  int actualv(v);
  m_map[pname] = new Preference<int>(pname, actualv);
}
template <>
void Preferences::create_preference<char>(const std::string& pname, char  v){
  std::string prefhint = get_typehint(pname);
  if(!prefhint.size()){registry[pname] = "int";}
  int actualv(v);
  m_map[pname] = new Preference<int>(pname, actualv);
}
/*
template <>
void Preferences::create_preference<size_t>(const std::string& pname, size_t  v){
  std::string prefhint = get_typehint(pname);
  if(!prefhint.size()){registry[pname] = "uint";}
  unsigned int actualv(v);
  m_map[pname] = new Preference<unsigned int>(pname, actualv);
}

template <>
void Preferences::create_preference<ptrdiff_t>(const std::string& pname, ptrdiff_t  v){
  std::string prefhint = get_typehint(pname);
  if(!prefhint.size()){registry[pname] = "int";}
  int actualv(v);
  m_map[pname] = new Preference<int>(pname, actualv);
}

*/
template <>
void Preferences::create_preference<unsigned short>(const std::string& pname, unsigned short  v){
  std::string prefhint = get_typehint(pname);
  if(!prefhint.size()){registry[pname] = "uint";}
  unsigned int actualv(v);
  m_map[pname] = new Preference<unsigned int>(pname, actualv);
}
template <>
void Preferences::create_preference<unsigned char>(const std::string& pname, unsigned char v){
  std::string prefhint = get_typehint(pname);
  if(!prefhint.size()){registry[pname] = "uint";}
  unsigned int actualv(v);
  m_map[pname] = new Preference<unsigned int>(pname, actualv);
}

    template <>
void Preferences::create_preference<unsigned long>(const std::string& pname, unsigned long v){//WARNING - data narrowed
  std::string prefhint = get_typehint(pname);
  if(!prefhint.size()){registry[pname] = "uint";}
  unsigned int actualv(v);
  m_map[pname] = new Preference<unsigned int>(pname, actualv);
}
template <>
void Preferences::create_preference<long>(const std::string& pname, long v){
  //WARNING - data narrowed
  std::string prefhint = get_typehint(pname);
  if(!prefhint.size()){registry[pname] = "int";}
  int actualv(v);
  m_map[pname] = new Preference<int>(pname, actualv);
}
template <>
void Preferences::create_preference<float>(const std::string& pname, float v){
  //WARNING - data narrowed
  std::string prefhint = get_typehint(pname);
  if(!prefhint.size()){registry[pname] = "double";}
  double actualv(v);
  m_map[pname] = new Preference<double>(pname, actualv);
}

std::ostream& NAMESPACE::operator<<(std::ostream& os, const Preferences& prefs){
  os <<"\nSnarky Preferences\n";
  typedef SnarkyMap<std::string, std::string>::const_iterator const_stringmap_iterator;
  for(Preferences::map_type::const_iterator w = prefs.m_map.begin(); w != prefs.m_map.end(); w++){
    std::string optionname =  (*w).first;
    os << (*w).first << " : ";
    const_stringmap_iterator place = prefs.registry.find((*w).first);
    if(place != prefs.registry.end()){
      std::string mtype = prefs.registry[(*w).first];
      if(mtype == "string"){
	std::string val;
	prefs.get_value(optionname, val);
	os << "'" <<val << "'";
      }
      else if (mtype == "uint"){
	unsigned int val;
	prefs.get_value(optionname, val);
	os << "'" << val<< "'";
      }
      else if(mtype == "ulong"){
	unsigned long val;
	prefs.get_value(optionname, val);
	os << "'" << val<< "'";
      }
      else if (mtype == "int" ){
	int val;
	prefs.get_value(optionname, val);
	os << "'" << val<< "'";
      }
      else if (mtype == "long"){
	long val;
	prefs.get_value(optionname, val);
	os << "'" << val<< "'";
      }

      else if (mtype == "float"){
	double val;
	prefs.get_value(optionname, val);
	os << "'" << val<< "'";
      }
      else if (mtype == "double"){
double val;
	prefs.get_value(optionname, val);
	os << "'" << val<< "'";
      }
     
      else {
	try{
	  int val;
	  prefs.get_value(optionname, val);
	  os << "[" << mtype << " - generic handler(assumes int)] '"<< val<<"'";
	}
	catch( const PreferenceError& e){
	  os << "\n" << e.what();
	}
      }
      os << std::endl;
    }
  }
  return os << std::endl;
}

std::deque<std::string> Preferences::keys()const{
      std::deque<std::string> result;
      for(const_iterator iter = m_map.begin();iter != m_map.end();iter++){
	result.push_back((*iter).first);
      }
      return result;
    }



//TODO also: Pictures, Documents, Movies, and any others that seem
//fairly portable and useful
std::string NAMESPACE::get_user_prefsdirpath(){
 
  std::ostringstream os_tmp;
  os_tmp << get_user_home() << PATH_SEP << "." << PACKAGE_NAME;
  return os_tmp.str();
}

std::string NAMESPACE::get_hometempdirpath(){
  std::string s =get_user_prefsdirpath() + PATH_SEP_PTR;
  s += "tmp";
  s += PATH_SEP_PTR;
  return s;
}

std::string NAMESPACE::get_sys_prefsdirpath(){
  // create a h.in file for make to put install prefix into and fix this - or just AC_DEFINE it into config.h
  std::string prefixshare = "/home/johno/share/snarky";
  return prefixshare;
}

Preferencebase* Preferencebase::selfcopy(){
      return new Preferencebase(name);
    }


void Preferences::set_value(const std::string& name, const char* value){ 
  std::string setting(value);
  if(have_pref(name)){
    set_value(name, setting);
  } 
  else{
    create_preference(name, setting);
  }
  if(!(registry[name]).size()){
    registry[name] = "string";
  } 
}

unsigned int Preferences::get_depth()const{
  unsigned int d;
  get_value(std::string("depth"), d);
  return d;
}

bool Preferences::have_pref(const std::string& s)const{
  for(const_iterator w = m_map.begin(); w!= m_map.end(); w++){
    if ((*w).first == s){
      return true;
    }
  }
  return false;
}

void Preferences::failed_get_exception(const std::string& prefrequest)const{
  std::string s = std::string("No such Preference: ") + prefrequest;
  throw PreferenceError(s);
}

const Preferencebase& Preferences::get_preference(const std::string& prefname)const{
  const_iterator iter = m_map.find(prefname);
  if (iter == m_map.end()){
    failed_get_exception(prefname);
  }
  return *(*iter).second;
}

Preferencebase& Preferences::get_preference(const std::string& prefname){
  const_iterator iter = m_map.find(prefname);
  if (iter == m_map.end()){
    failed_get_exception(prefname);
  }
  return *(*iter).second;
}

std::locale Preferences::getloc(){
  std::string loc;
  std::string option("locale");
  get_value(option, loc);
  try{
    return std::locale(loc.c_str());
  }
  catch(std::runtime_error& e){
    return std::locale();
  }
}

void Preferences::read_prefsfile(std::istream& fprefs){
  typedef std::pair<std::string::const_iterator, std::string::const_iterator> const_iterator_pair;
  //std::deque<std::string> prefslist;
  std::string s;
  std::string option;
  std::locale loc = getloc();
  while(std::getline(fprefs, s)){
    std::cout << s<< std::endl;
    const_iterator_pair stripped = snarky_strip_whitespace<char>(s.begin(), s.end(), loc);
    if (*(stripped.first) == '#' || stripped.first == stripped.second){ // comment or empty line
      continue;
    }
    std::string::const_iterator valuestart = stripped.first; //temporarily
    std::string::const_iterator w = stripped.first;
    while(! std::isspace(*w++, loc) ){ 
      if (w == stripped.second){
	s = "ran off end of string trying to read prefs option and value from the following line in prefs file :\n'" + s;
	s = s + "'";
	throw WordchopError(s);
      }
    }
    option = std::string(stripped.first, w);
    std::string typehint;
    if(registry.find(option) != registry.end()){
      typehint = registry[option];
    }
    while(std::isspace(*w++, loc) || *w == ':' || *w == '='){}
    s = std::string(w, stripped.second);
    set_valuefromstring(option, s, typehint);
  }
}
std::string Preferences::get_typehint(const std::string& key){
      typehint_const_iterator location = registry.find(key);
      if(location != registry.end()){
	return (*location).second;
      }
      std::string result("");
      return result;
    }

std::string Preferencebase::get_typeHint()const{
  return Preferences::get_typehint(name);
}

std::string NAMESPACE::get_prefsTypeHint(const std::string& key){
  return Preferences::get_typehint(key);
}
void NAMESPACE::set_prefsTypeHint(const std::string& name, const std::string& typehint){
  Preferences::set_typehint(name, typehint);
}

std::string Preferences::get_typeHint(const char* pname)const{
  std::string p(pname);
  return get_typeHint(p);
}
std::string Preferences::get_typeHint(const std::string& pname)const{
  return get_typehint(pname);//static function
}


//wouldn't hurt to validate
void Preferences::set_valuefromstring(const std::string& option, const std::string& val, std::string typehint){
  std::cout << "option : "<< option << ", val : "<< val << std::endl;
  typedef SnarkyMap<std::string, std::string>::const_iterator stringmap_const_iterator;
  stringmap_const_iterator place = registry.find(option);
  if(place != registry.end()){
    if(typehint != registry[option]){
      std::cerr<< "typehint '" <<typehint << "' not the same as in registry(" << registry[option]<< "\nGoing with registry " << std::endl;
      typehint =  registry[option];
    }
  }
  errno = 0;
  if(typehint == "uint"){
    unsigned int newval(strtoul(val.c_str(), NULL, 10));
    if(errno){std::cerr << strerror(errno) << std::endl;}
    else{    set_value(option, newval);}
  }
  else if (typehint == "string"){
    set_value(option, val);
  }
  else if(typehint == "float"){
float newval = strtod(val.c_str(), NULL);
    if(errno){std::cerr << strerror(errno) << std::endl;}
    else{ set_value(option, newval);}
  }
else if (typehint == "double"){
    double newval = strtod(val.c_str(), NULL);
    if(errno){std::cerr << strerror(errno) << std::endl;}
    else{ set_value(option, newval);}
  }
  else if (typehint == "int" || !typehint.size()){
    int newval = strtol(val.c_str(), NULL, 10);
    if(errno){std::cerr << strerror(errno) << std::endl;}
    else{ set_value(option, newval);}
  }
  else{ 
    std::string s("No handler for '");
    s += typehint + "'";
    throw WordchopError(s);
  }
  if(typehint.size() && place == registry.end()){
    Preferences::set_typehint(option, typehint);
  }
}

void Preferences::set_typehint(const std::string& name, const std::string& typehint){
      registry[name] = typehint;
    }
