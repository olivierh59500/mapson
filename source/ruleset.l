/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

%{
#include <ctype.h>
#include "ruleset_scan.h"
%}
%%
^[ \t]*#.*$	/* ignore comments */
[ \t]		/* ignore whitespace */
\n		rlst_lineno++;

if		return TOK_IF;
=		return TOK_EQUAL;
==		return TOK_EQUAL;
from		return TOK_FROM;
subject		return TOK_SUBJECT;
envelope	return TOK_ENVELOPE;
header		return TOK_HEADER;
and		return TOK_AND;
or		return TOK_OR;
not		return TOK_NOT;
then		return TOK_THEN;
match(es)?	return TOK_MATCH;
\"[^\"]*\"	{
			yytext[yyleng-1] = '\0';
			yytext++;
			yyleng -= 2;
			return TOK_STRING;
		}
drop		return TOK_DROP;
pass		return TOK_PASS;
saveto		return TOK_SAVETO;

.		{ yylval = yytext[0]; return yylval; } /* literal */
%%
