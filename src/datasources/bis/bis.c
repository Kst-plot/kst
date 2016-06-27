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

#include "bis.h"

char *BIS_ERRORSTR[] = {"OK", "Could not open file", "Unknown file format"};


BISfile *BISopen(char *filename) {
  BISfile *bis;
  int nr;
  unsigned short us_in;
  
  bis = (BISfile*) malloc(sizeof(BISfile));
  
  bis->status = BIS_OK;
  
  bis->fileName = malloc (strlen(filename)+1);
  
  strcpy(bis->fileName,filename);
  
  bis->fp = open(filename, O_RDONLY);
  
  if (bis->fp<0) {
    bis->status = BIS_NOOPEN;
    return (bis);
  }
  
  nr = read(bis->fp, &us_in,2);
  bis->formatType = us_in;
  nr += read(bis->fp, &us_in,2);
  bis->frameSize = us_in;
  
  switch (bis->formatType) {
    case 0xe6b0:
      bis->imagesPerFrame = 5;
      break;
    default:
      bis->status = BIS_UNKNOWN;
      break;
  }
  
  return (bis);
}


void BISclose(BISfile *bis) {
  if ((bis->fp>0) && (bis->status!=BIS_NOOPEN)) {
    close(bis->fp);
  }
  free(bis->fileName);
  free(bis);
}


/* initialize the image into a empty image, ready for use */
/* this is essentially a constructor, and should be called on */
/* any new bis image before use! */
void BISInitImage(BISimage *image) {
  image->w = image->h = image->x = image->y = 0;
  image->allocated = 0;
  image->img = NULL;
}


/* free any memory allocated to the bis image */
/* note: the image is still valid and can be used again */
/* without calling BISInitImage */
void BISFreeImage(BISimage *image) {
  if (image->img) {
    free(image->img);
  }
  BISInitImage(image);
}


int isBISfile(char *filename) {
  BISfile *bis;
  int is_bis;
  
  bis = BISopen(filename);
  is_bis = (bis->status == BIS_OK);
  BISclose(bis);
  
  return (is_bis);
}

int BISnframes(BISfile *bis) {
  off_t bytes;
  
  bytes = lseek(bis->fp,0,SEEK_END);
  
  if (bytes<0) bytes = 0L;
  
  if (bis->frameSize >0L) {
    //return (bytes);
    return ((bytes-4L)/bis->frameSize);
  } else {
    return 0;
  }
}

int BISreadimage(BISfile *bis, int frame, int i_img, BISimage *I) {
  int nframes;
  //unsigned short us_in[5];
  unsigned short image_offsets[5];
  unsigned short image_dim[4];
  int nr;
  int img_size;
  
  nframes = BISnframes(bis);
  if (frame < 0) { // last frame
    frame = nframes - 1;
  }
  
  if ((frame >= nframes) || (nframes<1)) { // can't read past end;
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }
  
  if (i_img >= bis->imagesPerFrame) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }
  
  // read the image offsets
  off_t offset = (off_t)frame * (off_t)bis->frameSize + 4L;
  //lseek(bis->fp, frame*bis->frameSize+4, SEEK_SET);
  lseek(bis->fp, offset, SEEK_SET);
  nr = read(bis->fp, &image_offsets, 10); // read offsets
  if ((nr!=10) || (image_offsets[i_img] <1)) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }
  
  // Sanity check: offset points to the beginning of the frame
  // image_offsets[i_img] + offset points to the start of the image.
  // so image_offsets[i_img] had better be smaller than frameSize - 8 for
  // a 0 size image.  FIXME: tighter constraint?
  if (image_offsets[i_img] > bis->frameSize- 8) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }

  // Read the image size and position data
  //lseek(bis->fp, frame*bis->frameSize+4+us_in[i_img], SEEK_SET);
  lseek(bis->fp, offset+(off_t)image_offsets[i_img], SEEK_SET);
  nr = read(bis->fp, &image_dim, 8); // read image dimensions
  if (nr!=8) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }
  I->w = image_dim[0];
  I->h = image_dim[1];
  I->x = image_dim[2];
  I->y = image_dim[3];

  if ((I->w < 1) || (I->h < 1)) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }

  // read the image
  img_size = I->w * I->h;

  // Sanity Check:
  // image_size + image_offsets[i_image] + 8 had better be smaller than
  // frameSize in order to fit in the frame.
  if (image_offsets[i_img] + img_size > bis->frameSize- 8) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }

  if (img_size > I->allocated) {
    I->img = realloc(I->img, img_size+1);
    I->allocated = img_size;
  }
  
  nr = read(bis->fp, I->img, img_size); // read image
  if (nr != img_size) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }
    
  return 1;
}

