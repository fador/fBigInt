#include "fbigint.hpp"
#include <cassert>
#include <iostream>

void operator_tests() {
    std::cout << "Running operator tests..." << std::endl;

    // BigInt()
    BigInt default_constructor;
    assert(default_constructor == 0);

    // BigInt(long long int)
    BigInt long_long_constructor(1234567890123456789LL);
    assert(long_long_constructor == BigInt("1234567890123456789"));
    BigInt neg_long_long_constructor(-1234567890123456789LL);
    assert(neg_long_long_constructor == BigInt("-1234567890123456789"));

    // operator=(const BigInt &rhs)
    BigInt assign_op;
    BigInt val_to_assign("987654321");
    assign_op = val_to_assign;
    assert(assign_op == val_to_assign);

    // operator=(const int &rhs)
    BigInt assign_op_int;
    assign_op_int = 12345;
    assert(assign_op_int == BigInt("12345"));

    // operator+(const int &rhs)
    BigInt add_int("100");
    BigInt add_int_res = add_int + 50;
    assert(add_int_res == BigInt("150"));

    // operator-(const int &rhs)
    BigInt sub_int("100");
    BigInt sub_int_res = sub_int - 50;
    assert(sub_int_res == BigInt("50"));

    // operator*=(const BigInt &rhs)
    BigInt mul_assign("10");
    mul_assign *= BigInt("12");
    assert(mul_assign == BigInt("120"));

    // operator/(const int &rhs)
    BigInt div_int("100");
    BigInt div_int_res = div_int / 4;
    assert(div_int_res == BigInt("25"));

    // operator%=(const BigInt &rhs)
    BigInt mod_assign("10");
    mod_assign %= BigInt("3");
    assert(mod_assign == BigInt("1"));

    // operator%(const int &rhs)
    BigInt mod_int("10");
    BigInt mod_int_res = mod_int % 3;
    assert(mod_int_res == BigInt("1"));

    // operator++(int) (post-increment)
    BigInt post_inc("10");
    BigInt post_inc_res = post_inc++;
    assert(post_inc == BigInt("11"));
    assert(post_inc_res == BigInt("10"));

    // operator--(int) (post-decrement)
    BigInt post_dec("10");
    BigInt post_dec_res = post_dec--;
    assert(post_dec == BigInt("9"));
    assert(post_dec_res == BigInt("10"));

    // operator-() (unary minus)
    BigInt unary_minus("123");
    assert(-unary_minus == BigInt("-123"));
    BigInt neg_unary_minus("-123");
    assert(-neg_unary_minus == BigInt("123"));

    // Comparison operators
    BigInt comp1("100"), comp2("200"), comp3("100");
    assert(comp1 < comp2);
    assert(!(comp2 < comp1));
    assert(comp1 <= comp2);
    assert(comp1 <= comp3);
    assert(comp2 > comp1);
    assert(!(comp1 > comp2));
    assert(comp2 >= comp1);
    assert(comp1 >= comp3);
    assert(comp1 != comp2);
    assert(comp1 == comp3);

    // Comparison with int
    assert(BigInt("10") < 20);
    assert(BigInt("10") <= 20);
    assert(BigInt("10") <= 10);
    assert(BigInt("20") > 10);
    assert(BigInt("20") >= 10);
    assert(BigInt("10") >= 10);
    assert(BigInt("10") != 20);
    assert(BigInt("10") == 10);

    // Bitwise operators
    BigInt band1("12"); // 1100
    BigInt band2("10"); // 1010
    assert((band1 & band2) == BigInt("8")); // 1000
    assert((band1 & 10) == BigInt("8"));
    assert((band1 | band2) == BigInt("14")); // 1110

    // Logical OR
    BigInt log1("0"), log2("123"), log3("0");
    assert(log1 || log2);
    assert(!(log1 || log3));

    // pow()
    BigInt pow_base("2");
    assert(pow_base.pow(10) == BigInt("1024"));
    assert(pow_base.pow(0) == BigInt("1"));

    // abs()
    BigInt abs_val("-123");
    assert(abs_val.abs() == BigInt("123"));
    BigInt abs_val2("123");
    assert(abs_val2.abs() == BigInt("123"));

    // isEven()
    BigInt even("1234");
    BigInt odd("1235");
    assert(even.isEven());
    assert(!odd.isEven());

    // gcd()
    BigInt gcd1("48"), gcd2("18");
    assert(BigInt::gcd(gcd1, gcd2) == BigInt("6"));

    // modPow()
    BigInt mod_base("3"), mod_exp("4"), mod_mod("5");
    assert(BigInt::modPow(mod_base, mod_exp, mod_mod) == BigInt("1")); // 3^4 % 5 = 81 % 5 = 1

    // isPrime()
    BigInt prime("17");
    BigInt not_prime("18");
    assert(prime.isPrime());
    assert(!not_prime.isPrime());

    // BigInt + BigInt
    assert(BigInt("100") + BigInt("50") == BigInt("150"));
    // BigInt - BigInt
    assert(BigInt("100") - BigInt("50") == BigInt("50"));
    // BigInt * BigInt
    assert(BigInt("10") * BigInt("12") == BigInt("120"));
    // BigInt / BigInt
    assert(BigInt("100") / BigInt("4") == BigInt("25"));
    // BigInt % BigInt
    assert(BigInt("10") % BigInt("3") == BigInt("1"));

    // +=
    BigInt add_assign("100");
    add_assign += BigInt("50");
    assert(add_assign == BigInt("150"));

    // -=
    BigInt sub_assign("100");
    sub_assign -= BigInt("50");
    assert(sub_assign == BigInt("50"));


    // Pre-increment
    BigInt pre_inc("10");
    assert(++pre_inc == BigInt("11"));
    assert(pre_inc == BigInt("11"));

    // Pre-decrement
    BigInt pre_dec("10");
    assert(--pre_dec == BigInt("9"));
    assert(pre_dec == BigInt("9"));

    // /=
    BigInt div_assign_2("100");
    div_assign_2 /= BigInt("4");
    assert(div_assign_2 == BigInt("25"));

    // ^
    BigInt xor1("12"); // 1100
    BigInt xor2("10"); // 1010
    assert((xor1 ^ xor2) == BigInt("6")); // 0110

    // <<
    BigInt shift_left("10"); // 1010
    assert((shift_left << 2) == BigInt("40")); // 101000

    // >>
    BigInt shift_right("10"); // 1010
    assert((shift_right >> 2) == BigInt("2")); // 10

    // <<=
    BigInt shift_left_assign("10");
    shift_left_assign <<= 2;
    assert(shift_left_assign == BigInt("40"));

    // >>=
    BigInt shift_right_assign("10");
    shift_right_assign >>= 2;
    assert(shift_right_assign == BigInt("2"));

    std::cout << "Operator tests passed!" << std::endl;
}
