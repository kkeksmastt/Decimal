CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11
TEST_LIBS = -lcheck -lm -lpthread #-lsubunit
OBJECTS = s21_decimal.o s21_arrays.o
SOURSES = s21_decimal.c s21_arrays.c
GCOVFLAGS = -fprofile-arcs -ftest-coverage



all: clean s21_decimal.a

s21_decimal.a: s21_decimal.o
	$(CC) $(CFLAGS) -g -c  $(SOURSES)
	ar rcs s21_decimal.a $(OBJECTS)

clean:
	rm -rf ./*.o ./*.a ./a.out gcov_test *.html *.css ./GcovReport ./*.gcno ./*.gcda ./report ./*.info ./s21_decimal.so ./*.dSYM ./s21_decimal_test ./CPPLINT* test

test: test.c s21_decimal.a
	$(CC) -c test.c $(TEST_LIBS) -o test.o
	$(CC) -g test.o s21_decimal.a $(TEST_LIBS) -o test

other_test: other_test.c s21_decimal.a
	$(CC) -c other_test.c $(TEST_LIBS) -o other_test.o
	$(CC) -g other_test.o s21_decimal.a $(TEST_LIBS) -o other_test

gcov_report: s21_decimal.a
	$(CC) $(CFLAGS) --coverage test.c s21_decimal.c s21_decimal.a $(TEST_LIBS) -o gcov_test
	chmod +x *
	./gcov_test
	lcov -t "gcov_test" -o gcov_test.info --no-external -c -d .
	genhtml -o report/ gcov_test.info
	open ./report/index.html

check: s21_decimal.a
	cppcheck *.h *.c
	cp ../materials/linters/CPPLINT.cfg CPPLINT.cfg
	python3 ../materials/linters/cpplint.py --extension=c *.c *.h
	$(CC) test.c s21_decimal.a -lcheck
	$(CC) $(CFLAGS) -c test.c -o test.o
	$(CC) test.o s21_decimal.a $(TEST_LIBS) -o test
	CK_FORK=no leaks --atExit -- ./test

rebuild: clean all

python_test:
	gcc -c -fPIC $(SOURSES) --coverage
	gcc $(OBJECTS) --coverage -shared -o s21_decimallib.so
	python3 test.py
	lcov -t "python_test" -o gcov_test.info --no-external -c -d .
	genhtml -o report/ gcov_test.info

