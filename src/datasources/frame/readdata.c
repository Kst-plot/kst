/*                           (C) 1996 C. Barth Netterfield */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************/
/*                                                                         */
/*    ReadData: This subroutine reads data from stratoplate data files     */
/*              See 'FileFormats' file for usage documentation             */
/*              'ReadData()' is last in this file.                         */
/*                                                                         */
/***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "readdata.h"
#define MAX_LINE_LENGTH 120
#define MAX_FIELDS_IN_FORMAT 500

#ifndef FILEFORMATS_DIR
#define FILEFORMATS_DIR "/data/etc"
#endif

/* define error codes */
const char *const RD_ERROR_CODES[7] = {"OK",
                           "Could not open FileFormat file",
                           "Error in FileFormat file",
                           "Could not open Data file",
                           "Unrecognized file type",
                           "Field code not found in File Format",
                           "Unrecognized return type"
};

/* define the structure which holds the FileFormat file */
struct FileFormatStruct {
  int format_spec;  /* the file format specifier */
  int block_length;
  int blocks_per_file;
  int n_fields;
  unsigned long long code[MAX_FIELDS_IN_FORMAT];
  char type[MAX_FIELDS_IN_FORMAT];
  int start_byte[MAX_FIELDS_IN_FORMAT];
  int num_elements[MAX_FIELDS_IN_FORMAT];
  int skip[MAX_FIELDS_IN_FORMAT];
} *fstruct;

static int rd_n_formats=0;

int RD_GetFFLine(FILE *fp, char *line);
int RD_StripFileNN(char *filename);

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
static void rd_flip_bytes(char *d, int nwords) {
  int i,imax;
  char h;

  imax=nwords*2;
  
  for (i=0;i<imax; i+=2) {
    h=d[i];
    d[i]=d[i+1];
    d[i+1]=h;
  }
}

static void rd_flip_words(short *d, int nints) {
  int i,imax;
  short h;

  imax=nints*2;
  
  for (i=0;i<imax; i+=2) {
    h=d[i];
    d[i]=d[i+1];
    d[i+1]=h;
  }
}
#else
#define rd_flip_bytes(a,b)  /* do nothing on alpha or intel machines */
#define rd_flip_words(a,b)  /* do nothing on alpha or intel machines */
#endif


/***************************************************************************/
/*                                                                         */
/*    GetFFLine: read non-comment line from file format file               */
/*        The line is placed   in *line.                                   */
/*        Returns 1 if successful, 0 if unsuccessful                         */
/*                                                                         */
/***************************************************************************/
int RD_GetFFLine(FILE *fp, char *line) {
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
static long long RD_StringToCode(const char *tmpcode) {
  long long code;
  char *cptr;
  int i;

  cptr=(char*) &code;
  code=0;
  
  for (i=0; i<(int)strlen(tmpcode); i++) {
    cptr[i]=tmpcode[i];
  }
  for (i=strlen(tmpcode); i<8; i++) {
    cptr[i]=0;
  }
  
  return code;
}

/***************************************************************************/
/*                                                                         */
/*    Read one structure from the data stream fp                           */
/*                                                                         */
/***************************************************************************/
static int RD_ReadOneStruct(FILE *fp, int i_format) {
  int done=0, i_field;
  char line[MAX_LINE_LENGTH];
  int n_scan_fields;
  char tmpcode[80],tmptype[10];
 
  /* read the begin line */
  RD_GetFFLine(fp,line);
  if (strncmp(line,"BEGIN",5)!=0) {
    return(E_FFFILE_FORMAT);
  }
  /* read the format specifier */
  if (RD_GetFFLine(fp,line)==0) {
    return(E_FFFILE_FORMAT);
  }
  sscanf(line,"%x",&(fstruct[i_format].format_spec));
  /* read the  block length */
  if (RD_GetFFLine(fp,line)==0) {
    return(E_FFFILE_FORMAT);
  }
  sscanf(line,"%d",&(fstruct[i_format].block_length));
  /* read the  blocks per file */
  if (RD_GetFFLine(fp,line)==0) {
    return(E_FFFILE_FORMAT);
  }
  sscanf(line,"%d",&(fstruct[i_format].blocks_per_file));
  /* read the fields */
  fstruct[i_format].n_fields=0;
  done=0;
  i_field=0;
  while(!done) {
    if (RD_GetFFLine(fp,line)==0) {
      return(E_FFFILE_FORMAT);
    }
    if (strncmp(line,"END",3)==0) {
      done=1;
    } else {
      n_scan_fields = sscanf(line,"%s %s %d %d %d", tmpcode, tmptype,
                             &(fstruct[i_format].start_byte[i_field]),
                             &(fstruct[i_format].num_elements[i_field]),
                             &(fstruct[i_format].skip[i_field]));
      if (n_scan_fields==4) {
        fstruct[i_format].skip[i_field] = 1;
      } else if (n_scan_fields<4) {
        return(E_FFFILE_FORMAT);
      }
      fstruct[i_format].type[i_field]=tmptype[0];
      /* convert tmpcode to code */
      if(strlen(tmpcode)>8) {
        return(E_FFFILE_FORMAT);
      }
      fstruct[i_format].code[i_field]=RD_StringToCode(tmpcode);
      i_field++;
    }
  }
  fstruct[i_format].n_fields=i_field;

  return (E_OK);
}
/***************************************************************************/
/*                                                                         */
/*    Read File Format file: place the results in a global structure       */
/*                                                                         */
/***************************************************************************/
static int RD_ReadFileFormat() {
  FILE *fp, *fp_i;
  char line[MAX_LINE_LENGTH];
  char fileformats_filename[MAX_LINE_LENGTH];
  int i_format, n_includes;
  char include_file_name[160];

  sprintf(fileformats_filename, "%s/FileFormats", FILEFORMATS_DIR);
  fp = fopen(fileformats_filename,"r");
  if (fp==NULL) {
    return(E_OPEN_FFFILE);  /* error opening FileFormats */
  }
  
  /* count the number of 'BEGIN' lines found */
  rd_n_formats = n_includes=0;
  while (RD_GetFFLine(fp, line)) {
    if (strncmp(line,"BEGIN",5)==0) rd_n_formats++;
    else if (strncmp(line,"INCLUDE",7)==0) {
      rd_n_formats++;
      n_includes++;
    }
  }
  if (rd_n_formats==0) {
    return(E_FFFILE_FORMAT);
  }
  
  /* allocate memory for fstruct */
  fstruct=(struct FileFormatStruct *)
    malloc(rd_n_formats * sizeof(struct FileFormatStruct));
  if (fstruct==NULL) {
    printf("ReadData error: could not allocate memory for fstruct\n");
    exit(0);
  }
  
  /* read in the format structures */
  rewind(fp);

  /* Read in the Includes */
  for (i_format=0; i_format<n_includes; i_format++) {
    RD_GetFFLine(fp,line);
    
    if (strncmp(line,"INCLUDE",7)!=0) {
      return(E_FFFILE_FORMAT);
    }
    sscanf(line,"INCLUDE %s",include_file_name);
    
    fp_i = fopen(include_file_name,"r");
    if (fp_i==NULL) {
      return(E_OPEN_FFFILE);  /* error opening FileFormats */
    }
    
    RD_ReadOneStruct(fp_i, i_format);

    fclose (fp_i);
  }
  
  for (i_format=n_includes; i_format<rd_n_formats; i_format++) {
    RD_ReadOneStruct(fp, i_format);
  }
  return (E_OK);
}

/***************************************************************************/
/*                                                                         */
/*      Get File Format Index: read the first byte of the file to see what */
/*          format it is in                                                */
/*                                                                         */
/***************************************************************************/
static int RD_GetFileFormatIndex(char *filename,int *i_format) {
  int fp;
  unsigned short tmp_in;
  int file_code;

  fp=open(filename,O_RDONLY);
  if (fp<0) return(E_FILE_OPEN);

  read(fp, &tmp_in, sizeof(unsigned short));
  rd_flip_bytes((char*)&tmp_in,1);
  file_code=tmp_in;

  for (*i_format=0; (fstruct[*i_format].format_spec!=file_code) &&
         (*i_format<rd_n_formats); (*i_format)++);

  if (*i_format>= rd_n_formats) return (E_BAD_FILETYPE);
  
  close(fp);
  return(0);
}


/***************************************************************************/
/*                                                                         */
/*  GetFieldIndex: convert field code into an index number                 */
/*    get the fstruct index for the field to be read                       */
/*    *i_field gets the index                                              */
/*    returns error code                                                   */
/*                                                                         */
/***************************************************************************/
static int RD_GetFieldIndex(const char *field_code,int i_format, int *i_field) {
  unsigned long long lfield_code;
  static int i_f=0;
  int n;	

  n = fstruct[i_format].n_fields;
  lfield_code=RD_StringToCode(field_code);
  
  /** if we are asking for the same field as last time, don't search */
  if (i_f<n) {
    if (fstruct[i_format].code[i_f]==lfield_code) {
      *i_field = i_f;
      return(0);
    }
  }

  for (i_f=0; (fstruct[i_format].code[i_f]!=lfield_code) &&
         (i_f<n); i_f++);

  *i_field = i_f;
  if (i_f>=fstruct[i_format].n_fields) return (E_BAD_CODE);
  
  return(0);
}

/***************************************************************************/
/*                                                                         */
/*   Skip Read: read from from fp, taking into account skipping            */
/*              if skip=1, skipping is ignored and read is used            */
/*              otherwise, nsamp * skip * sizeof(type) bytes are read and  */
/*               the skipped data is returned                              */
/*              The number of items read is returned (not # bytes)         */
/*                                                                         */
/***************************************************************************/
static int RD_SkipRead(int fp, unsigned char *out_buf, int n_samp, int size, int skip){
  int samps_read, bytes_read;
  int bytes_to_read,i,j;
  static char *in_buf=NULL;
  static int inbuf_size=0;
  
  if (skip==1) {
    bytes_read=read(fp, out_buf, n_samp*size);
  } else {
    bytes_to_read = (n_samp-1) * skip+size;
    /* allocate in_buf */
    if (bytes_to_read>inbuf_size) {
      in_buf = (char *)realloc(in_buf, bytes_to_read);
      if (in_buf==NULL) {
        printf("readdata: error allocating read buffer\n");
        exit(0);
      }
      inbuf_size = bytes_to_read;
    }
    /* read in the data */
    bytes_read = read(fp, in_buf, bytes_to_read);
    if (bytes_read>=size) {
      samps_read= 1 + bytes_read/skip;
    } else {
      samps_read=0;
    }
    

    /* copy the buffer to out_buf */
    for (i=0;i<samps_read; i++) {
      for (j=0;j<size; j++) {
        out_buf[i*size+j] = in_buf[i*skip+j];
      }
    }
  }
  return(bytes_read);
}

/***************************************************************************/
/*                                                                         */
/*    ReadFromBlock: Read samples from a block                             */
/*         The data are returned in &(data_buffer[*buff_ptr]).  The next   */
/*         starting position is returned in *buff_ptr.                     */
/*         The function returns the number of elements read                */
/*                                                                         */
/***************************************************************************/
static int RD_ReadFromBlock(int fp, int i_block, int first_samp,
                  int n_samp, struct FileFormatStruct *fs, int i_field, 
                  unsigned char *data_buffer, int *buff_ptr) {
  char type;
  int bytes_read;
  static int tempCsize=0;
  static char *tempCBuff=NULL; /* temp buf for 1 block of C data */
  short C_gain, C_offset;
  int i;
  int skip, adv;

  skip = fs->skip[i_field];

  type=fs->type[i_field];

  if (type=='c') { /* Read in a char structure */
    /* move to the begining of the read */
    lseek(fp, i_block*fs->block_length + fs->start_byte[i_field] +
          first_samp*skip, SEEK_SET);
    bytes_read = RD_SkipRead(fp, &(data_buffer[*buff_ptr]), n_samp, 1,
                          skip);
    if (bytes_read>=1) {
      n_samp = (bytes_read-1)/skip + 1;
    } else {
      n_samp=0;
    }
    *buff_ptr+=n_samp;

  } else if (type=='C') { /* read in compressed data */
    /* make/enlarge C buffer */
    if (tempCsize<fs->num_elements[i_field]) {
      tempCBuff=(char *)realloc(tempCBuff, fs->num_elements[i_field]);
      if (tempCBuff==NULL) {
        printf("Error alocating tempCBuff.  Ack!!\n");
        exit(0);
      }
    }
    /* read gain and offset, which are located at the beg of block */
    lseek(fp, i_block*fs->block_length + fs->start_byte[i_field],
          SEEK_SET);
    read(fp, &C_gain, 2);
    rd_flip_bytes((char*)&C_gain,1);
    read(fp, &C_offset, 2);
    rd_flip_bytes((char*)&C_offset,1);

    /* read the char array from the block */
    lseek(fp, i_block*fs->block_length + fs->start_byte[i_field] +
          first_samp+4, SEEK_SET);
    bytes_read=read(fp, tempCBuff, n_samp);
    /* convert the char data to shorts */
    for (i=0;i<bytes_read;i++) {
      ((int *)data_buffer)[*buff_ptr+i]=(int)tempCBuff[i] *(int)C_gain +
        (int)C_offset;
    }
    *buff_ptr+=bytes_read;
    n_samp=bytes_read;
  } else if ((type=='i') || (type=='S') || (type=='U')) { /* read 4 byte data */
    /* move to the begining of the read */
    if (skip==1) adv=4;
    else adv = skip;
    
    lseek(fp, i_block*fs->block_length + fs->start_byte[i_field] +
          first_samp*adv, SEEK_SET);
    bytes_read=RD_SkipRead(fp, &(data_buffer[*buff_ptr]), n_samp, 4,
                          skip);

    if (bytes_read>=4) {
      n_samp= 1 + (bytes_read-4)/adv;
    } else {
      n_samp=0;
    }

    rd_flip_bytes((char*)&(data_buffer[*buff_ptr]), n_samp*2);
    rd_flip_words((short*)&(data_buffer[*buff_ptr]), n_samp);
    *buff_ptr+=n_samp*4;
  } else if (type=='s') { /* read in 2 byte int data */
    if (skip==1) adv=2;
    else adv = skip;

    /* move to the begining of the read */
    lseek(fp, i_block*fs->block_length + fs->start_byte[i_field] +
          first_samp*adv, SEEK_SET);
    bytes_read=RD_SkipRead(fp, &(data_buffer[*buff_ptr]), n_samp, 2,
                          fs->skip[i_field]);
    if (bytes_read>=2) {
      n_samp= 1 + (bytes_read-2)/adv;
    } else {
      n_samp=0;
    }

    rd_flip_bytes((char*)&(data_buffer[*buff_ptr]), n_samp);
    *buff_ptr+=n_samp*2;
  } else if (type=='u') { /* read in 2 byte unsigned data */
    if (skip==1) adv=2;
    else adv = skip;

    /* move to the begining of the read */
    lseek(fp, i_block*fs->block_length + fs->start_byte[i_field] +
          first_samp*adv, SEEK_SET);

    bytes_read=RD_SkipRead(fp, &(data_buffer[*buff_ptr]), n_samp, 2,
                          fs->skip[i_field]);

    if (bytes_read>=2) {
      n_samp= 1 + (bytes_read-2)/adv;
    } else {
      n_samp=0;
    }
    
    rd_flip_bytes((char*)&(data_buffer[*buff_ptr]), n_samp);
    *buff_ptr+=n_samp*2;
  } else { /* this should never happen */
    printf("Unexpected bad type error in readdata:RD_ReadFromBlock.\n");
    exit(0);
  }
  

  return(n_samp);
}


/***************************************************************************/
/*                                                                         */
/*    StripFileNN: removes file number from filename; returns number       */
/*      The file name must be of the form [name].?#[#...]                  */
/*      egs: file.B00  file.b4a  file.B12f file.x0                         */
/*                                                                         */
/***************************************************************************/
int RD_StripFileNN(char *filename) {
  int i_dot;
  int fileNN;

  i_dot = strlen(filename)-1;
  while((filename[i_dot] !='.') && (i_dot>0)) i_dot--;

  fileNN = atoi(filename+i_dot+2);
  filename[i_dot+2] = '\0';

  return(fileNN);
}

/***************************************************************************/
/*                                                                         */
/*    OpenFile: opens file number nn                                       */
/*                                                                         */
/***************************************************************************/
static int RD_OpenFile(char *filename, int file_nn) {
  int fp;

  RD_StripFileNN(filename);
  sprintf(filename+strlen(filename),"%.2x",file_nn);
  fp=open(filename,O_RDONLY);

  return(fp);
}

/***************************************************************************/
/*                                                                         */
/*    Allocate temp data buffer                                            */
/*                                                                         */
/***************************************************************************/
static unsigned char *RD_AllocBuffer(char type, int num_samp) {
  unsigned char *data_buffer;
  
  if (type=='c') {
    data_buffer = (unsigned char *) malloc(num_samp);
  } else if (type=='s') {
    data_buffer = (unsigned char *) malloc(num_samp*sizeof(short));
  } else if (type=='u') {
    data_buffer = (unsigned char *) malloc(num_samp*sizeof(unsigned short));
  } else if (type=='C') {
    data_buffer = (unsigned char *) malloc(num_samp*sizeof(int));
  } else if ((type=='i')||(type=='S')) {
    data_buffer = (unsigned char *) malloc(num_samp*sizeof(int));
  } else if (type=='U') {
    data_buffer = (unsigned char *) malloc(num_samp*sizeof(unsigned int));
  } else {
    data_buffer=NULL;
  }

  return(data_buffer);
}
  
/***************************************************************************/
/*                                                                         */
/*    ConvertType: copy data to output buffer while converting type        */
/*           Returns error code                                            */
/*                                                                         */
/***************************************************************************/
static int RD_ConvertType(unsigned char *data_in, char in_type,
                       void *data_out, char out_type, int npts) {
  int i;

  if (out_type=='n') { /* null return type: don't return data */
    return(0);
  }
  
  if (in_type=='c') {
    if (out_type=='c') {
      for (i=0;i<npts;i++) {
        ((unsigned char*)data_out)[i]=data_in[i];
      }
    } else if (out_type=='s') {
      for (i=0;i<npts;i++) {
        ((short*)data_out)[i]=data_in[i];
      }
    } else if (out_type=='u') {
      for (i=0;i<npts;i++) {
        ((unsigned short*)data_out)[i]=data_in[i];
      }
    } else if ((out_type=='i')||(out_type=='S')) {
      for (i=0;i<npts;i++) {
        ((int*)data_out)[i]=data_in[i];
      }
    } else if (out_type=='U') {
      for (i=0;i<npts;i++) {
        ((unsigned int*)data_out)[i]=data_in[i];
      }
    } else if (out_type=='f') {
      for (i=0;i<npts;i++) {
        ((float*)data_out)[i]=data_in[i];
      }
    } else if (out_type=='d') {
      for (i=0;i<npts;i++) {
        ((double*)data_out)[i]=data_in[i];
      }
    } else {
      return (E_BAD_RETURN_TYPE);
    }
  } else if (in_type=='s') {
    if (out_type=='c') {
      for (i=0;i<npts;i++) {
        ((unsigned char*)data_out)[i]=((short *)data_in)[i];
      }
    } else if (out_type=='s') {
      for (i=0;i<npts;i++) {
        ((short*)data_out)[i]=((short *)data_in)[i];
      }
    } else if (out_type=='u') {
      for (i=0;i<npts;i++) {
        ((unsigned short*)data_out)[i]=((short *)data_in)[i];
      }
    } else if ((out_type=='i')||(out_type=='S')) {
      for (i=0;i<npts;i++) {
        ((int*)data_out)[i]=((short *)data_in)[i];
      }
    } else if (out_type=='U') {
      for (i=0;i<npts;i++) {
        ((unsigned int*)data_out)[i]=((short *)data_in)[i];
      }
    } else if (out_type=='f') {
      for (i=0;i<npts;i++) {
        ((float*)data_out)[i]=((short *)data_in)[i];
      }
    } else if (out_type=='d') {
      for (i=0;i<npts;i++) {
        ((double*)data_out)[i]=((short *)data_in)[i];
      }
    } else {
      return (E_BAD_RETURN_TYPE);
    }
  } else if (in_type=='u') {
    if (out_type=='c') {
      for (i=0;i<npts;i++) {
        ((unsigned char*)data_out)[i]=((unsigned short *)data_in)[i];
      }
    } else if (out_type=='s') {
      for (i=0;i<npts;i++) {
        ((short*)data_out)[i]=((unsigned short *)data_in)[i];
      }
    } else if (out_type=='u') {
      for (i=0;i<npts;i++) {
        ((unsigned short*)data_out)[i]=((unsigned short *)data_in)[i];
      }
    } else if ((out_type=='i')||(out_type=='S')) {
      for (i=0;i<npts;i++) {
        ((int*)data_out)[i]=((unsigned short *)data_in)[i];
      }
    } else if (out_type=='U') {
      for (i=0;i<npts;i++) {
        ((unsigned int*)data_out)[i]=((unsigned short *)data_in)[i];
      }
    } else if (out_type=='f') {
      for (i=0;i<npts;i++) {
        ((float*)data_out)[i]=((unsigned short *)data_in)[i];
      }
    } else if (out_type=='d') {
      for (i=0;i<npts;i++) {
        ((double*)data_out)[i]=((unsigned short *)data_in)[i];
      }
    } else {
      return (E_BAD_RETURN_TYPE);
    }
  } else if ((in_type=='i')||(in_type=='C')||(in_type=='S')) { /* 32 bit int */
    if (out_type=='c') {
      for (i=0;i<npts;i++) {
        ((unsigned char*)data_out)[i]=((int *)data_in)[i];
      }
    } else if (out_type=='s') {
      for (i=0;i<npts;i++) {
        ((short*)data_out)[i]=((int *)data_in)[i];
      }
    } else if (out_type=='u') {
      for (i=0;i<npts;i++) {
        ((unsigned short*)data_out)[i]=((int *)data_in)[i];
      }
    } else if ((out_type=='i')||(out_type=='S')) {
      for (i=0;i<npts;i++) {
        ((int*)data_out)[i]=((int *)data_in)[i];
      }
    } else if (out_type=='U') {
      for (i=0;i<npts;i++) {
        ((unsigned int*)data_out)[i]=((int *)data_in)[i];
      }
    } else if (out_type=='f') {
      for (i=0;i<npts;i++) {
        ((float*)data_out)[i]=((int *)data_in)[i];
      }
    } else if (out_type=='d') {
      for (i=0;i<npts;i++) {
        ((double*)data_out)[i]=((int *)data_in)[i];
      }
    } else {
      return (E_BAD_RETURN_TYPE);
    }
  } else if (in_type=='U') { /* 32 bit unsigned */
    if (out_type=='c') {
      for (i=0;i<npts;i++) {
        ((unsigned char*)data_out)[i]=((unsigned int *)data_in)[i];
      }
    } else if (out_type=='s') {
      for (i=0;i<npts;i++) {
        ((short*)data_out)[i]=((unsigned int *)data_in)[i];
      }
    } else if (out_type=='u') {
      for (i=0;i<npts;i++) {
        ((unsigned short*)data_out)[i]=((unsigned int *)data_in)[i];
      }
    } else if ((out_type=='i')||(out_type=='S')) {
      for (i=0;i<npts;i++) {
        ((int*)data_out)[i]=((unsigned int *)data_in)[i];
      }
    } else if (out_type=='U') {
      for (i=0;i<npts;i++) {
        ((unsigned int*)data_out)[i]=((unsigned int *)data_in)[i];
      }
    } else if (out_type=='f') {
      for (i=0;i<npts;i++) {
        ((float*)data_out)[i]=((unsigned int *)data_in)[i];
      }
    } else if (out_type=='d') {
      for (i=0;i<npts;i++) {
        ((double*)data_out)[i]=((unsigned int *)data_in)[i];
      }
    } else {
      return (E_BAD_RETURN_TYPE);
    }
  } else {
    return (E_BAD_RETURN_TYPE);
  }
  return(0);
  
}
  
/***************************************************************************/
/*                                                                         */
/*    ReadData: This subroutine reads data from stratoplate data files     */
/*              See 'DataStorage' file for usage documentation             */
/*                                                                         */
/***************************************************************************/
int ReadData(const char *filename_in, const char *field_code,
             int first_sframe, int first_samp,
             int num_sframes, int num_samp,
             char return_type, void *data_out,
             int *error_code) {

  static int first_time=1;
  int file_nn, i_format, i_field;
  int i_file, block_in_file, samps_read, buff_ptr;
  char done='n';
  unsigned char *data_buffer;
  int fp;
  char filename[100];

  strcpy(filename, filename_in);

  /****************************/
  /* Read the FileFormat file */
  if (first_time) {
    *error_code=RD_ReadFileFormat();
    if (*error_code!=E_OK) {
      return(0);
    }
    first_time=0;
  }

  /***************************************************************/
  /* Read the first byte of the data file to determine file type */
  *error_code=RD_GetFileFormatIndex(filename,&i_format);
  if (*error_code!=0) {
    return(0);
  }

  /*************************************/
  /* if asking for "FFINFO" return it */
  if (strcmp(field_code,"FFINFO")==0) {
    if (num_samp + num_sframes <2) return(0);
    switch (return_type) {
    case 'c':
      ((char *)data_out)[0] = (char) fstruct[i_format].block_length;
      ((char *)data_out)[1] = (char) fstruct[i_format].blocks_per_file;
      break;
    case 'u':
      ((unsigned *)data_out)[0] = (unsigned) fstruct[i_format].block_length;
      ((unsigned *)data_out)[1] = (unsigned) fstruct[i_format].blocks_per_file;
      break;
    case 's':
      ((short *)data_out)[0] = (short) fstruct[i_format].block_length;
      ((short *)data_out)[1] = (short) fstruct[i_format].blocks_per_file;
      break;
    case 'i':
    case 'S':
      ((int *)data_out)[0] = (int) fstruct[i_format].block_length;
      ((int *)data_out)[1] = (int) fstruct[i_format].blocks_per_file;
      break;
    case 'U':
      ((unsigned int *)data_out)[0] =
	(unsigned int) fstruct[i_format].block_length;
      ((unsigned int *)data_out)[1] =
	(unsigned int) fstruct[i_format].blocks_per_file;
      break;
    case 'f':
      ((float *)data_out)[0] = (float) fstruct[i_format].block_length;
      ((float *)data_out)[1] = (float) fstruct[i_format].blocks_per_file;
      break;
    case 'd':
      ((double *)data_out)[0] = (double) fstruct[i_format].block_length;
      ((double *)data_out)[1] = (double) fstruct[i_format].blocks_per_file;
      break;
    default:
      *error_code=E_BAD_CODE;
      return(0);
      break;
    }
    *error_code = E_OK;
    return(1);
  }
  
  /******************************************************/
  /* determine which field has been requested (i_field) */
  *error_code=RD_GetFieldIndex(field_code, i_format, &i_field);
  if (*error_code!=0) {
    return(0);
  }

  /* convert filenum of the filename to int (file_nn) */
  file_nn = RD_StripFileNN(filename);
  /*sscanf(filename+strlen(filename)-2,"%x",&file_nn); */
  /*************************************/
  /* reduce starting points and length */
  first_sframe+=first_samp/fstruct[i_format].num_elements[i_field];
  first_samp=first_samp%fstruct[i_format].num_elements[i_field];
  file_nn+=first_sframe/fstruct[i_format].blocks_per_file;
  first_sframe=first_sframe%fstruct[i_format].blocks_per_file;
  num_samp=num_samp+num_sframes*fstruct[i_format].num_elements[i_field];

  /************************/
  /* allocate data buffer */
  data_buffer=RD_AllocBuffer(fstruct[i_format].type[i_field], num_samp);
  if (data_buffer==NULL) {
    printf("Error in readdata allocating data_buffer\n");
    printf("(most likely due to a bad type code in /data/etc/FileFormats)\n");
    printf("code: %c\n", fstruct[i_format].type[i_field]);
    exit(0);
  }

  /********************************/
  /* initialze variables for loop */
  i_file=file_nn;
  fp = RD_OpenFile(filename,i_file);
  if (fp<0) {
    *error_code=E_FILE_OPEN;
    return(0);
  }
  block_in_file=first_sframe;
  samps_read=0;
  buff_ptr=0;

  /****************************************************/
  /* Loop: Each pass reads one sframe (block) of data */
  done='n';
  while(done=='n') {
    if (num_samp-samps_read>fstruct[i_format].num_elements[i_field] -
        first_samp) {  /* if we need to read to the end of the block */
      samps_read+=RD_ReadFromBlock(fp, block_in_file, first_samp,
                    fstruct[i_format].num_elements[i_field] - first_samp,
                    fstruct+i_format, i_field,
                    data_buffer, &buff_ptr);
      /* update indexes */
      first_samp=0;
      block_in_file++;
      /* new file if necessary */
      if (block_in_file>=fstruct[i_format].blocks_per_file) {
        block_in_file=0;
        i_file++;
        close(fp);
        fp = RD_OpenFile(filename,i_file);
        if (fp<0) {
          done='y'; /* got to the last file */
        }
      }
    } else { /* only need to read part of the block */
      samps_read+=RD_ReadFromBlock(fp, block_in_file, first_samp,
                    num_samp-samps_read,
                    fstruct+i_format, i_field,
                    data_buffer, &buff_ptr);
      close(fp);
      done='y';
    }
  }  /* End while loop */

  /* data have been read in whatever format they were written.  Now convert
     to the requested output type. */
  
  RD_ConvertType(data_buffer, fstruct[i_format].type[i_field], data_out,
              return_type, samps_read);
  
  free(data_buffer);
  return(samps_read);
}
