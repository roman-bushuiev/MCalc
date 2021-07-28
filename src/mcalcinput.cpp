#include <iostream>
#include <list>
#include <stack>
#include "mcalcinput.h"
#include "mcalcmatrix.h"

using namespace std;

unordered_map< string, shared_ptr< COperand > > variables;

// identify char type
int charType( const char a )
{
  if( ( a >= '0' && a <= '9' ) || a == '.' )
    return 'D';  // Digit or dot
  else if( ( a >= 'a' && a <= 'z' ) || ( a >= 'A' && a <= 'Z' ) )
    return 'L'; // Letter
  else if( a == '+' || a == '*' || a == '/' || a == '=' || a == '^' || a == '-' )
    return 'S'; // Symbol
  else if( a == '[' || a == ']' )
    return 'B'; // Square brackets
  else if( a == '(' )
    return '(';
  else if( a == ')' )
    return ')';
  else if( a == '{' )
    return '{';
  else if( a== ',' )
    return ',';
  else
    return 'O';
}

string deleteWhiteSpaces( const string & a )
{
  string b;
  for( auto it = a.begin(); it != a.end(); it++ )
  {
    if( *it != ' ' && *it != '\t' && *it != '\r' )
      b += *it;
  }
  return b;
}

// return number of arguments for the operation
// return -1, when inputed word is not an operation
int isOperation( const string & a )
{
  if ( a == "transpose" ||
       a == "determinant" ||
       a == "inverse" ||
       a == "rank" ||
       a == "ge" ||
       a == "print" ||
       a == "minors" )
    return 1;
  else if( a == "merge" )
    return 3;
  else if( a == "split" )
    return 5;
  else
    return -1;
}

// return priority of the operation
// return -1, when inputed sequence of symbols is not an operator
int operatorPriority( const string & a )
{
  if( isOperation( a ) > 0 || a == "-u" ) // -u is unary minus
    return 6;
  else if( a == "^" )
    return 5;
  else if( a == "*" || a == "/" )
    return 4;
  else if( a == "+" || a == "-" || a == "-b" ) // -b is binary minus
    return 3;
  else if( a == "(" || a ==")" )
    return 2;
  else if( a == "=" || a == "==" )
    return 1;
  else
    return -1;
}

// test if the string is valid double number
bool testDouble( const string & a )
{
  auto it = a.begin();
  bool dot = 0;

  while( it != a.end() && ( charType( *it ) == 'D' ) )
  {
    if( *it == '.' )
    {
      if( !dot )
        dot = 1;
      else
        return false;
    }
    it++;
  }

  return it == a.end() && !a.empty() && *a.begin() != '.';
}

bool doubleIsUnsigned( double a )
{
  return a >= 0 && (float)( (int)a ) == a;
}

bool testVariable( const string & a )
{
  return !( isOperation( a ) > 0 || ( a.at(0) >= '0' && a.at(0) <= '9' ) );
}

// get the value, that is going to be pushed to the new matrix
double newMatrixValue( string::const_iterator & it )
{
  string tmpVariable;
  int minus = 1;

  if( *it == '-' )
  {
    minus = -1;
    it++;
  }

  // parse string to number or to variable

  if( charType( *it ) == 'D' )
  {
    while( charType( *it ) == 'D' )
    {
      tmpVariable += *it;
      it++;
    }

    if( !testDouble( tmpVariable ) )
      throw string( "Invalid numeric expression <" + tmpVariable + ">." );
    return minus * stod( tmpVariable );

  }else if( charType( *it ) == 'L' )
  {
    while( charType( *it ) == 'L' || charType( *it ) == 'D' )
    {
      tmpVariable += *it;
      it++;
    }

    // test is the variable is valid
    if( variables.find( tmpVariable ) == variables.end() )
      throw string( "Invalid variable <" + tmpVariable + ">." );

    return minus * variables.find( tmpVariable )->second->getValue();
  }else
  {
    throw string( "Invalid syntax." );
  }
}

void createMatrix( const string & a , const string & name )
{
  auto it = a.begin();
  unsigned columnPrev = 0;
  unsigned columnsCounter = 0;
  unsigned rowsCounter = 0;
  unsigned zerosCounter = 0;
  double valueToPush;

  shared_ptr<CMatrixDense> newMatrix  = make_shared< CMatrixDense >( CMatrixDense ( name ) );

  // test the string for the format {[.,...],...,[...,.]} and save the new matrix

  while( 1 )
  {
    if( *it != '[' )
      throw string( "Invalid syntax." );
    it++;
    while( 1 )
    {
      valueToPush = newMatrixValue( it );
      if( valueToPush == 0 )
        zerosCounter++;

      newMatrix->pushValue( valueToPush, rowsCounter );
      columnsCounter++;

      if( *it == ',' )
        it++;
      else if( *it == ']' )
      {
        it++;
        break;
      }
      else
        throw string( "Invalid syntax." );
    }

    if( columnPrev != 0 && columnsCounter != columnPrev )
      throw string( "Invalid matrix size." );
    columnPrev = columnsCounter;

    if( *it == ',' )
    {
      columnsCounter = 0;
      rowsCounter++;
      it++;
    }
    else if( *it == '}' && it == a.end() - 1 )
      break;
    else
      throw string( "Invalid syntax." );
  }
  rowsCounter++;

  newMatrix->updateRows( rowsCounter );
  newMatrix->updateColumns( columnsCounter );
  newMatrix->updateZeros( zerosCounter );

  // recreate element if it has the same name
  if( variables.find( name ) != variables.end() )
  {
    variables.erase( name );
  }

  // remake matrix from dense to sparse
  if( zerosCounter > rowsCounter * columnsCounter / 2 )
  {
    //newMatrix->createSparseMatrix().show();
    variables.insert({ name, make_shared< CMatrixSparse >( newMatrix->createSparseMatrix() ) } );
  }else
  {
    //newMatrix->show();
    variables.insert( { name, newMatrix } );
  }
}

// main function to parce the string, test if it is valid and
// create the list of arguments in reverse polish notation

list< shared_ptr< CObject > > lexer( const string & strFromInput )
{
  string str = deleteWhiteSpaces( strFromInput );

  list< shared_ptr< CObject > > objectsList;
  stack< COperation > operationsStack;

  string currSubstring;
  string testComas;
  unsigned comasCounter = 0;
  char prevType = '_';
  int exprBegin = 0;
  bool newMatrix = 0;

  for( auto it = str.begin(); it != str.end() + 1; it++ )
  {
    if( charType( *it ) == 'O' && it != str.end() )
      throw string( "Incorrect character: <"  + string( 1, *it ) + ">." );

    // New object
    if( prevType != '_' &&
        ( charType( *it ) != prevType || *it == '(' || *it == ')' || *it == '-' || *it == ',' ) )
    {
      currSubstring = str.substr( exprBegin, it - str.begin() - exprBegin );

      if( prevType == 'L' && isOperation( currSubstring ) < 0 )
      {
        if( variables.find( currSubstring ) != variables.end() )
          objectsList.push_back( make_shared< CVariable >( CVariable( variables.find( currSubstring )->first ) ) );
        else
          objectsList.push_back( make_shared< CVariable >( CVariable( currSubstring ) ) );
      }else // Operand ( numeric )
      if( prevType == 'D' )
      {
        if( !testDouble( currSubstring ) )
          throw string( "Invalid numeric value <" + currSubstring + ">." );

        objectsList.push_back( make_shared< CScalar >( CScalar( "", stod( currSubstring ) ) ) );
      }else // Bracket ( left )
      if( prevType == '(' )
      {
        operationsStack.push( COperation ( currSubstring ) );
      }else // Bracket ( right )
      if( prevType == ')' )
      {
        if( !testComas.empty() && (int)comasCounter != isOperation( testComas ) - 1 )
        {
          throw string( "Invalid number of arguments." );
        }else
        {
          testComas.clear();
          comasCounter = 0;
        }

        for(; !operationsStack.empty() && operationsStack.top().getName() != "("; operationsStack.pop() )
        {
          objectsList.push_back( make_shared< COperation >( operationsStack.top() ) );
        }

        if( operationsStack.empty() )
        {
          throw string( "Invalid number of brackets." );
          break;
        }
        operationsStack.pop();
      }
      else // Operator ( unary function )
      if( prevType == 'L'  && isOperation( currSubstring ) > 0 )
      {
        if( *it != '(' )
          throw string( "No brackets after call of a function." );

        testComas = currSubstring;

        operationsStack.push( COperation( currSubstring ) );
      }else // Operator
      if( prevType == 'S' && operatorPriority( currSubstring ) > 0 )
      {
        if( currSubstring == "="  )
        {
          if( objectsList.size() != 1 ||
              !operationsStack.empty() ||
              !testVariable( objectsList.front()->getName() ) )
            throw string( "Input before assigment is invalid lvalue." );
        }

        if( currSubstring == "-" )
        {
          if( objectsList.empty() ||
              *( it - 2 ) == '-' ||
              ( operatorPriority( string( 1, *( it - 2 ) ) ) > 0 && *( it - 2 ) != ')' ) ||
              *( it - 1 ) == '(' )
          {
            currSubstring = "-u";
          }else
          {
            currSubstring = "-b";
          }
        }

        if( operationsStack.empty() ||
        operatorPriority( currSubstring ) >= operatorPriority( operationsStack.top().getName() ) )
        {
          operationsStack.push( COperation( currSubstring ) );
        }
        else
        {
          while( operatorPriority( currSubstring ) < operatorPriority( operationsStack.top().getName() ) )
          {
            objectsList.push_back( make_shared< COperation >( operationsStack.top() ) );
            operationsStack.pop();
            if( operationsStack.empty() )
              break;
          }
          operationsStack.push( COperation( currSubstring ) );
        }
      }else // New matrix
      if( prevType == '{' )
      {
        if( !operationsStack.empty() && operationsStack.top().getName() == "=" )
        {
          createMatrix( str.substr( exprBegin + 1 ), objectsList.front()->getName() );
          newMatrix = 1;
        }
        else
        {
          cout << string( "Invalid syntax." ) << endl;
        }
        break;
      }else
      if ( prevType == ',' )
      {
        comasCounter++;
      }else
      {
        throw string( "Invalid operator <" + currSubstring + ">." );
      }

      exprBegin = it - str.begin();
    }
    prevType = charType( *it );
  }

  for(; !operationsStack.empty(); operationsStack.pop() )
  {
    if( operationsStack.top().getName() == ")" ||
        operationsStack.top().getName() == "(" )
    {
      throw string( "Invalid number of brackets." );
      break;
    }
    objectsList.push_back( make_shared< COperation >( operationsStack.top() ) );
  }

  if( newMatrix )
    objectsList.clear();

  return objectsList;
}
