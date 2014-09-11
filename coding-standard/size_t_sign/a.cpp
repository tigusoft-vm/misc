#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>




/***

signed long a; size_t b;

if ( a < b ) ... // error

if ( (xsize_t)a < b ) // ok
if ( (xsize_t)a <= b ) // ok
if ( (xsize_t)a > b ) // ok
if ( (xsize_t)a >= b ) // ok

if ( b < a(xsize_t) ) // ok
if ( b <= a(xsize_t) ) // ok
if ( b > a(xsize_t) ) // ok
if ( b >= a(xsize_t) ) // ok

*/


/** 
This is a speciall struct that is suppose to:
- allow comparing integrals that can be negative with a size_t
- also possibly do a (partial) check is the value used makes sense to be used in size_t context

The full version of size_t but with sign.
Use this ONLY for casting for ONLY comparsions.
This type is allowed to give you garbaged value in other cases, e.g. if you would like to store large negative
number here, then it only guarantees to be able to confirm that will be smaller then your size_t (e.g. we could store here value like -1)
*/
struct xsize_t_struct { 
	private:
		size_t m_value; // the value (without size) minus zero is not allowed.
		bool m_minus; // 0=plus.  if value==0 then m_minus=0 always!
		
	public:
		xsize_t_struct() { }
		xsize_t_struct(size_t t) { m_minus=0; m_value=t; }

		void my_abort(const char *msg) {
			std::cout << "size_t arithmetics error in " << __FUNCTION__ << " " << __FILE__<<" : " << msg << std::endl;
			abort();
		}

		// TODO also for signed long int
		xsize_t_struct(signed long long int v) { 
			if (v<0) { 
				m_minus = 1; 
				// if (v < limit_lowest) my_abort("Underflow: value is too small (negative) to be placed in signed-size_t."); // not needed since 
				// we are not guaranteeing to store other values then needed for comparsing, so no need for this check
				m_value = -v;
			}
			else {
				const size_t limit_max = std::numeric_limits<size_t>::max(); 
				m_minus = 0; 
				if (v > (long long int)limit_max) my_abort("Overflow: value is too big to be placed in signed-size_t, "
					"you are probably doing something wrong if you were about to compare it to size_t then.");
				m_value = v;
			}
		}

		#define ATTR

		ATTR bool operator<=(size_t b) const { if (m_minus) return 1; return m_value <= b; }
		ATTR bool operator<(size_t b) const { if (m_minus) return 1; return m_value < b; }
		ATTR bool operator>=(size_t b) const { if (m_minus) return 0; return m_value >= b; }
		ATTR bool operator>(size_t b) const { if (m_minus) return 0; return m_value > b; }

		ATTR operator size_t() { if (m_minus) my_abort("Trying to convert negative value to size_t!"); return m_value; }

		#undef ATTR

		// TEST CASE it -2,-1,0,+1,+2  N * N tests
};


typedef xsize_t_struct xsize_t; // TODO sfinae

#ifdef ALLOWED_BOOST
	#include <boost/integer.hpp>

	template <typename T> 
	void f(typename T::foo) {} // Definition #1

	template <typename T> 
	void f(T) {}                // Definition #2
	 
	// typedef boost::int_t< 128 >::fast xsize_t;  // TODO use SFINAE

//			f<Test>(10); // Call #1.
//			f<int>(10);  // Call #2. Without error (even though there is no int::foo) thanks to SFINAE.


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

void FOO() { }

// TODO test case!
void main3() {
	int long long x = -3;
	vector<int> tab={0,1,2};
	// if ( x < tab.size()) { FOO(); } //  warning: comparison between signed and unsigned integer expressions [-Wsign-compare]
	if ( (xsize_t_struct) x < tab.size()) { FOO(); } 
	if ( (xsize_t_struct) x <= tab.size()) { FOO(); } 
	if ( (xsize_t_struct) x > tab.size()) { FOO(); } 
	if ( (xsize_t_struct) x >= tab.size()) { FOO(); } 
}

int main() {
	cout << " --- START --- " << endl << endl << endl;
	cout << "size=" << sizeof( size_t ) << endl;
//	main1();
//	main2();
	main3();
}

