/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

#ifndef __MAPSON_H__
#define __MAPSON_H__

#ifdef DEBUG_DMALLOC
#  include <dmalloc.h>
#endif

/********** Prototypes for mapSoN's routines **********/

/* main.c */

int main(int argc, char *argv[]);

/* mail_rescue.c */

char *get_home_directory(void);
char *get_mail_rescue_filename(void);

/* fail_safe.c */

void *fail_safe_malloc(size_t size);
char *fail_safe_strdup(char *string);
char *fail_safe_sprintf(const char *fmt, ...  );
void *fail_safe_realloc(void * ptr, size_t size);
void *fail_safe_calloc(size_t nmemb, size_t size);
void fail_safe_fwrite(void *buffer, size_t size, size_t nmemb, FILE *stream);

/* array.c */

struct array_state {
    char **       array;
    unsigned int  array_size;
    unsigned int  array_pos;
};
typedef struct array_state * array_t;

array_t build_array(void  );
void append_to_array(array_t state, char *string);
char **get_array(array_t state);
void free_array(char **array);

/* parse_mail.c */

struct Mail {
    char *      header;		/* continuation lines are gone here */
    char *      envelope;
    char **     from;
    char **     reply_to;
    char **     to;
    char **     cc;
};

struct Mail * parse_mail(char * buffer);

#endif /* !defined(__MAPSON_H__) */
