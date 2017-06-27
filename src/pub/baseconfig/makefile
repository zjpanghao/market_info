
INCLUDE_PATH=./

target : main.out clean

main.out : main.o base_config.o
	g++ -g main.o base_config.o -o main.out

main.o : main.cc
	g++ -g -I${INCLUDE_PATH} -c main.cc -o main.o

base_config.o : base_config.cc ${INCLUDE_PATH}/base_config.h
	g++ -g -I${INCLUDE_PATH} -c base_config.cc -o base_config.o

clean:
	rm *.o -f

