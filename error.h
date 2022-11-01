/**
 * @file error.h
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief Header file for error handling
 */

#ifndef __ERROR_H__
#define __ERROR_H__

// Compiler return codes
enum return_code {
    RET_OK = 0,        // No error
    ERR_LEX = 1,       // Lexical error
    ERR_SYN = 2,       // Syntax error
    ERR_SEM_FUN = 3,   // Semantic error - function (undeclared, redefinition, ...)
    ERR_SEM_CALL = 4,  // Semantic error - function call (wrong number of parameters, ...)
    ERR_SEM_VAR = 5,   // Semantic error - undefined variable
    ERR_SEM_RET = 6,   // Semantic error - return statement
    ERR_SEM_COMP = 7,  // Semantic error - incompatible types
    ERR_SEM = 8,       // Other semantic error
    ERR_INTERNAL = 99  // Internal error (e.g. memory allocation error)
};

/**
 * @brief Used when we want to exit due to some error
 *
 * @param code Error code
 */
void error_exit(enum return_code code);

/**
 * @brief Used when something is not implemented yet
 */
void error_not_implemented();

#endif  // __ERROR_H__
