/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

/********** Prototypes for mapSoN **********/

/* main.c */

int main(int argc, char *argv[]);

/* mail_rescue.c */

char *get_home_directory(void);
char *get_mail_rescue_filename(void);

/* fail_safe.c */

void *fail_safe_malloc(size_t size);
char *fail_safe_strdup(char *string);
char *fail_safe_sprintf(const char *fmt, ...  );
void fail_safe_fwrite(void *buffer, size_t size, size_t nmemb, FILE *stream);
