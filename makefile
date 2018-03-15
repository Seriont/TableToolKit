all: csvmap csvreduce

csvreduce: parser.o main_reduce.o reduce_parser.o csv.o reduce.o
	g++ -pthread -std=c++14 -o csvreduce parser.o main_reduce.o reduce_parser.o csv.o reduce.o

csvmap: parser.o csv.o map.o main_map.o
	g++ -std=c++14 -o csvmap parser.o csv.o map.o main_map.o

main_reduce.o: main_reduce.cpp parser.h
	g++ -std=c++14 -o main_reduce.o -c main_reduce.cpp

reduce.o: reduce.cpp reduce.h reduce_parser.h
	g++ -std=c++14 -o reduce.o -c reduce.cpp

reduce_parser.o: reduce_parser.cpp reduce_parser.h parser.h
	g++ -std=c++14 -o reduce_parser.o -c reduce_parser.cpp

main_map.o: main_map.cpp map.h
	g++ -std=c++14 -o main_map.o -c main_map.cpp 

map.o: map.h map.cpp csv.h parser.h
	g++ -std=c++14 -o map.o -c map.cpp

parser.o: reduce_parser.h parser.h parser.cpp
	g++ -std=c++14 -o parser.o -c parser.cpp

csv.o: csv.h csv.cpp
	g++ -std=c++14 -o csv.o -c csv.cpp

clean:
	rm *.o csvmap csvreduce