#pragma once



#ifdef __cplusplus
extern "C" {
#endif



#include "m_fs_conf.h"



typedef enum {
	M_DISK_IO_RES_OK = 0,		/* 0: Function succeeded */
	M_DISK_IO_RES_ERROR,		/* 1: Disk error */
	M_DISK_IO_RES_NOT_RDY,		/* 2: Not ready */
	M_DISK_IO_RES_PAR_ERR		/* 3: Invalid parameter */
} m_disk_io_res_e;



//TODO: convert to CPP
m_disk_io_res_e m_disk_io_init();
m_disk_io_res_e m_disk_io_read(uint8_t* buff, UINT sector, UINT offset, UINT count);
m_disk_io_res_e m_disk_io_write(const uint8_t* buff, UINT sector, UINT offset, UINT count);



#ifdef __cplusplus
}
#endif