/**
 * @file buildin.h
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief Header file for IFJ22 buildin functions
 */

#ifndef __BUILDIN_H__
#define __BUILDIN_H__

#include "gen.h"

/**
 * @brief function write (term1 ,term2 , …, termn) : void
 * Writes all terms to the standard output.
 * Similar to echo in php.
 *
 * @param gen Generator instance
 */
void gen_func_write(gen_t* gen);

/**
 * @brief function readi() : ?int
 * Reads an integer from the standard input.
 * Returns null if input is invalid.
 *
 * @param gen Generator instance
 */
void gen_func_readi(gen_t* gen);

/**
 * @brief function readf() : ?float
 * Reads a float from the standard input.
 * Returns null if input is invalid.
 *
 * @param gen Generator instance
 */
void gen_func_readf(gen_t* gen);

/**
 * @brief function reads() : ?string
 * Reads a string from the standard input.
 * Returns null if input is invalid.
 *
 * @param gen Generator instance
 */
void gen_func_reads(gen_t* gen);

/**
 * @brief function strlen(string $s) : int
 * Returns length of string.
 *
 * @param gen Generator instance
 */
void gen_func_strlen(gen_t* gen);

/**
 * @brief function chr(int $i) : string
 * Returns character with ASCII code $i.
 *
 * @param gen Generator instance
 */
void gen_func_chr(gen_t* gen);

/**
 * @brief function ord(string $c) : int
 * Returns ASCII code of first character of $c.
 * Returns 0 if $c is empty.
 *
 * @param gen Generator instance
 */
void gen_func_ord(gen_t* gen);

/**
 * @brief function substring(string $s, int $i, int $j) : ?string
 * Returns substring of $s starting at $i and ending at $j.
 *
 * Returns null when
 * $i < 0
 * $j < 0
 * $i > $j
 * $i >= strlen($s)
 * $j > strlen($s)
 *
 * @param gen Generator instance
 */
void gen_func_substring(gen_t* gen);

/**
 * @brief function floatval(term) : float
 * Returns float value of term.
 * Term can be int, float or null, otherwise generates error
 *
 * @param gen Generator instance
 */
void gen_func_floatval(gen_t* gen);

/**
 * @brief function intval(term) : int
 * Returns int value of term.
 * Term can be int, float or null, otherwise generates error
 *
 * @param gen Generator instance
 */
void gen_func_intval(gen_t* gen);

/**
 * @brief function strval(term) : string
 * Returns string value of term.
 * Term can be string or null, otherwise generates error
 *
 * @param gen Generator instance
 */
void gen_func_strval(gen_t* gen);

/**
 * @brief Prepare parameters for arithmetic functions (+, -, *)
 * If either of the parameters is null, they are replaced with 0
 * If both parameters are int, they are left as int
 * If one of parameters is float, both are converted to float
 * If either of parameters is string, it raises error
 *
 * @param gen Generator instance
 */
void gen_num_prepare(gen_t* gen);

/**
 * @brief Prepare parameters for division
 * If either of the parameters is null, they are replaced with 0
 * Converts both parameters to float
 * If either of parameters is string, it raises error
 *
 * @param gen Generator instance
 */
void gen_num_prepare_div(gen_t* gen);

/**
 * @brief Concat two strings
 * Parameters are on top of stack
 * Parameters must be strings or null otherwise generates error
 *
 * @param gen Generator instance
 */
void gen_concat(gen_t* gen);

/**
 * @brief Convert condition to bool value
 * Parameter is on top of stack
 *
 * @param gen Generator instance
 */
void gen_to_bool(gen_t* gen);

/**
 * @brief Evaluate === operator
 * Parameters are on the top of the stack
 *
 * @param gen Generator instance
 */
void gen_equals(gen_t* gen);

/**
 * @brief Evaluate > operator
 * Parameters are GF@_tmp1 and GF@_tmp2
 *
 * @param gen Generator instance
 */
void gen_greater(gen_t* gen);

/**
 * @brief Evaluate >= operator
 * Parameters are GF@_tmp1 and GF@_tmp2
 *
 * @param gen Generator instance
 */
void gen_greater_equals(gen_t* gen);

#endif
