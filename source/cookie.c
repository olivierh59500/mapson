/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

#include "mapson.h"
#include "md5.h"

char *
encode_digest_to_ascii(unsigned char digest[16])
{
    int                  i;
    static const char    hex[] = "0123456789abcdef";
    char *               buffer;

    buffer = fail_safe_malloc(33);
    for (i = 0; i < 16; i++) {
	buffer[i+i] = hex[digest[i] >> 4];
	buffer[i+i+1] = hex[digest[i] & 0x0f];
    }

    buffer[i+i] = '\0';
    return buffer;
}

char *
generate_cookie(const char * buffer)
{
    MD5_CTX        context;
    unsigned char  digest[16];

    MD5Init(&context);
    MD5Update(&context, (unsigned char *) buffer, strlen((char *) buffer));
    MD5Final(digest, &context);
    return encode_digest_to_ascii(digest);
}

