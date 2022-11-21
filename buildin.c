/**
 * @file buildin.h
 * @author Josef Kuchař (xkucha28@stud.fit.vutbr.cz)
 * @author Matej Sirovatka (xsirov00@stud.fit.vutbr.cz)
 * @author Tomáš Běhal (xbehal02@stud.fit.vutbr.cz)
 * @author Šimon Benčík (xbenci01@stud.fit.vutbr.cz)
 * @brief IFJ22 buildin functions
 */

#include "buildin.h"

void gen_func_write(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?write$declared\n"
                 "MOVE GF@?write$declared bool@true\n");

    str_add_cstr(&gen->functions,
                 "LABEL write\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@i\n"                          // Loop counter
                 "DEFVAR LF@current\n"                    // Current term
                 "POPS LF@i\n"                            // Get number of terms
                 "LABEL !write_loop\n"                    // Loop
                 "JUMPIFEQ !write_loop_end int@0 LF@i\n"  // Exit loop if i == 0
                 "SUB LF@i LF@i int@1\n"                  // i--
                 "POPS LF@current\n"                      // Get current term
                 "WRITE LF@current\n"                     // Output current term
                 "JUMP !write_loop\n"                     // Back to loop
                 "LABEL !write_loop_end\n"                // End of loop
                 "PUSHS nil@nil\n"                        // Return null
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_func_readi(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?readi$declared\n"
                 "MOVE GF@?readi$declared bool@true\n");

    str_add_cstr(&gen->functions,
                 "LABEL readi\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@tmp\n"
                 "READ LF@tmp int\n"  // Read int
                 "PUSHS LF@tmp\n"     // Return
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_func_readf(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?readf$declared\n"
                 "MOVE GF@?readf$declared bool@true\n");

    str_add_cstr(&gen->functions,
                 "LABEL readf\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@tmp\n"
                 "READ LF@tmp float\n"  // Read float
                 "PUSHS LF@tmp\n"       // Return
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_func_reads(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?reads$declared\n"
                 "MOVE GF@?reads$declared bool@true\n");

    str_add_cstr(&gen->functions,
                 "LABEL reads\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@tmp\n"
                 "READ LF@tmp string\n"  // Read string
                 "PUSHS LF@tmp\n"        // Return
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_func_strlen(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?strlen$declared\n"
                 "MOVE GF@?strlen$declared bool@true\n");

    str_add_cstr(&gen->functions,
                 "LABEL strlen\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@tmp\n"
                 "POPS LF@tmp\n"            // Get string
                 "TYPE GF@?type1 LF@tmp\n"  // Get type
                 "JUMPIFNEQ !ERR_SEM_CALL string@string GF@?type1\n"
                 "STRLEN LF@tmp LF@tmp\n"  // Get length
                 "PUSHS LF@tmp\n"          // Return
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_func_chr(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?chr$declared\n"
                 "MOVE GF@?chr$declared bool@true\n");

    // TODO: Check if $i is in range 0-255
    str_add_cstr(&gen->functions,
                 "LABEL chr\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@tmp\n"
                 "POPS LF@tmp\n"            // Get int
                 "TYPE GF@?type1 LF@tmp\n"  // Get type
                 "JUMPIFNEQ !ERR_SEM_CALL string@int GF@?type1\n"
                 "INT2CHAR LF@tmp LF@tmp\n"  // Convert to char
                 "PUSHS LF@tmp\n"            // Return
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_func_ord(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?ord$declared\n"
                 "MOVE GF@?ord$declared bool@true\n");

    // TODO: Return 0 if $c is empty
    str_add_cstr(&gen->functions,
                 "LABEL ord\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@tmp\n"
                 "POPS LF@tmp\n"                                      // Get string
                 "TYPE GF@?type1 LF@tmp\n"                            // Get type
                 "JUMPIFNEQ !ERR_SEM_CALL string@string GF@?type1\n"  // Check type
                 "JUMPIFEQ !ord_0 string@ LF@tmp\n"                   // Check if empty
                 "STRI2INT LF@tmp LF@tmp int@0\n"  // Get ASCII code of first char
                 "PUSHS LF@tmp\n"                  // Return
                 "POPFRAME\n"
                 "RETURN\n"
                 "LABEL !ord_0\n"
                 "PUSHS int@0\n"
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_func_floatval(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?floatval$declared\n"
                 "MOVE GF@?floatval$declared bool@true\n");

    str_add_cstr(&gen->functions,
                 "LABEL floatval\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@tmp\n"
                 "POPS LF@tmp\n"  // Get term
                 "TYPE GF@?type1 LF@tmp\n"
                 "JUMPIFEQ !floatval_null string@nil GF@?type1\n"   // null
                 "JUMPIFEQ !floatval_int string@int GF@?type1\n"    // int
                 "JUMPIFEQ !floatval_end string@float GF@?type1\n"  // float
                 "JUMP !ERR_SEM_COMP\n"
                 "LABEL !floatval_null\n"
                 "MOVE LF@tmp float@0x0p+0\n"  // null -> 0.0
                 "JUMP !floatval_end\n"
                 "LABEL !floatval_int\n"
                 "INT2FLOAT LF@tmp LF@tmp\n"  // int -> float
                 "LABEL !floatval_end\n"
                 "PUSHS LF@tmp\n"  // Return
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_func_intval(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?intval$declared\n"
                 "MOVE GF@?intval$declared bool@true\n");

    str_add_cstr(&gen->functions,
                 "LABEL intval\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@tmp\n"
                 "POPS LF@tmp\n"  // Get term
                 "TYPE GF@?type1 LF@tmp\n"
                 "JUMPIFEQ !intval_null string@nil GF@?type1\n"     // null
                 "JUMPIFEQ !intval_end string@int GF@?type1\n"      // int
                 "JUMPIFEQ !intval_float string@float GF@?type1\n"  // float
                 "JUMP !ERR_SEM_COMP\n"
                 "LABEL !intval_null\n"
                 "MOVE LF@tmp int@0\n"  // null -> 0
                 "JUMP !intval_end\n"
                 "LABEL !intval_float\n"
                 "FLOAT2INT LF@tmp LF@tmp\n"  // float -> int
                 "LABEL !intval_end\n"
                 "PUSHS LF@tmp\n"  // Return
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_func_strval(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?strval$declared\n"
                 "MOVE GF@?strval$declared bool@true\n");

    str_add_cstr(&gen->functions,
                 "LABEL strval\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@tmp\n"
                 "POPS LF@tmp\n"  // Get term
                 "TYPE GF@?type1 LF@tmp\n"
                 "JUMPIFEQ !strval_null string@nil GF@?type1\n"    // null
                 "JUMPIFEQ !strval_end string@string GF@?type1\n"  // string
                 "JUMP !ERR_SEM_COMP\n"
                 "LABEL !strval_null\n"
                 "MOVE LF@tmp string@\n"  // null -> ""
                 "LABEL !strval_end\n"
                 "PUSHS LF@tmp\n"  // Return
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_func_substring(gen_t* gen) {
    str_add_cstr(&gen->header,
                 "DEFVAR GF@?substring$declared\n"
                 "MOVE GF@?substring$declared bool@true\n");

    // TODO: Return null when conditions in function description are met
    str_add_cstr(&gen->functions,
                 "LABEL substring\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "DEFVAR LF@i\n"
                 "DEFVAR LF@j\n"
                 "DEFVAR LF@str\n"
                 "DEFVAR LF@tmp\n"
                 "DEFVAR LF@res\n"
                 "DEFVAR LF@len\n"
                 "POPS LF@str\n"                                      // Get string
                 "TYPE GF@?type1 LF@str\n"                            // Get type
                 "JUMPIFNEQ !ERR_SEM_CALL string@string GF@?type1\n"  // Check type
                 "STRLEN LF@len LF@str\n"                             // Get length
                 "POPS LF@i\n"                                        // Get start index
                 "TYPE GF@?type1 LF@i\n"                              // Get type
                 "JUMPIFNEQ !ERR_SEM_CALL string@int GF@?type1\n"     // Check type
                 "POPS LF@j\n"                                        // Get end index
                 "TYPE GF@?type1 LF@j\n"                              // Get type
                 "LT GF@?tmp1 LF@i int@0\n"                           // Check start index
                 "JUMPIFEQ !substring_null GF@?tmp1 bool@true\n"      //
                 "LT GF@?tmp1 LF@j int@0\n"                           // Check end index
                 "JUMPIFEQ !substring_null GF@?tmp1 bool@true\n"      //
                 "GT GF@?tmp1 LF@i LF@j\n"                            // Check start index
                 "JUMPIFEQ !substring_null GF@?tmp1 bool@true\n"      //
                 "GT GF@?tmp1 LF@i LF@len\n"                          // Check start index
                 "JUMPIFEQ !substring_null GF@?tmp1 bool@true\n"      //
                 "JUMPIFEQ !substring_null LF@i LF@len\n"             //
                 "GT GF@?tmp1 LF@j LF@len\n"                          // Check end index
                 "JUMPIFEQ !substring_null GF@?tmp1 bool@true\n"      //
                 "JUMPIFNEQ !ERR_SEM_CALL string@int GF@?type1\n"     // Check type
                 "MOVE LF@res string@\n"                              // res = ""
                 "LABEL !substring_loop\n"                            // Loop
                 "JUMPIFEQ !substring_loop_end LF@i LF@j\n"           // If i == j, end
                 "GETCHAR LF@tmp LF@str LF@i\n"                       // Get char at index i
                 "CONCAT LF@res LF@res LF@tmp\n"                      // res += char
                 "ADD LF@i LF@i int@1\n"                              // i++
                 "JUMP !substring_loop\n"                             // Jump to loop
                 "LABEL !substring_loop_end\n"                        // Loop end
                 "PUSHS LF@res\n"                                     // Return
                 "POPFRAME\n"
                 "RETURN\n"
                 "LABEL !substring_null\n"
                 "PUSHS nil@nil\n"
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_num_prepare(gen_t* gen) {
    str_add_cstr(&gen->functions,
                 "LABEL !num_prepare\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 // Get values from stack
                 "POPS GF@?tmp2\n"
                 "POPS GF@?tmp1\n"
                 // Check if values are int or float or null
                 "TYPE GF@?type1 GF@?tmp1\n"
                 "JUMPIFEQ !ERR_SEM_COMP string@string GF@?type1\n"
                 "JUMPIFEQ !ERR_SEM_COMP string@bool GF@?type1\n"
                 "TYPE GF@?type2 GF@?tmp2\n"
                 "JUMPIFEQ !ERR_SEM_COMP string@string GF@?type2\n"
                 "JUMPIFEQ !ERR_SEM_COMP string@bool GF@?type2\n"
                 // Check if first value is null
                 "JUMPIFNEQ !num_prepare_nil GF@?type1 string@nil\n"
                 // If yes convert to int 0
                 "MOVE GF@?tmp1 int@0\n"
                 "MOVE GF@?type1 string@int\n"
                 "LABEL !num_prepare_nil\n"
                 // Check if second value is null
                 "JUMPIFNEQ !num_prepare_start GF@?type2 string@nil\n"
                 // If yes convert to int 0
                 "MOVE GF@?tmp2 int@0\n"
                 "MOVE GF@?type2 string@int\n"
                 // Start converting
                 "LABEL !num_prepare_start\n"
                 // First operand is float
                 "JUMPIFEQ !num_prepare_first string@float GF@?type1\n"
                 // Second operand is float
                 "JUMPIFEQ !num_prepare_second string@float GF@?type2\n"
                 // Both operands are int, that means we are done
                 "JUMP !num_prepare_end\n"
                 // First operant is float, check second operand
                 "LABEL !num_prepare_first\n"
                 // Both operands are float, that means we are done
                 "JUMPIFEQ !num_prepare_end string@float GF@?type2\n"
                 // First operand is float, second is int, convert second to float
                 "INT2FLOAT GF@?tmp2 GF@?tmp2\n"
                 "JUMP !num_prepare_end\n"
                 // Second operand is float, check first operand
                 "LABEL !num_prepare_second\n"
                 // Both operands are float, that means we are done
                 "JUMPIFEQ !num_prepare_end string@float GF@?type1\n"
                 // Second operand is float, first is int, convert first to float
                 "INT2FLOAT GF@?tmp1 GF@?tmp1\n"
                 "LABEL !num_prepare_end\n"
                 // Push result to stack
                 "PUSHS GF@?tmp1\n"
                 "PUSHS GF@?tmp2\n"
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_num_prepare_div(gen_t* gen) {
    str_add_cstr(&gen->functions,
                 "LABEL !num_prepare_div\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 // Get values from stack
                 "POPS GF@?tmp2\n"
                 "POPS GF@?tmp1\n"
                 // Check if values are int or float or null
                 "TYPE GF@?type1 GF@?tmp1\n"
                 "JUMPIFEQ !ERR_SEM_COMP string@string GF@?type1\n"
                 "JUMPIFEQ !ERR_SEM_COMP string@bool GF@?type1\n"
                 "TYPE GF@?type2 GF@?tmp2\n"
                 "JUMPIFEQ !ERR_SEM_COMP string@string GF@?type2\n"
                 "JUMPIFEQ !ERR_SEM_COMP string@bool GF@?type2\n"
                 // Check if first value is null
                 "JUMPIFNEQ !num_prepare_div_nil GF@?type1 string@nil\n"
                 // If yes convert to int 0
                 "MOVE GF@?tmp1 int@0\n"
                 "MOVE GF@?type1 string@int\n"
                 "LABEL !num_prepare_div_nil\n"
                 // Check if second value is null
                 "JUMPIFNEQ !num_prepare_div_start GF@?type2 string@nil\n"
                 // If yes convert to int 0
                 "MOVE GF@?tmp2 int@0\n"
                 "MOVE GF@?type2 string@int\n"
                 // Start converting
                 "LABEL !num_prepare_div_start\n"
                 // First operand is float
                 "JUMPIFEQ !num_prepare_div_first string@float GF@?type1\n"
                 // Convert first operand to float
                 "INT2FLOAT GF@?tmp1 GF@?tmp1\n"
                 "LABEL !num_prepare_div_first\n"
                 // Second operand is float
                 "JUMPIFEQ !num_prepare_div_second string@float GF@?type2\n"
                 // Convert second operand to float
                 "INT2FLOAT GF@?tmp2 GF@?tmp2\n"
                 "LABEL !num_prepare_div_second\n"
                 // Push result to stack
                 "PUSHS GF@?tmp1\n"
                 "PUSHS GF@?tmp2\n"
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_comp_prepare(gen_t* gen) {
    str_add_cstr(&gen->functions,
                 "LABEL !comp_prepare\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "TYPE GF@?type1 GF@?tmp1\n"
                 "TYPE GF@?type2 GF@?tmp2\n"
                 "MOVE GF@?tmp3 GF@?tmp1\n"
                 "MOVE GF@?tmp4 GF@?tmp2\n"
                 "JUMPIFNEQ !comp_prepare_int1 string@int GF@?type1\n"
                 "INT2FLOAT GF@?tmp3 GF@?tmp1\n"
                 "MOVE GF@?type1 string@float\n"
                 "LABEL !comp_prepare_int1\n"
                 "JUMPIFNEQ !comp_prepare_int2 string@int GF@?type2\n"
                 "INT2FLOAT GF@?tmp4 GF@?tmp2\n"
                 "MOVE GF@?type2 string@float\n"
                 "LABEL !comp_prepare_int2\n"
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_concat(gen_t* gen) {
    str_add_cstr(&gen->functions,
                 "LABEL !concat\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 // Get values from stack
                 "POPS GF@?tmp2\n"
                 "POPS GF@?tmp1\n"
                 // Check if values are string or nil
                 "TYPE GF@?type1 GF@?tmp1\n"
                 "JUMPIFEQ !ERR_SEM_COMP string@int GF@?type1\n"
                 "JUMPIFEQ !ERR_SEM_COMP string@float GF@?type1\n"
                 "JUMPIFEQ !ERR_SEM_COMP string@bool GF@?type1\n"
                 "TYPE GF@?type2 GF@?tmp2\n"
                 "JUMPIFEQ !ERR_SEM_COMP string@int GF@?type2\n"
                 "JUMPIFEQ !ERR_SEM_COMP string@float GF@?type2\n"
                 "JUMPIFEQ !ERR_SEM_COMP string@bool GF@?type2\n"
                 // Check if first value is nil
                 "JUMPIFNEQ !concat_nil GF@?type1 string@nil\n"
                 // If yes convert to string ""
                 "MOVE GF@?tmp1 string@\n"
                 "LABEL !concat_nil\n"
                 // Check if second value is nil
                 "JUMPIFNEQ !concat_end GF@?type2 string@nil\n"
                 "MOVE GF@?tmp2 string@\n"
                 "LABEL !concat_end\n"
                 // Concatenate strings
                 "CONCAT GF@?tmp3 GF@?tmp1 GF@?tmp2\n"
                 "PUSHS GF@?tmp3\n"
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_to_bool(gen_t* gen) {
    str_add_cstr(&gen->functions,
                 "LABEL !to_bool\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "TYPE GF@?type1 GF@?tmp1\n"
                 "JUMPIFEQ !to_bool_string string@string GF@?type1\n"
                 "JUMPIFEQ !to_bool_int string@int GF@?type1\n"
                 "JUMPIFEQ !to_bool_float string@float GF@?type1\n"
                 "JUMPIFEQ !to_bool_false string@nil GF@?type1\n"
                 "POPFRAME\n"
                 "RETURN\n"
                 "LABEL !to_bool_string\n"
                 "JUMPIFEQ !to_bool_false string@ GF@?tmp1\n"
                 "JUMPIFEQ !to_bool_false string@0 GF@?tmp1\n"
                 "JUMP !to_bool_true\n"
                 "LABEL !to_bool_int\n"
                 "JUMPIFEQ !to_bool_false int@0 GF@?tmp1\n"
                 "JUMP !to_bool_true\n"
                 "LABEL !to_bool_float\n"
                 "JUMPIFEQ !to_bool_false float@0x0p+0 GF@?tmp1\n"
                 "JUMP !to_bool_true\n"
                 "LABEL !to_bool_false\n"
                 "MOVE GF@?tmp1 bool@false\n"
                 "POPFRAME\n"
                 "RETURN\n"
                 "LABEL !to_bool_true\n"
                 "MOVE GF@?tmp1 bool@true\n"
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_equals(gen_t* gen) {
    str_add_cstr(&gen->functions,
                 "LABEL !equals\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 // Get values from stack
                 "POPS GF@?tmp2\n"
                 "POPS GF@?tmp1\n"
                 // Check if types are same
                 "TYPE GF@?type1 GF@?tmp1\n"
                 "TYPE GF@?type2 GF@?tmp2\n"
                 "JUMPIFNEQ !equals_false GF@?type1 GF@?type2\n"
                 // If yes check if values are same
                 "EQ GF@?tmp3 GF@?tmp1 GF@?tmp2\n"
                 "PUSHS GF@?tmp3\n"
                 "JUMP !equals_end\n"
                 "LABEL !equals_false\n"
                 "PUSHS bool@false\n"
                 "LABEL !equals_end\n"
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_greater(gen_t* gen) {
    str_add_cstr(&gen->functions,
                 "LABEL !greater\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "CALL !comp_prepare\n"
                 "JUMPIFEQ !greater_false string@nil GF@?type1\n"
                 "JUMPIFEQ !greater_same GF@?type1 GF@?type2\n"
                 "TYPE GF@?type1 GF@?tmp1\n"
                 "TYPE GF@?type2 GF@?tmp2\n"
                 "JUMP !greater_diff\n"
                 // If yes check if values are same
                 "LABEL !greater_same\n"
                 "GT GF@?tmp3 GF@?tmp3 GF@?tmp4\n"
                 "PUSHS GF@?tmp3\n"
                 "JUMP !greater_end\n"
                 "LABEL !greater_diff\n"
                 "JUMPIFEQ !greater_string1 string@string GF@?type1\n"
                 "JUMPIFEQ !greater_string2 string@string GF@?type2\n"
                 "JUMPIFEQ !greater_null string@nil GF@?type2\n"
                 "JUMP !greater_true\n"  // Default is true
                 // First param is string
                 "LABEL !greater_string1\n"
                 "JUMPIFEQ !greater_false string@ GF@?tmp1\n"
                 "JUMP !greater_true\n"
                 // Second param is string
                 "LABEL !greater_string2\n"
                 "JUMPIFEQ !greater_true string@ GF@?tmp2\n"
                 "JUMP !greater_false\n"
                 // Second param is null
                 "LABEL !greater_null\n"
                 "JUMPIFEQ !greater_int string@int GF@?type1\n"
                 "JUMPIFEQ !greater_float string@float GF@?type1\n"
                 "JUMP !greater_true\n"
                 // First param is int
                 "LABEL !greater_int\n"
                 "JUMPIFEQ !greater_false int@0 GF@?tmp1\n"
                 "JUMP !greater_true\n"
                 // First param is float
                 "LABEL !greater_float\n"
                 "JUMPIFEQ !greater_false float@0x0p+0 GF@?tmp1\n"
                 "JUMP !greater_true\n"
                 // Return true
                 "LABEL !greater_true\n"
                 "PUSHS bool@true\n"
                 "JUMP !greater_end\n"
                 // Return false
                 "LABEL !greater_false\n"
                 "PUSHS bool@false\n"
                 "LABEL !greater_end\n"
                 "POPFRAME\n"
                 "RETURN\n");
}

void gen_greater_equals(gen_t* gen) {
    str_add_cstr(&gen->functions,
                 "LABEL !greater_equals\n"
                 "CREATEFRAME\n"
                 "PUSHFRAME\n"
                 "CALL !comp_prepare\n"
                 "JUMPIFEQ !greater_equals_true string@nil GF@?type2\n"
                 "JUMPIFEQ !greater_equals_same GF@?type1 GF@?type2\n"
                 "TYPE GF@?type1 GF@?tmp1\n"
                 "TYPE GF@?type2 GF@?tmp2\n"
                 "JUMP !greater_equals_diff\n"
                 // If yes check if greater or equal
                 "LABEL !greater_equals_same\n"
                 "GT GF@?tmp1 GF@?tmp3 GF@?tmp4\n"
                 "EQ GF@?tmp2 GF@?tmp3 GF@?tmp4\n"
                 "OR GF@?tmp3 GF@?tmp1 GF@?tmp2\n"
                 "PUSHS GF@?tmp3\n"
                 "JUMP !greater_equals_end\n"
                 "LABEL !greater_equals_diff\n"
                 "JUMPIFEQ !greater_equals_string1 string@string GF@?type1\n"
                 "JUMPIFEQ !greater_equals_string2 string@string GF@?type2\n"
                 "JUMPIFEQ !greater_equals_null string@nil GF@?type1\n"
                 "JUMP !greater_equals_false\n"  // Default is false
                 // First param is string
                 "LABEL !greater_equals_string1\n"
                 "JUMPIFEQ !greater_equals_false string@ GF@?tmp1\n"
                 "JUMP !greater_equals_true\n"
                 // Second param is string
                 "LABEL !greater_equals_string2\n"
                 "JUMPIFEQ !greater_equals_true string@ GF@?tmp2\n"
                 "JUMP !greater_equals_false\n"
                 // First param is null
                 "LABEL !greater_equals_null\n"
                 "JUMPIFEQ !greater_equals_int string@int GF@?type2\n"
                 "JUMPIFEQ !greater_equals_float string@float GF@?type2\n"
                 "JUMP !greater_equals_false\n"
                 // Second param is int
                 "LABEL !greater_equals_int\n"
                 "JUMPIFEQ !greater_equals_true int@0 GF@?tmp2\n"
                 "JUMP !greater_equals_false\n"
                 // Second param is float
                 "LABEL !greater_equals_float\n"
                 "JUMPIFEQ !greater_equals_true float@0x0p+0 GF@?tmp2\n"
                 "JUMP !greater_equals_false\n"
                 // Return true
                 "LABEL !greater_equals_true\n"
                 "PUSHS bool@true\n"
                 "JUMP !greater_equals_end\n"
                 // Return false
                 "LABEL !greater_equals_false\n"
                 "PUSHS bool@false\n"
                 "LABEL !greater_equals_end\n"
                 "POPFRAME\n"
                 "RETURN\n");
}
