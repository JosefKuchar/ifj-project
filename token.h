#ifndef __TOKEN_H__
#define __TOKEN_H__

// All tokens
enum Token {
  TOK_ID,         // Identifier (eg. $foo)
  TOK_STR_LIT,    // String literal (eg. "foo")
  TOK_INT_LIT,    // Integer literal (eg. 0)
  TOK_FLOAT_LIT,  // Float literal (eg. 0.0)
  TOK_BOOL_LIT,   // Bool literal (eg. true)
  TOK_FUN_NAME,   // Function name (eg. write)
  TOK_DOT,        // .
  TOK_COMMA,      // ,
  TOK_COLON,      // :
  TOK_SEMICOLON,  // ,
  TOK_ASSIGN,     // =
  TOK_EQUALS,     // ===
  TOK_NEQUALS,    // !==
  TOK_PLUS,       // +
  TOK_MINUS,      // -
  TOK_MULTIPLY,   // *
  TOK_DIVIDE,     // /
  TOK_LESS,       // <
  TOK_GREATER,    // >
  TOK_LESS_E,     // <=
  TOK_GREATER_E,  // >=
  TOK_LPAREN,     // (
  TOK_RPAREN,     // )
  TOK_LBRACE,     // {
  TOK_RBRACE,     // {

  /* KEYWORDS */
  TOK_ELSE,      // else
  TOK_FLOAT,     // float
  TOK_FUNCTION,  // function
  TOK_IF,        // if
  TOK_INT,       // int
  TOK_NULL,      // null
  TOK_RETURN,    // return
  TOK_STRING,    // string
  TOK_VOID,      // void
  TOK_WHILE,     // while
};

#endif  // __TOKEN_H__
