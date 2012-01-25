default:
	gcc -o out/masking-filling-stroking -g `pkg-config --libs --cflags gtk+-2.0 cairo cairo-glx  x11` masking-filling-stroking.c
