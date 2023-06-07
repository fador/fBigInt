# Fador's BigInt Library

The fBigInt library is a single header C++ library for arbitrary-precision integer arithmetic. It provides a `BigInt` class that can represent integers of any size, limited only by the amount of memory available.

## Features

The BigInt library provides the following features:

- Support for integers of any size, limited only by the amount of memory available.
- Support for basic arithmetic operations, including addition, subtraction, multiplication, and division.
- Support for comparison operations, including equality, inequality, less than, and greater than.
- Support for conversion to and from strings and integers.
- Support for random number generation.
- Support for modular arithmetic operations, including modular addition, subtraction, multiplication, and exponentiation.
- Support for computing the greatest common divisor and the modular inverse of two integers.

**Some features such as the bitwise operations do have problems and do not work properly**

## Usage

To use the BigInt library, simply include the `fbigint.hpp` header file in your C++ code:

```cpp
#include "fbigint.hpp"
```

You can then create `BigInt` objects and perform arithmetic and comparison operations on them:

```cpp
BigInt a("12345678901234567890");
BigInt b("98765432109876543210");

BigInt c = a + b;
BigInt d = a * b;

if (c < d) {
    std::cout << "c is less than d" << std::endl;
} else {
    std::cout << "c is greater than or equal to d" << std::endl;
}
```

## Installation

To install the fBigInt library, simply clone the repository and build the library using CMake:

```bash
git clone https://github.com/fador/fBigInt.git
cd fBigInt
mkdir build
cd build
cmake ..
make
sudo make install
```

Optionally you can also run `make test` for the tests.

This will build the library and install the header file and the shared library to the appropriate system directories.

## License

The fBigInt library is licensed under the permissive ISC License. See the `LICENSE` file for details.

## Contributing

Contributions to the fBigInt library are welcome. To contribute, simply fork the repository, make your changes, and submit a pull request.