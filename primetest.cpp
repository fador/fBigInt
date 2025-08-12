#include "fbigint.hpp"

void operator_tests();

void testModInverse() {
  BigInt a = 3;
  BigInt m = 11;  // m must be a prime number for a to have a modular inverse

  BigInt a_inverse = BigInt::modInverse(a, m);

  // Check that (a * a_inverse) % m == 1
  assert((a * a_inverse) % m == 1);
}


bool tests() {
    
  operator_tests();

  BigInt a(123);
  BigInt b("123");
  assert(a == b);

  BigInt c = a + b;
  assert(c == BigInt("246"));

  // Test operator !=
  assert(a != c);

  // Test operator >
  assert(c > a);

  // Test operator *
  BigInt d = a * 2;
  assert(d == BigInt("246"));

  assert(a + b == BigInt("246"));
  assert(a + b + 1 == BigInt("247"));
  // Test operator +=
  a += b;
  assert(a == BigInt("246"));

  // Test operator / and operator %
  BigInt e("246"), f("123");
  std::cout << e / f << std::endl;
  assert(e / f == BigInt("2"));
  assert(e % f == BigInt("0"));

  {
    // Test operator / with random operands
    for (int i = 0; i < 10; ++i) {
      BigInt a = BigInt::generateRandom(100);
      BigInt b = BigInt::generateRandom(50);
      if (b == 0) {
        continue; // Avoid division by zero
      }
      //std::cout << a << " / " << b << std::endl;
      BigInt q = a / b;
      BigInt r = a % b;
      std::cout << a << " / " << b << " = " << q << std::endl;
      assert(a == b * q + r);
    }

    // Test operator / with zero dividend
    BigInt zero(0);
    BigInt nonZero = BigInt::generateRandom(10);
    assert(zero / nonZero == BigInt(0));

    // Test operator / with equal operands
    BigInt equal = BigInt::generateRandom(10);
    assert(equal / equal == BigInt(1));

    // Test operator / with dividend less than divisor
    BigInt smallDividend = BigInt::generateRandom(5);
    BigInt largeDivisor = BigInt::generateRandom(10);
    assert(smallDividend / largeDivisor == BigInt(0));

    // Test operator / with dividend equal to divisor
    BigInt dividend = BigInt::generateRandom(10);
    BigInt divisor = dividend;
    assert(dividend / divisor == BigInt(1));  
  }
  // Test operator -=
  e -= f;
  assert(e == BigInt("123"));

  // Test operator ++, operator-- and conversion to bool
  BigInt g("0");
  assert(!g);
  ++g;
  assert(g);
  --g;
  assert(!g);

  // Test operator >>=
  BigInt shiftRight("246");
  shiftRight >>= 1;
  std::cout << shiftRight << std::endl;
  assert(shiftRight == BigInt("123")); // 246 / 2


  // Test operator <<=
  BigInt shiftLeft("123456789");
  shiftLeft <<= 2;
  assert(shiftLeft == BigInt("493827156")); // 123456789 * 2^2

  // Test operator << (should not modify original)
  BigInt original("123456789");
  BigInt shiftLeftResult = original << 3;
  assert(shiftLeftResult == BigInt("987654312")); // 123456789 * 2^3
  assert(original == BigInt("123456789")); // Original should not change


  BigInt shiftRight2("123456789");
  //std::cout << shiftRight2 << std::endl;
  shiftRight2 >>= 2;
  //std::cout << shiftRight2 << std::endl;
  assert(shiftRight2 == BigInt("30864197")); // 123456789 / 2^2

  // Test operator >> (should not modify original)
  original = BigInt("123456789");
  BigInt shiftRightResult = original >> 3;
  std::cout << original << " " << shiftRightResult << std::endl;
  assert(shiftRightResult == BigInt("15432098")); // 123456789 / 2^3
  assert(original == BigInt("123456789")); // Original should not change

  
   // Test operator *
  BigInt aa("123456789"), bb("987654321");
  BigInt mulResult = aa * bb;
  assert(mulResult == BigInt("121932631112635269"));

  // Test operator /
  BigInt dividend("121932631112635269"), divisor("123456789");
  BigInt divResult = dividend / divisor;
  assert(divResult == BigInt("987654321"));

  // Negative values
  // Test addition
  assert(BigInt("-1234") + BigInt("-5678") == BigInt("-6912"));
  assert(BigInt("-1234") + BigInt("5678") == BigInt("4444"));
  assert(BigInt("1234") + BigInt("-5678") == BigInt("-4444"));

  // Test subtraction
  assert(BigInt("-1234") - BigInt("-5678") == BigInt("4444"));
  assert(BigInt("-1234") - BigInt("5678") == BigInt("-6912"));
  assert(BigInt("1234") - BigInt("-5678") == BigInt("6912"));

  // Test multiplication
  assert(BigInt("-1234") * BigInt("-5678") == BigInt("7006652"));
  assert(BigInt("-1234") * BigInt("5678") == BigInt("-7006652"));
  assert(BigInt("1234") * BigInt("-5678") == BigInt("-7006652"));

  // Test division
  assert(BigInt("-7006652") / BigInt("-1234") == BigInt("5678"));
  assert(BigInt("-7006652") / BigInt("1234") == BigInt("-5678"));
  assert(BigInt("7006652") / BigInt("-1234") == BigInt("-5678"));

  // Test ++ and -- operator sign changes
  {
    BigInt a = 1;
    BigInt b = -1;

    a--;  // decrement a to 0
    assert(a == BigInt("0"));
    a--;  // decrement a to -1
    assert(a == BigInt("-1"));

    b++;  // increment b to 0
    assert(b == BigInt("0"));
    b++;  // increment b to 1
    assert(b == BigInt("1"));

    BigInt c = 0;
    c--;  // decrement c to -1
    assert(c == BigInt("-1"));
    c++;  // increment c back to 0
    assert(c == BigInt("0"));
  }

  // Test modInverse()
  testModInverse();

  BigInt dd = BigInt::modInverse(BigInt("65537"), BigInt("91273218666296797593635512882458304367850604767639817636318203449771530310880"));
  BigInt d_real("34654492883004458771723338984912491990558411255217382276340172062204021983393");// = BigInt::modInverse(e, phi);
  
  std::cout << "d: " << dd << " " << d_real << std::endl;

  assert(dd == d_real);

  
  std::cout << "All tests passed!\n";
  return true;
}

int main() {

  //std::cout << "Running tests" << std::endl;
  tests();

  std::cout << "Generating keys" << std::endl;
  //std::cout << BigInt::generatePrime(128) << std::endl;

  // Small prime numbers for simplicity
  BigInt p = BigInt("316069278939301595364059265048725001061"); 
  BigInt q = BigInt("288775989152128383322997467687931821049");

  std::cout << "p: " << p << std::endl;
  std::cout << "q: " << q << std::endl;

  BigInt n = p * q;
  BigInt phi = (p - 1) * (q - 1);

  // Smaller exponent
  BigInt e = 65537;
  while (BigInt::gcd(e, phi) != 1) {
    ++e;
    std::cout << "e: " << e << std::endl;
    assert(e < phi);
  }

  std::cout << "e: " << e << std::endl;
  std::cout << "phi: " << phi << std::endl;
  
  BigInt d = BigInt::modInverse(e, phi);  

  std::cout << "d: " << d << std::endl;
  std::cout << "n: " << n << std::endl;

  BigInt message = 128; // The "message" is a number smaller than n
  BigInt encryptedMessage = BigInt::modPow(message, e, n);
  BigInt decryptedMessage = BigInt::modPow(encryptedMessage, d, n);

  std::cout << "Original Message: " << message << std::endl;
  std::cout << "Encrypted Message: " << encryptedMessage << std::endl;
  std::cout << "Decrypted Message: " << decryptedMessage << std::endl;

  return 0;
}
