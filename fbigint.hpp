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


#define VERSION "0.1.0"

class BigInt
{
public:
  
  std::string getVersion() {
    return VERSION;
  }

  BigInt() : digits(1, 0) {}

  BigInt(const std::string &str)
  {
    for (int i = str.size() - 1; i >= 0; --i)
    {
      if (str[i] >= '0' && str[i] <= '9')
      {
        digits.push_back(str[i] - '0');
      }
      else
      {
        if (i == 0 && str[i] == '-')
        {
          // Negative number
          sign = true;
          continue;
        }
        // Invalid digit
        digits.clear();
        digits.push_back(0);
        return;
      }
    }
  }
  BigInt(int num)
  {
    *this = BigInt(std::to_string(num));
  }

  BigInt &operator=(const BigInt &rhs)
  {
    // Self-assignment detection
    if (&rhs == this)
      return *this;

    // Release any resource we're holding
    // No resource to release in this example

    // Copy the values from rhs
    digits = rhs.digits;
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
    *this = BigInt(std::to_string(num));
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

    if (rhs == 0 || *this == 0)
      return 0;

    // ToDo: Use Karatsuba multiplication for large numbers    
    if (false && digits.size() > 16 && rhs.digits.size() > 16)
    {
      std::cout << "Karatsuba multiplication" << std::endl;
      return karatsubaMultiply(rhs);
    }

    BigInt result;
    result.digits.resize(digits.size() + rhs.digits.size());

    for (size_t i = 0; i < digits.size(); ++i)
    {
      for (size_t j = 0; j < rhs.digits.size(); ++j)
      {
        result.digits[i + j] += digits[i] * rhs.digits[j];
        if (result.digits[i + j] >= 10)
        {
          result.digits[i + j + 1] += result.digits[i + j] / 10;
          result.digits[i + j] %= 10;
        }
      }
    }

    while (result.digits.size() > 1 && result.digits.back() == 0)
    {
      result.digits.pop_back();
    }

    // Handle result sign, if either of the operands is negative, result is negative
    result.sign = (sign != rhs.sign);

    return result;
  }

  BigInt karatsubaMultiply(const BigInt &rhs) const
  {
    // Compute the size of the numbers
    size_t n = std::max(digits.size(), rhs.digits.size());

    // Base case: if the numbers are small enough, use the naive algorithm
    /*if (n < 16)
    {
      return *this * rhs;
    }*/

    // Split the numbers into two halves
    size_t k = n / 2;
    BigInt high1, low1, high2, low2;
    if (digits.size() > k)
    {
      high1.digits = std::vector<int8_t>(digits.begin(), digits.end() - k);
      low1.digits = std::vector<int8_t>(digits.end() - k, digits.end());
    }
    else
    {
      high1 = 0;
      low1 = *this;
    }
    if (rhs.digits.size() > k)
    {
      high2.digits = std::vector<int8_t>(rhs.digits.begin(), rhs.digits.end() - k);
      low2.digits = std::vector<int8_t>(rhs.digits.end() - k, rhs.digits.end());
    }
    else
    {
      high2 = 0;
      low2 = rhs;
    }

    // Compute the three intermediate products
    BigInt z0 = low1 * low2;
    BigInt z1 = (low1 + high1) * (low2 + high2);
    BigInt z2 = high1 * high2;

    // Compute the result using the three intermediate products
    BigInt result = z2;
    result.digits.resize(2 * k + z1.digits.size());
    for (size_t i = 0; i < z1.digits.size(); ++i)
    {
      result.digits[k + i] += z1.digits[i];
    }
    for (size_t i = 0; i < z0.digits.size(); ++i)
    {
      result.digits[i] += z0.digits[i];
    }
    for (size_t i = 0; i < z2.digits.size(); ++i)
    {
      result.digits[2 * k + i] -= z2.digits[i];
    }
    for (size_t i = 0; i < result.digits.size() - 1; ++i)
    {
      if (result.digits[i] < 0)
      {
        result.digits[i] += 10;
        result.digits[i + 1] -= 1;
      }
      else if (result.digits[i] >= 10)
      {
        result.digits[i] -= 10;
        result.digits[i + 1] += 1;
      }
    }
    while (result.digits.size() > 1 && result.digits.back() == 0)
    {
      result.digits.pop_back();
    }

    // Handle result sign
    if (sign != rhs.sign)
    {
      result.sign = true;
    }
    else
    {
      result.sign = false;
    }

    return result;
  }

  BigInt &operator*=(const BigInt &rhs)
  {
    *this = *this * rhs;
    return *this;
  }

  bool operator==(const BigInt &rhs) const
  {

    // If the signs are different, then the numbers are different
    if (sign != rhs.sign) return false;

    if (digits.size() != rhs.digits.size())
    {
      return false;
    }
    for (size_t i = 0; i < digits.size(); ++i)
    {
      if (digits[i] != rhs.digits[i])
      {
        return false;
      }
    }

    // If we get here, then the numbers are equal
    return true;
  }

  BigInt &operator+=(const BigInt &rhs)
  {
    // std::cout << "\r\nBefore: " << *this << "+" << rhs << std::endl;

    // Different signs need special handling
    if (sign != rhs.sign)
    {
      // rhs larger than this
      if ((*this).abs() < rhs.abs())
      {

        // If *this is negative, then we need to subtract *this from rhs and set the sign to positive
        if (sign)
        {
          BigInt temp = rhs.abs();
          temp -= (*this).abs();
          *this = temp;
          sign = false;
        }
        else // if *this is positive
        {
          *this = rhs.abs() - (*this).abs();
          sign = !sign;
        }
        return *this;
      }
      else
      {
        if (sign)
        {
          *this = (*this).abs() - rhs.abs();
          sign = true;
          return *this;
        }
        else
        {
          *this -= (*this).abs();
          return *this;
        }
      }
    }

    size_t size = std::max(digits.size(), rhs.digits.size());
    digits.resize(size, 0);

    int carry = 0;
    for (size_t i = 0; i < size || carry; ++i)
    {
      if (i == digits.size())
      {
        digits.push_back(0);
      }
      // Add the two digits together, plus the carry, limit to rhs size
      digits[i] += carry + (i < rhs.digits.size() ? rhs.digits[i] : 0);
      
      // If the digit is greater than 10, then we need to carry the rest
      carry = (digits[i] >= 10);      

      if (carry)
      {
        digits[i] -= 10;
      }
    }
    
    return *this;
  }

  // Check if BigInt and int are equal
  bool operator==(const int &rhs) const
  {
    return *this == BigInt(rhs);
  }

  std::pair<BigInt, BigInt> divmod(const BigInt &divisor) const
  {
    BigInt quotient = *this / divisor;
    BigInt product = quotient * divisor;
    BigInt remainder = *this - product;
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
    for (size_t i = 0; i < digits.size(); ++i)
    {
      if (digits[i] != 0)
      {
        return true;
      }
    }
    return false;
  }

  BigInt operator/(const BigInt &rhs) const
  {
    if (rhs == 0)
    {
      throw std::invalid_argument("Division by zero");
    }

    // std::cout << "Dividing " << *this << " by " << rhs << std::endl;

    BigInt dividend = (*this).abs();
    BigInt divisor = rhs.abs();
    BigInt quotient;

    // Resize quotient.digits to hold the result
    quotient.digits.resize(dividend.digits.size());

    // Find the highest power of 2 of the divisor that is less than the dividend
    int shift = 0;
    while (dividend > divisor)
    {
      divisor <<= 1;
      shift++;
    }
    // We already went one too far, so shift back
    if (shift > 0)
    {
      shift--;
    }

    // Restore divisor
    divisor = rhs.abs();

    // Our current value is the divisor * 2^shift
    BigInt current = divisor * (BigInt(1) << shift);

    // Currently we have fit the divisor into the dividend 2^shift times
    BigInt count = BigInt(1) << shift;

    // std::cout << "Count: " << count << std::endl;

    // Start testing from the highest power of 2 of the divisor that is less than the dividend
    int testShift = shift;

    for (int i = testShift; i >= 0; i--)
    {
      BigInt test = divisor * (BigInt(1) << i);
      
      // If we canfit the test into the current value
      // then add it to the current value and add 2^i to the count
      if (current + test < dividend)
      {
        current += test;
        count += (BigInt(1) << i);
      }
    }

    // Finally, add divisor to current value until we reach the dividend
    while (current <= dividend)
    {
      current += divisor;
      count++;
    }

    // We went one too far, so subtract one
    if (count > 0)
    {
      count--;
    }

    // Handle signs, same signs result in positive, different signs result in negative
    count.sign = (sign != rhs.sign);

    return count;
  }

  // Helper function to prepend a digit at the beginning of the BigInt
  void prependDigit(int digit)
  {
    digits.insert(digits.begin(), digit);
  }

  // Helper function to trim leading zeros
  void trim()
  {
    while (digits.size() > 1 && digits.back() == 0)
    {
      digits.pop_back();
    }
  }

  bool operator<=(const BigInt &rhs) const
  {
    // A number is less than or equal to another if it's less than the other or it's equal to the other
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
    // Adding one to a negative number is the same as subtracting one from the absolute value
    if (sign)
    {      
      *this = (*this).abs()-1;
      if(*this != 0) sign = true; // Make sure the sign is still negative (or we reached zero, which is positive)
      return *this;
    }

    for (size_t i = 0; i < digits.size(); ++i)
    {
      if (++digits[i] <= 9)
      {
        break;
      }
      digits[i] = 0;
      if (i + 1 == digits.size())
      {
        digits.push_back(1);
        break;
      }
    }
    return *this;
  }

  BigInt operator++(int)
  {

    BigInt temp = *this;

    // Adding one to a negative number is the same as subtracting one from the absolute value
    if (sign)
    {      
      *this = (*this).abs()-1;
      if(*this != 0) sign = true; // Make sure the sign is still negative (or we reached zero, which is positive)
      return temp;
    }

    for (size_t i = 0; i < digits.size(); ++i)
    {
      if (++digits[i] <= 9)
      {
        break;
      }
      digits[i] = 0;
      if (i + 1 == digits.size())
      {
        digits.push_back(1);
        break;
      }
    }
    return temp;
  }

  bool operator>=(const BigInt &rhs) const
  {
    // If the sign is different, then the bigger number is the one with the positive sign
    if (sign != rhs.sign)
    {
      return !sign;
    }

    if (digits.size() != rhs.digits.size())
    {
      return digits.size() > rhs.digits.size();
    }
    for (int i = digits.size() - 1; i >= 0; --i)
    {
      if (digits[i] != rhs.digits[i])
      {
        return digits[i] > rhs.digits[i];
      }
    }
    return true; // They are equal
  }

  // Absolute value is just the same number with a positive sign
  BigInt abs() const
  {
    BigInt result = *this;
    result.sign = false;
    return result;
  }

  // Check if two BigInts are not equal
  bool operator!=(const BigInt &rhs) const
  {
    return !(*this == rhs);
  }

  // Check if BigInt and int are not equal
  bool operator!=(const int &rhs) const
  {
    return !(*this == BigInt(rhs));
  }

  // Check if one BigInt is greater than another
  bool operator>(const BigInt &rhs) const
  {

    if (sign != rhs.sign)
    {
      return !sign;
    }
    // If both numbers are negative, then the bigger number is the one with the smaller absolute value
    if (sign && rhs.sign)
    {
      return rhs.abs() >= abs();
    }

    if (digits.size() != rhs.digits.size())
    {
      return digits.size() > rhs.digits.size();
    }
    for (int i = digits.size() - 1; i >= 0; --i)
    {
      if (digits[i] != rhs.digits[i])
      {
        return digits[i] > rhs.digits[i];
      }
    }
    return false;
  }

  bool operator>(const int &rhs) const
  {
    return *this > BigInt(rhs);
  }

  // Perform bitwise AND operation
  BigInt operator&(const BigInt &rhs) const
  {
    BigInt result;
    result.digits.resize(std::max(digits.size(), rhs.digits.size()), 0);
    for (size_t i = 0; i < result.digits.size(); ++i)
    {
      if (i < digits.size() && i < rhs.digits.size())
      {
        result.digits[i] = digits[i] & rhs.digits[i];
      }
    }
    return result;
  }

  BigInt operator&(const int &rhs) const
  {
    return (*this & BigInt(rhs));
  }

  // Perform multiplication operation
  BigInt operator*(int rhs) const
  {
    return *this * BigInt(rhs);
  }

  // Perform right shift operation
  BigInt &operator>>=(int shift)
  {
    // If the shift is negative, then perform a left shift
    if (shift < 0)
    {
      return (*this) <<= (-shift);
    }
    for (int i = 0; i < shift; ++i)
    {
      int carry = 0;
      for (size_t j = digits.size(); j > 0; j--)
      {
        int temp = digits[j - 1] + carry * BASE;
        digits[j - 1] = temp / 2;
        carry = temp % 2;
      }
      if (digits.back() == 0 && digits.size() > 1)
      {
        digits.pop_back();
      }
    }
    return *this;
  }

  bool operator<(const BigInt &rhs) const
  {

    if (sign != rhs.sign)
    {
      return sign;
    }

    if (sign && rhs.sign)
    {
      return rhs.abs() <= abs();
    }

    // If the numbers have the same sign
    if (digits.size() != rhs.digits.size())
    {
      // If one number has more digits than the other, the one with more digits is larger when the numbers are positive
      return digits.size() < rhs.digits.size();
    }

    // If the numbers have the same number of digits
    for (int i = digits.size() - 1; i >= 0; --i)
    {
      if (digits[i] != rhs.digits[i])
      {
        // The first digit that differs determines which number is larger
        return digits[i] < rhs.digits[i];
      }
    }

    // If all digits are the same, the numbers are equal and thus the first is not less than the second
    return false;
  }
  // Operator< for comparing BigInt and int
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
  /*
  BigInt& operator<<=(int shift) {
    BigInt two = 2;
    BigInt multiplier = two.pow(shift);
    *this *= multiplier;
    return *this;
  }
  */
  BigInt &operator<<=(int shift)
  {
    if (shift < 0)
    {
      return (*this) >>= (-shift);
    }
    for (int i = 0; i < shift; ++i)
    {
      int carry = 0;
      for (size_t j = 0; j < digits.size(); ++j)
      {
        int temp = digits[j] * 2 + carry;
        digits[j] = temp % BASE;
        carry = temp / BASE;
      }
      if (carry > 0)
      {
        digits.push_back(carry);
      }
    }
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

  // Decrement operator
  BigInt &operator--()
  {

    // When the number is negative, decrementing it means increasing its absolute value
    if (sign)
    {      
      *this = (*this).abs()+1;
      sign = true;
      return *this;
    }
    // When the number is zero, decrementing it means making it -1
    if (*this == 0)
    {
      *this += 1;
      sign = true;      
      return *this;
    }

    // Propagate the decrement operation from the least significant digit to the most significant digit
    for (size_t i = 0; i < digits.size(); ++i)
    {
      if (digits[i]--)
      {
        break;
      }
      digits[i] = 9;
      if (i + 1 == digits.size())
      {
        digits.pop_back();
        break;
      }
    }
    return *this;
  }

  BigInt operator--(int)
  {
    // Store the original value to return it later
    BigInt result = *this;

    // When the number is negative, decrementing it means increasing its absolute value
    if (sign)
    {      
      *this = (*this).abs()+1;
      sign = true;
      return result;
    }
    // When the number is zero, decrementing it means making it -1
    if (*this == 0)
    {
      *this += 1;
      sign = true;      
      return result;
    }

    for (size_t i = 0; i < digits.size(); ++i)
    {
      if (digits[i]--)
      {
        break;
      }
      digits[i] = 9;
      if (i + 1 == digits.size())
      {
        digits.pop_back();
        break;
      }
    }
    return result;
  }

  BigInt operator|(const BigInt &rhs) const
  {
    BigInt result;
    size_t size = std::max(digits.size(), rhs.digits.size());
    for (size_t i = 0; i < size; i++)
    {
      int lhsDigit = i < digits.size() ? digits[i] : 0;
      int rhsDigit = i < rhs.digits.size() ? rhs.digits[i] : 0;
      result.digits.push_back(lhsDigit | rhsDigit);
    }
    return result;
  }

  BigInt operator||(const BigInt &rhs) const
  {
    if (*this != 0)
    {
      return 1;
    }
    else if (rhs != 0)
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }

  // Sign change operator
  BigInt operator-() const
  {
    BigInt result = *this;
    result.sign = !sign;
    return result;
  }

  // Minus equals operator
  BigInt &operator-=(const BigInt &rhs)
  {
    // Different signs need special handling
    if (sign != rhs.sign)
    {
      // If the first number is negative, we can just add the second number to it
      if (sign)
      {
        BigInt temp = rhs.abs();
        temp += (*this).abs();
        *this = temp;
        sign = true;
      }
      else
      {
        *this += rhs.abs();
        sign = false;
      }
      return *this;
    }
    else if (rhs.abs() > (*this).abs())
    {
      BigInt temp = rhs.abs();
      temp -= (*this).abs();
      *this = temp;
      sign = !rhs.sign;
      return *this;
    }

    int borrow = 0;
    for (int i = 0; i < digits.size(); i++)
    {
      int diff = digits[i] - borrow;
      if (i < rhs.digits.size())
      {
        diff -= rhs.digits[i];
      }
      if (diff < 0)
      {
        // Borrow from the next non-zero digit
        int j = i + 1;
        while (digits[j] == 0)
        {
          digits[j] = 9;
          j++;
        }
        digits[j]--;
        diff += 10;
      }
      digits[i] = diff;
    }
    // Remove leading zeroes
    while (digits.size() > 1 && digits.back() == 0)
    {
      digits.pop_back();
    }
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
    std::mt19937 generator(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(0, 9);

    BigInt result;
    for (int i = 0; i < bitLength; ++i)
    {
      result = result * 10 + distribution(generator);
    }

    return result;
  }

  bool isPrime()
  {
    if ((*this) <= 1)
    {
      return false;
    }

    for (BigInt i = 2; i * i <= (*this); ++i)
    {
      if ((*this) % i == 0)
      {
        return false;
      }
    }

    return true;
  }

  static BigInt generatePrime(int bitLength)
  {
    // Here we generate a random number of the specified bit length.
    // You'll need to implement the generateRandom function, and the code will
    // depend on the details of your BigInt class and what libraries are available.
    BigInt candidate = BigInt::generateRandom(bitLength);

    // Make sure the candidate is odd.
    // Many ways to make a number odd, one simple way is adding 1 if it's even.
    if (candidate % 2 == 0)
    {
      candidate += 1;
    }

    // Test candidate primes until we find one that's really prime.
    while (!candidate.isPrime())
    {
      candidate += 2;
    }

    return candidate;
  }

  bool isEven() const
  {
    // std::cout << "isEven(): " << *this << ": " << digits[0]  << ": " << (((digits[0] & 1) == 0)?"even":"odd") << std::endl;
    return (digits[0] & 1) == 0;
  }

  static BigInt gcd(BigInt a, BigInt b)
  {

    // std::cout << a << " " << b << std::endl;

    if (a == 0)
    {
      return b;
    }
    if (b == 0)
    {
      return a;
    }

    int shift = 0;
    while (a.isEven() && b.isEven())
    {
      // while (((a.digits[0] | b.digits[0]) & 1) == 0) {
      a >>= 1;
      b >>= 1;
      ++shift;
      // std::cout << a << ":" << a.isEven() << " " << b << ":" << b.isEven() << std::endl;
    }

    while (a.isEven())
    {
      a >>= 1;
    }

    do
    {
      while (b != 0 && b.isEven())
      {
        b >>= 1;
        // std::cout << "b: " << b << std::endl;
      }
      if (a > b)
      {
        std::swap(a, b);
      }
      if (b == 0)
      {
        break;
      }
      b -= a;
      // std::cout << "a: " << a << " b: " << b << std::endl;
    } while (b != 0);
    // std::cout << "gcd: " << (a << shift) << std::endl;
    return a << shift;
  }

  static BigInt modPow(BigInt base, BigInt exp, BigInt modulus)
  {
    if (modulus == 1)
      return 0;
    base %= modulus;
    BigInt result = 1;
    while (exp > 0)
    {
      if (!exp.isEven()) {        
        result = (result * base);
        if (result >= modulus) result %= modulus;
      }
      base = (base * base);
      if (base >= modulus) base %= modulus;
      exp >>= 1;
    }
    return result;
  }

  /*
    static BigInt modInverse(BigInt a, BigInt b) {
      BigInt b0 = b, t, q;
      BigInt x0 = 0, x1 = 1;
      if (b == 1) return b;
      while (a > 1) {
        std::cout << "a: " << a << " b: " << b << std::endl;
        q = a / b;
        std::cout << "q: " << q << std::endl;
        t = b, b = a % b, a = t;
        t = x0;

        std::cout << x1 << "-" << q << "*" << x0 << "=" << x1 - q * x0 << std::endl;
        x0 = x1 - q * x0;
        x1 = t;
      }
      std::cout << "x1: " << x1 << std::endl;
      if (x1 < 0) x1 += b0;
      return x1;
    }*/

  static BigInt modInverse(BigInt a, BigInt p)
  {
    BigInt val(0);
    BigInt nt(1);
    BigInt r(p);
    BigInt nr(a);

    while (nr != 0)
    {
      BigInt q = r / nr;
      // std::cout << "q: " << q << std::endl;
      BigInt tmp = nt;
      // std::cout << val << "-" << q << "*" << nt << "=" << val - q * nt << std::endl;
      nt = val - q * nt;
      // std::cout << "nt: " << nt << std::endl;
      val = tmp;
      tmp = nr;
      nr = r - q * nr;
      // std::cout << "nr: " << nr << std::endl;
      r = tmp;
    }
    if (val < 0)
      val += p;

    return val;
  }

  friend std::ostream &operator<<(std::ostream &os, const BigInt &bi);

private:
  static const int BASE = 10;
  bool sign = false;      // false = positive, true = negative
  std::vector<int8_t> digits; // digits are stored in reverse order
};

// Output to stream as string.
std::ostream &operator<<(std::ostream &os, const BigInt &bi)
{
  if (bi.sign)
  {
    os << "-";
  }
  for (auto digit = bi.digits.rbegin(); digit != bi.digits.rend(); ++digit)
  {
    os << (int)*digit;
  }
  return os;
}
