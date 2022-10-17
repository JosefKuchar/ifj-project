extern "C" {
#include "../scanner.h"
}

#include "gtest/gtest.h"

TEST(LexicalTest, Numbers) {
    auto input = tmpfile();
    fprintf(input, "<?php\n123 0 3e3");
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

    fclose(input);
}
