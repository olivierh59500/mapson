/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

%{
        /* Definitions we need in the parser. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <assert.h>

#include <myexceptions.h>
#include "mapson.h"

static int yyerror(char *);
static int yylex(void);

unsigned int           rlst_lineno;
static struct Mail *   rlst_mail;
static char *          rlst_parameter;

#define YYERROR_VERBOSE
#include "ruleset_scan.c"
%}
%token TOK_IF TOK_THEN TOK_EQUAL TOK_MATCH TOK_FROM TOK_SUBJECT
%token TOK_STRING TOK_ENVELOPE TOK_HEADER TOK_MAIL TOK_PASS TOK_RFC
%token TOK_DROP TOK_SAVETO TOK_AND TOK_OR TOK_NOT
%left  TOK_AND TOK_OR
%right TOK_NOT
%%

input:    /* empty */
        | input statmt
;

statmt:   ';'
        | TOK_IF exp TOK_THEN action ';'
;

exp:      qualifier TOK_EQUAL TOK_STRING
        | qualifier TOK_MATCH TOK_STRING
        | exp TOK_OR exp                  { $$ = $1 || $3; }
        | exp TOK_AND exp                 { $$ = $1 && $3; }
        | TOK_NOT exp                     { $$ = ! $2; }
        | '(' exp ')'                     { $$ = $2; }
;

qualifier: TOK_FROM                       { $$ = TOK_FROM; }
        | TOK_SUBJECT                     { $$ = TOK_SUBJECT; }
        | TOK_ENVELOPE                    { $$ = TOK_ENVELOPE; }
        | TOK_HEADER                      { $$ = TOK_HEADER; }
        | TOK_MAIL                        { $$ = TOK_MAIL; }
;

action:   TOK_PASS                        { $$ = TOK_PASS; }
        | TOK_DROP                        { $$ = TOK_DROP; }
        | TOK_RFC                         { $$ = TOK_RFC; }
        | TOK_SAVETO TOK_STRING		  {
                                            $$ = TOK_SAVETO;
                                            if (rlst_parameter != NULL)
                                              free(rlst_parameter);
                                            rlst_parameter = fail_safe_strdup(yytext);
                                          }
;
%%
/***** internal routines *****/

int
yywrap(void)
{
    return 1;
}

static int
yyerror(char * string)
{
    syslog(LOG_ERR, "Syntax error in ruleset file, line %u: %s\n", rlst_lineno, string);
    return 0;
}

/****** public routines ******/

int
check_ruleset_file(struct Mail *   Mail,
		   char **         parameter_ptr)
{
    char *  home_dir;
    char *  filename;
    int     rc;


    /* Sanity checks. */

    assert(Mail != NULL);
    assert(parameter_ptr != NULL);


    /* Init parser and lexer. */

    rlst_mail   = Mail;
    rlst_lineno = 1;
    BEGIN(INITIAL);

    home_dir = get_home_directory();
    filename = fail_safe_sprintf("%s/.mapson/ruleset", home_dir);
    free(home_dir);

    yyin = fopen(filename, "r");
    if (yyin == NULL) {
        if (errno == ENOENT) {
	    syslog(LOG_WARNING, "Ruleset file '%s' does not exist!", filename);
	    free(filename);
	    return 0;
	}
	else {
	    syslog(LOG_ERR, "Couldn't open ruleset file '%s': %m", filename);
	    free(filename);
	    THROW(IO_EXCEPTION);
        }
    }
    free(filename);


    /* Parse the ruleset file. */

    rc = yyparse();
    fclose(yyin);
    yyin = NULL;

    if (rc != 0) {
        syslog(LOG_ERR, "Error occured while parsing the ruleset file.");
        THROW(RULESET_FILE_PARSE_EXCEPTION);
    }


    /* Return to the caller. */

    *parameter_ptr = NULL;
    return 0;
}
