/* pcm_sin.c
 *  This extra small demo sends a sine wave to your speakers.
 *  sudo apt-get install libasound2
 *  sudo apt-get install libsound2-dev
 *  get this file from web or
 *  cp  /afs/umbc.edu/users/s/q/squire/pub/download/pcm_sin.c  .
 *  gcc pcm_sin.c -l asound
 *  a.out  # you should hear a sine wave
 */

#include "alsa/asoundlib.h"
#include <math.h>
static char *device = "default"; /* playback device */

snd_output_t *output = NULL;
unsigned char buffer[64*1024];   /* some sound data */
int nbuf;

int main(void)
{
  int err;
  int nrepeat = 2;
  unsigned int i;
  double x, y;
  double volume=1.0; /* range 0.0 to 1.0 max loud */
  snd_pcm_t *handle;
  snd_pcm_sframes_t frames;

  nbuf=sizeof(buffer);
  for(i=0; i<nbuf; i++)
  {
    /* buffer[i] = random() & 0xff;*/ /* noise */
                         /* put your sound here */
    x=(double)i/128.0;   /* 256.0 or 512.0 lower frequency */
    y=sin(2.0*M_PI*x);
    buffer[i] = (int)((y+1.0)*127.0*volume) & 0xff; /* unsigned char */ 
  }
  if ((err=snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
  {
    printf("Playback open error: %s\n", snd_strerror(err));
    exit(EXIT_FAILURE);
  }
  /* many options available for parameters */
  if((err=snd_pcm_set_params(handle,SND_PCM_FORMAT_U8,
                              SND_PCM_ACCESS_RW_INTERLEAVED,
                              1, 48000, 1, 500000)) < 0)
  {   /* 0.5sec */
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
} /* end pcm_sin.c */
