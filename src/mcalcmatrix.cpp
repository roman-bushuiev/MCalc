#include <iostream>
#include <iomanip>
#include <string>
#include <stack>
#include <vector>
#include <memory>
#include <cmath>
#include "mcalcmatrix.h"
#include "mcalcinput.h"

using namespace std;

#define PRECISION 7

/*------------------------CObject------------------------*/

CObject::CObject( const string & name ) : name( name ) {}

string CObject::getName() const
{
  return name;
}

void CObject::updateName( const string & newName )
{
  name = newName;
}

/*------------------------COperation------------------------*/

COperation::COperation( const string & name ) : CObject( name ) {}

// get the operation and take required amout of operands from stack
void COperation::compute( stack< shared_ptr< COperand > > & operandsStack )
{
  vector< shared_ptr< COperand > > operandsVector;
  int operandsNumber = isOperation( name );

  if( name == "-u" ) // unary minus
    operandsNumber = 1;
  if( operandsNumber < 0 ) // is not a function
    operandsNumber = 2;

  operandsVector.reserve( operandsNumber );

  // not enough operands
  for( int operand = 0; operand < operandsNumber; operandsStack.pop() )
  {
    if( operandsStack.empty() )
      throw string( "Invalid syntax." );

    operandsVector.push_back( operandsStack.top() );
    operand++;
  }

  if( name == "+" )
  {
    operandsStack.push( *( operandsVector.end()[-2] ) +
                        *( operandsVector.end()[-1] ) );
  }else if( name == "-b" )
  {
    operandsStack.push( *( operandsVector.end()[-1] ) -
                        *( operandsVector.end()[-2] ) );
  }
  else if( name == "-u" )
  {
    operandsStack.push( ( operandsVector.end()[-1] )->unaryMinus() );
  }else if( name == "*" )
  {
    operandsStack.push( *( operandsVector.end()[-2] ) *
                        *( operandsVector.end()[-1] ) );
  }else if( name == "/" )
  {
    operandsStack.push( *( operandsVector.end()[-2] ) /
                        *( operandsVector.end()[-1] ) );
  }else if( name == "=" )
  {
    *( operandsVector.end()[-2] ) = *( operandsVector.end()[-1] );
  }else if( name == "==" )
  {
    operandsStack.push( *( operandsVector.end()[-2] ) ==
                        *( operandsVector.end()[-1] ) );
  }else if( name == "^" )
  {
    if( ( operandsVector.end()[-2] )->getValue() == -1 )
      operandsStack.push( ( operandsVector.end()[-1] )->inverse() );
    else if( doubleIsUnsigned( ( operandsVector.end()[-2] )->getValue() ) )
      operandsStack.push( *( operandsVector.end()[-1] ) ^
                           ( operandsVector.end()[-2] )->getValue() );
    else
      throw string( "Invalid arguments for operator \"^\"." );
  }else if( name == "transpose" )
  {
    operandsStack.push( ( operandsVector.end()[-1] )->transpose() );
  }else if( name == "split" )
  {
    if( !doubleIsUnsigned( ( operandsVector.end()[-2] )->getValue() ) ||
        !doubleIsUnsigned( ( operandsVector.end()[-3] )->getValue() ) ||
        !doubleIsUnsigned( ( operandsVector.end()[-4] )->getValue() ) ||
        !doubleIsUnsigned( ( operandsVector.end()[-5] )->getValue() ) )
    {
      throw string( "Invalid arguments for operation \"split\"." );
    }

    operandsStack.push( ( operandsVector.end()[-1] )->split(
                        ( operandsVector.end()[-2] )->getValue(),
                        ( operandsVector.end()[-3] )->getValue(),
                        ( operandsVector.end()[-4] )->getValue(),
                        ( operandsVector.end()[-5] )->getValue() ) );
    }else if( name == "merge" )
    {
      operandsStack.push( ( operandsVector.end()[-1] )->merge(
                          *( operandsVector.end()[-2] ),
                          ( operandsVector.end()[-3] )->getValue() ) );
    }else if( name == "determinant" )
    {
      operandsStack.push( ( operandsVector.end()[-1] )->determinant() );
    }else if( name == "ge" )
    {
      operandsStack.push( ( operandsVector.end()[-1] )->ge() );
    }else if( name == "rank" )
    {
      operandsStack.push( ( operandsVector.end()[-1] )->rank() );
    }else if( name == "minors" )
    {
      operandsStack.push( ( operandsVector.end()[-1] )->minors() );
    }else // if( name == "inverse" )
    {
      operandsStack.push( ( operandsVector.end()[-1] )->inverse() );
    }

}

/*------------------------COperand------------------------*/

COperand::COperand( const string & name ) : CObject( name ) {}

/*------------------------CVariable------------------------*/

CVariable::CVariable( const string & name ) : CObject( name ) {}
void CVariable::compute( stack< shared_ptr< COperand > > & operandsStack )
{
  if( variables.find( name ) != variables.end() )
  {
    operandsStack.push( variables.find( name )->second );
  }else
  {
    throw string( "Invalid variable: <" + name + ">" );
  }
}

/*------------------------CScalar------------------------*/

CScalar::CScalar( const string & name, double value ) : COperand( name ), value( value ) {}
void CScalar::show() const
{
  cout << value << endl;
}

void CScalar::compute( stack< shared_ptr< COperand > > & operandsStack )
{
  operandsStack.push( make_shared< CScalar >( *this ) );
}

double CScalar::getValue() const
{
  return value;
}

shared_ptr< COperand > CScalar::operator+ ( const COperand & other ) const
{
  return other.addWith( *this );
}

shared_ptr< CScalar > CScalar::addWith( const CScalar & other ) const
{
  return make_shared< CScalar >( CScalar( "", value + other.getValue() ) );
}

shared_ptr< COperand > CScalar::operator- ( const COperand & other ) const
{
  return other.unaryMinus()->addWith( *this );
}

shared_ptr< COperand > CScalar::unaryMinus() const
{
  return make_shared< CScalar >( CScalar( "", -value ) );
}

shared_ptr< COperand > CScalar::operator/ ( const COperand & other ) const
{
  return other.divideWith( *this );
}

shared_ptr< CScalar > CScalar::divideWith( const CScalar & other ) const
{
  return make_shared< CScalar >( CScalar( "", value / other.getValue() ) );
}

shared_ptr< COperand > CScalar::operator* ( const COperand & other ) const
{
  return other.multiplyWith( *this );
}

shared_ptr< CScalar > CScalar::multiplyWith( const CScalar & other ) const
{
  return make_shared< CScalar >( CScalar( "", value * other.getValue() ) );
}

shared_ptr< CMatrix > CScalar::multiplyWith( const CMatrixDense & other ) const
{

  if( value == 0 )
  {
    CMatrixSparse newMatrix( "" );
    newMatrix.updateRows( other.getRows() );
    newMatrix.updateColumns( other.getColumns() );
    newMatrix.updateZeros( newMatrix.getColumns() * newMatrix.getRows() );
    return make_shared< CMatrixSparse >( newMatrix );
  }else
  {
    CMatrixDense newMatrix = other;
    newMatrix.updateName( "" );

    for( unsigned row = 0; row < newMatrix.getRows(); row++ )
    {
      for( unsigned column = 0; column < newMatrix.getColumns(); column++ )
      {
        newMatrix.updateSingleValue( row, column, value * newMatrix.getSingleValue( row, column ) );
      }
    }
    return make_shared< CMatrixDense >( newMatrix );
  }

}

shared_ptr< CMatrix > CScalar::multiplyWith( const CMatrixSparse & other ) const
{
  if( value == 0 )
  {
    CMatrixSparse newMatrix( "" );
    newMatrix.updateRows( other.getRows() );
    newMatrix.updateColumns( other.getColumns() );
    newMatrix.updateZeros( newMatrix.getColumns() * newMatrix.getRows() );
    return make_shared< CMatrixSparse >( newMatrix );
  }else
  {
    CMatrixSparse newMatrix = other;
    newMatrix.updateName( "" );
    for( unsigned number = 0; number < newMatrix.getValuesSize(); number++ )
    {
      newMatrix.updateElementByNumber( number, value * other.getElementByNumber( number ).value );
    }
    return make_shared< CMatrixSparse >( newMatrix );
  }
}

void CScalar::operator= ( const COperand & other ) const
{
  other.assignWhat( *this );
}

void CScalar::assignWhat( const CMatrixDense & other ) const
{
  // recreate element if it has the same name
  if( variables.find( name ) != variables.end() )
  {
    variables.erase( name );
  }

  CMatrixDense newMatrix = other;
  newMatrix.updateName( name );
  variables.insert( { name, make_shared< CMatrixDense >( newMatrix ) } );
}

void CScalar::assignWhat( const CMatrixSparse & other ) const
{
  // recreate element if it has the same name
  if( variables.find( name ) != variables.end() )
  {
    variables.erase( name );
  }

  CMatrixSparse newMatrix = other;
  newMatrix.updateName( name );
  variables.insert( { name, make_shared< CMatrixSparse >( newMatrix ) } );
}

void CScalar::assignWhat( const CScalar & other ) const
{
  // recreate element if it has the same name
  if( variables.find( name ) != variables.end() )
  {
    variables.erase( name );
  }

  CScalar newScalar = other;
  newScalar.updateName( name );
  newScalar.show();
  variables.insert( { name, make_shared< CScalar >( newScalar ) } );
}

shared_ptr< COperand > CScalar::operator== ( const COperand & other ) const
{
  return other.equalToWhat( *this );
}

shared_ptr< CScalar > CScalar::equalToWhat( const CScalar & other ) const
{
  return make_shared< CScalar >( CScalar( "", value == other.getValue() ) );
}

shared_ptr< COperand > CScalar::operator^ ( double number ) const
{
  return make_shared< CScalar >( CScalar( "", pow( value, number ) ) );
}

/*------------------------CMatrix------------------------*/

CMatrix::CMatrix( const string & name ) : COperand( name ) {}

unsigned CMatrix::getRows() const
{
  return rows;
}

unsigned CMatrix::getColumns() const
{
  return columns;
}

unsigned CMatrix::getZeros() const
{
  return zeros;
}

void CMatrix::updateRows( unsigned newRows )
{
  rows = newRows;
}

void CMatrix::updateColumns( unsigned newColumns )
{
  columns = newColumns;
}

void CMatrix::updateZeros( unsigned zerosNumber )
{
  zeros = zerosNumber;
}

/*------------------------CMatrixDense------------------------*/

CMatrixDense::CMatrixDense( const string & name ) : CMatrix( name )
{
  values.resize( 1 );
}
vector< vector< double > > CMatrixDense::getDenseValues() const
{
  return values;
}

double CMatrixDense::getSingleValue( unsigned row, unsigned column ) const
{
  return values.at( row ).at( column );
}

void CMatrixDense::compute( stack< shared_ptr< COperand > > & operandsStack )
{
  operandsStack.push( make_shared< CMatrixDense >( *this ) );
}

void CMatrixDense::show() const
{
  bool vector = values.size() == 1;

  cout << "Name: <" << name << "> ; Type: <Dense> ; Rows: <" <<
           rows << "> ; Columns: <" << columns << ">" << " ; " <<
           "Zeros: <" << zeros << ">" << endl;
  for( auto it1 = values.begin(); it1 != values.end(); it1++ )
  {
    cout << ( vector ? "<" :
            ( it1 == values.begin() ? "/" :
            ( it1 == values.end() - 1 ? "\\" : "|" ) ) );
    for( auto it2 = it1->begin(); it2 != it1->end(); it2++ )
    {
      cout << " " << setw( PRECISION + 3 ) << setprecision( PRECISION ) << *it2;
    }
    cout << setw( PRECISION + 3 ) << ( vector ? " >" :
            ( it1 == values.begin() ? " \\" :
            ( it1 == values.end() - 1 ? " /" : " |" ) ) ) << endl;
  }
}

void CMatrixDense::pushValue( double newValue, unsigned newRow )
{
  if( newRow > rows )
  {
    values.resize( newRow + 1 );
    rows++;
  }

  values.at( rows ).push_back( newValue );
}

void CMatrixDense::updateSingleValue( unsigned row, unsigned column, double newValue )
{
  values.at( row ).at( column ) = newValue;
}

void CMatrixDense::updateValues( vector<vector<double>> & newValues )
{
  values = newValues;
}

void CMatrixDense::resizeValues( unsigned rows, unsigned columns, double fillValue )
{
  values.resize( rows );
  for( unsigned currRow = 0; currRow < rows; currRow++ )
  {
    values.at( currRow ).resize( columns, fillValue );
  }
}

// create sparse matrix from dense
CMatrixSparse CMatrixDense::createSparseMatrix() const
{
  CMatrixSparse newMatrix( name, rows, columns, zeros );
  for( auto it1 = values.begin(); it1 != values.end(); it1++ )
  {
    for( auto it2 = it1->begin(); it2 != it1->end(); it2++ )
    {
      if( *it2 != 0 )
      {
        newMatrix.pushValue( SparseValue( *it2, it1 - values.begin(), it2 - it1->begin() ) );
      }
    }
  }
  return newMatrix;
}

CMatrixDense CMatrixDense::createDenseMatrix() const
{
  return *this;
}

shared_ptr< COperand > CMatrixDense::operator+( const COperand & other ) const
{
  return other.addWith( *this );
}

shared_ptr< CMatrix > CMatrixDense::addWith( const CMatrixDense & other ) const
{
  if( rows != other.getRows() && columns != other.getColumns() )
    throw string( "Added matrixes have different size values." );

  CMatrixDense newMatrix( "" );
  vector< vector< double > > valuesOther = other.getDenseValues();
  double newValue;
  unsigned zerosCounter = 0;

  // add values
  for( unsigned n = 0; n < rows; n++ )
  {
    for( unsigned m = 0; m < columns; m++ )
    {
      newValue = values.at(n).at(m) + valuesOther.at(n).at(m);
      newMatrix.pushValue( newValue, n );
      if( newValue == 0 )
        zerosCounter++;
    }
  }

  newMatrix.updateRows( rows );
  newMatrix.updateColumns( columns );
  newMatrix.updateZeros( zerosCounter );

  // transform to sparse matrix if it is needed
  if( zerosCounter > rows * columns / 2 )
    return make_shared< CMatrixSparse >( newMatrix.createSparseMatrix() );
  else
    return make_shared< CMatrixDense >( newMatrix );
}

shared_ptr< CMatrix > CMatrixDense::addWith( const CMatrixSparse & other ) const
{

  if( rows != other.getRows() && columns != other.getColumns() )
    throw string( "Added matrixes have different size values." );

  CMatrixDense newMatrix = *this;
  newMatrix.updateName( "" );

  vector< SparseValue > valuesOther = other.getSparseValues();
  unsigned zerosCounter = zeros;
  double newValue;

  // add values
  for( auto it = valuesOther.begin(); it != valuesOther.end(); it++ )
  {
    newValue = getSingleValue( it->row, it->column ) + it->value;

    if( newValue == 0 && getSingleValue( it->row, it->column ) != 0 )
      zerosCounter++;

    if( newValue != 0 && getSingleValue( it->row, it->column ) == 0 )
      zerosCounter--;

    newMatrix.updateSingleValue( it->row, it->column, newValue );
  }

  // transform to sparse matrix if it is needed
  newMatrix.updateZeros( zerosCounter );
  if( zerosCounter > rows * columns / 2 )
    return make_shared< CMatrixSparse >( newMatrix.createSparseMatrix() );
  else
    return make_shared< CMatrixDense >( newMatrix );
}

shared_ptr< COperand > CMatrixDense::operator- ( const COperand & other ) const
{
  return other.unaryMinus()->addWith( *this );
}

shared_ptr< COperand > CMatrixDense::operator* ( const COperand & other ) const
{
  return other.multiplyWith( *this );
}

shared_ptr< CMatrix > CMatrixDense::multiplyWith( const CMatrixDense & other ) const
{
  if( columns != other.getRows() )
    throw string( "Multiplied matrixes have inconpatible sizes.");

  CMatrixDense newMatrix( "" );
  float valueToPush;
  unsigned zerosCounter = 0;

  // multiply matrixes according to the definition
  for( unsigned row1 = 0; row1 < rows; row1++ )
  {
    for( unsigned column2 = 0; column2 < other.getColumns(); column2++ )
    {
      for( unsigned counter = 0; counter < columns; counter++ )
      {
        valueToPush += values.at(row1).at(counter) * other.getSingleValue( counter, column2 );
      }

      if( valueToPush == 0 )
        zerosCounter++;
      newMatrix.pushValue( valueToPush, row1 );
      valueToPush = 0;
    }
  }

  newMatrix.updateRows( rows );
  newMatrix.updateColumns( other.getColumns() );
  newMatrix.updateZeros( zerosCounter );

  // transform to sparse matrix if it is needed
  if( zerosCounter > rows * other.getColumns() )
    return make_shared< CMatrixSparse >( newMatrix.createSparseMatrix() );
  else
    return make_shared< CMatrixDense >( newMatrix );
}

shared_ptr< CMatrix > CMatrixDense::multiplyWith( const CMatrixSparse & other ) const
{
  return multiplyWith( other.createDenseMatrix() );
}

shared_ptr< COperand > CMatrixDense::unaryMinus() const
{
  CMatrixDense newMatrix = *this;
  newMatrix.updateName( "" );

  for( unsigned row = 0; row < rows; row++ )
  {
    for( unsigned column = 0; column < columns; column++ )
    {
      newMatrix.updateSingleValue( row, column, -getSingleValue( row, column ) );
    }
  }
  return make_shared< CMatrixDense >( newMatrix );
}

void CMatrixDense::operator= ( const COperand & other ) const
{
  other.assignWhat( *this );
}

shared_ptr< COperand > CMatrixDense::transpose() const
{
  CMatrixDense newMatrix( name );

  for( unsigned column = 0; column < columns; column++ )
  {
    for( unsigned row = 0; row < rows; row++ )
    {
      newMatrix.pushValue( values.at(row).at(column), column );
    }
  }

  newMatrix.updateZeros( zeros );
  newMatrix.updateRows( columns );
  newMatrix.updateColumns( rows );

  return make_shared< CMatrixDense >( newMatrix );
}

shared_ptr< COperand > CMatrixDense::determinant() const
{
  if( rows != columns )
    throw string( "Invalid matrix size for operation \"determinant\"." );

  return make_shared< CScalar >( CScalar( "", determinantDouble() ) );
}

shared_ptr< COperand > CMatrixDense::rank() const
{

  vector< vector < double > > newValues = ge()->createDenseMatrix().getDenseValues();

  // with simple gaussian elimination the way to find rank is to find minimun
  // betwen rank of the matrix and rank of the same transposed matrix
  unsigned rank = 0;
  unsigned rankTranspose = 0;

  // count rows containing only zeros and then subtracting this from the
  // general amount of rows

  for( unsigned row = 0; row < rows; row++ )
  {
    for( unsigned column = 0; column < columns; column++ )
    {
      if( newValues.at(row).at(column) != 0 )
        break;
      if( column == columns - 1 )
        rank++;
    }
  }

  rank = rows - rank;

  for( unsigned column = 0; column < columns; column++ )
  {
    for( unsigned row = 0; row < rows; row++ )
    {
      if( newValues.at(row).at(column) != 0 )
        break;
      if( row == rows - 1 )
        rankTranspose++;
    }
  }

  rankTranspose = columns - rankTranspose;

  return make_shared< CScalar >( CScalar( "", rankTranspose < rank ? rankTranspose : rank ) );
}

shared_ptr< COperand > CMatrixDense::operator== ( const COperand & other ) const
{
  return other.equalToWhat( *this );
}

shared_ptr< CScalar > CMatrixDense::equalToWhat( const CMatrixDense & other ) const
{
  for( unsigned row = 0; row < rows; row++ )
  {
    for( unsigned column = 0; column < columns; column++ )
    {
      if( getSingleValue( row, column ) != other.getSingleValue( row, column ) )
        return make_shared< CScalar >( CScalar( "", 0 ) );
    }
  }
  return make_shared< CScalar >( CScalar( "", 1 ) );
}

shared_ptr< CScalar > CMatrixDense::equalToWhat( const CMatrixSparse & other ) const
{
  return equalToWhat( other.createDenseMatrix() );
}

shared_ptr< COperand > CMatrixDense::inverse() const
{
  if( rows != columns )
    throw string( "Ivalid size of matrix for operation \"inverse\"." );

  double det = determinantDouble();

  if( det == 0 )
    throw string( "Inverse matrix doesn`t exist" );

  CMatrixDense newMatrix( "" );
  newMatrix.updateColumns( columns );
  newMatrix.updateZeros( 0 );
  vector< vector< double > > valuesCopy = getDenseValues();

  // find minors accorging to the definition
  if( rows == 2 )
  {
    newMatrix.pushValue( valuesCopy.at(1).at(1), 0 );
    newMatrix.pushValue( -valuesCopy.at(0).at(1), 0 );
    newMatrix.pushValue( -valuesCopy.at(1).at(0), 1 );
    newMatrix.pushValue( valuesCopy.at(0).at(0), 1 );
  }else
  {
    minorsRecursion( newMatrix, valuesCopy, 0 );

    // multiply values with -1 according to the definition
    for( unsigned row = 0; row < rows ; row++ )
      for( unsigned column = 0; column < columns ; column++ )
        if( ( ( row + 1 ) % 2 == 0 && ( column + 1 ) % 2 == 0 ) ||
            (  ( row + 1 ) % 2 != 0 && ( column + 1 ) % 2 != 0 ) )
          newMatrix.updateSingleValue( row, column, -1 * newMatrix.getSingleValue( row, column ) );
  }

  newMatrix.updateRows( rows );

  for( unsigned row = 0; row < newMatrix.getRows(); row++ )
  {
    for( unsigned column = 0; column < newMatrix.getColumns(); column++ )
    {
      newMatrix.updateSingleValue( row, column, 1 / det * newMatrix.getSingleValue( row, column ) );
    }
  }

  return make_shared< CMatrixDense >( newMatrix );
}

shared_ptr< CMatrix > CMatrixDense::ge() const
{
  if( rows == 1 || columns == 1 )
    return make_shared< CMatrixDense >( *this );

  CMatrixDense newMatrix = *this;
  newMatrix.updateName( "" );
  vector< vector< double > >newValues = newMatrix.getDenseValues();

  unsigned piv = 0;
  unsigned savedPiv = 0;
  unsigned zerosCounter = 0;

  // for "vertical" matrices with rows > columns + 1 pivot is no more in (0, 0)
  if( rows > columns + 1 )
  {
    piv = rows - columns;
    savedPiv = piv;
  }


  // pivotisation
  for( ; piv < rows; piv++ )
  {
    //cout << "PIV: " << piv << endl;
    for( unsigned row = piv + 1; row < rows; row++ )
    {
      //cout << newValues.at(piv).at(piv-savedPiv) << " < " << newValues.at(row).at(piv-savedPiv) << endl;
      //cout << "----------------------" << endl;
      if( newValues.at(piv).at(piv-savedPiv) < newValues.at(row).at(piv-savedPiv) )
      {
        // swap 2 rows
        for( unsigned n = 0; n < columns; n++ )
        {
          //cout << "+++" << endl;
          double tmp = newValues.at(piv).at(n);
          newValues.at(piv).at(n) = newValues.at(row).at(n);
          newValues.at(row).at(n) = tmp;
        }
      }
    }
  }

  piv = 0;
  if( rows > columns + 1 )
  {
    piv = rows - columns;
  }

  // elimination of elements under the pivots
  for( ; piv < rows - 1; piv++ )
  {
    for( unsigned row = piv + 1; row < rows; row++ )
    {
      // if pivot is zero, it means, that whole line under it contains nothing but zeros
      double mult;
      if( newValues.at(piv).at(piv-savedPiv) == 0 )
        mult = 0;
      else
        mult = newValues.at(row).at(piv-savedPiv)/newValues.at(piv).at(piv-savedPiv);

      for( unsigned n = 0; n < columns; n++ )
      {
        newValues.at(row).at(n) -= mult * newValues.at(piv).at(n);
      }
    }
  }

  // count zeros; because of multiple actions with matrix elements during
  // elimination this is the only way to do it
  for( unsigned row = 0; row < rows; row++ )
  {
    for( unsigned column = 0; column < columns; column++ )
    {
      if( newValues.at(row).at(column) == 0 )
        zerosCounter++;
    }
  }

  newMatrix.updateValues( newValues );
  newMatrix.updateZeros( zerosCounter );

  // transform to sparse matrix if it is needed
  if( zerosCounter > rows * columns / 2 )
    return make_shared< CMatrixSparse >( newMatrix.createSparseMatrix() );
  else
    return make_shared< CMatrixDense >( newMatrix );
}

double CMatrixDense::determinantDouble() const
{

  if( columns == 1 )
  {
    return values.at(0).at(0);
  }else
  if( columns == 2 )
  {
    return values.at(0).at(0) * values.at(1).at(1) -
           values.at(0).at(1) * values.at(1).at(0);
  }else // rows >= 3
  {
    double det = 0;

    for( unsigned j = 0; j < columns; j++ )
    {
      CMatrixDense subMatrix( "" );
      subMatrix.updateColumns( columns - 1 );

      for( unsigned k = 1; k < columns; k++ )
      {
        for( unsigned s = 0; s < columns; s++ )
        {
          if( s != j )
          {
            //cout << "V: " << values.at(k).at(s) << endl;
            subMatrix.pushValue( values.at(k).at(s), k-1 );
          }
        }
      }
      //subMatrix.show();
      //cout << "Submatrix determinant: " << subMatrix.determinantDouble() << endl;
      det += pow( -1, (double)j + 2 ) * values.at(0).at(j) * subMatrix.determinantDouble();
    }
    //cout << "Determinant: " << det << endl;
    return det;
  }
}

// the function finds minors solving determinants always by eliminating
// first line and swapping other lines with the first one
void CMatrixDense::minorsRecursion( CMatrixDense & matrix,
                                    vector< vector< double > > & newValues,
                                    unsigned rowsCounter ) const
{
  if( columns == 1 )
  {
    values.at(0).at(0);
  }else
  if( columns == 2 )
  {
    if( values.at(0).at(1) * values.at(1).at(0) -
        values.at(0).at(0) * values.at(1).at(1) == 0 )
    matrix.updateZeros( matrix.getZeros() + 1 );

    // only the last line elimination doesn`t cause swapping submatrixes lines
    if( rowsCounter == matrix.getColumns() - 1 )
    {
      matrix.pushValue(
      values.at(0).at(1) * values.at(1).at(0) -
      values.at(0).at(0) * values.at(1).at(1),
      rowsCounter );
    }else
    {
      matrix.pushValue(
      values.at(0).at(0) * values.at(1).at(1) -
      values.at(0).at(1) * values.at(1).at(0),
      rowsCounter );
    }
  }else // columns >= 3
  {

    for( unsigned row = 0; row < rows; row++ )
    {
      // swap rowsCounter line with the first line
      for( unsigned n = 0; n < columns; n++ )
      {
        double tmp = newValues.at(0).at(n);
        newValues.at(0).at(n) = newValues.at(rowsCounter).at(n);
        newValues.at(rowsCounter).at(n) = tmp;
      }

      // create submatrixes to find determinant
      for( unsigned j = 0; j < matrix.getColumns(); j++ )
      {
        CMatrixDense subMatrix( "" );
        subMatrix.updateColumns( matrix.getColumns() - 1 );

        for( unsigned k = 1; k < matrix.getColumns(); k++ )
        {
          for( unsigned s = 0; s < matrix.getColumns(); s++ )
          {
            if( s != j )
            {
              subMatrix.pushValue( newValues.at(k).at(s), k-1 );
            }
          }
        }

        subMatrix.minorsRecursion( matrix, newValues, rowsCounter );
      }
      // swap lines back
      for( unsigned n = 0; n < columns; n++ )
      {
        double tmp = newValues.at(0).at(n);
        newValues.at(0).at(n) = newValues.at(rowsCounter).at(n);
        newValues.at(rowsCounter).at(n) = tmp;
      }
      rowsCounter++;
    }
  }
}

shared_ptr< CMatrix > CMatrixDense::minors() const
{
  if( rows != columns )
    throw string( "Ivalid size of matrix for operation \"minors\"." );

  CMatrixDense newMatrix( "" );
  newMatrix.updateColumns( columns );
  newMatrix.updateZeros( 0 );
  vector< vector< double > > valuesCopy = getDenseValues();

  // find minors accorging to the definition
  if( rows == 2 )
  {
    newMatrix.pushValue( valuesCopy.at(1).at(1), 0 );
    newMatrix.pushValue( -valuesCopy.at(0).at(1), 0 );
    newMatrix.pushValue( -valuesCopy.at(1).at(0), 1 );
    newMatrix.pushValue( valuesCopy.at(0).at(0), 1 );
  }else
  {
    minorsRecursion( newMatrix, valuesCopy, 0 );

    // multiply values with -1 according to the definition
    for( unsigned row = 0; row < rows ; row++ )
      for( unsigned column = 0; column < columns ; column++ )
        if( ( ( row + 1 ) % 2 == 0 && ( column + 1 ) % 2 == 0 ) ||
            (  ( row + 1 ) % 2 != 0 && ( column + 1 ) % 2 != 0 ) )
          newMatrix.updateSingleValue( row, column, -1 * newMatrix.getSingleValue( row, column ) );
  }

  newMatrix.updateRows( rows );

  // transform to sparse matrix if it is needed
  if( newMatrix.getZeros() > rows * columns / 2 )
    return make_shared< CMatrixSparse >( newMatrix.createSparseMatrix() );
  else
    return make_shared< CMatrixDense >( newMatrix );
}

shared_ptr< COperand > CMatrixDense::split( double x1, double y1, double x2, double y2 ) const
{
  if( x1 >= columns || x2 >= columns ||
      y1 >= rows || y2 >= rows ||
      x1 > x2 || y1 > y2 )
    throw string( "Inappropriate arguments for operation \"split\"." );

  CMatrixDense newMatrix( "" );
  unsigned rowsCounter = 0;
  unsigned zerosCounter = 0;

  // plit the matrix by coordinates
  for( unsigned row = y1; row <= y2; row++ )
  {
    for( unsigned column = x1; column <= x2; column++ )
    {
      newMatrix.pushValue( values.at(row).at(column), rowsCounter );
      if( values.at(row).at(column) == 0 )
        zerosCounter++;
    }
    rowsCounter++;
  }

  newMatrix.updateRows( y2 - y1 + 1 );
  newMatrix.updateColumns( x2 - x1 + 1 );
  newMatrix.updateZeros( zerosCounter );

  // transform to sparse matrix if it is needed
  if( zerosCounter > ( y2 - y1 + 1 ) * ( x2 - x1 + 1 ) / 2 )
    return make_shared< CMatrixSparse >( newMatrix.createSparseMatrix() );
  else
    return make_shared< CMatrixDense >( newMatrix );
}

shared_ptr< COperand > CMatrixDense::merge( const COperand & other, double direction ) const
{
  return other.mergeWith( *this, direction );
}

// mergeHorizontal merges matrix b to the right side of the matrix a
CMatrixDense mergeHorizontal( const CMatrixDense & a, const CMatrixDense & b )
{
  if( a.getRows() != b.getRows() )
    throw string( "Merged matrixes have incompatible sizes." );

  CMatrixDense newMatrix = a;
  newMatrix.updateName( "" );

  vector< vector< double > > valuesA = a.getDenseValues();
  vector< vector< double > > valuesB = b.getDenseValues();

  for( unsigned rowA = 0; rowA < a.getRows(); rowA++ )
  {
    for( unsigned columnB = 0; columnB < b.getColumns(); columnB++ )
    {
      valuesA.at(rowA).push_back( valuesB.at(rowA).at(columnB));
    }
  }

  newMatrix.updateColumns( a.getColumns() + b.getColumns() );
  newMatrix.updateZeros( a.getZeros() + b.getZeros() );
  newMatrix.updateValues( valuesA );

  return newMatrix;
}

// mergeVertical merges matrix b under the matrix a
CMatrixDense mergeVertical( const CMatrixDense & a, const CMatrixDense & b )
{
  if( a.getColumns() != b.getColumns() )
    throw string( "Merged matrixes have incompatible sizes." );

  CMatrixDense newMatrix = a;
  newMatrix.updateName( "" );

  vector< vector< double > > valuesA = a.getDenseValues();
  vector< vector< double > > valuesB = b.getDenseValues();
  unsigned rowsCounter = a.getRows();

  for( unsigned rowB = 0; rowB < b.getRows(); rowB++ )
  {
    valuesA.resize( valuesA.size() + 1 );
    for( unsigned columnB = 0; columnB < b.getColumns(); columnB++ )
    {
      valuesA.at(rowsCounter).push_back( valuesB.at(rowB).at(columnB));
    }
    rowsCounter++;
  }

  newMatrix.updateRows( a.getRows() + b.getRows() );
  newMatrix.updateZeros( a.getZeros() + b.getZeros() );
  newMatrix.updateValues( valuesA );

  return newMatrix;
}

shared_ptr< CMatrix > CMatrixDense::mergeWith( const CMatrixDense & other, double direction ) const
{
  // append to the right side
  if( direction == 1 )
  {
    CMatrixDense newMatrix = mergeHorizontal( other, *this );

    if( newMatrix.getZeros() > newMatrix.getRows() * newMatrix.getColumns() / 2 )
      return make_shared< CMatrixSparse >( newMatrix.createSparseMatrix() );
    else
      return make_shared< CMatrixDense >( newMatrix );
  }else // append to the left side
  if( direction == 2 )
  {
    CMatrixDense newMatrix = mergeHorizontal( *this, other );

    if( newMatrix.getZeros() > newMatrix.getRows() * newMatrix.getColumns() / 2 )
      return make_shared< CMatrixSparse >( newMatrix.createSparseMatrix() );
    else
      return make_shared< CMatrixDense >( newMatrix );
  }else // append to the upper side
  if( direction == 0 )
  {
    CMatrixDense newMatrix = mergeVertical( *this, other );

    if( newMatrix.getZeros() > newMatrix.getRows() * newMatrix.getColumns() / 2 )
      return make_shared< CMatrixSparse >( newMatrix.createSparseMatrix() );
    else
      return make_shared< CMatrixDense >( newMatrix );
  }else //append to the down side
  if( direction == 3 )
  {
    CMatrixDense newMatrix = mergeVertical( other, *this );

    if( newMatrix.getZeros() > newMatrix.getRows() * newMatrix.getColumns() / 2 )
      return make_shared< CMatrixSparse >( newMatrix.createSparseMatrix() );
    else
      return make_shared< CMatrixDense >( newMatrix );
  }else
  {
    throw string( "Invalid third argument of operation \"merge\". Use only numbers 0, 1, 2 or 3." );
  }

}

shared_ptr< CMatrix > CMatrixDense::mergeWith( const CMatrixSparse & other, double direction ) const
{
  return other.mergeWith( *this, direction );
}

/*------------------------SparseValue------------------------*/

SparseValue::SparseValue( double value, unsigned row, unsigned column ) :
                          value( value ), row( row ), column( column ){}

/*------------------------CMatrixSparse------------------------*/

CMatrixSparse::CMatrixSparse( const string & name, unsigned rowsNumber,
                              unsigned columnsNumber, unsigned zerosNumber ) :
                              CMatrix( name )
{
  rows = rowsNumber;
  columns = columnsNumber;
  zeros = zerosNumber;
}

CMatrixSparse::CMatrixSparse( const string & name ) : CMatrix( name ) {}

unsigned CMatrixSparse::getValuesSize() const
{
  return values.size();
}

vector<SparseValue> CMatrixSparse::getSparseValues() const
{
  return values;
}

// getElementByNumber is need to get elements in cycles
SparseValue CMatrixSparse::getElementByNumber( unsigned number ) const
{
  return values.at( number );
}

void CMatrixSparse::pushValue( SparseValue newValue )
{
  values.push_back( newValue );
}

// updateCoordinatesByNumber is need to update elements in cycles
void CMatrixSparse::updateCoordinatesByNumber( unsigned number, unsigned newRow, unsigned newColumn )
{
  values.at( number ).row = newRow;
  values.at( number ).column = newColumn;
}

void CMatrixSparse::compute( stack< shared_ptr< COperand > > & operandsStack )
{
  operandsStack.push( make_shared< CMatrixSparse >( *this ) );
}

void CMatrixSparse::show() const
{
  auto it = values.begin();
  bool vector = rows == 1;

  cout << "Name: <" << name << "> ; Type: <Sparse> ; Rows: <" <<
           rows << "> ; Columns: <" << columns << ">" << " ; " <<
           "Zeros: <" << zeros << ">" << endl;
  for( unsigned currRow = 0; currRow < rows ; currRow++ )
  {
    cout << ( vector ? "<" :
            ( currRow == 0 ? "/" :
            ( currRow == rows - 1 ? "\\" : "|" ) ) );

    for( unsigned currColumn = 0; currColumn < columns ; currColumn++ )
    {
      if( !values.empty() && currRow == it->row && currColumn == it->column )
      {
        cout << " " << setw( PRECISION + 3 ) << setprecision( PRECISION ) << it->value;
        it++;
      }
      else
      {
        cout << " " << setw( PRECISION + 3 ) << "0";
      }
    }

    cout << setw( PRECISION + 3 ) << ( vector ? " >" :
                                        ( currRow == 0 ? " \\" :
                                        ( currRow == rows - 1 ? " /" : " |" ) ) ) << endl;
  }
}

void CMatrixSparse::updateElementByNumber( unsigned number, double newValue )
{
  values.at( number ).value = newValue;
}

shared_ptr< COperand > CMatrixSparse::operator+ ( const COperand & other ) const
{
  return other.addWith( *this );
}

shared_ptr< CMatrix > CMatrixSparse::addWith( const CMatrixDense & other ) const
{
  return other.addWith( *this );
}

shared_ptr< CMatrix > CMatrixSparse::addWith( const CMatrixSparse & other ) const
{
  CMatrixDense newMatrix( "" );
  newMatrix.updateRows( rows );
  newMatrix.updateColumns( columns );
  newMatrix.resizeValues( rows, columns, 0 );

  vector< SparseValue > valuesOther = other.getSparseValues();
  double newValue;
  unsigned nonZerosCounter = 0;

  // create new dense matrix
  for( auto it = values.begin(); it != values.end(); it++ )
  {
    newValue = it->value;

    if( newValue != 0 )
      nonZerosCounter++;

    newMatrix.updateSingleValue( it->row, it->column, newValue );
  }

  // fill new matrix with summ of values from operands
  for( auto it = valuesOther.begin(); it != valuesOther.end(); it++ )
  {
    newValue = newMatrix.getSingleValue( it->row, it->column ) + it->value;

    if( newValue != 0 && newMatrix.getSingleValue( it->row, it->column ) == 0 )
      nonZerosCounter++;

    newMatrix.updateSingleValue( it->row, it->column, newValue );
  }

  newMatrix.updateZeros( rows * columns - nonZerosCounter );

  if( nonZerosCounter > rows * columns / 2 )
    return make_shared< CMatrixDense >( newMatrix );
  else
    return make_shared< CMatrixSparse >( newMatrix.createSparseMatrix() );
}

shared_ptr< COperand > CMatrixSparse::operator- ( const COperand & other ) const
{
  return other.unaryMinus()->addWith( *this );
}

shared_ptr< COperand > CMatrixSparse::operator* ( const COperand & other ) const
{
  return other.multiplyWith( *this );
}

shared_ptr< CMatrix > CMatrixSparse::multiplyWith( const CMatrixDense & other ) const
{
  return createDenseMatrix().multiplyWith( other );
}

shared_ptr< CMatrix > CMatrixSparse::multiplyWith( const CMatrixSparse & other ) const
{
  return createDenseMatrix().multiplyWith( other.createDenseMatrix() );
}

shared_ptr< COperand > CMatrixSparse::operator== ( const COperand & other ) const
{
  return other.equalToWhat( *this );
}

shared_ptr< CScalar > CMatrixSparse::equalToWhat( const CMatrixDense & other ) const
{
  return createDenseMatrix().equalToWhat( other.createDenseMatrix() );
}

shared_ptr< CScalar > CMatrixSparse::equalToWhat( const CMatrixSparse & other ) const
{
  return equalToWhat( other.createDenseMatrix() );
}

shared_ptr< COperand > CMatrixSparse::unaryMinus() const
{
  CMatrixSparse newMatrix = *this;

  for( unsigned number = 0; number < values.size(); number++ )
  {
    newMatrix.updateElementByNumber( number, -values.at( number ).value );
  }

  return make_shared< CMatrixSparse >( newMatrix );
}

void CMatrixSparse::operator= ( const COperand & other ) const
{
  other.assignWhat( *this );
}

shared_ptr< COperand > CMatrixSparse::transpose() const
{
  CMatrixSparse newMatrix = *this;
  newMatrix.updateRows( columns );
  newMatrix.updateColumns( rows );

  for( unsigned number = 0; number < values.size(); number++ )
  {
    newMatrix.updateCoordinatesByNumber( number,
                                         values.at( number ).column,
                                         values.at( number ).row );
  }

  return make_shared< CMatrixSparse >( newMatrix );
}

// create dense matrix from sparse
CMatrixDense CMatrixSparse::createDenseMatrix() const
{
  CMatrixDense newMatrix( name );
  newMatrix.updateColumns( columns );
  newMatrix.updateZeros( zeros );

  auto it = values.begin();

  for( unsigned row = 0; row < rows; row++ )
  {
    for( unsigned column = 0; column < columns; column++ )
    {
      if( !values.empty() && row == it->row && column == it->column )
      {
        newMatrix.pushValue( it->value, row );
        it++;
      }
      else
      {
        newMatrix.pushValue( 0, row );
      }
    }
  }
  newMatrix.updateRows( rows );

  return newMatrix;
}

CMatrixSparse CMatrixSparse::createSparseMatrix() const
{
  return *this;
}

shared_ptr< COperand > CMatrixSparse::determinant() const
{
  if( rows != columns )
    throw string( "Invalid matrix size for operation \"determinant\"." );

  return make_shared< CScalar >( CScalar( "", determinantDouble() ) );
}

shared_ptr< CMatrix > CMatrixSparse::minors() const
{
  return createDenseMatrix().minors();
}

double CMatrixSparse::determinantDouble() const
{
  return createDenseMatrix().determinantDouble();
}

shared_ptr< CMatrix > CMatrixSparse::ge() const
{
  if( rows == 1 || columns == 1 )
    return make_shared< CMatrixSparse >( *this );

  CMatrixSparse newMatrix = *this;
  return make_shared< CMatrixSparse >( newMatrix );
}

shared_ptr< COperand > CMatrixSparse::rank() const
{
  return createDenseMatrix().rank();
}

shared_ptr< COperand > CMatrixSparse::inverse() const
{
  return createDenseMatrix().inverse();
}

shared_ptr< COperand > CMatrixSparse::split( double x1, double y1, double x2, double y2 ) const
{
  return createDenseMatrix().split(x1,y1,x2,y2);
}

shared_ptr< COperand > CMatrixSparse::merge( const COperand & other, double direction ) const
{
  return other.mergeWith( *this, direction );
}

shared_ptr< CMatrix > CMatrixSparse::mergeWith( const CMatrixDense & other, double direction ) const
{
  return createDenseMatrix().mergeWith( other, direction );
}

shared_ptr< CMatrix > CMatrixSparse::mergeWith( const CMatrixSparse & other, double direction ) const
{
  return createDenseMatrix().mergeWith( other.createDenseMatrix(), direction );
}
