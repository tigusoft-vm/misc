#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <iomanip>

#define ALLOWED_BOOST 1

#ifdef ALLOWED_BOOST 
#include <boost/multiprecision/cpp_int.hpp> 
#endif


#ifdef ALLOWED_BOOST 

typedef boost::multiprecision::checked_int1024_t t_safenumber;

// typedef boost::multiprecision::number<cpp_int_backend<16384, 16384, signed_magnitude, checked, void> >    t_safenumber;

#else

typedef long long double t_safenumber;

#endif

/***

size_t size;
int b;
if ( (opsize_t)size < b ) // ok

*/


/***
@struct
@brief you cast an size_t to this type, and then resulting type is comparable to all build-in integral types without warning nor overflows.

This is a speciall struct that is suppose to:
- allow comparing integrals that can be negative

Use this ONLY for casting for ONLY comparsions.
*/

#define ATTR __attribute__((pure)) // TODO attribute operator?
struct xsize_t_struct { 
	public: /* public to not mock around with friending templated operators */
		size_t m_v; // the value of size_t

	public:
		explicit xsize_t_struct(size_t size) : m_v(size) {} // only proper way to init this
		//xsize_t_struct(long long int size) : m_v(size) {} // why is this needed?
		template <class TYPE> xsize_t_struct(TYPE integral)=delete; // this is blocked
		xsize_t_struct()=delete; // this is blocked too

		// TEST CASE it -2,-1,0,+1,+2  N * N tests
};

template <class T_INT> bool operator< (T_INT value, xsize_t_struct size)   { std::cerr<<" {value="<<value<<"<"<<size.m_v<<" 1 } "; if (value<0) return 1; return value < size.m_v; }
template <class T_INT> bool operator< (xsize_t_struct size, T_INT value)   { std::cerr<<" {xsize="<<size.m_v<<" < value="<<value<<" 2 } "; if (value<0) return 0; return size.m_v < value; }

#undef ATTR



typedef xsize_t_struct xsize_t; // TODO sfinae

//#ifdef ALLOWED_BOOST
//	#include <boost/integer.hpp>

	// typedef boost::int_t< 128 >::fast xsize_t;  // TODO use SFINAE

//#else

	typedef signed long long int msize_t;

//	// TODO static assert that this type can handle minus MAX_SIZE too
//#endif

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

template <class T_VECTOR, class T_INT>
bool test_vector_size(T_INT AI, size_t BV, bool loud) {
	using namespace std; 
	if (loud) { std::cout << setw(20) << AI << " vs vector size BV="<< setw(20) << BV <<"... " << std::flush; }
	T_VECTOR tab;
	tab.resize(BV);

	bool all_ok=1;

	#define COMPARE(OPERATOR, OPERATOR_NAME) \
	do { \
		bool c1 = ( (AI)  OPERATOR  ((xsize_t_struct)tab.size()) ); \
		bool c2 = ( (long double)AI)      OPERATOR  ((long double)BV); \
		bool OK = (c1==c2); \
		if (OK) { if (loud) { cout << "ok" << OPERATOR_NAME << " "; } } \
		else { cout <<"\nFAILED AI="<<AI<<"  " << "(" << OPERATOR_NAME << ")" << "  " << "BV="<<BV<<" c1="<<c1<<" c2(double)="<<c2<<"! "; } \
		all_ok = all_ok && OK; \
	} while(0)

	COMPARE( < , "<" );
//	COMPARE( <= , "<=" );
//	COMPARE( > , ">" );
//	COMPARE( > , ">=" );
//	COMPARE( == , "==" );
//	COMPARE( != , "!=" );
	#undef COMPARE


	#define COMPARE(OPERATOR, OPERATOR_NAME) \
	do { \
		bool c1 = ( (xsize_t_struct)(tab.size())       OPERATOR  (AI) ); \
		bool c2 = ( ((long double)BV)  OPERATOR  ((long double)AI) ); \
		bool OK = (c1==c2); \
		if (OK) { if (loud) { cout << "ok" << OPERATOR_NAME << " "; } } \
		else { cout <<"\nFAILED BV="<<BV<<"  " << "(" << OPERATOR_NAME << ")" << "  " << "AI="<<AI<<" c1="<<c1<<" c2(double)="<<c2<<"! "; } \
		all_ok = all_ok && OK; \
	} while(0)
	COMPARE( < , "<" );
//	COMPARE( <= , "<=" );
//	COMPARE( > , ">" );
//	COMPARE( > , ">=" );
//	COMPARE( == , "==" );
//	COMPARE( != , "!=" );
	#undef COMPARE

	cout << "." << endl;

	if (!all_ok) abort();
	return all_ok;
}


template <class T_INT,  class T_I, class T_V>
bool RunTests(const T_I &i_values,  const T_V &v_values, bool loud) {
	cout<<"Array i_values for the integral numbers:      ";  for (auto i : i_values) std::cout<<i<<" ";  std::cout << std::endl;
	cout<<"Array v_values for the vector size_t numbers: ";  for (auto v : v_values) std::cout<<v<<" ";  std::cout << std::endl;
	cout<<"Testing on type: " << typeid(T_INT).name() << endl;
	bool all_ok = 1;
	for (auto i : i_values) { 
		for (auto v : v_values) {
			const bool v_huge = v > static_cast<long long int>(std::pow(2,16));
			if (!v_huge) {
				const bool ok = test_vector_size< vector<char> >( i.template convert_to<long long int>() , v.template convert_to<size_t>() , loud );
				all_ok = all_ok && ok;
			} else {
				const bool ok = test_vector_size< fake_vector<char> >( i.template convert_to<long long int>() , v.template convert_to<size_t>() , loud );
				all_ok = all_ok && ok;
			}
		}	
	}

	return all_ok;

}

void main3() {
	//int long long x = -3;
	//vector<int> tab={0,1,2};
	// if ( x < tab.size()) { FOO(); } //  warning: comparison between signed and unsigned integer expressions [-Wsign-compare]
/*	if ( (xsize_t_struct) x < tab.size()) { FOO(); } 
	if ( (xsize_t_struct) x <= tab.size()) { FOO(); } 
	if ( (xsize_t_struct) x > tab.size()) { FOO(); } 
	if ( (xsize_t_struct) x >= tab.size()) { FOO(); } 
*/
	typedef t_safenumber integral;

	integral p31 = static_cast<long long int>(std::pow(2,31));
	integral p32 = static_cast<long long int>(std::pow(2,32));
	typedef long long int big;
	integral min1 = std::numeric_limits<big>::min();
	integral max1 = std::numeric_limits<big>::max();
	integral min2 = std::numeric_limits<big>::min();
	integral max2 = std::numeric_limits<big>::max();

	vector<integral> i_values = { 
		min2,min2+1,min2+2,min1,min1+1, 
		-p32-1, -p32, -p32+1, -p31-1, -p31, -p31+1, 
		-2,-1,
		0,+1,+2,+3, +100,
		+p31-1,+p31,+p31+1, +p32-1,+p32,+p32+1, 
		max1/2-1, max1/2, max2/2-1, max2/2, /* max2+... would go out */
		max1-1, max1, max2-1, max2, /* max2+... would go out */
		};

	vector<integral> v_values = { 
		0,+1,+2,+3, +100,
		+p31-1,+p31,+p31+1, +p32-1,+p32,+p32+1, 
		max1/2-1, max1/2, max2/2-1, max2/2, max2/2+1, max2/2+2, 
		max1-1, max1, max2-1, max2, max2+1, max2+2 
	};


	bool all_ok=1;

	{
		const bool ok = RunTests<signed long long int>(i_values, v_values, true);
		all_ok = all_ok && ok;
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

