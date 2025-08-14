#include "scc/FileManager/MemoryBuffer.h"
#include <iostream>

int main() {
	try {
		scc::MemoryBuffer("test");
	}catch (...) {
		std::cout << "nooop" << std::endl;
		return 1;
	}
	std::cout << "yeeppp" << std::endl;
	return 5;
}
