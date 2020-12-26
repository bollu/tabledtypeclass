ttr: ttr.cpp parser.h
	clang++ -g \
		-std=c++17 \
		-Wall -Werror \
		-fsanitize=address -fsanitize=undefined \
		-I./include \
		ttr.cpp -ottr
