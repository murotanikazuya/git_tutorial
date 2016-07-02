/*================================================================
** Copyright 2000, Clark Cooper
** All rights reserved.
**
** This is free software. You are permitted to copy, distribute, or modify
** it under the terms of the MIT/X license (contained in the COPYING file
** with this distribution.)
*/

#ifndef EXPAT_OS_H
#define EXPAT_OS_H

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#define XML_BUILDING_EXPAT  1

#define XML_NS              1
#define XML_DTD             1
#define XML_CONTEXT_BYTES   1024

/* we will assume all Windows platforms are little endian */
#define BYTEORDER           1234

/* Windows has memmove() available. */
#define HAVE_MEMMOVE


#endif /* EXPAT_OS_H */

