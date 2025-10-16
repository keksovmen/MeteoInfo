#include "m_fs.h"

#include "m_disk_io.h"



static m_fs_t* _fs;



typedef struct
{
	UINT blockSizeB;
	UINT nextBlock;
	UINT prevBlock;
} _block_meta_t;

typedef struct
{
	UINT totalSizeB;
	_block_meta_t meta;
} _file_header_t;



m_fs_result_e m_fs_init(m_fs_t* fs)
{
	if(m_disk_io_init() != M_DISK_IO_RES_OK){
		return M_FS_RESULT_ERROR;
	}
	//do initialization of fs
	fs->firstFreeSector = 256;
	fs->rootSector = 128;

	_fs = fs;

	return M_FS_RESULT_OK;
}

m_fs_result_e m_fs_open(const char* name, UINT mode, m_fs_file_t* file)
{
	UINT sector = _fs->rootSector;
	if(name){
		//find from parent file

		//assume if file does not exist
		if(!(mode & M_FS_OPEN_MODE_CREATE)){
			return M_FS_RESULT_NO_FILE;
		}

		//create file
	}else{
		//assume it exists

	}

	_file_header_t header;
	if(m_disk_io_read(&header, sector, 0, sizeof(_file_header_t)) != M_DISK_IO_RES_OK){
		return M_FS_RESULT_ERROR;
	}
	

	if(header.meta.prevBlock != 0){
		//file does not exist, so memory is not formatted
		header.meta.blockSizeB = 0;
		header.meta.prevBlock = 0;
		header.meta.nextBlock = 0;
		if(m_disk_io_write(&header, sector, 0, sizeof(_file_header_t)) != M_DISK_IO_RES_OK){
			return M_FS_RESULT_ERROR;
		}
		//created our first root file here
		file->sizeB = 0;
	}else{
		//file exists fill up structure
		file->sizeB = header.totalSizeB;
	}

	file->startSector = sector;
	file->currentIndex = 0;


	return M_FS_RESULT_OK;
}

m_fs_result_e m_fs_write(m_fs_file_t* file, const uint8_t* data, UINT length)
{
	_file_header_t header;
	if(m_disk_io_read(&header, file->startSector, 0, sizeof(_file_header_t)) != M_DISK_IO_RES_OK){
		return M_FS_RESULT_ERROR;
	}

	//calculate target sector
	UINT sector = file->startSector;
	if(m_disk_io_write(data, sector, sizeof(_file_header_t) + file->currentIndex, length) != M_DISK_IO_RES_OK){
		return M_FS_RESULT_ERROR;
	}

	file->currentIndex += length;
	file->sizeB += length;
	header.totalSizeB += length;
	header.meta.blockSizeB += length;

	//write start sector with new updated meta
	if(m_disk_io_write(&header, file->startSector, 0, sizeof(_file_header_t)) != M_DISK_IO_RES_OK){
		return M_FS_RESULT_ERROR;
	}

	return M_FS_RESULT_OK;
}

m_fs_result_e m_fs_read(m_fs_file_t* file, uint8_t* buffer, UINT length)
{
	_file_header_t header;
	if(m_disk_io_read(&header, file->startSector, 0, sizeof(_file_header_t)) != M_DISK_IO_RES_OK){
		return M_FS_RESULT_ERROR;
	}

	if(header.totalSizeB < length + file->currentIndex){
		return M_FS_RESULT_ERROR;
	}

	//calculate target sector
	UINT sector = file->startSector;
	if(m_disk_io_read(buffer, sector, sizeof(_file_header_t) + file->currentIndex, length) != M_DISK_IO_RES_OK){
		return M_FS_RESULT_ERROR;
	}

	file->currentIndex += length;
	return M_FS_RESULT_OK;
}

m_fs_result_e m_fs_seek(m_fs_file_t* file, UINT length)
{
	return M_FS_RESULT_OK;
}

m_fs_result_e m_fs_close(m_fs_file_t* file)
{
	return M_FS_RESULT_OK;
}
