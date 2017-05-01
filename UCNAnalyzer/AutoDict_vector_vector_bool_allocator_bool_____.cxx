#include "vector"
#ifdef __CINT__ 
#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;
#pragma link C++ class vector<vector<bool,allocator<bool> > >+;
#pragma link C++ class vector<vector<bool,allocator<bool> > >::*;
#ifdef G__VECTOR_HAS_CLASS_ITERATOR
#pragma link C++ operators vector<vector<bool,allocator<bool> > >::iterator;
#pragma link C++ operators vector<vector<bool,allocator<bool> > >::const_iterator;
#pragma link C++ operators vector<vector<bool,allocator<bool> > >::reverse_iterator;
#endif
#endif
