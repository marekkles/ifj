/**
 * IFJ- Ruby
 * RETURN Values
 */


#ifndef _RET_VAL_H
#define _RET_VAL_H

// Return Values

#define EVERYTHING_OK 					         0 // No error.

#define LEXICAL_ERROR		                 1 // Lexical structure error.
#define SYNTAX_ERROR					           2 // Syntax error.
#define SEMANTIC_ERROR_UNDEFINED_VAR		 3 // Semantic error - undefined variable.
#define SEMANTIC_ERROR_TYPE_COMPATIBILY	 4 // Sem. error - types uncompatible.
#define SEMANTIC_ERROR_PARAMETERS_COUNT  5 // Sem. error - wrong count of parameters of a function.
#define SEMANTIC_ERROR_OTHER				     6 // Sem. error - other error.
#define RUN_ERROR_DIVISION_ZERO          9 // Error in the run division by zero
#define INTERNAL_ERROR           				99 // Internal error of compiler - e.g. allocation of memory etc.

#endif //_RET_VAL_H


// testing github
