#ifndef AXI_BRAM_H
#define AXI_BRAM_H
 
#define SUCCESS                 		0
#define FAILURE                         -1
 
#define MODULE_NAME             		"axi-bram"
 
#define PL_BRAM_STOP					0
#define PL_BRAM_START					1
 
#define AXI_BRAM_COUNT					1	
/* IOCTL defines */
#define AXI_BRAM_IOCTL_BASE				'X'							
#define AXI_BRAM_STOP					_IO(AXI_BRAM_IOCTL_BASE, 0)
#define AXI_BRAM_START					_IO(AXI_BRAM_IOCTL_BASE, 1)
#define AXI_BRAM_DATA_LEN				_IO(AXI_BRAM_IOCTL_BASE, 2)
#define AXI_BRAM_INIT_DATA				_IO(AXI_BRAM_IOCTL_BASE, 3)
#define AXI_BRAM_START_ADDR				_IO(AXI_BRAM_IOCTL_BASE, 4)
#define AXI_BRAAM_DEINIT				_IO(AXI_BRAM_IOCTL_BASE, 5)
 
struct axi_bram_dev
{
	struct mutex mutex;
	struct platform_device *pdev;
	/* PL_bram Hardware device constants */
	void *bram_virtaddr;
	/* pl bram cmd para*/
	int bram_start;
	int bram_data_len;
	int bram_init_data;
	int bram_start_addr;
};
 
#endif