/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      This file is derived from the the RSA Data Security, Inc. MD5
 *      Message-Digest Algorithm and has been modified for the mapSoN
 *      context and formatting.
 */
/*
 * Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 *
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 *
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 *
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 *
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 */

#ifndef __MD5_H__
#define __MD5_H__

#include <sys/types.h>

typedef unsigned char *  POINTER;

#if (SIZEOF_UNSIGNED_INT == 4)
   typedef unsigned int UINT4;
#else
#  if (SIZEOF_UNSIGNED_LONG == 4)
      typedef unsigned long UINT4;
#  else
#    if (SIZEOF_UNSIGNED_SHORT == 4)
        typedef unsigned short UINT4;
#    else
#      error "Ooops, we don't have a 4 byte type in C on this machine?"
#    endif
#  endif
#endif

/* MD5 context. */
typedef struct MD5Context {
    UINT4           state[4];	/* state (ABCD) */
    UINT4           count[2];	/* number of bits, modulo 2^64 (lsb first) */
    unsigned char   buffer[64];	/* input buffer */
} MD5_CTX;

void	MD5Init(MD5_CTX *);
void	MD5Update(MD5_CTX *, const unsigned char *, unsigned int);
void	MD5Final(unsigned char[16], MD5_CTX *);
char	*MD5End (MD5_CTX *, char *);
char	*MD5File (const char *, char *);
char	*MD5Data (const unsigned char *, unsigned int, char *);

#endif /* ___MD5_H__ */
