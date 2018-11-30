#include <stdio.h>
#include <stdlib.h>
#include "return.h"
#include "debug.h"
#include "dstr.h"
#include "fsm.h"
#include "symtable.h"
#include "codegen.h"

#ifndef PARSER_H

#define PARSER_H

extern FILE *input;
extern FILE *output;
extern int in_function;

int Parse(void);

#endif