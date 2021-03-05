#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

    int Length;
    int Status;
    int bazinga;

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

    unsigned long addr;
    unsigned int content;
    int i = 0;
    /**/
    printf("\nwrite data to bram\n");//Bram0 在发送地址B2初始化发送1开始的128个累加数
    for (i = 0; i < 128; i++) {
        addr = (unsigned long)(map_base2 + i*4); 
        content = i + 1;
        map_base2[i] = content;
        printf("TX%2dth data, address: 0x%lx data_write: 0x%x\t\t\n", i, addr, content);

    }   
    
    map_base0[1] = 0x100;//包长256 接收到64
    map_base0[2] = 0x1;//使能

    
    /**/
    printf("\nread data from bram\n");//先从B0+8地址取到接收包长，然后再B1接收地址读取打印
    Length = map_base0[2];
    //Length = *((unsigned int *) (map_base0 + 8));
    printf("length1 is 0x%x\r\n",Length);
    // bazinga = *((unsigned int *) (map_base0 + 2));
    // printf("bazinga1 is 0x%x\r\n",bazinga);
    for (i = 0; i< Length; i++) {
        addr = (unsigned long)(map_base1 + i*4); 
        content = map_base1[i];
        printf("RX%2dth data, address: 0x%lx data_read1: 0x%x\t\t\n", i, addr, content);
    }   
    sleep(30);
    /*发送第二包*/
     printf("\nwrite2 data to bram\n");//Bram0 在发送地址B2初始化发送1开始的128个累加数
    for (i = 0; i < 128; i++) {
        addr = (unsigned long)(map_base2 + i*4); 
        content = i + 2;
        map_base2[i] = content;
        printf("TX%2dth data, address: 0x%lx data2_write: 0x%x\t\t\n", i, addr, content);

    }   
    map_base0[2] = 0;//清除发送中断
    map_base0[0] = 1;//
    map_base0[0] = 0;//清除接收中断

    map_base0[1] = 0x10;//包长
    map_base0[2] = 0x1;//使能

    printf("\nread data2 from bram\n");//先从B0+8地址取到接收包长，然后再B1接收地址读取打印
    Length = map_base0[2];
    //Length = *((unsigned int *) (map_base0 + 8));
    printf("length2 is 0x%x\r\n",Length);
    for (i = 0; i< Length; i++) {
        addr = (unsigned long)(map_base1 + i*4); 
        content = map_base1[i];
        printf("RX%2dth data, address: 0x%lx data_read2: 0x%x\t\t\n", i, addr, content);
    }   
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


