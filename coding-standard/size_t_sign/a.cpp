#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

#define ALLOWED_BOOST 1 // XXX

#ifdef ALLOWED_BOOST
	#include <boost/integer.hpp>
	typedef boost::int_t< 128 >::fast msize_t; 
	// the full version of size_t but with sign
#else
	typedef signed long long int msize_t;
	// TODO static assert that this type can handle minus MAX_SIZE too
#endif

using namespace std;

void melt(const vector<string> &objects, int t) { // t = room temperature
	cout << "At temperature = " << t << " : " << endl;

	for (size_t ix=0; ix<objects.size(); ++ix) {

		// this objects are safe from melting at this and higher temperature:

//		msize_t ix_temp = ix;		if ( ix_temp > t ) break; 

		if ( ((msize_t)ix) > t ) break; 

		cout << " * " << objects[ix] << " ... melts" << endl;
	}

	cout << ";" << endl;
}

void main1() {
	vector<string> objects = { "snow", "icecube", "hard-ice", "iced-gell", "gell"};
	melt( objects , -10); // nothing melts
	melt( objects , 2); // snow and icecube melt
	melt( objects , 10); // everything melted
}

template <class T>
size_t make_size_t(T val) {
	if (val<0) {
		ostringstream oss; oss<<"Negative size_t value attempted: " << val; 
		throw std::runtime_error(oss.str());
	}
	cout << SIZE_MAX << endl;
	cout << (msize_t)SIZE_MAX << endl;
	if (val > (msize_t)SIZE_MAX) {
		ostringstream oss; oss<<"Overflow size_t value attempted: " << val; 
		throw std::runtime_error(oss.str());
	}
	return val;
}

void func( size_t ix ) {
	vector<int> tab;
	if (false) tab.at(ix);
}

void main2() {
	{
		int temp = 15;
		func( make_size_t(temp) );
	}
	
	{
		long long int temp = 999999999999LL;
		func( make_size_t(temp) );	
	}
}

int main() {
	cout << " --- START --- " << endl;
	cout << "size=" << sizeof( size_t ) << endl;
	main1();
	main2();
}

