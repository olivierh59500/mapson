/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

#include <myexceptions.h>
#include "mapson.h"
#include "version.h"

int
main(int argc, char * argv[])
{
    struct Mail *  Mail;
    FILE *         fh;
    char *         mail_rescue_filename,
         *         mail_buffer,
	 *         escaped_mail_buffer,
	 *         p,
	 *         cookie;
    unsigned int   mail_size;
    char           buffer[4096];
    int            rc, i;


    /* First of all initialize our environment. */

    assert_mapson_home_dir_exists();
    assert_mapson_spool_dir_exists();
    mail_rescue_filename = get_mail_rescue_filename();


    /* Copy the mail into the rescue file. */

    fh = fopen(mail_rescue_filename, "r+");
    if (fh == NULL) {
	log("Failed to open mail rescue file '%s': %s",
	       mail_rescue_filename, strerror(errno));
	THROW(IO_EXCEPTION);
    }

    for (rc = 1, mail_size = 0; rc != 0; mail_size += rc) {
	rc = fread(buffer, 1, sizeof(buffer), stdin);
	if (rc > 0)
	  fail_safe_fwrite(buffer, 1, rc, fh);

	if (rc == 0 && !feof(stdin)) {
	    log("I/O error while reading mail: %s", strerror(errno));
	    THROW(IO_EXCEPTION);
	}
    }


    /* Now read the mail into the buffer. */

    if (fseek(fh, 0L, SEEK_SET) != 0) {
	log("I/O while reading mail from rescue file '%s': %s",
	       mail_rescue_filename, strerror(errno));
	THROW(IO_EXCEPTION);
    }
    mail_buffer = fail_safe_malloc(mail_size+1);
    rc = fread(mail_buffer, 1, mail_size, fh);
    if (rc != mail_size) {
	log("I/O while reading mail from rescue file '%s': %s",
	       mail_rescue_filename, strerror(errno));
	THROW(IO_EXCEPTION);
    }
    fclose(fh);
    mail_buffer[mail_size] = '\0';


    /* Create the cookie for this mail. */

    cookie = generate_cookie(mail_buffer);


    /* Parse the mail. */

    TRY {
	Mail = parse_mail(mail_buffer);
    }
    HANDLE(INVALID_ADDRESS_EXCEPTION) {
	log("One or several addresses in the mail are syntactically incorrect.");
	log("I'll leave it in a rescue file and exit.");
	exit(0);
    }
    OTHERWISE {
	PASSTHROUGH();
    }

    /* Check whether the mail is complete. */

    rc = 0;
    if (Mail->envelope == NULL) {
	log("The incoming mail's envelope is syntactically incorrect.");
	rc++;
    }
    if (Mail->message_id == NULL) {
	log("The incoming mail has no message-id.");
	rc++;
    }
    if (Mail->from == NULL || (Mail->from)[0] == NULL) {
	log("The incoming mail has no From: line.");
	rc++;
    }
    if (rc > 0) {
	log("I'll leave it in a rescue file and exit.");
	exit(0);
    }

    /* Check whether the mail is a request for confirmation. */

    if ((fail_safe_pattern_match(Mail->header,
             "^Subject: \\[mapSoN\\] Request for Confirmation") == TRUE) ||
	(fail_safe_pattern_match(Mail->header,
	     "^X-mapSoN: requesting confirmation") == TRUE)) {

	/* Mail is an incoming request for confirmation. */

	p = is_confirmation_mail(mail_buffer);
	log("Received request for confirmation for mail '%s'.", p);
	save_to(mail_buffer, get_mailbox_path());
	goto terminate;
    }

    /* Check whether the mail is a comfirmation of an rfcmail. */

    p = is_confirmation_mail(mail_buffer);
    if (p != NULL) {
	log("Received confirmation for '%s'.", p);
	free_mail(Mail);
	free(mail_buffer);
	TRY {
	    mail_buffer = get_mail_from_spool(p);
	}
	HANDLE(IO_EXCEPTION) {
	    log("Mail '%s' is not there.", p);
	    PASSTHROUGH();
	}
	OTHERWISE {
	    PASSTHROUGH();
	}
	Mail = parse_mail(mail_buffer);
	for (i = 0; Mail->from && (Mail->from)[i] != NULL; i++) {
	    log("Adding '%s' to accept-database.", (Mail->from)[i]);
	    add_address_to_database((Mail->from)[i]);
	}
	save_to(mail_buffer, get_mailbox_path());
	free(p);
	goto terminate;
    }

    /* Escape "From " strings in the mail body, so that the mail
       readers don't get confused. */

    escaped_mail_buffer = escape_from_lines(mail_buffer);

    /* Let the rulset check the mail to decide what we'll do with it. */

    rc = check_ruleset_file(Mail, &p);
    switch(rc) {
      case RLST_CONTINUE:
	  log("No rule in ruleset matched '%s'.", Mail->message_id);
	  for (i = 0; Mail->from && (Mail->from)[i] != NULL; i++) {
	      if (does_address_exist_in_database((Mail->from)[i]) == TRUE) {
		  log("Sender '%s' is known. Delivering mail '%s'.",
			 (Mail->from)[i], Mail->message_id);
		  save_to(escaped_mail_buffer, get_mailbox_path());
		  break;
	      }
	  }

	  if ((Mail->from)[i] == NULL) {
	      log("Sender '%s' is unknown. Requesting confirmation for '%s'.",
		     Mail->envelope, Mail->message_id);
	      store_mail_in_spool(mail_buffer, cookie);
	      send_request_for_confirmation_mail(Mail->envelope, cookie);
	  }
	  break;
      case RLST_PASS:
	  log("Letting mail '%s' pass due to ruleset.",
		 Mail->message_id);
	  for (i = 0; Mail->from && (Mail->from)[i] != NULL; i++) {
	      log("Adding '%s' to accept-database.", (Mail->from)[i]);
	      add_address_to_database((Mail->from)[i]);
	  }
	  save_to(escaped_mail_buffer, get_mailbox_path());
	  break;
      case RLST_QUICKPASS:
	  log("Quickpassing mail '%s' pass due to ruleset.",
		 Mail->message_id);
	  save_to(escaped_mail_buffer, get_mailbox_path());
	  break;
      case RLST_DROP:
	  log("Dropping mail '%s' from '%s'.\n",
		 Mail->message_id, (Mail->from)[0]);
	  break;
      case RLST_RFC:
	  log("Requesting confirmation for '%s' from '%s'.\n",
		 Mail->message_id, Mail->envelope);
	  store_mail_in_spool(mail_buffer, Mail->message_id);
	  send_request_for_confirmation_mail(Mail->envelope, Mail->message_id);
	  break;
      case RLST_SAVETO:
	  assert(p != NULL);
	  if (!p) {
	      THROW(UNKNOWN_FATAL_EXCEPTION);
	  }
	  log("Writing mail '%s' to file '%s'.",
		 Mail->message_id, p);
	  save_to(escaped_mail_buffer, p);
	  break;
      default:
	  assert(0==1);
	  THROW(UNKNOWN_FATAL_EXCEPTION);
    }


    /* Terminating gracefully. */
terminate:

#ifdef DEBUG_DMALLOC
    /*
       Freeing the buffers directly before the exit isn't particular
       useful, but it will reduce the output of the dmalloc library,
       in case I am debugging the code.
     */
    free_mail(Mail);
    free(mail_buffer);
    if (escaped_mail_buffer != mail_buffer)
      free(escaped_mail_buffer);
#endif
    remove(mail_rescue_filename);
    return 0;
}
