#include "tool_setup.h"
#ifndef HAVE_LIBZ
/*
 * NEVER EVER edit this manually, fix the mkhelp.pl script instead!
 */
#ifdef USE_MANUAL
#include "tool_hugehelp.h"
void hugehelp(void)
{
    fputs(
        "                                  _   _ ____  _\n"
        "  Project                     ___| | | |  _ \\| |\n"
        "                             / __| | | | |_) | |\n"
        "                            | (__| |_| |  _ <| |___\n"
        "                             \\___|\\___/|_| \\_\\_____|\n"
        "\n",
        stdout);
}
#else /* !USE_MANUAL */
/* built-in manual is disabled, blank function */
#include "tool_hugehelp.h"
void hugehelp(void) {}
#endif /* USE_MANUAL */
#else
/*
 * NEVER EVER edit this manually, fix the mkhelp.pl script instead!
 */
#ifdef USE_MANUAL
#include "tool_hugehelp.h"
#include <zlib.h>
#include "memdebug.h" /* keep this as LAST include */
static const unsigned char hugehelpgz[] = {
    /* This mumbo-jumbo is the huge help text compressed with gzip.
       Thanks to this operation, the size of this data shrank from 252
       to 97 bytes. You can disable the use of compressed help
       texts by NOT passing -c to the mkhelp.pl tool. */
    0x1f, 0x8b, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x03, 0x53, 0x50, 0x20, 0x04,
    0xe2, 0xc1, 0x38, 0x1e, 0x08, 0x80, 0x34, 0x97, 0x82, 0x42, 0x40, 0x51, 0x7e, 0x56,
    0x6a, 0x72, 0x09, 0x76, 0xb5, 0xf1, 0xf1, 0x35, 0x0a, 0x10, 0x08, 0xd4, 0x13, 0x03,
    0xa4, 0xb9, 0xf0, 0x9a, 0xad, 0xaf, 0x80, 0xd0, 0x50, 0x13, 0xaf, 0xa9, 0x40, 0x48,
    0x43, 0x8d, 0x82, 0x06, 0x58, 0x43, 0x3c, 0xd4, 0x06, 0x1b, 0x10, 0x3b, 0x3e, 0x1e,
    0xbf, 0x25, 0x31, 0x20, 0x57, 0x81, 0x08, 0x7d, 0x90, 0xbe, 0x98, 0x78, 0x10, 0x13,
    0x28, 0xc2, 0xc5, 0x05, 0x00, 0xca, 0x0f, 0x55, 0xc8, 0xfc, 0x00, 0x00, 0x00,
};
#define BUF_SIZE 0x10000
static voidpf zalloc_func(voidpf opaque, unsigned int items, unsigned int size)
{
    (void)opaque;
    /* not a typo, keep it calloc() */
    return (voidpf)calloc(items, size);
}
static void zfree_func(voidpf opaque, voidpf ptr)
{
    (void)opaque;
    free(ptr);
}
/* Decompress and send to stdout a gzip-compressed buffer */
void hugehelp(void)
{
    unsigned char* buf;
    int status, headerlen;
    z_stream z;

    /* Make sure no gzip options are set */
    if (hugehelpgz[3] & 0xfe)
        return;

    headerlen = 10;
    memset(&z, 0, sizeof(z_stream));
    z.zalloc = (alloc_func)zalloc_func;
    z.zfree = (free_func)zfree_func;
    z.avail_in = (unsigned int)(sizeof(hugehelpgz) - headerlen);
    z.next_in = (unsigned char*)hugehelpgz + headerlen;

    if (inflateInit2(&z, -MAX_WBITS) != Z_OK)
        return;

    buf = malloc(BUF_SIZE);
    if (buf) {
        while (1) {
            z.avail_out = BUF_SIZE;
            z.next_out = buf;
            status = inflate(&z, Z_SYNC_FLUSH);
            if (status == Z_OK || status == Z_STREAM_END) {
                fwrite(buf, BUF_SIZE - z.avail_out, 1, stdout);
                if (status == Z_STREAM_END)
                    break;
            } else
                break; /* Error */
        }
        free(buf);
    }
    inflateEnd(&z);
}
#else /* !USE_MANUAL */
/* built-in manual is disabled, blank function */
#include "tool_hugehelp.h"
void hugehelp(void) {}
#endif /* USE_MANUAL */
#endif /* HAVE_LIBZ */
