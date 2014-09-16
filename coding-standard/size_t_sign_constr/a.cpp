#include <vector>
#include <iostream>
using namespace std;

struct xsize {
	public:
		size_t s;
		explicit xsize(size_t size) : s(size) { }
		template <class TYPE> xsize(TYPE integral)=delete; // this is blocked
		xsize()=delete; // this is blocked too
};
 
int main() { vector<int> v;
	xsize x(v.size());
	if ( ((xsize)5) < 0 ) ; 
}

