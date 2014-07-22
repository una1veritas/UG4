//============================================================================
// Name        : AutoEnumerator.cpp
// Author      : Sin
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

typedef uint16_t uint16;

template <class TYPE>
class Stack {
private:

	vector<TYPE> array;

	void init(uint16 n) {
		array.clear();
	}


public:
	Stack<TYPE>(uint16 n = 0) {
		init(n);
	}

	void push(const TYPE & elem) {
		array.push_back(elem);
	}

	TYPE & pop(void) {
	    if (array.empty()) {
	        throw out_of_range("Stack<>::pop(): empty stack");
	    }
		// remove last element
	    return array.pop_back();
	}

	ostream & printOn(ostream & stream) {
		for(int i = 0; i < array.size(); i++) {
			stream << "[";
			stream << array[i];
			stream << ", ";
			stream << array[i];
			stream << "] ";
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

	Stack< pair<uint16,uint16> > stack;
	pair<uint16,uint16> x;
	x.first = 0; x.second = 1;
	stack.push(x);
	x.first = 7; x.second = 3;
	stack.push(x);
	stack.printOn(std::cout);

	return 0;
}
