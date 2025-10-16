#pragma once



#ifdef __cplusplus
extern "C" {
#endif



#include "m_fs_conf.h"



typedef enum
{
	M_FS_RESULT_OK = 0,
	M_FS_RESULT_ERROR,
	M_FS_RESULT_NO_FILE,
} m_fs_result_e;

typedef enum
{
	M_FS_OPEN_MODE_READ = 0b1,
	M_FS_OPEN_MODE_WRITE = 0b10,
	M_FS_OPEN_MODE_APPEND = 0b100,
	M_FS_OPEN_MODE_CREATE = 0b1000,
} m_fs_open_mode_e;



typedef struct 
{
	UINT totalSectors;
	UINT firstFreeSector;
	UINT rootSector;
} m_fs_t;

typedef struct 
{
	UINT startSector;
	UINT sizeB;
	UINT currentIndex;
} m_fs_file_t;





m_fs_result_e m_fs_init(m_fs_t* fs);
m_fs_result_e m_fs_open(const char* name, UINT mode, m_fs_file_t* file);
m_fs_result_e m_fs_write(m_fs_file_t* file, const uint8_t* data, UINT length);
m_fs_result_e m_fs_read(m_fs_file_t* file, uint8_t* buffer, UINT length);
m_fs_result_e m_fs_seek(m_fs_file_t* file, UINT length);
m_fs_result_e m_fs_close(m_fs_file_t* file);



#ifdef __cplusplus
}
#endif