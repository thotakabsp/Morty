/*
 * test.c
 *
 * Author: Tomi Valkeinen <tomi.valkeinen@nokia.com>
 * Copyright (C) 2009-2012 Tomi Valkeinen

 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <linux/fb.h>
#include <linux/kd.h>

#include "common.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <unistd.h>
#include <string.h>
#include "fbv.h"

unsigned short red[256], green[256], blue[256];
struct fb_cmap map332 = {0, 256, red, green, blue, NULL};
unsigned short red_b[256], green_b[256], blue_b[256];
struct fb_cmap map_back = {0, 256, red_b, green_b, blue_b, NULL};


int openFB(const char *name);
void closeFB(int fh);
void getVarScreenInfo(int fh, struct fb_var_screeninfo *var);
void setVarScreenInfo(int fh, struct fb_var_screeninfo *var);
void getFixScreenInfo(int fh, struct fb_fix_screeninfo *fix);
void set332map(int fh);
void* convertRGB2FB(int fh, unsigned char *rgbbuff, unsigned long count, int bpp, int *cpp);
void blit2FB(int fh, void *fbbuff, unsigned char *alpha,
		unsigned int pic_xs, unsigned int pic_ys,
		unsigned int scr_xs, unsigned int scr_ys,
		unsigned int xp, unsigned int yp,
		unsigned int xoffs, unsigned int yoffs,
		int cpp);

void fb_display(unsigned char *rgbbuff, unsigned char * alpha, int x_size, int y_size, int x_pan, int y_pan, int x_offs, int y_offs)
{
	struct fb_var_screeninfo var;
	struct fb_fix_screeninfo fix;
	unsigned short *fbbuff = NULL;
	int fh = -1, bp = 0;
	unsigned long x_stride;

	/* get the framebuffer device handle */
	fh = openFB("/dev/fb0");

	/* read current video mode */
	getVarScreenInfo(fh, &var);
	getFixScreenInfo(fh, &fix);

	x_stride = (fix.line_length * 8) / var.bits_per_pixel;

	/* correct panning */
	if(x_pan > x_size - x_stride) x_pan = 0;
	if(y_pan > y_size - var.yres) y_pan = 0;
	/* correct offset */
	if(x_offs + x_size > x_stride) x_offs = 0;
	if(y_offs + y_size > var.yres) y_offs = 0;

	/* blit buffer 2 fb */
	fbbuff = convertRGB2FB(fh, rgbbuff, x_size * y_size, var.bits_per_pixel, &bp);
#if 0
	blit2FB(fh, fbbuff, alpha, x_size, y_size, x_stride, var.yres, x_pan, y_pan, x_offs, y_offs, bp);
#else
	blit2FB(fh, fbbuff, alpha, x_size, y_size, x_stride, var.yres_virtual, x_pan, y_pan, x_offs, y_offs + var.yoffset, bp);
#endif
	free(fbbuff);

	/* close device */
	closeFB(fh);
}

void getCurrentRes(int *x, int *y)
{
	struct fb_var_screeninfo var;
	int fh = -1;
	fh = openFB("/dev/fb0");
	getVarScreenInfo(fh, &var);
	*x = var.xres;
	*y = var.yres;
	closeFB(fh);
}

int openFB(const char *name)
{
	int fh;
	char *dev;

	if ((fh = open(name, O_RDWR)) == -1){
		fprintf(stderr, "open %s: %s\n", name, strerror(errno));
		exit(1);
	}
	return fh;
}

void closeFB(int fh)
{
	close(fh);
}

void getVarScreenInfo(int fh, struct fb_var_screeninfo *var)
{
	if (ioctl(fh, FBIOGET_VSCREENINFO, var)){
		fprintf(stderr, "ioctl FBIOGET_VSCREENINFO: %s\n", strerror(errno));
		exit(1);
	}
}

void setVarScreenInfo(int fh, struct fb_var_screeninfo *var)
{
	if (ioctl(fh, FBIOPUT_VSCREENINFO, var)){
		fprintf(stderr, "ioctl FBIOPUT_VSCREENINFO: %s\n", strerror(errno));
		exit(1);
	}
}

void getFixScreenInfo(int fh, struct fb_fix_screeninfo *fix)
{
	if (ioctl(fh, FBIOGET_FSCREENINFO, fix)){
		fprintf(stderr, "ioctl FBIOGET_FSCREENINFO: %s\n", strerror(errno));
		exit(1);
	}
}

void make332map(struct fb_cmap *map)
{
	int rs, gs, bs, i;
	int r = 8, g = 8, b = 4;

	map->red = red;
	map->green = green;
	map->blue = blue;

	rs = 256 / (r - 1);
	gs = 256 / (g - 1);
	bs = 256 / (b - 1);

	for (i = 0; i < 256; i++) {
		map->red[i]   = (rs * ((i / (g * b)) % r)) * 255;
		map->green[i] = (gs * ((i / b) % g)) * 255;
		map->blue[i]  = (bs * ((i) % b)) * 255;
	}
}

void set8map(int fh, struct fb_cmap *map)
{
	if (ioctl(fh, FBIOPUTCMAP, map) < 0) {
		fprintf(stderr, "Error putting colormap");
		exit(1);
	}
}

void get8map(int fh, struct fb_cmap *map)
{
	if (ioctl(fh, FBIOGETCMAP, map) < 0) {
		fprintf(stderr, "Error getting colormap");
		exit(1);
	}
}

void set332map(int fh)
{
	make332map(&map332);
	set8map(fh, &map332);
}

void blit2FB(int fh, void *fbbuff, unsigned char *alpha,
		unsigned int pic_xs, unsigned int pic_ys,
		unsigned int scr_xs, unsigned int scr_ys,
		unsigned int xp, unsigned int yp,
		unsigned int xoffs, unsigned int yoffs,
		int cpp)
{
	int i, xc, yc;
	unsigned char *fb;

	unsigned char *fbptr;
	unsigned char *imptr;

	xc = (pic_xs > scr_xs) ? scr_xs : pic_xs;
	yc = (pic_ys > scr_ys) ? scr_ys : pic_ys;

	fb = mmap(NULL, scr_xs * scr_ys * cpp, PROT_WRITE | PROT_READ, MAP_SHARED, fh, 0);

	if(fb == MAP_FAILED)
	{
		perror("mmap");
		return;
	}

	if(cpp == 1)
	{
		get8map(fh, &map_back);
		set332map(fh);
	}

	fbptr = fb     + (yoffs * scr_xs + xoffs) * cpp;
	imptr = fbbuff + (yp	* pic_xs + xp) * cpp;

	if(alpha)
	{
		unsigned char * alphaptr;
		int from, to, x;

		alphaptr = alpha + (yp	* pic_xs + xp);

		for(i = 0; i < yc; i++, fbptr += scr_xs * cpp, imptr += pic_xs * cpp, alphaptr += pic_xs)
		{
			for(x = 0; x<xc; x++)
			{
				int v;

				from = to = -1;
				for(v = x; v<xc; v++)
				{
					if(from == -1)
					{
						if(alphaptr[v] > 0x80) from = v;
					}
					else
					{
						if(alphaptr[v] < 0x80)
						{
							to = v;
							break;
						}
					}
				}
				if(from == -1)
					break;

				if(to == -1) to = xc;

				memcpy(fbptr + (from * cpp), imptr + (from * cpp), (to - from - 1) * cpp);
				x += to - from - 1;
			}
		}
	}
	else
		for(i = 0; i < yc; i++, fbptr += scr_xs * cpp, imptr += pic_xs * cpp)
		{
			usleep(1000);
			memcpy(fbptr, imptr, xc * cpp);
		}

	if(cpp == 1)
		set8map(fh, &map_back);

	munmap(fb, scr_xs * scr_ys * cpp);
}

inline static unsigned char make8color(unsigned char r, unsigned char g, unsigned char b)
{
	return (
			(((r >> 5) & 7) << 5) |
			(((g >> 5) & 7) << 2) |
			((b >> 6) & 3)       );
}

inline static unsigned short make15color(unsigned char r, unsigned char g, unsigned char b)
{
	return (
			(((r >> 3) & 31) << 10) |
			(((g >> 3) & 31) << 5)  |
			((b >> 3) & 31)        );
}

inline static unsigned short make16color(unsigned char r, unsigned char g, unsigned char b)
{
	return (
			(((r >> 3) & 31) << 11) |
			(((g >> 2) & 63) << 5)  |
			((b >> 3) & 31)        );
}

void* convertRGB2FB(int fh, unsigned char *rgbbuff, unsigned long count, int bpp, int *cpp)
{
	unsigned long i;
	void *fbbuff = NULL;
	u_int8_t  *c_fbbuff;
	u_int16_t *s_fbbuff;
	u_int32_t *i_fbbuff;

	switch(bpp)
	{
		case 8:
			*cpp = 1;
			c_fbbuff = (unsigned char *) malloc(count * sizeof(unsigned char));
			for(i = 0; i < count; i++)
				c_fbbuff[i] = make8color(rgbbuff[i*3], rgbbuff[i*3+1], rgbbuff[i*3+2]);
			fbbuff = (void *) c_fbbuff;
			break;
		case 15:
			*cpp = 2;
			s_fbbuff = (unsigned short *) malloc(count * sizeof(unsigned short));
			for(i = 0; i < count ; i++)
				s_fbbuff[i] = make15color(rgbbuff[i*3], rgbbuff[i*3+1], rgbbuff[i*3+2]);
			fbbuff = (void *) s_fbbuff;
			break;
		case 16:
			*cpp = 2;
			s_fbbuff = (unsigned short *) malloc(count * sizeof(unsigned short));
			for(i = 0; i < count ; i++)
				s_fbbuff[i] = make16color(rgbbuff[i*3], rgbbuff[i*3+1], rgbbuff[i*3+2]);
			fbbuff = (void *) s_fbbuff;
			break;
		case 24:
		case 32:
			*cpp = 4;
			i_fbbuff = (unsigned int *) malloc(count * sizeof(unsigned int));
			for(i = 0; i < count ; i++)
				i_fbbuff[i] = ((rgbbuff[i*3] << 16) & 0xFF0000) |
					((rgbbuff[i*3+1] << 8) & 0xFF00) |
					(rgbbuff[i*3+2] & 0xFF);
			fbbuff = (void *) i_fbbuff;
			break;
		default:
			fprintf(stderr, "Unsupported video mode! You've got: %dbpp\n", bpp);
			exit(1);
	}
	return fbbuff;
}


unsigned char * simple_resize(unsigned char * orgin,int ox,int oy,int dx,int dy)
{
	unsigned char *cr,*p,*l;
	int i,j,k,ip;
	assert(cr = (unsigned char*) malloc(dx*dy*3));
	l=cr;

	for(j=0;j<dy;j++,l+=dx*3)
	{
		p=orgin+(j*oy/dy*ox*3);
		for(i=0,k=0;i<dx;i++,k+=3)
		{
			ip=i*ox/dx*3;
			l[k]=p[ip];
			l[k+1]=p[ip+1];
			l[k+2]=p[ip+2];
		}
	}
	return(cr);
}

unsigned char * alpha_resize(unsigned char * alpha,int ox,int oy,int dx,int dy)
{
	unsigned char *cr,*p,*l;
	int i,j,k;
	cr=(unsigned char*) malloc(dx*dy); l=cr;

	for(j=0;j<dy;j++,l+=dx)
	{
		p = alpha+(j*oy/dy*ox);
		for(i=0,k=0;i<dx;i++)
			l[k++]=p[i*ox/dx];
	}

	return(cr);
}

unsigned char * color_average_resize(unsigned char * orgin,int ox,int oy,int dx,int dy)
{
	unsigned char *cr,*p,*q;
	int i,j,k,l,xa,xb,ya,yb;
	int sq,r,g,b;
	assert(cr=(unsigned char*) malloc(dx*dy*3)); p=cr;

	for(j=0;j<dy;j++)
	{
		for(i=0;i<dx;i++,p+=3)
		{
			xa=i*ox/dx;
			ya=j*oy/dy;
			xb=(i+1)*ox/dx; if(xb>=ox) xb=ox-1;
			yb=(j+1)*oy/dy; if(yb>=oy) yb=oy-1;
			for(l=ya,r=0,g=0,b=0,sq=0;l<=yb;l++)
			{
				q=orgin+((l*ox+xa)*3);
				for(k=xa;k<=xb;k++,q+=3,sq++)
				{
					r+=q[0]; g+=q[1]; b+=q[2];
				}
			}
			p[0]=r/sq; p[1]=g/sq; p[2]=b/sq;
		}
	}
	return(cr);
}

unsigned char * rotate(unsigned char *i, int ox, int oy, int rot)
{
	unsigned char * n, * p;
	int x, y;
	assert(n = (unsigned char*) malloc(ox * oy * 3));

	switch(rot)
	{
		case 1: /* 90 deg right */
			p = n + (oy - 1) * 3;
			for(y = 0; y<oy; y++, p -= 3)
			{
				unsigned char * r = p;
				for(x = 0; x<ox; x++, r += oy * 3)
				{
					r[0] = *(i++);
					r[1] = *(i++);
					r[2] = *(i++);
				}
			}
			break;
		case 2: /* 180 deg */
			i += ox * oy * 3; p = n;
			for(y = ox * oy; y > 0; y--)
			{
				i -= 3;
				p[0] = i[0];
				p[1] = i[1];
				p[2] = i[2];
				p += 3;
			}
			break;
		case 3: /* 90 deg left */
			p = n;
			for(y = 0; y<oy; y++, p += 3)
			{
				unsigned char * r = p + ((ox * 3) * oy);
				for(x = 0; x<ox; x++)
				{
					r -= oy * 3;
					r[0] = *(i++);
					r[1] = *(i++);
					r[2] = *(i++);
				}
			}
			break;
	}
	return(n);
}

unsigned char * alpha_rotate(unsigned char *i, int ox, int oy, int rot)
{
	unsigned char * n, * p;
	int x, y;
	assert(n = (unsigned char*) malloc(ox * oy));

	switch(rot)
	{
		case 1: /* 90 deg right */
			p = n + (oy - 1);
			for(y = 0; y<oy; y++, p --)
			{
				unsigned char * r = p;
				for(x = 0; x<ox; x++, r += oy)
					*r = *(i++);
			}
			break;
		case 2: /* 180 deg */
			i += ox * oy; p = n;
			for(y = ox * oy; y > 0; y--)
				*(p++) = *(i--);
			break;
		case 3: /* 90 deg left */
			p = n;
			for(y = 0; y<oy; y++, p++)
			{
				unsigned char * r = p + (ox * oy);
				for(x = 0; x<ox; x++)
				{
					r -= oy;
					*r = *(i++);
				}
			}
			break;
	}
	return(n);
}
static struct fb_info fb_info;

struct r_jpeg_error_mgr
{
	struct jpeg_error_mgr pub;
	jmp_buf envbuffer;
};


int fh_jpeg_id(char *name)
{
	int fd;
	unsigned char id[10];
	fd=open(name,O_RDONLY); if(fd==-1) return(0);
	read(fd,id,10);
	close(fd);
	if(id[6]=='J' && id[7]=='F' && id[8]=='I' && id[9]=='F') return(1);
	if(id[0]==0xff && id[1]==0xd8 && id[2]==0xff) return(1);
	return(0);
}


void jpeg_cb_error_exit(j_common_ptr cinfo)
{
	struct r_jpeg_error_mgr *mptr;
	mptr=(struct r_jpeg_error_mgr*) cinfo->err;
	(*cinfo->err->output_message) (cinfo);
	longjmp(mptr->envbuffer,1);
}

int fh_jpeg_load(char *filename,unsigned char *buffer, unsigned char ** alpha, int x,int y)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_decompress_struct *ciptr;
	struct r_jpeg_error_mgr emgr;
	unsigned char *bp;
	int px,py,c;
	FILE *fh;
	JSAMPLE *lb;

	ciptr=&cinfo;
	if(!(fh=fopen(filename,"rb"))) return(FH_ERROR_FILE);
	ciptr->err=jpeg_std_error(&emgr.pub);
	emgr.pub.error_exit=jpeg_cb_error_exit;
	if(setjmp(emgr.envbuffer)==1)
	{
		// FATAL ERROR - Free the object and return...
		jpeg_destroy_decompress(ciptr);
		fclose(fh);
		return(FH_ERROR_FORMAT);
	}

	jpeg_create_decompress(ciptr);
	jpeg_stdio_src(ciptr,fh);
	jpeg_read_header(ciptr,TRUE);
	ciptr->out_color_space=JCS_RGB;
	jpeg_start_decompress(ciptr);

	px=ciptr->output_width; py=ciptr->output_height;
	c=ciptr->output_components;


	if(c==3)
	{
		lb=(*ciptr->mem->alloc_small)((j_common_ptr) ciptr,JPOOL_PERMANENT,c*px);
		bp=buffer;
		while (ciptr->output_scanline < ciptr->output_height)
		{
			jpeg_read_scanlines(ciptr, &lb, 1);
			memcpy(bp,lb,px*c);
			bp+=px*c;
		}					   

	}
	jpeg_finish_decompress(ciptr);
	jpeg_destroy_decompress(ciptr);
	fclose(fh);
	return(FH_ERROR_OK);
}

int fh_jpeg_getsize(char *filename,int *x,int *y)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_decompress_struct *ciptr;
	struct r_jpeg_error_mgr emgr;
	int px,py,c;
	FILE *fh;

	ciptr=&cinfo;
	if(!(fh=fopen(filename,"rb"))) return(FH_ERROR_FILE);

	ciptr->err=jpeg_std_error(&emgr.pub);
	emgr.pub.error_exit=jpeg_cb_error_exit;
	if(setjmp(emgr.envbuffer)==1)
	{
		// FATAL ERROR - Free the object and return...
		jpeg_destroy_decompress(ciptr);
		fclose(fh);
		return(FH_ERROR_FORMAT);
	}

	jpeg_create_decompress(ciptr);
	jpeg_stdio_src(ciptr,fh);
	jpeg_read_header(ciptr,TRUE);
	ciptr->out_color_space=JCS_RGB;
	jpeg_start_decompress(ciptr);
	px=ciptr->output_width; py=ciptr->output_height;
	c=ciptr->output_components;
	*x=px; *y=py;
	jpeg_destroy_decompress(ciptr);
	fclose(fh);
	return(FH_ERROR_OK);
}
char fontdata_8x8[] = {

	/* 0 0x00 '^@' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 1 0x01 '^A' */
	0x7e, /* 01111110 */
	0x81, /* 10000001 */
	0xa5, /* 10100101 */
	0x81, /* 10000001 */
	0xbd, /* 10111101 */
	0x99, /* 10011001 */
	0x81, /* 10000001 */
	0x7e, /* 01111110 */

	/* 2 0x02 '^B' */
	0x7e, /* 01111110 */
	0xff, /* 11111111 */
	0xdb, /* 11011011 */
	0xff, /* 11111111 */
	0xc3, /* 11000011 */
	0xe7, /* 11100111 */
	0xff, /* 11111111 */
	0x7e, /* 01111110 */

	/* 3 0x03 '^C' */
	0x6c, /* 01101100 */
	0xfe, /* 11111110 */
	0xfe, /* 11111110 */
	0xfe, /* 11111110 */
	0x7c, /* 01111100 */
	0x38, /* 00111000 */
	0x10, /* 00010000 */
	0x00, /* 00000000 */

	/* 4 0x04 '^D' */
	0x10, /* 00010000 */
	0x38, /* 00111000 */
	0x7c, /* 01111100 */
	0xfe, /* 11111110 */
	0x7c, /* 01111100 */
	0x38, /* 00111000 */
	0x10, /* 00010000 */
	0x00, /* 00000000 */

	/* 5 0x05 '^E' */
	0x38, /* 00111000 */
	0x7c, /* 01111100 */
	0x38, /* 00111000 */
	0xfe, /* 11111110 */
	0xfe, /* 11111110 */
	0xd6, /* 11010110 */
	0x10, /* 00010000 */
	0x38, /* 00111000 */

	/* 6 0x06 '^F' */
	0x10, /* 00010000 */
	0x38, /* 00111000 */
	0x7c, /* 01111100 */
	0xfe, /* 11111110 */
	0xfe, /* 11111110 */
	0x7c, /* 01111100 */
	0x10, /* 00010000 */
	0x38, /* 00111000 */

	/* 7 0x07 '^G' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 8 0x08 '^H' */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xe7, /* 11100111 */
	0xc3, /* 11000011 */
	0xc3, /* 11000011 */
	0xe7, /* 11100111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */

	/* 9 0x09 '^I' */
	0x00, /* 00000000 */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0x42, /* 01000010 */
	0x42, /* 01000010 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 10 0x0a '^J' */
	0xff, /* 11111111 */
	0xc3, /* 11000011 */
	0x99, /* 10011001 */
	0xbd, /* 10111101 */
	0xbd, /* 10111101 */
	0x99, /* 10011001 */
	0xc3, /* 11000011 */
	0xff, /* 11111111 */

	/* 11 0x0b '^K' */
	0x0f, /* 00001111 */
	0x07, /* 00000111 */
	0x0f, /* 00001111 */
	0x7d, /* 01111101 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x78, /* 01111000 */

	/* 12 0x0c '^L' */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */

	/* 13 0x0d '^M' */
	0x3f, /* 00111111 */
	0x33, /* 00110011 */
	0x3f, /* 00111111 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x70, /* 01110000 */
	0xf0, /* 11110000 */
	0xe0, /* 11100000 */

	/* 14 0x0e '^N' */
	0x7f, /* 01111111 */
	0x63, /* 01100011 */
	0x7f, /* 01111111 */
	0x63, /* 01100011 */
	0x63, /* 01100011 */
	0x67, /* 01100111 */
	0xe6, /* 11100110 */
	0xc0, /* 11000000 */

	/* 15 0x0f '^O' */
	0x18, /* 00011000 */
	0xdb, /* 11011011 */
	0x3c, /* 00111100 */
	0xe7, /* 11100111 */
	0xe7, /* 11100111 */
	0x3c, /* 00111100 */
	0xdb, /* 11011011 */
	0x18, /* 00011000 */

	/* 16 0x10 '^P' */
	0x80, /* 10000000 */
	0xe0, /* 11100000 */
	0xf8, /* 11111000 */
	0xfe, /* 11111110 */
	0xf8, /* 11111000 */
	0xe0, /* 11100000 */
	0x80, /* 10000000 */
	0x00, /* 00000000 */

	/* 17 0x11 '^Q' */
	0x02, /* 00000010 */
	0x0e, /* 00001110 */
	0x3e, /* 00111110 */
	0xfe, /* 11111110 */
	0x3e, /* 00111110 */
	0x0e, /* 00001110 */
	0x02, /* 00000010 */
	0x00, /* 00000000 */

	/* 18 0x12 '^R' */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */

	/* 19 0x13 '^S' */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x00, /* 00000000 */
	0x66, /* 01100110 */
	0x00, /* 00000000 */

	/* 20 0x14 '^T' */
	0x7f, /* 01111111 */
	0xdb, /* 11011011 */
	0xdb, /* 11011011 */
	0x7b, /* 01111011 */
	0x1b, /* 00011011 */
	0x1b, /* 00011011 */
	0x1b, /* 00011011 */
	0x00, /* 00000000 */

	/* 21 0x15 '^U' */
	0x3e, /* 00111110 */
	0x61, /* 01100001 */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x86, /* 10000110 */
	0x7c, /* 01111100 */

	/* 22 0x16 '^V' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x7e, /* 01111110 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */

	/* 23 0x17 '^W' */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0xff, /* 11111111 */

	/* 24 0x18 '^X' */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 25 0x19 '^Y' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 26 0x1a '^Z' */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0xfe, /* 11111110 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 27 0x1b '^[' */
	0x00, /* 00000000 */
	0x30, /* 00110000 */
	0x60, /* 01100000 */
	0xfe, /* 11111110 */
	0x60, /* 01100000 */
	0x30, /* 00110000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 28 0x1c '^\' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 29 0x1d '^]' */
	0x00, /* 00000000 */
	0x24, /* 00100100 */
	0x66, /* 01100110 */
	0xff, /* 11111111 */
	0x66, /* 01100110 */
	0x24, /* 00100100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 30 0x1e '^^' */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x7e, /* 01111110 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 31 0x1f '^_' */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0x7e, /* 01111110 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 32 0x20 ' ' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 33 0x21 '!' */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 34 0x22 '"' */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x24, /* 00100100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 35 0x23 '#' */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0xfe, /* 11111110 */
	0x6c, /* 01101100 */
	0xfe, /* 11111110 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0x00, /* 00000000 */

	/* 36 0x24 '$' */
	0x18, /* 00011000 */
	0x3e, /* 00111110 */
	0x60, /* 01100000 */
	0x3c, /* 00111100 */
	0x06, /* 00000110 */
	0x7c, /* 01111100 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 37 0x25 '%' */
	0x00, /* 00000000 */
	0xc6, /* 11000110 */
	0xcc, /* 11001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x66, /* 01100110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 38 0x26 '&' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 39 0x27 ''' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 40 0x28 '(' */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x00, /* 00000000 */

	/* 41 0x29 ')' */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x00, /* 00000000 */

	/* 42 0x2a '*' */
	0x00, /* 00000000 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0xff, /* 11111111 */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 43 0x2b '+' */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 44 0x2c ',' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */

	/* 45 0x2d '-' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 46 0x2e '.' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 47 0x2f '/' */
	0x06, /* 00000110 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x60, /* 01100000 */
	0xc0, /* 11000000 */
	0x80, /* 10000000 */
	0x00, /* 00000000 */

	/* 48 0x30 '0' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xd6, /* 11010110 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x00, /* 00000000 */

	/* 49 0x31 '1' */
	0x18, /* 00011000 */
	0x38, /* 00111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */

	/* 50 0x32 '2' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0x06, /* 00000110 */
	0x1c, /* 00011100 */
	0x30, /* 00110000 */
	0x66, /* 01100110 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */

	/* 51 0x33 '3' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0x06, /* 00000110 */
	0x3c, /* 00111100 */
	0x06, /* 00000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 52 0x34 '4' */
	0x1c, /* 00011100 */
	0x3c, /* 00111100 */
	0x6c, /* 01101100 */
	0xcc, /* 11001100 */
	0xfe, /* 11111110 */
	0x0c, /* 00001100 */
	0x1e, /* 00011110 */
	0x00, /* 00000000 */

	/* 53 0x35 '5' */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xfc, /* 11111100 */
	0x06, /* 00000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 54 0x36 '6' */
	0x38, /* 00111000 */
	0x60, /* 01100000 */
	0xc0, /* 11000000 */
	0xfc, /* 11111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 55 0x37 '7' */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x00, /* 00000000 */

	/* 56 0x38 '8' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 57 0x39 '9' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7e, /* 01111110 */
	0x06, /* 00000110 */
	0x0c, /* 00001100 */
	0x78, /* 01111000 */
	0x00, /* 00000000 */

	/* 58 0x3a ':' */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 59 0x3b ';' */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */

	/* 60 0x3c '<' */
	0x06, /* 00000110 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x06, /* 00000110 */
	0x00, /* 00000000 */

	/* 61 0x3d '=' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 62 0x3e '>' */
	0x60, /* 01100000 */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x60, /* 01100000 */
	0x00, /* 00000000 */

	/* 63 0x3f '?' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 64 0x40 '@' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xde, /* 11011110 */
	0xde, /* 11011110 */
	0xde, /* 11011110 */
	0xc0, /* 11000000 */
	0x78, /* 01111000 */
	0x00, /* 00000000 */

	/* 65 0x41 'A' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 66 0x42 'B' */
	0xfc, /* 11111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x7c, /* 01111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0xfc, /* 11111100 */
	0x00, /* 00000000 */

	/* 67 0x43 'C' */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 68 0x44 'D' */
	0xf8, /* 11111000 */
	0x6c, /* 01101100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x6c, /* 01101100 */
	0xf8, /* 11111000 */
	0x00, /* 00000000 */

	/* 69 0x45 'E' */
	0xfe, /* 11111110 */
	0x62, /* 01100010 */
	0x68, /* 01101000 */
	0x78, /* 01111000 */
	0x68, /* 01101000 */
	0x62, /* 01100010 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */

	/* 70 0x46 'F' */
	0xfe, /* 11111110 */
	0x62, /* 01100010 */
	0x68, /* 01101000 */
	0x78, /* 01111000 */
	0x68, /* 01101000 */
	0x60, /* 01100000 */
	0xf0, /* 11110000 */
	0x00, /* 00000000 */

	/* 71 0x47 'G' */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xce, /* 11001110 */
	0x66, /* 01100110 */
	0x3a, /* 00111010 */
	0x00, /* 00000000 */

	/* 72 0x48 'H' */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 73 0x49 'I' */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 74 0x4a 'J' */
	0x1e, /* 00011110 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x78, /* 01111000 */
	0x00, /* 00000000 */

	/* 75 0x4b 'K' */
	0xe6, /* 11100110 */
	0x66, /* 01100110 */
	0x6c, /* 01101100 */
	0x78, /* 01111000 */
	0x6c, /* 01101100 */
	0x66, /* 01100110 */
	0xe6, /* 11100110 */
	0x00, /* 00000000 */

	/* 76 0x4c 'L' */
	0xf0, /* 11110000 */
	0x60, /* 01100000 */
	0x60, /* 01100000 */
	0x60, /* 01100000 */
	0x62, /* 01100010 */
	0x66, /* 01100110 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */

	/* 77 0x4d 'M' */
	0xc6, /* 11000110 */
	0xee, /* 11101110 */
	0xfe, /* 11111110 */
	0xfe, /* 11111110 */
	0xd6, /* 11010110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 78 0x4e 'N' */
	0xc6, /* 11000110 */
	0xe6, /* 11100110 */
	0xf6, /* 11110110 */
	0xde, /* 11011110 */
	0xce, /* 11001110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 79 0x4f 'O' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 80 0x50 'P' */
	0xfc, /* 11111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x7c, /* 01111100 */
	0x60, /* 01100000 */
	0x60, /* 01100000 */
	0xf0, /* 11110000 */
	0x00, /* 00000000 */

	/* 81 0x51 'Q' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xce, /* 11001110 */
	0x7c, /* 01111100 */
	0x0e, /* 00001110 */

	/* 82 0x52 'R' */
	0xfc, /* 11111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x7c, /* 01111100 */
	0x6c, /* 01101100 */
	0x66, /* 01100110 */
	0xe6, /* 11100110 */
	0x00, /* 00000000 */

	/* 83 0x53 'S' */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 84 0x54 'T' */
	0x7e, /* 01111110 */
	0x7e, /* 01111110 */
	0x5a, /* 01011010 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 85 0x55 'U' */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 86 0x56 'V' */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x00, /* 00000000 */

	/* 87 0x57 'W' */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xd6, /* 11010110 */
	0xd6, /* 11010110 */
	0xfe, /* 11111110 */
	0x6c, /* 01101100 */
	0x00, /* 00000000 */

	/* 88 0x58 'X' */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 89 0x59 'Y' */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 90 0x5a 'Z' */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0x8c, /* 10001100 */
	0x18, /* 00011000 */
	0x32, /* 00110010 */
	0x66, /* 01100110 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */

	/* 91 0x5b '[' */
	0x3c, /* 00111100 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 92 0x5c '\' */
	0xc0, /* 11000000 */
	0x60, /* 01100000 */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x06, /* 00000110 */
	0x02, /* 00000010 */
	0x00, /* 00000000 */

	/* 93 0x5d ']' */
	0x3c, /* 00111100 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 94 0x5e '^' */
	0x10, /* 00010000 */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 95 0x5f '_' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */

	/* 96 0x60 '`' */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 97 0x61 'a' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x78, /* 01111000 */
	0x0c, /* 00001100 */
	0x7c, /* 01111100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 98 0x62 'b' */
	0xe0, /* 11100000 */
	0x60, /* 01100000 */
	0x7c, /* 01111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0xdc, /* 11011100 */
	0x00, /* 00000000 */

	/* 99 0x63 'c' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc0, /* 11000000 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 100 0x64 'd' */
	0x1c, /* 00011100 */
	0x0c, /* 00001100 */
	0x7c, /* 01111100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 101 0x65 'e' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 102 0x66 'f' */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0x60, /* 01100000 */
	0xf8, /* 11111000 */
	0x60, /* 01100000 */
	0x60, /* 01100000 */
	0xf0, /* 11110000 */
	0x00, /* 00000000 */

	/* 103 0x67 'g' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x76, /* 01110110 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x7c, /* 01111100 */
	0x0c, /* 00001100 */
	0xf8, /* 11111000 */

	/* 104 0x68 'h' */
	0xe0, /* 11100000 */
	0x60, /* 01100000 */
	0x6c, /* 01101100 */
	0x76, /* 01110110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0xe6, /* 11100110 */
	0x00, /* 00000000 */

	/* 105 0x69 'i' */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x38, /* 00111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 106 0x6a 'j' */
	0x06, /* 00000110 */
	0x00, /* 00000000 */
	0x06, /* 00000110 */
	0x06, /* 00000110 */
	0x06, /* 00000110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */

	/* 107 0x6b 'k' */
	0xe0, /* 11100000 */
	0x60, /* 01100000 */
	0x66, /* 01100110 */
	0x6c, /* 01101100 */
	0x78, /* 01111000 */
	0x6c, /* 01101100 */
	0xe6, /* 11100110 */
	0x00, /* 00000000 */

	/* 108 0x6c 'l' */
	0x38, /* 00111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 109 0x6d 'm' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xec, /* 11101100 */
	0xfe, /* 11111110 */
	0xd6, /* 11010110 */
	0xd6, /* 11010110 */
	0xd6, /* 11010110 */
	0x00, /* 00000000 */

	/* 110 0x6e 'n' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xdc, /* 11011100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x00, /* 00000000 */

	/* 111 0x6f 'o' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 112 0x70 'p' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xdc, /* 11011100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x7c, /* 01111100 */
	0x60, /* 01100000 */
	0xf0, /* 11110000 */

	/* 113 0x71 'q' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x76, /* 01110110 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x7c, /* 01111100 */
	0x0c, /* 00001100 */
	0x1e, /* 00011110 */

	/* 114 0x72 'r' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xdc, /* 11011100 */
	0x76, /* 01110110 */
	0x60, /* 01100000 */
	0x60, /* 01100000 */
	0xf0, /* 11110000 */
	0x00, /* 00000000 */

	/* 115 0x73 's' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0xc0, /* 11000000 */
	0x7c, /* 01111100 */
	0x06, /* 00000110 */
	0xfc, /* 11111100 */
	0x00, /* 00000000 */

	/* 116 0x74 't' */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0xfc, /* 11111100 */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x36, /* 00110110 */
	0x1c, /* 00011100 */
	0x00, /* 00000000 */

	/* 117 0x75 'u' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 118 0x76 'v' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x00, /* 00000000 */

	/* 119 0x77 'w' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xc6, /* 11000110 */
	0xd6, /* 11010110 */
	0xd6, /* 11010110 */
	0xfe, /* 11111110 */
	0x6c, /* 01101100 */
	0x00, /* 00000000 */

	/* 120 0x78 'x' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 121 0x79 'y' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7e, /* 01111110 */
	0x06, /* 00000110 */
	0xfc, /* 11111100 */

	/* 122 0x7a 'z' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x4c, /* 01001100 */
	0x18, /* 00011000 */
	0x32, /* 00110010 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */

	/* 123 0x7b '{' */
	0x0e, /* 00001110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x70, /* 01110000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x0e, /* 00001110 */
	0x00, /* 00000000 */

	/* 124 0x7c '|' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 125 0x7d '}' */
	0x70, /* 01110000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x0e, /* 00001110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x70, /* 01110000 */
	0x00, /* 00000000 */

	/* 126 0x7e '~' */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 127 0x7f '' */
	0x00, /* 00000000 */
	0x10, /* 00010000 */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */

	/* 128 0x80 '' */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x0c, /* 00001100 */
	0x78, /* 01111000 */

	/* 129 0x81 '' */
	0xcc, /* 11001100 */
	0x00, /* 00000000 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 130 0x82 '' */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 131 0x83 '' */
	0x7c, /* 01111100 */
	0x82, /* 10000010 */
	0x78, /* 01111000 */
	0x0c, /* 00001100 */
	0x7c, /* 01111100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 132 0x84 '' */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */
	0x78, /* 01111000 */
	0x0c, /* 00001100 */
	0x7c, /* 01111100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 133 0x85 '' */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x78, /* 01111000 */
	0x0c, /* 00001100 */
	0x7c, /* 01111100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 134 0x86 '' */
	0x30, /* 00110000 */
	0x30, /* 00110000 */
	0x78, /* 01111000 */
	0x0c, /* 00001100 */
	0x7c, /* 01111100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 135 0x87 '' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0x7e, /* 01111110 */
	0x0c, /* 00001100 */
	0x38, /* 00111000 */

	/* 136 0x88 '' */
	0x7c, /* 01111100 */
	0x82, /* 10000010 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 137 0x89 '' */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 138 0x8a '' */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 139 0x8b '' */
	0x66, /* 01100110 */
	0x00, /* 00000000 */
	0x38, /* 00111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 140 0x8c '' */
	0x7c, /* 01111100 */
	0x82, /* 10000010 */
	0x38, /* 00111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 141 0x8d '' */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x38, /* 00111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 142 0x8e '' */
	0xc6, /* 11000110 */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 143 0x8f '' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 144 0x90 '' */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0xf8, /* 11111000 */
	0xc0, /* 11000000 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */

	/* 145 0x91 '' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0xd8, /* 11011000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */

	/* 146 0x92 '' */
	0x3e, /* 00111110 */
	0x6c, /* 01101100 */
	0xcc, /* 11001100 */
	0xfe, /* 11111110 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xce, /* 11001110 */
	0x00, /* 00000000 */

	/* 147 0x93 '' */
	0x7c, /* 01111100 */
	0x82, /* 10000010 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 148 0x94 '' */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 149 0x95 '' */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 150 0x96 '' */
	0x78, /* 01111000 */
	0x84, /* 10000100 */
	0x00, /* 00000000 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 151 0x97 '' */
	0x60, /* 01100000 */
	0x30, /* 00110000 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 152 0x98 '' */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7e, /* 01111110 */
	0x06, /* 00000110 */
	0xfc, /* 11111100 */

	/* 153 0x99 '' */
	0xc6, /* 11000110 */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x00, /* 00000000 */

	/* 154 0x9a '' */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 155 0x9b '' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 156 0x9c '' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0x64, /* 01100100 */
	0xf0, /* 11110000 */
	0x60, /* 01100000 */
	0x66, /* 01100110 */
	0xfc, /* 11111100 */
	0x00, /* 00000000 */

	/* 157 0x9d '' */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 158 0x9e '' */
	0xf8, /* 11111000 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xfa, /* 11111010 */
	0xc6, /* 11000110 */
	0xcf, /* 11001111 */
	0xc6, /* 11000110 */
	0xc7, /* 11000111 */

	/* 159 0x9f '' */
	0x0e, /* 00001110 */
	0x1b, /* 00011011 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0xd8, /* 11011000 */
	0x70, /* 01110000 */
	0x00, /* 00000000 */

	/* 160 0xa0 ' ' */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x78, /* 01111000 */
	0x0c, /* 00001100 */
	0x7c, /* 01111100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 161 0xa1 '¡' */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x38, /* 00111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 162 0xa2 '¢' */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */

	/* 163 0xa3 '£' */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 164 0xa4 '¤' */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0x00, /* 00000000 */
	0xdc, /* 11011100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x00, /* 00000000 */

	/* 165 0xa5 '¥' */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0x00, /* 00000000 */
	0xe6, /* 11100110 */
	0xf6, /* 11110110 */
	0xde, /* 11011110 */
	0xce, /* 11001110 */
	0x00, /* 00000000 */

	/* 166 0xa6 '¦' */
	0x3c, /* 00111100 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0x3e, /* 00111110 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 167 0xa7 '§' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 168 0xa8 '¨' */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x63, /* 01100011 */
	0x3e, /* 00111110 */
	0x00, /* 00000000 */

	/* 169 0xa9 '©' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 170 0xaa 'ª' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0x06, /* 00000110 */
	0x06, /* 00000110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 171 0xab '«' */
	0x63, /* 01100011 */
	0xe6, /* 11100110 */
	0x6c, /* 01101100 */
	0x7e, /* 01111110 */
	0x33, /* 00110011 */
	0x66, /* 01100110 */
	0xcc, /* 11001100 */
	0x0f, /* 00001111 */

	/* 172 0xac '¬' */
	0x63, /* 01100011 */
	0xe6, /* 11100110 */
	0x6c, /* 01101100 */
	0x7a, /* 01111010 */
	0x36, /* 00110110 */
	0x6a, /* 01101010 */
	0xdf, /* 11011111 */
	0x06, /* 00000110 */

	/* 173 0xad '­' */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 174 0xae '®' */
	0x00, /* 00000000 */
	0x33, /* 00110011 */
	0x66, /* 01100110 */
	0xcc, /* 11001100 */
	0x66, /* 01100110 */
	0x33, /* 00110011 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 175 0xaf '¯' */
	0x00, /* 00000000 */
	0xcc, /* 11001100 */
	0x66, /* 01100110 */
	0x33, /* 00110011 */
	0x66, /* 01100110 */
	0xcc, /* 11001100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 176 0xb0 '°' */
	0x22, /* 00100010 */
	0x88, /* 10001000 */
	0x22, /* 00100010 */
	0x88, /* 10001000 */
	0x22, /* 00100010 */
	0x88, /* 10001000 */
	0x22, /* 00100010 */
	0x88, /* 10001000 */

	/* 177 0xb1 '±' */
	0x55, /* 01010101 */
	0xaa, /* 10101010 */
	0x55, /* 01010101 */
	0xaa, /* 10101010 */
	0x55, /* 01010101 */
	0xaa, /* 10101010 */
	0x55, /* 01010101 */
	0xaa, /* 10101010 */

	/* 178 0xb2 '²' */
	0x77, /* 01110111 */
	0xdd, /* 11011101 */
	0x77, /* 01110111 */
	0xdd, /* 11011101 */
	0x77, /* 01110111 */
	0xdd, /* 11011101 */
	0x77, /* 01110111 */
	0xdd, /* 11011101 */

	/* 179 0xb3 '³' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 180 0xb4 '´' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xf8, /* 11111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 181 0xb5 'µ' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xf8, /* 11111000 */
	0x18, /* 00011000 */
	0xf8, /* 11111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 182 0xb6 '¶' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xf6, /* 11110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 183 0xb7 '·' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 184 0xb8 '¸' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xf8, /* 11111000 */
	0x18, /* 00011000 */
	0xf8, /* 11111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 185 0xb9 '¹' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xf6, /* 11110110 */
	0x06, /* 00000110 */
	0xf6, /* 11110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 186 0xba 'º' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 187 0xbb '»' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0x06, /* 00000110 */
	0xf6, /* 11110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 188 0xbc '¼' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xf6, /* 11110110 */
	0x06, /* 00000110 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 189 0xbd '½' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 190 0xbe '¾' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xf8, /* 11111000 */
	0x18, /* 00011000 */
	0xf8, /* 11111000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 191 0xbf '¿' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xf8, /* 11111000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 192 0xc0 'À' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x1f, /* 00011111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 193 0xc1 'Á' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 194 0xc2 'Â' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 195 0xc3 'Ã' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x1f, /* 00011111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 196 0xc4 'Ä' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 197 0xc5 'Å' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xff, /* 11111111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 198 0xc6 'Æ' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x1f, /* 00011111 */
	0x18, /* 00011000 */
	0x1f, /* 00011111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 199 0xc7 'Ç' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x37, /* 00110111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 200 0xc8 'È' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x37, /* 00110111 */
	0x30, /* 00110000 */
	0x3f, /* 00111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 201 0xc9 'É' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x3f, /* 00111111 */
	0x30, /* 00110000 */
	0x37, /* 00110111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 202 0xca 'Ê' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xf7, /* 11110111 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 203 0xcb 'Ë' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0xf7, /* 11110111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 204 0xcc 'Ì' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x37, /* 00110111 */
	0x30, /* 00110000 */
	0x37, /* 00110111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 205 0xcd 'Í' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 206 0xce 'Î' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xf7, /* 11110111 */
	0x00, /* 00000000 */
	0xf7, /* 11110111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 207 0xcf 'Ï' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 208 0xd0 'Ð' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 209 0xd1 'Ñ' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 210 0xd2 'Ò' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 211 0xd3 'Ó' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x3f, /* 00111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 212 0xd4 'Ô' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x1f, /* 00011111 */
	0x18, /* 00011000 */
	0x1f, /* 00011111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 213 0xd5 'Õ' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x1f, /* 00011111 */
	0x18, /* 00011000 */
	0x1f, /* 00011111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 214 0xd6 'Ö' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x3f, /* 00111111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 215 0xd7 '×' */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0xff, /* 11111111 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */

	/* 216 0xd8 'Ø' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xff, /* 11111111 */
	0x18, /* 00011000 */
	0xff, /* 11111111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 217 0xd9 'Ù' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xf8, /* 11111000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 218 0xda 'Ú' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x1f, /* 00011111 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 219 0xdb 'Û' */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */

	/* 220 0xdc 'Ü' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */

	/* 221 0xdd 'Ý' */
	0xf0, /* 11110000 */
	0xf0, /* 11110000 */
	0xf0, /* 11110000 */
	0xf0, /* 11110000 */
	0xf0, /* 11110000 */
	0xf0, /* 11110000 */
	0xf0, /* 11110000 */
	0xf0, /* 11110000 */

	/* 222 0xde 'Þ' */
	0x0f, /* 00001111 */
	0x0f, /* 00001111 */
	0x0f, /* 00001111 */
	0x0f, /* 00001111 */
	0x0f, /* 00001111 */
	0x0f, /* 00001111 */
	0x0f, /* 00001111 */
	0x0f, /* 00001111 */

	/* 223 0xdf 'ß' */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0xff, /* 11111111 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 224 0xe0 'à' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0xc8, /* 11001000 */
	0xdc, /* 11011100 */
	0x76, /* 01110110 */
	0x00, /* 00000000 */

	/* 225 0xe1 'á' */
	0x78, /* 01111000 */
	0xcc, /* 11001100 */
	0xcc, /* 11001100 */
	0xd8, /* 11011000 */
	0xcc, /* 11001100 */
	0xc6, /* 11000110 */
	0xcc, /* 11001100 */
	0x00, /* 00000000 */

	/* 226 0xe2 'â' */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0xc0, /* 11000000 */
	0x00, /* 00000000 */

	/* 227 0xe3 'ã' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0x00, /* 00000000 */

	/* 228 0xe4 'ä' */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0x60, /* 01100000 */
	0x30, /* 00110000 */
	0x60, /* 01100000 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */

	/* 229 0xe5 'å' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0xd8, /* 11011000 */
	0xd8, /* 11011000 */
	0xd8, /* 11011000 */
	0x70, /* 01110000 */
	0x00, /* 00000000 */

	/* 230 0xe6 'æ' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x7c, /* 01111100 */
	0xc0, /* 11000000 */

	/* 231 0xe7 'ç' */
	0x00, /* 00000000 */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */

	/* 232 0xe8 'è' */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x3c, /* 00111100 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */

	/* 233 0xe9 'é' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xfe, /* 11111110 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x00, /* 00000000 */

	/* 234 0xea 'ê' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0xee, /* 11101110 */
	0x00, /* 00000000 */

	/* 235 0xeb 'ë' */
	0x0e, /* 00001110 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x3e, /* 00111110 */
	0x66, /* 01100110 */
	0x66, /* 01100110 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */

	/* 236 0xec 'ì' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0xdb, /* 11011011 */
	0xdb, /* 11011011 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 237 0xed 'í' */
	0x06, /* 00000110 */
	0x0c, /* 00001100 */
	0x7e, /* 01111110 */
	0xdb, /* 11011011 */
	0xdb, /* 11011011 */
	0x7e, /* 01111110 */
	0x60, /* 01100000 */
	0xc0, /* 11000000 */

	/* 238 0xee 'î' */
	0x1e, /* 00011110 */
	0x30, /* 00110000 */
	0x60, /* 01100000 */
	0x7e, /* 01111110 */
	0x60, /* 01100000 */
	0x30, /* 00110000 */
	0x1e, /* 00011110 */
	0x00, /* 00000000 */

	/* 239 0xef 'ï' */
	0x00, /* 00000000 */
	0x7c, /* 01111100 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0xc6, /* 11000110 */
	0x00, /* 00000000 */

	/* 240 0xf0 'ð' */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */
	0xfe, /* 11111110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 241 0xf1 'ñ' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x7e, /* 01111110 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */

	/* 242 0xf2 'ò' */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */

	/* 243 0xf3 'ó' */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x18, /* 00011000 */
	0x0c, /* 00001100 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */

	/* 244 0xf4 'ô' */
	0x0e, /* 00001110 */
	0x1b, /* 00011011 */
	0x1b, /* 00011011 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */

	/* 245 0xf5 'õ' */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0xd8, /* 11011000 */
	0xd8, /* 11011000 */
	0x70, /* 01110000 */

	/* 246 0xf6 'ö' */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x7e, /* 01111110 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 247 0xf7 '÷' */
	0x00, /* 00000000 */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0x00, /* 00000000 */
	0x76, /* 01110110 */
	0xdc, /* 11011100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 248 0xf8 'ø' */
	0x38, /* 00111000 */
	0x6c, /* 01101100 */
	0x6c, /* 01101100 */
	0x38, /* 00111000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 249 0xf9 'ù' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 250 0xfa 'ú' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x18, /* 00011000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 251 0xfb 'û' */
	0x0f, /* 00001111 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0x0c, /* 00001100 */
	0xec, /* 11101100 */
	0x6c, /* 01101100 */
	0x3c, /* 00111100 */
	0x1c, /* 00011100 */

	/* 252 0xfc 'ü' */
	0x6c, /* 01101100 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x36, /* 00110110 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 253 0xfd 'ý' */
	0x78, /* 01111000 */
	0x0c, /* 00001100 */
	0x18, /* 00011000 */
	0x30, /* 00110000 */
	0x7c, /* 01111100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 254 0xfe 'þ' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x3c, /* 00111100 */
	0x3c, /* 00111100 */
	0x3c, /* 00111100 */
	0x3c, /* 00111100 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

	/* 255 0xff 'ÿ' */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */
	0x00, /* 00000000 */

};

void fb_open(int fb_num, struct fb_info *fb_info)
{
	char str[64];
	int fd,tty;

//	tty = open("/dev/tty1", O_RDWR);

//	if(ioctl(tty, KDSETMODE, KD_GRAPHICS) == -1)
//		printf("Failed to set graphics mode on tty1\n");

	sprintf(str, "/dev/fb%d", fb_num);
	fd = open(str, O_RDWR);

	ASSERT(fd >= 0);

	fb_info->fd = fd;
	IOCTL1(fd, FBIOGET_VSCREENINFO, &fb_info->var);
	IOCTL1(fd, FBIOGET_FSCREENINFO, &fb_info->fix);

	printf("fb res %dx%d virtual %dx%d, line_len %d, bpp %d\n",
			fb_info->var.xres, fb_info->var.yres,
			fb_info->var.xres_virtual, fb_info->var.yres_virtual,
			fb_info->fix.line_length, fb_info->var.bits_per_pixel);

	void *ptr = mmap(0,
			fb_info->var.yres_virtual * fb_info->fix.line_length,
			PROT_WRITE | PROT_READ,
			MAP_SHARED, fd, 0);

	ASSERT(ptr != MAP_FAILED);

	fb_info->ptr = ptr;
}

static void fb_clear_area(struct fb_info *fb_info, int x, int y, int w, int h)
{
	int i = 0;
	int loc;
	char *fbuffer = (char *)fb_info->ptr;
	struct fb_var_screeninfo *var = &fb_info->var;
	struct fb_fix_screeninfo *fix = &fb_info->fix;

	for (i = 0; i < h; i++) {
		loc = (x + var->xoffset) * (var->bits_per_pixel / 8)
			+ (y + i + var->yoffset) * fix->line_length;
		memset(fbuffer + loc, 0, w * var->bits_per_pixel / 8);
	}
}

static void draw_pixel(struct fb_info *fb_info, int x, int y, unsigned color)
{
	void *fbmem;

	fbmem = fb_info->ptr;
	if (fb_info->var.bits_per_pixel == 8) {
		unsigned char *p;

		fbmem += fb_info->fix.line_length * y;

		p = fbmem;

		p += x;

		*p = color;
	} else if (fb_info->var.bits_per_pixel == 16) {
		unsigned short c;
		unsigned r = (color >> 16) & 0xff;
		unsigned g = (color >> 8) & 0xff;
		unsigned b = (color >> 0) & 0xff;
		unsigned short *p;

		r = r * 32 / 256;
		g = g * 64 / 256;
		b = b * 32 / 256;

		c = (r << 11) | (g << 5) | (b << 0);

		fbmem += fb_info->fix.line_length * y;

		p = fbmem;

		p += x;

		*p = c;
	} else if (fb_info->var.bits_per_pixel == 24) {
		unsigned char *p;

		p = (unsigned char *)fbmem + fb_info->fix.line_length * y + 3 * x;
		*p++ = color;
		*p++ = color >> 8;
		*p = color >> 16;
	} else {
		unsigned int *p;

		fbmem += fb_info->fix.line_length * y;

		p = fbmem;

		p += x;

		*p = color;
	}
}
static void show_status(struct fb_info *fb_info, int val)
{
	unsigned x, y;
	unsigned h = fb_info->var.yres;
	unsigned w = fb_info->var.xres;

	if(val==0)
		fb_put_string(fb_info, 20, 30, "Showing the Softswitch status on screen", 64, 0xffffff, 1, 4);
	else if(val == 1)
	{
		fb_put_string(fb_info, 20, 70, "           ", 32, 0xffffff, 1, 4);
		fb_put_string(fb_info, 20, 70, "Pressed", 32, 0xffffff, 1, 4);
	}
	else if(val == 2){
		fb_put_string(fb_info, 20, 70, "           ", 32, 0xffffff, 1, 4);
		fb_put_string(fb_info, 20, 70, "NOT Pressed   ", 32, 0xffffff, 1, 4);
	}
	
}

void fill_screen_solid(struct fb_info *fb_info, unsigned int color)
{

	unsigned x, y;
	unsigned h = fb_info->var.yres;
	unsigned w = fb_info->var.xres;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++)
			draw_pixel(fb_info, x, y, color);
	}
}

static void fb_put_char(struct fb_info *fb_info, int x, int y, char c,
		unsigned color)
{
	int i, j, bits, loc;
	unsigned char *p8;
	unsigned short *p16;
	unsigned int *p32;
	struct fb_var_screeninfo *var = &fb_info->var;
	struct fb_fix_screeninfo *fix = &fb_info->fix;

	for (i = 0; i < 8; i++) {
		bits = fontdata_8x8[8 * c + i];
		for (j = 0; j < 8; j++) {
			loc = (x + j + var->xoffset) * (var->bits_per_pixel / 8)
				+ (y + i + var->yoffset) * fix->line_length;
			if (loc >= 0 && loc < fix->smem_len &&
					((bits >> (7 - j)) & 1)) {
				switch (var->bits_per_pixel) {
					case 8:
						p8 =  fb_info->ptr + loc;
						*p8 = color;
					case 16:
						p16 = fb_info->ptr + loc;
						*p16 = color;
						break;
					case 24:
					case 32:
						p32 = fb_info->ptr + loc;
						*p32 = color;
						break;
				}
			}
		}
	}
}

int fb_put_string(struct fb_info *fb_info, int x, int y, char *s, int maxlen,
		int color, int clear, int clearlen)
{
	int i;
	int w = 0;

	if (clear)
		fb_clear_area(fb_info, x, y, clearlen * 8, 8);

	for (i = 0; i < strlen(s) && i < maxlen; i++) {
		fb_put_char(fb_info, (x + 8 * i), y, s[i], color);
		w += 8;
	}

	return w;
}

static inline void do_fit_to_screen(struct image *i, int screen_width, int screen_height, int ignoreaspect, int cal)
{
	if((i->width > screen_width) || (i->height > screen_height))
	{
		unsigned char * new_image, * new_alpha = NULL;
		int nx_size = i->width, ny_size = i->height;

		if(ignoreaspect)
		{
			if(i->width > screen_width)
				nx_size = screen_width;
			if(i->height > screen_height)
				ny_size = screen_height;
		}
		else
		{
			if((i->height * screen_width / i->width) <= screen_height)
			{
				nx_size = screen_width;
				ny_size = i->height * screen_width / i->width;
			}
			else
			{
				nx_size = i->width * screen_height / i->height;
				ny_size = screen_height;
			}
		}

		if(cal)
			new_image = color_average_resize(i->rgb, i->width, i->height, nx_size, ny_size);
		else
			new_image = simple_resize(i->rgb, i->width, i->height, nx_size, ny_size);

		if(i->alpha)
			new_alpha = alpha_resize(i->alpha, i->width, i->height, nx_size, ny_size);

		if(i->do_free)
		{
			free(i->alpha);
			free(i->rgb);
		}

		i->rgb = new_image;
		i->alpha = new_alpha;
		i->do_free = 1;
		i->width = nx_size;
		i->height = ny_size;
	}
}
static int opt_clear = 1,
	   opt_alpha = 0,
	   opt_hide_cursor = 1,
	   opt_image_info = 1,
	   opt_stretch = 0,
	   opt_delay = 0,
	   opt_enlarge = 0,
	   opt_ignore_aspect = 0;


int show_image(char *filename)
{
	int (*load)(char *, unsigned char *, unsigned char **, int, int);

	unsigned char * image = NULL;
	unsigned char * alpha = NULL;

	int x_size, y_size, screen_width, screen_height;
	int x_pan, y_pan, x_offs, y_offs, refresh = 1, c, ret = 1;
	int delay = opt_delay, retransform = 1;

	int transform_stretch = opt_stretch, transform_enlarge = opt_enlarge, transform_cal = (opt_stretch == 2),
	    transform_iaspect = opt_ignore_aspect, transform_rotation = 0;

	struct image i;

	if(fh_jpeg_id(filename))
		if(fh_jpeg_getsize(filename, &x_size, &y_size) == FH_ERROR_OK)
		{
			load = fh_jpeg_load;
			goto identified;
		}
	fprintf(stderr, "%s: Unable to access file or file format unknown.\n", filename);
	return(1);

identified:

	if(!(image = (unsigned char*) malloc(x_size * y_size * 3)))
	{
		fprintf(stderr, "%s: Out of memory.\n", filename);
		goto error_mem;
	}

	if(load(filename, image, &alpha, x_size, y_size) != FH_ERROR_OK)
	{
		fprintf(stderr, "%s: Image data is corrupt?\n", filename);
		goto error_mem;
	}

	if(!opt_alpha)
	{
		free(alpha);
		alpha = NULL;
	}

	getCurrentRes(&screen_width, &screen_height);
	i.do_free = 0;
	{
		if(retransform)
		{
			if(i.do_free)
			{
				free(i.rgb);
				free(i.alpha);
			}
			i.width = x_size;
			i.height = y_size;
			i.rgb = image;
			i.alpha = alpha;
			i.do_free = 0;

			do_fit_to_screen(&i, screen_width, screen_height, transform_iaspect, transform_cal);

			x_pan = y_pan = 0;
			refresh = 1; retransform = 0;
			if(opt_clear)
			{
				printf("\033[H\033[J");
				fflush(stdout);
			}
			if(opt_image_info)
				printf("fbv - The Framebuffer Viewer\n%s\n%d x %d\n", filename, x_size, y_size); 
		}
		if(refresh)
		{
			if(i.width < screen_width)
				x_offs = (screen_width - i.width) / 2;
			else
				x_offs = 0;

			if(i.height < screen_height)
				y_offs = (screen_height - i.height) / 2;
			else
				y_offs = 0;

			fb_display(i.rgb, i.alpha, i.width, i.height, x_pan, y_pan, x_offs, y_offs);
			refresh = 0;
		}
	}

done:
	if(opt_clear)
	{
		printf("\033[H\033[J");
		fflush(stdout);
	}

error_mem:
	free(image);
	free(alpha);
	if(i.do_free)
	{
		free(i.rgb);
		free(i.alpha);
	}
	return(ret);

}


uint32_t pixel_color(uint8_t r, uint8_t g, uint8_t b, struct fb_var_screeninfo *vinfo)
{
	return (r<<vinfo->red.offset) | (g<<vinfo->green.offset) | (b<<vinfo->blue.offset);
}

int resetfb()
{
	struct fb_fix_screeninfo finfo;
	struct fb_var_screeninfo vinfo;
	uint8_t red = 0x0,  green = 0x0, blue=0x0;

	int fb_fd = open("/dev/fb0",O_RDWR);
	//Get variable screen information
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
	vinfo.grayscale=0;
	vinfo.bits_per_pixel=16;
	ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

	ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

	close(fb_fd);
	return 0;
}

int setcolor(int color)
{
	struct fb_fix_screeninfo finfo;
	struct fb_var_screeninfo vinfo;
	uint8_t red = 0x0,  green = 0x0, blue=0x0;

	int fb_fd = open("/dev/fb0",O_RDWR);
	switch (color) {
		case 1:
			red = 0xff;
			break;
		case 2:
			green = 0xff;
			break;
		case 3:
			blue = 0xff;
			break;
		case 4:
			red = green = blue = 0xff;
			break;
		case 5:
			break;
	}

	//Get variable screen information
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);
//	vinfo.grayscale=0;
//	vinfo.bits_per_pixel=32;
	ioctl(fb_fd, FBIOPUT_VSCREENINFO, &vinfo);
	ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo);

	ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo);

	long screensize = vinfo.yres_virtual * finfo.line_length;

	uint8_t *fbp = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, (off_t)0);

	int x,y;

	for (x=0;x<vinfo.xres;x++)
		for (y=0;y<vinfo.yres;y++)
		{
			long location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
			*((uint32_t*)(fbp + location)) = pixel_color(red,green,blue, &vinfo);
		}

	close(fb_fd);
	return 0;
}


int main(int argc, char **argv)
{
	int req_fb = 0;
	int req_pattern = 0;
	int i = 0, count = 0;

	char value;
	int fd;

	fb_open(req_fb, &fb_info);

	setcolor(5);
	sleep(1);
	setcolor(4);

	return 0;
}
