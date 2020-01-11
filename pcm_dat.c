/* pcm_dat.c
 *  This extra small demo sends a sound data file to your speakers.
 *  The file has floating point samples in range -1.0 to +1.0
 *  You need to set sample_reate
 *  sudo apt-get install libasound2
 *  sudo apt-get install libsound2-dev
 *  get this file from web or
 *  cp  /afs/umbc.edu/users/s/q/squire/pub/download/pcm_dat.c  .
 *  gcc pcm_dat.c -l asound
 *  a.out  long_count.dat 10000 # you should hear a voice counting
 */

#include "alsa/asoundlib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char *device = "default"; /* playback device */
snd_output_t *output = NULL;
unsigned char *buffer;   /* sound playback buffer */
int nbuf;

int main(int argc, char *argv[])
{
  int err;
  int nrepeat = 1;
  unsigned int i;
  float x, y;
  float volume=1.0; /* range 0.0 to 1.0 max loud */
  int nsamp, sample_rate;
  FILE * inp;
  char file_name[100];
  int stat;
  snd_pcm_t *handle;
  snd_pcm_sframes_t frames;

  if(argc<3)
  {
    printf("sound data file and sample rate needed \n");
    exit(1);
  }
  strcpy(file_name, argv[1]);
  printf("reading sound data from %s \n", file_name);
  sample_rate=atoi(argv[2]);
  printf("at sample rate %d Hz \n", sample_rate);
  inp=fopen(file_name, "r");
  if(inp==NULL)
  {
    printf("can not open file %s for reading \n", file_name);
    exit(1);
  }
  nbuf=32*sample_rate; /* max 32 seconds played */
  buffer = (unsigned char *)malloc(nbuf);
  for(i=0; i<nbuf; i++)
  {
    /* buffer[i] = random() & 0xff;*/ /* noise */
                        /* put your sound here */
    /* x=(double)i/sample_rate; */
    /* y=sin(2.0*M_PI*x); */
    stat=fscanf(inp, "%f\n", &y);
    if(i<6) printf("sound input=%f \n",y);
    if(stat==EOF)
    {
      nbuf=i; /* end of input samples */
      break;
    } 
    buffer[i] = (int)((y+1.0)*127.0f*volume) & 0xff; /* unsigned char */ 
  }
  printf("%d sound samples read \n", nbuf);

  /* pad to multiple of sample_rate */
  nsamp=nbuf/sample_rate;
  for(i=nbuf; i<(nsamp+1)*sample_rate; i++) buffer[i]=0;
  nbuf=(nsamp+1)*sample_rate;
  printf("%d sound samples to play \n", nbuf);

  if ((err=snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
  {
    printf("Playback open error: %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);
  }
  /* many options available for parameters */
  if((err=snd_pcm_set_params(handle,
                             SND_PCM_FORMAT_U8,
                             SND_PCM_ACCESS_RW_INTERLEAVED,
                             1,
                             sample_rate,
                             1,
                             0)) < 0)
  {
    printf("Playback open error: %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);
  }
  for(i=0; i<nrepeat; i++)
  {
    frames=snd_pcm_writei(handle, buffer, nbuf);
    if(frames < 0)
      frames=snd_pcm_recover(handle, frames, 0);
    if(frames < 0)
    {
      printf("snd_pcm_writei failed: %s\n", snd_strerror(err));
      break;
    }
    if(frames > 0 && frames < (long)nbuf)
       printf("Short write (expected %li, wrote %li)\n",
              (long)sizeof(buffer), frames);
  }
  snd_pcm_close(handle);
  return 0;
} /* end pcm_dat.c */
