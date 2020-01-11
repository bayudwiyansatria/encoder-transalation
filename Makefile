# Makefile_sound  uses Linux libraries

all: pcm_min pcm_sin pcm_dat

pcm_min: pcm_min.c
	 gcc -o pcm_min pcm_min.c -l asound
	 pcm_min

pcm_sin: pcm_sin.c
	 gcc -o pcm_sin pcm_sin.c -l asound
	 pcm_sin

pcm_dat: pcm_dat.c
	 gcc -o pcm_dat pcm_dat.c -l asound
	 pcm_dat short_count.dat 10000
	 pcm_dat long_count.dat 10000

