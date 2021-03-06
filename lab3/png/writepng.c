/*
 * Copyright 2002-2008 Guillaume Cottenceau, 2015 Aleksander Denisiuk
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>


#define OUT_FILE "initials.png"
#define WIDTH 600
#define HEIGHT 600
#define COLOR_TYPE PNG_COLOR_TYPE_RGB
#define BIT_DEPTH 8


void abort_(const char * s, ...)
{
va_list args;
va_start(args, s);
vfprintf(stderr, s, args);
fprintf(stderr, "\n");
va_end(args);
abort();
}

int x, y;

int width, height;
png_byte color_type;
png_byte bit_depth;

png_structp png_ptr;
png_infop info_ptr;
int number_of_passes;
png_bytep * row_pointers;

void create_png_file()
{
width = WIDTH;
height = HEIGHT;
        bit_depth = BIT_DEPTH;
        color_type = COLOR_TYPE;

row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
for (y=0; y<height; y++)
row_pointers[y] = (png_byte*) malloc(width*bit_depth*3);


}


void write_png_file(char* file_name)
{
/* create file */
FILE *fp = fopen(file_name, "wb");
if (!fp)
abort_("[write_png_file] File %s could not be opened for writing", file_name);


/* initialize stuff */
png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

if (!png_ptr)
abort_("[write_png_file] png_create_write_struct failed");

info_ptr = png_create_info_struct(png_ptr);
if (!info_ptr)
abort_("[write_png_file] png_create_info_struct failed");

if (setjmp(png_jmpbuf(png_ptr)))
abort_("[write_png_file] Error during init_io");

png_init_io(png_ptr, fp);


/* write header */
if (setjmp(png_jmpbuf(png_ptr)))
abort_("[write_png_file] Error during writing header");

png_set_IHDR(png_ptr, info_ptr, width, height,
    bit_depth, color_type, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

png_write_info(png_ptr, info_ptr);


/* write bytes */
if (setjmp(png_jmpbuf(png_ptr)))
abort_("[write_png_file] Error during writing bytes");

png_write_image(png_ptr, row_pointers);


/* end write */
if (setjmp(png_jmpbuf(png_ptr)))
abort_("[write_png_file] Error during end of write");

png_write_end(png_ptr, NULL);

        /* cleanup heap allocation */
for (y=0; y<height; y++)
free(row_pointers[y]);
free(row_pointers);

        fclose(fp);
}

void write_pixel(int x, int y, png_byte cr, png_byte cg, png_byte cb){
png_byte* row = row_pointers[y];
png_byte* ptr = &(row[x*3]);
ptr[0] = cr;
ptr[1] = cg;
ptr[2] = cb;
}

void bresenham(int i1, int j1, int i2, int j2, png_byte cr, png_byte cg, png_byte cb){
    int m,i,j,b,p;
    //II/IV quater (x/y)
    if( i2>i1 && j2>=j1 && j2-j1<=i2-i1){
        m = 2*(j2-j1);
        b=0;
        write_pixel(i1,j1,cr,cg,cb);
        j = j1;
        p = i2-i1;
        for(i=i1+1;i<=i2;i++){
            b +=m;
            if(b>p){
                j+=1;
                b-=2*p;
            }
            write_pixel(i,j,cr,cg,cb);
        }
    }

    // I/III quater (x/y)
    if(i2>i1 && j2<j1 && j2-j1<=i2-i1){
        m=2*(j2-j1);
        b=0;
        write_pixel(i1, j1, cr, cg, cb);
        j=j1;
        p=i2-i1;
        for(i=i1+1; i<=i2; i++){
            b-=m;
            if(b>p){
                j -=1;
                b -= 2*p;
            }
            write_pixel(i, j, cr, cg, cb);
        }
    }

    //vertical
    if(i2==i1 && j2>j1){
        for(j=j1;j<=j2;j++){
            write_pixel(i1,j,cr,cg,cb);
        }
    }
    //horizontal
    if(i2>i1 && j2==j1){
        for(i=i1;i<=i2;i++){
            write_pixel(i,j1,cr,cg,cb);
        }
    }
}

void circleRasterisation(int i1, int j1, int R, png_byte cr, png_byte cg, png_byte cb){
int i,j,f;
i = 0;
j = R;
f = 5 - 4*R;
write_pixel(i,j, cr, cg, cb);
while (i <= j){
    if (f > 0){
        f = f + 8*i - 8*j +20;
        j--;
    }
    else{
        f = f + 8*i + 12;
    }
    i++;
    write_pixel(i1+i,j1+j, cr, cg, cb); // (y, -x)
    write_pixel(i1-i,j1+j, cr, cg, cb); // (-y, -x)
    write_pixel(i1+i,j1-j, cr, cg, cb); // (y, x)
    write_pixel(i1-i,j1-j, cr, cg, cb); // (-y, x)
    write_pixel(j+200,i+250, cr, cg, cb); // (x, -y)
    write_pixel(450-j1+j,i1-i+50, cr, cg, cb);// (x, y)
    write_pixel(-j+200,-i+250, cr, cg, cb);// (-x, y)
    write_pixel(450-j1-j,i1+i+50, cr, cg, cb);// (-x, -y)

}
}


void process_file(void)
{
for (y=0; y<height; y++) {
png_byte* row = row_pointers[y];
for (x=0; x<width; x++) {
png_byte* ptr = &(row[x*3]);
ptr[0] = 0;
ptr[1] = ptr[2] = 255;
}
}
//M
    //left
    bresenham(150,220,150,280,0,0,0);
    bresenham(150,280,155,280,0,0,0);
    bresenham(155,240,155,280,0,0,0);
    //middle
    bresenham(150,220,170,240,255,255,255);
    bresenham(170,240,190,220,255,255,255);
    bresenham(155,240,170,255,255,255,255);
    bresenham(170,255,185,240,255,255,255);
    //right
    bresenham(190,220,190,280,0,0,0);
    bresenham(185,280,190,280,0,0,0);
    bresenham(185,240,185,280,0,0,0);


//K
    //left
    bresenham(210,220,210,280,0,0,0);
    bresenham(215,220,215,280,0,0,0);
    bresenham(210,220,215,220,0,0,0);
    bresenham(210,280,215,280,0,0,0);
    //upperLeg
    bresenham(215,245,240,215,255,0,0);
    bresenham(240,220,245,220,250,0,0);
    bresenham(215,250,245,220,250,0,0);
    //lowerLeg
    bresenham(215,250,245,280,255,0,0);
    bresenham(240,280,245,280,250,0,0);
    bresenham(215,255,240,280,250,0,0);

    circleRasterisation(200,250,106,150,0,125);

}


int main(int argc, char **argv)
{
create_png_file();
process_file();
write_png_file(OUT_FILE);

        return 0;
}
