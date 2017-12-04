#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#define N 10
struct fb_var_screeninfo varinfo;
struct fb_fix_screeninfo fixinfo;
int fp = 0;
char* fbp = 0;
long int size = 0;
const int char_a[N][N]={
        {0,0,1,1,1,1,1,1,0,0},
        {0,1,1,1,0,0,1,1,1,0},
        {0,1,1,0,0,0,1,1,1,0},
        {0,0,0,1,1,1,1,1,1,0},
        {0,1,1,1,1,0,1,1,1,0},
        {1,1,1,0,0,0,1,1,1,0},
        {1,1,1,0,0,0,1,1,1,0},
	{1,1,1,0,0,1,1,1,1,1},
	{0,1,1,1,1,1,1,1,1,1},
	{0,0,0,0,0,0,0,0,0,0}
};
int get_info(int fp) {
	if(ioctl(fp,FBIOGET_VSCREENINFO,&varinfo)){
		printf("Error reading fb_var_screeninfo.\n");
		return 0;
	}
	if(ioctl(fp,FBIOGET_FSCREENINFO,&fixinfo)){
		printf("Error reading fb_fix_screeninfo.\n");
		return 0;
	}
	printf("Variable screeninfo:\n"
	"xres: %d\tyres: %d\n"
	"bits_per_pixel: %d\n"
	,varinfo.xres,varinfo.yres,varinfo.bits_per_pixel
	);
	printf("Fixed screeninfo:\n"
	"line_length: %d\n",fixinfo.line_length
	);
	return 1;

}
void pixel(int i,int j) {
	long int pos = i * (varinfo.bits_per_pixel / 8) + j * fixinfo.line_length;
	*(fbp + pos) = 0;
	*(fbp + pos + 1) = 0;
	*(fbp + pos + 2) = 255;
	*(fbp + pos + 3) = 0;
}
void drawChar() {
	int i,j;
	for(i = 0;i<N;i++){
		for(j = 0;j<N;j++){
			if(char_a[i][j]==1){
				pixel(j,i);
			}	
		}
	}	
}
int main() {
	fp=open("/dev/fb0",O_RDWR);
	if(fp<0){
		printf("Error opening /dev/fb0.\n");
		return 0;
	}
	if(!get_info(fp)){
		printf("Error reading framebuffer device info.\n");
		return 0;	
	}
	size = varinfo.xres * varinfo.yres * varinfo.bits_per_pixel / 8;
	fbp = (char *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fp, 0);
	if((int)fbp == -1){
		printf("Error mapping framebuffer to memory.\n");
		return 0;
	}
	drawChar();
	munmap(fbp, size);
	close(fp);
	return 0;
}
