//
// compiler.h
//

#ifndef COMPILER_H
#define COMPILER_H

#include "closure.h"

Closure *compile(char *source, bool emit, Result *result);

#endif // COMPILER_H
