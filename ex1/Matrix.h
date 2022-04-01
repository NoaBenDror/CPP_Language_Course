#include <iostream>
#ifndef EXERCISE5_MATRIX_H
#define EXERCISE5_MATRIX_H

#define INIT_ROW 1
#define INIT_COL 1

/**
 * This class represents a matrix
 */
class Matrix
{

private:
    int _rows, _cols;
    float** _mat;

    /**
     * This method deletes the _mat which was dynamically allocated
     */
    void _deleteTheMat();

    /**
     * This method calculates a cell in a matrix after performing matrix multiplication
     * @param rhs - the matrix on the right
     * @param row - the number of the row
     * @param col - the number of the column
     * @return the value that was calculated that should be in the cell [row][col]
     */
    float _calcMultCell(const Matrix& rhs, const int& row, const int& col) const;

public:

    /**
     * Constructs matrix rows * cols, initiates all elements to 0.
     * @param rows - num of rows in the matrix
     * @param cols - num of cols in the matrix
     */
    Matrix(int rows, int cols);

    /**
     * Default constructor - Constructs 1*1 matrix, the single element is initiated to 0.
     */
    Matrix(): Matrix(INIT_ROW, INIT_COL){}

    /**
     * Copy constructor - constructs matrix from another matrix
     * @param m - the matrix that is being copied
     */
    Matrix(const Matrix &m);

    /**
     * Destructure - Destroys the matrix
     */
    ~Matrix();

    /**
     * Getter to number of rows
     * @return the amount of rows
     */
    int getRows() const { return  _rows; }

    /**
     * Getter to number of columns
     * @return the amount of columns
     */
    int getCols() const { return _cols; }

    /**
     * Transforms the matrix into a column vector
     * @return the matrix after transformation
     */
    Matrix& vectorize();

    /**
     * Prints matrix elements
     */
    void print() const;

    /**
     * Assignment operator
     * @param rhs - the matrix we are copying
     * @return the matrix after assignment
     */
    Matrix& operator=(const Matrix& rhs);

    /**
     * Matrix multiplication
     * @param rhs - the matrix from the right in the multiplication
     * @return A new matrix after multiplication
     */
    Matrix operator*(const Matrix& rhs) const;

    /**
     * Scalar multiplication on the right
     * @param rhs - the scalar
     * @return new matrix after multiplication
     */
    Matrix operator*(const float &rhs) const;

    /**
     * Scalar multiplication on the left
     * @param lhs - the scalar
     * @param rhs - the matrix to be multiplied
     * @return new matrix after multiplication
     */
    friend Matrix operator*(const float &lhs, const Matrix &rhs);

    /**
     * Matrix multiplication accumulation
     * @param rhs - the matrix on the right
     * @return the matrix after multiplication
     */
    Matrix& operator*=(const Matrix& rhs);

    /**
     * Scalar multiplication accumulation
     * @param rhs - the scalar
     * @return the matrix after scalar multiplication accumulation
     */
    Matrix& operator*=(const float &rhs);

    /**
     * Scalar division on the right
     * @param rhs - the scalar
     * @return new matrix after division
     */
    Matrix operator/(const float &rhs) const;

    /**
     * Scalar division accumulation
     * @param rhs - the scalar
     * @return the matrix after scalar division accumulation
     */
    Matrix& operator/=(const float &rhs);

    /**
     * Matrix addition
     * @param rhs - the matrix to add
     * @return new matrix after addition
     */
    Matrix operator+(const Matrix& rhs) const;

    /**
     * Matrix addition accumulation
     * @param rhs - the matrix to add
     * @return the matrix after addition accumulation
     */
    Matrix& operator+=(const Matrix& rhs);

    /**
     * Matrix scalar addition accumulation
     * @param rhs - the scalar
     * @return the matrix after scalar addition accumulation
     */
    Matrix& operator+=(const float& rhs);

    /**
     * Parenthesis indexing (const)
     * @param pos1 - num of row
     * @param pos2 - num of column
     * @return the number in the index in the matrix
     */
    float operator()(int pos1, int pos2) const;

    /**
     * Parenthesis indexing (non-const)
     * @param pos1 - num of row
     * @param pos2 - num of column
     * @return a reference to the number in the index in the matrix
     */
    float& operator()(int pos1, int pos2);

    /**
     * Brackets indexing (const)
     * @param pos - the index
     * @return the number in the index in the matrix
     */
    float operator[] (int pos) const;

    /**
     * Brackets indexing (non-const)
     * @param pos - the index
     * @return a reference to the number in the index in the matrix
     */
    float& operator[](int pos);

    /**
     * Equality operator
     * @param rhs - the matrix to check equality to
     * @return true if the matrices are equal, false otherwise
     */
    bool operator==(const Matrix& rhs) const;

    /**
     * Inequality operator
     * @param rhs - the matrix to check inequality to
     * @return true if the matrices are inequal, false otherwise
     */
    bool operator!=(const Matrix& rhs) const;

    /**
     * Input stream operator
     * @param is - the input stream
     * @param rhs - the matrix
     * @return the input stream
     */
    friend std::istream& operator>>(std::istream& is, Matrix& rhs);

    /**
     * Output stream operator
     * @param os - the output stream
     * @param rhs - the matrix
     * @return the output stream
     */
    friend std::ostream& operator<<(std::ostream& os, const Matrix& rhs);

};

#endif //EXERCISE5_MATRIX_H
