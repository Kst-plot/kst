/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2015 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include "its.h"

char *ITS_ERRORSTR[] = {"OK", "Could not open file", "Unknown file format"};


ITSfile *ITSopen(char *filename) {
  ITSfile *its;
  char *index_filename;
  
  its = (ITSfile*) malloc(sizeof(ITSfile));
  
  its->status = ITS_OK;
  
  its->fileName = malloc (strlen(filename)+1);
  index_filename = malloc (strlen(filename)+5);

  sprintf(index_filename, "%s.its", filename);
  strcpy(its->fileName,filename);
    
  its->fp_data = open(filename, O_RDONLY);
  if (its->fp_data<0) {
    its->status = ITS_NOOPEN;
    return (its);
  }

  its->fp_index = open(index_filename, O_RDONLY);
  if (its->fp_index<0) {
    its->status = ITS_NOOPEN;
    return (its);
  }

  return (its);
}


void ITSclose(ITSfile *its) {
  if (its->status != ITS_NOOPEN) {
    if (its->fp_index > 0) {
      close(its->fp_index);
    }
    if (its->fp_data > 0) {
      close(its->fp_data);
    }
  }

  free(its->fileName);
  free(its);
}


/* initialize the image into a empty image, ready for use */
/* this is essentially a constructor, and should be called on */
/* any new its image before use! */
void ITSInitImage(ITSimage *image) {
  image->w = image->h = image->x = image->y = 0;
  image->allocated = 0;
  image->img = NULL;
}


/* free any memory allocated to the its image */
/* note: the image is still valid and can be used again */
/* without calling ITSInitImage */
void ITSFreeImage(ITSimage *image) {
  if (image->img) {
    free(image->img);
  }
  ITSInitImage(image);
}


int isITSfile(char *filename) {
  ITSfile *its;
  int is_its;
  
  its = ITSopen(filename);
  is_its = (its->status == ITS_OK);
  ITSclose(its);
  
  return (is_its);
}

int checkHeader(unsigned char *h) {
  unsigned char sw[] = {0xeb, 0x90, 0x14, 0x6f, 0x00};
  unsigned char crc = 0;
  int i;

  for (i=0; i<5; i++) {
    if (h[i] != sw[i]) {
      fprintf(stderr, "bad byte %d in checkHeader\n", i);
      return (0);
    }
  }
  for (i=0; i<14; i++) {
    crc ^= h[i];
  }
  if (crc != h[14]) {
    fprintf(stderr, "bad checksum in header\n");
    return 0;
  }

  return(1);
}

// how many frames in the file?  Check the length of the index.
int ITSnframes(ITSfile *its) {
  off_t bytes;
  
  bytes = lseek(its->fp_index,0,SEEK_END);
  
  if (bytes<0) bytes = 0L;

  return (bytes/INDEX_WORD_SIZE);
}

int ITSreadimage(ITSfile *its, int frame, int i_img, ITSimage *I) {
  int nframes;
  unsigned offset;
  int nr;
  uint64_t index;
  unsigned char buf_in[1024];
  unsigned short w, h;
  unsigned char ni;
  int img_size;

  nframes = ITSnframes(its);
  if (frame < 0) { // last frame
    frame = nframes - 1;
  }
  
  if ((frame >= nframes) || (nframes<1)) { // can't read past end;
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }

  // First read the index file to find the index.
  offset = lseek(its->fp_index,frame*INDEX_WORD_SIZE, SEEK_SET);
  if (offset != frame*INDEX_WORD_SIZE) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }

  nr = read(its->fp_index, &index, INDEX_WORD_SIZE);
  if (nr != INDEX_WORD_SIZE) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }

  // FIXME: INDEX is wrong! (?)
  //index-=3;

  // Second: read the header in the data file
  offset = lseek(its->fp_data, index, SEEK_SET);
  if (offset != index) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }

  nr = read(its->fp_data, buf_in, 15);
  if (nr != 15) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }

  if (!checkHeader(buf_in)) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }

  w = *((unsigned short *)(buf_in+9));
  h = *((unsigned short *)(buf_in+11));
  ni = *((unsigned char *)(buf_in+13));

  if (i_img >= ni) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }

  img_size = w * h;
  if (I->allocated < img_size) {
    I->img = realloc(I->img, img_size+1);
    I->allocated = img_size;
  }

  // Now read in the actual image
  offset = lseek(its->fp_data, i_img*(img_size + 4), SEEK_CUR);
  nr  = read(its->fp_data, &(I->x), 2);
  nr += read(its->fp_data, &(I->y), 2);
  nr += read(its->fp_data, I->img, img_size);

  if (nr != img_size + 4) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }

  I->w = w;
  I->h = h;

  return 1;
}

