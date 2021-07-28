#include <iostream>
#include <list>
#include <stack>
#include <memory>
#include "mcalccompute.h"
#include "mcalcmatrix.h"
#include "mcalcinput.h"

using namespace std;

void compute( list< shared_ptr< CObject > > & objectsList )
{
  stack< shared_ptr< COperand > > operandsStack;

  // parcing reverse polish list
  for( auto it = objectsList.begin(); it != objectsList.end() ; it++ )
  {
    if( it == objectsList.begin() && (*prev(objectsList.end()))->getName() == "=" )
    {
      operandsStack.push( make_shared< CScalar >( CScalar( (*it)->getName(), 0 ) ) );
    }
    else
    {
      (*it)->compute( operandsStack );
    }
  }

  // stack is empty if assigment operation was in input
  // if something except output value is still on the stack it means that
  // input was incorrect
  if( operandsStack.size() == 0 )
  {
    ;
  }else
  if( operandsStack.size() != 1 )
  {
    throw string( "Invalid syntax." );
  }
  else
  {
    cout << "<--Output--:" << endl;
    operandsStack.top()->show();
  }

}
