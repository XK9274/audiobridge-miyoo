# audiobridge-miyoo

This is a custom audiobridge that will accept audio on stdin while running in background and send to /dev/dsp formatted as 44100 / 2 / S16_LE

# Issues
- Can be buggy with audio
- Speaker pops
- Audioserver must be killed before starting

# Usage 

./audiobridge & 


