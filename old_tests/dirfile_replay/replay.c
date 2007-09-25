/***************************************************************************
 *                                  replay.c 
 *                             -------------------
 *                begin                : Aug 22 2006
 *                copyright            : (C) 2006 by Barth Netterfield
 *                email                :
 ****************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "getdata.h"
#include "getdata_struct.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>


#define OUTDIR "/data/replay"
#define PREPLAY 1000
#define BUFSIZE 1000000
#define DT 0.2;

char *dirfile;

void usage() {
  fprintf(stderr,"usage: replay <dirfilename>\n");
  fprintf(stderr,"Replays a dirfile at a 5Hz frame rate, in %s\n", OUTDIR);
  fprintf(stderr,"This program is a hack and doesn't create %s\n", OUTDIR);
  fprintf(stderr,"so you need to do that by hand\n");
  exit(0);
}

void ProcessField(int fp, struct RawEntryType *R, int f0, int nf) {
  static char buffer[BUFSIZE];
  int error_code = 0;

  GetData(dirfile, R->field, f0, 0, nf, 0, R->type, buffer, &error_code);

  write(fp, buffer, nf*R->size*R->samples_per_frame); 
}

int main(int argc, char *argv[]) {
  int error_code = GD_E_OK;
  int N_frames;
  int i_raw, j_first;
  int *fp_out;
  char fieldfilename[200];
  char *first_field;
  char *field;
  struct FormatType *P;
  struct timeval tv;
  struct timezone zn;
  double last_t, new_t, dt;
  int nf;
  int f0;
  char copy_command[555];
  
  if (argc !=2) usage();
  if (argv[1][0]=='-') usage();

  dirfile = argv[1];
  P = GetFormat(argv[1], &error_code);
  if (error_code != GD_E_OK) {
    fprintf(stderr,"Error Reading format: %s in %s\n", GD_ERROR_CODES[error_code], argv[1]);
    exit(0);
  }

  sprintf(copy_command, "cp %s/format %s/format", argv[1], OUTDIR);
  system(copy_command);

  N_frames = GetNFrames(argv[1], &error_code, P->first_field.field);
  if (error_code != GD_E_OK) {
    fprintf(stderr,"Error: %s in %s (%s)\n", GD_ERROR_CODES[error_code], argv[1],P->first_field.field);
    exit(0);
  }

  fp_out = malloc(P->n_raw*sizeof(int));

  for (i_raw = 0; i_raw<P->n_raw; i_raw++) {
    sprintf(fieldfilename, "%s/%s", OUTDIR, P->rawEntries[i_raw].field);
    fp_out[i_raw] = open(fieldfilename, O_CREAT|O_WRONLY, 00644);
    if (fp_out[i_raw] < 0) {
      printf("could not create %s for writing\n", fieldfilename);
    } 
  }


  printf("%d frames availible\n", N_frames);

  first_field = P->first_field.field;
  
  // preplay the fields
  for (i_raw = 0; i_raw < P->n_raw; i_raw++) {
    field = P->rawEntries[i_raw].field;
    if (strcmp(field,first_field)!=0) {
      ProcessField(fp_out[i_raw], &(P->rawEntries[i_raw]), 0, PREPLAY);
    } else {
      j_first = i_raw;
    }
  }
  ProcessField(fp_out[j_first], &(P->rawEntries[j_first]), 0, PREPLAY);

  printf("preplay done\n");
  f0 = PREPLAY;

  error_code = gettimeofday(&tv, &zn);
  last_t = (double)tv.tv_sec + (double)tv.tv_usec/1.0E6;
  
  usleep(1000);
  while (f0<N_frames-2) {
    gettimeofday(&tv, &zn);
    new_t = (double)tv.tv_sec + (double)tv.tv_usec/1.0E6;
    dt = new_t - last_t;
    //printf("last_t: %lg new_t: %lg dt: %lg\n", last_t, new_t, dt); 
    nf = dt/DT;
    if (nf>0) {
      for (i_raw = 0; i_raw < P->n_raw; i_raw++) {
        field = P->rawEntries[i_raw].field;
        if (strcmp(field,first_field)!=0) {
          ProcessField(fp_out[i_raw], &(P->rawEntries[i_raw]), f0, nf);
        } else {
          j_first = i_raw;
        }
      }
      ProcessField(fp_out[j_first], &(P->rawEntries[j_first]), f0, nf);
      f0 += nf;
      last_t+=nf*DT;
      printf("%d\n", f0);
    } else {
      usleep(1000);
    }
  }
}

