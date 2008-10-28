/*                           (C) 1996 C. Barth Netterfield */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*** Header file for creaddata.c, which reads barth's frame files ***/

#ifndef _CREADDATA_H
#define _CREADDATA_H

/* define error codes */
#include "readdata.h"

#define E_OPEN_CSFILE      10
#define E_CSFILE_FORMAT    11
#define E_SIZE_MISMATCH	   12
#define E_OPEN_LINFILE     13
#define E_RECURSE_LEVEL    14

/* Define conversion rule types */
#define CST_END 0
#define CST_LINCOM    1
#define CST_LINFILE   2
#define CST_MPLEX     3
#define CST_FRAMETIME 4
#define CST_BITFIELD  5
#define CST_SIN       6
#define CST_COS       7
#define CST_ASIN      8
#define ATAN2         9
#define CST_SHR      20
#define CST_SHL      21
#define CST_DIFF     22

extern const char *const CRD_ERROR_CODES[15];

#ifdef __cplusplus
extern "C" {
#endif
           int CReadData(const char *filename, const char *field_code,
                         int first_sframe, int first_samp,
                         int num_sframes, int num_samp,
                         char return_type, void *data_out,
                         int *error_code);
#ifdef __cplusplus
}
#endif
#endif
