#include <iostream>
#include "mcalcinput.h"
#include "mcalcmatrix.h"
#include "mcalccompute.h"

using namespace std;

int main()
{
  string input;
  while ( 1 )
  {
    cout << "--Input-->:" << endl;

    // stop program when EOF is in input
    if ( !getline( cin, input ) )
      break;

    try
    {
      list< shared_ptr< CObject > > objectsList = lexer( input );
      compute( objectsList );
    }catch ( const string & message )
    {
      cout << "<--Output--:" << endl;
      cout << message << endl;
    }
  }

  return 0;
}
