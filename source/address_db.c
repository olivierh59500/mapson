/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <db.h>
#include <errno.h>
#ifndef O_EXLOCK
#  define O_EXLOCK 0
#endif

#include <myexceptions.h>
#include <paths.h>
#include "mapson.h"

/********** static variables **********/

DB * db = NULL;

/********** Internal routines **********/

static int
dbclose(DB * db)
{
    return (db->close)(db);
}

static int
dbget(DB * db, DBT * key, DBT * data, u_int flags)
{
    return (db->get)(db, key, data, flags);
}

static int
dbput(DB * db, DBT * key, DBT * data, u_int flags)
{
    return (db->put)(db, key, data, flags);
}

DBT *
init_dbt(char * string)
{
    DBT *   dbt;

    dbt = fail_safe_malloc(sizeof(DBT));
    if (dbt == NULL)
      return NULL;

    dbt->data = fail_safe_strdup(string);
    if (dbt->data == NULL) {
        free(dbt);
        return NULL;
    }
    dbt->size = strlen(string);

    return dbt;
}

void
free_dbt(DBT * dbt)
{
    free(dbt->data);
    free(dbt);
}

static void
close_address_database(void)
{
    assert(db != NULL);
    dbclose(db);
}

static void
open_address_database(void)
{
    char *   home_directory;
    char *   database_path;

    if (db != NULL)
      return;

    home_directory = get_home_directory();
    database_path = fail_safe_sprintf("%s/%s", home_directory,
				      MAPSON_ADDRESS_DB_FILE_PATH);
    free(home_directory);

    db = dbopen(database_path, O_RDWR | O_CREAT | O_EXLOCK, 0600, DB_HASH, NULL);
    if (db == NULL) {
	log("Failed to open address database '%s': %s", database_path,
	    strerror(errno));
	free(database_path);
	THROW(IO_EXCEPTION);
    }
    free(database_path);

    atexit(close_address_database);
}


/********** Public routines **********/

void
add_address_to_database(char * address)
{
    DBT *    key,
	*    data;
    int      rc;

    open_address_database();
    data = key = init_dbt(address);
    rc = dbput(db, key, data, R_NOOVERWRITE);
    free_dbt(key);
    if (rc == -1) {
	log("Inserting address '%s' to the database failed: %s", address,
	    strerror(errno));
	THROW(ADDRESS_DATABASE_EXCEPTION);
    }

}

bool
does_address_exist_in_database(char * address)
{
    DBT *    key,
	*    data;
    int      rc;

    open_address_database();
    key = init_dbt(address);
    data = fail_safe_malloc(sizeof(DBT));
    rc = dbget(db, key, data, 0);
    free_dbt(key);
    free(data);
    if (rc == -1) {
	log("Finding address '%s' in the database failed: %",
	    strerror(errno), address);
	THROW(ADDRESS_DATABASE_EXCEPTION);
    }

    if (rc == 0)
      return TRUE;
    else
      return FALSE;
}
