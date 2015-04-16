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

#ifndef BIS_H
#define BIS_H

#ifdef __cplusplus
    extern "C" {
#endif

#define BIS_OK 0
#define BIS_NOOPEN 1
#define BIS_UNKNOWN 2

extern char *BIS_ERRORSTR[];

typedef struct {
  int fp;
  char *fileName;
  int status;
  int frameSize;
  int formatType;
  int imagesPerFrame;
} BISfile;


typedef struct {
  unsigned short w;
  unsigned short h;
  unsigned short x;
  unsigned short y;
  int allocated;
  unsigned char *img;
} BISimage;

BISfile *BISopen(char *filename);
void BISclose(BISfile *bis);

int isBISfile(char *filename);

int BISnframes(BISfile *bis);

int BISreadimage(BISfile *bis, int frame, int i_img, BISimage *I);

void BISInitImage(BISimage *image);
void BISFreeImage(BISimage *image);


#ifdef __cplusplus
}
#endif

#endif
