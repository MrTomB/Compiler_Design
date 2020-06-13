// $Id: stringset.h,v 1.4 2014-10-09 15:42:13-07 - - $
//Thomas Burch
//tburch1@ucsc.edu
//CMPS104A-ASG1

#ifndef __STRINGSET__
#define __STRINGSET__

#include <string>
#include <unordered_set>
using namespace std;

#include <stdio.h>

#include "auxlib.h"

const string* intern_stringset (const char*);

void dump_stringset (FILE*);

#endif