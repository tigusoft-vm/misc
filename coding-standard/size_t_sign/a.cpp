#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <iomanip>


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

#define ATTR // TODO attribute operator?
struct xsize_t_struct { 
	private:
		size_t m_value; // the value (without size) minus zero is not allowed.
		bool m_minus; // 0=plus.  if value==0 then m_minus=0 always!
		
	public:
		xsize_t_struct() { }
		xsize_t_struct(size_t t) { m_minus=0; m_value=t; }

		void my_abort(const std::string & msg) {
			std::cout << "size_t arithmetics error in " << __FUNCTION__ << " " << __FILE__<<" : " << msg << std::endl;
			abort();
		}

		// TODO also for signed long int
		xsize_t_struct(signed long long int v) { 
			if (v<0) { 
				// we are not guaranteeing to store other values then needed for comparsing, so no need to check if -v is stored correctly
				m_minus = 1; 
				m_value = 1; // -v  we just store it as representing "-1". we avoid "-v" in case if that could wrap around,
				// and result in minus=1,value=0 which would represent "-0" and that is not allowed (because other code 
				// is not prepared to handle that correctly when comparing "-0" <= 0.
			}
			else {
				m_minus = 0; 
				const size_t limit_max = std::numeric_limits<size_t>::max(); 
				unsigned long long int vu = v;
				if (vu > limit_max) {
					my_abort(std::string("Overflow: value of v=") + std::to_string(v) + std::string(" is too big to be placed in signed-size_t, "
						"you are probably doing something wrong if you were about to compare it to size_t then"));
				}
				m_value = v;
			}
		}

		ATTR bool operator<=(size_t b) const  { if (m_minus) return 1; return m_value <= b; }
		ATTR bool operator<(size_t b) const   { if (m_minus) return 1; return m_value < b; }
		ATTR bool operator>=(size_t b) const  { if (m_minus) return 0; return m_value >= b; }
		ATTR bool operator>(size_t b) const   { if (m_minus) return 0; return m_value > b; }
		ATTR bool operator==(size_t b) const  { if (m_minus) return 0; return m_value == b; }
		ATTR bool operator!=(size_t b) const  { if (m_minus) return 1; return m_value != b; }

		ATTR operator size_t() { 
			// to catch any errors
			my_abort( std::string("Trying to convert the special signed-size_t object back to use it by value; "
				"That is not allowed, it shoul be used ONLY FOR COMPARSION with size_t."
				" value=")+std::to_string(m_value)+std::string(" back to size_t!"));				
			//if (m_minus) my_abort( std::string("Trying to convert negative value=")+std::to_string(m_value)+std::string(" back to size_t!"));
			return 0; // m_value; 
		}

		friend ATTR bool operator<=(size_t b, xsize_t_struct a);
		friend ATTR bool operator<(size_t b, xsize_t_struct a);
		friend ATTR bool operator>=(size_t b, xsize_t_struct a);
		friend ATTR bool operator>(size_t b, xsize_t_struct a);
		friend ATTR bool operator==(size_t b, xsize_t_struct a);
		friend ATTR bool operator!=(size_t b, xsize_t_struct a);

		// TEST CASE it -2,-1,0,+1,+2  N * N tests
};
#undef ATTR

#define ATTR // TODO attribute operator?
ATTR bool operator<=(size_t b, xsize_t_struct a)  { if (a.m_minus) return 0; return b <= a.m_value; }
ATTR bool operator<(size_t b, xsize_t_struct a)   { if (a.m_minus) return 0; return b < a.m_value; }
ATTR bool operator>=(size_t b, xsize_t_struct a)  { if (a.m_minus) return 1; return b >= a.m_value; }
ATTR bool operator>(size_t b, xsize_t_struct a)   { if (a.m_minus) return 1; return b > a.m_value; }
ATTR bool operator==(size_t b, xsize_t_struct a)  { if (a.m_minus) return 0; return b == a.m_value; }
ATTR bool operator!=(size_t b, xsize_t_struct a)  { if (a.m_minus) return 1; return b != a.m_value; }
#undef ATTR


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

template <class T> 
class fake_vector {
	public:
		size_t m_size;
		fake_vector() : m_size(0) { }
		void resize(size_t new_size) { m_size = new_size; }
		size_t size() const { return m_size; }
};

template <class T_VECTOR>
bool test_vector_size(long long int AI, size_t BV) {
	using namespace std; 
	std::cout << setw(20) << AI << " vs vector size BV="<< setw(20) << BV <<"... " << std::flush;
	T_VECTOR tab;
	tab.resize(BV);

	bool all_ok=1;

	#define COMPARE(OPERATOR, OPERATOR_NAME) \
	do { \
		bool c1 = ( ((xsize_t_struct)AI)  OPERATOR  (tab.size()) ); \
		bool c2 = ( (long double)AI)      OPERATOR  ((long double)BV); \
		bool OK = (c1==c2); \
		if (OK) cout << "ok" << OPERATOR_NAME << " "; \
		else { cout <<"\nFAILED AI="<<AI<<"  " << "(" << OPERATOR_NAME << ")" << "  " << "BV="<<BV<<" c1="<<c1<<" c2(double)="<<c2<<"! "; } \
		all_ok = all_ok && OK; \
	} while(0)
	COMPARE( < , "<" );
	COMPARE( <= , "<=" );
	COMPARE( > , ">" );
	COMPARE( > , ">=" );
	COMPARE( == , "==" );
	COMPARE( != , "!=" );
	#undef COMPARE

	#define COMPARE(OPERATOR, OPERATOR_NAME) \
	do { \
		bool c1 = ( (tab.size())       OPERATOR  ((xsize_t_struct)AI) ); \
		bool c2 = ( ((long double)BV)  OPERATOR  ((long double)AI) ); \
		bool OK = (c1==c2); \
		if (OK) cout << "ok" << OPERATOR_NAME << " "; \
		else { cout <<"\nFAILED BV="<<BV<<"  " << "(" << OPERATOR_NAME << ")" << "  " << "AI="<<AI<<" c1="<<c1<<" c2(double)="<<c2<<"! "; } \
		all_ok = all_ok && OK; \
	} while(0)
	COMPARE( < , "<" );
	COMPARE( <= , "<=" );
	COMPARE( > , ">" );
	COMPARE( > , ">=" );
	COMPARE( == , "==" );
	COMPARE( != , "!=" );
	#undef COMPARE

	cout << "." << endl;

	if (!all_ok) abort();
	return all_ok;
}

void main3() {
	int long long x = -3;
	vector<int> tab={0,1,2};
	// if ( x < tab.size()) { FOO(); } //  warning: comparison between signed and unsigned integer expressions [-Wsign-compare]
	if ( (xsize_t_struct) x < tab.size()) { FOO(); } 
	if ( (xsize_t_struct) x <= tab.size()) { FOO(); } 
	if ( (xsize_t_struct) x > tab.size()) { FOO(); } 
	if ( (xsize_t_struct) x >= tab.size()) { FOO(); } 

	typedef signed long long int integral;

	integral p31 = std::pow(2,31);
	integral p32 = std::pow(2,32);
	size_t min1 = std::numeric_limits<size_t>::min();
	size_t max1 = std::numeric_limits<size_t>::max();
	size_t min2 = std::numeric_limits<integral>::min();
	size_t max2 = std::numeric_limits<integral>::max();

	vector<integral> i_values = { 
		min2,min2+1,min2+2,min1,min1+1, 
		-p32-1, -p32, -p32+1, -p31-1, -p31, -p31+1, 
		-2,-1,
		0,+1,+2,+3, +100,
		+p31-1,+p31,+p31+1, +p32-1,+p32,+p32+1, 
		max1/2-1, max1/2, max2/2-1, max2/2, /* max2+... would go out */
		max1-1, max1, max2-1, max2, /* max2+... would go out */
		};

	vector<size_t> v_values = { 
		0,+1,+2,+3, +100,
		+p31-1,+p31,+p31+1, +p32-1,+p32,+p32+1, 
		max1/2-1, max1/2, max2/2-1, max2/2, max2/2+1, max2/2+2, 
		max1-1, max1, max2-1, max2, max2+1, max2+2 
		};


	for (auto i : i_values) std::cout<<i<<" "; std::cout << std::endl;
	for (auto v : v_values) std::cout<<v<<" "; std::cout << std::endl;
	
	bool all_ok = 1;
	for (auto i : i_values) { 
		for (auto v : v_values) {
			const bool v_huge = v > std::pow(2,16);
			if (!v_huge) {
				// cout << "v_huge="<<v_huge<<" real vector for v="<<v << endl;
				const bool ok = test_vector_size< vector<char> >(i,v);
				all_ok = all_ok && ok;
			} else {
				// cout << "v_huge="<<v_huge<<" FAKE vector for v="<<v << endl;
				const bool ok = test_vector_size< fake_vector<char> >(i,v);
				all_ok = all_ok && ok;
			}
		}	
	}

	if (!all_ok) {
		cout << "!!! SOME TESTS FAILED !!!" << endl;
	} else cout << "All good :)" << endl;

}

int main() {
	cout << " --- START --- " << endl << endl << endl;
	cout << "size=" << sizeof( size_t ) << endl;
//	main1();
//	main2();
	main3();
}

