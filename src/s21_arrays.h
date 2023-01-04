

#ifndef SRC_S21_ARRAYS_H_
#define SRC_S21_ARRAYS_H_
//

#include <stdint.h>
#include <stdlib.h>
int reduce_scale_arr(uint32_t *arr, size_t size, int *scale);

uint32_t getBits(const void *ptr, int offset, int n);

void setBits(const void *dest, uint32_t bits, int offset, int n);

int shiftl(void *object, size_t size, int n);

void shiftl1(uint32_t *object, size_t size);

void shiftr1(uint32_t *object, size_t size);

void OR(void *arr1, void *arr2, void *res, size_t size);

void XOR(void *arr1, void *arr2, void *res, size_t size);

void AND(void *arr1, void *arr2, void *res, size_t size);

void NOT(void *arr, void *res, size_t size);

int is_0(void *arr, size_t size);

void bit_add(void *value_1, uint32_t number, size_t arr_size);

void bit_add_arr(void *res_arr, void *number, size_t arr_size);

void bit_sub_arr(uint32_t *res_arr, uint32_t *number, size_t arr_size);

void bit_mod_arr(uint32_t *arr1, uint32_t *arr2, uint32_t *res, size_t size);

void bit_div_mod_arr(uint32_t *arr1, uint32_t *arr2, uint32_t *div,
                     uint32_t *mod, size_t size);

void bit_mul_arr(uint32_t *val1, uint32_t *val2, uint32_t *res, size_t size);

void bit_div_arr(uint32_t *arr1, uint32_t *arr2, uint32_t *res, size_t size);

void bit_div_mod(uint32_t *arr1, uint32_t number, uint32_t *div, uint32_t *mod,
                 size_t size);

void bit_div(uint32_t *arr1, uint32_t number, uint32_t *res, size_t size);

int bank_round_arr(uint32_t *arr, int *scale, size_t size);

int cmp(const uint32_t *a, const uint32_t *b, size_t size);

void init_0(uint32_t *arr, int size);

void move_scale_arr(int cycles, uint32_t *arr, size_t size);

int eq_scale_arr(uint32_t *x, uint32_t *y, int scalex, int scaley, size_t size);

void copyArray(const uint32_t *from, uint32_t *to, size_t len);

void mul10(uint32_t *x, int size);

void div10(uint32_t *x, size_t size);

void div_mod10(uint32_t *x, size_t size, int *exp);

#endif  //  SRC_S21_ARRAYS_H_
