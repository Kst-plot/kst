/*                           (C) 1996 C. Barth Netterfield */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* creaddata.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "readdata.h"
#include "creaddata.h"
#define MAX_LINE_LENGTH 120
#define MAX_FIELDS_IN_CFORMAT 500
#define MAX_LINCOM_ENTRIES 4

#ifndef CALSPECS_DIR
#define CALSPECS_DIR "/data/etc"
#endif

struct CalEntryType {
  int type;
  int n_lincom; /* number if fields if this is a lincom entry */
  char in_code[MAX_LINCOM_ENTRIES][8];
  double m[MAX_LINCOM_ENTRIES];
  double b[MAX_LINCOM_ENTRIES];
  char file[80]; /* filename if this is a linterp type */
  double *lx;    /* interpolation file ADU data */
  double *ly;    /* interpolation file eng data */
  double maxx;   /* max ADU */
  double minx;   /* min ADU */
  int n_linfile; /* number of linfile points */
  int cntval;    /* active multiplex value */
  int cntmax;
  int bitmask;   /* bit mask for SHR and SHL */
  float modulo;  /* old value used for differentiation */
  double framerate; /* the framerate for frametime entries */
};

struct CalSpecStruct {
  int format_spec;  /* the file format specifier */
  int n_fields;
  struct CalEntryType field[MAX_FIELDS_IN_CFORMAT];
  unsigned long long code[MAX_FIELDS_IN_CFORMAT];
} *cstruct;

static int n_cformats=0;

const char *const CRD_ERROR_CODES[15] = {"OK",
                             "Could not open FileFormat file",
                             "Error in FileFormat file",
                             "Could not open Data file",
                             "Unrecognized file type",
                             "Field code not found in File Format",
                             "Unrecognized return type",
                             " ", " ", " ",
                             "Could not open CalSpec file",
                             "Error on CalSpec file",
                             "Size mismatch in linear combination",
                             "Could not open interpolation file ",
                             "Too many levels of recursion"
                             
};

/***************************************************************************/
/*                                                                         */
/*       Subroutines for correcting binary byte order on Sparcs            */
/*                                                                         */
/***************************************************************************/
#ifdef __sparc
#define FLIP_BYTES
#endif

#if __BYTE_ORDER == __BIG_ENDIAN
#define FLIP_BYTES
#endif


#ifdef FLIP_BYTES
static void flip_bytes(char *d, int nwords) {
  int i,imax;
  char h;

  imax=nwords*2;
  
  for (i=0;i<imax; i+=2) {
    h=d[i];
    d[i]=d[i+1];
    d[i+1]=h;
  }
}
#else
static void flip_bytes(char *d, int nwords) {
  /* do nothing on alpha or linux machines */
  /* remove compiler warning of unused variables */
  if (d) {}
  if (nwords) {}
}
#endif

/***************************************************************************/
/*                                                                         */
/*    GetCSLine: read non-comment line from calibration spec file          */
/*        The line is placed   in *line.                                   */
/*        Returns 1 if successful, 0 if unsuccessful                         */
/*                                                                         */
/***************************************************************************/
static int GetCSLine(FILE *fp, char *line) {
  char *ret_val;
  int first_char;

  do {
    ret_val=fgets(line, MAX_LINE_LENGTH, fp);
    first_char=0;
    while ((line[first_char]==' ') || (line[first_char]=='\t')) first_char++;
    line += first_char;
  } while (((line[0] =='#') || (strlen(line)<2)) && (ret_val!=NULL));

  if (ret_val!=NULL) {
    return (1); /* a line was read */
  } else {
    return(0);  /* there were no valid lines */
  }
}

/***************************************************************************/
/*                                                                         */
/*     StringToCode: turn a <=8 byte string into a 64 bit int              */
/*                                                                         */
/***************************************************************************/
static long long StringToCode(const char *tmpcode) {
  long long code;
  char *cptr;
  int i;

  cptr=(char*) &code;
  code=0;
  
  for (i=0;i<(int)strlen(tmpcode); i++) {
    cptr[i]=tmpcode[i];
  }
  for (i=strlen(tmpcode); i<8; i++) {
    cptr[i]=0;
  }
  
  return code;
}

/***************************************************************************/
/*                                                                         */
/*    Read One cstruct                                                     */
/*                                                                         */
/***************************************************************************/
static int ReadOneCStruct(FILE *fp, int i_format) {
  char line[MAX_LINE_LENGTH];
  int i_field;
  int done=0;
  char in_cols[15][80];
  int n_cols;
  int n_lincom, i_lincom;
  
  /* read the begin line */
  GetCSLine(fp,line);
  if (strncmp(line,"BEGIN",5)!=0) return(E_CSFILE_FORMAT);
  /* read the format specifier */
  if (GetCSLine(fp,line)==0) return(E_CSFILE_FORMAT);
  sscanf(line,"%x",&(cstruct[i_format].format_spec));
  /* read the fields */
  cstruct[i_format].n_fields=0;
  done=0;
  i_field=0;
  while(!done) {
    /* read in the cols */
    if (GetCSLine(fp,line)==0) return(E_CSFILE_FORMAT);
    n_cols = sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
                    in_cols[0], in_cols[1], in_cols[2], in_cols[3], 
                    in_cols[4], in_cols[5], in_cols[6], in_cols[7], 
                    in_cols[8], in_cols[9], in_cols[10], in_cols[11], 
                    in_cols[12], in_cols[13], in_cols[14]);
    
    /* parse the various types */
    if (n_cols<3) return(E_CSFILE_FORMAT);
    if (strcmp(in_cols[0], "END")==0) {
      cstruct[i_format].code[i_field] = 0;
      cstruct[i_format].field[i_field].type = CST_END;
      cstruct[i_format].field[i_field].n_lincom = 1;
      cstruct[i_format].field[i_field].m[0] = atof(in_cols[1]);
      cstruct[i_format].field[i_field].b[0] = atof(in_cols[2]);
      done = 1;
      i_field++;
    } else if (strcmp(in_cols[1], "LINCOM")==0) {
      cstruct[i_format].code[i_field] = StringToCode(in_cols[0]);
      cstruct[i_format].field[i_field].type = CST_LINCOM;
      /* read in the number of fields to linearly combine */
      n_lincom = atoi(in_cols[2]);
      if ((n_lincom<1) || (n_lincom>MAX_LINCOM_ENTRIES))
        return(E_CSFILE_FORMAT);
      if (n_cols<3+n_lincom*3) return(E_CSFILE_FORMAT);
      
      cstruct[i_format].field[i_field].n_lincom = n_lincom;
      for (i_lincom=0; i_lincom<n_lincom; i_lincom++) {
        strcpy(cstruct[i_format].field[i_field].in_code[i_lincom],
               in_cols[i_lincom*3 + 3]);
        cstruct[i_format].field[i_field].m[i_lincom] =
          atof(in_cols[i_lincom*3 + 4]);
        cstruct[i_format].field[i_field].b[i_lincom] =
          atof(in_cols[i_lincom*3 + 5]);
      }
      i_field++;
    } else if (strcmp(in_cols[1], "LINTERP")==0) {
      cstruct[i_format].code[i_field] = StringToCode(in_cols[0]);
      strcpy(cstruct[i_format].field[i_field].in_code[0],in_cols[2]);
      cstruct[i_format].field[i_field].n_linfile = -1;
      strcpy(cstruct[i_format].field[i_field].file, in_cols[3]);
      cstruct[i_format].field[i_field].type = CST_LINFILE;
      i_field++;
    } else if (strcmp(in_cols[1], "FRAMETIME")==0) {
      cstruct[i_format].code[i_field] = StringToCode(in_cols[0]);
      strcpy(cstruct[i_format].field[i_field].in_code[0],in_cols[2]);
      cstruct[i_format].field[i_field].framerate = atof(in_cols[3]);
      cstruct[i_format].field[i_field].type = CST_FRAMETIME;
      i_field++;
    } else if (strcmp(in_cols[1], "MPLEX")==0) {
      /** Put CNTFIELD in incols[0] and DATAFIELD in incols[1] **/
      cstruct[i_format].code[i_field] = StringToCode(in_cols[0]);
      strcpy(cstruct[i_format].field[i_field].in_code[0],in_cols[2]);
      strcpy(cstruct[i_format].field[i_field].in_code[1],in_cols[3]);
      cstruct[i_format].field[i_field].cntval = atoi(in_cols[4]);
      cstruct[i_format].field[i_field].cntmax = atoi(in_cols[5]);
      cstruct[i_format].field[i_field].type = CST_MPLEX;
      i_field++;
    } else if (strcmp(in_cols[1], "BIT")==0) {
      cstruct[i_format].code[i_field] = StringToCode(in_cols[0]);
      strcpy(cstruct[i_format].field[i_field].in_code[0],in_cols[2]);
      cstruct[i_format].field[i_field].cntval = atoi(in_cols[3]);
      cstruct[i_format].field[i_field].type = CST_BITFIELD;
      i_field++;
    } else if (strcmp(in_cols[1], "SHR")==0) {
      cstruct[i_format].code[i_field] = StringToCode(in_cols[0]);
      strcpy(cstruct[i_format].field[i_field].in_code[0],in_cols[2]);
      cstruct[i_format].field[i_field].cntval = atoi(in_cols[3]);
      cstruct[i_format].field[i_field].bitmask = 
	strtol(in_cols[4], (char **)NULL, 0);
      cstruct[i_format].field[i_field].type = CST_SHR;
      i_field++;
    } else if (strcmp(in_cols[1], "SHL")==0) {
      cstruct[i_format].code[i_field] = StringToCode(in_cols[0]);
      strcpy(cstruct[i_format].field[i_field].in_code[0],in_cols[2]);
      cstruct[i_format].field[i_field].cntval = atoi(in_cols[3]);
      cstruct[i_format].field[i_field].bitmask =
	strtol(in_cols[4], (char **)NULL, 0);
      cstruct[i_format].field[i_field].type = CST_SHL;
      i_field++;
    } else if (strcmp(in_cols[1], "DIFF")==0) {
      cstruct[i_format].code[i_field] = StringToCode(in_cols[0]);
      strcpy(cstruct[i_format].field[i_field].in_code[0],in_cols[2]);
      cstruct[i_format].field[i_field].modulo = atof(in_cols[3]);
      cstruct[i_format].field[i_field].type = CST_DIFF;
      i_field++;
    } else if (strcmp(in_cols[1], "SIN")==0) {
      cstruct[i_format].code[i_field] = StringToCode(in_cols[0]);
      strcpy(cstruct[i_format].field[i_field].in_code[0],in_cols[2]);
      cstruct[i_format].field[i_field].type = CST_SIN;
      i_field++;
    } else {
      return (E_CSFILE_FORMAT);
    }
  }
  cstruct[i_format].n_fields=i_field;

  return(E_OK);
}

/***************************************************************************/
/*                                                                         */
/*    Read File Format file: place the results in a global structure       */
/*                                                                         */
/***************************************************************************/
static int ReadCalFile() {
  FILE *fp, *fp_i;
  char line[MAX_LINE_LENGTH];
  char calspecs_filename[MAX_LINE_LENGTH];
  char include_file_name[160];
  int i_format;
  int n_includes=0;

  sprintf(calspecs_filename, "%s/CalSpecs", CALSPECS_DIR);
  fp = fopen(calspecs_filename,"r");

  if (fp==NULL) {
    return(E_OPEN_CSFILE);  /* error opening FileFormats */
  }

  /* count the number of 'BEGIN' lines found */
  n_includes = n_cformats = 0;
  while (GetCSLine(fp, line)) {
    if (strncmp(line,"BEGIN",5)==0) n_cformats++;
    else if (strncmp(line,"INCLUDE",7)==0) {
      n_cformats++;
      n_includes++;
    }
  }
  if (n_cformats==0) {
    return(E_CSFILE_FORMAT);
  }

  /* allocate memory for cstruct */
  cstruct=(struct CalSpecStruct *)
    malloc(n_cformats * sizeof(struct CalSpecStruct));
  if (cstruct==NULL) {
    printf("CReadData error: could not allocate memory for cstruct\n");
    exit(0);
  }

  /* read in the format structures */
  rewind(fp);

    /* Read in the Includes */
  for (i_format=0; i_format<n_includes; i_format++) {
    GetCSLine(fp,line);
    
    if (strncmp(line,"INCLUDE",7)!=0) {
      return(E_CSFILE_FORMAT);
    }
    sscanf(line,"INCLUDE %s",include_file_name);
    
    fp_i = fopen(include_file_name,"r");
    if (fp_i==NULL) {
      return(E_OPEN_CSFILE);  /* error opening FileFormats */
    }
    
    ReadOneCStruct(fp_i, i_format);

    fclose (fp_i);
  }

  for (i_format=n_includes; i_format<n_cformats; i_format++) {
    ReadOneCStruct(fp, i_format);
  }
  return (E_OK);
}

/***************************************************************************/
/*                                                                         */
/*      Get File Format Index: read the first byte of the file to see what */
/*          format it is in                                                */
/*                                                                         */
/***************************************************************************/
static int GetFileFormatIndex(char *filename,int *i_format) {
  int fp;
  unsigned short tmp_in;
  int file_code;

  fp=open(filename,O_RDONLY);
  if (fp<0) return(E_FILE_OPEN);

  read(fp, &tmp_in, sizeof(unsigned short));
  flip_bytes((char*)&tmp_in,1);
  file_code=tmp_in;

  for (*i_format=0; (cstruct[*i_format].format_spec!=file_code) &&
         (*i_format<n_cformats); (*i_format)++);

  if (*i_format>= n_cformats) return (E_BAD_FILETYPE);
  
  close(fp);
  return(E_OK);
}


/***************************************************************************/
/*                                                                         */
/*  GetFieldIndex: convert field code into an index number                 */
/*    get the cstruct index for the field to be read                       */
/*    *i_field gets the index                                              */
/*    returns the index for END if the code isn't found first          */
/*                                                                         */
/***************************************************************************/
static int GetFieldIndex(const char *field_code,int i_format, int *i_field) {
  unsigned long long lfield_code;
  unsigned long long *codes;
  int i_f,n;
  
  i_f = *i_field;
  n = cstruct[i_format].n_fields-1;
  codes = cstruct[i_format].code;

  lfield_code=StringToCode(field_code);

  /*
    if (last_lfield_code == lfield_code) {
    i_f = last_i_f;
    } else {
    */
  for (i_f=0; (codes[i_f]!=lfield_code) &&
         (i_f<n); i_f++); 

  /*
    last_i_f = *i_field = i_f;
    last_lfield_code = lfield_code;
    
    }
    */
  *i_field = i_f;
  return(0);
}

/***************************************************************************/
/*                                                                         */
/*   GetIndex:                                                             */
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
static void LinterpData(void *data, char type, int npts, double *lx, double *ly,
                 int n_ln) {
  int i, idx=0;
  double x;

  for (i=0; i<npts; i++) {
    switch(type) {
    case 'n':
      return;
      break;
    case 'c':
      x = ((char *)data)[i];
      idx = GetIndex(x, lx, idx,n_ln);
      ((char *)data)[i] = (char)(ly[idx] + (ly[idx+1]-ly[idx])/
        (lx[idx+1]-lx[idx]) * (x-lx[idx]));
      break;
    case 's':
      x = ((short *)data)[i];
      idx = GetIndex(x, lx, idx,n_ln);
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
    case 'i':
      x = ((int *)data)[i];
      idx = GetIndex(x, lx, idx,n_ln);
      ((int *)data)[i] = (int)(ly[idx] + (ly[idx+1]-ly[idx])/
        (lx[idx+1]-lx[idx]) * (x-lx[idx]));
      break;
    case 'f':
      x = ((float *)data)[i];
      idx = GetIndex(x, lx, idx,n_ln);
      ((float *)data)[i] = (float)(ly[idx] + (ly[idx+1]-ly[idx])/
        (lx[idx+1]-lx[idx]) * (x-lx[idx]));
      break;
    case 'd':
      x = ((double *)data)[i];
      idx = GetIndex(x, lx, idx,n_ln);
      ((double *)data)[i] = (double)(ly[idx] + (ly[idx+1]-ly[idx])/
        (lx[idx+1]-lx[idx]) * (x-lx[idx]));
      break;
    default:
      printf("Another impossible error\n"); exit(0);
      break;
    } 
    
  }
}
/***************************************************************************/
/*                                                                         */
/*   ScaleData: out = m*in+b                                               */
/*                                                                         */
/***************************************************************************/
static void ScaleData(void *data, char type, int npts, double m, double b) {
  char *data_c;
  short *data_s;
  unsigned short *data_u;
  int *data_i;
  float *data_f;
  double *data_d;  

  int i;

  switch(type) {
  case 'n':
    break;
  case 'c':
    data_c = (char *)data;
    for (i=0; i<npts; i++) {
      data_c[i] =(char)((double)data_c[i] * m + b);
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
  case 'i':
    data_i = (int *)data;
    for (i=0; i<npts; i++) {
      data_i[i] = (int)((double)data_i[i] * m + b);
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
    printf("Another impossible error\n"); exit(0);
    break;
  } 
}
/***************************************************************************/
/*                                                                         */
/*            AllocTmpbuff: allocate a buffer of the right type and size   */
/*                                                                         */
/***************************************************************************/
static void *AllocTmpbuff(char type, int n) {
  void *buff=NULL;
  switch(type) {
  case 'n':
    buff = NULL;
    break;
  case 'c':
    buff = malloc(n*sizeof(char));
    break;
  case 'i':
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
    exit(0);
    break;
  }
  if ((type != 'n') && (buff==NULL)) {
    printf("Memory Allocation error in AllocTmpbuff\n");
    exit(0);
  }
  return(buff);
}
    
/***************************************************************************/
/*                                                                         */
/*            AddData: add B to A.  A is unchanged                         */
/*                                                                         */
/***************************************************************************/
static void AddData(void *A, void *B, char type, int n) {
  int i;
  
  switch(type) {
  case 'n': /* null read */
    break;
  case 'c':
    for (i=0; i<n; i++) {
      ((char*)A)[i]+=((char*)B)[i];
    }
    break;
  case 'i':
    for (i=0; i<n; i++) {
      ((int*)A)[i]+=((int*)B)[i];
    }
    break;
  case 's':
    for (i=0; i<n; i++) {
      ((short*)A)[i]+=((short*)B)[i];
    }
    break;
  case 'u':
    for (i=0; i<n; i++) {
      ((unsigned short*)A)[i]+=((unsigned short*)B)[i];
    }
    break;
  case 'f':
    for (i=0; i<n; i++) {
      ((float*)A)[i]+=((float*)B)[i];
    }
    break;
  case 'd':
    for (i=0; i<n; i++) {
      ((double*)A)[i]+=((double*)B)[i];
    }
    break;
  default:
    printf("Unexpected bad type error in AddData\n");
    exit(0);
    break;
  }
}
    
    
/***************************************************************************/
/*                                                                         */
/*   ReadLinterpFile: Read in the linterp data for this field              */
/*                                                                         */
/***************************************************************************/
static int ReadLinterpFile(struct CalEntryType *E) {
  FILE *fp;
  int i;
  char line[255];

  fp = fopen(E->file, "r");
  if (fp==NULL) {
    return (E_OPEN_LINFILE);
  }
  /* first read the file to see how big it is */
  i=0;
  while (GetCSLine(fp, line)) {
    i++;
  }
  E->n_linfile = i;
  E->lx = (double *)malloc(i*sizeof(double));
  E->ly = (double *)malloc(i*sizeof(double));
  E->maxx = -1E10;
  E->minx = 1E10;
  rewind(fp);
  /* now read in the data */
  for (i=0; i<E->n_linfile; i++) {
    GetCSLine(fp, line);
    sscanf(line, "%lg %lg",&(E->lx[i]), &(E->ly[i]));
    if (E->minx<E->lx[i]) E->minx = E->lx[i];
    if (E->maxx>E->lx[i]) E->maxx = E->lx[i];
  }
  return (E_OK);
}
/***************************************************************************/
/*                                                                         */
/*     File File Frame numbers into dataout                                */
/*                                                                         */
/***************************************************************************/
static void FillFileFrame(void *dataout, char rtype, int s0, int n) {
  int i;
  switch(rtype) {
  case 'c':
    for (i=0; i<n; i++) {
      ((char*)dataout)[i] = (char)i+s0;
    }
    break;
  case 'i':
        for (i=0; i<n; i++) {
      ((int*)dataout)[i] = (int)i+s0;
    }
    break;
  case 's':
        for (i=0; i<n; i++) {
      ((short*)dataout)[i] = (short)i+s0;
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
/*    FindT0: find creation time of the dt file                            */
/*                                                                         */
/***************************************************************************/
static int FindT0(const char *filename, double srate) {
  char tmpfilename[180];
  struct stat buf;
  int t0;

  /** Find t0 **/
  strcpy(tmpfilename, filename);
  tmpfilename[strlen(tmpfilename)-2] = 'd';
  tmpfilename[strlen(tmpfilename)-1] = 't';
  stat(tmpfilename, &buf);
  t0 = (int)(buf.st_mtime - (double)buf.st_size/srate);

  return(t0);
}

/***************************************************************************/
/*                                                                         */
/*   TypeCopy.  Copy from type 'i' to some other type                      */
/*                                                                         */
/***************************************************************************/
static void TypeCopy(void *data_out, char return_type,
              int *data_in,int n) {
  int i;
  switch(return_type) {
  case 'c':
    for (i=0; i<n; i++) {
      ((char *)data_out)[i] = data_in[i];
    }
    break;
  case 'i':
    for (i=0; i<n; i++) {
      ((int *)data_out)[i] = data_in[i];
    }
    break;
  case 's':
    for (i=0; i<n; i++) {
      ((short *)data_out)[i] = data_in[i];
    }
    break;
  case 'u':
    for (i=0; i<n; i++) {
      ((unsigned short *)data_out)[i] = data_in[i];
    }
    break;
  case 'f':
    for (i=0; i<n; i++) {
      ((float *)data_out)[i] = data_in[i];
    }
    break;
  case 'd':
    for (i=0; i<n; i++) {
      ((double *)data_out)[i] = data_in[i];
    }
    break;
  default:
    printf("Unexpected bad type error in TypeCopy\n");
    exit(0);
    break;
  }
}

static void FTypeCopy(void *data_out, char return_type,
              float *data_in,int n) {
  int i;
  switch(return_type) {
  case 'c':
    for (i=0; i<n; i++) {
      ((char *)data_out)[i] = (char)data_in[i];
    }
    break;
  case 'i':
    for (i=0; i<n; i++) {
      ((int *)data_out)[i] = (int)data_in[i];
    }
    break;
  case 's':
    for (i=0; i<n; i++) {
      ((short *)data_out)[i] = (short)data_in[i];
    }
    break;
  case 'u':
    for (i=0; i<n; i++) {
      ((unsigned short *)data_out)[i] = (unsigned short)data_in[i];
    }
    break;
  case 'f':
    for (i=0; i<n; i++) {
      ((float *)data_out)[i] = data_in[i];
    }
    break;
  case 'd':
    for (i=0; i<n; i++) {
      ((double *)data_out)[i] = data_in[i];
    }
    break;
  default:
    printf("Unexpected bad type error in MPFill\n");
    exit(0);
    break;
  }
}


/***************************************************************************/
/*                                                                         */
/*   CReadData: This subroutine returns calibrated data.  Data is acquired */
/*              by first calling readdata().                               */
/*                                                                         */
/***************************************************************************/
int CReadData(const char *filename_in, const char *field_code,
             int first_sframe, int first_samp,
             int num_sframes, int num_samp,
             char return_type, void *data_out,
             int *error_code) {

  int i_format, i_field, i_lincom;
  int s_per_frame;
  static int first_time=1;
  char filename[100], tmpfilename[100];
  int i, n_read;
  void *tmpbuf;
  int *mp_cnt=NULL, *mp_data, cp_data;
  static int recurse_level = 0;
  int t0;
  double f0;
  float *tmpin;

  if (recurse_level>10) {
    *error_code = E_RECURSE_LEVEL;
    return(0);
  }

  strcpy(filename, filename_in);

  if (first_time) {
    *error_code = ReadCalFile();
    if (*error_code != E_OK) return(0);
    first_time=0;
  }
  
  /***************************************************************/
  /* Read the first byte of the data file to determine file type */
  *error_code=GetFileFormatIndex(filename,&i_format);
  if (*error_code!=0) {
    return(0);
  }

  /********************************************/
  /* if Asking for "FILEFRAM" or "INDEX", just return it */
  if ((strcmp(field_code,"FILEFRAM")==0) ||
      (strcmp(field_code,"INDEX")==0)) {
    n_read = num_sframes + num_samp;
    if (data_out!=NULL) {
      FillFileFrame(data_out, return_type, first_sframe+first_samp, n_read);
    }
    *error_code=E_OK;
    return(n_read);
  }
      
  /******************************************************/
  /* determine which field has been requested (i_field) */
  *error_code=GetFieldIndex(field_code, i_format, &i_field);
  if (*error_code!=0) {
    return(0);
  }

  /***************************/
  /* Do the Conversions      */
  switch (cstruct[i_format].field[i_field].type) {
  case CST_END:
    n_read = ReadData(filename, field_code,
             first_sframe, first_samp,
             num_sframes, num_samp,
             return_type, data_out, error_code);
    if (*error_code!=E_OK) return(0);
    break;
  case CST_FRAMETIME:
    /* Find t0 from the file creation time */
    t0 = FindT0(filename_in,  cstruct[i_format].field[i_field].framerate);
    /* Find f0 from reading the first frame val */
    strcpy(tmpfilename, filename);
    tmpfilename[strlen(tmpfilename)-2] = '0';
    tmpfilename[strlen(tmpfilename)-1] = '0';

    recurse_level++;
    CReadData(tmpfilename,
              cstruct[i_format].field[i_field].in_code[0],
              0, 0,
              0, 1,
              'd', &f0, error_code);
    recurse_level--;
    if (*error_code!=E_OK) return(0);

    /* read the framenum data */
    recurse_level++;
    n_read = CReadData(filename,
                      cstruct[i_format].field[i_field].in_code[0],
                      first_sframe, first_samp,
                      num_sframes, num_samp,
                      return_type, data_out, error_code);
    recurse_level--;
    if (*error_code!=E_OK) return(0);
    
    /* convert to seconds using f0, t0 and framerate */
    ScaleData(data_out, return_type, n_read,
              1.0/cstruct[i_format].field[i_field].framerate,
              (double)t0 - f0/cstruct[i_format].field[i_field].framerate);
    break;
  case CST_BITFIELD:
    if (return_type=='n') { /* null read to find size */
      recurse_level++;
      n_read = CReadData(filename,cstruct[i_format].field[i_field].in_code[0],
           0,0, /* 1st sframe, 1st samp */
           1,0, /* num sframes, num samps */
           'n',(void*)NULL,
           error_code);
      recurse_level--;
    } else {
      recurse_level++;
      s_per_frame =
        CReadData(filename,cstruct[i_format].field[i_field].in_code[0],
           0,0, /* 1st sframe, 1st samp */
           1,0, /* num sframes, num samps */
           'n',(void*)NULL,
           error_code);
      recurse_level--;
      
      num_samp+=num_sframes*s_per_frame;
      num_sframes=0;
      mp_cnt = (int *)AllocTmpbuff('i',num_samp);
      recurse_level++;
      n_read = CReadData(filename,
                         cstruct[i_format].field[i_field].in_code[0],
                         first_sframe, first_samp,
                         num_sframes, num_samp,
                         'i', mp_cnt, error_code);
      recurse_level--;
      for (i=0; i<n_read; i++) {
        mp_cnt[i] = (mp_cnt[i] >> cstruct[i_format].field[i_field].cntval) & 0x0001;
      }
      TypeCopy(data_out, return_type,mp_cnt, n_read);
      free(mp_cnt);
    }
    break;
  case CST_SHR:
    if (return_type=='n') { /* null read to find size */
      recurse_level++;
      n_read = CReadData(filename,cstruct[i_format].field[i_field].in_code[0],
           0,0, /* 1st sframe, 1st samp */
           1,0, /* num sframes, num samps */
           'n',(void*)NULL,
           error_code);
      recurse_level--;
    } else {
      recurse_level++;
      s_per_frame =
        CReadData(filename,cstruct[i_format].field[i_field].in_code[0],
           0,0, /* 1st sframe, 1st samp */
           1,0, /* num sframes, num samps */
           'n',(void*)NULL,
           error_code);
      recurse_level--;
      
      num_samp+=num_sframes*s_per_frame;
      num_sframes=0;
      mp_cnt = (int *)AllocTmpbuff('i',num_samp);
      recurse_level++;
      n_read = CReadData(filename,
                         cstruct[i_format].field[i_field].in_code[0],
                         first_sframe, first_samp,
                         num_sframes, num_samp,
                         'i', mp_cnt, error_code);
      recurse_level--;
      for (i=0; i<n_read; i++) {
        mp_cnt[i] = (mp_cnt[i] >> cstruct[i_format].field[i_field].cntval) & cstruct[i_format].field[i_field].bitmask;
      }
      TypeCopy(data_out, return_type,mp_cnt, n_read);
      free(mp_cnt);
    }
    break;
  case CST_SHL:
    if (return_type=='n') { /* null read to find size */
      recurse_level++;
      n_read = CReadData(filename,cstruct[i_format].field[i_field].in_code[0],
           0,0, /* 1st sframe, 1st samp */
           1,0, /* num sframes, num samps */
           'n',(void*)NULL,
           error_code);
      recurse_level--;
    } else {
      recurse_level++;
      s_per_frame =
        CReadData(filename,cstruct[i_format].field[i_field].in_code[0],
           0,0, /* 1st sframe, 1st samp */
           1,0, /* num sframes, num samps */
           'n',(void*)NULL,
           error_code);
      recurse_level--;
      
      num_samp+=num_sframes*s_per_frame;
      num_sframes=0;
      mp_cnt = (int *)AllocTmpbuff('i',num_samp);
      recurse_level++;
      n_read = CReadData(filename,
                         cstruct[i_format].field[i_field].in_code[0],
                         first_sframe, first_samp,
                         num_sframes, num_samp,
                         'i', mp_cnt, error_code);
      recurse_level--;
      for (i=0; i<n_read; i++) {
        mp_cnt[i] = (mp_cnt[i] << cstruct[i_format].field[i_field].cntval) & cstruct[i_format].field[i_field].bitmask;
      }
      TypeCopy(data_out, return_type,mp_cnt, n_read);
      free(mp_cnt);
    }
    break;
  case CST_DIFF:
    if (return_type=='n') { /* null read to find size */
      recurse_level++;
      n_read = CReadData(filename,cstruct[i_format].field[i_field].in_code[0],
           0,0, /* 1st sframe, 1st samp */
           1,0, /* num sframes, num samps */
           'n',(void*)NULL,
           error_code);
      recurse_level--;
    } else {
      recurse_level++;
      s_per_frame =
        CReadData(filename,cstruct[i_format].field[i_field].in_code[0],
           0,0, /* 1st sframe, 1st samp */
           1,0, /* num sframes, num samps */
           'n',(void*)NULL,
           error_code);
      recurse_level--;
      
      num_samp+=num_sframes*s_per_frame+1; /* we must read one more sample */
      num_sframes=0;

      first_samp--;
      if(first_samp<0) {
	first_samp = s_per_frame-1;
	first_sframe--;
	if(first_sframe < 0) {
	  first_sframe = 0;
	  first_samp = 0;
	}
      }

      tmpin = (float *)AllocTmpbuff('f', num_samp);
      recurse_level++;
      n_read = CReadData(filename,
                         cstruct[i_format].field[i_field].in_code[0],
                         first_sframe, first_samp,
                         num_sframes, num_samp,
                         'f', tmpin, error_code);
      recurse_level--;
      for (i=1; i<n_read; i++) {
        tmpin[i-1] = tmpin[i]-tmpin[i-1];
	  if( tmpin[i-1] > (cstruct[i_format].field[i_field].modulo / 4.0) )
	    tmpin[i-1] -= cstruct[i_format].field[i_field].modulo;
	if( tmpin[i-1] < -(cstruct[i_format].field[i_field].modulo / 4.0) )
	  tmpin[i-1] += cstruct[i_format].field[i_field].modulo;
      }
      n_read--;
      FTypeCopy(data_out, return_type,tmpin, n_read);
      free(tmpin);
    }
    break;
  case CST_SIN:
    if (return_type=='n') { /* null read to find size */
      recurse_level++;
      n_read = CReadData(filename,cstruct[i_format].field[i_field].in_code[0],
           0,0, /* 1st sframe, 1st samp */
           1,0, /* num sframes, num samps */
           'n',(void*)NULL,
           error_code);
      recurse_level--;
    } else {
      recurse_level++;
      s_per_frame =
        CReadData(filename,cstruct[i_format].field[i_field].in_code[0],
           0,0, /* 1st sframe, 1st samp */
           1,0, /* num sframes, num samps */
           'n',(void*)NULL,
           error_code);
      recurse_level--;
      
      num_samp+=num_sframes*s_per_frame;
      num_sframes=0;
      tmpin = (float *)AllocTmpbuff('f', num_samp);
      recurse_level++;
      n_read = CReadData(filename,
                         cstruct[i_format].field[i_field].in_code[0],
                         first_sframe, first_samp,
                         num_sframes, num_samp,
                         'f', tmpin, error_code);
      recurse_level--;
      for (i=0; i<n_read; i++) {
        tmpin[i] = sin(tmpin[i]*(180.0/M_PI));
      }
      FTypeCopy(data_out, return_type,tmpin, n_read);
      free(tmpin);
    }
    break;
  case CST_MPLEX:
    if (return_type=='n') { /* null read to find size */
      n_read = ReadData(filename,cstruct[i_format].field[i_field].in_code[0],
           0,0, /* 1st sframe, 1st samp */
           1,0, /* num sframes, num samps */
           'n',(void*)NULL,
           error_code);
    } else {
      s_per_frame =
        ReadData(filename,cstruct[i_format].field[i_field].in_code[0],
                 0,0, /* 1st sframe, 1st samp */
                 1,0, /* num sframes, num samps */
                 'n',(void*)NULL,
           error_code);

      /* num_samp holds how many samples we want to return */
      /* n_read holds how many samples we need to read to be sure we get it */
      /*    it must be larger than cntmax */
      num_samp += num_sframes * s_per_frame;
      if (num_samp < cstruct[i_format].field[i_field].cntmax+1) {
        n_read = num_samp + cstruct[i_format].field[i_field].cntmax+1;
        first_samp += first_sframe * s_per_frame -
          cstruct[i_format].field[i_field].cntmax+1;
        if (first_samp<0) {
          first_samp=0;
        }
      } else {
        n_read = num_samp;
        first_samp += first_sframe * s_per_frame;
      }

      /* read CNTFIELD */
      mp_cnt = (int *)AllocTmpbuff('i',n_read);
      ReadData(filename, cstruct[i_format].field[i_field].in_code[0],
                        0, first_samp,
                        0, n_read,
                        'i', mp_cnt, error_code);
      
      /* read DATAFIELD */
      mp_data = (int *)AllocTmpbuff('i', n_read);
      n_read = ReadData(filename, cstruct[i_format].field[i_field].in_code[1],
               0, first_samp,
               0, n_read,
               'i', mp_data, error_code);


      /* search for first of the requested field */
      cp_data=0;
      for (i=0; (i<n_read-1) &&
             (mp_cnt[i]!=cstruct[i_format].field[i_field].cntval); i++);
      cp_data = mp_data[i];

      if (n_read > num_samp) n_read=num_samp;
      
      /* refill only the req field (repeated) into mp_data. */
      for (i=0; i<n_read; i++) {
        if (mp_cnt[i]==cstruct[i_format].field[i_field].cntval)
          cp_data=mp_data[i];
        mp_data[i]=cp_data;
      }
      TypeCopy(data_out, return_type, mp_data, n_read);

      free(mp_data);
      free(mp_cnt);
    }
    break;
  case CST_LINCOM:
    /** Read the first into data_out and scale it **/
    recurse_level++;
    n_read = CReadData(filename,
                      cstruct[i_format].field[i_field].in_code[0],
                      first_sframe, first_samp,
                      num_sframes, num_samp,
                      return_type, data_out, error_code);
    recurse_level--;

    if (*error_code!=E_OK) return(0);
    ScaleData(data_out, return_type, n_read,
              cstruct[i_format].field[i_field].m[0],
              cstruct[i_format].field[i_field].b[0]);

    /** Now read the rest into a dummy buffer one at a time **/
    tmpbuf = AllocTmpbuff(return_type, n_read);
    for (i_lincom=1; i_lincom<cstruct[i_format].field[i_field].n_lincom; 
         i_lincom++) {
      recurse_level++;
      if (CReadData(filename,
                   cstruct[i_format].field[i_field].in_code[i_lincom],
                   first_sframe, first_samp,
                   num_sframes, num_samp,
                   return_type, tmpbuf, error_code) != n_read) {
        *error_code=E_SIZE_MISMATCH;
        recurse_level--;
        return(0);
      }
      recurse_level--;

      /** scale them **/
      ScaleData(tmpbuf, return_type, n_read,
                cstruct[i_format].field[i_field].m[i_lincom],
                cstruct[i_format].field[i_field].b[i_lincom]);
      /** add them to data_out **/
      AddData(data_out, tmpbuf, return_type, n_read);
    }
    free(tmpbuf);
    break;
  case CST_LINFILE:
    if (cstruct[i_format].field[i_field].n_linfile==-1) {
      *error_code = ReadLinterpFile(&(cstruct[i_format].field[i_field]));
      if (*error_code!=0) {
        return(0);
      }
    }
    /* Call ReadData and Read the data */
    recurse_level++;
    n_read = CReadData(filename,
                      cstruct[i_format].field[i_field].in_code[0],
                      first_sframe, first_samp,
                      num_sframes, num_samp,
                      return_type, data_out, error_code);
    recurse_level--;

    if (*error_code!=E_OK) return(0);
    LinterpData(data_out, return_type, n_read,
                cstruct[i_format].field[i_field].lx,
                cstruct[i_format].field[i_field].ly,
                cstruct[i_format].field[i_field].n_linfile);
    break;
  default:
    printf("Impossible error in creaddata... Ack!!\n");
    exit(0);
  }

  return(n_read);
}

