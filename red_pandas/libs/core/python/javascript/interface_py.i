%module red_pandas

%{


#include "serie.h"
#include "serie2d.h"
#include "calculation.h"
#include "div.h"
#include "mul.h"
#include "sub.h"
#include "sum.h"
#include "exp.h"
#include "log.h"
#include "pow.h"
#include "sqrt.h"
#include "erf.h"
#include "neg.h"
#include "abs.h"
#include "operators/less.h"
#include "operators/isfinite.h"
#include "pdf.h"
#include "cdf.h"
#include "ppf.h"
#include "max0.h"
#include "rsi.h"
#include "clean.h"
#include "rand_normal.h"
#include "formulas/greeks.h"
// finance library
#include "npv.h"
// portfolio library
#include "portfolio.h"
// simulator library
#include "simulator.h"
%}


// %nodefaultctor Calculation;
// %nodefaultctor Serie;

// %nodefaultdtor Calculation;
// %nodefaultdtor Serie;

// https://www.swig.org/Doc4.0/SWIGDocumentation.html#Library_stl_cpp_library

// typedefs
//namespace nc {
//    typedef NdArray<double> NumpyArray;
//}



// string
%include <std_string.i>
// %include <std_wstring.i>
%apply const std::string& {std::string* foo};


// map
// %include <std_unordered_map.i>
/*
namespace std {
    %template(map_string_serie) unordered_map<string, shared_ptr<Serie> >;
    %template(map_serie_serie) unordered_map<shared_ptr<Serie>, shared_ptr<Serie> >;
    //
    %template(map_string_serie_ref) unordered_map<string, Serie >;
}
*/

// set
// %include <std_unordered_set.i>
// namespace std {
//     %template(set_serie) unordered_set<shared_ptr<Serie> >;
// }


// vector
%include <std_vector.i>
namespace std {
    %template(vectors) vector<string>;
    %template(vectorn) vector<NoneType>;
    %template(vectord) vector<double>;
    %template(vectorl) vector<long>;
    %template(vectorser) vector<shared_ptr<Serie> >;
    %template(vectorm) vector<unordered_map<shared_ptr<Serie>, shared_ptr<Serie> > >;
    //
    %template(vector_results) vector<unordered_map<string, Serie > >;
};


// exceptions
%include "exception.i"

/*
SWIG_MemoryError
SWIG_IOError
SWIG_RuntimeError
SWIG_IndexError
SWIG_TypeError
SWIG_DivisionByZero
SWIG_OverflowError
SWIG_SyntaxError
SWIG_ValueError
SWIG_SystemError
*/

// numpy
%include "numpy.i"
 
%init %{
import_array();
%}

// %numpy_typemaps(double, NPY_DOUBLE, int)
// %apply  ( double* IN_ARRAY1, int DIM1 ) {(double *xx, int xx_n)};
// %apply  ( double* IN_ARRAY2, long DIM1, long DIM2 ) {(double *yy, int yy_n, int yy_m)};



//%ignore Serie::Serie();
//%ignore Serie::Serie(const Serie& other);
//%ignore Serie::Serie(Serie&& other);
//%ignore Serie::operator=(const Serie& other);
//%ignore Serie::operator=(Serie&& other);
//
//%ignore Calculation::Calculation();
//%ignore Calculation::Calculation(const Calculation& other);
//%ignore Calculation::Calculation(Calculation&& other);
//%ignore Calculation::operator=(const Calculation& other);
//%ignore Calculation::operator=(Calculation&& other);



%include "serie.h"
%include "serie2d.h"
%include "calculation.h"
%include "operators/less.h"
%include "operators/isfinite.h"
%include "div.h"
%include "mul.h"
%include "sub.h"
%include "sum.h"
%include "exp.h"
%include "log.h"
%include "pow.h"
%include "sqrt.h"
%include "erf.h"
%include "neg.h"
%include "abs.h"
%include "pdf.h"
%include "cdf.h"
%include "ppf.h"
%include "max0.h"
%include "rsi.h"
%include "clean.h"
%include "rand_normal.h"
%include "formulas/greeks.h"
// finance library
%include "npv.h"
// portfolio library
%include "portfolio.h"
// simulator library
%include "simulator.h"

