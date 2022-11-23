#include <errno.h>
#include <pthread.h>
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

static void run_decompress_test(int i)
{
    char *original = NULL;
    char *compressed = NULL;
    char *decompressed = NULL;
    size_t orig_len, compr_len, decmp_len, exp_decmp_len;
    int err;

    original = read_file(i, ORIGINAL, &orig_len);
    compressed = read_file(i, COMPRESSED, &compr_len);

    printf("%.3u(decm)... ", i);
    fflush(stdout);

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
            printf("SUCCESS - ratio: %.2f\n", (double)orig_len / compr_len);
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

static void save_compression(const char *buf, size_t size, int i)
{
    char name[24];
    FILE *file = NULL;

    if(i > 999)
        FATAL(i);
    snprintf(name, 24, "files/%.3u-%s", i, "recompressed");
    file = fopen(name, "w");
    if(!file)
        FATAL(i);

    if(fwrite(buf, 1, size, file) != size)
        FATAL(i);
    if(fclose(file))
        FATAL(i);
    file = NULL;
}

struct arguments {
    char        dest[ZBM_MAX_CHUNK_SIZE];
    const void  *src;
    size_t      src_size;
    off_t       index;
    size_t      out_len;
    int         err;
};

static void *compress_chunk(void *arguments)
{
    struct arguments *args = arguments;

    args->err = zbm_compress_chunk(args->dest, ZBM_MAX_CHUNK_SIZE, args->src, args->src_size, args->index, &args->out_len);
    return NULL;
}

#define NUM_THREADS 32

static int compress(void *dest, size_t dest_size, const void *src, size_t src_size, size_t *out_len, int testnum)
{
    static struct arguments args[NUM_THREADS] = {0};
    pthread_t threads[NUM_THREADS];
    off_t i, j, k;
    int last_error = 0;

    *out_len = 0;

    for(i = 0; i < NUM_THREADS; ++i) {
        args[i].src = src;
        args[i].src_size = src_size;
        args[i].index = i;
        if(pthread_create(&threads[i], NULL, compress_chunk, &args[i]))
            FATAL(testnum);
    }

    j = 0;
    for(j = 1; ; ++j) {
        for(i = 0; i < NUM_THREADS; ++i) {
            if(pthread_join(threads[i], NULL))
                FATAL(testnum);
            if(args[i].err)
                last_error = args[i].err;
            if(args[i].out_len > dest_size)
                last_error = ZBM_RANGE;
            if(last_error)
                goto done;

            memcpy(dest, args[i].dest, args[i].out_len);
            dest += args[i].out_len;
            dest_size -= args[i].out_len;
            *out_len += args[i].out_len;
            if(args[i].out_len == ZBM_LAST_CHUNK_SIZE)
                goto done;

            args[i].index = NUM_THREADS * j + i;
            if(args[i].index < j)
                FATAL(testnum);
            if(pthread_create(&threads[i], NULL, compress_chunk, &args[i]))
                FATAL(testnum);
        }
    }

done:
    for(k = 0; k < NUM_THREADS; ++k) {
        if(k == i) /* This one thread has been joined already */
            continue;
        if(pthread_join(threads[k], NULL))
            FATAL(testnum);
    }
    return last_error;
}

static void run_compress_test(int i)
{
    char *original = NULL;
    char *compressed = NULL;
    char *decompressed = NULL;
    size_t orig_len, compr_len, max_compr_len, decmp_len;
    int err;

    original = read_file(i, ORIGINAL, &orig_len);

    printf("%.3u(comp)... ", i);
    fflush(stdout);

    err = zbm_compress(NULL, 0, original, orig_len, &max_compr_len);
    if(err) {
        printf("FAILURE - error code: %d\n", err);
        ++tests_failed;
        goto out;
    }
    compressed = malloc(max_compr_len);
    if(!compressed)
        FATAL(i);

    err = compress(compressed, max_compr_len, original, orig_len, &compr_len, i);
    if(err) {
        printf("FAILURE - compression error code: %d\n", err);
        ++tests_failed;
    } else {
        save_compression(compressed, compr_len, i);
        decompressed = malloc(orig_len);
        if(!decompressed)
            FATAL(i);
        err = zbm_decompress(decompressed, orig_len, compressed, compr_len, &decmp_len);
        if(err) {
            printf("FAILURE - decompression error code: %d\n", err);
            ++tests_failed;
        } else if(decmp_len == orig_len && memcmp(decompressed, original, orig_len) == 0) {
            printf("SUCCESS - ratio: %.2f\n", (double)orig_len / compr_len);
        } else {
            printf("FAILURE - decompression doesn't match original\n");
            ++tests_failed;
        }
    }
out:
    ++tests_run;

    free(compressed);
    free(original);
    free(decompressed);
}

static void run_test(int i)
{
    run_decompress_test(i);
    run_compress_test(i);
}

int main(int argc, char *argv[])
{
    int i;

    if(argc == 2) {
        run_test(atoi(argv[1]));
    } else {
        for(i = 1;; ++i)
            run_test(i);
    }
}
