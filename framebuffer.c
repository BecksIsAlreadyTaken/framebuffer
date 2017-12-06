#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>
#define N 10
#define xpos 200
#define ypos 200
struct fb_var_screeninfo varinfo;
struct fb_fix_screeninfo fixinfo;	
int fp = 0;				//file descriptor
char* fbp = 0;				//pointer to the mapped area
long int size = 0;			//length of the mapping
const int char_a[N][N]={		//bitmap   'a'
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
int get_info(int fp) { //read screeninfo
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
	"xres_virtual: %d\tyres_virtual: %d\n"
	,varinfo.xres,varinfo.yres,varinfo.bits_per_pixel,varinfo.xres_virtual,varinfo.yres_virtual
	);
	printf("Fixed screeninfo:\n"
	"line_length: %d\n",fixinfo.line_length
	);
	return 1;

}
void pixel(int i,int j) { //draw a pixel (BGRA) 
	long int pos = i * (varinfo.bits_per_pixel / 8) + j * fixinfo.line_length;
	*(fbp + pos + 0) = 0;
	*(fbp + pos + 1) = 0;
	*(fbp + pos + 2) = 255;
	*(fbp + pos + 3) = 0;
}
void drawChar() { //draw the character 'a'
	int i,j;
	for(i = 0;i<N;i++){
		for(j = 0;j<N;j++){
			if(char_a[i][j]==1){
				pixel(j + xpos,i + ypos); //switch i,j i(row) j(column)
			}	
		}
	}	
}
int main() {
	char* p = NULL;
	//char* p = 0xe8000000;   //test
	fp=open("/dev/fb0",O_RDWR);  //open /dev/fb0
	if(fp<0){
		printf("Error opening /dev/fb0.\n");
		exit(1);
	}
	if(!get_info(fp)){
		printf("Error reading framebuffer device info.\n");
		exit(2);	
	}
	size = varinfo.xres * varinfo.yres * varinfo.bits_per_pixel / 8; //calculate the length of the mapping
	printf("size: %ld k\n",size/1024);
	fbp = (char *)mmap(p, size, PROT_READ | PROT_WRITE, MAP_SHARED, fp, 0);
	if((int)fbp == -1){
		printf("Error mapping framebuffer to memory.\n");
		exit(3);
	}
	memset(fbp, 0, size/2); 
	drawChar();
	munmap(fbp, size);
	close(fp);
	return 0;
}
