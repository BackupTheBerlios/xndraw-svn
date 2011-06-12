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



#ifndef LIBSNARKOV_EXCEPTIONS_H
#define LIBSNARKOV_EXCEPTIONS_H

#ifdef HAVE_CONFIG_H
#include "appcommonconfig.h"
#endif

#ifndef NAMESPACE_APPCOMMON
#define NAMESPACE_APPCOMMON namespace APPCOMMON
#define NAMESPACE APPCOMMON
#endif

#include <cerrno>
#include <exception>
#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>
#define EXCEPTIONS_NAMESPACE project_namespace()

#define SNARKY_DEFINEERROR(Errortype, Parenttype) class Errortype: \
  public Parenttype{					     \
  public:                                                    \
    Errortype(const Errortype& src):                         \
      Parenttype(src) { }                                   \
                                                             \
      Errortype(const char* e):                              \
	Parenttype(e) { }                                   \
							     \
	virtual void rethrow(){				     \
	  throw *this;					     \
	}							     \
								     \
	std::string  project_namespace()const{		     \
	  return std::string("NAMESPACE");			     \
}							     \
							     \
	virtual std::string errorname()const{		     \
	  std::string ename = project_namespace() + "::";    \
	  return ename + std::string( #Errortype );		     \
	}						     \
                                                             \
	Errortype(const std::string& e):                     \
	  Parenttype(e) { }                                 \
                                                             \
	  virtual ~Errortype()throw(){}                      \
  }



NAMESPACE_APPCOMMON{

class ErrorBase:public std::exception{
  ErrorBase& operator=(const ErrorBase&); //don't need this
protected:
    std::string errr;
    int syserr;
public:

  std::string project_namespace()const{
    return std::string("NAMESPACE");
  }

  
  ErrorBase(const ErrorBase& src)throw():std::exception(src), errr(src.errr), syserr(src.syserr){} 

  virtual ~ErrorBase() throw (){ }
  
  ErrorBase(const char* err) throw():errr(err), syserr(errno) { }
  ErrorBase(const std::string& err) throw():errr(err), syserr(errno) { }

// generally don't override this, but instead what_string() below,
//unless maybe you want to use static strings
 const char* what()const throw(){
    // free() the string returned
   if(errno & ENOMEM){
     return "out of memory";
   }
    std::string estring = what_string();
    const char * ret =strdup(estring.c_str());
    if(!ret ){
      std::cerr<< "out of memory => attempting to return error.what():" <<estring<< std::endl;}
    return ret;
  }
//override
  virtual std::string what_string()const throw(){
    if(errno == ENOMEM){return "no memory";}/*wish for a few bytes on heap!*/
std::string estring= errorname() + ": " + errr;
    if(syserr){
      estring = estring + "\nSystem error was set when exception thrown:\n'";
      estring = estring + strerror(syserr);
      estring += "'";
    } 
    return estring;
  }

  //override
  virtual void notify_user()const{
    std::cerr << this->errorname() << " @ " << this << "\n"
	      << this->what_string()<< std::endl;
}

  virtual std::string errorname()const{
    std::string ename = project_namespace() + "::";
    return ename + "ErrorBase";		     
  }

};

  class FileexistsError: public ErrorBase{
    
    FileexistsError& operator=(const FileexistsError& src);
 
  protected:
    std::string efile;
    
  public:
    FileexistsError(const FileexistsError& src)throw():ErrorBase(src), efile(src.efile){}
    virtual ~FileexistsError()throw(){}

    FileexistsError(const char* e) throw():ErrorBase(e), efile(e){ }
    FileexistsError(const std::string& e) throw():ErrorBase(e),efile(e){ }

    virtual  std::string what_string()const throw(){
      // caller use 'delete' to clear return val
      std::string tmp= errorname() +": "+ efile+" already exists";
      return tmp;
    }

virtual std::string errorname()const{	
std::string ename = project_namespace() + "::";	     
	  return ename + std::string("FileexistsError");		     
	}
  };

  class WordchopFatalException: public ErrorBase{
    int syserr;
  public:
    /**
     * WordchopFatalException.
     * not derived from WordchopError so we don't catch and 
     * 'handle' the error ,erroneously, 
     * as things are far too serious to recover from

     * TODO: common base class above exception(but not to catch on, 
     * rather to hold common members like syserr, msg, what, errorname
     */
    WordchopFatalException()throw():ErrorBase(""), syserr(errno){}
    virtual ~WordchopFatalException()throw(){}
    WordchopFatalException(const char* msg)throw():ErrorBase(msg), syserr(errno){}
    WordchopFatalException(const std::string& msg)throw():ErrorBase(msg), syserr(errno){}
    WordchopFatalException(const WordchopFatalException& src)throw():
      ErrorBase(src), 
      syserr(errno)
      {}
    virtual std::string errorname()const{
      std::string enamespace = project_namespace();		     
      return enamespace + "WordchopFatalException";		     
    }

   virtual  std::string what_string()throw(){
      std::string tmpwhat = errorname() +": --> an error can't be handled has arisen\n";
      tmpwhat += errr;
      if(syserr){
      tmpwhat = tmpwhat + "\nSystem error was set when exception thrown:\n'";
      tmpwhat = tmpwhat + strerror(syserr);
      tmpwhat += "'";
    } 
      return tmpwhat;
}
  };

  typedef WordchopFatalException WordchopFatalError;

  class WordchopError: public ErrorBase{
    
    WordchopError& operator=(const WordchopError&); //not implemented
    
  public:
    WordchopError(const WordchopError& src)throw():ErrorBase(src), syserr(src.syserr){} 
      virtual ~WordchopError() throw (){ }
    WordchopError(const char* err) throw():ErrorBase(err), syserr(errno) { }

    WordchopError(const std::string& e_) throw() :ErrorBase(e_), syserr(errno) { }

    virtual void rethrow(){throw *this;}

virtual std::string errorname()const{
  std::string enamespasce = EXCEPTIONS_NAMESPACE;
	  return enamespasce + std::string("WordchopError");		     
	}

  protected:
    std::string errr;
    int syserr;

  
  };
  class MemoryError : public WordchopError,
		      public std::runtime_error{
  public:
    virtual ~MemoryError ()throw(){}
    MemoryError(const MemoryError& src):WordchopError(src),
					std::runtime_error(src){}
    MemoryError(const std::string& s):WordchopError(s),
				      std::runtime_error(s){}
    
    virtual void rethrow(){throw *this;}
    virtual std::string errorname(){		     
      return std::string("MemoryError");		     
    }
  };
  class SystemError : public WordchopError,
		      public std::runtime_error{
  public:
    virtual ~SystemError ()throw(){}
    SystemError(const SystemError& src):WordchopError(src),
					std::runtime_error(src){}
    SystemError(const std::string& s):WordchopError(s),
				      std::runtime_error(s){}
    
    virtual void rethrow(){throw *this;}
    virtual std::string errorname(){		     
      return std::string("SystemError");		     
    }
  };
  ///IF EXCEPTIONTYPE needs no extra data, define like so
		      ///catch on derived error, in order not to
		      /// catch other error by mistake
  SNARKY_DEFINEERROR(ReaderError, WordchopError);
  SNARKY_DEFINEERROR(WriterError, WordchopError);
SNARKY_DEFINEERROR(DefaultError, WordchopError);
 typedef DefaultError Error;
  SNARKY_DEFINEERROR(NoMemoryError, MemoryError);
  SNARKY_DEFINEERROR(BadaccessError, MemoryError);
  SNARKY_DEFINEERROR(SequenceError, WordchopError);
 SNARKY_DEFINEERROR(ContainerError, WordchopError);
 SNARKY_DEFINEERROR(BadAllocError, ContainerError);
  SNARKY_DEFINEERROR(StringtypeError, WordchopError);
  SNARKY_DEFINEERROR(PathError, WordchopError);
  SNARKY_DEFINEERROR(WheelError, WordchopError);
  SNARKY_DEFINEERROR(RankError, WordchopError);
  SNARKY_DEFINEERROR(EOFError, ReaderError);
  SNARKY_DEFINEERROR(PreferenceError, WordchopError);
  SNARKY_DEFINEERROR(WriterEncodingError, WriterError);// ;)~
  SNARKY_DEFINEERROR(DocumentError, WordchopError);
  SNARKY_DEFINEERROR(HtmlTagQuoteError, DocumentError);
  
  //EILSEQ : errno for incomplete or invalid byte sequence from mbsrtowcs
  // (called by basic_ifstream<wchar_t>::get etc)
  
  class ReaderEncodingError: 
    public ReaderError{
    
  protected:
    std::string enc;
    
  public:                                                    
    ReaderEncodingError(const ReaderEncodingError& src):   
      ReaderError(src) , enc(src.enc){ }                                   
    
    ReaderEncodingError(const char* e, const char* encoding):  
      ReaderError(e), enc(encoding){ }                                   
    
    ReaderEncodingError(const std::string& e, const std::string& encoding):
      ReaderError(e) ,enc(encoding){ }                                 
    
    virtual ~ReaderEncodingError()throw(){}   
    virtual void rethrow(){throw *this;}
    
    const std::string& get_encoding()const{
      return enc;
    }
    
    virtual std::string errorname()const{		     
      return std::string("ReaderEncodingError");		     
    }
  };


  //terminate handler - 
  void do_terminate();

}/* NAMESPACE_APPCOMMON */

#endif /*LIBSNARKOV_EXCEPTIONS_H*/
