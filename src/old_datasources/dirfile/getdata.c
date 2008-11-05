/*                           (C) 2002 C. Barth Netterfield */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "getdata.h"
#include "getdata_struct.h"

#define DIRFILE_DEBUG 0

const char *GD_ERROR_CODES[23] = {"OK",
                            "Could not open Format file",
                            "Error in Format file",
                            "Could not open Data file",
                            "Field name too long",
                            "Field code not found in File Format",
                            "Unrecognized return type",
                            "Could not open field file",
                            "Could not open included Format file",
                            "Internal error",
                            " ",
                            "Memory allocation failed",
                            "Size mismatch in linear combination",
                            "Could not open interpolation file",
                            "Too many levels of recursion",
                            "Cannot write non-RAW data",
                            "Could not open field file for writing",
                            "Could not close read-only field file",
                            "Could not write to field file (already in use)",
                            "Could not allocate file lock struct",
                            "Cannot write to LINCOM with multiple raw fields",
                            "Error in Endian conversion"
};

/* Suberror codes -- these don't need to be public */
#define GD_E_FORMAT_SE_BAD_TYPE   0
#define GD_E_FORMAT_SE_BAD_SPF    1
#define GD_E_FORMAT_SE_N_FIELDS   2
#define GD_E_FORMAT_SE_N_COLS     3
#define GD_E_FORMAT_SE_MAX_I      4
#define GD_E_FORMAT_SE_NUMBITS    5
#define GD_E_FORMAT_SE_BITNUM     6
#define GD_E_FORMAT_SE_BITSIZE    7
#define GD_E_FORMAT_SE_FIELD_LEN  8
#define GD_E_FORMAT_SE_BAD_LINE   9
#define GD_E_FORMAT_SE_N_RAW     10

#define GD_E_LINFILE_SE_OPEN      0
#define GD_E_LINFILE_SE_LENGTH    1

static struct {
  int n;
  struct FormatType *F;
} Formats;

static int recurse_level = 0;
static int first_time = 1;
static int getdata_error = GD_E_OK;
static int getdata_suberror = 0;
static char getdata_error_string[MAX_FILENAME_LENGTH + 6];
static char getdata_error_file[MAX_FILENAME_LENGTH + 6];
static int getdata_error_line = 0;


static int DoField(struct FormatType *F, const char *field_code,
    int first_frame, int first_samp,
    int num_frames, int num_samp,
    char return_type, void *data_out,
    int *error_code);

static int DoFieldOut(struct FormatType *F, const char *field_code,
      int first_frame, int first_samp,
      int num_frames, int num_samp, 
      char data_type, void *data_in,
      int *error_code);

/***************************************************************************/
/*                                                                         */
/*    GetLine: read non-comment line from format file                      */
/*        The line is placed in *line.                                     */
/*        Returns 1 if successful, 0 if unsuccessful                       */
/*                                                                         */
/***************************************************************************/
static int GetLine(FILE *fp, char *line, int* linenum) {
  char *ret_val;
  int first_char;
  int i, len;

  do {
    ret_val = fgets(line, MAX_LINE_LENGTH, fp);
    (*linenum)++;
    first_char = 0;
    while (line[first_char] == ' ' || line[first_char] == '\t') ++first_char;
    line += first_char;
  } while (ret_val && (line[0] == '#' || line[0] == 0 || line[1] == 0));


  if (ret_val) {
    /* truncate comments from end of lines */
    len = strlen(line);
    for (i = 0; i < len; i++) {
      if (line[i]=='#')
        line[i] = '\0';
    }

    return(1); /* a line was read */
  }
  return(0);  /* there were no valid lines */
}

/* SetGetDataError: Sets the global error variables for a library error */
static int SetGetDataError(int error, int suberror,
    const char* format_file, int line, const char* token)
{
  getdata_error = error;
  getdata_suberror = suberror;
  getdata_error_line = line;
  if (format_file != NULL)
    strncpy(getdata_error_file, format_file, MAX_FILENAME_LENGTH + 6);
  if (token != NULL)
    strncpy(getdata_error_string, token, MAX_FILENAME_LENGTH + 6);

  return error;
}

/***************************************************************************/
/*                                                                         */
/*    GetDataErrorString: Write a descriptive message in the supplied      */
/*    buffer describing the last library error.  The message may be        */
/*    truncated but should be null terminated.                             */
/*                                                                         */
/*      buffer: memory into which to write the string                      */
/*      buflen: length of the buffer.  GetDataErrorString will not write   */
/*              more than buflen characters (including the trailing '\0')  */
/*                                                                         */
/*    return value: buffer or NULL if buflen < 1                           */
/*                                                                         */
/***************************************************************************/
char* GetDataErrorString(char* buffer, size_t buflen)
{
  char* ptr;

  /* Sanity check */
  if (buffer == NULL || buflen < 1)
    return NULL;

  /* Copy the default error message into the buffer and make sure
   * the result is null terminated */
  strncpy(buffer, GD_ERROR_CODES[getdata_error], buflen - 1);
  buffer[buflen - 1] = 0;

  /* point to the end of the string and reduce buflen appropriately */
  ptr = buffer + strlen(buffer);
  buflen -= strlen(buffer);

  /* add the anciliary data - we use snprintfs here to ensure the resultant
   * string is properly null terminated (while not overflowing the buffer) */
  switch (getdata_error) {
    case GD_E_INTERNAL_ERROR: /* internal error: report line and source file
                                 where it happened */
      snprintf(ptr, buflen, "  [%s,%i]", getdata_error_file,
          getdata_error_line);
      break;
    case GD_E_OPEN_FORMAT: /* main format file couldn't be opened -- report
                              the filename we tried to open */
      snprintf(ptr, buflen, " %s", getdata_error_file);
      break;
    case GD_E_FORMAT: /* syntax errors in the format file -- lots of
                         suberror types here */

      /* No RAW fields specified -- this isn't tied to a particular line */
      if (getdata_suberror == GD_E_FORMAT_SE_N_RAW) {
        snprintf(ptr, buflen, ": no raw fields defined");
        break;
      }

      /* otherwise, add the format filename and line number where the
       * syntax error was found */
      snprintf(ptr, buflen, " on line %i of %s: ", getdata_error_line,
          getdata_error_file);
      buflen -= strlen(ptr);
      ptr += strlen(ptr);

      switch (getdata_suberror) {
        case GD_E_FORMAT_SE_BAD_TYPE: /* bad field type; include the thing
                                         we thought was the type specifier */
          snprintf(ptr, buflen, "bad raw field type: %c",
              getdata_error_string[0]);
          break;
        case GD_E_FORMAT_SE_BAD_SPF: /* SPF < 0 -- print the column we expected
                                        to hold the SPF */
          snprintf(ptr, buflen, "samples per frame out of range: %s",
              getdata_error_string);
          break;
        case GD_E_FORMAT_SE_N_FIELDS: /* number of fields in the LINCOM and
                                         the number of columns in the format
                                         file don't match */
          snprintf(ptr, buflen, "lincom field count out of range: %s",
              getdata_error_string);
          break;
        case GD_E_FORMAT_SE_N_COLS: /* missing data we expected to find on this
                                       line */
          snprintf(ptr, buflen, "missing column");
          break;
        case GD_E_FORMAT_SE_MAX_I: /* max_i out of range (what is an MPLEX?) */
          snprintf(ptr, buflen, "max_i out of range: %s", getdata_error_string);
          break;
        case GD_E_FORMAT_SE_NUMBITS: /* bitfield numbits is less than 1 */
          snprintf(ptr, buflen, "numbits out of range");
          break;
        case GD_E_FORMAT_SE_BITNUM: /* bitnum is less than 0 */
          snprintf(ptr, buflen, "starting bit out of range");
          break;
        case GD_E_FORMAT_SE_BITSIZE: /* bitfield extends past 32 bits */
          snprintf(ptr, buflen, "end of bitfield is out of bounds");
          break;
        case GD_E_FORMAT_SE_FIELD_LEN: /* field name is too long */
          snprintf(ptr, buflen, "field name too long: %s",
              getdata_error_string);
          break;
        case GD_E_FORMAT_SE_BAD_LINE: /* couldn't make heads nor tails of the
                                         line -- ie. a mistyped keyword &c. */
          snprintf(ptr, buflen, "line indecypherable");
          break;
      }
      break;
    case GD_E_OPEN_INCLUDE: /* Couldn't open an INCLUDEd file -- report the
                               included filename as well as the line and name
                               of the format file where it was encountered */
      snprintf(ptr, buflen, " %s on line %i of %s", getdata_error_string,
          getdata_error_line, getdata_error_file);
      break;
    case GD_E_BAD_RETURN_TYPE: /* unsupported data return type passed to
                                  GetData */
      snprintf(ptr, buflen, ": %c", (char)getdata_suberror);
      break;
    case GD_E_RECURSE_LEVEL: /* recursion too deep -- report field name for
                                which this happened */
      snprintf(ptr, buflen, " while resolving field %s", getdata_error_string);
      break;
    case GD_E_BAD_CODE: /* A required field name wasn't defined */
    case GD_E_OPEN_RAWFIELD: /* A raw field file wasn't found on disk */
      snprintf(ptr, buflen, ": %s", getdata_error_string);
      break;
    case GD_E_OPEN_LINFILE: /* problems with LINTERPs: report the linterp
                               filename with the error message */
      snprintf(ptr, buflen, " %s: %s", getdata_error_string,
          (getdata_suberror == GD_E_LINFILE_SE_OPEN) ? "open failed"
          : "file too short");
      break;
  }

  return buffer;
}

/***************************************************************************/
/*                                                                         */
/*   FreeF: free any entries that have been allocated in F                 */
/*                                                                         */
/***************************************************************************/
static void FreeF(struct FormatType *F) {
  if (F->n_raw > 0) free(F->rawEntries);
  if (F->n_lincom > 0) free(F->lincomEntries);
  if (F->n_multiply > 0) free(F->multiplyEntries);
  if (F->n_linterp >0) free(F->linterpEntries);
  if (F->n_mplex > 0) free(F->mplexEntries);
  if (F->n_bit > 0) free(F->bitEntries);
  if (F->n_phase > 0) free(F->phaseEntries);
}

/***************************************************************************/
/*                                                                         */
/*   ParseRaw: parse a RAW data type in the formats file                   */
/*                                                                         */
/***************************************************************************/
static int ParseRaw(char in_cols[MAX_IN_COLS][MAX_LINE_LENGTH],
    struct RawEntryType *R, const char* subdir, const char* format_file,
    int line)
{
  strcpy(R->field, in_cols[0]); /* field */
  snprintf(R->file, MAX_FILENAME_LENGTH + FIELD_LENGTH + 2, "%s/%s", subdir,
      in_cols[0]); /* path and filename */
  R->fp = -1; /* file not opened yet */
  switch (in_cols[2][0]) {
    case 'c':
      R->size = 1;
      break;
    case 's': case 'u':
      R->size = 2;
      break;
    case 'S': case 'U': case 'f': case 'i':
      R->size = 4;
      break;
    case 'd':
      R->size = 8;
      break;
    default:
      return SetGetDataError(GD_E_FORMAT, GD_E_FORMAT_SE_BAD_TYPE, format_file,
          line, in_cols[2]);
  }
  R->type = in_cols[2][0];
  R->samples_per_frame = atoi(in_cols[3]);
  if (R->samples_per_frame<=0) {
    return SetGetDataError(GD_E_FORMAT, GD_E_FORMAT_SE_BAD_SPF, format_file,
        line, in_cols[3]);
  }

  return SetGetDataError(GD_E_OK, 0, NULL, 0, NULL);
}

/***************************************************************************/
/*                                                                         */
/*  ParseLincom: parse a LINCOM data type in the formats file              */
/*                                                                         */
/***************************************************************************/
static int ParseLincom(char in_cols[MAX_IN_COLS][MAX_LINE_LENGTH], int n_cols,
    struct LincomEntryType *L, const char* format_file, int line)
{
  int i;
  strcpy(L->field, in_cols[0]); /* field */
  L->n_infields = atoi(in_cols[2]);
  if ((L->n_infields<1) || (L->n_infields>MAX_LINCOM) ||
      (n_cols < L->n_infields*3 + 3)) {
    return SetGetDataError(GD_E_FORMAT, GD_E_FORMAT_SE_N_FIELDS, format_file,
        line, in_cols[2]);
  }

  for (i=0; i<L->n_infields; i++) {
    strncpy(L->in_fields[i], in_cols[i*3+3], FIELD_LENGTH);
    L->m[i] = atof(in_cols[i*3+4]);
    L->b[i] = atof(in_cols[i*3+5]);
  }

  return SetGetDataError(GD_E_OK, 0, NULL, 0, NULL);
}
/***************************************************************************/
/*                                                                         */
/*  ParseLinterp: parse a LINTERP data type in the formats file            */
/*                                                                         */
/***************************************************************************/
static int ParseLinterp(char in_cols[MAX_IN_COLS][MAX_LINE_LENGTH],
    struct LinterpEntryType *L)
{
  strcpy(L->field, in_cols[0]); /* field */
  strncpy(L->raw_field, in_cols[2], FIELD_LENGTH);
  strcpy(L->linterp_file, in_cols[3]);
  L->n_interp = -1; /* linterp file not read yet */

  return SetGetDataError(GD_E_OK, 0, NULL, 0, NULL);
}

/***************************************************************************/
/*                                                                         */
/*   ParseMultiply: parse MULTIPLY data type entry in formats file         */
/*                                                                         */
/***************************************************************************/
static int ParseMultiply(char in_cols[MAX_IN_COLS][MAX_LINE_LENGTH], int n_cols,
    struct MultiplyEntryType *M, const char* format_file, int line)
{
  if (n_cols < 4)
    return SetGetDataError(GD_E_FORMAT, GD_E_FORMAT_SE_N_COLS, format_file,
        line, NULL);

  strcpy(M->field, in_cols[0]); /* field */

  strncpy(M->in_fields[0], in_cols[2], FIELD_LENGTH);
  strncpy(M->in_fields[1], in_cols[3], FIELD_LENGTH);

  return SetGetDataError(GD_E_OK, 0, NULL, 0, NULL);
}

/***************************************************************************/
/*                                                                         */
/*   ParseMplex: parse MPLEX data type entry in formats file               */
/*                                                                         */
/***************************************************************************/
static int ParseMplex(char in_cols[MAX_IN_COLS][MAX_LINE_LENGTH], int n_cols,
    struct MplexEntryType *M, const char* format_file, int line)
{
  if (n_cols < 6)
    return SetGetDataError(GD_E_FORMAT, GD_E_FORMAT_SE_N_COLS, format_file,
        line, NULL);

  strcpy(M->field, in_cols[0]); /* field */
  strncpy(M->cnt_field, in_cols[2], FIELD_LENGTH);
  strncpy(M->data_field, in_cols[3], FIELD_LENGTH);
  M->i = atoi(in_cols[4]);
  M->max_i = atoi(in_cols[5]);
  if ((M->max_i < 1) || (M->max_i < M->i))
    return SetGetDataError(GD_E_FORMAT, GD_E_FORMAT_SE_MAX_I, format_file,
        line, NULL);

  return SetGetDataError(GD_E_OK, 0, NULL, 0, NULL);
}

/***************************************************************************/
/*                                                                         */
/*   ParseBit: parse BIT data type entry in formats file                   */
/*                                                                         */
/***************************************************************************/
static int ParseBit(char in_cols[MAX_IN_COLS][MAX_LINE_LENGTH], int n_cols,
    struct BitEntryType *B, const char* format_file, int line)
{

  strcpy(B->field, in_cols[0]); /* field */
  strncpy(B->raw_field, in_cols[2], FIELD_LENGTH); /* field */

  B->bitnum=atoi(in_cols[3]);
  if (n_cols>4) {
    B->numbits=atoi(in_cols[4]);
  } else {
    B->numbits=1;
  }

  if (B->numbits < 1)
    return SetGetDataError(GD_E_FORMAT, GD_E_FORMAT_SE_NUMBITS, format_file,
        line, NULL);
  if (B->bitnum < 0)
    return SetGetDataError(GD_E_FORMAT, GD_E_FORMAT_SE_BITNUM, format_file,
        line, NULL);
  if (B->bitnum + B->numbits - 1 > 31)
    return SetGetDataError(GD_E_FORMAT, GD_E_FORMAT_SE_BITSIZE, format_file,
        line, NULL);

  return SetGetDataError(GD_E_OK, 0, NULL, 0, NULL);
}

/***************************************************************************/
/*                                                                         */
/*   ParsePhase: parse PHASE data type entry in formats file               */
/*                                                                         */
/***************************************************************************/
static int ParsePhase(char in_cols[MAX_IN_COLS][MAX_LINE_LENGTH], int n_cols,
    struct PhaseEntryType *P, const char* format_file, int line)
{
  strcpy(P->field, in_cols[0]); /* field */
  strncpy(P->raw_field, in_cols[2], FIELD_LENGTH); /* field */

  P->shift=atoi(in_cols[3]); /*shift*/

  /*FIXME make sure the shift is within the range of the raw field...*/
  return SetGetDataError(GD_E_OK, 0, NULL, 0, NULL);
}

/***************************************************************************/
/*                                                                         */
/*   Compare functions for sorting the lists (using stdlib qsort)          */
/*                                                                         */
/***************************************************************************/
static int RawCmp(const void *A, const void *B) {
  return (strcmp(((struct RawEntryType *)A)->field,
        ((struct RawEntryType *)B)->field));
}

static int LincomCmp(const void *A, const void *B) {
  return (strcmp(((struct LincomEntryType *)A)->field,
        ((struct LincomEntryType *)B)->field));
}

static int LinterpCmp(const void *A, const void *B) {
  return (strcmp(((struct LinterpEntryType *)A)->field,
        ((struct LinterpEntryType *)B)->field));
}

static int MultiplyCmp(const void *A, const void *B) {
  return (strcmp(((struct MultiplyEntryType *)A)->field,
        ((struct MultiplyEntryType *)B)->field));
}

static int MplexCmp(const void *A, const void *B) {
  return (strcmp(((struct MplexEntryType *)A)->field,
        ((struct MplexEntryType *)B)->field));
}

static int BitCmp(const void *A, const void *B) {
  return (strcmp(((struct BitEntryType *)A)->field,
        ((struct BitEntryType *)B)->field));
}

static int PhaseCmp(const void *A, const void *B) {
  return (strcmp(((struct PhaseEntryType *)A)->field,
        ((struct PhaseEntryType *)B)->field));
}

/***************************************************************************/
/*                                                                         */
/*  ParseFormatFile: Perform the actual parsing of the format file.  This  */
/*     function is called from GetFormat once for the main format file and */
/*     once for each included file.                                        */
/*                                                                         */
/***************************************************************************/
static int ParseFormatFile(FILE* fp, struct FormatType *F, const char* filedir,
    const char* subdir, const char* format_file, char*** IncludeList,
    int *i_include)
{
  char instring[MAX_LINE_LENGTH];
  char in_cols[MAX_IN_COLS][MAX_LINE_LENGTH];
  int n_cols, error_code = SetGetDataError(GD_E_OK, 0, NULL, 0, NULL);
  int linenum = 0;

  /***** start parsing ****/
  while (GetLine(fp, instring, &linenum)) {
    /* ok, brute force parse...  slow and ugly but convenient... */
    n_cols = sscanf(instring, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
        in_cols[0], in_cols[1], in_cols[2], in_cols[3],
        in_cols[4], in_cols[5], in_cols[6], in_cols[7],
        in_cols[8], in_cols[9], in_cols[10], in_cols[11],
        in_cols[12], in_cols[13], in_cols[14]);

    if (n_cols<2) {
      error_code = SetGetDataError(GD_E_FORMAT, GD_E_FORMAT_SE_N_COLS,
          format_file, linenum, NULL);
    } else if (strlen(in_cols[0])>FIELD_LENGTH) {
      error_code = SetGetDataError(GD_E_FORMAT, GD_E_FORMAT_SE_FIELD_LEN,
          format_file, linenum, in_cols[0]);
    } else if (strcmp(in_cols[1], "RAW")==0) {
      F->n_raw++;
      F->rawEntries =
        realloc(F->rawEntries, F->n_raw*sizeof(struct RawEntryType));
      error_code = ParseRaw(in_cols, F->rawEntries+F->n_raw - 1, subdir,
          format_file, linenum);
    } else if (strcmp(in_cols[1], "LINCOM")==0) {
      F->n_lincom++;
      F->lincomEntries =
        realloc(F->lincomEntries,
            F->n_lincom*sizeof(struct LincomEntryType));
      error_code = ParseLincom(in_cols, n_cols, F->lincomEntries+F->n_lincom
          - 1, format_file, linenum);
    } else if (strcmp(in_cols[1], "LINTERP")==0) {
      F->n_linterp++;
      F->linterpEntries =
        realloc(F->linterpEntries,
            F->n_linterp*sizeof(struct LinterpEntryType));
      error_code = ParseLinterp(in_cols, F->linterpEntries+F->n_linterp - 1);
    } else if (strcmp(in_cols[1], "MULTIPLY")==0) {
      F->n_multiply++;
      F->multiplyEntries =
        realloc(F->multiplyEntries,
            F->n_multiply*sizeof(struct MultiplyEntryType));
      error_code = ParseMultiply(in_cols, n_cols, F->multiplyEntries +
          F->n_multiply - 1, format_file, linenum);
    } else if (strcmp(in_cols[1], "MPLEX")==0) {
      F->n_mplex++;
      F->mplexEntries =
        realloc(F->mplexEntries,
            F->n_mplex*sizeof(struct MplexEntryType));
      error_code = ParseMplex(in_cols, n_cols, F->mplexEntries+F->n_mplex - 1,
          format_file, linenum);
    } else if (strcmp(in_cols[1], "BIT")==0) {
      F->n_bit++;
      F->bitEntries =
        realloc(F->bitEntries,
            F->n_bit*sizeof(struct BitEntryType));
      error_code = ParseBit(in_cols, n_cols, F->bitEntries+F->n_bit - 1,
          format_file, linenum);
    } else if (strcmp(in_cols[1], "PHASE")==0) {
      F->n_phase++;
      F->phaseEntries =
        realloc(F->phaseEntries,
            F->n_phase*sizeof(struct PhaseEntryType));
      error_code = ParsePhase(in_cols, n_cols, F->phaseEntries+F->n_phase - 1,
          format_file, linenum);
    } else if (strcmp(in_cols[0], "FRAMEOFFSET")==0) {
      F->frame_offset = atoi(in_cols[1]);
    } else if (strcmp(in_cols[0], "INCLUDE")==0) {
      int i, found = 0;
      char temp_buffer[MAX_FILENAME_LENGTH + 6];
      char new_format_file[MAX_FILENAME_LENGTH + 6];
      char new_subdir[MAX_FILENAME_LENGTH + 1];
      FILE* new_fp = NULL;

      /* Run through the include list to see if we've already included this
       * file */
      for (i = 0; i < *i_include; ++i)
        if (strcmp(in_cols[1], (*IncludeList)[i]) == 0) {
          found = 1;
          break;
        }

      /* If we found the file, we won't reopen it.  Continue parsing. */
      if (found)
        continue;

      /* Otherwise, try to open the file */
      snprintf(new_format_file, MAX_FILENAME_LENGTH + 6, "%s/%s/%s", filedir,
          subdir, in_cols[1]);
      new_fp = fopen(new_format_file, "r");

      /* If opening the file failed, set the error code and abort parsing. */
      if (new_fp == NULL) {
        error_code = SetGetDataError(GD_E_OPEN_INCLUDE, 0, format_file, linenum,
            new_format_file);
        break;
      }

      /* If we got here, we managed to open the inlcuded file; parse it */
      *IncludeList = realloc(*IncludeList, ++(*i_include) * sizeof(char*));
      (*IncludeList)[*i_include - 1] = strdup(in_cols[1]);

      /* extract the subdirectory name - dirname both returns a volatile string
       * and modifies its argument, ergo strcpy */
      strcpy(temp_buffer, in_cols[1]);
      if (strcmp(subdir, ".") == 0)
        strcpy(new_subdir, dirname(temp_buffer));
      else
        snprintf(new_subdir, MAX_FILENAME_LENGTH, "%s/%s", subdir,
            dirname(temp_buffer));

      error_code = ParseFormatFile(new_fp, F, filedir, new_subdir,
          new_format_file, IncludeList, i_include);
      fclose(new_fp);
    } else
      error_code = SetGetDataError(GD_E_FORMAT, GD_E_FORMAT_SE_BAD_LINE,
          format_file, linenum, NULL);

    /* break out of loop (so we can return) if we've encountered an error */
    if (error_code != GD_E_OK)
      break;
  }

  return error_code;
}

/***************************************************************************/
/*                                                                         */
/*   GetFormat: Read format file and fill structure.  The format           */
/*      is cached.                                                         */
/*                                                                         */
/***************************************************************************/
struct FormatType *GetFormat(const char *filedir, int *error_code) {
  int i_format, i;
  struct stat statbuf;
  FILE *fp;
  char format_file[MAX_FILENAME_LENGTH+6];
  struct FormatType *F;
  char raw_data_filename[MAX_FILENAME_LENGTH+FIELD_LENGTH+2];
  char **IncludeList = NULL;
  int i_include;

  /** first check to see if we have already read it **/
  for (i_format=0; i_format<Formats.n; i_format++) {
    if (strncmp(filedir,
          Formats.F[i_format].FileDirName, MAX_FILENAME_LENGTH) == 0) {
      *error_code = SetGetDataError(GD_E_OK, 0, NULL, 0, NULL);
      return(Formats.F + i_format);
    }
  }

  /** if we get here, the file has not yet been read */
  /** Allocate the memory, then fill.  If we have an error, */
  /*  we will have to free the memory... */
  Formats.n++;
  Formats.F = realloc(Formats.F, Formats.n * sizeof(struct FormatType));

  F = Formats.F+Formats.n-1;

  /***** open the format file (if there is one) ******/
  snprintf(format_file, MAX_FILENAME_LENGTH+6, "%s/format", filedir);
  fp = fopen(format_file, "r");
  if (fp == NULL) {
    *error_code = SetGetDataError(GD_E_OPEN_FORMAT, 0, format_file, 0, NULL);
    Formats.n--; /* no need to free.  The next realloc will just do nothing */
    return (NULL);
  }

  strcpy(F->FileDirName, filedir);
  F->n_raw = F->n_lincom = F->n_multiply = F->n_linterp = F->n_mplex = F->n_bit = F->n_phase
    = 0;
  F->frame_offset = 0;
  F->rawEntries = NULL;
  F->lincomEntries = NULL;
  F->multiplyEntries = NULL;
  F->linterpEntries = NULL;
  F->mplexEntries = NULL;
  F->bitEntries = NULL;
  F->phaseEntries = NULL;

  /* Parse the file.  This will take care of any necessary inclusions */
  i_include = 1;
  IncludeList = malloc(sizeof(char*));
  IncludeList[0] = strdup("format");
  *error_code = ParseFormatFile(fp, F, filedir, ".", format_file, &IncludeList,
      &i_include);
  fclose(fp);

  /* Clean up IncludeList.  We don't need it anymore */
  for (i = 0; i < i_include; ++i)
    free(IncludeList[i]);
  free(IncludeList);

  if (*error_code!=GD_E_OK) {
    FreeF(F);
    Formats.n--;
    return(NULL);
  }

  for (i=0; i<F->n_raw; i++) {
    snprintf(raw_data_filename, MAX_FILENAME_LENGTH+FIELD_LENGTH+2, 
        "%s/%s", filedir, F->rawEntries[i].file);
    if (stat(raw_data_filename, &statbuf) >=0) {
      F->first_field = F->rawEntries[i];
      break;
    }
  }

  /** Now sort the lists */
  if (F->n_raw > 1) {
    qsort(F->rawEntries, F->n_raw, sizeof(struct RawEntryType),
        RawCmp);
  }

  if (F->n_lincom > 1) {
    qsort(F->lincomEntries, F->n_lincom, sizeof(struct LincomEntryType),
        LincomCmp);
  }
  if (F->n_linterp > 1) {
    qsort(F->linterpEntries, F->n_linterp, sizeof(struct LinterpEntryType),
        LinterpCmp);
  }
  if (F->n_multiply > 1) {
    qsort(F->multiplyEntries, F->n_multiply, sizeof(struct MultiplyEntryType),
        MultiplyCmp);
  }
  if (F->n_mplex > 1) {
    qsort(F->mplexEntries, F->n_mplex, sizeof(struct MplexEntryType),
        MplexCmp);
  }
  if (F->n_bit > 1) {
    qsort(F->bitEntries, F->n_bit, sizeof(struct BitEntryType),
        BitCmp);
  }
  if (F->n_phase > 1) {
    qsort(F->phaseEntries, F->n_phase, sizeof(struct PhaseEntryType),
        PhaseCmp);
  }
  return(F);
}

/***************************************************************************/
/*                                                                         */
/*     Fill File Frame numbers into dataout                                */
/*                                                                         */
/***************************************************************************/
static void FillFileFrame(void *dataout, char rtype, int s0, int n) {
  int i;

  switch(rtype) {
    case 'c':
      for (i=0; i<n; i++) {
        ((unsigned char*)dataout)[i] = (unsigned char)i+s0;
      }
      break;
    case 'i': /* for compatibility with creaddata. (deprecated) */
    case 'S':
      for (i=0; i<n; i++) {
        ((int*)dataout)[i] = (int)i+s0;
      }
      break;
    case 's':
      for (i=0; i<n; i++) {
        ((short*)dataout)[i] = (short)i+s0;
      }
      break;
    case 'U':
      for (i=0; i<n; i++) {
        ((unsigned int *)dataout)[i] = (unsigned int)i+s0;
      }
      break;
    case 'u':
      for (i=0; i<n; i++) {
        ((unsigned short *)dataout)[i] = (unsigned short)i+s0;
      }
      break;
    case 'f':
      for (i=0; i<n; i++) {
        ((float*)dataout)[i] = (float)i+s0;
      }
      break;
    case 'd':
      for (i=0; i<n; i++) {
        ((double*)dataout)[i] = (double)i+s0;
      }
      break;
  }
}
/***************************************************************************/
/*                                                                         */
/*    ConvertType: copy data to output buffer while converting type        */
/*           Returns error code                                            */
/*                                                                         */
/***************************************************************************/
static int ConvertType(const unsigned char *data_in, char in_type,
                       void *data_out, char out_type, int n) {
  int i;

  if (out_type=='n') { /* null return type: don't return data */
    return(0);
  }

  switch (in_type) {
    case 'c':
      switch (out_type) {
        case 'c':
          memcpy(data_out, data_in, n*sizeof(unsigned char));
          break;
        case 's':
          for (i=0;i<n;i++) ((short*)data_out)[i]=data_in[i];
          break;
        case 'u':
          for (i=0;i<n;i++) ((unsigned short*)data_out)[i]=data_in[i];
          break;
        case 'i': case 'S':
          for (i=0;i<n;i++) ((int*)data_out)[i]=data_in[i];
          break;
        case 'U':
          for (i=0;i<n;i++)  ((unsigned int*)data_out)[i]=data_in[i];
          break;
        case 'f':
          for (i=0;i<n;i++) ((float*)data_out)[i]=data_in[i];
          break;
        case 'd':
          for (i=0;i<n;i++) ((double*)data_out)[i]=data_in[i];
          break;
        default:
          return SetGetDataError(GD_E_BAD_RETURN_TYPE, out_type, NULL, 0, NULL);
      }
      break;
    case 's':
      switch (out_type) {
        case 'c':
          for (i=0;i<n;i++) ((unsigned char*) data_out)[i]=((short *)data_in)[i];
          break;
        case 's':
          memcpy(data_out, data_in, n*sizeof(short));
          break;
        case 'u':
          for (i=0;i<n;i++) ((unsigned short*)data_out)[i]=((short *)data_in)[i];
          break;
        case 'S': case 'i':
          for (i=0;i<n;i++) ((int*)data_out)[i]=((short *)data_in)[i];
          break;
        case 'U':
          for (i=0;i<n;i++) ((unsigned int*)data_out)[i]=((short *)data_in)[i];
          break;
        case 'f':
          for (i=0;i<n;i++) ((float*)data_out)[i]=((short *)data_in)[i];
          break;
        case 'd':
          for (i=0;i<n;i++) ((double*)data_out)[i]=((short *)data_in)[i];
          break;
        default:
          return SetGetDataError(GD_E_BAD_RETURN_TYPE, out_type, NULL, 0, NULL);
      }
      break;
    case 'u':
      switch (out_type) {
        case 'c':
          for (i=0;i<n;i++) ((unsigned char*) data_out)[i]=
            ((unsigned short *)data_in)[i];
          break;
        case 's':
          for (i=0;i<n;i++) ((short*)data_out)[i]=
            ((unsigned short *)data_in)[i];
          break;
        case 'u':
          memcpy(data_out, data_in, n*sizeof(unsigned short));
          break;
        case 'i': case 'S':
          for (i=0;i<n;i++) ((int*)data_out)[i]=
            ((unsigned short *)data_in)[i];
          break;
        case 'U':
          for (i=0;i<n;i++)  ((unsigned int*)data_out)[i]=
            ((unsigned short *)data_in)[i];
          break;
        case 'f':
          for (i=0;i<n;i++) ((float*)data_out)[i]=
            ((unsigned short *)data_in)[i];
          break;
        case 'd':
          for (i=0;i<n;i++) ((double*)data_out)[i]=
            ((unsigned short *)data_in)[i];
          break;
        default:
          return SetGetDataError(GD_E_BAD_RETURN_TYPE, out_type, NULL, 0, NULL);
      }
      break;
    case 'i':
    case 'S':
      switch (out_type) {
        case 'c':
          for (i=0;i<n;i++) ((unsigned char*) data_out)[i]=((int *)data_in)[i];
          break;
        case 's':
          for (i=0;i<n;i++) ((short*)data_out)[i]=((int *)data_in)[i];
          break;
        case 'u':
          for (i=0;i<n;i++) ((unsigned short*)data_out)[i]=((int *)data_in)[i];
          break;
        case 'i': case 'S':
          memcpy(data_out, data_in, n*sizeof(int));
          break;
        case 'U':
          for (i=0;i<n;i++)  ((unsigned int*)data_out)[i]=((int *)data_in)[i];
          break;
        case 'f':
          for (i=0;i<n;i++) ((float*)data_out)[i]=((int *)data_in)[i];
          break;
        case 'd':
          for (i=0;i<n;i++) ((double*)data_out)[i]=((int *)data_in)[i];
          break;
        default:
          return SetGetDataError(GD_E_BAD_RETURN_TYPE, out_type, NULL, 0, NULL);
      }
      break;
    case 'U':
      switch (out_type) {
        case 'c':
          for (i=0;i<n;i++) ((unsigned char*) data_out)[i]=
            ((unsigned *)data_in)[i];
          break;
        case 's':
          for (i=0;i<n;i++) ((short*)data_out)[i]=
            ((unsigned *)data_in)[i];
          break;
        case 'u':
          for (i=0;i<n;i++) ((unsigned short*)data_out)[i]=
            ((unsigned *)data_in)[i];
          break;
        case 'i': case 'S':
          for (i=0;i<n;i++) ((int*)data_out)[i]=
            ((unsigned *)data_in)[i];
          break;
        case 'U':
          memcpy(data_out, data_in, n*sizeof(unsigned int));
          break;
        case 'f':
          for (i=0;i<n;i++) ((float*)data_out)[i]=
            ((unsigned *)data_in)[i];
          break;
        case 'd':
          for (i=0;i<n;i++) ((double*)data_out)[i]=
            ((unsigned *)data_in)[i];
          break;
        default:
          return SetGetDataError(GD_E_BAD_RETURN_TYPE, out_type, NULL, 0, NULL);
      }
      break;
    case 'f':
      switch (out_type) {
        case 'c':
          for (i=0;i<n;i++) ((unsigned char*) data_out)[i]=((float *)data_in)[i];
          break;
        case 's':
          for (i=0;i<n;i++) ((short*)data_out)[i]=((float *)data_in)[i];
          break;
        case 'u':
          for (i=0;i<n;i++) ((unsigned short*)data_out)[i]=((float *)data_in)[i];
          break;
        case 'i': case 'S':
          for (i=0;i<n;i++) ((int*)data_out)[i]=((float *)data_in)[i];
          break;
        case 'U':
          for (i=0;i<n;i++)  ((unsigned int*)data_out)[i]=((float *)data_in)[i];
          break;
        case 'f':
          memcpy(data_out, data_in, n*sizeof(float));
          break;
        case 'd':
          for (i=0;i<n;i++) ((double*)data_out)[i]=((float *)data_in)[i];
          break;
        default:
          return SetGetDataError(GD_E_BAD_RETURN_TYPE, out_type, NULL, 0, NULL);
      }
      break;
    case 'd':
      switch (out_type) {
        case 'c':
          for (i=0;i<n;i++) ((unsigned char*) data_out)[i]=((double *)data_in)[i];
          break;
        case 's':
          for (i=0;i<n;i++) ((short*)data_out)[i]=((double *)data_in)[i];
          break;
        case 'u':
          for (i=0;i<n;i++) ((unsigned short*)data_out)[i]=((double *)data_in)[i];
          break;
        case 'i': case 'S':
          for (i=0;i<n;i++) ((int*)data_out)[i]=((double *)data_in)[i];
          break;
        case 'U':
          for (i=0;i<n;i++)  ((unsigned int*)data_out)[i]=((double *)data_in)[i];
          break;
        case 'f':
          for (i=0;i<n;i++) ((float*)data_out)[i]=((double *)data_in)[i];
          break;
        case 'd':
          memcpy(data_out, data_in, n*sizeof(double));
          break;
        default:
          return SetGetDataError(GD_E_BAD_RETURN_TYPE, out_type, NULL, 0, NULL);
      }
      break;
    default:
      return SetGetDataError(GD_E_INTERNAL_ERROR, in_type, __FILE__, __LINE__,
          NULL);
  }

  return SetGetDataError(GD_E_OK, 0, NULL, 0, NULL);
}


/***************************************************************************/
/*                                                                         */
/*      FillZero: fill data buffer with zero/NaN of the appropriate type   */
/*        used if s0<0 - fill up to 0, or up to ns+s0, whichever is less   */
/*                                                                         */
/***************************************************************************/
static int FillZero(unsigned char *databuffer, char type, int s0, int ns) {
  int i, nz;
  const double NaN = NAN;

  if (s0>=0) return 0;

  if (s0+ns>0) nz = -s0;
  else nz = ns;

  switch (type) {
    case 'c':
      memset(databuffer, 0, nz);
      break;
    case 's':
    case 'u':
      memset(databuffer, 0, nz*sizeof(short));
      break;
    case 'i':
    case 'S':
    case 'U':
      memset(databuffer, 0, nz*sizeof(int));
      break;
    case 'f':
      for (i = 0; i < nz; ++i)
        *((float*)databuffer + i) = (float)NaN;
      break;
    case 'd':
      for (i = 0; i < nz; ++i)
        *((double*)databuffer + i) = (double)NaN;
      break;
  }

  return (nz);

}
/***************************************************************************/
/*                                                                         */
/*   Get samples per frame for field, given FormatType *F                  */
/*                                                                         */
/***************************************************************************/
static int GetSPF(const char *field_code, struct FormatType *F, int *error_code) {
  struct RawEntryType tR;
  struct RawEntryType *R;
  struct LincomEntryType tL;
  struct LincomEntryType *L;
  struct MultiplyEntryType tM;
  struct MultiplyEntryType *M;
  struct BitEntryType tB;
  struct BitEntryType *B;
  struct PhaseEntryType tP;
  struct PhaseEntryType *P;
  struct LinterpEntryType tI;
  struct LinterpEntryType *I;
  int spf;

  if (!F) { /* don't crash */
    return(0);
  }

  if (recurse_level > 10) {
    *error_code = SetGetDataError(GD_E_RECURSE_LEVEL, 0, NULL, 0, field_code);
    return(0);
  }

  if ((strcmp(field_code,"FILEFRAM")==0) ||
      (strcmp(field_code,"INDEX")==0)) {
    return(1);
  }

  /***************************************/
  /** Check to see if it is a raw entry **/
  /* binary search for the field */
  /* make a RawEntry we can compare to */
  strncpy(tR.field, field_code, FIELD_LENGTH);
  /** use the stdlib binary search */
  R = bsearch(&tR, F->rawEntries, F->n_raw,
      sizeof(struct RawEntryType), RawCmp);
  if (R!=NULL) {
    spf = R->samples_per_frame;
    return(spf);
  }

  /***************************************/
  /** Check to see if it is a lincom entry **/
  /* binary search for the field */
  /* make a RawEntry we can compare to */
  strncpy(tL.field, field_code, FIELD_LENGTH);
  /** use the stdlib binary search */
  L = bsearch(&tL, F->lincomEntries, F->n_lincom,
      sizeof(struct LincomEntryType), LincomCmp);
  if (L!=NULL) {
    recurse_level++;
    spf = GetSPF(L->in_fields[0], F, error_code);
    recurse_level--;
    return(spf);
  }

  /***************************************/
  /** Check to see if it is a multiply entry **/
  /* binary search for the field */
  /* make a RawEntry we can compare to */
  strncpy(tM.field, field_code, FIELD_LENGTH);
  /** use the stdlib binary search */
  M = bsearch(&tM, F->multiplyEntries, F->n_multiply,
      sizeof(struct MultiplyEntryType), MultiplyCmp);
  if (M != NULL) {
    int spf2;
    recurse_level++;
    spf = GetSPF(M->in_fields[0], F, error_code);
    spf2 = GetSPF(M->in_fields[1], F, error_code);
    recurse_level--;
    if (spf2 > spf)
      spf = spf2;
    return(spf);
  }

  /***************************************/
  /** Check to see if it is a bit entry **/
  /* binary search for the field */
  /* make a BitEntry we can compare to */
  strncpy(tB.field, field_code, FIELD_LENGTH);
  /** use the stdlib binary search */
  B = bsearch(&tB, F->bitEntries, F->n_bit,
      sizeof(struct BitEntryType), BitCmp);
  if (B!=NULL) {
    recurse_level++;
    spf = GetSPF(B->raw_field, F, error_code);
    recurse_level--;
    return(spf);
  }

  /***************************************/
  /** Check to see if it is a phase entry **/
  /* binary search for the field */
  /* make a PhaseEntry we can compare to */
  strncpy(tP.field, field_code, FIELD_LENGTH);
  /** use the stdlib binary search */
  P = bsearch(&tP, F->phaseEntries, F->n_phase,
      sizeof(struct PhaseEntryType), PhaseCmp);
  if (P!=NULL) {
    recurse_level++;
    spf = GetSPF(P->raw_field, F, error_code);
    recurse_level--;
    return(spf);
  }

  /***************************************/
  /** Check to see if it is a linterp entry **/
  /* binary search for the field */
  /* make a LinterpEntry we can compare to */
  strncpy(tI.field, field_code, FIELD_LENGTH);
  /** use the stdlib binary search */
  I = bsearch(&tI, F->linterpEntries, F->n_linterp,
      sizeof(struct LinterpEntryType), LinterpCmp);
  if (I!=NULL) {
    recurse_level++;
    spf = GetSPF(I->raw_field, F, error_code);
    recurse_level--;
    return(spf);
  }

  *error_code = SetGetDataError(GD_E_BAD_CODE, 0, NULL, 0, field_code);
  return(0);
}

/***************************************************************************/
/*                                                                         */
/*   Look to see if the field code belongs to a raw.  If so, parse it.     */
/*                                                                         */
/***************************************************************************/
static int DoIfRaw(struct FormatType *F, const char *field_code,
    int first_frame, int first_samp,
    int num_frames, int num_samp,
    char return_type, void *data_out,
    int *error_code, int *n_read) {

  struct RawEntryType tR;
  struct RawEntryType *R;
  int s0, ns, bytes_read;
  char datafilename[2 * MAX_FILENAME_LENGTH + FIELD_LENGTH + 2];
  unsigned char *databuffer;

  /******* binary search for the field *******/
  /* make a RawEntry we can compare to */
  strncpy(tR.field, field_code, FIELD_LENGTH);
  /** use the stdlib binary search */
  R = bsearch(&tR, F->rawEntries, F->n_raw,
      sizeof(struct RawEntryType), RawCmp);
  if (R==NULL) return(0);

  /** if we got here, we found the field! **/
  s0 = first_samp + first_frame*R->samples_per_frame;
  ns = num_samp + num_frames*R->samples_per_frame;

  /** open the file (and cache the fp) if it hasn't been opened yet. */
  if (R->fp <0) {
    snprintf(datafilename, 2 * MAX_FILENAME_LENGTH + FIELD_LENGTH + 2, 
        "%s/%s", F->FileDirName, R->file);
    R->fp = open(datafilename, O_RDONLY);
    if (R->fp<0) {
      *n_read = 0;
      *error_code = SetGetDataError(GD_E_OPEN_RAWFIELD, 0, NULL, 0,
          datafilename);
      return(1);
    }
  }

  databuffer = (unsigned char *)malloc(ns*R->size);

  *n_read = 0;
  if (s0 < 0) {
    *n_read = FillZero(databuffer, R->type, s0, ns);
    ns -= *n_read;
    s0 = 0;
  }

  if (ns>0) {
    lseek(R->fp, s0*R->size, SEEK_SET);
    bytes_read = read(R->fp, databuffer + *n_read*R->size, ns*R->size);
    *n_read += bytes_read/R->size;
  }

  *error_code =
    ConvertType(databuffer, R->type, data_out, return_type, *n_read);

  free(databuffer);

  return(1);
}


/***************************************************************************/
/*                                                                         */
/*            AllocTmpbuff: allocate a buffer of the right type and size   */
/*                                                                         */
/***************************************************************************/
static void *AllocTmpbuff(char type, int n) {
  assert(n > 0);
  void *buff=NULL;
  switch(type) {
    case 'n':
      buff = NULL;
      break;
    case 'c':
      buff = malloc(n*sizeof(char));
      break;
    case 'i':
    case 'S':
    case 'U':
      buff = malloc(n*sizeof(int));
      break;
    case 's':
    case 'u':
      buff = malloc(n*sizeof(short));
      break;
    case 'f':
      buff = malloc(n*sizeof(float));
      break;
    case 'd':
      buff = malloc(n*sizeof(double));
      break;
    default:
      printf("Unexpected bad type error in AllocTmpbuff (%c)\n",type);
      abort();
      break;
  }
  if ((type != 'n') && (buff==NULL)) {
    printf("Memory Allocation error in AllocTmpbuff\n");
  }
  return(buff);
}

/***************************************************************************/
/*                                                                         */
/*   ScaleData: out = m*in+b                                               */
/*                                                                         */
/***************************************************************************/
static void ScaleData(void *data, char type, int npts, double m, double b) {
  unsigned char *data_c;
  short *data_s;
  unsigned short *data_u;
  unsigned *data_U;
  int *data_i;
  float *data_f;
  double *data_d;

  int i;

  switch(type) {
    case 'n':
      break;
    case 'c':
      data_c = (unsigned char *)data;
      for (i=0; i<npts; i++) {
        data_c[i] =(unsigned char)((double)data_c[i] * m + b);
      }
      break;
    case 's':
      data_s = (short *)data;
      for (i=0; i<npts; i++) {
        data_s[i] =  (short)((double)data_s[i] * m + b);
      }
      break;
    case 'u':
      data_u = (unsigned short *)data;
      for (i=0; i<npts; i++) {
        data_u[i] = (unsigned short)((double)data_u[i] * m + b);
      }
      break;
    case 'S': case 'i':
      data_i = (int *)data;
      for (i=0; i<npts; i++) {
        data_i[i] = (int)((double)data_i[i] * m + b);
      }
      break;
    case 'U':
      data_U = (unsigned*)data;
      for (i=0; i<npts; i++) {
        data_U[i] = (unsigned)((double)data_U[i] * m + b);
      }
      break;
    case 'f':
      data_f = (float *)data;
      for (i=0; i<npts; i++) {
        data_f[i] = (float)((double)data_f[i] * m + b);
      }
      break;
    case 'd':
      data_d = (double *)data;
      for (i=0; i<npts; i++) {
        data_d[i] = data_d[i] * m + b;
      }
      break;
    default:
      printf("Another impossible error\n");
      abort();
      break;
  }
}

/***************************************************************************/
/*                                                                         */
/*            AddData: add B to A.  B is unchanged                         */
/*                                                                         */
/***************************************************************************/
static void AddData(void *A, int spfA, void *B, int spfB, char type, int n) {
  int i;

  switch(type) {
    case 'n': /* null read */
      break;
    case 'c':
      for (i=0; i<n; i++) {
        ((unsigned char*)A)[i] += ((unsigned char*)B)[i * spfB / spfA];
      }
      break;
    case 'S': case 'i':
      for (i=0; i<n; i++) {
        ((int*)A)[i] += ((int*)B)[i * spfB / spfA];
      }
      break;
    case 's':
      for (i=0; i<n; i++) {
        ((short*)A)[i] += ((short*)B)[i * spfB / spfA];
      }
      break;
    case 'u':
      for (i=0; i<n; i++) {
        ((unsigned short*)A)[i] += ((unsigned short*)B)[i * spfB / spfA];
      }
      break;
    case 'U':
      for (i=0; i<n; i++) {
        ((unsigned*)A)[i] += ((unsigned*)B)[i * spfB / spfA];
      }
      break;
    case 'f':
      for (i=0; i<n; i++) {
        ((float*)A)[i] += ((float*)B)[i * spfB / spfA];
      }
      break;
    case 'd':
      for (i=0; i<n; i++) {
        ((double*)A)[i] += ((double*)B)[i * spfB / spfA];
      }
      break;
    default:
      printf("Unexpected bad type error in AddData\n");
      abort();
      break;
  }
}

/***************************************************************************/
/*                                                                         */
/*            MultiplyData: multiply B by A.  B is unchanged               */
/*                                                                         */
/***************************************************************************/
static void MultiplyData(void *A, int spfA, void *B, int spfB, char type, int n)
{
  int i;

  switch(type) {
    case 'n': /* null read */
      break;
    case 'c':
      for (i=0; i<n; i++) {
        ((unsigned char*)A)[i] *= ((unsigned char*)B)[i * spfB / spfA];
      }
      break;
    case 'S': case 'i':
      for (i=0; i<n; i++) {
        ((int*)A)[i] *= ((int*)B)[i * spfB / spfA];
      }
      break;
    case 's':
      for (i=0; i<n; i++) {
        ((short*)A)[i] *= ((short*)B)[i * spfB / spfA];
      }
      break;
    case 'u':
      for (i=0; i<n; i++) {
        ((unsigned short*)A)[i] *= ((unsigned short*)B)[i * spfB / spfA];
      }
      break;
    case 'U':
      for (i=0; i<n; i++) {
        ((unsigned*)A)[i] *= ((unsigned*)B)[i * spfB / spfA];
      }
      break;
    case 'f':
      for (i=0; i<n; i++) {
        ((float*)A)[i] *= ((float*)B)[i * spfB / spfA];
      }
      break;
    case 'd':
      for (i=0; i<n; i++) {
        ((double*)A)[i] *= ((double*)B)[i * spfB / spfA];
      }
      break;
    default:
      printf("Unexpected bad type error in MultiplyData\n");
      abort();
      break;
  }
}


/***************************************************************************/
/*                                                                         */
/*   Look to see if the field code belongs to a lincom.  If so, parse it.  */
/*                                                                         */
/***************************************************************************/
static int DoIfLincom(struct FormatType *F, const char *field_code,
    int first_frame, int first_samp,
    int num_frames, int num_samp,
    char return_type, void *data_out,
    int *error_code, int *n_read) {
  struct LincomEntryType tL;
  struct LincomEntryType *L;
  void *tmpbuf;
  int i;
  int spf1, spf2, num_samp2, first_samp2;
  int n_read2;

  /******* binary search for the field *******/
  /* make a LincomEntry we can compare to */
  strncpy(tL.field, field_code, FIELD_LENGTH);
  /** use the stdlib binary search */
  L = bsearch(&tL, F->lincomEntries, F->n_lincom,
      sizeof(struct LincomEntryType), LincomCmp);
  if (L==NULL) return(0);

  /*****************************************/
  /** if we got here, we found the field! **/
  /** read into dataout and scale the first element **/
  recurse_level++;
  spf1 = GetSPF(L->in_fields[0], F, error_code);
  if (*error_code != GD_E_OK) return(1);

  /* read and scale the first field and record the number of samples
   * returned */
  *n_read = DoField(F, L->in_fields[0],
      first_frame, first_samp,
      num_frames, num_samp,
      return_type, data_out,
      error_code);

  recurse_level--;

  if (*error_code != GD_E_OK)
    return(1);

  /* Nothing to lincomise */
  if (*n_read == 0)
    return 1;
  
  ScaleData(data_out, return_type, *n_read, L->m[0], L->b[0]);

  if (L->n_infields > 1) {
    for (i=1; i<L->n_infields; i++) {
      recurse_level++;

      /* find the samples per frame of the next field */
      spf2 = GetSPF(L->in_fields[i], F, error_code);
      if (*error_code != GD_E_OK) return(1);

      /* calculate the first sample and number of samples to read of the
       * next field */
      num_samp2 = (int)ceil((double)*n_read * spf2 / spf1);
      first_samp2 = (first_frame * spf2 + first_samp * spf2 / spf1);

      /* Allocate a temporary buffer for the next field */
      tmpbuf = AllocTmpbuff(return_type, num_samp2);
      if (!tmpbuf && return_type != 'n') {
        return(0);
      }

      /* read the next field */
      n_read2 = DoField(F, L->in_fields[i],
          0, first_samp2,
          0, num_samp2,
          return_type, tmpbuf,
          error_code);
      recurse_level--;
      if (*error_code != GD_E_OK) {
        free(tmpbuf);
        return(1);
      }

      ScaleData(tmpbuf, return_type, n_read2, L->m[i], L->b[i]);

      if (n_read2 > 0 && n_read2 * spf1 != *n_read * spf2) {
        *n_read = n_read2 * spf1 / spf2;
      }

      AddData(data_out, spf1, tmpbuf, spf2, return_type, *n_read);

      free(tmpbuf);
    }
  }

  return(1);
}

/***************************************************************************/
/*                                                                         */
/*  Look to see if the field code belongs to a multiply.  If so, parse it. */
/*                                                                         */
/***************************************************************************/
static int DoIfMultiply(struct FormatType *F, const char *field_code,
    int first_frame, int first_samp,
    int num_frames, int num_samp,
    char return_type, void *data_out,
    int *error_code, int *n_read) {
  struct MultiplyEntryType tM;
  struct MultiplyEntryType *M;
  void *tmpbuf;
  int spf1, spf2, num_samp2, first_samp2;
  int n_read2;

  /******* binary search for the field *******/
  /* make a LincomEntry we can compare to */
  strncpy(tM.field, field_code, FIELD_LENGTH);
  /** use the stdlib binary search */
  M = bsearch(&tM, F->multiplyEntries, F->n_multiply,
      sizeof(struct MultiplyEntryType), MultiplyCmp);
  if (M==NULL) return(0);

  /*****************************************/
  /** if we got here, we found the field! **/
  /** read into dataout and scale the first element **/
  recurse_level++;

  /* find the samples per frame of the first field */
  spf1 = GetSPF(M->in_fields[0], F, error_code);
  if (*error_code != GD_E_OK) return(1);

  /* read the first field and record the number of samples
   * returned */
  *n_read = DoField(F, M->in_fields[0],
      first_frame, first_samp,
      num_frames, num_samp,
      return_type, data_out,
      error_code);

  recurse_level--;

  if (*error_code != GD_E_OK)
    return 1;

  /* Nothing to multiply */
  if (*n_read == 0)
    return 1;

  recurse_level++;

  /* find the samples per frame of the second field */
  spf2 = GetSPF(M->in_fields[1], F, error_code);
  if (*error_code != GD_E_OK) return(1);

  /* calculate the first sample and number of samples to read of the
   * second field */
  num_samp2 = (int)ceil((double)*n_read * spf2 / spf1);
  first_samp2 = (first_frame * spf2 + first_samp * spf2 / spf1);

  /* Allocate a temporary buffer for the second field */
  tmpbuf = AllocTmpbuff(return_type, num_samp2);
  if (!tmpbuf && return_type != 'n') {
    return(0);
  }

  /* read the second field */
  n_read2 = DoField(F, M->in_fields[1],
      0, first_samp2,
      0, num_samp2,
      return_type, tmpbuf,
      error_code);
  recurse_level--;
  if (*error_code != GD_E_OK) {
    free(tmpbuf);
    return(1);
  }

  if (n_read2 > 0 && n_read2 * spf1 < *n_read * spf2) {
    *n_read = n_read2 * spf1 / spf2;
  }
  MultiplyData(data_out, spf1, tmpbuf, spf2, return_type, *n_read);
  free(tmpbuf);

  return(1);
}

/***************************************************************************/
/*                                                                         */
/*   Look to see if the field code belongs to a bitfield.  If so, parse it.*/
/*                                                                         */
/***************************************************************************/
static int DoIfBit(struct FormatType *F, const char *field_code,
    int first_frame, int first_samp,
    int num_frames, int num_samp,
    char return_type, void *data_out,
    int *error_code, int *n_read) {
  struct BitEntryType tB;
  struct BitEntryType *B;
  unsigned *tmpbuf;
  int i;
  int spf;
  int ns;
  unsigned mask;

  /******* binary search for the field *******/
  /* make a BitEntry we can compare to */
  strncpy(tB.field, field_code, FIELD_LENGTH);
  /** use the stdlib binary search */
  B = bsearch(&tB, F->bitEntries, F->n_bit,
      sizeof(struct BitEntryType), BitCmp);
  if (B==NULL) return(0);

  /*****************************************/
  /** if we got here, we found the field! **/
  recurse_level++;
  spf = GetSPF(B->raw_field, F, error_code);
  recurse_level--;
  if (*error_code!=GD_E_OK) {
    *n_read = 0;
    return(1);
  }

  ns = num_samp + num_frames*spf;
  tmpbuf = (unsigned *)malloc(ns*sizeof(unsigned));

  recurse_level++;
  *n_read = DoField(F, B->raw_field,
      first_frame, first_samp,
      num_frames, num_samp,
      'U', tmpbuf,
      error_code);
  recurse_level--;
  if (*error_code!=GD_E_OK) {
    free(tmpbuf);
    return(1);
  }

  if (B->numbits==32) mask = 0xffffffff;
  else mask = (unsigned)(pow(2,B->numbits)-0.9999);

  for (i=0; i<*n_read; i++) {
    tmpbuf[i] = (tmpbuf[i]>>B->bitnum) & mask;
  }

  *error_code = ConvertType((unsigned char *)tmpbuf, 'U',
      data_out, return_type, *n_read);
  free(tmpbuf);

  return(1);
}

/***************************************************************************/
/*                                                                         */
/*   Look to see if the field code belongs to phasefield.  If so, parse it.*/
/*                                                                         */
/***************************************************************************/
static int DoIfPhase(struct FormatType *F, const char *field_code,
    int first_frame, int first_samp,
    int num_frames, int num_samp,
    char return_type, void *data_out,
    int *error_code, int *n_read) {
  struct PhaseEntryType tP;
  struct PhaseEntryType *P;

  /******* binary search for the field *******/
  /* make a PhaseEntry we can compare to */
  strncpy(tP.field, field_code, FIELD_LENGTH);
  /** use the stdlib binary search */
  P = bsearch(&tP, F->phaseEntries, F->n_phase,
      sizeof(struct PhaseEntryType), PhaseCmp);
  if (P==NULL) return(0);

  /*****************************************/
  /** if we got here, we found the field! **/
  recurse_level++;
  *n_read = DoField(F, P->raw_field,
      first_frame, first_samp + P->shift,
      num_frames, num_samp,
      return_type, data_out,
      error_code);
  recurse_level--; 

  return(1);
}

/***************************************************************************/
/*                                                                         */
/*   ReadLinterpFile: Read in the linterp data for this field              */
/*                                                                         */
/***************************************************************************/
void MakeDummyLinterp(struct LinterpEntryType *E); /* prototype => no warning... */
void MakeDummyLinterp(struct LinterpEntryType *E) {
  E->n_interp = 2;
  E->x = (double *)malloc(2*sizeof(double));
  E->y = (double *)malloc(2*sizeof(double));
  E->x[0] = 0;
  E->y[0] = 0;
  E->x[1] = 1;
  E->y[1] = 1;
}

static int ReadLinterpFile(struct LinterpEntryType *E) {
  FILE *fp;
  int i;
  char line[255];
  int linenum = 0;

  fp = fopen(E->linterp_file, "r");
  if (fp==NULL) {
    MakeDummyLinterp(E);
    return SetGetDataError(GD_E_OPEN_LINFILE, GD_E_LINFILE_SE_OPEN, NULL, 0,
        E->linterp_file);
  }

  /* first read the file to see how big it is */
  i=0;
  while (GetLine(fp, line, &linenum)) {
    i++;
  }
  if (i<2) {
    MakeDummyLinterp(E);
    return SetGetDataError(GD_E_OPEN_LINFILE, GD_E_LINFILE_SE_LENGTH, NULL, 0,
        E->linterp_file);
  }
  E->n_interp = i;
  E->x = (double *)malloc(i*sizeof(double));
  E->y = (double *)malloc(i*sizeof(double));
  /* now read in the data */
  rewind(fp);
  linenum = 0;
  for (i=0; i<E->n_interp; i++) {
    GetLine(fp, line, &linenum);
    sscanf(line, "%lg %lg",&(E->x[i]), &(E->y[i]));
  }
  return (GD_E_OK);
}

/***************************************************************************/
/*                                                                         */
/*   GetIndex: just linearly search - we are probably right to start with  */
/*                                                                         */
/***************************************************************************/
static int GetIndex(double x, double lx[], int idx, int n) {
  /* increment until we are bigger */
  while ((idx < n-2) && (x > lx[idx])) {
    idx++;
  }
  /* decrement until we are smaller */
  while ((idx > 0) && (x < lx[idx])) {
    idx--;
  }

  return(idx);
}

/***************************************************************************/
/*                                                                         */
/*   LinterpData: calibrate data using lookup table lx and ly              */
/*                                                                         */
/***************************************************************************/
static void LinterpData(const void *data, char type, int npts,
    double *lx, double *ly, int n_ln) {
  int i, idx=0;
  double x;

  for (i=0; i<npts; i++) {
    switch(type) {
      case 'n':
        return;
        break;
      case 'c':
        x = ((unsigned char *)data)[i];
        idx = GetIndex(x, lx, idx, n_ln);
        ((unsigned char *)data)[i] =
          (unsigned char)(ly[idx] + (ly[idx+1]-ly[idx])/
                          (lx[idx+1]-lx[idx]) * (x-lx[idx]));
        break;
      case 's':
        x = ((short *)data)[i];
        idx = GetIndex(x, lx, idx, n_ln);
        ((short *)data)[i] = (short)(ly[idx] + (ly[idx+1]-ly[idx])/
                                     (lx[idx+1]-lx[idx]) * (x-lx[idx]));
        break;
      case 'u':
        x = ((unsigned short *)data)[i];
        idx = GetIndex(x, lx, idx,n_ln);
        ((unsigned short *)data)[i] =
          (unsigned short)(ly[idx] + (ly[idx+1]-ly[idx])/
                           (lx[idx+1]-lx[idx]) * (x-lx[idx]));
        break;
      case 'S': case 'i':
        x = ((int *)data)[i];
        idx = GetIndex(x, lx, idx, n_ln);
        ((int *)data)[i] = (int)(ly[idx] + (ly[idx+1]-ly[idx])/
                                 (lx[idx+1]-lx[idx]) * (x-lx[idx]));
        break;
      case 'U':
        x = ((unsigned *)data)[i];
        idx = GetIndex(x, lx, idx, n_ln);
        ((unsigned *)data)[i] =
          (unsigned)(ly[idx] + (ly[idx+1]-ly[idx])/
                     (lx[idx+1]-lx[idx]) * (x-lx[idx]));
        break;
      case 'f':
        x = ((float *)data)[i];
        idx = GetIndex(x, lx, idx, n_ln);
        ((float *)data)[i] = (float)(ly[idx] + (ly[idx+1]-ly[idx])/
                                     (lx[idx+1]-lx[idx]) * (x-lx[idx]));
        break;
      case 'd':
        x = ((double *)data)[i];
        idx = GetIndex(x, lx, idx, n_ln);
        ((double *)data)[i] = (double)(ly[idx] + (ly[idx+1]-ly[idx])/
                                       (lx[idx+1]-lx[idx]) * (x-lx[idx]));
        break;
      default:
        printf("Another impossible error\n");
        abort();
        break;
    }
  }
}

/***************************************************************************/
/*                                                                         */
/*   Look to see if the field code belongs to a bitfield.  If so, parse it.*/
/*                                                                         */
/***************************************************************************/
static int DoIfLinterp(struct FormatType *F, const char *field_code,
    int first_frame, int first_samp,
    int num_frames, int num_samp,
    char return_type, void *data_out,
    int *error_code, int *n_read) {
  struct LinterpEntryType tI;
  struct LinterpEntryType *I;

  /******* binary search for the field *******/
  /* make a LinterpEntry we can compare to */
  strncpy(tI.field, field_code, FIELD_LENGTH);
  /** use the stdlib binary search */
  I = bsearch(&tI, F->linterpEntries, F->n_linterp,
      sizeof(struct LinterpEntryType), LinterpCmp);
  if (I==NULL) return(0);

  /*****************************************/
  /** if we got here, we found the field! **/
  if (I->n_interp<0) {
    *error_code = ReadLinterpFile(I);
    if (*error_code != GD_E_OK) {
      *n_read = 0;
      return(1);
    }
  }
  recurse_level++;
  *n_read = DoField(F, I->raw_field,
      first_frame, first_samp,
      num_frames, num_samp,
      return_type, data_out,
      error_code);
  recurse_level--;
  if (*error_code!=GD_E_OK) return(1);
  LinterpData(data_out, return_type, *n_read, I->x, I->y, I->n_interp);
  return(1);

}

/***************************************************************************/
/*                                                                         */
/*  DoField: Doing one field once F has been identified                    */
/*                                                                         */
/***************************************************************************/
static int DoField(struct FormatType *F, const char *field_code,
    int first_frame, int first_samp,
    int num_frames, int num_samp,
    char return_type, void *data_out,
    int *error_code) {
  int n_read = 0;

  if (recurse_level>10) {
    *error_code = SetGetDataError(GD_E_RECURSE_LEVEL, 0, NULL, 0, field_code);
    return(0);
  }


  /********************************************/
  /* if Asking for "FILEFRAM" or "INDEX", just return it */
  if ((strcmp(field_code,"FILEFRAM")==0) ||
      (strcmp(field_code,"INDEX")==0)) {
    n_read = num_frames + num_samp;
    if (data_out!=NULL) {
      FillFileFrame(data_out, return_type, first_frame+first_samp+
          F->frame_offset, n_read);
    }
    *error_code = SetGetDataError(GD_E_OK, 0, NULL, 0, NULL);
    return(n_read);
  }

  if (DoIfRaw(F, field_code,
        first_frame, first_samp,
        num_frames, num_samp,
        return_type, data_out,
        error_code, &n_read)) {
    return(n_read);
  } else if (DoIfLincom(F, field_code,
        first_frame, first_samp,
        num_frames, num_samp,
        return_type, data_out,
        error_code, &n_read)) {
    return(n_read);
  } else if (DoIfBit(F, field_code,
        first_frame, first_samp,
        num_frames, num_samp,
        return_type, data_out,
        error_code, &n_read)) {
    return(n_read);
  } else if (DoIfPhase(F, field_code,
        first_frame, first_samp,
        num_frames, num_samp,
        return_type, data_out,
        error_code, &n_read)) {
    return(n_read);
  } else if (DoIfLinterp(F, field_code,
        first_frame, first_samp,
        num_frames, num_samp,
        return_type, data_out,
        error_code, &n_read)) {
    return(n_read);
  } else if (DoIfMultiply(F, field_code,
        first_frame, first_samp,
        num_frames, num_samp,
        return_type, data_out,
        error_code, &n_read)) {
    return(n_read);
  } else {
    *error_code = SetGetDataError(GD_E_BAD_CODE, 0, NULL, 0, field_code);
    return(0);
  }
}

/***************************************************************************/
/*                                                                         */
/*  GetData: read BLAST format files.                                      */
/*    filename_in: the name of the file directory (raw files are in here)  */
/*    field_code: the name of the field you want to read                   */
/*    first_frame, first_samp: the first sample read is                    */
/*              first_samp + samples_per_frame*first_frame                 */
/*    num_frames, num_samps: the number of samples read is                 */
/*              num_samps + samples_per_frame*num_frames                   */
/*    return_type: data type of *data_out.  's': 16 bit signed             */
/*              'u' 16bit unsigned 'S' 32bit signed 'U' 32bit unsigned     */
/*              'c' 8 bit unsigned                                         */
/*    void *data_out: array to put the data                                */
/*    *error_code: error code is returned here.                            */
/*                                                                         */
/*    return value: returns number of samples actually read into data_out  */
/*                                                                         */
/***************************************************************************/
int GetData(const char *filename_in, const char *field_code,
    int first_frame, int first_samp,
    int num_frames, int num_samp,
    char return_type, void *data_out,
    int *error_code) {

  struct FormatType *F;
  int n_read=0;
  char filename[MAX_FILENAME_LENGTH+1];

  *error_code = SetGetDataError(GD_E_OK, 0, NULL, 0, NULL);

  if (first_time) {
    Formats.n = 0;
    Formats.F = NULL;
    first_time = 0;
  }

  strncpy(filename, filename_in, MAX_FILENAME_LENGTH);
  if (filename[strlen(filename)-1]=='/') filename[strlen(filename)-1]='\0';
  F = GetFormat(filename, error_code);
  if (!F || *error_code != GD_E_OK) {
    return(0);
  }
  first_frame -= F->frame_offset;

  n_read = DoField(F, field_code,
      first_frame, first_samp,
      num_frames, num_samp,
      return_type, data_out,
      error_code);

  return(n_read);
}

/***************************************************************************/
/*                                                                         */
/*    Get the number of frames available                                   */
/*                                                                         */
/***************************************************************************/
int GetNFrames(const char *filename_in, int *error_code, const char *in_field) {
  struct FormatType *F;
  char filename[MAX_FILENAME_LENGTH+1];
  char raw_data_filename[2 * MAX_FILENAME_LENGTH + FIELD_LENGTH + 2];
  struct stat statbuf;
  int nf;

  *error_code = SetGetDataError(GD_E_OK, 0, NULL, 0, NULL);

  if (first_time) {
    Formats.n = 0;
    Formats.F = NULL;
    first_time = 0;
  }

  strncpy(filename, filename_in, MAX_FILENAME_LENGTH);
  if (filename[strlen(filename)-1]=='/') filename[strlen(filename)-1]='\0';
  F = GetFormat(filename, error_code);
  if (*error_code != GD_E_OK) {
    return(0);
  }

  if (!F || F->n_raw==0) {
    *error_code = SetGetDataError(GD_E_FORMAT, GD_E_FORMAT_SE_N_RAW, NULL, 0,
        NULL);
    return(0);
  }

  /* load the first valid raw field */
  snprintf(raw_data_filename, 2 * MAX_FILENAME_LENGTH + FIELD_LENGTH + 2, 
      "%s/%s", filename, F->first_field.file);
  if (stat(raw_data_filename, &statbuf) < 0) {
    return(0);
  }

  nf = statbuf.st_size/
    (F->first_field.size*F->first_field.samples_per_frame);
  nf += F->frame_offset;

  nf -= 2;
  if (nf < 0)
    nf = 0;
  return(nf);
}

/***************************************************************************/
/*                                                                         */
/*    Get the number of samples for each frame for the given field         */
/*                                                                         */
/***************************************************************************/
int GetSamplesPerFrame(const char *filename_in, const char *field_name, int *error_code) {
  struct FormatType *F;
  char filename[MAX_FILENAME_LENGTH+1];

  *error_code = SetGetDataError(GD_E_OK, 0, NULL, 0, NULL);

  if (first_time) {
    Formats.n = 0;
    Formats.F = NULL;
    first_time = 0;
  }

  strncpy(filename, filename_in, MAX_FILENAME_LENGTH);
  if (filename[strlen(filename)-1]=='/') filename[strlen(filename)-1]='\0';
  F = GetFormat(filename, error_code);
  if (*error_code!=GD_E_OK) {
    return(0);
  }

  if (!F || F->n_raw==0) {
    *error_code = SetGetDataError(GD_E_FORMAT, GD_E_FORMAT_SE_N_RAW, NULL, 0,
        NULL);
    return(0);
  }

  return GetSPF(field_name, F, error_code);
}

/***************************************************************************/
/*                                                                         */
/* Look to see if the output field code belongs to a raw.  If so, parse it.*/
/*                                                                         */
/***************************************************************************/

static int DoIfRawOut(struct FormatType *F, const char *field_code,
    int first_frame, int first_samp,
    int num_frames, int num_samp, 
    char data_type, const void *data_in,
    int *error_code, int *n_write) {

  struct RawEntryType tR;
  struct RawEntryType *R;
  int s0, ns;
  char datafilename[MAX_FILENAME_LENGTH+FIELD_LENGTH + 1];
  void *databuffer;
  struct stat statbuf;

  strncpy(tR.field, field_code, FIELD_LENGTH); 
  R = bsearch(&tR, F->rawEntries, F->n_raw,
      sizeof(struct RawEntryType), RawCmp);
  if (DIRFILE_DEBUG) {
    fprintf(stdout,"DoIfRawOut:  searched for field %s\n",field_code);
  }
  if (R==NULL){
    if (DIRFILE_DEBUG) {
      fprintf(stdout,"DoIfRawOut:  Cannot find field in Format struct\n");
    }
    return(0);
  }
  s0 = first_samp + first_frame * (int)(R->samples_per_frame);
  ns = num_samp + num_frames * (int)(R->samples_per_frame);

  if (DIRFILE_DEBUG) {
    fprintf(stdout,"DoIfRawOut:  file pointer for field %s = %d\n",field_code,R->fp);
  } 
  if (R->fp < 0) {
    /* open file for reading / writing if not already opened */

    sprintf(datafilename, "%s/%s", F->FileDirName, field_code);
    if (DIRFILE_DEBUG) {
      fprintf(stdout,"DoIfRawOut:  stat(%s) = %d\n",datafilename,stat(datafilename,&statbuf));
    } 
    if(stat(datafilename, &statbuf) == 0){
      R->fp = open(datafilename, O_RDWR);
      if (R->fp < 0) {
        *n_write = 0;
        *error_code = PD_E_OPEN_RAWFIELD;
        return(1);
      }
    }else{
      R->fp = open(datafilename, O_RDWR | O_CREAT);
      if (R->fp < 0) {
        *n_write = 0;
        *error_code = PD_E_OPEN_RAWFIELD;
        return(1);
      }
    }
    if (DIRFILE_DEBUG) {
      fprintf(stdout,"DoIfRawOut:  opening file %s for writing\n",datafilename);
    }

  } else {
    /* make sure that file is in read / write mode        */
    /* if not, close file and reopen in read / write mode */

    if (DIRFILE_DEBUG) {
      fprintf(stdout,"DoIfRawOut:  file is already open\n");
    }
    sprintf(datafilename, "%s/%s", F->FileDirName, field_code);
    if (close(R->fp) < 0) {
      *n_write = 0;
      *error_code = PD_E_OPEN_RAWFIELD;
      return(1);
    } else {
      R->fp = open(datafilename, O_RDWR);
    }
    if (R->fp < 0) {
      *n_write = 0;
      *error_code = PD_E_OPEN_RAWFIELD;
      return(1);
    }
  }

  databuffer = malloc((size_t)(ns * (int)R->size));

  *error_code = ConvertType(data_in,data_type,databuffer,R->type,ns);

  /* write data to file.  Note that if the first sample is beyond     */
  /* the current end of file, the gap will be filled with zero bytes. */

  lseek(R->fp, s0 * (int)(R->size), SEEK_SET);
  *n_write = ((int)write(R->fp, databuffer, (size_t)(R->size) * (size_t)ns))/(R->size);

  if (DIRFILE_DEBUG) {
    fprintf(stdout,"DoIfRawOut:  %d samples\n",(int)*n_write);
  }

  free(databuffer);

  return(1);
}

/***************************************************************************/
/*                                                                         */
/*   Look to see if output field belongs to a linterp.  If so, parse it.   */
/*                                                                         */
/***************************************************************************/
static int DoIfLinterpOut(struct FormatType *F, const char *field_code,
    int first_frame, int first_samp,
    int num_frames, int num_samp, 
    char data_type, void *data_in,
    int *error_code, int *n_write) {
  struct LinterpEntryType tI;
  struct LinterpEntryType *I;
  int spf;
  int ns;

  /******* binary search for the field *******/
  /* make a LinterpEntry we can compare to */
  if (DIRFILE_DEBUG) {
    fprintf(stdout,"DoIfLinterp:  field = %s\n",field_code);
  }
  strncpy(tI.field, field_code, FIELD_LENGTH); 
  /** use the stdlib binary search */
  I = bsearch(&tI, F->linterpEntries, F->n_linterp,
      sizeof(struct LinterpEntryType), LinterpCmp);
  if (I==NULL) return(0);
  if (DIRFILE_DEBUG) {
    fprintf(stdout,"DoIfLinterp:  I->n_interp = %u\n",I->n_interp);
  }

  /*****************************************/
  /** if we got here, we found the field! **/
  if ((I->n_interp)<0) {
    *error_code = ReadLinterpFile(I);
    if (*error_code != GD_E_OK) return(1);
  }

  /* Interpolate X(y) instead of Y(x) */

  recurse_level++;
  spf = GetSPF(I->raw_field, F, error_code);
  recurse_level--;
  ns = num_samp + num_frames * (int)spf;

  LinterpData(data_in, data_type, ns, I->y, I->x, I->n_interp);

  recurse_level++;
  *n_write = DoFieldOut(F, I->raw_field,
      first_frame, first_samp,
      num_frames, num_samp,
      data_type, data_in,
      error_code);
  recurse_level--;
  if (*error_code!=GD_E_OK) return(1);

  return(1);

}

/***************************************************************************/
/*                                                                         */
/*   Look to see if output field belongs to a lincom.  If so, parse it.    */
/*                                                                         */
/***************************************************************************/
static int DoIfLincomOut(struct FormatType *F, const char *field_code,
    int first_frame, int first_samp,
    int num_frames, int num_samp, 
    char data_type, void *data_in,
    int *error_code, int *n_write) {
  struct LincomEntryType tL;
  struct LincomEntryType *L;
  int spf;
  int ns;

  /******* binary search for the field *******/
  /* make a LincomEntry we can compare to */
  strncpy(tL.field, field_code, FIELD_LENGTH); 
  /** use the stdlib binary search */
  L = bsearch(&tL, F->lincomEntries, F->n_lincom,
      sizeof(struct LincomEntryType), LincomCmp);
  if (L==NULL) return(0);

  /*****************************************/
  /** if we got here, we found the field! **/
  /** read into dataout and scale the first element **/

  /* we cannot write to LINCOM fields that are a linear combination */
  /* of more than one raw field (no way to know how to split data). */

  if((L->n_infields) > 1){
    *error_code = PD_E_MULT_LINCOM;
    return(1);
  }

  recurse_level++;

  /* do the inverse scaling */

  recurse_level++;
  spf = GetSPF(L->in_fields[0], F, error_code);
  recurse_level--;
  ns = num_samp + num_frames * (int)spf;

  ScaleData(data_in, data_type, ns, (1/(L->m[0])), (-(L->b[0])/(L->m[0])));
  *n_write = DoFieldOut(F, L->in_fields[0],
      first_frame, first_samp,
      num_frames, num_samp,
      data_type, data_in,
      error_code);

  recurse_level--;
  if (*error_code != GD_E_OK) return(1);

  return(1);
}

/***************************************************************************/
/*                                                                         */
/*   Look to see if output field belongs to a bitfield.  If so, parse it.  */
/*                                                                         */
/***************************************************************************/
static int DoIfBitOut(struct FormatType *F, const char *field_code,
    int first_frame, int first_samp,
    int num_frames, int num_samp, 
    char data_type, const void *data_in,
    int *error_code, int *n_write) {

  struct BitEntryType tB;
  struct BitEntryType *B;
  unsigned int *tmpbuf;
  unsigned int *readbuf;
  int i;
  int spf;
  int ns;
  int n_read;
  unsigned highmask;
  unsigned lowmask;

  /******* binary search for the field *******/
  /* make a BitEntry we can compare to */
  strncpy(tB.field, field_code, FIELD_LENGTH); 
  /** use the stdlib binary search */
  B = bsearch(&tB, F->bitEntries, F->n_bit,
      sizeof(struct BitEntryType), BitCmp);
  if (B==NULL) return(0);

  /*****************************************/
  /** if we got here, we found the field! **/

  recurse_level++;
  spf = GetSPF(B->raw_field, F, error_code);
  recurse_level--;
  if (*error_code!=GD_E_OK) return(1);

  ns = num_samp + num_frames * (int)spf;

  /* use calloc instead of malloc so that the memory is zeroed */
  tmpbuf = (unsigned int*)calloc((size_t)ns,sizeof(unsigned int));
  readbuf = (unsigned int*)calloc((size_t)ns,sizeof(unsigned int));

  *error_code = ConvertType(data_in, data_type, (void*)tmpbuf, 'U', ns);

  /* first, READ the field in so that we can change the bits    */
  /* do not check error code, since the field may not exist yet */

  if (DIRFILE_DEBUG) {
    fprintf(stdout,"DoIfBitOut:  reading in bitfield %s\n",B->raw_field);
  }
  recurse_level++;
  n_read = DoField(F, B->raw_field,
      first_frame, first_samp,
      num_frames, num_samp,
      'U', readbuf,
      error_code);
  recurse_level--;
  *error_code = 0;

  /* now go through and set the correct bit in each field value */

  highmask = 1 << (B->bitnum);
  lowmask = ~highmask;
  if (DIRFILE_DEBUG) {
    fprintf(stdout,"DoBitOut:  bitnum = %d highmask = %u lowmask = %u\n",B->bitnum,highmask,lowmask);
  }
  for(i=0; i<ns; i++){
    if(tmpbuf[i]){ /*set the bit to 1*/
      readbuf[i] = readbuf[i] | highmask;
    }else{ /*set the bit to 0*/
      readbuf[i] = readbuf[i] & lowmask;
    }
  }

  /* write the modified data out */

  *n_write = DoFieldOut(F, B->raw_field,
      first_frame, first_samp,
      num_frames, num_samp,
      'U', (void*)readbuf,
      error_code);

  free(readbuf);
  free(tmpbuf);
  return(1);
}

/***************************************************************************/
/*                                                                         */
/*  DoFieldOut: Do one output field once F has been identified             */
/*                                                                         */
/***************************************************************************/

static int DoFieldOut(struct FormatType *F, const char *field_code,
    int first_frame, int first_samp,
    int num_frames, int num_samp, 
    char data_type, void *data_in,
    int *error_code) {
  int n_write;

  if (recurse_level>10) {
    *error_code = GD_E_RECURSE_LEVEL;
    return(0);
  }

  if (DoIfRawOut(F, field_code,
        first_frame, first_samp,
        num_frames, num_samp,
        data_type, data_in,
        error_code, &n_write)) {
    return(n_write);
  } else if (DoIfLincomOut(F, field_code,
        first_frame, first_samp,
        num_frames, num_samp,
        data_type, data_in,
        error_code, &n_write)) {
    return(n_write);
  } else if (DoIfBitOut(F, field_code,
        first_frame, first_samp,
        num_frames, num_samp,
        data_type, data_in,
        error_code, &n_write)) {
    return(n_write);
  } else if (DoIfLinterpOut(F, field_code,
        first_frame, first_samp,
        num_frames, num_samp,
        data_type, data_in,
        error_code, &n_write)) {
    return(n_write);
  } else {
    *error_code = PD_E_BAD_CODE;
    return(0);
  }
}

/***************************************************************************/
/*                                                                         */
/*  PutData: write BLAST format RAW files.                                 */
/*    filename_in: the name of the file directory (raw files are in here)  */
/*    field_code: the name of the field you want to write                  */
/*    first_frame, first_samp: the first sample written is                 */
/*              first_samp + samples_per_frame*first_frame                 */
/*    num_frames, num_samps: the number of samples written is              */
/*              num_samps + samples_per_frame*num_frames                   */
/*    data_type: data type of *data_in.  's': 16 bit signed                */
/*              'u' 16bit unsigned 'S' 32bit signed 'U' 32bit unsigned     */
/*              'c' 8 bit unsigned 'f' 32bit float 'd' 64bit double        */
/*    void *data_in: array containing the data                             */
/*    *error_code: error code is returned here. If error_code==null,       */
/*               PutData prints the error message and exits                */
/*                                                                         */
/*    return value: returns # of samples actually written to file          */
/*                                                                         */
/***************************************************************************/

int PutData(const char *filename_in, const char *field_code,
    int first_frame, int first_samp,
    int num_frames, int num_samp,
    char data_type, void *data_in,
    int *error_code) {

  struct FormatType *F;
  int n_write=0;
  char filename[MAX_FILENAME_LENGTH+1];

  *error_code = GD_E_OK;

  if (first_time) {
    Formats.n = 0;
    Formats.F = NULL;
    first_time = 0;
  }

  strncpy(filename, filename_in, MAX_FILENAME_LENGTH);
  if (filename[strlen(filename)-1]=='/') filename[strlen(filename)-1]='\0';
  F = GetFormat(filename, error_code);
  if (*error_code!=GD_E_OK) {
    return(0);
  }

  n_write = DoFieldOut(F, field_code,
      first_frame, first_samp,
      num_frames, num_samp,
      data_type, data_in,
      error_code);

  return(n_write);
}
/* vim: ts=2 sw=2 et
*/
