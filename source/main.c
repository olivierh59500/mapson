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
#include <assert.h>
#include <syslog.h>
#ifndef LOG_PERROR
#  define LOG_PERROR 0
#endif

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
	 *         p;
    unsigned int   mail_size;
    char           buffer[4096];
    int            rc, i;


    /* First of all initialize our environment. */

    openlog("mapson", LOG_CONS | LOG_PERROR | LOG_PID, LOG_MAIL);
    assert_mapson_home_dir_exists();
    assert_mapson_spool_dir_exists();
    mail_rescue_filename = get_mail_rescue_filename();


    /* Copy the mail into the rescue file. */

    fh = fopen(mail_rescue_filename, "r+");
    if (fh == NULL) {
	syslog(LOG_ERR, "Failed to open mail rescue file '%s': %m",
	       mail_rescue_filename);
	THROW(IO_EXCEPTION);
    }

    for (rc = 1, mail_size = 0; rc != 0; mail_size += rc) {
	rc = fread(buffer, 1, sizeof(buffer), stdin);
	if (rc > 0)
	  fail_safe_fwrite(buffer, 1, rc, fh);

	if (rc == 0 && !feof(stdin)) {
	    syslog(LOG_ERR, "I/O error while reading mail: %m");
	    THROW(IO_EXCEPTION);
	}
    }


    /* Now read the mail into the buffer. */

    if (fseek(fh, 0L, SEEK_SET) != 0) {
	syslog(LOG_ERR, "I/O while reading mail from rescue file '%s': %m",
	       mail_rescue_filename);
	THROW(IO_EXCEPTION);
    }
    mail_buffer = fail_safe_malloc(mail_size+1);
    rc = fread(mail_buffer, 1, mail_size, fh);
    if (rc != mail_size) {
	syslog(LOG_ERR, "I/O while reading mail from rescue file '%s': %m",
	       mail_rescue_filename);
	THROW(IO_EXCEPTION);
    }
    fclose(fh);
    mail_buffer[mail_size] = '\0';

    /* Parse the mail. */

    TRY {
	Mail = parse_mail(mail_buffer);
    }
    HANDLE(INVALID_ADDRESS_EXCEPTION) {
	syslog(LOG_WARNING, "One or several addresses in the mail are syntactically incorrect.");
	syslog(LOG_WARNING, "I'll leave it in a rescue file and exit.");
	exit(0);
    }
    OTHERWISE {
	PASSTHROUGH();
    }

    /* Check whether the mail is complete. */

    if (Mail->envelope == NULL ||
	Mail->message_id == NULL ||
	Mail->from == NULL || (Mail->from)[0] == NULL) {
	syslog(LOG_WARNING, "The incoming mail is syntactically incorrect.");
	syslog(LOG_WARNING, "I'll leave it in a rescue file and exit.");
	exit(0);
    }


    /* Let the rulset check the mail to decide what we'll do with it. */

    rc = check_ruleset_file(Mail, &p);
    switch(rc) {
      case RLST_CONTINUE:
	  syslog(LOG_DEBUG, "No rule in ruleset matched.");
	  p = is_confirmation_mail(mail_buffer);
	  if (p != NULL) {
	      syslog(LOG_INFO, "Received confirmation for '%s'.", p);
	      free_mail(Mail);
	      free(mail_buffer);
	      TRY {
		  mail_buffer = get_mail_from_spool(p);
	      }
	      HANDLE(IO_EXCEPTION) {
		  syslog(LOG_ERR, "Mail '%s' is not there.", p);
		  PASSTHROUGH();
	      }
	      OTHERWISE {
		  PASSTHROUGH();
	      }
	      Mail = parse_mail(mail_buffer);
	      for (i = 0; Mail->from && (Mail->from)[i] != NULL; i++) {
		  syslog(LOG_INFO, "Adding '%s' to accept-database.", (Mail->from)[i]);
		  add_address_to_database((Mail->from)[i]);
	      }
	      save_to(mail_buffer, get_mailbox_path());
	      free(p);
	  }
	  else {
	      syslog(LOG_DEBUG, "Mail '%s' is no confirmation.",
		     Mail->message_id);
	      for (i = 0; Mail->from && (Mail->from)[i] != NULL; i++) {
		  if (does_address_exist_in_database((Mail->from)[i]) == TRUE) {
		      syslog(LOG_INFO, "Sender '%s' is known. Delivering mail '%s'.",
			     (Mail->from)[i], Mail->message_id);
		      save_to(mail_buffer, get_mailbox_path());
		      break;
		  }
	      }

	      if ((Mail->from)[i] == NULL) {
		  syslog(LOG_INFO, "Sender '%s' is unknown. Requesting confirmation for '%s'.",
			 Mail->envelope, Mail->message_id);
		  store_mail_in_spool(mail_buffer, Mail->message_id);
		  send_request_for_confirmation_mail(Mail->envelope, Mail->message_id);
	      }
	  }
	  break;
      case RLST_PASS:
	  syslog(LOG_INFO, "Letting mail '%s' pass due to ruleset.",
		 Mail->message_id);
	  for (i = 0; Mail->from && (Mail->from)[i] != NULL; i++) {
	      syslog(LOG_INFO, "Adding '%s' to accept-database.", (Mail->from)[i]);
	      add_address_to_database((Mail->from)[i]);
	  }
	  save_to(mail_buffer, get_mailbox_path());
	  break;
      case RLST_QUICKPASS:
	  syslog(LOG_INFO, "Quickpassing mail '%s' pass due to ruleset.",
		 Mail->message_id);
	  save_to(mail_buffer, get_mailbox_path());
	  break;
      case RLST_DROP:
	  syslog(LOG_INFO, "Dropping mail '%s' from '%s'.\n",
		 Mail->message_id, (Mail->from)[0]);
	  break;
      case RLST_RFC:
	  syslog(LOG_INFO, "Requesting confirmation for '%s' from '%s'.\n",
		 Mail->message_id, Mail->envelope);
	  store_mail_in_spool(mail_buffer, Mail->message_id);
	  send_request_for_confirmation_mail(Mail->envelope, Mail->message_id);
	  break;
      case RLST_SAVETO:
	  assert(p != NULL);
	  if (!p) {
	      THROW(UNKNOWN_FATAL_EXCEPTION);
	  }
	  syslog(LOG_INFO, "Writing mail '%s' to file '%s'.",
		 Mail->message_id, p);
	  save_to(mail_buffer, p);
	  break;
      default:
	  assert(0==1);
	  THROW(UNKNOWN_FATAL_EXCEPTION);
    }


    /* Terminating gracefully. */

#ifdef DEBUG_DMALLOC
    /*
       Freeing the buffers directly before the exit isn't particular
       useful, but it will reduce the output of the dmalloc library,
       in case I am debugging the code.
     */
    free_mail(Mail);
    free(mail_buffer);
#endif
    remove(mail_rescue_filename);
    return 0;
}
