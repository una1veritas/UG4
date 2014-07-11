//============================================================================
// Name        : AutoEnumerator.cpp
// Author      : Sin
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
using namespace std;

typedef uint16_t uint16;

class Stack {
	static const uint16 min_size = 32;

	pair<uint16, uint16> * array;
	uint16 tally;
	uint16 limit;

	void init(uint16 n) {
		array = new pair<uint16, uint16>[n];
		tally = 0;
		limit = n;
	}


public:
	Stack(uint16 size = min_size) {
		init(size);
	}

	~Stack(void) {
		delete[] array;
		limit = 0;
		tally = 0;
	}

	void push(const pair<uint16, uint16> & x) {
		array[tally++] = x;
	}

	void pop(pair<uint16, uint16> & x) {
		x = array[--tally];
	}

	ostream & printOn(ostream & stream) {
		for(int i = 0; i < tally; i++) {
			stream << "[" << array[i].first << ", " << array[i].second << "] ";
		}
		return stream;
	}
};

int main(int argc, char * argv[]) {
	string example = "", label = "";

	if (argc <= 1) {
		std::cerr << "too few arguments." << std::endl;
		return 1;
	}

	example += argv[1];
	if ( (argc == 3) && (strlen(argv[2]) == example.length() + 1) ) {
		label += argv[2];
	} else {
		label = string(example.length()+1, '0');
	}
	cout << "example = " << example << ", label = " << label << std::endl;

	Stack stack;
	pair<uint16,uint16> x;
	x.first = 0; x.second = 1;
	stack.push(x);
	x.first = 7; x.second = 3;
	stack.push(x);
	stack.printOn(std::cout);

	return 0;
}
