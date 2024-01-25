main:
	mkdir -p bin/
	g++ src/construct.cpp  src/deconstruct.cpp  src/construct_debug.cpp src/reconstruct.cpp src/construct_flags.cpp -o bin/construct
