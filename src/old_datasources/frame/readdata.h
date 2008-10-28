/*                           (C) 1996 C. Barth Netterfield */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*** Header file for readdata.c, which reads StratoPlate files ***/

#ifndef _READDATA_H
#define _READDATA_H

/* define error codes */
#define E_OK               0
#define E_OPEN_FFFILE      1
#define E_FFFILE_FORMAT    2
#define E_FILE_OPEN        3
#define E_BAD_FILETYPE     4
#define E_BAD_CODE         5
#define E_BAD_RETURN_TYPE  6

extern const char *const RD_ERROR_CODES[7];

#ifdef __cplusplus
extern "C" {
#endif
           int ReadData(const char *filename, const char *field_code,
                        int first_sframe, int first_samp,
                        int num_sframes, int num_samp,
                        char return_type, void *data_out,
                        int *error_code);
#ifdef __cplusplus
}
#endif
#endif
