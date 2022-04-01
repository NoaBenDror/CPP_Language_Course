#include "Matrix.h"
#include <cmath>
#define CONV_ROWS 3
#define CONV_COLS 3
#define BLUR_CONV_CONST 1.0/16
#define SOBEL_CONV_CONST 1.0/8
#define NUM_OF_SHADES 256
#define MAX_SHADE 255
#define MIN_SHADE 0
#define BLUR_CONV_0_0 1
#define BLUR_CONV_0_1 2
#define BLUR_CONV_0_2 1
#define BLUR_CONV_1_0 2
#define BLUR_CONV_1_1 4
#define BLUR_CONV_1_2 2
#define BLUR_CONV_2_0 1
#define BLUR_CONV_2_1 2
#define BLUR_CONV_2_2 1
#define SOBEL_CONV1_0_0 1
#define SOBEL_CONV1_0_1 0
#define SOBEL_CONV1_0_2 -1
#define SOBEL_CONV1_1_0 2
#define SOBEL_CONV1_1_1 0
#define SOBEL_CONV1_1_2 -2
#define SOBEL_CONV1_2_0 1
#define SOBEL_CONV1_2_1 0
#define SOBEL_CONV1_2_2 -1
#define SOBEL_CONV2_0_0 1
#define SOBEL_CONV2_0_1 2
#define SOBEL_CONV2_0_2 1
#define SOBEL_CONV2_1_0 0
#define SOBEL_CONV2_1_1 0
#define SOBEL_CONV2_1_2 0
#define SOBEL_CONV2_2_0 -1
#define SOBEL_CONV2_2_1 -2
#define SOBEL_CONV2_2_2 -1

/**
 * Performs quantization on the input image by the given number of levels.
 * @param image - the matrix on which the quantization will be performed
 * @param levels - the quantization is performed according to this number of levels
 * @return new matrix which is the result of running the operator on the image
 */
Matrix quantization(const Matrix& image, int levels)
{
    int rows = image.getRows();
    int cols = image.getCols();
    Matrix quantMat(rows, cols);
    int range = NUM_OF_SHADES / levels; // the size of each range of shades that will get the same output shade
    int* newShade = new int[levels]; // array of the new shades
    // fill output shades
    newShade[0] = range / 2 - 1;
    for (int i = 1; i < levels; i++)
    {
        newShade[i] = newShade[i - 1] + range;
    }

    // fill output matrix
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int intVal = (int)image(i, j) / range; // find range index
            quantMat(i, j) = (float)newShade[intVal];
        }
    }
    delete [] newShade;
    return quantMat;
}

/**
 * Calculates the value that should be in image[r][c] after convolution
 * @param image - the image we want to perform convolution on
 * @param convMat - the convolution matrix
 * @param r - the row
 * @param c - the column
 * @return the value that should be in image[r][c] after convolution
 */
float calcConvCell(const Matrix& image, const Matrix& convMat, const int& r, const int& c)
{
    float sum = 0;
    int rows = image.getRows();
    int cols = image.getCols();
    for (int i = 0; i < CONV_ROWS; i++)
    {
        for (int j = 0; j < CONV_COLS; j++)
        {
            if (r + i - 1 >= 0 && r + i - 1 < rows && c + j - 1 >= 0 && c + j - 1 < cols)
            {
                sum += image(r + i - 1, c + j - 1) * convMat(i, j);
            }
        }
    }
    return sum;
}

/**
 * Performs convolution on the input image
 * @param image - the image we want to perform convolution on
 * @param convMat - the convolution matrix
 * @return new matrix which is the result of running the operator on the image
 */
Matrix convolution(const Matrix& image, const Matrix& convMat)
{
    int rows = image.getRows();
    int cols = image.getCols();
    Matrix resMat(rows, cols);
    // perform convolution
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            resMat(i, j) = rintf(calcConvCell(image, convMat, i, j));
        }
    }
    return resMat;
}

/**
 * This function checks if the values in the image are out of limits (smaller than 0 or larger than 255). if yes,
 * sets the values to be in the right range.
 * @param image - the image we are working on
 */
void limitVals(Matrix& image)
{
    int rows = image.getRows();
    int cols = image.getCols();
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (image(i, j) < MIN_SHADE)
            {
                image(i, j) = MIN_SHADE;
            }
            else if (image(i, j) > MAX_SHADE)
            {
                image(i, j) = MAX_SHADE;
            }
        }
    }
}

/**
 * Performs gaussian blurring on the input image.
 * @param image - the image we want to blur
 * @return new matrix which is the result of the operator on the image
 */
Matrix blur(const Matrix& image)
{
    Matrix convMat(CONV_ROWS, CONV_COLS);
    convMat(0, 0) = BLUR_CONV_0_0;
    convMat(0, 1) = BLUR_CONV_0_1;
    convMat(0, 2) = BLUR_CONV_0_2;
    convMat(1, 0) = BLUR_CONV_1_0;
    convMat(1, 1) = BLUR_CONV_1_1;
    convMat(1, 2) = BLUR_CONV_1_2;
    convMat(2, 0) = BLUR_CONV_2_0;
    convMat(2, 1) = BLUR_CONV_2_1;
    convMat(2, 2) = BLUR_CONV_2_2;
    convMat *= BLUR_CONV_CONST;

    Matrix res = convolution(image, convMat);
    limitVals(res);
    return res;
}

/**
 * Performs sobel edge detection on the input image
 * @param image - the image we want to perform sobel edge detection on
 * @return new matrix which is the result of running the operator on the image
 */
Matrix sobel(const Matrix& image)
{
    Matrix convMat1(CONV_ROWS, CONV_COLS);
    convMat1(0, 0) = SOBEL_CONV1_0_0;
    convMat1(0, 1) = SOBEL_CONV1_0_1;
    convMat1(0, 2) = SOBEL_CONV1_0_2;
    convMat1(1, 0) = SOBEL_CONV1_1_0;
    convMat1(1, 1) = SOBEL_CONV1_1_1;
    convMat1(1, 2) = SOBEL_CONV1_1_2;
    convMat1(2, 0) = SOBEL_CONV1_2_0;
    convMat1(2, 1) = SOBEL_CONV1_2_1;
    convMat1(2, 2) = SOBEL_CONV1_2_2;
    convMat1 *= SOBEL_CONV_CONST;

    Matrix convMat2(CONV_ROWS, CONV_COLS);
    convMat2(0, 0) = SOBEL_CONV2_0_0;
    convMat2(0, 1) = SOBEL_CONV2_0_1;
    convMat2(0, 2) = SOBEL_CONV2_0_2;
    convMat2(1, 0) = SOBEL_CONV2_1_0;
    convMat2(1, 1) = SOBEL_CONV2_1_1;
    convMat2(1, 2) = SOBEL_CONV2_1_2;
    convMat2(2, 0) = SOBEL_CONV2_2_0;
    convMat2(2, 1) = SOBEL_CONV2_2_1;
    convMat2(2, 2) = SOBEL_CONV2_2_2;
    convMat2 *= SOBEL_CONV_CONST;

    Matrix res = convolution(image, convMat1) + convolution(image, convMat2);
    limitVals(res);
    return res;
}