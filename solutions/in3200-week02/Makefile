CC := gcc
FLAGS := -Wall
PROGRAMS := w2_e1_binary_rw w2_e2_memcpy w2_e3_solve

.PHONY : all clean

all: $(PROGRAMS)

w2_e1_binary_rw : w2_e1_binary_rw.c
	$(CC) $(FLAGS) w2_e1_binary_rw.c -o w2_e1_binary_rw

w2_e2_memcpy : w2_e2_memcpy.c
	$(CC) $(FLAGS) w2_e2_memcpy.c -o w2_e2_memcpy

w2_e3_solve : w2_e2_memcpy.c
	$(CC) $(FLAGS) w2_e3_solve.c -o w2_e3_solve -lm -O3

clean :
	rm -f *.o $(PROGRAMS)

all:
	make w2_e1_binary_rw w2_e2_memcpy w2_e3_solve
