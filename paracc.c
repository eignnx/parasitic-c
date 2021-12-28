#include "lex.h" // lex_all_input

// Parasitic C

int main()
{
    char *input =
        "#include <stdio.h>\n"
        "#include \"my_file.h\"\n"
        "\n"
        "struct my_type {\n"
        "    int x;\n"
        "};\n"
        "\n"
        "void my_func123()\n"
        "{ // this is a comment! \n"
        "    bool my_bool = true;\n"
        "    int my_int = -1234;\n"
        "    char my_char1 = '\\n';\n"
        "    char my_char2 = 'A';\n"
        "    char my_char3 = '\\'';\n"
        "    char *my_str = \"qwea sdf sgd \\\" \\n rty\";\n"
        "}\n";

    lex_all_input(input);
}