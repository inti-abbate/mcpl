#ifndef ssvmcpl_h
#define ssvmcpl_h

//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
// Functions for converting ASCII SSV files (space sep. values) to MCPL files.      //
//                                                                                  //
// Written 2021, osiris.abbate@ib.edu.ar, (Instituto Balseiro).                     //
//                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
// Create mcplfile based on content in ssvfile. Using this function will not
// enable double-precision or user-flags in the output file. Returns 1 on success,
// 0 on failure:
int ssv2mcpl(const char * ssvfile, const char * mcplfile);

//////////////////////////////////////////////////////////////////////////////////////
// Advanced version of the above with more options:
//
//  opt_dp  : Set to 1 to enable double-precision storage of floating point
//            values. Set to 0 for single-precision.
//  opt_gzip: Set to 1 to gzip the resulting mcpl file. Set to 0 to leave the
//            resulting file uncompressed.
//
int ssv2mcpl2(const char * ssvfile, const char * mcplfile,
              int opt_dp, int opt_gzip);

//////////////////////////////////////////////////////////////////////////////////////
// For easily creating standard ssv2mcpl cmdline applications:
int ssv2mcpl_app(int argc,char** argv);

#endif
