/* File: traveller.i */
%module ga_traveller

%include "std_vector.i"

namespace std
{
  %template(IntVector) vector<int>;
}

%{
#define SWIG_FILE_WITH_INIT
#include "traveller.h"
%}

%include "traveller.h"