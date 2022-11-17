#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libzbitmap.h"

static void fatal_with_loc(const char *fn, int line, int test)
{
    fprintf(stderr, "Fatal error in test %.3u (function %s(), line %d)\n", test, fn, line);
    exit(1);
}
#define FATAL(test)  fatal_with_loc(__func__, __LINE__, test)

#define ORIGINAL    0
#define COMPRESSED  1

static int tests_run, tests_failed;

static void report_results(void)
{
    puts("");
    if(tests_failed == 0)
        printf("Ran %d tests with no failures\n", tests_run);
    else
        printf("Ran %d tests with %d failures\n", tests_run, tests_failed);
    puts("");

    exit(tests_failed != 0);
}

static char *read_file(int i, int which, size_t *len)
{
    char name[24];
    FILE *file = NULL;
    char *buf = NULL;
    size_t length;

    if(i > 999)
        FATAL(i);
    snprintf(name, 24, "files/%.3u-%s", i, which == ORIGINAL ? "original" : "compressed");
    file = fopen(name, "r");
    if(!file) {
        if(errno == ENOENT) /* No more tests */
            report_results();
        FATAL(i);
    }

    if(fseek(file, 0, SEEK_END))
        FATAL(i);
    length = ftell(file);
    if(fseek(file, 0, SEEK_SET))
        FATAL(i);

    buf = malloc(length);
    if(!buf)
        FATAL(i);

    if(fread(buf, 1, length, file) != length)
        FATAL(i);
    if(fclose(file))
        FATAL(i);
    file = NULL;

    *len = length;
    return buf;
}

static void run_test(int i)
{
    char *original = NULL;
    char *compressed = NULL;
    char *decompressed = NULL;
    size_t orig_len, compr_len, decmp_len, exp_decmp_len;
    int err;

    original = read_file(i, ORIGINAL, &orig_len);
    compressed = read_file(i, COMPRESSED, &compr_len);

    printf("%.3u... ", i);

    err = zbm_decompress(NULL, 0, compressed, compr_len, &exp_decmp_len);
    if(err) {
        printf("FAILURE - error code: %d\n", err);
        ++tests_failed;
    } else {
        decompressed = malloc(exp_decmp_len);
        if(!decompressed)
            FATAL(i);
        err = zbm_decompress(decompressed, exp_decmp_len, compressed, compr_len, &decmp_len);
        if(err) {
            printf("FAILURE - error code: %d\n", err);
            ++tests_failed;
        } else if(exp_decmp_len != decmp_len) {
            printf("FAILURE - inconsistent decompressed length\n");
            ++tests_failed;
        } else if(decmp_len == orig_len && memcmp(decompressed, original, orig_len) == 0) {
            printf("SUCCESS\n");
        } else {
            printf("FAILURE - output doesn't match\n");
            ++tests_failed;
        }
    }
    ++tests_run;

    free(compressed);
    free(original);
    free(decompressed);
}

int main(void)
{
    int i;

    for(i = 1;; ++i)
        run_test(i);
}
