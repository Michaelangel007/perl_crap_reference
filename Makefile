all: bin bin/test3fix bin/test3opt1 bin/test3opt2 bin/test3opt3 bin/test3opt4 bin/test3opt5 bin/test3opt6

clean:
	rm bin/test3fix bin/test3opt1 bin/test3opt2 bin/test3opt3 bin/test3opt4 bin/test3opt5 bin/test3opt6

CC=gcc

# -Wall turn on all warnings
# Disable bogus: "warning: always_inline function might not be inlinable [-Wattributes]"
# -Wno-attributes
H_INC=$(wildcard src/*.h)
CFLAGS=-O3 -Wall -Wno-attributes -Isrc/
LIB_OMP=-fopenmp

bin:
	test -d bin || mkdir bin

bin/test3fix: src/test3_fixed.c $(H_INC)
	$(CC) $(CFLAGS) $< -o $@

bin/test3opt1: src/test3_opt1.c $(H_INC)
	$(CC) $(CFLAGS) $< -o $@

bin/test3opt2: src/test3_opt2.c $(H_INC)
	$(CC) $(CFLAGS) $< -o $@

bin/test3opt3: src/test3_opt3.c $(H_INC)
	$(CC) $(CFLAGS) $< -o $@

bin/test3opt4: src/test3_opt4.c $(H_INC)
	$(CC) $(CFLAGS) $< -o $@

bin/test3opt5: src/test3_opt5.c $(H_INC)
	$(CC) $(CFLAGS) $< -o $@

bin/test3opt6: src/test3_opt6.c $(H_INC)
	$(CC) $(CFLAGS) $(LIB_OMP) $< -o $@

