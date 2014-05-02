

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <stdarg.h>
#include <libtcc.h>

int add(int a, int b)
{
    return a + b;
}

char my_program[] =
"int fib(int n)\n"
"{\n"
" if (n <= 2)\n"
" return 1;\n"
" else\n"
" return fib(n-1) + fib(n-2);\n"
"}\n"
"\n"
"float foo(int n)\n"
"{float a = 0.0; for(int i=0; i<1000000; i++) { a = i + (float)i/2 + (float)i/3 + (float)i/4 + (float)i/5;}; printf(\"qqqq %f\n\", a); return a;"
"}\n";

int main(int argc, char **argv)
{
    TCCState *s;
    int (*func)(int);

    s = tcc_new();
    if (!s) {
        fprintf(stderr, "Could not create tcc state\n");
        exit(1);
    }

    /* if tcclib.h and libtcc1.a are not installed, where can we find them */
    if (argc == 2 && !memcmp(argv[1], "lib_path=",9))
        tcc_set_lib_path(s, argv[1]+9);

    /* MUST BE CALLED before any compilation */
    tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

    if (tcc_compile_string(s, my_program) == -1)
        return 1;

    /* as a test, we add a symbol that the compiled program can use.
	You may also open a dll with tcc_add_dll() and use symbols from that */
    tcc_add_symbol(s, "add", add);

    /* relocate the code */
    if (tcc_relocate(s, TCC_RELOCATE_AUTO) < 0)
        return 1;

    /* get entry symbol */
    func = tcc_get_symbol(s, "foo");
    if (!func)
        return 1;

    /* run the code */
    func(32);

    /* delete the state */
    tcc_delete(s);

    return 0;
}

