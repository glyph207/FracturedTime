#ifndef FRACTUREDTIME_H
#define FRACTUREDTIME_H

#include <cmath>
#include <stdint.h>
#include <chrono>
#include <tuple>
#include <utility>
#include <type_traits>

#define REGISTER_FRACTURED_TIME(type, value)\
using T = type;\
using Time = FracturedTime<type, value>;\
Time operator"" _th(unsigned long long int t)\
{\
    return Time::fromTH(static_cast<T>(t));\
}\
Time operator"" _tl(unsigned long long int t)\
{\
    return Time::fromTL(static_cast<T>(t));\
}

template <typename T, T maxTl>
class FracturedTime
{
using TH = std::chrono::duration<double, std::milli>;
using TL = std::chrono::duration<double, std::ratio<1, maxTl * TH::period::den>>;

public:
    FracturedTime() = default;
    FracturedTime(const T th, const T tl);

    static FracturedTime fromTH(const T th);
    static FracturedTime fromTL(const T tl);
    static FracturedTime fromMs(const double t);

    void setTH(const T val);
    void setTL(const T val);
    void setTHTL(const T th, const T tl);

    T getTh() const;
    T getTl() const;

    double timeMS() const;

    FracturedTime<T, maxTl> operator + (const FracturedTime<T, maxTl> &right) const;
    FracturedTime<T, maxTl> operator - (const FracturedTime<T, maxTl> &right) const;
    FracturedTime<T, maxTl> &operator +=(const FracturedTime &right);
    FracturedTime<T, maxTl> &operator -=(const FracturedTime &right);
    bool operator ==(const FracturedTime &right) const;
    bool operator !=(const FracturedTime &right) const;
    bool operator  <(const FracturedTime &right) const;
    bool operator  >(const FracturedTime &right) const;
    bool operator <=(const FracturedTime &right) const;
    bool operator >=(const FracturedTime &right) const;

private:
    TH th;
    TL tl;

    void recalculate();
    static std::pair<TH, TL> thtlFromMs(const double val);
};

template<typename T, T maxTl>
FracturedTime<T, maxTl>::FracturedTime(const T th, const T tl)
    : th{th}
    , tl{tl}
{
    recalculate();
}

template<typename T, T maxTl>
FracturedTime<T, maxTl> FracturedTime<T, maxTl>::fromTH(const T th)
{
    return {th, 0};
}

template<typename T, T maxTl>
FracturedTime<T, maxTl> FracturedTime<T, maxTl>::fromTL(const T tl)
{
    return {0, tl};
}

template<typename T, T maxTl>
FracturedTime<T, maxTl> FracturedTime<T, maxTl>::fromMs(const double ms)
{
    const auto thtl = thtlFromMs(ms);
    return {thtl.first, thtl.second};
}

template<typename T, T maxTl>
void FracturedTime<T, maxTl>::setTH(const T val)
{
    th = val;
}

template<typename T, T maxTl>
void FracturedTime<T, maxTl>::setTL(const T val)
{
    tl = val;
    recalculate();
}

template<typename T, T maxTl>
void FracturedTime<T, maxTl>::setTHTL(const T th, const T tl)
{
    this->th = th;
    this->tl = tl;
    recalculate();
}

template<typename T, T maxTl>
T FracturedTime<T, maxTl>::getTh() const
{
    return static_cast<T>(std::round(th.count()));
}

template<typename T, T maxTl>
T FracturedTime<T, maxTl>::getTl() const
{
    return static_cast<T>(std::round(tl.count()));
}

template<typename T, T maxTl>
double FracturedTime<T, maxTl>::timeMS() const
{
    return std::chrono::duration_cast<TH>(th + tl).count();
}

template<typename T, T maxTl>
FracturedTime<T, maxTl> FracturedTime<T, maxTl>::operator +(const FracturedTime<T, maxTl> &right) const
{
    return {this->getTh() + right.getTh(), this->getTl() + right.getTl()};
}

template<typename T, T maxTl>
FracturedTime<T, maxTl> FracturedTime<T, maxTl>::operator -(const FracturedTime<T, maxTl> &right) const
{
    if (!std::is_signed<T>::value) {
        if (*this < right) {
            return {0, 0};
        }
    }
    if (tl < right.tl) { // TODO find better way
        const T rtl = right.getTl();
        T ctl = getTl();
        T cth = getTh();
        while (ctl < rtl) {
            ctl += maxTl;
            cth -= 1;
        }
        return {cth, ctl - rtl};
    }
    return {this->getTh() - right.getTh(), this->getTl() - right.getTl()};
}

template<typename T, T maxTl>
FracturedTime<T, maxTl> &FracturedTime<T, maxTl>::operator +=(const FracturedTime &right)
{
    *this = *this + right;
    return *this;
}

template<typename T, T maxTl>
FracturedTime<T, maxTl> &FracturedTime<T, maxTl>::operator -=(const FracturedTime &right)
{
    *this = *this - right;
    return *this;
}

template<typename T, T maxTl>
bool FracturedTime<T, maxTl>::operator ==(const FracturedTime &right) const
{
    return std::tie(th, tl) == std::tie(right.th, right.tl);
}

template<typename T, T maxTl>
bool FracturedTime<T, maxTl>::operator !=(const FracturedTime &right) const
{
    return !(*this == right);
}

template<typename T, T maxTl>
bool FracturedTime<T, maxTl>::operator >(const FracturedTime &right) const
{
    return std::tie(this->th, this->tl) > std::tie(right.th, right.tl);
}

template<typename T, T maxTl>
bool FracturedTime<T, maxTl>::operator >=(const FracturedTime &right) const
{
    return !(*this < right);
}

template<typename T, T maxTl>
bool FracturedTime<T, maxTl>::operator <(const FracturedTime &right) const
{
    return std::tie(th, tl) < std::tie(right.th, right.tl);
}

template<typename T, T maxTl>
bool FracturedTime<T, maxTl>::operator <=(const FracturedTime &right) const
{
    return !(*this > right);
}

template<typename T, T maxTl>
void FracturedTime<T, maxTl>::recalculate()
{
    if (tl.count() < maxTl) {
        return;
    }
    const auto newThTl = thtlFromMs(std::chrono::duration_cast<TH>(tl).count());
    th += newThTl.first;
    tl = newThTl.second;
}

template<typename T, T maxTl>
std::pair<typename FracturedTime<T, maxTl>::TH,
          typename FracturedTime<T, maxTl>::TL>
                   FracturedTime<T, maxTl>::thtlFromMs(const double val)
{
    double integer = 0;
    const double fractional = std::modf(val, &integer);
    return std::make_pair(TH{integer}, std::chrono::duration_cast<TL>(TH{fractional}));
}

#endif // FRACTUREDTIME_H
