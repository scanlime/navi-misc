#!/bin/sh
import -window root sshot.bmp
bmptoppm sshot.bmp > sshot.ppm
./decoder
eog lsb_map.ppm
