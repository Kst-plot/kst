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

/* ITS IMAGE HEADER FORMAT
 * ------------------------
 * [0-3]   = syncword (0xeb, 0x90, 0x14, 0x64)
 * [4]     = version
 * [5-8]   = fast frame counter (100 Hz counter from pcm)
 * [9-1]   = width [px]
 * [11-12] = height [px]
 * [13]    = number of images
 * [14]    = checksum (bitwise XOR (^) of bytes 0-13)
 * [15-]   = image data
 *
 * An individual image is formatted as:
 * Offset +
 * [0-1]   = X coordinate
 * [2-3]   = Y coordinate
 * [4-]    = raw pixel data
*/


#ifndef ITS_H
#define ITS_H

#ifdef __cplusplus
    extern "C" {
#endif

#define ITS_OK 0
#define ITS_NOOPEN 1
#define ITS_UNKNOWN 2

#define INDEX_WORD_SIZE 8

extern char *ITS_ERRORSTR[];

typedef struct {
  int fp_index;
  int fp_data;
  char *fileName;
  int status;
  int formatType;
} ITSfile;


typedef struct {
  unsigned short w;
  unsigned short h;
  unsigned short x;
  unsigned short y;
  int allocated;
  unsigned char *img;
} ITSimage;

ITSfile *ITSopen(char *filename);
void ITSclose(ITSfile *its);

int isITSfile(char *filename);

int ITSnframes(ITSfile *its);

int ITSreadimage(ITSfile *its, int frame, int i_img, ITSimage *I);

void ITSInitImage(ITSimage *image);
void ITSFreeImage(ITSimage *image);


#ifdef __cplusplus
}
#endif

#endif
