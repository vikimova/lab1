all:
	g++ main.cpp game.cpp -o game.o -lncurses
run:
	./game.o
test:
	g++ ./tests/main.cpp game.cpp -o test.o -lncurses -lgtest -lgmock -pthread
run_test:
	./test.o
