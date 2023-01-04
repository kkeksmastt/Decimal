

#include "s21_arrays.h"

#include <stdio.h>
#include <string.h>

#include "s21_decimal.h"

uint32_t getBits(const void *ptr, int offset, int n) {
  uint32_t num = *(uint32_t *)ptr;
  uint32_t buf = num;
  uint32_t mask = 0xFFFFFFFF >> (32 - n);
  mask <<= offset;
  buf &= mask;
  return buf >> offset;
}

void setBits(const void *dest, uint32_t bits, int offset, int n) {
  uint32_t mask = 0xFFFFFFFF >> (32 - n);
  mask <<= offset;
  bits <<= offset;
  bits &= mask;
  *(uint32_t *)dest &= ~mask;
  *(uint32_t *)dest |= bits;
}

int shiftl(void *object, size_t size, int n) {
  if ((size_t)n > 32 * size) {
    puts("dont shift more than arr size");
    return 1;
  }
  for (int i = 0; i < n; ++i) shiftl1(object, size);
  return 0;
}

void shiftl1(uint32_t *arr, size_t size) {
  for (size_t i = size - 1; i >= 1; --i) {
    arr[i] <<= 1;
    arr[i] += (arr[i - 1] & (1u << 31)) >> 31;
  }
  arr[0] <<= 1;
}

void shiftr1(uint32_t *arr, size_t size) {
  for (size_t i = 0; i < size - 1; ++i) {
    arr[i] >>= 1;
    arr[i] += (arr[i + 1] & (1u)) << 31;
  }
  arr[size - 1] >>= 1;
}

void OR(void *arr1, void *arr2, void *res, size_t size) {
  uint32_t *a1 = (uint32_t *)arr1;
  uint32_t *a2 = (uint32_t *)arr2;
  uint32_t *r = (uint32_t *)res;
  for (size_t i = 0; i < size; i++) r[i] = a1[i] | a2[i];
}

void XOR(void *arr1, void *arr2, void *res, size_t size) {
  uint32_t *a1 = (uint32_t *)arr1;
  uint32_t *a2 = (uint32_t *)arr2;
  uint32_t *r = (uint32_t *)res;
  for (size_t i = 0; i < size; i++) r[i] = a1[i] ^ a2[i];
}

void AND(void *arr1, void *arr2, void *res, size_t size) {
  uint32_t *a1 = (uint32_t *)arr1;
  uint32_t *a2 = (uint32_t *)arr2;
  uint32_t *r = (uint32_t *)res;
  for (size_t i = 0; i < size; i++) {
    r[i] = a1[i] & a2[i];
  }
}

void NOT(void *arr, void *res, size_t size) {
  uint32_t *a = (uint32_t *)arr;
  uint32_t *r = (uint32_t *)res;
  for (size_t i = 0; i < size; i++) r[i] = ~a[i];
}

int is_0(void *arr, size_t size) {
  uint32_t *a = (uint32_t *)arr;
  for (size_t i = 0; i < size; i++) {
    if (a[i] > 0) return 0;
  }
  return 1;
}

void bit_add(void *value_1, uint32_t number, size_t arr_size) {
  uint32_t *x = (uint32_t *)value_1;
  uint32_t *y = calloc(arr_size, sizeof(uint32_t));
  y[0] = number;
  uint32_t *sum = calloc(arr_size, sizeof(uint32_t));
  uint32_t *carry = calloc(arr_size, sizeof(uint32_t));
  XOR(x, y, sum, arr_size);
  AND(x, y, carry, arr_size);
  while (!is_0(carry, arr_size)) {
    shiftl(carry, arr_size, 1);
    for (size_t i = 0; i < arr_size; i++) {
      x[i] = sum[i];
      y[i] = carry[i];
    }
    XOR(x, y, sum, arr_size);
    AND(x, y, carry, arr_size);
  }
  for (size_t i = 0; i < arr_size; i++) {
    x[i] = sum[i];
  }
  free(carry);
  free(sum);
  free(y);
}

void bit_add_arr(void *res_arr, void *number, size_t arr_size) {
  uint32_t *x = (uint32_t *)res_arr;
  uint32_t *y = (uint32_t *)calloc(arr_size, sizeof(uint32_t));
  copyArray(number, y, arr_size);
  uint32_t *sum = (uint32_t *)calloc(arr_size, sizeof(uint32_t));
  uint32_t *carry = (uint32_t *)calloc(arr_size, sizeof(uint32_t));

  XOR(x, y, sum, arr_size);
  AND(x, y, carry, arr_size);
  while (!is_0(carry, arr_size)) {
    shiftl(carry, arr_size, 1);
    for (size_t i = 0; i < arr_size; i++) {
      x[i] = sum[i];
      y[i] = carry[i];
    }
    XOR(x, y, sum, arr_size);
    AND(x, y, carry, arr_size);
  }
  for (size_t i = 0; i < arr_size; i++) {
    x[i] = sum[i];
  }
  free(y);
  free(carry);
  free(sum);
}

int cmp(const uint32_t *a, const uint32_t *b, size_t size) {
  for (int i = size - 1; i >= 0; i--) {
    if (a[i] == b[i] && i > 0) continue;
    if (a[i] > b[i]) return 1;
    if (a[i] < b[i]) return -1;
  }
  return 0;
}

void bit_sub_arr(uint32_t *res_arr, uint32_t *number, size_t arr_size) {
  uint32_t *x = (uint32_t *)res_arr;
  uint32_t *y = (uint32_t *)calloc(arr_size, sizeof(uint32_t));
  copyArray(number, y, arr_size);
  uint32_t *borrow = (uint32_t *)calloc(arr_size, sizeof(uint32_t));
  uint32_t *tmp = (uint32_t *)calloc(arr_size, sizeof(uint32_t));
  while (!is_0(y, arr_size)) {
    NOT(x, tmp, arr_size);
    AND(tmp, y, borrow, arr_size);
    XOR(x, y, x, arr_size);
    copyArray(borrow, y, arr_size);
    shiftl(y, arr_size, 1);
  }
  free(y);
  free(borrow);
  free(tmp);
}

void bit_mul_arr(uint32_t *val1, uint32_t *val2, uint32_t *res, size_t size) {
  uint32_t *a1 = calloc(size, sizeof(uint32_t));
  copyArray(val1, a1, size);
  uint32_t *a2 = calloc(size, sizeof(uint32_t));
  copyArray(val2, a2, size);
  while (!is_0(a2, size)) {
    if (getBits(a2, 0, 1)) {
      bit_add_arr(res, a1, size);
    }
    shiftl1(a1, size);
    shiftr1(a2, size);
  }
  free(a1);
  free(a2);
}

void bit_div_arr(uint32_t *arr1, uint32_t *arr2, uint32_t *res, size_t size) {
  uint32_t *a1 = calloc(size, sizeof(uint32_t));
  copyArray(arr1, a1, size);
  uint32_t *a2 = calloc(size, sizeof(uint32_t));
  copyArray(arr2, a2, size);
  uint32_t *acc = calloc(size, sizeof(uint32_t));
  uint32_t *rb = calloc(size, sizeof(uint32_t));
  uint32_t *buf = calloc(size, sizeof(uint32_t));
  uint32_t *one = calloc(size, sizeof(uint32_t));
  one[0] = 1;
  init_0(res, size);
  setBits(&(rb[size - 1]), 1, 31, 1); /*rb = 0x80000000*/
  while (!is_0(rb, size)) {           /*for*/
    shiftl1(acc, size);
    AND(rb, a1, buf, size);
    if (!is_0(buf, size)) OR(acc, one, acc, size);
    int cmp_res = cmp(acc, a2, size);
    if (cmp_res == 1 || cmp_res == 0) {
      bit_sub_arr(acc, a2, size);
      OR(res, rb, res, size);
    }
    shiftr1(rb, size); /*rb >>= 1*/
  }
  free(a1);
  free(a2);
  free(acc);
  free(rb);
  free(buf);
  free(one);
}

void bit_mod_arr(uint32_t *arr1, uint32_t *arr2, uint32_t *res, size_t size) {
  uint32_t *div = calloc(size, sizeof(uint32_t));
  uint32_t *mul = calloc(size, sizeof(uint32_t));
  uint32_t *a1 = calloc(size, sizeof(uint32_t));
  copyArray(arr1, a1, size);
  uint32_t *a2 = calloc(size, sizeof(uint32_t));
  copyArray(arr2, a2, size);

  bit_div_arr(a1, a2, div, size);
  bit_mul_arr(a2, div, mul, size);
  bit_sub_arr(a1, mul, size);
  copyArray(a1, res, size);

  free(div);
  free(mul);
  free(a1);
  free(a2);
}

void bit_div_mod_arr(uint32_t *arr1, uint32_t *arr2, uint32_t *div,
                     uint32_t *mod, size_t size) {
  uint32_t *mul = calloc(size, sizeof(uint32_t));
  uint32_t *a1 = calloc(size, sizeof(uint32_t));
  copyArray(arr1, a1, size);
  uint32_t *a2 = calloc(size, sizeof(uint32_t));
  copyArray(arr2, a2, size);

  bit_div_arr(a1, a2, div, size);
  bit_mul_arr(a2, div, mul, size);
  bit_sub_arr(a1, mul, size);
  copyArray(a1, mod, size);
  free(mul);
  free(a1);
  free(a2);
}

void bit_div_mod(uint32_t *arr1, uint32_t number, uint32_t *div, uint32_t *mod,
                 size_t size) {
  uint32_t *tmp = calloc(size, sizeof(uint32_t));
  tmp[0] = number;
  bit_div_mod_arr(arr1, tmp, div, mod, size);
  free(tmp);
}

void bit_div(uint32_t *arr1, uint32_t number, uint32_t *res, size_t size) {
  uint32_t *tmp = calloc(size, sizeof(uint32_t));
  tmp[0] = number;
  bit_div_arr(arr1, tmp, res, size);
  free(tmp);
}

void init_0(uint32_t *arr, int size) {
  for (int i = 0; i < size; ++i) {
    arr[i] = 0;
  }
}

void move_scale_arr(int cycles, uint32_t *arr, size_t size) {
  for (int i = 0; i < cycles; ++i) {
    mul10(arr, size);
  }
}

int eq_scale_arr(uint32_t *x, uint32_t *y, int scalex, int scaley,
                 size_t size) {
  int maxscale;
  if (scalex > scaley) {
    maxscale = scalex;
    move_scale_arr(scalex - scaley, y, size);
  } else if (scaley > scalex) {
    maxscale = scaley;
    move_scale_arr(scaley - scalex, x, size);
  } else {
    maxscale = scalex;
  }
  return maxscale;
}

int bank_round_arr(uint32_t *arr, int *scale, size_t size) {
  uint32_t *buf = calloc(size, sizeof(uint32_t));
  uint32_t *five = calloc(size, sizeof(uint32_t));
  uint32_t *mod = calloc(size, sizeof(uint32_t));
  five[0] = 5;
  copyArray(arr, buf, size);
  bit_div_mod(buf, 10, buf, mod, size);
  if (cmp(mod, five, size) > 0) {
    bit_add(buf, 1, size);
  } else if (cmp(mod, five, size) == 0) {
    if (buf[0] & 1) {
      bit_add(buf, 1, size);
    }
  }

  int ret = 0;
  if (!is_0(&buf[3], size - 3)) {
    ret = TOOLARGE;
  } else {
    copyArray(buf, arr, size);
    (*scale)--;
  }
  free(buf);
  free(five);
  free(mod);
  return ret;
}

void copyArray(const uint32_t *from, uint32_t *to, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    to[i] = from[i];
  }
}

void mul10(uint32_t *x, int size) {
  uint32_t *tmp = malloc(size * sizeof(uint32_t));
  copyArray(x, tmp, size);
  shiftl(x, size, 3);
  shiftl(tmp, size, 1);
  bit_add_arr(x, tmp, size);
  free(tmp);
}

void div10(uint32_t *x, size_t size) { bit_div(x, 10, x, size); }

int reduce_scale_arr(uint32_t *arr, size_t size, int *scale) {
  uint32_t *buf = calloc(size, sizeof(uint32_t));
  uint32_t *ten = calloc(size, sizeof(uint32_t));
  uint32_t *mod = calloc(size, sizeof(uint32_t));
  ten[0] = 10;
  copyArray(arr, buf, size);
  while (*scale > 0) {
    bit_div_mod_arr(buf, ten, buf, mod, size);
    if (!is_0(mod, size)) {
      mul10(buf, size);
      bit_add_arr(buf, mod, size);
      break;
    }
    (*scale)--;
  }
  copyArray(buf, arr, size);
  free(buf);
  free(ten);
  free(mod);
  return OK;
}

void div_mod10(uint32_t *x, size_t size, int *exp) {
  if (!is_0(x + 3, size - 3) && (*exp > 0)) {
    uint32_t tmp[size], tmp2[size];
    init_0(tmp, (int)size);
    init_0(tmp2, (int)size);
    for (int i = 0; !is_0(x + 3, size - 3) && *exp > 0; i++) {
      copyArray(x, tmp, size);
      div10(x, size);
      *exp -= 1;
    }
    copyArray(tmp, x, size);
    *exp += 1;
    bank_round_arr(x, exp, size);
  } else if (*exp == 29) {
    bank_round_arr(x, exp, size);
  }
}