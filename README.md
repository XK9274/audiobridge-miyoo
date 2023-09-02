# audiobridge-miyoo

This is a custom audiobridge that will accept audio on stdin while running in background and send to /dev/dsp formatted as 44100 / 2 / S16_LE

Actually now piggybacks on audioserver, will accept audio from any source on stdin. 

This was created to use with apps that can pipe audio to something else, but didn't (and wasn't easy to smush in) have an OSS audio backend

# Issues
- Can be buggy with audio
- Speaker pops

# Usage 

Send audio to stdin.



