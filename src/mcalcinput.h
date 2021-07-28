#ifndef __MCALCINPUT_H__
#define __MCALCINPUT_H__

#include <list>
#include <unordered_map>
#include "mcalcmatrix.h"

extern unordered_map< string, shared_ptr< COperand > > variables;

list< shared_ptr< CObject > > lexer( const string & strFromInput );

bool doubleIsUnsigned( double a );

int isOperation( const string & a );

#endif
