/*
  Permissive BSD 2-Clause License - see LICENSE file for details
  by Marko 'Fador' Viitanen 2023
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <random>
#include <chrono>
#include <stdexcept>
#include <functional>


#define VERSION "0.1.0"

class BigInt
{
public:
  
  std::string getVersion() {
    return VERSION;
  }

  BigInt() : bits(1, false) {}

  BigInt(const std::string &str)
  {
    bits.assign(1, false); // Initialize to 0
    if (str.empty() || str == "0" || str == "-0") {
        return;
    }

    std::string s = str;
    bool is_neg = false;
    if (s[0] == '-') {
        is_neg = true;
        s = s.substr(1);
    }

    if (s.length() < 9) { // Use stoll for small strings, it's faster
        long long num = std::stoll(s);
        *this = BigInt(num);
    } else { // Divide and conquer for large strings
        size_t k = s.length() / 2;
        std::string left_s = s.substr(0, s.length() - k);
        std::string right_s = s.substr(s.length() - k);

        BigInt left(left_s);
        BigInt right(right_s);

        BigInt ten_k = BigInt(10).pow(k);

        *this = left * ten_k + right;
    }

    sign = is_neg;
    trim();
  }
  BigInt(int num)
  {
    *this = BigInt((long long int)num);
  }

  BigInt &operator=(const BigInt &rhs)
  {
    if (&rhs == this)
      return *this;
    bits = rhs.bits;
    sign = rhs.sign;
    return *this;
  }

  BigInt &operator=(const int &rhs)
  {
    *this = BigInt(rhs);
    return *this;
  }

  BigInt(long long int num)
  {
    if (num == 0) {
      bits.push_back(false);
      return;
    }
    if (num < 0) {
      sign = true;
      num = -num;
    }
    while (num > 0) {
      bits.push_back(num % 2);
      num /= 2;
    }
  }

  BigInt operator+(const BigInt &rhs) const
  {
    BigInt result = *this;
    result += rhs;
    return result;
  }

  BigInt operator+(const int &rhs) const
  {
    return *this + BigInt(rhs);
  }

  BigInt operator*(const BigInt &rhs) const
  {
    if (!(*this) || !rhs) {
        return BigInt(0);
    }

    BigInt result = this->abs().karatsubaMultiply(rhs.abs());
    result.sign = (sign != rhs.sign);
    result.trim();
    return result;
  }

  BigInt karatsubaMultiply(const BigInt &rhs) const
  {
    size_t n = std::max(bits.size(), rhs.bits.size());

    if (n < 32) { // Base case for small numbers
        BigInt result;
        if (!(*this) || !rhs) return result;

        for (size_t i = 0; i < rhs.bits.size(); ++i) {
            if (rhs.bits[i]) {
                result += (*this << (int)i);
            }
        }
        return result;
    }

    size_t k = n / 2;

    BigInt high1, low1, high2, low2;

    if (bits.size() > k) {
        low1.bits.assign(bits.begin(), bits.begin() + k);
        high1.bits.assign(bits.begin() + k, bits.end());
    } else {
        low1 = *this;
    }

    if (rhs.bits.size() > k) {
        low2.bits.assign(rhs.bits.begin(), rhs.bits.begin() + k);
        high2.bits.assign(rhs.bits.begin() + k, rhs.bits.end());
    } else {
        low2 = rhs;
    }

    high1.trim(); low1.trim(); high2.trim(); low2.trim();

    BigInt z0 = low1.karatsubaMultiply(low2);
    BigInt z2 = high1.karatsubaMultiply(high2);
    BigInt z1 = (low1 + high1).karatsubaMultiply(low2 + high2);

    BigInt temp = z1 - z2 - z0;

    return (z2 << (2 * (int)k)) + (temp << (int)k) + z0;
  }

  BigInt &operator*=(const BigInt &rhs)
  {
    *this = *this * rhs;
    return *this;
  }

  bool operator==(const BigInt &rhs) const
  {
    if (sign != rhs.sign) return false;
    if (bits.size() != rhs.bits.size()) return false;
    return bits == rhs.bits;
  }

  BigInt &operator+=(const BigInt &rhs)
  {
    if (sign == rhs.sign) {
        // Same sign addition: a + b
        bool carry = false;
        size_t n = std::max(bits.size(), rhs.bits.size());
        bits.resize(n, false);
        for (size_t i = 0; i < n; ++i) {
            bool rhs_bit = (i < rhs.bits.size()) ? rhs.bits[i] : false;
            bool sum = bits[i] ^ rhs_bit ^ carry;
            carry = (bits[i] && rhs_bit) || (bits[i] && carry) || (rhs_bit && carry);
            bits[i] = sum;
        }
        if (carry) {
            bits.push_back(true);
        }
    } else {
        // Different signs: a - b or b - a
        if (this->abs() >= rhs.abs()) {
            // |a| >= |b|, result sign is sign of a.
            // Perform |a| - |b|.
            BigInt rhs_abs = rhs.abs();
            bool borrow = false;
            for (size_t i = 0; i < bits.size(); ++i) {
                bool rhs_bit = (i < rhs_abs.bits.size()) ? rhs_abs.bits[i] : false;
                bool diff = bits[i] ^ rhs_bit ^ borrow;
                borrow = (!bits[i] && borrow) || (!bits[i] && rhs_bit) || (borrow && rhs_bit);
                bits[i] = diff;
            }
        } else {
            // |a| < |b|, result sign is sign of b.
            // Perform |b| - |a|.
            BigInt this_abs = this->abs();
            bits = rhs.bits; // start with |b|
            sign = rhs.sign;

            bool borrow = false;
            for (size_t i = 0; i < bits.size(); ++i) {
                bool lhs_bit = (i < this_abs.bits.size()) ? this_abs.bits[i] : false;
                bool diff = bits[i] ^ lhs_bit ^ borrow;
                borrow = (!bits[i] && borrow) || (!bits[i] && lhs_bit) || (borrow && lhs_bit);
                bits[i] = diff;
            }
        }
    }
    trim();
    return *this;
  }

  bool operator==(const int &rhs) const
  {
    return *this == BigInt(rhs);
  }

  std::pair<BigInt, BigInt> divmod(const BigInt &divisor) const
  {
    if (divisor == 0) {
        throw std::invalid_argument("Division by zero");
    }
    if (*this == 0) {
        return {BigInt(0), BigInt(0)};
    }

    BigInt dividend_abs = this->abs();
    BigInt divisor_abs = divisor.abs();

    if (dividend_abs < divisor_abs) {
        return {BigInt(0), *this};
    }

    BigInt quotient;
    BigInt remainder;

    quotient.bits.assign(dividend_abs.bits.size(), false);

    for (int i = dividend_abs.bits.size() - 1; i >= 0; i--) {
        remainder <<= 1;
        remainder.bits[0] = dividend_abs.bits[i];
        if (remainder >= divisor_abs) {
            remainder -= divisor_abs;
            quotient.bits[i] = true;
        }
    }

    quotient.sign = (this->sign != divisor.sign);
    quotient.trim();

    remainder.sign = this->sign;
    remainder.trim();

    return {quotient, remainder};
  }

  BigInt operator%(const BigInt &divisor) const
  {
    return divmod(divisor).second;
  }

  BigInt operator%(const int &rhs) const
  {
    return *this % BigInt(rhs);
  }

  BigInt &operator%=(const BigInt &rhs)
  {
    *this = *this % rhs;
    return *this;
  }

  operator bool() const
  {
    return bits.size() > 1 || (bits.size() == 1 && bits[0]);
  }

  BigInt operator/(const BigInt &rhs) const
  {
    return divmod(rhs).first;
  }

  // Helper function to trim leading zeros
  void trim()
  {
    while (bits.size() > 1 && bits.back() == false)
    {
      bits.pop_back();
    }
    if (bits.size() == 1 && bits[0] == false) {
        sign = false; // Canonical representation for 0
    }
  }

  bool operator<=(const BigInt &rhs) const
  {
    return (*this < rhs) || (*this == rhs);
  }

  bool operator<=(const int &rhs) const
  {
    return *this <= BigInt(rhs);
  }

  BigInt operator/(const int &rhs) const
  {
    return *this / BigInt(rhs);
  }

  BigInt &operator++()
  {
    *this += 1;
    return *this;
  }

  BigInt operator++(int)
  {
    BigInt temp = *this;
    *this += 1;
    return temp;
  }

  bool operator>=(const BigInt &rhs) const
  {
    return !(*this < rhs);
  }

  BigInt abs() const
  {
    BigInt result = *this;
    result.sign = false;
    return result;
  }

  bool operator!=(const BigInt &rhs) const
  {
    return !(*this == rhs);
  }

  bool operator!=(const int &rhs) const
  {
    return !(*this == BigInt(rhs));
  }

  bool operator>(const BigInt &rhs) const
  {
    return !(*this <= rhs);
  }

  bool operator>(const int &rhs) const
  {
    return *this > BigInt(rhs);
  }

  BigInt operator&(const BigInt &rhs) const
  {
    BigInt result;
    size_t size = std::max(bits.size(), rhs.bits.size());
    result.bits.resize(size);
    for (size_t i = 0; i < size; ++i) {
        bool this_bit = (i < bits.size()) ? bits[i] : false;
        bool rhs_bit = (i < rhs.bits.size()) ? rhs.bits[i] : false;
        result.bits[i] = this_bit & rhs_bit;
    }
    result.trim();
    return result;
  }

  BigInt operator&(const int &rhs) const
  {
    return (*this & BigInt(rhs));
  }

  BigInt operator*(int rhs) const
  {
    return *this * BigInt(rhs);
  }

  BigInt &operator>>=(int shift)
  {
    if (shift < 0) {
        return (*this) <<= (-shift);
    }
    if (shift == 0 || !(*this)) {
        return *this;
    }
    if (shift >= (int)bits.size()) {
        bits.assign(1, false);
        sign = false;
        return *this;
    }
    bits.erase(bits.begin(), bits.begin() + shift);
    return *this;
  }

  bool operator<(const BigInt &rhs) const
  {
    if (sign != rhs.sign) {
        return sign; // If this is negative (sign==true), it's smaller.
    }

    // Signs are the same
    if (sign) { // Both negative
        // For negative numbers, bigger magnitude means smaller value.
        if (bits.size() != rhs.bits.size()) {
            return bits.size() > rhs.bits.size();
        }
        for (int i = bits.size() - 1; i >= 0; --i) {
            if (bits[i] != rhs.bits[i]) {
                return bits[i];
            }
        }
        return false; // equal
    } else { // Both positive
        if (bits.size() != rhs.bits.size()) {
            return bits.size() < rhs.bits.size();
        }
        for (int i = bits.size() - 1; i >= 0; --i) {
            if (bits[i] != rhs.bits[i]) {
                return !bits[i];
            }
        }
        return false; // equal
    }
  }

  bool operator<(const int &rhs) const
  {
    return *this < BigInt(rhs);
  }

  BigInt operator>>(int shift) const
  {
    BigInt result = *this;
    result >>= shift;
    return result;
  }

  BigInt &operator<<=(int shift)
  {
    if (shift < 0) {
        return (*this) >>= (-shift);
    }
    if (shift == 0 || !(*this)) {
        return *this;
    }
    bits.insert(bits.begin(), shift, false);
    return *this;
  }

  BigInt operator<<(int shift) const
  {
    BigInt result = *this;
    result <<= shift;
    return result;
  }

  BigInt pow(int exponent) const
  {
    if (exponent < 0)
    {
      throw std::invalid_argument("Negative exponent is not allowed for BigInt::pow");
    }

    BigInt base = *this;
    BigInt result = 1;

    while (exponent > 0)
    {
      if (exponent & 1)
      {
        result *= base;
      }
      base *= base;
      exponent >>= 1;
    }

    return result;
  }

  BigInt &operator--()
  {
    *this -= 1;
    return *this;
  }

  BigInt operator--(int)
  {
    BigInt result = *this;
    *this -= 1;
    return result;
  }

  BigInt operator|(const BigInt &rhs) const
  {
    BigInt result;
    size_t size = std::max(bits.size(), rhs.bits.size());
    result.bits.resize(size);
    for (size_t i = 0; i < size; ++i) {
        bool this_bit = (i < bits.size()) ? bits[i] : false;
        bool rhs_bit = (i < rhs.bits.size()) ? rhs.bits[i] : false;
        result.bits[i] = this_bit | rhs_bit;
    }
    result.trim();
    return result;
  }

  BigInt operator||(const BigInt &rhs) const
  {
    return BigInt(0); // placeholder
  }

  BigInt operator-() const
  {
    BigInt result = *this;
    if (*this != 0) result.sign = !sign;
    return result;
  }

  BigInt &operator-=(const BigInt &rhs)
  {
    BigInt temp = rhs;
    if (temp != 0) {
      temp.sign = !temp.sign;
    }
    *this += temp;
    return *this;
  }

  BigInt operator-(const BigInt &rhs) const
  {
    BigInt result = *this;
    result -= rhs;
    return result;
  }

  BigInt operator-(const int &rhs) const
  {
    return *this - BigInt(rhs);
  }

  static BigInt generateRandom(int bitLength)
  {
    if (bitLength <= 0) return BigInt(0);
    std::mt19937 generator(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(0, 1);

    BigInt result;
    result.bits.resize(bitLength);
    for (int i = 0; i < bitLength; ++i) {
        result.bits[i] = distribution(generator);
    }
    // Ensure the most significant bit is 1 to have the desired bit length
    result.bits[bitLength - 1] = true;
    result.trim();
    return result;
  }

  bool millerRabinTest(BigInt d) const {
    BigInt a = 2 + BigInt::generateRandom(bits.size() - 2) % (*this - 4);
    BigInt x = modPow(a, d, *this);

    if (x == 1 || x == *this - 1) {
        return true;
    }

    while (d != *this - 1) {
        x = (x * x) % *this;
        d <<= 1;
        if (x == 1) return false;
        if (x == *this - 1) return true;
    }

    return false;
  }

  bool isPrime(int k = 20) const {
    if (*this <= 1 || *this == 4) return false;
    if (*this <= 3) return true;
    if (this->isEven()) return false;

    BigInt d = *this - 1;
    while (d.isEven()) {
        d >>= 1;
    }

    for (int i = 0; i < k; i++) {
        if (!millerRabinTest(d)) return false;
    }

    return true;
  }

  static BigInt generatePrime(int bitLength)
  {
    BigInt candidate = BigInt::generateRandom(bitLength);

    // Make sure the candidate is odd.
    if (candidate.isEven()) {
      candidate += 1;
    }

    while (!candidate.isPrime())
    {
      candidate += 2;
    }

    return candidate;
  }

  bool isEven() const
  {
    return (bits.size() > 0 && !bits[0]);
  }

  static BigInt gcd(BigInt a, BigInt b)
  {
    a = a.abs();
    b = b.abs();

    if (a == 0) return b;
    if (b == 0) return a;

    int shift = 0;
    while (a.isEven() && b.isEven()) {
      a >>= 1;
      b >>= 1;
      shift++;
    }
    while (a.isEven()) {
      a >>= 1;
    }
    do {
      while (b.isEven()) {
        b >>= 1;
      }
      if (a > b) {
        std::swap(a, b);
      }
      b -= a;
    } while (b != 0);

    return a << shift;
  }

  static BigInt modPow(BigInt base, BigInt exp, BigInt modulus)
  {
    if (modulus == 1) return 0;
    BigInt result = 1;
    base %= modulus;
    while (exp > 0) {
        if (!exp.isEven()) result = (result * base) % modulus;
        base = (base * base) % modulus;
        exp >>= 1;
    }
    return result;
  }

  static BigInt modInverse(BigInt a, BigInt p)
  {
    BigInt val(0);
    BigInt nt(1);
    BigInt r(p);
    BigInt nr(a.abs());

    while (nr != 0)
    {
      auto dm = r.divmod(nr);
      BigInt q = dm.first;

      BigInt tmp = nt;
      nt = val - q * nt;
      val = tmp;

      tmp = nr;
      nr = dm.second;
      r = tmp;
    }

    if (r > 1) {
        return BigInt(0); // No modular inverse exists
    }

    if (val < 0)
      val += p;

    return val;
  }

  friend std::ostream &operator<<(std::ostream &os, const BigInt &bi);

private:
  bool sign = false;      // false = positive, true = negative
  std::vector<bool> bits; // bits are stored in reverse order, LSB first
};

std::ostream &operator<<(std::ostream &os, const BigInt &bi)
{
    if (!bi) {
        os << "0";
        return os;
    }
    if (bi.sign) {
        os << "-";
    }

    std::function<std::string(const BigInt&)> to_string_rec =
        [&](const BigInt& n) -> std::string {
        if (n.bits.size() < 60) {
            long long val = 0;
            for(size_t i = 0; i < n.bits.size(); ++i) {
                if (n.bits[i]) {
                    val |= (1LL << i);
                }
            }
            return std::to_string(val);
        }

        size_t num_digits_approx = (n.bits.size() * 1000) / 3322 + 1;
        size_t k = num_digits_approx / 2;
        if (k == 0) k = 1;

        BigInt ten_k = BigInt(10).pow(k);
        auto dm = n.divmod(ten_k);

        std::string r_str = to_string_rec(dm.second);

        if (dm.first == 0) {
            return r_str;
        }

        std::string q_str = to_string_rec(dm.first);

        while (r_str.length() < k) {
            r_str.insert(0, 1, '0');
        }

        return q_str + r_str;
    };

    os << to_string_rec(bi.abs());
    return os;
}
