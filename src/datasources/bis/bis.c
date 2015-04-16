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
  int bytes;
  
  bytes = lseek(bis->fp,0,SEEK_END);
  
  if (bytes<0) bytes = 0;
  
  if (bis->frameSize >0) {
    //return (bytes);
    return ((bytes-4)/bis->frameSize);
  } else {
    return 0;
  }
}

int BISreadimage(BISfile *bis, int frame, int i_img, BISimage *I) {
  int nframes;
  unsigned short us_in[5];
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
  lseek(bis->fp, frame*bis->frameSize+4, SEEK_SET);
  nr = read(bis->fp, &us_in, 10); // read offsets
  if ((nr!=10) || (us_in[i_img] <1)) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }
  
  // Read the image size and position data
  lseek(bis->fp, frame*bis->frameSize+4+us_in[i_img], SEEK_SET);
  nr = read(bis->fp, &us_in, 8); // read offsets
  if (nr!=8) {
    I->w = I->h = I->x = I->y = 0;
    return 0;
  }
  I->w = us_in[0];
  I->h = us_in[1];
  I->x = us_in[2];
  I->y = us_in[3];
  
  // read the image
  img_size = I->w * I->h;
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

