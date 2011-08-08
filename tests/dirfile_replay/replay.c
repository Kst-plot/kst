#include <stdio.h>
#include <stdlib.h>
#include <getdata.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>


void usage() {
  fprintf(stderr,"usage: dirfile_replay [-r <framerate>] [-p <preplay>] <source> <dest>\n"
                 "  -r <framerate>:  rate at which frames are written.  Default 5Hz\n"
		 "  -p   <preplay>:  initial replay time before timer starts\n"
	         "        <source>:  dirfile to copy from\n"
	         "          <dest>:  dirfile to copy to (erases any existing file)\n");
  exit (0);
}

void *buffer = NULL;

void process_field_entry(int i_frame, gd_entry_t *entry, 
			 DIRFILE *sourcedf, DIRFILE *destdf) {
  int n_rw;
  n_rw = gd_getdata(sourcedf, entry->field, i_frame, 0 , 1, 0, 
		      entry->data_type, buffer);
  if (n_rw != entry->spf) {
    fprintf(stderr, "read wrong number of samples (%d != %d) in %s\n",
	    n_rw, entry->spf, entry->field);
    exit(0);
  }
  
  n_rw = gd_putdata(destdf, entry->field, i_frame, 0 , 1, 0, 
		      entry->data_type, buffer);
  if (n_rw != entry->spf) {
    fprintf(stderr, "wrote wrong number of samples (%d != %d) in %s\n%s",
	    n_rw, entry->spf, entry->field, gd_error_string(destdf,NULL, 0));
    exit(0);
  }  
}

int main(int argc, char *argv[]) {
  char *source = NULL;
  char *dest = NULL;
  double rate = 5.0;
  int i_c;
  int ok_to_write = 0;
  char instr[1024];
  int i_field;
  int n_fields;
  int i_frame;
  int n_frames;
  int i_ref = 0;
  int max_spf = 0;
  gd_entry_t *entry_list;
  gd_entry_t entry;
  char *reference;
  time_t t0;
  time_t preplay = 0;
  double t;
  struct timeval tv;
  
  
  DIRFILE *sourcedf;
  DIRFILE *destdf;
  struct stat sb;
  
  for (i_c = 1; i_c < argc; i_c++) {
    if (argv[i_c][0] == '-') { // flags
      if (argv[i_c][1] == 'r') {
	i_c++;
	if (i_c < argc) {
	  rate = atof(argv[i_c]);
	} else {
	  usage(); // no argument to -r
	}
      } else if (argv[i_c][1] == 'p') {
	i_c++;
	if (i_c < argc) {
	  preplay = atoi(argv[i_c]);
	} else {
	  usage(); // no argument to -p
	}
      } else {
	usage(); // unrecognised flag
      }
    } else { // files
      if (source == NULL) {
	source = argv[i_c];
      } else if (dest == NULL) {
	dest = argv[i_c];
      } else {
	usage(); // too many parameters
      }
    }
  }
  if ((dest == NULL) || (rate <=0.0)) {
    usage();
  }	

  /* open source file */
  sourcedf = gd_open(source, GD_RDONLY);
  if (gd_error(sourcedf) != GD_E_OK) {
    fprintf(stderr, "dirfile_replay: error opening source file %s\n"
                    "%s\n", source, gd_error_string(sourcedf,NULL, 0));
    exit(0);
  } else {
    printf("opened source file %s with %ld frames\n", source, gd_nframes(sourcedf));
  }
  
  /* open dest file */
  /* see if the dest file already exists */
  errno=0;
  ok_to_write = stat(dest, &sb);
  //stat(dest, buf);
  if (ok_to_write == 0) {
    printf("The destination %s already exists! \n"
           "  Overwriting will irretrivably delete what is already there!\n"
	   "  Overwite it? (yes/no) ", dest);
    fgets(instr, 6, stdin);
    if (strncmp(instr,"yes\n",4)!=0) {
      printf("You didn't say 'yes', so we are aborting rather than deleting the data.\n");
      exit(0);
    }
  }
  
  destdf = gd_open(dest, GD_RDWR|GD_CREAT|GD_TRUNC);
  if (gd_error(destdf) != GD_E_OK) {
    fprintf(stderr, "dirfile_replay: error opening destination file %s\n"
                    "%s\n",
	    dest, gd_error_string(destdf,NULL, 0));
    exit(0);
  } else {
    printf("replaying %s into %s at %gHz\n", source, dest, rate);
  }
  
  /* create the format file */
  n_fields = gd_nfields(sourcedf);
  //field_list = gd_field_list(sourcedf);
  for (i_field = 0; i_field<n_fields; i_field++) {
    int i_meta;
    int n_meta;
    char field[300];
    strncpy(field, gd_field_list(sourcedf)[i_field], 299);
    gd_entry(sourcedf, field, &entry);
    if (gd_add(destdf, &entry)!=0) {
      fprintf(stderr, "dirfile_replay: error adding %s to %s\n"
                    "%s\n",
	    field, dest, gd_error_string(destdf,NULL, 0));
      //exit(0);
      
    }
    printf("%s %d\n", entry.field, entry.field_type);
    //n_meta = gd_nmfields(sourcedf, field);
    //for (i_meta=0; i_meta<n_meta; i_meta++) {
      //char mfield[300];
      //int error;
      //strncpy(mfield, gd_mfield_list(sourcedf,field)[i_meta], 299);
      //error = gd_entry(sourcedf, mfield, &entry);
      //printf("  %s %d\n", mfield, error);
      //gd_madd(destdf, &entry, field);
    //}
  }
  gd_flush(destdf, NULL);
  
  n_frames = gd_nframes(sourcedf);
  n_fields = gd_nfields_by_type(sourcedf,GD_RAW_ENTRY);
  
  // Create the entry list; allocate buffers.
  entry_list = (gd_entry_t *)malloc(n_fields*sizeof(gd_entry_t));
  for (i_field= 0; i_field<n_fields; i_field++) {
    char field[300];
    strncpy(field, 
	    gd_field_list_by_type(sourcedf, GD_RAW_ENTRY)[i_field], 
	    299);
    gd_entry(sourcedf, field, entry_list + i_field);
    if (entry_list[i_field].spf>max_spf) {
      max_spf = entry_list[i_field].spf;
    }
    if (strncmp(gd_reference(sourcedf, NULL), field, 299)==0) {
      i_ref = i_field;
    }
  }
  buffer = (void *)malloc(max_spf * sizeof(double));
  
  gettimeofday(&tv, NULL);
  t0 = tv.tv_sec-preplay;
  printf("started %s", ctime(&t0));
  for (i_frame = 0; i_frame < n_frames; ) {
    gettimeofday(&tv, NULL);
    t = (double)(tv.tv_sec-t0) + (double)tv.tv_usec/1.0E6;
    if (t > (double)i_frame/rate) { // we are behind
      printf("frame %d  (%.2fs)\r", i_frame, t);
      fflush(stdout);
      for (i_field = 0; i_field<n_fields; i_field++) {
        if (i_field != i_ref) {
	  process_field_entry(i_frame, entry_list+i_field, sourcedf, destdf);
        }
      }
      process_field_entry(i_frame, entry_list+i_ref, sourcedf, destdf);
      i_frame++;
    } else { // we are caught up
      gd_flush(destdf, NULL);
      usleep(10000);
    }
  }
  gd_flush(destdf, NULL);
  
  return (0);
}
