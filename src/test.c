

#include <check.h>
#include <stdio.h>

#include "s21_arrays.h"
#include "s21_decimal.h"

START_TEST(SUB_TEST) {
  s21_decimal test1 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 1u << 31}};
  s21_decimal test2 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 1u << 31}};
  s21_decimal result;

  int res = s21_sub(test1, test2, &result);
  ck_assert_int_eq(res, 0);

  test2.bits[3] = 0;

  res = s21_sub(test1, test2, &result);
  ck_assert_int_eq(res, 2);

  res = s21_sub(test2, test1, &result);
  ck_assert_int_eq(res, 1);

  init_0((uint32_t *)test1.bits, 3);
  init_0((uint32_t *)test2.bits, 3);
  test1.bits[3] = 0;
  test2.bits[3] = 0;
  test1.bits[0] = 1;
  test2.bits[2] = 1;

  s21_sub(test2, test1, &result);
  ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[0], 0xFFFFFFFF);

  s21_sub(test1, test2, &result);

  ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[0], 0xFFFFFFFF);
  ck_assert_uint_eq(result.bits[3], 1u << 31);

  for (int i = -10; i < 10; ++i) {
    for (int j = -10; j < 10; ++j) {
      int a = i;
      int b = j;
      int r;
      decimal aa;
      decimal bb;
      decimal rr;

      s21_from_int_to_decimal(a, &aa);

      s21_from_int_to_decimal(b, &bb);

      s21_sub(aa, bb, &rr);
      s21_from_decimal_to_int(rr, &r);
      if (r != a - b) {
        printf("res : %d != (%d) - (%d) == (%d)\n", r, a, b, a - b);
        ck_assert_int_eq(r, a - b);
      }
    }
  }

  for (float i = -10; i < 10; i += 0.3) {
    for (float j = -10; j < 10; j += 0.3) {
      float a = i;
      float b = j;
      float r;
      decimal aa;
      decimal bb;
      decimal rr;

      s21_from_float_to_decimal(a, &aa);

      s21_from_float_to_decimal(b, &bb);

      s21_sub(aa, bb, &rr);
      s21_from_decimal_to_float(rr, &r);
      if (fabsf(r - (a - b)) > 0.0001) {
        printf("%f != (%f) - (%f) == (%f)\n", r, a, b, a - b);
        ck_assert_float_eq_tol(r, a - b, 0.0001);
      }
    }
  }
}

END_TEST

START_TEST(ADD_TEST) {
  s21_decimal test1 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 1u << 31}};
  s21_decimal test2 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 1u << 31}};
  s21_decimal result;
  int res = s21_add(test1, test2, &result);
  ck_assert_int_eq(res, 2);

  test2.bits[3] = 0;
  s21_add(test1, test2, &result);
  for (int i = 0; i < 3; ++i) ck_assert_int_eq(result.bits[i], 0);

  test1.bits[3] = 0;
  res = s21_add(test1, test2, &result);
  ck_assert_int_eq(res, 1);

  init_0((uint32_t *)test1.bits, 3);
  init_0((uint32_t *)test2.bits, 3);
  test1.bits[0] = 1;
  test1.bits[3] = 1u << 31;
  test2.bits[2] = 1;
  s21_add(test2, test1, &result);
  ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[0], 0xFFFFFFFF);

  s21_negate(test1, &test1);
  s21_negate(test2, &test2);
  ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[0], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[3], 1u << 31);
}

END_TEST

START_TEST(TO_FROM_INT) {
  decimal d;
  int check;
  for (int i = -10; i < 10; i++) {
    s21_from_int_to_decimal(i, &d);
    s21_from_decimal_to_int(d, &check);
    if (check != i) {
      printf("%d != %d\n", check, i);
      ck_assert_int_eq(check, i);
    }
  }
  int max = INT32_MAX;
  int min = INT32_MIN;
  s21_from_int_to_decimal(max, &d);
  s21_from_decimal_to_int(d, &check);
  if (check != max) {
    printf("%d != %d\n", check, max);
    ck_assert_int_eq(check, max);
  }
  s21_from_int_to_decimal(min, &d);
  s21_from_decimal_to_int(d, &check);
  if (check != min) {
    printf("%d != %d\n", check, min);
    ck_assert_int_eq(check, min);
  }
}
END_TEST

START_TEST(FLOAT_TO_INT) {
  decimal a = {{15, 0, 0, 0}};
  setDecimalExp(&a, 1);
  int dst;
  s21_from_decimal_to_int(a, &dst);
  ck_assert_int_eq(dst, 1);
  a.bits[0] = UINT32_MAX;
  s21_from_decimal_to_int(a, &dst);
  ck_assert_int_eq(dst, 429496729);
}
END_TEST

START_TEST(TO_FROM_FLOAT) {
  float f;
  float check;
  decimal res;
  f = 0.5f;
  s21_from_float_to_decimal(f, &res);
  s21_from_decimal_to_float(res, &check);
  ck_assert_float_eq_tol(f, check, 0.00005);
  f = 1;
  s21_from_float_to_decimal(f, &res);
  s21_from_decimal_to_float(res, &check);
  ck_assert_float_eq_tol(f, check, 0.00005);
  f = 18446744073709551616.0f;
  s21_from_float_to_decimal(f, &res);
  s21_from_decimal_to_float(res, &check);
  ck_assert_float_eq_tol(f, check, 0.00005);
  f = 79228157791897854723898736640.0f;
  s21_from_float_to_decimal(f, &res);
  s21_from_decimal_to_float(res, &check);
  ck_assert_float_eq_tol(f, check, 0.00005);
  f = 79228162514264337593543950336.0f;
  int err = s21_from_float_to_decimal(f, &res);
  ck_assert_int_eq(err, 1);
  f = INFINITY;
  err = s21_from_float_to_decimal(f, &res);
  ck_assert_int_eq(err, 1);
  f = NAN;
  err = s21_from_float_to_decimal(f, &res);
  ck_assert_int_eq(err, 1);
}

START_TEST(NEGATE_TEST) {
  s21_decimal d;
  int check;
  for (int i = -10; i < 10; ++i) {
    s21_from_int_to_decimal(i, &d);
    s21_negate(d, &d);
    s21_from_decimal_to_int(d, &check);
    ck_assert_int_eq(-i, check);
  }
}

END_TEST

START_TEST(GREATER_TEST) {
  s21_decimal test1 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 1u << 31}};
  s21_decimal test2 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  int res = s21_is_greater(test1, test2);
  ck_assert_int_eq(res, 0);

  res = s21_is_greater(test2, test1);
  ck_assert_int_eq(res, 1);

  s21_negate(test1, &test1);
  res = s21_is_greater(test2, test1);
  ck_assert_int_eq(res, 0);

  decimal test3 = {{11, 0, 0, 0}};
  decimal test4 = {{10, 0, 0, 0}};

  res = s21_is_greater(test3, test4);
  ck_assert_int_eq(res, 1);

  res = s21_is_less(test4, test3);
  ck_assert_int_eq(res, 1);

  test3.bits[0] = 10;

  res = s21_is_greater(test3, test4);
  ck_assert_int_eq(res, 0);

  res = s21_is_not_equal(test3, test4);
  ck_assert_int_eq(res, 0);

  res = s21_is_equal(test3, test4);
  ck_assert_int_eq(res, 1);
}

END_TEST

START_TEST(EQUAL_TEST) {
  s21_decimal test1 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 1u << 31}};
  s21_decimal test2 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  int res = s21_is_equal(test1, test2);
  ck_assert_int_eq(res, 0);

  res = s21_is_equal(test2, test1);
  ck_assert_int_eq(res, 0);

  res = s21_is_equal(test2, test2);
  ck_assert_int_eq(res, 1);

  s21_decimal test3 = {{0, 0, 0, 1u << 31}};
  s21_decimal test4 = {{0, 0, 0, 0}};
  res = s21_is_equal(test3, test4);
  ck_assert_int_eq(res, 1);

  decimal test5 = {{0, 0, 0, 0}};
  decimal test6 = {{10, 0, 0, 0}};
  setDecimalExp(&test6, 1);
  int scale = 1;
  for (int i = 10; i < 10000000; i *= 10) {
    test5.bits[0] = i;
    setDecimalExp(&test5, scale);
    scale++;
    res = s21_is_equal(test5, test6);
    ck_assert_int_eq(res, 1);
  }

  uint32_t a[] = {
      0b00010000000000000000000000000000,
      0b00111110001001010000001001100001,
      0b00100000010011111100111001011110,
  };

  uint32_t b[] = {
      0b11101000000000000000000000000000,
      0b10011111110100001000000000111100,
      0b00000011001110110010111000111100,
  };

  decimal test7 = {0};
  decimal test8 = {0};
  copyArray(a, test7.bits, 3);
  copyArray(b, test8.bits, 3);
  setDecimalExp(&test7, 1);
  res = s21_is_equal(test7, test8);
  ck_assert_int_eq(res, 1);
  mul10(test8.bits, 3);
  setDecimalExp(&test7, 0);
  res = s21_is_equal(test7, test8);
  ck_assert_int_eq(res, 1);
}

END_TEST

START_TEST(LESS_OR_EQUAL_TEST) {
  decimal a = {{0, 0, 0, 0}};
  decimal b = {{0, 0, 0, 0}};
  int res = s21_is_less_or_equal(a, b);
  ck_assert_int_eq(res, 1);
  b.bits[0] = 1;
  res = s21_is_less_or_equal(a, b);
  ck_assert_int_eq(res, 1);
  a.bits[0] = 1;
  b.bits[0] = 1;
  setDecimalSign(&a, 1);
  res = s21_is_less_or_equal(a, b);
  ck_assert_int_eq(res, 1);
  setDecimalSign(&b, 1);
  setDecimalSign(&a, 0);
  res = s21_is_less_or_equal(a, b);
  ck_assert_int_eq(res, 0);
}
END_TEST

START_TEST(MUL_TEST) {
  s21_decimal test1 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 1u << 31}};
  s21_decimal test2 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal result;
  int res = s21_mul(test1, test2, &result);
  ck_assert_int_eq(res, 2);

  s21_negate(test1, &test1);
  res = s21_mul(test1, test2, &result);
  ck_assert_int_eq(res, 1);

  init_0((uint32_t *)test2.bits, 3);
  test2.bits[0] = 1;
  setDecimalExp(&test2, 28);
  res = s21_mul(test1, test2, &result);
  ck_assert_int_eq(result.bits[3], 28 << 16);
  ck_assert_int_eq(result.bits[0], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[2], 0xFFFFFFFF);
  ck_assert_int_eq(res, 0);

  for (int i = -10; i < 10; ++i) {
    for (int j = -10; j < 10; ++j) {
      int a = i;
      int b = j;
      int r;
      decimal aa;
      decimal bb;
      decimal rr;

      s21_from_int_to_decimal(a, &aa);

      s21_from_int_to_decimal(b, &bb);

      s21_mul(aa, bb, &rr);
      s21_from_decimal_to_int(rr, &r);
      if (r != a * b) {
        printf("%d != (%d) * (%d) == (%d)\n", r, a, b, a * b);
        ck_assert_int_eq(r, a * b);
      }
    }
  }
  for (float i = -10; i < 10; i += 0.3) {
    for (float j = -10; j < 10; j += 0.3) {
      float a = i;
      float b = j;
      float r;
      decimal aa;
      decimal bb;
      decimal rr;

      s21_from_float_to_decimal(a, &aa);

      s21_from_float_to_decimal(b, &bb);

      s21_mul(aa, bb, &rr);
      s21_from_decimal_to_float(rr, &r);
      if (fabsf(r - (a * b)) > 0.001) {
        printf("%f != (%f) * (%f) == (%f)\n", r, a, b, a * b);
        ck_assert_float_eq_tol(r, a * b, 0.001);
      }
    }
  }
}

END_TEST

START_TEST(DIV_TEST) {
  decimal a = {{1, 0, 0, 0}};
  decimal b = {{1, 0, 0, 0}};
  decimal r = {{0, 0, 0, 0}};
  int err = 0;

  for (int i = 0; i < 29; i++) {
    setDecimalExp(&a, i);
    err = s21_div(a, b, &r);
    if (!err) {
      if (!s21_is_equal(r, a)) {
        ck_assert_int_eq(0, 1);
      }
    } else {
      ck_assert_int_eq(0, 1);
    }
  }
  setDecimalExp(&a, 0);
  setDecimalExp(&b, 0);
  for (int i = 0; i < 28; i++) {
    setDecimalExp(&b, i);
    err = s21_div(a, b, &r);
    if (!err) {
      ck_assert_int_eq(getDecimalExp(r), 1);
    } else {
      ck_assert_int_eq(0, 1);
    }
  }

  int signs[4][2] = {
      {0, 0},
      {0, 1},
      {1, 0},
      {1, 1},
  };
  for (int i = 0; i < 4; ++i) {
    int s1 = signs[i][0];
    int s2 = signs[i][1];
    setDecimalSign(&a, s1);
    setDecimalSign(&b, s2);
    err = s21_div(a, b, &r);
    if (!err) {
      ck_assert_int_eq(getDecimalSign(r), s1 != s2);
    } else {
      ck_assert_int_eq(0, 1);
    }
  }

  init_0((uint32_t *)&a.bits, 4);
  init_0((uint32_t *)&b.bits, 4);
  a.bits[0] = 10;
  b.bits[0] = 3;
  err = s21_div(a, b, &r);
  ck_assert_int_eq(err, 0);

  init_0((uint32_t *)&a.bits, 4);
  a.bits[0] = 0xFFFFFFFF;
  a.bits[1] = 0xFFFFFFFF;
  a.bits[2] = 0xFFFFFFFF;
  init_0((uint32_t *)&b.bits, 4);
  b.bits[0] = 1;
  setDecimalExp(&b, 28);
  err = s21_div(a, b, &r);
  ck_assert_int_eq(err, 1);

  setDecimalSign(&b, 1);
  err = s21_div(a, b, &r);
  ck_assert_int_eq(err, 2);

  decimal del = {{10, 0, 0, 0}};
  decimal div = {{0, 0, 0, 0}};
  decimal res = {{0, 0, 0, 0}};
  err = s21_div(del, div, &res);
  ck_assert_int_eq(err, DIVBY0);
  err = s21_mod(del, div, &res);
  ck_assert_int_eq(err, DIVBY0);
}
END_TEST

START_TEST(MOD_TEST) {
  decimal a = {{10, 0, 0, 0}};
  decimal b = {{3, 0, 0, 0}};
  decimal r = {{0, 0, 0, 0}};
  int err;

  err = s21_mod(a, b, &r);
  ck_assert_int_eq(err, 0);
  ck_assert_int_eq(r.bits[0], 1);

  a.bits[0] = 256;
  b.bits[0] = 240;
  err = s21_mod(a, b, &r);
  ck_assert_int_eq(err, 0);
  ck_assert_int_eq(r.bits[0], 16);

  a.bits[0] = 0xFFFFFFFF;
  a.bits[1] = 0xFFFFFFFF;
  a.bits[2] = 0xFFFFFFFF;
  b.bits[0] = 1;
  setDecimalExp(&b, 28);
  err = s21_mod(b, a, &r);
  ck_assert_int_eq(err, 0);
  ck_assert_int_eq(s21_is_equal(b, r), 1);
}
END_TEST

START_TEST(TRUNCATE_TEST) {
  decimal a = {{15000, 0, 0, 0}};
  decimal b = {{345678, 0, 0, 0}};
  decimal r = {{0, 0, 0, 0}};
  setDecimalExp(&a, 4);
  s21_truncate(a, &r);
  ck_assert_int_eq(r.bits[0], 1);
  setDecimalExp(&a, 5);
  s21_truncate(a, &r);
  ck_assert_int_eq(r.bits[0], 0);
  setDecimalExp(&b, 5);
  s21_truncate(b, &r);
  ck_assert_int_eq(r.bits[0], 3);
}
END_TEST

START_TEST(ROUND_TEST) {
  decimal a = {{15, 0, 0, 0}};
  decimal b = {{1500000, 0, 0, 0}};
  decimal r = {{0, 0, 0, 0}};
  setDecimalExp(&a, 1);
  s21_round(a, &r);
  ck_assert_int_eq(r.bits[0], 2);
  setDecimalExp(&b, 6);
  s21_round(b, &r);
  ck_assert_int_eq(r.bits[0], 2);
}
END_TEST

START_TEST(FLOOR_TEST) {
  decimal a = {{15, 0, 0, 0}};
  decimal b = {{1900000, 0, 0, 0}};
  decimal r = {{0, 0, 0, 0}};
  setDecimalExp(&a, 1);
  s21_floor(a, &r);
  ck_assert_int_eq(r.bits[0], 1);
  setDecimalExp(&b, 6);
  setDecimalSign(&b, 1);
  s21_floor(b, &r);
  ck_assert_int_eq(r.bits[0], 2);
  ck_assert_int_eq(getDecimalSign(r), 1);
}
END_TEST

START_TEST(BANK_ROUND) {
  decimal big = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 0}};
  decimal big_m_1 = {{UINT32_MAX - 1, UINT32_MAX, UINT32_MAX, 0}};
  decimal small = {{5, 0, 0, 0}};
  decimal res = {{0, 0, 0, 0}};
  setDecimalExp(&small, 3);
  s21_sub(big, small, &res);
  puts("");
  int r = s21_is_equal(big, res);
  ck_assert_int_eq(r, 1);
  setDecimalExp(&small, 1);
  s21_sub(big, small, &res);
  r = s21_is_equal(big_m_1, res);
  ck_assert_int_eq(r, 1);

  decimal two = {{2, 0, 0, 0}};
  decimal big2 = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 0}};
  s21_div(big2, two, &big2);
  decimal zero_3 = {{3, 0, 0, 0}};
  setDecimalExp(&zero_3, 2);
  s21_add(big2, zero_3, &big2);
  int err = s21_mul(big2, two, &res);
  ck_assert_int_eq(err, 1);
}

Suite *f_example_suite_create() {
  Suite *s1 = suite_create("Test_decimal");

  TCase *p_case = tcase_create("Core");

  tcase_set_timeout(p_case, 0);
  tcase_add_test(p_case, FLOAT_TO_INT);
  tcase_add_test(p_case, SUB_TEST);
  tcase_add_test(p_case, ADD_TEST);
  tcase_add_test(p_case, GREATER_TEST);
  tcase_add_test(p_case, EQUAL_TEST);
  tcase_add_test(p_case, MUL_TEST);
  tcase_add_test(p_case, NEGATE_TEST);
  tcase_add_test(p_case, TO_FROM_INT);
  tcase_add_test(p_case, TO_FROM_FLOAT);
  tcase_add_test(p_case, MOD_TEST);
  tcase_add_test(p_case, DIV_TEST);
  tcase_add_test(p_case, TRUNCATE_TEST);
  tcase_add_test(p_case, ROUND_TEST);
  tcase_add_test(p_case, FLOOR_TEST);
  tcase_add_test(p_case, BANK_ROUND);
  tcase_add_test(p_case, LESS_OR_EQUAL_TEST);

  suite_add_tcase(s1, p_case);
  return s1;
}

int main() {
  Suite *s1 = f_example_suite_create();

  SRunner *runner = srunner_create(s1);
  int number_failed;
  srunner_run_all(runner, CK_NORMAL);
  number_failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return number_failed == 0 ? 0 : 1;
}
