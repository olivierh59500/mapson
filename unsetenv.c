/*
** unsetenv.c: ISO C implementation
** Copyright (c) 2003 Michael Schloh von Bennewitz <michael@schloh.com>
** Copyright (c) 2003 Cable & Wireless <http://www.cw.com/de/>
**
** Permission to use, copy, modify, and distribute this software for
** any purpose with or without fee is hereby granted, provided that
** the above copyright notice and this permission notice appear in all
** copies.
**
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
** WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHORS AND COPYRIGHT HOLDERS AND THEIR
** CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
** USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
** OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
** OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
*/

#ifdef HAVE_CONFIG_H
#include <sys/types.h>
#include "config.h"
#endif

#ifndef HAVE_UNSETENV
#include <stdlib.h> /* For putenv(3) and malloc(3) */
#include <string.h> /* For strcpy(3) and strcat(3) */


extern char **environ;

/*
** Implements unsetenv(3) C library function for platforms not including it
*/
void unsetenv(const char *kszName)
{
    int nLen;
    const char *kszTail;
    char **ppEnv;

    if (kszName == 0 || environ == 0)
        return;

    for (kszTail = kszName; *kszTail && *kszTail != '='; kszTail++)
        /* noop */;

    nLen = kszTail - kszName;

    for (ppEnv = environ; *ppEnv != 0; ppEnv++)
        if (strncmp(*ppEnv, kszName, nLen) == 0 && (*ppEnv)[nLen] == '=')
            break;

    while (*ppEnv != 0) {
        *ppEnv = *(ppEnv + 1);
        ppEnv++;
    }
}
#endif /* !HAVE_UNSETENV */

