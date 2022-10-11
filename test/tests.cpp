extern "C" {
#include "../scanner.h"
}

#include "gtest/gtest.h"

TEST(TestSuiteName, TestName) {
    FILE* input = tmpfile();
    fprintf(input, "int main() { return 0; }");
    scanner_t scanner = scanner_new(input);
    EXPECT_DEATH(scanner_get_next(&scanner), "Lexical error");
    fclose(input);
}
