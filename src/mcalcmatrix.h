#ifndef __MCALCMATRIX_H__
#define __MCALCMATRIX_H__

#include <string>
#include <vector>
#include <stack>
#include <memory>

using namespace std;

class COperand;
class CVariable;
class CScalar;
class CMatrix;
class CMatrixSparse;
class CMatrixDense;
struct SparseValue;

class CObject : public enable_shared_from_this< CObject >
{
public:
  CObject( const string & name );
  string getName() const;
  void updateName( const string & newName );
  virtual void compute( stack< shared_ptr< COperand > > & operandsStack ) = 0;
protected:
  string name;
};


class COperation : public CObject
{
public:
  COperation( const string & name );
  virtual void compute( stack< shared_ptr< COperand > > & operandsStack );
};


class COperand : public CObject
{
public:
  COperand( const string & name );
  virtual void show() const = 0;
  virtual double getValue() const{ throw string( "Scalar value as argument required" ); };

  virtual shared_ptr< COperand > operator+ ( const COperand & other ) const = 0;
  virtual shared_ptr< CMatrix > addWith( const CMatrixDense & other ) const
  { throw string( "Invalid operands for operator +." ); }
  virtual shared_ptr < CMatrix > addWith( const CMatrixSparse & other ) const
  { throw string( "Invalid operands for operator +." ); }
  virtual shared_ptr< CScalar > addWith( const CScalar & other ) const
  { throw string( "Invalid operands for operator +." ); }

  virtual shared_ptr< COperand > operator- ( const COperand & other ) const = 0;
  virtual shared_ptr< CMatrix > subtractWith( const CMatrixDense & other ) const
  { throw string( "Invalid operands for operator -." ); }
  virtual shared_ptr < CMatrix > subtractWith( const CMatrixSparse & other ) const
  { throw string( "Invalid operands for operator -." ); }
  virtual shared_ptr< CScalar > subtractWith( const CScalar & other ) const
  { throw string( "Invalid operands for operator -." ); }

  virtual shared_ptr< COperand > operator* ( const COperand & other ) const = 0;
  virtual shared_ptr< CMatrix > multiplyWith( const CMatrixDense & other ) const
  { throw string( "Invalid operands for operator *." ); }
  virtual shared_ptr < CMatrix > multiplyWith( const CMatrixSparse & other ) const
  { throw string( "Invalid operands for operator *." ); }
  virtual shared_ptr< CScalar > multiplyWith( const CScalar & other ) const
  { throw string( "Invalid operands for operator *." ); }

  virtual shared_ptr< COperand > operator / ( const COperand & other ) const
  { throw string( "Invalid operands for operator /." ); }
  virtual shared_ptr< CScalar > divideWith( const CScalar & other ) const
  { throw string( "Invalid operands for operator /." ); }

  virtual void operator= ( const COperand & other ) const = 0;
  virtual void assignWhat( const CMatrixDense & other ) const
  { throw string( "Invalid operands for operator =." ); }
  virtual void assignWhat( const CMatrixSparse & other ) const
  { throw string( "Invalid operands for operator =." ); }
  virtual void assignWhat( const CScalar & other ) const
  { throw string( "Invalid operands for operator =." ); }

  virtual shared_ptr< COperand > operator== ( const COperand & other ) const = 0;
  virtual shared_ptr< CScalar > equalToWhat( const CMatrixDense & other ) const
  { throw string( "Invalid operands for operator ==." ); }
  virtual shared_ptr< CScalar > equalToWhat( const CMatrixSparse & other ) const
  { throw string( "Invalid operands for operator ==." ); }
  virtual shared_ptr< CScalar > equalToWhat( const CScalar & other ) const
  { throw string( "Invalid operands for operator ==." ); }

  virtual shared_ptr< COperand > operator^ ( double number ) const
  { throw string( "Invalid argument for operator ^." ); }

  virtual shared_ptr< COperand > unaryMinus() const = 0;

  virtual shared_ptr< COperand > transpose() const
  { throw string( "Invalid argument for operation \"transpose\"." ); }

  virtual shared_ptr< CMatrix > ge() const
  { throw string( "Invalid argument for operation \"ge\"." ); }

  virtual shared_ptr< COperand > determinant() const
  { throw string( "Invalid argument for operation \"determinant\"." ); }

  virtual shared_ptr< CMatrix > minors() const
  { throw string( "Invalid argument for operation \"minors\"." ); }

  virtual double determinantDouble() const
  { throw string( "Invalid argument for operation \"determinant\"." ); }

  virtual shared_ptr< COperand > rank() const
  { throw string( "Invalid argument for operation \"rank\"." ); }

  virtual shared_ptr< COperand > inverse() const
  { throw string( "Invalid argument for operation \"inverse\"." ); }

  virtual shared_ptr< COperand > split( double x1, double y1, double x2, double y2 ) const
  { throw string( "Invalid arguments for operation \"split\"." ); }

  virtual shared_ptr< COperand > merge( const COperand & other, double direction ) const
  { throw string( "Invalid arguments for operation \"merge\"." ); }
  virtual shared_ptr< CMatrix > mergeWith( const CMatrixDense & other, double direction ) const
  { throw string( "Invalid arguments for operation \"merge\"." ); }
  virtual shared_ptr< CMatrix > mergeWith( const CMatrixSparse & other, double direction ) const
  { throw string( "Invalid arguments for operation \"merge\"." ); }
};


class CVariable : public CObject
{
public:
  CVariable( const string & name );
  virtual void compute( stack< shared_ptr< COperand > > & operandsStack );
};


class CScalar : public COperand
{
public:
  CScalar( const string & name, double value );
  virtual void compute( stack< shared_ptr< COperand > > & operandsStack );
  virtual void show() const;
  virtual double getValue() const;

  virtual shared_ptr< COperand > operator + ( const COperand & other ) const;
  virtual shared_ptr< CScalar > addWith( const CScalar & other ) const;

  virtual shared_ptr< COperand > operator - ( const COperand & other ) const;

  virtual shared_ptr< COperand > unaryMinus() const;

  virtual shared_ptr< COperand > operator* ( const COperand & other ) const;
  virtual shared_ptr< CScalar > multiplyWith( const CScalar & other ) const;
  virtual shared_ptr< CMatrix > multiplyWith( const CMatrixDense & other ) const;
  virtual shared_ptr< CMatrix > multiplyWith( const CMatrixSparse & other ) const;

  virtual shared_ptr< COperand > operator / ( const COperand & other ) const;
  virtual shared_ptr< CScalar > divideWith( const CScalar & other ) const;

  virtual shared_ptr< COperand > operator^ ( double number ) const;

  virtual void operator= ( const COperand & other ) const;
  virtual void assignWhat( const CMatrixDense & other ) const;
  virtual void assignWhat( const CMatrixSparse & other ) const;
  virtual void assignWhat( const CScalar & other ) const;

  virtual shared_ptr< COperand > operator== ( const COperand & other ) const;
  virtual shared_ptr< CScalar > equalToWhat( const CScalar & other ) const;
private:
  double value;
};


class CMatrix : public COperand
{
public:
  CMatrix( const string & name );

  unsigned getRows() const;
  unsigned getColumns() const;
  unsigned getZeros() const;
  virtual vector<vector<double>> getDenseValues() const
  { throw string( "No dense values in a sparse matrix."); }
  virtual vector<SparseValue> getSparseValues() const
  { throw string( "No sparse values in a dense matrix."); }

  virtual CMatrixDense createDenseMatrix() const = 0;
  virtual CMatrixSparse createSparseMatrix() const = 0;

  void updateRows( unsigned newRows );
  void updateColumns( unsigned newColumns );
  void updateZeros( unsigned zerosNumber );

protected:
  bool square;
  unsigned rows = 0;
  unsigned columns = 0;
  unsigned zeros;
};

class CMatrixDense : public CMatrix
{
public:
  CMatrixDense( const string & name );
  vector<vector<double>> getDenseValues() const;
  double getSingleValue( unsigned row, unsigned column ) const;

  virtual void show() const;
  virtual void compute( stack< shared_ptr< COperand > > & operandsStack );
  void pushValue( double newValue, unsigned newRow );
  void updateSingleValue( unsigned row, unsigned column, double newValue );
  void updateValues( vector<vector<double>> & newValues );
  void resizeValues( unsigned rows, unsigned columns, double fillValue );
  virtual CMatrixSparse createSparseMatrix() const;
  virtual CMatrixDense createDenseMatrix() const;

  virtual shared_ptr< COperand > operator + ( const COperand & other ) const;
  virtual shared_ptr< CMatrix > addWith( const CMatrixDense & other ) const;
  virtual shared_ptr< CMatrix > addWith( const CMatrixSparse & other ) const;

  virtual shared_ptr< COperand > operator - ( const COperand & other ) const;

  virtual shared_ptr< COperand > operator* ( const COperand & other ) const;
  virtual shared_ptr< CMatrix > multiplyWith( const CMatrixDense & other ) const;
  virtual shared_ptr< CMatrix > multiplyWith( const CMatrixSparse & other ) const;

  virtual shared_ptr< COperand > unaryMinus() const;

  virtual void operator= ( const COperand & other ) const;

  virtual shared_ptr< COperand > operator== ( const COperand & other ) const;
  virtual shared_ptr< CScalar > equalToWhat( const CMatrixDense & other ) const;
  virtual shared_ptr< CScalar > equalToWhat( const CMatrixSparse & other ) const;

  virtual shared_ptr< COperand > transpose() const;

  virtual shared_ptr< COperand > determinant() const;

  virtual shared_ptr< COperand > rank() const;

  virtual shared_ptr< COperand > inverse() const;

  virtual shared_ptr< CMatrix > ge() const;

  virtual double determinantDouble() const;

  virtual shared_ptr< CMatrix > minors() const;

  void minorsRecursion( CMatrixDense & matrix,
                        vector< vector< double > > & newValues,
                        unsigned rowsCounter ) const;

  virtual shared_ptr< COperand > split( double x1, double y1, double x2, double y2 ) const;

  virtual shared_ptr< COperand > merge( const COperand & other, double direction ) const;
  virtual shared_ptr< CMatrix > mergeWith( const CMatrixDense & other, double direction ) const;
  virtual shared_ptr< CMatrix > mergeWith( const CMatrixSparse & other, double direction ) const;
private:
  vector<vector<double>> values;
};

struct SparseValue
{
  SparseValue( double value, unsigned row, unsigned column );

  double value;
  unsigned row, column;
};


class CMatrixSparse : public CMatrix
{
public:
  CMatrixSparse( const string & name );
  CMatrixSparse( const string & name, unsigned rowsNumber, unsigned columnsNumber, unsigned zerosNumber );
  vector<SparseValue> getSparseValues() const;
  unsigned getValuesSize() const;
  SparseValue getElementByNumber( unsigned number ) const;

  virtual void show() const;
  virtual void compute( stack< shared_ptr< COperand > > & operandsStack );
  void pushValue( SparseValue newValue );
  void updateElementByNumber( unsigned number, double Value );
  void updateCoordinatesByNumber( unsigned number, unsigned newRow, unsigned newColumn );
  virtual CMatrixDense createDenseMatrix() const;
  virtual CMatrixSparse createSparseMatrix() const;

  virtual shared_ptr< COperand > operator+ ( const COperand & other ) const;
  virtual shared_ptr< CMatrix > addWith( const CMatrixDense & other ) const;
  virtual shared_ptr< CMatrix > addWith( const CMatrixSparse & other ) const;

  virtual shared_ptr< COperand > operator- ( const COperand & other ) const;

  virtual shared_ptr< COperand > operator* ( const COperand & other ) const;
  virtual shared_ptr< CMatrix > multiplyWith( const CMatrixDense & other ) const;
  virtual shared_ptr< CMatrix > multiplyWith( const CMatrixSparse & other ) const;

  virtual shared_ptr< COperand > operator== ( const COperand & other ) const;
  virtual shared_ptr< CScalar > equalToWhat( const CMatrixDense & other ) const;
  virtual shared_ptr< CScalar > equalToWhat( const CMatrixSparse & other ) const;

  virtual shared_ptr< COperand > unaryMinus() const;

  virtual void operator= ( const COperand & other ) const;

  virtual shared_ptr< COperand > transpose() const;

  virtual shared_ptr< COperand > determinant() const;

  virtual shared_ptr< CMatrix > minors() const;

  virtual shared_ptr< COperand > rank() const;

  virtual shared_ptr< COperand > inverse() const;

  virtual shared_ptr< CMatrix > ge() const;

  virtual double determinantDouble() const;

  virtual shared_ptr< COperand > split( double x1, double y1, double x2, double y2 ) const;

  virtual shared_ptr< COperand > merge( const COperand & other, double direction ) const;
  virtual shared_ptr< CMatrix > mergeWith( const CMatrixDense & other, double direction ) const;
  virtual shared_ptr< CMatrix > mergeWith( const CMatrixSparse & other, double direction ) const;
private:
  vector<SparseValue> values;
};

#endif
