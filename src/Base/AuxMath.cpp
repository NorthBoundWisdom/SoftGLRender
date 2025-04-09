#include "Base/AuxMath.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <vector>

namespace SoftGL
{

const float k_float_eps = std::numeric_limits<float>::epsilon();
const double k_dbl_eps = std::numeric_limits<double>::epsilon();

bool absEqual(double d1, double d2, double eps)
{
    return std::abs(d1 - d2) <= eps;
}

bool relEqual(double d1, double d2, double precision)
{
    return std::abs(d1 - d2) <= precision * std::max(std::abs(d1), std::abs(d2));
}

bool absEqual(float f1, float f2, float eps)
{
    return std::abs(f1 - f2) <= eps;
}

bool relEqual(float f1, float f2, float precision)
{
    return std::abs(f1 - f2) <= precision * std::max(std::abs(f1), std::abs(f2));
}
bool lessThan(float f1, float f2, float eps)
{
    return f1 < f2 - eps;
}
bool lessThan(double d1, double d2, double eps)
{
    return d1 < d2 - eps;
}
bool greaterThan(float f1, float f2, float eps)
{
    return f1 > f2 + eps;
}
bool greaterThan(double d1, double d2, double eps)
{
    return d1 > d2 + eps;
}
bool lessThanEqual(float f1, float f2, float eps)
{
    return !greaterThan(f1, f2, eps);
}
bool lessThanEqual(double d1, double d2, double eps)
{
    return !greaterThan(d1, d2, eps);
}
bool greaterThanEqual(float f1, float f2, float eps)
{
    return !lessThan(f1, f2, eps);
}
bool greaterThanEqual(double d1, double d2, double eps)
{
    return !lessThan(d1, d2, eps);
}

// Implement other methods similarly, accessing the value via pimpl->value
/*
 * trailingZeroTable[i] is the number of trailing zero bits in the binary
 * representation of i.
 *
 * The `trailingZeroTable` is a lookup table used to quickly find the number of
 * trailing zeros in the binary representation of a number from 0 to 255.
 *
 * In the context of the `binaryGcd` function, it's used to normalize the input
 * numbers `a` and `b` by removing all trailing zeros, which is equivalent to
 * dividing by 2 until the number is odd. This is a common step in the binary
 * GCD algorithm.
 *
 * The binary GCD algorithm is an efficient variant of the Euclidean algorithm
 * for finding the greatest common divisor (GCD) of two numbers. It avoids
 * expensive division operations by using bitwise operations instead.
 *
 * The `trailingZeroTable` lookup table speeds up the process of finding the
 * number of trailing zeros, which would otherwise require a loop or recursive
 * function.
 */
// The trailingZeroTable as provided
static const std::vector<long long> trailingZeroTable = {
    -25, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2,
    0,   1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0,
    1,   0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1,
    0,   3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0,
    2,   0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
    0,   1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0,
    1,   0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1,
    0,   2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0,
    3,   0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};

// Function to compute the greatest common divisor using binary GCD algorithm
long long binaryGcd(long long a, long long b)
{
    // Handle absolute values to avoid negative results
    a = std::abs(a);
    b = std::abs(b);

    if (b == 0)
        return a;
    if (a == 0)
        return b;

    long long x;
    long long aZeros = 0;
    while ((x = a & 0xff) == 0)
    {
        a >>= 8;
        aZeros += 8;
    }
    long long y = trailingZeroTable[static_cast<unsigned char>(x)];
    aZeros += y;
    a >>= y;

    long long bZeros = 0;
    while ((x = b & 0xff) == 0)
    {
        b >>= 8;
        bZeros += 8;
    }
    y = trailingZeroTable[static_cast<unsigned char>(x)];
    bZeros += y;
    b >>= y;

    long long t = (std::min(aZeros, bZeros));

    while (a != b)
    {
        if (a > b)
        {
            a -= b;

            while ((x = a & 0xff) == 0)
                a >>= 8;
            a >>= trailingZeroTable[static_cast<unsigned char>(x)];
        }
        else
        {
            b -= a;

            while ((x = b & 0xff) == 0)
                b >>= 8;
            b >>= trailingZeroTable[static_cast<unsigned char>(x)];
        }
    }

    long long res = a << t;
    assert(res >= 0);
    return res;
}

long long roundAwayFromZero(double x, double epsilon)
{
    if (std::abs(x) < epsilon)
    {
        return 0;
    }
    return x > 0 ? static_cast<long long>(std::ceil(x - epsilon)) :
                   static_cast<long long>(std::floor(x + epsilon));
}

} // namespace SoftGL
