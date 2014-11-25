vision: vision.cpp remote.h
	clang++ vision.cpp -o vision -std=c++14 -lboost_system

clean:
	rm vision
