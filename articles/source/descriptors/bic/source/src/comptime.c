#include "comptime.h"

timer *Tic(){ /* It marks the initial time */
  timer *tic=NULL;
  tic = (timer *)malloc(sizeof(timer));
  gettimeofday(tic,NULL); 
  return(tic);
}

timer *Toc(){ /* It marks the final time */
  timer *toc=NULL;
  toc = (timer *)malloc(sizeof(timer));
  gettimeofday(toc,NULL);
  return(toc);
}

float CTime(timer *tic, timer *toc) /* It computes the time difference */
{ 
  float t=0.0;
  if ((tic!=NULL)&&(toc!=NULL)){
    t = (toc->tv_sec-tic->tv_sec)*1000.0 + 
      (toc->tv_usec-tic->tv_usec)*0.001;
    free(tic);free(toc);
    tic=NULL; toc=NULL;
  }
  return(t);
}



