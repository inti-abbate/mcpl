
/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
//  ssvread : Code for reading ASCII SSV files (space sep. values).                //
//                                                                                 //
//                                                                                 //
//  Compilation of ssvread.c can proceed via any compliant C-compiler using        //
//  -std=c99 or later, and the resulting code must always be linked with libm      //
//  (using -lm). Furthermore, the following preprocessor flags can be used         //
//  when compiling ssvread.c to fine tune the build process and the                //
//  capabilities of the resulting binary.                                          //
//                                                                                 //
//  SSVREAD_HDR_INCPATH : Specify alternative value if the ssvread header itself   //
//                        is not to be included as "ssvread.h".                    //
//                                                                                 //
// Written 2021, osiris.abbate@ib.edu.ar (Instituto Balseiro).                     //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

#ifdef SSVREAD_HDR_INCPATH
#  include SSVREAD_HDR_INCPATH
#else
#  include "ssvread.h"
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>


//Should be large enough to hold first record in all supported files:
#define SSVREAD_MAXLINESIZE 1024

void ssv_error(const char * msg) {
  printf("ERROR: %s\n",msg);
  exit(1);
}

typedef struct {
  FILE * file;
  ssv_particle_t part;
  char line[SSVREAD_MAXLINESIZE];//for holding line from file
} ssv_fileinternal_t;

ssv_file_t ssv_open_internal( const char * filename )
{
  ssv_fileinternal_t * f = (ssv_fileinternal_t*)calloc(sizeof(ssv_fileinternal_t),1);
  assert(f);

  ssv_file_t out;
  out.internal = f;

  f->file = fopen(filename,"r");
  if (!f->file)
    ssv_error("Unable to open file!");

  // Procesar header
  if(!fgets(f->line, SSVREAD_MAXLINESIZE, f->file)) // SSV
    ssv_error("Unexpected format in SSV file");
  if(strcmp(f->line, "#MCPL-ASCII\n"))
    ssv_error("Unexpected format in SSV file");
  if(!fgets(f->line, SSVREAD_MAXLINESIZE, f->file)) // ASCII-FORMAT: v1
    ssv_error("Unexpected format in SSV file");
  if(!fgets(f->line, SSVREAD_MAXLINESIZE, f->file)) // NPARTICLES
    ssv_error("Unexpected format in SSV file");
  if(!fgets(f->line, SSVREAD_MAXLINESIZE, f->file)) // END-HEADER
    ssv_error("Unexpected format in SSV file");
  if(!fgets(f->line, SSVREAD_MAXLINESIZE, f->file)) // Variable names
    ssv_error("Unexpected format in SSV file");

  const char * bn = strrchr(filename, '/');
  bn = bn ? bn + 1 : filename;
  printf("ssv_open_file: Opened file \"%s\":\n",bn);

  return out;
}

ssv_file_t ssv_open_file( const char * filename )
{
  if (!filename)
    ssv_error("ssv_open_file called with null string for filename");

  //Open file and initialize internal:
  ssv_file_t out = ssv_open_internal( filename );
  ssv_fileinternal_t * f = (ssv_fileinternal_t *)out.internal; assert(f);

  //Return handle:
  out.internal = f;
  return out;
}

int ssv_read(const char* line, ssv_particle_t* part){
  double aux;
  int idx, uf;
  ssv_particle_t p;
  int nreaded = sscanf(line, "%i %li %g %g %g %g %g %g %g %g %g %g %g %g %x\n",
    &idx,&p.pdgcode,&p.ekin,&p.x,&p.y,&p.z,&p.dirx,&p.diry,&p.dirz,&p.time,&p.weight,&p.polx,&p.poly,&p.polz,&p.uf);
  if (nreaded == 15){
    *part = p;
    return 1;
  }
  return 0;
}

const ssv_particle_t * ssv_load_particle(ssv_file_t ff){
  ssv_fileinternal_t * f = (ssv_fileinternal_t *)ff.internal;
  assert(f);

  double ndir, ndir2;
  while(fgets(f->line, SSVREAD_MAXLINESIZE, f->file)){
    if(ssv_read(f->line, &f->part)){
      ndir2 = f->part.dirx*f->part.dirx + f->part.diry*f->part.diry + f->part.dirz*f->part.dirz;
      if(ndir2 != 1){ // Normalization may be inexact in this format
        ndir = sqrt(ndir2);
        f->part.dirx /= ndir;
        f->part.diry /= ndir;
        f->part.dirz /= ndir;
      }
      return &f->part;
    }
  }
  return 0;
}

void ssv_close_file(ssv_file_t ff){
  ssv_fileinternal_t * f = (ssv_fileinternal_t *)ff.internal;
  assert(f);

  if (!f)
    return;
  if (f->file) {
    fclose(f->file);
    f->file = 0;
  }
  free(f);
  ff.internal = 0;
}
