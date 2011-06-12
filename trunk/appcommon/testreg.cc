
#define COMPILING_STUPIDAPP 1

#include <regex.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "exceptions.h"

using NAMESPACE_APPCOMMON;

#define ERRBUFSZ 1024

void print_regerror(int err, regex_t* reg ,char* errstr, int errstr_size){
int errresult = regerror(err, reg, errstr, errstr_size);
    std::cerr << errstr;
}
/*
std::ostream& operator<<(std::ostream& os, const std::pair<std::string, regmatch_t>& regpair){
  int sz = regpair.first.size();
  const regmatch_t&match = regpair.second;  
  int matchbegin = match.rm_so;
  if(matchbegin == -1){os << "no match";}
  else{
    int matchend = match.rm_eo;
    std::string::const_iterator start = regpair.first.begin() + matchbegin;
    std::string::const_iterator stop = regpair.first.begin() + matchend;
    os << "Match : '" << std::string(start, stop) << "\n["<< matchbegin << ", " << matchend<< "] ";
  }
  os <<"in ' " << regpair.first << "'"<<std::endl;
}
*/
std::ostream& operator<<(std::ostream& os, const std::pair<const std::string*, const regmatch_t*>& regpair){
  int sz = regpair.first->size();
  const regmatch_t& match = *(regpair.second);  
  int matchbegin = match.rm_so;
  if(matchbegin == -1){os << "no match ";}
  else{
    int matchend = match.rm_eo;
    std::string::const_iterator start = regpair.first->begin() + matchbegin;
    std::string::const_iterator stop = regpair.first->begin() + matchend;
    os << "Match : '" << std::string(start, stop) << "\n["<< matchbegin << ", " << matchend<< "] ";
  }
  os <<"in ' " << *(regpair.first) << "'"<<std::endl;
}

void print_regmatch(const std::string& stringmatched, const regmatch_t& match){
  const std::pair<const std::string*, const regmatch_t*> mpair(&stringmatched, &match);
  std::cout <<mpair;
  }

int compile_reg(regex_t* reg, char* s_pattern){
  if(!reg){throw WordchopError("compile_reg:NULL regex_t* passed");}
  if(!s_pattern){throw WordchopError("compile_reg:NULL patternstring passed");}
  int compresult = regcomp(reg, s_pattern, REG_EXTENDED);
  if(compresult){
    char buf[ERRBUFSZ];
    print_regerror(compresult, reg , buf, ERRBUFSZ);
    return compresult;
  }
  return 0;
}

std::string create_regIP(){
char protodelim[] = "(:([/]{2}))";
 char protochars[]= "((([a-zA-Z])|([:alpha:])){1}(([a-zA-Z0-9\\+])|([:alnum:]))*)";
 char port[]= "([:][0123456789]{1,5})";
  char path[]= "((/[a-zA-Z0-9%_\\+-]*)+)";
 char wwwserver[]= "(([a-zA-Z0-9]*)([\\.]([a-zA-Z0-9]*))*)";
 std::ostringstream streamproto;
 streamproto << protochars <<protodelim;
 std::string s_proto = streamproto.str();
 char twofivefive[] = "(([01]?[0123456789]{1,2})|([2][01234][0123456789])|(25[012345]))";
// character class [:space:] not working on linux?
  char start[]= "((^)|([:space:])|([ \t])|(/))";//these characters not actually
  char end[] = "(($)|([:space:])|([ \t])|(:)|(/))";
//not partt of a match, but the characters that should be around it -
//perhaps a 2pass approach - find those strings which do contain an address
// then extract the actual address(ie ignore the 
//"characters aeround the address" the second time.
//or determine whether eg the address has or hasn't protocol and port and filepath etc elements
 std::ostringstream ostmp;
 std::ostringstream ostmp2;
 ostmp2 << "(" <<start<< "("<< s_proto.c_str()<<"{0,1}(("
//err um likes to match against a single 'word'
// which coulg be 'localhost' be just as easily 'frog'
// whatever is first in the line...
	<<twofivefive<<"{1}([\\.]){1}"<<twofivefive<<"{1}([\\.]){1}"
       <<twofivefive<<"{1}([\\.]){1}"<<twofivefive<<"{1})|"
	<< wwwserver <<"){1}" << port <<"{0,1})|"<<path<<"{1}" <<end <<")";
 ostmp << "(" <<start <<"{1}"
       <<twofivefive<<"{1}([\\.]){1}"<<twofivefive<<"{1}([\\.]){1}"
       <<twofivefive<<"{1}([\\.]){1}"<<twofivefive<<"{1}"<< end<< "{1})";
 std::cout<< "pattern string: '"<<ostmp2.str()<<"'";
  return ostmp.str();
}

int main(int argc, char** argv){
char* pattern= NULL;
  char errbuf[ERRBUFSZ];
  char* errstr = errbuf;
  std::string s_fname("regexercise.txt");
  regex_t reg;//misnomer
  regex_t* p_reg = &reg;
  bool delete_pattern=false;
  if(argc >1){
  s_fname = argv[1];
  }
  if(argc > 2){
  pattern = argv[2];
  }
  if(!pattern){
    std::string numericIP = create_regIP();
    pattern= new char[numericIP.size() +1];
    strcpy(pattern, numericIP.c_str());
    delete_pattern=true;
  }
  int reg_OK = compile_reg(p_reg, pattern);
  std::string s;
  char* line =NULL;
    regmatch_t* regmatches = NULL;
  try{
    std::ifstream ifs(s_fname.c_str());
    ifs.exceptions(std::ios_base::badbit | std::ios_base::failbit);
    int linecount(0);
    while(std::getline(ifs,s)){
      linecount++;
      if(s.size()){
	line = new char[s.size() +1];
	strcpy(line, s.c_str());
	std::cout << "\n*****************\nline:"<<linecount<<" '"<< line << "'"<<std::endl;
	int nmatch = 8;
	regmatch_t* regmatches =new regmatch_t[nmatch];
	int execresult = regexec(p_reg, line, nmatch, regmatches, 0);
	if(execresult){
	  print_regerror(execresult, p_reg, errstr, ERRBUFSZ);
	}
	else{
	  int count = 0;
	  regmatch_t* regcounter = regmatches;
	  for(int i =0 ; i< nmatch; i++){
	    print_regmatch(s, regmatches[i]);
	    if(i ==0){
	      std::cout << "matching subexpressions..."<< std::endl;
	    }
	  } 
	}
	delete line;
	line = NULL;
	delete regmatches;
	regmatches =NULL;
      }
    }
  }
  catch(std::ios_base::failure& iofail){
    std::cerr << iofail.what();
    if(line) {delete[] line;}
    if(regmatches) {delete[]regmatches;}
  }
  if(delete_pattern){
    delete[] pattern;
  }
  regfree(p_reg);
  //delete[] regmatches;
  return 0;
}
