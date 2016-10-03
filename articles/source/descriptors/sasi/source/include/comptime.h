#ifndef _CTIME_H_
#define _CTIME_H_

#include "common.h"

// to measure time
#ifndef _WIN32 
//UNIX
#include <sys/time.h> 
#define TIMING_START(M) timer start, now, now1;\
  gettimeofday(&start,NULL);\
  now = start;\
  fprintf(ftime,"================================================\nStarting measuring %s (in milliseconds)\n\n",M);
#define TIMING_SECTION(M) gettimeofday(&now1,NULL);\
  fprintf(ftime,"%f:\tSECTION %s\n",(now1.tv_sec-now.tv_sec)*1000.0 + (now1.tv_usec-now.tv_usec)*0.001,M);\
  now=now1;
#define TIMING_END(M) gettimeofday(&now1,NULL);\
  fprintf(ftime,"%f:\tSECTION %s\n",(now1.tv_sec-now.tv_sec)*1000.0 + (now1.tv_usec-now.tv_usec)*0.001,M);\
  fprintf(ftime,"\nTotal time: %f\n================================================\n",\
      	 (now1.tv_sec-start.tv_sec)*1000.0 + (now1.tv_usec-start.tv_usec)*0.001);
#else 
//WINDOWS
#include <windows.h>
#define TIMING_START(M)  LARGE_INTEGER _start, _now, _now1, _tic, _toc, _freq;   \
  QueryPerformanceFrequency(&_freq);                                    \
  QueryPerformanceCounter(&_start);    _now = _start;                   \
  fprintf(ftime,"================================================\nStarting measuring %s (in seconds)\n\n",M);
#define TIMING_SECTION(M)                                            \
  QueryPerformanceCounter(&_now1);                                      \
  fprintf(ftime,"%lf:\tSECTION %s\n",(long double)(_now1.LowPart - _now.LowPart)/(long double)(_freq.LowPart), M);\
 _now=_now1;
#define TIMING_TIC                                            \
  QueryPerformanceCounter(&_tic);
#define TIMING_TOC(M)                                            \
  QueryPerformanceCounter(&_toc);                                      \
  fprintf(ftime,"\t\t\t{%s: %lf}\n",M,(long double)(_toc.LowPart - _tic.LowPart)/(long double)(_freq.LowPart));
#define TIMING_END(M)                                                \
  QueryPerformanceCounter(&_now1);                                      \
  fprintf(ftime,"%lf:\tSECTION %s\n",(long double)(_now1.LowPart - _now.LowPart)/(long double)(_freq.LowPart), M);\
  fprintf(ftime,"\nTotal time: %lf\n================================================\n",                          \
                      (long double)(_now1.LowPart - _start.LowPart)/(long double)(_freq.LowPart));
#endif

//extern timer *Tic(); /* It marks the initial time */
//extern timer *Toc(); /* It marks the final time */
//extern float CTime(timer *tic, timer *toc); /* It computes the time difference */

#endif
