CFLAGS = -std=c++17 -fno-strict-aliasing -ffast-math -march=native -O3 -Wall -I "../"
OBJS = binary.o

all: ${OBJS}
	g++ ${CFLAGS} ${OBJS} 

binary.o: binary.cpp
	g++ ${CFLAGS} -c binary.cpp

clean:
	rm -f binary ${OBJS}
