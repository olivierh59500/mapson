/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <syslog.h>
#include <errno.h>

#include <myexceptions.h>
#include "mapson.h"

static unsigned int   rlst_lineno;
static struct Mail *  rlst_mail;
static int            rlst_rc;
static char *         rlst_pattern;
static char *         rlst_parameter;
static int            rlst_action;
static bool           rlst_donot;

%}
%x action quote not parameter
%%
\n				{
    bool rc;

    printf("%d: Pattern: '%s', Action: %d, Parameter '%s', NOT: %d\n",
	   rlst_lineno, rlst_pattern, rlst_action, rlst_parameter, rlst_donot);
    if (rlst_pattern != NULL && rlst_action != -1) {
	rc = fail_safe_pattern_match(rlst_mail->header, rlst_pattern);
	if (rlst_donot == TRUE)
	  rc = !rc;

	if (rc == TRUE) {
	    printf("*** Pattern matched ***\n");
	    free(rlst_pattern);
	    rlst_rc = rlst_action;
	    return 0;
	}
    }

    if (rlst_pattern != NULL)
      free(rlst_pattern);
    if (rlst_parameter != NULL)
      free(rlst_parameter);
    rlst_pattern   = NULL;
    rlst_parameter = NULL;
    rlst_action    = -1;
    rlst_donot     = FALSE;
    rlst_lineno++;
				}

^[ \t#].*$			/* ignore comments */

^[^ \t\n"!]+			{
				  if (yytext[0] == ' ' ||
				      yytext[0] == '\t' ||
				      yytext[0] == '#') {
					REJECT;
				  }
				  else {
					rlst_pattern = fail_safe_strdup(yytext);
					BEGIN(action);
				  }
				}

^![ \t]*			{ rlst_donot = TRUE; BEGIN(not); }
<not>\"				{ BEGIN(quote); }
<not>[^ \t\n"]+			{
				  rlst_pattern = fail_safe_strdup(yytext);
				  BEGIN(action);
				}

^\"				{ BEGIN(quote); }
<quote>[^\"]*			{ rlst_pattern = fail_safe_strdup(yytext); }
<quote>\"			{ BEGIN(action); }

<action>[ \t]+			/* Eat up whitespace. */
<action>continue[ \t]*$		{ rlst_action = RLST_CONTINUE; BEGIN(INITIAL); }
<action>pass[ \t]*$		{ rlst_action = RLST_PASS; BEGIN(INITIAL); }
<action>quickpass[ \t]*$	{ rlst_action = RLST_QUICKPASS; BEGIN(INITIAL); }
<action>drop[ \t]*$		{ rlst_action = RLST_DROP; BEGIN(INITIAL); }
<action>rfc[ \t]*$		{ rlst_action = RLST_RFC; BEGIN(INITIAL); }
<action>saveto[ \t]+\"		{ rlst_action = RLST_SAVETO; BEGIN(parameter); }

<parameter>[^"]+		{ rlst_parameter = fail_safe_strdup(yytext); }
<parameter>\"[ \t]*$		{ BEGIN(INITIAL); }

<action,INITIAL>.		{
				  printf("%d: Literal '%c'.\n", rlst_lineno, yytext[0]);
				  syslog(LOG_ERR, "Unexpected syntax in rulset file, line %d.", rlst_lineno);
				  return -1;
				}
%%

/********** internal routines **********/

int
yywrap(void)
{
    return 1;
}

/********** public routines **********/

int
check_ruleset_file(struct Mail * mail, char ** parameter_ptr)
{
    char *  filename;
    char *  home_dir;


    /* Sanity checks. */

    assert(mail != NULL);
    assert(parameter_ptr != NULL);
    if (!mail || !parameter_ptr) {
	THROW(UNKNOWN_FATAL_EXCEPTION);
    }


    /* Set the lexer up. */

    rlst_lineno    = 1;
    rlst_mail      = mail;
    rlst_rc        = RLST_CONTINUE;
    rlst_pattern   = NULL;
    rlst_parameter = NULL;
    rlst_action    = -1;
    rlst_donot     = FALSE;
    BEGIN(INITIAL);

    home_dir = get_home_directory();
    filename = fail_safe_sprintf("%s/.mapson/ruleset", home_dir);
    free(home_dir);

    yyin = fopen(filename, "r");
    if (yyin == NULL) {
        switch(errno) {
          case ENOENT:
	      syslog(LOG_WARNING, "Ruleset file '%s' does not exist!", filename);
	      free(filename);
              break;
          default:
              syslog(LOG_ERR, "Couldn't open ruleset file '%s': %m", filename);
	      free(filename);
	      THROW(IO_EXCEPTION);
        }
    }
    free(filename);


    /* Parse the ruleset file. */

    TRY {
	if (yylex() != 0) {
	    syslog(LOG_ERR, "Error occured while parsing the ruleset file!");
	    THROW(RULESET_FILE_PARSE_EXCEPTION);
	}
    }
    HANDLE(REGEX_EXCEPTION) {
	syslog(LOG_ERR, "regular expression error while compiling pattern '%s'.",
	       rlst_pattern);
	if (rlst_pattern)
	  free(rlst_pattern);
	if (rlst_parameter)
	  free(rlst_parameter);
	PASSTHROUGH();
    }
    OTHERWISE {
	fclose(yyin);
	if (rlst_pattern)
	  free(rlst_pattern);
	if (rlst_parameter)
	  free(rlst_parameter);
	PASSTHROUGH();
    }
    fclose(yyin);

    *parameter_ptr = rlst_parameter;
    return rlst_rc;
}
