/*-----------------------------------------------------------------------------------
 *  Copyright (c) 2014,2015, ReTiS Lab., Scuola Superiore Sant'Anna.
 *
 *  This file is part of tres_omnetpp.
 *
 *  You can redistribute it and/or modify tres_omnetpp under the terms
 *  of the Academic Public License as published at
 *  http://www.omnetpp.org/intro/license.
 *
 *  tres_omnetpp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY. See the file `AP_License' for details on this
 *  and other legal matters.
 *--------------------------------------------------------------------------------- */

/**
 * \file tresdefs.h
 */

#ifndef TRES_TRESDEFS_HDR
#define TRES_TRESDEFS_HDR
#include <platdefs.h>

NAMESPACE_BEGIN

#if defined(TRESENV_EXPORT)
#  define TRESENV_API OPP_DLLEXPORT
#elif defined(TRESENV_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define TRESENV_API OPP_DLLIMPORT
#else
#  define TRESENV_API
#endif


NAMESPACE_END


#endif
