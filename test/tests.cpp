extern "C" {
#include "../scanner.h"
}

#include "gtest/gtest.h"

TEST(LexicalTest, Keywords) {
    auto input = tmpfile();
    fprintf(input,"<?php\nelse float ?float function if int ?int null return string ?string void while");
    rewind(input);
    auto scanner = scanner_new(input);

    auto token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_ELSE);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_FLOAT);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_FLOAT);    

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_FUNCTION);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_IF);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_INT);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_INT);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_NULL);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_RETURN);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_STRING);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_STRING);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_VOID);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_WHILE);

    fclose(input);
}

TEST(LexicalTest, Numbers) {
    auto input = tmpfile();
    fprintf(input, "<?php\n123 0 3e3 10e-2 2.25");
    rewind(input);
    auto scanner = scanner_new(input);

    auto token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_INT_LIT);
    EXPECT_EQ(token.attr.val_i, 123);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_INT_LIT);
    EXPECT_EQ(token.attr.val_i, 0);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_FLOAT_LIT);
    EXPECT_EQ(token.attr.val_f, 3000.0);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_FLOAT_LIT);
    EXPECT_EQ(token.attr.val_f, 0.1);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_FLOAT_LIT);
    EXPECT_EQ(token.attr.val_f, 2.25);

    fclose(input);
}

TEST(LexicalTest, Operators){
    auto input = tmpfile();
    fprintf(input, "<?php\n+ - * / < <= > >= === !==");
    rewind(input);
    auto scanner = scanner_new(input);

    auto token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_PLUS);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_MINUS);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_MULTIPLY);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_DIVIDE);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_LESS);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_LESS_E);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_GREATER);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_GREATER_E);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_EQUALS);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_NEQUALS);

    fclose(input);
}

TEST(LexicalTest, Misc){
    auto input = tmpfile();
    fprintf(input, "<?php\n( ) { } , . : ; =");
    rewind(input);
    auto scanner = scanner_new(input);

    auto token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_LPAREN);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_RPAREN);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_LBRACE);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_RBRACE);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_COMMA);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_DOT);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_COLON);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_SEMICOLON);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_ASSIGN);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_EOF);

    fclose(input);
}

TEST(LexicalTest, VariableAndFunction) {
    auto input = tmpfile();
    fprintf(input, "<?php\n$a = readi();");
    rewind(input);

    auto scanner = scanner_new(input);

    auto token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_VAR);
    EXPECT_STREQ(token.attr.val_s.val, "$a");

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_ASSIGN);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_FUN_NAME);
    EXPECT_STREQ(token.attr.val_s.val, "readi");

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_LPAREN);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_RPAREN);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_SEMICOLON);

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_EOF);

    fclose(input);
}

TEST(LexicalTest, StringAndComment) {
    auto input = tmpfile();
    fprintf(input, "<?php\n\"Hello World\" // Comment");
    rewind(input);

    auto scanner = scanner_new(input);

    auto token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_STR_LIT);
    EXPECT_STREQ(token.attr.val_s.val, "Hello World");

    token = scanner_get_next(&scanner);
    EXPECT_EQ(token.type, TOK_EOF);

    fclose(input);
}

// TEST(LexicalTest, Bonus){
//     auto input = tmpfile();
//     fprintf(input, "<?php\ntrue false");
//     rewind(input);

//     auto scanner = scanner_new(input);

//     auto token = scanner_get_next(&scanner);
//     EXPECT_EQ(token.type, TOK_TRUE);

//     token = scanner_get_next(&scanner);
//     EXPECT_EQ(token.type, TOK_FALSE);
//     fclose(input);
// }
