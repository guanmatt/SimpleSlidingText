all: main get_key

main: main.cpp
	g++ -o main main.cpp -lncurses

get_key: get_key.cpp
	g++ -o get_key get_key.cpp -lncurses

.PHONY: clean
clean:
	rm -rf main get_key