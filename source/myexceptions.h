/*
 *      $Source$
 *      $Revision$
 *      $Date$
 *
 *      Copyright (C) 1997 by Peter Simons, Germany
 *      All rights reserved.
 */

#ifndef __LIB_MY_EXCEPTIONS_H__
#define __LIB_MY_EXCEPTIONS_H__ 1

#include <exceptions.h>

enum {
    UNKNOWN_FATAL_EXCEPTION = BEGIN_USER_DEFINED_EXCEPTION,
    MAPSON_HOME_DIR_EXCEPTION,
    ADDRESS_DATABASE_EXCEPTION,
    INVALID_ADDRESS_EXCEPTION
};

#endif /* !__LIB_MY_EXCEPTIONS_H__ */
