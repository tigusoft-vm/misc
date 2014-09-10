#include <iostream>
#include <vector>

#define ALLOWED_BOOST 1 // XXX

#ifdef ALLOWED_BOOST
	#include <boost/integer.hpp>
	typedef boost::int_t< sizeof(size_t)*8 +1 >::fast msize_t; // the full version of size_t but with sign
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

int main() {
	cout << " --- START --- " << endl;

	vector<string> objects = { "snow", "icecube", "hard-ice", "iced-gell", "gell"};

	melt( objects , -10); // nothing melts
	melt( objects , 2); // snow and icecube melt
	melt( objects , 10); // everything melted
	
}

