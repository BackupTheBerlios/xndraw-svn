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

#include <vector>
#include "boundinstruction.h"

using namespace xNDRAW;

int main(int args, char** argv){
std::vector<boundMethod<TestA, int> > contnr;
TestA* testa = new TestA;
TestA* testb = new TestB;
TestA* testc = new TestC;
contnr.push_back(xNDRAW::bind_method(testa, & TestA::test, 5));
contnr.push_back(xNDRAW::bind_method(testb, & TestA::test, 5));
contnr.push_back(xNDRAW::bind_method(testc, & TestA::test, 5));


typedef std::vector<boundMethod<TestA, int> >::iterator iterator;
 for(iterator start = contnr.begin(); start != contnr.end(); start++){
(*start)();
}
return 0;
}
