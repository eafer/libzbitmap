#include <stddef.h>

#define ZBM_ERRNO   (-1)    /* System error, check errno for details */
#define ZBM_INVAL   (-2)    /* Compressed input is invalid */
#define ZBM_RANGE   (-3)    /* The destination buffer is too small */

/**
 * zbm_decompress - Decompress an LZBITMAP buffer
 * @dest:       destination buffer (may be NULL)
 * @dest_size:  size of the destination buffer
 * @src:        source buffer
 * @src_size:   size of the source buffer
 * @out_len:    on return, the length of the decompressed output
 *
 * May be called with a NULL destination buffer to retrieve the expected length
 * of the decompressed data. Returns 0 on success, or a negative error code in
 * case of failure.
 */
int zbm_decompress(void *dest, size_t dest_size, const void *src, size_t src_size, size_t *out_len);
