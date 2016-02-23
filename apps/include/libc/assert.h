/*++

Copyright (c) 2013 Minoca Corp. All Rights Reserved

Module Name:

    assert.h

Abstract:

    This header contains the definition for the assert function.

Author:

    Evan Green 27-May-2013

--*/

#ifndef _ASSERT_H
#define _ASSERT_H

//
// ------------------------------------------------------------------- Includes
//

#include <libcbase.h>
#include <stddef.h>

//
// --------------------------------------------------------------------- Macros
//

#ifndef NDEBUG

#define assert(_Expression) \
    ((_Expression) ? (void)0 : _assert(#_Expression, __FILE__, __LINE__))

#else

#define assert(_Condition) ((void)0)

#endif

#endif

//
// ---------------------------------------------------------------- Definitions
//

#ifdef __cplusplus

extern "C" {

#endif

//
// -------------------------------------------------------- Function Prototypes
//

LIBC_API
void
_assert (
    char *Expression,
    char *File,
    int Line
    );

/*++

Routine Description:

    This routine implements the underlying assert function that backs the
    assert macro.

Arguments:

    File - Supplies a pointer to the null terminated string describing the file
        the assertion failure occurred in.

    Line - Supplies the line number the assertion failure occurred on.

    Expression - Supplies a pointer to the string representation of the
        source expression that failed.

Return Value:

    This routine does not return.

--*/

#ifdef __cplusplus

}

#endif
