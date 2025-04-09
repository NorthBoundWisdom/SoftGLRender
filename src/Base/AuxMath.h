#ifndef AUXMATH_H
#define AUXMATH_H

#include <cmath>
#include <utility>

namespace SoftGL
{
constexpr double k_E = 2.71828182845904523536;        // e
constexpr double k_LOG2E = 1.44269504088896340736;    // log2(e)
constexpr double k_LOG10E = 0.434294481903251827651;  // log10(e)
constexpr double k_LN2 = 0.693147180559945309417;     // ln(2)
constexpr double k_LN10 = 2.30258509299404568402;     // ln(10)
constexpr double k_PI = 3.14159265358979323846;       // pi
constexpr double k_PI_2 = 1.57079632679489661923;     // pi/2
constexpr double k_PI_4 = 0.785398163397448309616;    // pi/4
constexpr double k_2_PI = 6.28318530717958647693;     // 2*pi
constexpr double k_SQRT2 = 1.41421356237309504880;    // sqrt(2)
constexpr double k_SQRT2_2 = 0.707106781186547524401; // 1/sqrt(2)

constexpr double k_eps = 1e-15;
constexpr float k_feps = 1e-6f;
extern const float k_float_eps;
extern const double k_dbl_eps;

inline bool isZero(float f, float eps = k_feps)
{
    return std::abs(f) <= eps;
}

inline bool isZero(double d, double eps = k_eps)
{
    return std::abs(d) <= eps;
}

bool absEqual(float f1, float f2, float eps = k_feps);
bool absEqual(double d1, double d2, double eps = k_eps);
bool relEqual(float f1, float f2, float precision = k_float_eps);
bool relEqual(double d1, double d2, double precision = k_dbl_eps);
bool lessThan(float f1, float f2, float eps = k_float_eps);
bool lessThan(double d1, double d2, double eps = k_dbl_eps);
bool greaterThan(float f1, float f2, float eps = k_float_eps);
bool greaterThan(double d1, double d2, double eps = k_dbl_eps);
bool lessThanEqual(float f1, float f2, float eps = k_float_eps);
bool lessThanEqual(double d1, double d2, double eps = k_dbl_eps);
bool greaterThanEqual(float f1, float f2, float eps = k_float_eps);
bool greaterThanEqual(double d1, double d2, double eps = k_dbl_eps);

template <typename T, typename U>
static constexpr bool cmpEqual(T t, U u) noexcept
{
    return std::cmp_equal(t, u);
}

template <typename T, typename U>
static constexpr bool cmpNotEqual(T t, U u) noexcept
{
    return std::cmp_not_equal(t, u);
}

template <typename T, typename U>
static constexpr bool cmpLess(T t, U u) noexcept
{
    return std::cmp_less(t, u);
}

template <typename T, typename U>
static constexpr bool cmpGreater(T t, U u) noexcept
{
    return std::cmp_greater(t, u);
}

template <typename T, typename U>
static constexpr bool cmpLessEqual(T t, U u) noexcept
{
    return std::cmp_less_equal(t, u);
}

template <typename T, typename U>
static constexpr bool cmpGreaterEqual(T t, U u) noexcept
{
    return std::cmp_greater_equal(t, u);
}

template <typename R, typename T>
static constexpr bool inRange(T t) noexcept
{
    return std::in_range<T>(t);
}

/**
 * Maps a value from one range to another.
 *
 * Given a value `val`, this function maps it from the original range `[omin,
 * omax]` to the new range `[nmin, nmax]`. The function returns the mapped
 * value.
 *
 * @param val The value to map.
 * @param omin The minimum value of the original range.
 * @param omax The maximum value of the original range.
 * @param nmin The minimum value of the new range.
 * @param nmax The maximum value of the new range.
 * @return The mapped value.
 */
template <typename T, typename T1, typename T2, typename T3, typename T4>
static double mappedValue(T val, T1 omin, T2 omax, T3 nmin, T4 nmax)
{
    const auto dist1 = static_cast<double>(omax - omin);
    const auto dist2 = nmax - nmin;
    const auto distVal = val - omin;
    return ((distVal * dist2) / dist1) + nmin;
}

template <typename T, typename T1, typename T2>
static double toPercent(T val, T1 omin, T2 omax)
{
    return mappedValue(val, omin, omax, 0, 100);
}

template <typename T, typename U>
T lerp(T a, T b, U t)
{
    return std::lerp(a, b, t);
}

/** Calculate GCD of a and b interpreted as unsigned integers. */
long long binaryGcd(long long a, long long b);

long long roundAwayFromZero(double x, double epsilon);

} // namespace SoftGL
#endif
