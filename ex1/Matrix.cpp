#include "Matrix.h"

#define ERR_DIM_MSG "Invalid matrix dimensions."
#define ERR_DIV_MSG "Division by zero."
#define ERR_INDEX_MSG "Index out of range."
#define ERR_IS_MSG "Error loading from input stream."
#define INIT_VAL 0
#define NEGATIVE 0
#define DIV_BY_ZERO 0
#define VEC_COL 1
#define END_OF_LINE "\n"
#define SPACE " "

/**
* Constructs matrix rows * cols, initiates all elements to 0.
* @param rows - num of rows in the matrix
* @param cols - num of cols in the matrix
*/
Matrix::Matrix(int rows, int cols): _rows(rows), _cols(cols)
{
    if (_rows < NEGATIVE || _cols < NEGATIVE)
    {
        exit(EXIT_FAILURE);
    }

    // allocate memory for _mat
    _mat = new float*[_rows];
    for (int i = 0; i <_rows; i++)
    {
        _mat[i] = new float[_cols];
    }

    // initiate all elements to 0
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < _cols; j++)
        {
            _mat[i][j] = INIT_VAL;
        }
    }
}

/**
* Copy constructor - constructs matrix from another matrix
* @param m - the matrix that is being copied
*/
Matrix::Matrix(const Matrix &m): _rows(m._rows), _cols(m._cols)
{
    // allocate memory for _mat
    _mat = new float*[_rows];
    for (int i = 0; i <_rows; i++)
    {
        _mat[i] = new float[_cols];
    }

    // copy the values from m._mat to _mat
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < _cols; j++)
        {
            _mat[i][j] = m._mat[i][j];
        }
    }
}

/**
* This method deletes the _mat which was dynamically allocated
*/
void Matrix::_deleteTheMat()
{
    for (int i = 0; i < _rows; i++)
    {
        delete [] _mat[i];
    }
    delete [] _mat;
}

/**
* Destructure - Destroys the matrix
*/
Matrix::~Matrix()
{
    _deleteTheMat();
}

/**
* Transforms the matrix into a column vector
* @return the matrix after transformation
*/
Matrix& Matrix::vectorize()
{
    int vecRows = _rows * _cols;
    int vecCols = VEC_COL;
    int vecIdx = 0;

    // allocate new vector mat (_rows*_cols rows, 1 column)
    auto** vecMat = new float*[vecRows];
    for (int i = 0; i <vecRows; i++)
    {
       vecMat[i] = new float[vecCols];
    }

    // copy the elements to the transformed matrix
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < _cols; j++)
        {
            vecMat[vecIdx++][0] = _mat[i][j];
        }
    }

    _deleteTheMat(); // delete the previous _mat
    _mat = vecMat; // set _mat as the vector mat
    _rows = vecRows;
    _cols = vecCols;
    return *this;
}

/**
* Prints matrix elements
*/
void Matrix::print() const
{
    std::cout << *this;
}

/**
* Assignment operator
* @param rhs - the matrix we are copying
* @return the matrix after assignment
*/
Matrix& Matrix::operator=(const Matrix& rhs)
{
    if (this != &rhs)
    {
        _deleteTheMat(); // delete the _mat (dynamically allocated)
        _rows = rhs._rows;
        _cols = rhs._cols;
        // allocate memory for new _mat
        _mat = new float*[_rows];
        for (int i = 0; i <_rows; i++)
        {
            _mat[i] = new float[_cols];
        }

        // copy the values from rhs._mat to _mat
        for (int i = 0; i < _rows; i++)
        {
            for (int j = 0; j < _cols; j++)
            {
                _mat[i][j] = rhs._mat[i][j];
            }
        }
    }
    return *this;
}

/**
* This method calculates a cell in a matrix after performing matrix multiplication
* @param rhs - the matrix on the right
* @param row - the number of the row
* @param col - the number of the column
* @return the value that was calculated that should be in the cell [row][col]
*/
float Matrix::_calcMultCell(const Matrix &rhs, const int& row, const int& col) const
{
    float sum = 0;
    for (int i = 0; i < _cols; i++)
    {
        sum += _mat[row][i] * rhs._mat[i][col];
    }
    return sum;
}

/**
* Matrix multiplication
* @param rhs - the matrix from the right in the multiplication
* @return A new matrix after multiplication
*/
Matrix Matrix::operator*(const Matrix &rhs) const
{
    if (_cols != rhs._rows) // check valid dimensions
    {
        std::cerr << ERR_DIM_MSG << std::endl;
        exit(EXIT_FAILURE);
    }

    Matrix multMat(_rows, rhs._cols);
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < rhs._cols; j++)
        {
           multMat._mat[i][j] = _calcMultCell(rhs, i, j);
        }
    }
    return multMat;
}

/**
* Scalar multiplication on the right
* @param rhs - the scalar
* @return new matrix after multiplication
*/
Matrix Matrix::operator*(const float &rhs) const
{
    Matrix multMat = Matrix(_rows, _cols);
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < _cols; j++)
        {
            multMat._mat[i][j] = _mat[i][j] * rhs;
        }
    }
    return multMat;
}

/**
* Scalar multiplication on the left
* @param lhs - the scalar
* @param rhs - the matrix to be multiplied
* @return new matrix after multiplication
*/
Matrix operator*(const float &lhs, const Matrix &rhs) { return  rhs * lhs; }

/**
* Matrix multiplication accumulation
* @param rhs - the matrix on the right
* @return the matrix after multiplication
*/
Matrix& Matrix::operator*=(const Matrix &rhs)
{
    if (_cols != rhs._rows) // check valid dimensions
    {
        std::cerr << ERR_DIM_MSG << std::endl;
        exit(EXIT_FAILURE);
    }

    int multRows = _rows;
    int multCols = rhs._cols;
    auto** multMat = new float*[multRows];
    for (int i = 0; i < multRows; i++)
    {
        multMat[i] = new float[multCols];
    }

    // calculate matrices multiplication
    for (int i = 0; i < multRows; i++)
    {
        for (int j = 0; j < multCols; j++)
        {
            multMat[i][j] = _calcMultCell(rhs, i, j);
        }
    }

    _deleteTheMat(); // delete the previous _mat
    _mat = multMat;
    _rows = multRows;
    _cols = multCols;
    return *this;
}

/**
* Scalar multiplication accumulation
* @param rhs - the scalar
* @return the matrix after scalar multiplication accumulation
*/
Matrix& Matrix::operator*=(const float &rhs)
{
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < _cols; j++)
        {
            _mat[i][j] *= rhs;
        }
    }
    return *this;
}

/**
* Scalar division on the right
* @param rhs - the scalar
* @return new matrix after division
*/
Matrix Matrix::operator/(const float &rhs) const
{
    if (rhs == DIV_BY_ZERO)
    {
        std::cerr << ERR_DIV_MSG << std::endl;
        exit(EXIT_FAILURE);
    }
    return (*this) * (1 / rhs);
}

/**
* Scalar division accumulation
* @param rhs - the scalar
* @return the matrix after scalar division accumulation
*/
Matrix& Matrix::operator/=(const float &rhs)
{
    if (rhs == DIV_BY_ZERO)
    {
        std::cerr << ERR_DIV_MSG << std::endl;
        exit(EXIT_FAILURE);
    }
    return (*this) *= (1 / rhs);
}

/**
* Matrix addition
* @param rhs - the matrix to add
* @return new matrix after addition
*/
Matrix Matrix::operator+(const Matrix &rhs) const
{
    if (_rows != rhs._rows || _cols != rhs._cols) // check valid dimensions
    {
        std::cerr << ERR_DIM_MSG << std::endl;
        exit(EXIT_FAILURE);
    }

    Matrix addMat = Matrix(_rows, _cols);
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < _cols; j++)
        {
            addMat._mat[i][j] = _mat[i][j] + rhs._mat[i][j];
        }
    }
    return addMat;
}

/**
* Matrix addition accumulation
* @param rhs - the matrix to add
* @return the matrix after addition accumulation
*/
Matrix& Matrix::operator+=(const Matrix &rhs)
{
    if (_rows != rhs._rows || _cols != rhs._cols) // check valid dimensions
    {
        std::cerr << ERR_DIM_MSG << std::endl;
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < _cols; j++)
        {
            _mat[i][j] += rhs._mat[i][j];
        }
    }
    return *this;
}

/**
* Matrix scalar addition accumulation
* @param rhs - the scalar
* @return the matrix after scalar addition accumulation
*/
Matrix& Matrix::operator+=(const float &rhs)
{
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < _cols; j++)
        {
            _mat[i][j] += rhs;
        }
    }
    return *this;
}

/**
* Parenthesis indexing (non-const)
* @param pos1 - num of row
* @param pos2 - num of column
* @return the number in the index in the matrix
*/
float& Matrix::operator()(int pos1, int pos2)
{
    if (pos1 < NEGATIVE || pos1 > _rows - 1 || pos2 < NEGATIVE || pos2 > _cols - 1) // check indexes validity
    {
        std::cerr << ERR_INDEX_MSG << std::endl;
        exit(EXIT_FAILURE);
    }
    return _mat[pos1][pos2];
}

/**
* Parenthesis indexing (const)
* @param pos1 - num of row
* @param pos2 - num of column
* @return a reference to the number in the index in the matrix
*/
float Matrix::operator()(int pos1, int pos2) const
{
    if (pos1 < NEGATIVE || pos1 > _rows - 1 || pos2 < NEGATIVE || pos2 > _cols - 1) // check indexes validity
    {
        std::cerr << ERR_INDEX_MSG << std::endl;
        exit(EXIT_FAILURE);
    }
    return _mat[pos1][pos2];
}

/**
* Brackets indexing (const)
* @param pos - the index
* @return the number in the index in the matrix
*/
float& Matrix::operator[](int pos)
{
    if (pos < NEGATIVE || pos > _rows * _cols - 1) // check index validity
    {
        std::cerr << ERR_INDEX_MSG << std::endl;
        exit(EXIT_FAILURE);
    }
    int r = pos / _cols;
    int c = pos % _cols;
    return _mat[r][c];
}

/**
* Brackets indexing (non-const)
* @param pos - the index
* @return a reference to the number in the index in the matrix
*/
float Matrix::operator[](int pos) const
{
    if (pos < NEGATIVE || pos > _rows * _cols - 1) // check index validity
    {
        std::cerr << ERR_INDEX_MSG << std::endl;
        exit(EXIT_FAILURE);
    }
    int r = pos / _cols;
    int c = pos % _cols;
    return _mat[r][c];
}

/**
* Equality operator
* @param rhs - the matrix to check equality to
* @return true if the matrices are equal, false otherwise
*/
bool Matrix::operator==(const Matrix &rhs) const
{
    if (_rows != rhs._rows || _cols != rhs._cols)
    {
        return false;
    }

    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < _cols; j++)
        {
            if (_mat[i][j] != rhs._mat[i][j])
            {
                return false;
            }
        }
    }
    return true;
}

/**
* Inequality operator
* @param rhs - the matrix to check inequality to
* @return true if the matrices are inequal, false otherwise
*/
bool Matrix::operator!=(const Matrix &rhs) const
{
    return !((*this) == rhs);
}

/**
* Input stream operator
* @param is - the input stream
* @param rhs - the matrix
* @return the input stream
*/
std::istream& operator>>(std::istream& is, Matrix& rhs)
{
    if (!is.good())
    {
        std::cerr << ERR_IS_MSG << std::endl;
        exit(EXIT_FAILURE);
    }
    int row = 0;
    int col = 0;
    float val;
    while (is >> val)
    {
        rhs._mat[row][col] = val;
        col++;
        if (rhs._cols == col)
        {
            col = 0;
            row++;
        }
    }
    return is;
}

/**
* Output stream operator
* @param os - the output stream
* @param rhs - the matrix
* @return the output stream
*/
std::ostream& operator<<(std::ostream& os, const Matrix& rhs)
{
    for (int i = 0; i < rhs._rows; i++)
    {
        for (int j = 0; j < rhs._cols; j++)
        {
            os << rhs._mat[i][j];
            if (j != rhs._cols - 1)
            {
                os << SPACE;
            }
        }
        if (i != rhs._rows - 1)
        {
            os << END_OF_LINE;
        }
    }
    return os;
}