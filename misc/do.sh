g++ -g -O0 -o $1 $1.cpp -std=c++0x `pkg-config --cflags --libs opencv`
