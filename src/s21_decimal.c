

#include "s21_decimal.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "s21_arrays.h"
void printBits(const size_t size, const void *ptr, int sep_n) {
  unsigned char *b = (unsigned char *)ptr;
  unsigned char byte;
  int i, j;

  for (i = size - 1; i >= 0; i--) {
    for (j = 7; j >= 0; j--) {
      byte = (b[i] >> j) & 1;
      printf("%u", byte);
    }
    if (!(i % sep_n)) printf(" ");
  }
  puts("");
}
int getDecimalExp(decimal d) {
  int i = d.bits[3] << 1;
  int j = i >> 17;
  return j;
}

void setDecimalExp(decimal *d, int exp) { setBits(&d->bits[3], exp, 16, 8); }

int getDecimalSign(decimal d) { return getBits(&d.bits[3], 31, 1); }

void setDecimalSign(decimal *d, int sign) { setBits(&d->bits[3], sign, 31, 1); }

int move_scale(int cycles, s21_decimal *num) {
  uint32_t x[4] = {0};
  copyArray(num->bits, x, 3);
  for (int i = 0; i < cycles; ++i) {
    mul10(x, 4);
  }
  int new_scale = getDecimalExp(*num) + cycles;
  if (x[3] > 0 || new_scale > 28) {
    return CE;
  }
  copyArray(x, num->bits, 3);
  setDecimalExp(num, new_scale);
  return OK;
}

int eq_scale(decimal *x, decimal *y) {
  int scale1 = getDecimalExp(*x);
  int scale2 = getDecimalExp(*y);
  int ret = 0;
  if (scale1 > scale2) {
    ret = move_scale(scale1 - scale2, y);
  } else if (scale2 > scale1) {
    ret = move_scale(scale2 - scale1, x);
  }
  return ret;
}

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  init_0(dst->bits, 4);
  if (src < 0) {
    dst->bits[3] = INT32_MIN;
  }
  dst->bits[0] = abs(src);

  return OK;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  s21_truncate(src, &src);
  if (src.bits[1] || src.bits[2]) {
    return CE;
  }
  int value = src.bits[0];
  if (getDecimalSign(src)) {
    if (value == INT32_MIN)
      *dst = INT32_MIN;
    else
      *dst = -1 * value;
  } else {
    if (getBits(src.bits, 31, 1)) {
      return CE;
    }
    *dst = value;
  }

  return OK;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  init_0(dst->bits, 4);
  int sign = getBits(&src, 31, 1);
  if (sign) {
    src *= -1;
  }
  char ch[100];

  snprintf(ch, sizeof(ch), "%.7f", src);
  src = atof(ch);
  if (strcmp("inf", ch) == 0 || strcmp("nan", ch) == 0 ||
      strcmp("-inf", ch) == 0)
    return CE;
  int exp = strlen(ch) - (strchr(ch, '.') - ch) - 1;
  for (int i = strlen(ch) - exp - 1; (size_t)i < strlen(ch); ++i) {
    ch[i] = ch[i + 1];
  }

  for (size_t i = strlen(ch) - 1; i > 0 && ch[i] == '0' && exp > 0; --i) {
    exp--;
    ch[i] = '\0';
  }
  if (exp > 28 || src >= 79228162514264337593543950336.0f ||
      (0 < src && src < 1e-28))
    return CE;
  size_t digits_len = strlen(ch);
  for (size_t i = 0; i < digits_len; ++i) {
    mul10(dst->bits, 3);
    bit_add(dst->bits, ch[i] - '0', 3);
  }

  setDecimalExp(dst, exp);
  setDecimalSign(dst, sign);

  return OK;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  if (dst == NULL) {
    return CE;
  }
  uint32_t x[3];
  copyArray(src.bits, x, 3);
  int p = 0;
  double res = 0;
  while (!is_0(x, 3)) {
    float d = getBits(x, 0, 1) * powf(2, p);
    res += d;
    shiftr1(x, 3);
    if (res >= FLT_MAX && !is_0(x, 3)) {
      *dst = 0;
      return getDecimalSign(src) ? TOOSMALL : TOOLARGE;
    }
    p++;
  }

  int _exp = getDecimalExp(src);
  res /= pow(10.0, _exp);
  if (res > FLT_MAX) {
    *dst = 0;
    return getDecimalSign(src) ? TOOSMALL : TOOLARGE;
  }
  res *= getDecimalSign(src) ? -1 : 1;
  *dst = (float)res;

  return OK;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  eq_scale(&value_1, &value_2);
  init_0(result->bits, 4);
  int s1 = getDecimalSign(value_1);
  int s2 = getDecimalSign(value_2);
  int sign_diff = 0;
  int res = OK;
  if (!s1 && s2) {
    sign_diff = 1;
    s21_negate(value_2, &value_2);
  }
  if (s1 && !s2) {
    sign_diff = 1;
    s21_negate(value_1, &value_1);
  }
  if (sign_diff) {
    if (s21_is_greater(value_1, value_2)) {
      res = s21_sub(value_1, value_2, result);
      s21_negate(*result, result);
      return res;
    } else if (s21_is_greater(value_2, value_1)) {
      res = s21_sub(value_2, value_1, result);
      s21_negate(*result, result);
      return res;
    } else {
      init_0(result->bits, 4);
      return OK;
    }
  }
  uint32_t x[7] = {0};
  uint32_t y[7] = {0};

  copyArray(value_1.bits, x, 3);
  copyArray(value_2.bits, y, 3);
  int exp_x = getDecimalExp(value_1);
  int exp_y = getDecimalExp(value_2);
  int max_scale = eq_scale_arr(x, y, exp_x, exp_y, 7);

  bit_add_arr(x, y, 7);
  reduce_scale_arr(x, 7, &max_scale);
  div_mod10(x, 7, &max_scale);
  setDecimalSign(result, s1 & s2);
  if (max_scale > 28 || x[3] > 0) {
    if (getDecimalSign(*result)) {
      return TOOSMALL;
    } else {
      return TOOLARGE;
    }
  } else {
    copyArray(x, result->bits, 3);

    setDecimalExp(result, max_scale);
  }

  return res;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int ret = 0;
  init_0(result->bits, 4);
  if (getDecimalSign(value_1) == 0 && getDecimalSign(value_2) == 0) {
    if (s21_is_equal(value_1, value_2)) {
      return 0;
    } else if (s21_is_greater(value_1, value_2)) {
      uint32_t x[7] = {0};
      uint32_t y[7] = {0};
      copyArray(value_1.bits, x, 3);
      copyArray(value_2.bits, y, 3);
      int max_scale =
          eq_scale_arr(x, y, getDecimalExp(value_1), getDecimalExp(value_2), 7);
      bit_sub_arr(x, y, 7);
      reduce_scale_arr(x, 7, &max_scale);
      div_mod10(x, 7, &max_scale);
      copyArray(x, result->bits, 3);
      setDecimalExp(result, max_scale);

      return ret;
    } else {
      ret = s21_sub(value_2, value_1, result);
      s21_negate(*result, result);
    }
  } else if (getDecimalSign(value_1) == 1 && getDecimalSign(value_2) == 1) {
    if (s21_is_greater(value_1, value_2)) {
      s21_negate(value_2, &value_2);
      s21_negate(value_1, &value_1);
      ret = s21_sub(value_2, value_1, result);
    } else if (s21_is_greater(value_2, value_1)) {
      s21_negate(value_2, &value_2);
      s21_negate(value_1, &value_1);
      ret = s21_sub(value_1, value_2, result);
      setDecimalSign(result, 1);
    }
  } else if (getDecimalSign(value_1) && !getDecimalSign(value_2)) {
    s21_negate(value_1, &value_1);
    ret = s21_add(value_1, value_2, result);
    s21_negate(*result, result);
  } else {
    s21_negate(value_2, &value_2);
    ret = s21_add(value_1, value_2, result);
  }
  if (ret != 0) ret = getDecimalSign(*result) ? 2 : 1;
  return ret;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
  if (result != NULL) {
    copyArray(value.bits, result->bits, 4);
    setDecimalSign(result, !getDecimalSign(value));
    return OK;
  } else {
    return CE;
  }
}

int s21_is_equal(s21_decimal num1, s21_decimal num2) {
  s21_decimal tmp_num1 = num1, tmp_num2 = num2;
  if (eq_scale(&tmp_num1, &tmp_num2)) {
    return 0;
  }
  int ret;
  int is_01 = is_0(num1.bits, 3);
  int is_02 = is_0(num2.bits, 3);
  if (is_01 && is_02) {
    ret = 1;
  } else if (is_01 || is_02) {
    ret = 0;
  } else if (getDecimalSign(num1) != getDecimalSign(num2)) {
    ret = 0;
  } else {
    if ((tmp_num1.bits[0] != tmp_num2.bits[0])) {
      ret = 0;
    } else if (tmp_num1.bits[1] != tmp_num2.bits[1]) {
      ret = 0;
    } else if (tmp_num1.bits[2] != tmp_num2.bits[2]) {
      ret = 0;
    } else if (tmp_num1.bits[3] != tmp_num2.bits[3]) {
      ret = 0;
    } else {
      ret = 1;
    }
  }
  return ret;
}

int s21_is_not_equal(s21_decimal num1, s21_decimal num2) {
  return s21_is_equal(num1, num2) ? 0 : 1;
}

int s21_is_greater_or_equal(s21_decimal num1, s21_decimal num2) {
  return s21_is_equal(num1, num2) || s21_is_greater(num1, num2);
}

int s21_is_greater(s21_decimal x, s21_decimal y) {
  int res = 0;
  eq_scale(&x, &y);
  if (getDecimalSign(x) == 1 && getDecimalSign(y) == 0) {
    res = 0;
  } else if (getDecimalSign(x) == 0 && getDecimalSign(y) == 1) {
    res = 1;
  } else {
    for (int i = 2; i >= 0; i--) {
      if (x.bits[i] > y.bits[i]) {
        res = getDecimalSign(x) ? 0 : 1;
        break;
      } else if (y.bits[i] > x.bits[i]) {
        res = getDecimalSign(y) ? 1 : 0;
        break;
      }
    }
  }
  return res;
}

int s21_is_less(s21_decimal num1, s21_decimal num2) {
  return !s21_is_greater_or_equal(num1, num2);
}

int s21_is_less_or_equal(s21_decimal num1, s21_decimal num2) {
  return !s21_is_greater(num1, num2);
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int exp = getDecimalExp(value);
  int sign = getDecimalSign(value);
  *result = value;
  if (exp != 0) {
    int tmp_int;
    for (int i = 0; i < exp; i++) {
      uint64_t u_num;
      u_num = result->bits[2];
      for (int j = 2; j >= 0; j--) {
        if (j == 0) {
          result->bits[j] = u_num / 10;
        } else {
          tmp_int = u_num % 10;
          result->bits[j] = u_num / 10;
          u_num = tmp_int * (4294967296) + result->bits[j - 1];
        }
      }
    }
    result->bits[3] = 0;
  }

  if (sign) setDecimalSign(result, 1);
  return OK;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
  int exp = getDecimalExp(value);
  *result = value;
  if (exp > 0) {
    int sign = getDecimalSign(*result);
    if (sign) setDecimalSign(result, 0);
    s21_truncate(*result, result);
    if (sign) {
      s21_decimal tmp = {{1, 0, 0, 0}};
      s21_add(*result, tmp, result);
    }
    if (sign) setDecimalSign(result, 1);
  }
  return 0;
}

int s21_round(s21_decimal value, s21_decimal *result) {
  int exp = getDecimalExp(value);
  copyArray(value.bits, result->bits, 4);
  if (exp > 0) {
    int sign = getDecimalSign(*result);
    if (sign) setDecimalSign(result, 0);
    result->bits[3] -= 1 << 16;
    s21_truncate(*result, result);
    s21_decimal last_digit = {{0, 0, 0, 0}};
    s21_decimal ten = {{10, 0, 0, 0}};
    s21_mod(*result, ten, &last_digit);
    result->bits[3] += 1 << 16;
    s21_truncate(*result, result);
    if (last_digit.bits[0] >= 5) {
      s21_decimal tmp = {{1, 0, 0, 0}};
      s21_add(*result, tmp, result);
    }
    if (sign) setDecimalSign(result, 1);
  }
  return 0;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  init_0(result->bits, 4);
  uint32_t val1[7] = {0};
  uint32_t val2[7] = {0};
  uint32_t res[7] = {0};
  copyArray(value_1.bits, val1, 3);
  copyArray(value_2.bits, val2, 3);
  bit_mul_arr(val1, val2, res, 7);
  int exp = getDecimalExp(value_1) + getDecimalExp(value_2);
  if (getDecimalSign(value_1) != getDecimalSign(value_2))
    setDecimalSign(result, 1);
  if (res[3] || res[4] || res[5] || res[6]) {
    return getDecimalSign(*result) ? TOOSMALL : TOOLARGE;
  } else {
    reduce_scale_arr(res, 7, &exp);
  }
  if (exp > 28) {
    return getDecimalSign(*result) ? TOOSMALL : TOOLARGE;
  } else {
    div_mod10(res, 7, &exp);
    copyArray(res, result->bits, 3);
    setDecimalExp(result, exp);
  }
  return OK;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (is_0(value_2.bits, 3)) {
    return DIVBY0;
  }
  if (is_0(value_1.bits, 3)) {
    init_0(result->bits, 4);
    return OK;
  }
  init_0(result->bits, 4);
  size_t size = 7;

  uint32_t *a1 = calloc(size, sizeof(uint32_t));
  copyArray(value_1.bits, a1, 3);
  uint32_t *a2 = calloc(size, sizeof(uint32_t));
  copyArray(value_2.bits, a2, 3);

  uint32_t *res = calloc(size, sizeof(uint32_t));
  uint32_t *mod = calloc(size, sizeof(uint32_t));
  uint32_t *div = calloc(size, sizeof(uint32_t));
  eq_scale_arr(a1, a2, getDecimalExp(value_1), getDecimalExp(value_2), size);

  int res_exp = 0;
  while (cmp(a1, a2, size) < 0) {
    mul10(a1, size);
    res_exp++;
  }

  do {
    bit_div_mod_arr(a1, a2, div, mod, size);
    mul10(mod, size);
    copyArray(mod, a1, size);
    bit_add_arr(res, div, size);
    mul10(res, size);
    res_exp++;
  } while (!is_0(mod, size) && res_exp <= 29);

  reduce_scale_arr(res, size, &res_exp);
  div_mod10(res, size, &res_exp);
  int ret = 0;
  if (!res[3] && res_exp <= 28) {
    copyArray(res, result->bits, 3);
    setDecimalExp(result, res_exp);
    setDecimalSign(result, getDecimalSign(value_1) ^ getDecimalSign(value_2));
  } else {
    ret = getDecimalSign(value_1) == getDecimalSign(value_2) ? TOOLARGE
                                                             : TOOSMALL;
  }
  free(a1);
  free(a2);
  free(res);
  free(mod);
  free(div);
  return ret;
}

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (is_0(value_2.bits, 3)) {
    return DIVBY0;
  }
  if (s21_is_less(value_1, value_2)) {
    copyArray(value_1.bits, result->bits, 4);
    return OK;
  }
  init_0(result->bits, 4);
  int s2 = getDecimalExp(value_2);
  int s1 = getDecimalExp(value_1);
  int scale = s1 > s2 ? s1 : s2;
  size_t size = 6;
  uint32_t *x = calloc(size, sizeof(uint32_t));
  copyArray(value_1.bits, x, 3);
  uint32_t *y = calloc(size, sizeof(uint32_t));
  copyArray(value_2.bits, y, 3);
  uint32_t *res = calloc(size, sizeof(uint32_t));
  eq_scale_arr(x, y, s1, s2, size);

  bit_mod_arr(x, y, res, size);
  reduce_scale_arr(res, size, &scale);
  copyArray(res, result->bits, 3);
  setDecimalExp(result, scale);
  setDecimalSign(result, getDecimalSign(value_2));
  free(x);
  free(y);
  free(res);
  return OK;
}
