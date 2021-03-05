#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>

#define XPAR_BRAM_0_BASEADDR 0x40000000
#define XPAR_BRAM_1_BASEADDR 0x42000000
#define XPAR_BRAM_2_BASEADDR 0x44000000
#define XPAR_BRAM_3_BASEADDR 0x46000000
#define XPAR_BRAM_4_BASEADDR 0x48000000
#define XPAR_BRAM_5_BASEADDR 0x4A000000
#define XPAR_BRAM_6_BASEADDR 0x4C000000
#define XPAR_BRAM_7_BASEADDR 0x4E000000
#define XPAR_BRAM_8_BASEADDR 0x50000000
#define DATA_LEN    1024

#define MSG(args...) printf(args) 
static int gpio_export(int pin);
static int gpio_unexport(int pin);
static int gpio_direction(int pin, int dir);
static int gpio_write(int pin, int value);
static int gpio_read(int pin);
static int gpio_edge(int pin, int edge);

static void XBram_Out32(unsigned int * Addr, unsigned long Value)
{
	volatile unsigned long *LocalAddr = (volatile unsigned long *)Addr;
	*LocalAddr = Value;
}
static  XBram_In32(unsigned int * Addr)
{
	return *(volatile unsigned long *) Addr;
}
static int gpio_export(int pin)  
{  
    char buffer[64];  
    int len;  
    int fd;  
  
    fd = open("/sys/class/gpio/export", O_WRONLY);  
    if (fd < 0) 
    {  
        MSG("Failed to open export for writing!\n");  
        return(-1);  
    }  
  
    len = snprintf(buffer, sizeof(buffer), "%d", pin);  
    printf("%s,%d,%d\n",buffer,sizeof(buffer),len);
    if (write(fd, buffer, len) < 0) 
    {  
        MSG("Failed to export gpio!");  
        return -1;  
    }  
     
    close(fd);  
    return 0;  
}  
static int gpio_unexport(int pin)  
{  
    char buffer[64];  
    int len;  
    int fd;  
  
    fd = open("/sys/class/gpio/unexport", O_WRONLY);  
    if (fd < 0) 
    {  
        MSG("Failed to open unexport for writing!\n");  
        return -1;  
    }  
  
    len = snprintf(buffer, sizeof(buffer), "%d", pin);  
    if (write(fd, buffer, len) < 0) 
    {  
        MSG("Failed to unexport gpio!");  
        return -1;  
    }  
     
    close(fd);  
    return 0;  
} 
//dir: 0输入, 1输出
static int gpio_direction(int pin, int dir)  
{  
    static const char dir_str[] = "in\0out";  
    char path[64];  
    int fd;  
  
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);  
    fd = open(path, O_WRONLY);  
    if (fd < 0) 
    {  
        MSG("Failed to open gpio direction for writing!\n");  
        return -1;  
    }  
  
    if (write(fd, &dir_str[dir == 0 ? 0 : 3], dir == 0 ? 2 : 3) < 0) 
    {  
        MSG("Failed to set direction!\n");  
        return -1;  
    }  
  
    close(fd);  
    return 0;  
}  
//value: 0-->LOW, 1-->HIGH
static int gpio_write(int pin, int value)  
{  
    static const char values_str[] = "01";  
    char path[64];  
    int fd;  
  
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);  
    fd = open(path, O_WRONLY);  
    if (fd < 0) 
    {  
        MSG("Failed to open gpio value for writing!\n");  
        return -1;  
    }  
  
    if (write(fd, &values_str[value == 0 ? 0 : 1], 1) < 0) 
    {  
        MSG("Failed to write value!\n");  
        return -1;  
    }  
  
    close(fd);  
    return 0;  
}
static int gpio_read(int pin)  
{  
    char path[64];  
    char value_str[3];  
    int fd;  
  
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);  
    fd = open(path, O_RDONLY);  
    if (fd < 0) 
    {  
        MSG("Failed to open gpio value for reading!\n");  
        return -1;  
    }  
  
    if (read(fd, value_str, 3) < 0)
    {  
        MSG("Failed to read value!\n");  
        return -1;  
    }  
  
    close(fd);  
    return (atoi(value_str));
}  
// none表示引脚为输入，不是中断引脚
// rising表示引脚为中断输入，上升沿触发
// falling表示引脚为中断输入，下降沿触发
// both表示引脚为中断输入，边沿触发
// 0-->none, 1-->rising, 2-->falling, 3-->both
static int gpio_edge(int pin, int edge)
{
const char dir_str[] = "none\0rising\0falling\0both"; 
char ptr;
char path[64];  
    int fd; 
switch(edge)
{
    case 0:
        ptr = 0;
        break;
    case 1:
        ptr = 5;
        break;
    case 2:
        ptr = 12;
        break;
    case 3:
        ptr = 20;
        break;
    default:
        ptr = 0;
} 
  
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/edge", pin);  
    fd = open(path, O_WRONLY);  
    if (fd < 0) 
    {  
        MSG("Failed to open gpio edge for writing!\n");  
        return -1;  
    }  
  
    if (write(fd, &dir_str[ptr], strlen(&dir_str[ptr])) < 0) 
    {  
        MSG("Failed to set edge!\n");  
        return -1;  
    }  
  
    close(fd);  
    return 0;  
}

int main(int argc, char **argv)
{
    unsigned int *map_base0;
    // unsigned int *map_ctrl0;
    unsigned int *map_base1;
    unsigned int *map_base2;
    unsigned int *map_base3;
    unsigned int *map_base4;
    unsigned int *map_base5;
    unsigned int *map_base6;
    unsigned int *map_base7;
    unsigned int *map_base8;

    unsigned long addr;
    unsigned int content;

    int Length;
    int bazinga;
    int Status;
    int i ;

    int gpio_fd, ret;
    struct pollfd fds[1];
    char buff[10];
    unsigned char cnt = 0;


   /********************BRAMinit****************************************************/

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        printf("can not open /dev/mem \n");
        return (-1);
    }   
    printf("/dev/mem is open \n");
    /*
    mmap（阿巴阿巴阿巴）
    第二个参数表示内存映射的大小、
    第三个参数是一个 flag标志， PROT_READ | PROT_WRITE 的组合表示映射的内存空间是可读可写的、
    第四个参数MAP_SHARED、
    第五个参数表示文件描述符 fd。
     mmap 函数的返回值就等于映射之后得到的实际地址
    */
    map_base0 = mmap(NULL, DATA_LEN * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, XPAR_BRAM_0_BASEADDR);
    map_base1 = mmap(NULL, DATA_LEN * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, XPAR_BRAM_1_BASEADDR);
    map_base2 = mmap(NULL, DATA_LEN * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, XPAR_BRAM_2_BASEADDR);
    map_base3 = mmap(NULL, DATA_LEN * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, XPAR_BRAM_3_BASEADDR);
    map_base4 = mmap(NULL, DATA_LEN * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, XPAR_BRAM_4_BASEADDR);
    map_base5 = mmap(NULL, DATA_LEN * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, XPAR_BRAM_5_BASEADDR);
    map_base6 = mmap(NULL, DATA_LEN * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, XPAR_BRAM_6_BASEADDR);
    map_base7 = mmap(NULL, DATA_LEN * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, XPAR_BRAM_7_BASEADDR);
    map_base8 = mmap(NULL, DATA_LEN * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, XPAR_BRAM_8_BASEADDR);
    if (map_base0 == 0 || map_base1 == 0|| map_base2 == 0|| map_base3 == 0|| map_base4 == 0||
                          map_base5 == 0|| map_base6 == 0|| map_base7 == 0|| map_base8 == 0 ) { 
        printf("NULL pointer\n");
    }   
    else {
        printf("mmap successful\n");
    }   
     
    /****************************GPIOinit********************************************/
    gpio_unexport(958);
    gpio_unexport(959);
 
    /*********************rx handle**************/ 
    gpio_export(958);
    gpio_direction(958, 0);//
    gpio_edge(958,1);
 
    gpio_fd = open("/sys/class/gpio/gpio958/value",O_RDONLY);
    if(gpio_fd < 0)
    {
        MSG("Failed to open value!\n");  
        return -1;  
    }
    fds[0].fd = gpio_fd;
    fds[0].events  = POLLPRI;
    
    /**********************TX handle*******************************/
    gpio_export(959);
    gpio_direction(959, 0);//input in
    gpio_edge(959,1);
    gpio_fd = open("/sys/class/gpio/gpio959/value",O_RDONLY);
    if(gpio_fd < 0)
    {
        MSG("Failed to open value!\n");  
        return -1;  
    }
    fds[0].fd = gpio_fd;
    fds[0].events  = POLLPRI;
    /************************************************************************/
    ret = read(gpio_fd,buff,10);
        if( ret == -1 )
        MSG("read\n");
    
        ret = poll(fds,1,-1);
        if( ret == -1 )
            MSG("poll\n");
        if( fds[0].revents & POLLPRI)
        {
            ret = lseek(gpio_fd,0,SEEK_SET);
            if( ret == -1 )
            MSG("lseek\n");
            printf("TXdone\r\n");
            //gpio_write(44, cnt++%2);
            XBram_Out32(map_base0+2,0);//清除发送中断,实际地址加8
        }


    /********************************sendpacket1********************************/
    printf("\nwrite data to bram\n");//Bram0 在发送地址B2初始化发送1开始的128个累加数
    for (i = 0; i < 128; i++) {
        addr = (unsigned long)(map_base2 + i*4); 
        content = i + 1;
        map_base2[i] = content;
        // printf("TX%2dth data, address: 0x%lx data_write: 0x%x\t\t\n", i, addr, content);

    }   
    XBram_Out32(map_base0+1,0x100);
	XBram_Out32(map_base0+2,0x1);

    printf("before rxread \r\n");
    ret = read(gpio_fd,buff,10);
    if( ret == -1 )
        MSG("read\n");
    printf("before rxpoll \r\n");
    ret = poll(fds,1,-1);
    if( ret == -1 )
        MSG("poll\n");
    if( fds[0].revents & POLLPRI)
    {
        ret = lseek(gpio_fd,0,SEEK_SET);
        if( ret == -1 )
        MSG("lseek\n");

        //gpio_write(44, cnt++%2);
        printf("\nread data from bram\n");//先从B0+8地址取到接收包长，然后再B1接收地址读取打印
        Length = XBram_In32(map_base0+2);
        printf("length1 is 0x%x\r\n",Length);
        for (i = 0; i< Length ; i++) {
            addr = (unsigned long)(map_base1 + i*4); 
            content = map_base1[i];
            printf("RX%2dth data, address: 0x%lx data_read1: 0x%x\t\t\n", i, addr, content);
        }   
        XBram_Out32(map_base0,0x1);
	    XBram_Out32(map_base0,0);//清除接收中断
        XBram_Out32(map_base0+2,0);//清除发送中断,实际地址加8
    }
    else
    printf("poll nothing--------------------------\r\n");
    sleep(3);


    /*发送第二包*/
    printf("\nwrite data2 to bram\n");//Bram0 在发送地址B2初始化发送1开始的128个累加数
    for (i = 0; i < 128; i++) {
        addr = (unsigned long)(map_base2 + i*4); 
        content = i + 2;
        map_base2[i] = content;
        // printf("TX%2dth data, address: 0x%lx data_write: 0x%x\t\t\n", i, addr, content);

    }  
    XBram_Out32(map_base0+1,0x10);//实际地址加4
	XBram_Out32(map_base0+2,0x1);//实际地址加8

    close(fd);
    munmap(map_base0, DATA_LEN);
    munmap(map_base1, DATA_LEN);
    munmap(map_base2, DATA_LEN);
    munmap(map_base3, DATA_LEN);
    munmap(map_base4, DATA_LEN);
    munmap(map_base5, DATA_LEN);
    munmap(map_base6, DATA_LEN);
    munmap(map_base7, DATA_LEN);

    return 0;
}