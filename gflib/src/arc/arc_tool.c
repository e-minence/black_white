
//============================================================================================
/**
 * @file	arc_tool.c
 * @bfief	�A�[�J�C�o�p�c�[���v���O����
 * @author	HisashiSogabe
 * @date	05.05.30
 */
//============================================================================================

#include "gflib.h"

#include "arc_tool.h"
#include "arc_tool_def.h"

static	void	ArchiveDataLoadIndex(void *data,const char *name,int index,int ofs,int ofs_size);
static	void	*ArchiveDataLoadIndexMalloc(const char *name,int index,HEAPID heapID,int ofs,int ofs_size);

void	GFL_ARC_sysInit(const char **tbl,int tbl_max);
void	GFL_ARC_sysExit(void);

void	GFL_ARC_DataLoad(void *data,int arcID,int datID);
void	*GFL_ARC_DataLoadMalloc(int arcID,int datID,HEAPID heapID);
void	GFL_ARC_DataLoadOfs(void *data,int arcID,int datID,int ofs,int size);
void	*GFL_ARC_DataLoadMallocOfs(int arcID,int datID,HEAPID heapID,int ofs,int size);
void	*GFL_ARC_DataLoadFilePathMalloc(const char *name,int datID,HEAPID heapID);
u16		GFL_ARC_DataFileCntGet(int arcID,int datID);
u32		GFL_ARC_DataSizeGet(int arcID,int datID);

#define	ALLOC_HEAD	(0)		//�̈���m�ۂ���Ƃ��擪����m��
#define	ALLOC_TAIL	(1)		//�̈���m�ۂ���Ƃ���납��m��

//============================================================================================
//	�A�[�J�C�u�e�[�u���i�[�ϐ�
//============================================================================================

static	char	***ArchiveFileTable=NULL;
static	int		ArchiveFileTableMax=0;

//============================================================================================
/**
 *
 *	�A�[�J�C�u�V�X�e��������
 *
 * @param[in]	tbl		�A�[�J�C�u�f�[�^�e�[�u���̃A�h���X
 * @param[in]	tbl_max	�A�[�J�C�u�f�[�^�e�[�u���̗v�f����MAX
 *
 */
//============================================================================================
void	GFL_ARC_sysInit(const char **tbl,int tbl_max)
{
	ArchiveFileTable=(char ***)tbl;
	ArchiveFileTableMax=tbl_max;
}

//============================================================================================
/**
 *
 *	�A�[�J�C�u�V�X�e���I��
 *
 */
//============================================================================================
void	GFL_ARC_sysExit(void)
{
}

//============================================================================================
/**
 *
 *	nnsarc�ō쐬�����A�[�J�C�u�t�@�C���ɑ΂���Index���w�肵�ĔC�ӂ̃f�[�^�����o��
 *
 * @param[in]	data		�ǂݍ��񂾃f�[�^���i�[���郏�[�N�̃|�C���^
 * @param[in]	name		�ǂݍ��ރA�[�J�C�u�t�@�C����
 * @param[in]	index		�ǂݍ��ރf�[�^�̃A�[�J�C�u��̃C���f�b�N�X�i���o�[
 * @param[in]	ofs			�ǂݍ��ރf�[�^�̐擪����̃I�t�Z�b�g
 * @param[in]	ofs_size	�ǂݍ��ރf�[�^�T�C�Y
 *
 */
//============================================================================================
static	void	ArchiveDataLoadIndex(void *data,const char *name,int index,int ofs,int ofs_size)
{
	FSFile		p_file;
	u32			size=0;
	u32			fat_top=0;
	u32			fnt_top=0;
	u32			img_top=0;
	u32			top=0;
	u32			bottom=0;
	u16			file_cnt=0;

	FS_InitFile(&p_file);
	FS_OpenFile(&p_file,name);
	FS_SeekFile(&p_file,ARC_HEAD_SIZE_POS,FS_SEEK_SET);				///<�A�[�J�C�u�w�b�_�̃T�C�Y�i�[�ʒu�Ɉړ�
	FS_ReadFile(&p_file,&size,2);									///<�A�[�J�C�u�w�b�_�T�C�Y�����[�h
	fat_top=size;
	FS_SeekFile(&p_file,fat_top+SIZE_OFFSET,FS_SEEK_SET);			///<FAT�̃T�C�Y�i�[�ʒu�Ɉړ�
	FS_ReadFile(&p_file,&size,4);									///<FAT�T�C�Y�����[�h
	FS_ReadFile(&p_file,&file_cnt,2);								///<FileCount�����[�h
	GF_ASSERT_MSG(file_cnt>index,"ServerArchiveDataLoadIndex file=%s, fileCnt=%d, index=%d", name, file_cnt, index);
	fnt_top=fat_top+size;
	FS_SeekFile(&p_file,fnt_top+SIZE_OFFSET,FS_SEEK_SET);			///<FNT�̃T�C�Y�i�[�ʒu�Ɉړ�
	FS_ReadFile(&p_file,&size,4);									///<FNT�T�C�Y�����[�h
	img_top=fnt_top+size;
	
	FS_SeekFile(&p_file,fat_top+FAT_HEAD_SIZE+index*8,FS_SEEK_SET);	///<���o������FAT�e�[�u���Ɉړ�
	FS_ReadFile(&p_file,&top,4);									///<FAT�e�[�u��top�����[�h
	FS_ReadFile(&p_file,&bottom,4);									///<FAT�e�[�u��bottom�����[�h

	FS_SeekFile(&p_file,img_top+IMG_HEAD_SIZE+top+ofs,FS_SEEK_SET);	///<���o������IMG�̐擪�Ɉړ�
	
	if(ofs_size){
		size=ofs_size;
	}
	else{
		size=bottom-top;
	}

	GF_ASSERT_MSG(size!=0,"ServerArchiveDataLoadIndex:ReadDataSize=0!");
	FS_ReadFile(&p_file,data,size);									///<�f�[�^�����[�h

	FS_CloseFile(&p_file);
}


//============================================================================================
/**
 *
 *	nnsarc�ō쐬�����A�[�J�C�u�t�@�C���ɑ΂���Index���w�肵�ĔC�ӂ̃f�[�^�����o��
 *	�ǂݍ��񂾃f�[�^���i�[���郏�[�N�����̊֐����Ŋm�ۂ��āA�|�C���^��Ԃ�
 *
 * @param[in]	name		�ǂݍ��ރA�[�J�C�u�t�@�C����
 * @param[in]	index		�ǂݍ��ރf�[�^�̃A�[�J�C�u��̃C���f�b�N�X�i���o�[
 * @param[in]	heapID		���������m�ۂ���q�[�v�̈��ID
 * @param[in]	ofs			�ǂݍ��ރf�[�^�̐擪����̃I�t�Z�b�g
 * @param[in]	ofs_size	�ǂݍ��ރf�[�^�T�C�Y
 *
 */
//============================================================================================
static	void	*ArchiveDataLoadIndexMalloc(const char *name,int index,HEAPID heapID,int ofs,int ofs_size)
{
	FSFile		p_file;
	u32			size=0;
	u32			fat_top=0;
	u32			fnt_top=0;
	u32			img_top=0;
	u32			top=0;
	u32			bottom=0;
	void		*data;
	u16			file_cnt=0;

	FS_InitFile(&p_file);
	FS_OpenFile(&p_file,name);
	FS_SeekFile(&p_file,ARC_HEAD_SIZE_POS,FS_SEEK_SET);				///<�A�[�J�C�u�w�b�_�̃T�C�Y�i�[�ʒu�Ɉړ�
	FS_ReadFile(&p_file,&size,2);									///<�A�[�J�C�u�w�b�_�T�C�Y�����[�h
	fat_top=size;
	FS_SeekFile(&p_file,fat_top+SIZE_OFFSET,FS_SEEK_SET);			///<FAT�̃T�C�Y�i�[�ʒu�Ɉړ�
	FS_ReadFile(&p_file,&size,4);									///<FAT�T�C�Y�����[�h
	FS_ReadFile(&p_file,&file_cnt,2);								///<FileCount�����[�h
	GF_ASSERT_MSG(file_cnt>index,"ServerArchiveDataLoadIndex file=%s, fileCnt=%d, index=%d", name, file_cnt, index);
	fnt_top=fat_top+size;
	FS_SeekFile(&p_file,fnt_top+SIZE_OFFSET,FS_SEEK_SET);			///<FNT�̃T�C�Y�i�[�ʒu�Ɉړ�
	FS_ReadFile(&p_file,&size,4);									///<FNT�T�C�Y�����[�h
	img_top=fnt_top+size;
	
	FS_SeekFile(&p_file,fat_top+FAT_HEAD_SIZE+index*8,FS_SEEK_SET);	///<���o������FAT�e�[�u���Ɉړ�
	FS_ReadFile(&p_file,&top,4);									///<FAT�e�[�u��top�����[�h
	FS_ReadFile(&p_file,&bottom,4);									///<FAT�e�[�u��bottom�����[�h

	FS_SeekFile(&p_file,img_top+IMG_HEAD_SIZE+top+ofs,FS_SEEK_SET);		///<���o������IMG�̐擪�Ɉړ�

	if(ofs_size){
		size=ofs_size;
	}
	else{
		size=bottom-top;
	}
	GF_ASSERT_MSG(size!=0,"ServerArchiveDataLoadIndex:ReadDataSize=0!");

	data=GFL_HEAP_AllocMemory(heapID,size);

	FS_ReadFile(&p_file,data,size);									///<�f�[�^�����[�h

	FS_CloseFile(&p_file);

	return data;
}

//============================================================================================
/**
 * �A�[�J�C�u�t�@�C���f�[�^���[�h
 *
 * @param[in]	data		�ǂݍ��񂾃f�[�^���i�[���郏�[�N�̃|�C���^
 * @param[in]	arcID		�ǂݍ��ރA�[�J�C�u�t�@�C���̎�ރC���f�b�N�X�i���o�[�iarc_tool.h�ɋL�q�j
 * @param[in]	datID		�ǂݍ��ރf�[�^�̃A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 */
//============================================================================================
void	GFL_ARC_DataLoad(void *data, int arcID, int datID)
{
	ArchiveDataLoadIndex(data, (char *)ArchiveFileTable[arcID], datID, OFS_NO_SET, SIZE_NO_SET);
}

//============================================================================================
/**
 * �A�[�J�C�u�t�@�C���f�[�^���[�h�i�ǂݍ��񂾃f�[�^���i�[���郏�[�N���֐����Ŋm�ۂ���o�[�W�����j
 *
 * ���֐����Ń��[�N��Alloc����̂ŁA���O�ŊJ������������K�v������܂�(sys_FreeMemoryEz(�̈�|�C���^)�ŊJ���j
 *
 * @param[in]	arcID		�ǂݍ��ރA�[�J�C�u�t�@�C���̎�ރC���f�b�N�X�i���o�[�iarc_tool.h�ɋL�q�j
 * @param[in]	datID		�ǂݍ��ރf�[�^�̃A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 * @param[in]	heapID		���������m�ۂ���q�[�v�̈��ID
 *
 * @retval	�֐����Ŋm�ۂ����f�[�^�i�[���[�N�̃A�h���X
 */
//============================================================================================
void* GFL_ARC_DataLoadMalloc(int arcID, int datID, HEAPID heapID)
{
	return	ArchiveDataLoadIndexMalloc((char *)ArchiveFileTable[arcID],datID,heapID,OFS_NO_SET,SIZE_NO_SET);
}

//============================================================================================
/**
 * �A�[�J�C�u�t�@�C���f�[�^���[�h�i�I�t�Z�b�g�w�肠��j
 *
 * @param[in]	data		�ǂݍ��񂾃f�[�^���i�[���郏�[�N�̃|�C���^
 * @param[in]	arcID		�ǂݍ��ރA�[�J�C�u�t�@�C���̎�ރC���f�b�N�X�i���o�[�iarc_tool.h�ɋL�q�j
 * @param[in]	datID		�ǂݍ��ރf�[�^�̃A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 * @param[in]	ofs			�ǂݍ��ރf�[�^�̐擪����̃I�t�Z�b�g
 * @param[in]	size		�ǂݍ��ރf�[�^�T�C�Y
 */
//============================================================================================
void GFL_ARC_DataLoadOfs(void *data, int arcID, int datID, int ofs, int size)
{
	ArchiveDataLoadIndex(data, (char *)ArchiveFileTable[arcID], datID, ofs, size);
}

//============================================================================================
/**
 * �A�[�J�C�u�t�@�C���f�[�^���[�h�i�ǂݍ��񂾃f�[�^���i�[���郏�[�N���֐����Ŋm�ۂ���o�[�W�����ƃI�t�Z�b�g�w�肠��j
 *
 * ���֐����Ń��[�N��Alloc����̂ŁA���O�ŊJ������������K�v������܂�(sys_FreeMemoryEz(�̈�|�C���^)�ŊJ���j
 *
 * @param[in]	arcID		�ǂݍ��ރA�[�J�C�u�t�@�C���̎�ރC���f�b�N�X�i���o�[�iarc_tool.h�ɋL�q�j
 * @param[in]	datID		�ǂݍ��ރf�[�^�̃A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 * @param[in]	heapID		���������m�ۂ���q�[�v�̈��ID
 * @param[in]	ofs			�ǂݍ��ރf�[�^�̐擪����̃I�t�Z�b�g
 * @param[in]	size		�ǂݍ��ރf�[�^�T�C�Y
 *
 * @retval	�֐����Ŋm�ۂ����f�[�^�i�[���[�N�̃A�h���X
 */
//============================================================================================
void* GFL_ARC_DataLoadMallocOfs(int arcID, int datID, HEAPID heapID, int ofs, int size)
{
	return	ArchiveDataLoadIndexMalloc((char *)ArchiveFileTable[arcID],datID,heapID,ofs,size);
}

//============================================================================================
/**
 *
 *	nnsarc�ō쐬�����A�[�J�C�u�t�@�C���ɑ΂��Ē��ڃt�@�C�������w�肵�ĔC�ӂ̃f�[�^�����o��
 *	�ǂݍ��񂾃f�[�^���i�[���郏�[�N�����̊֐����Ŋm�ۂ��āA�|�C���^��Ԃ�
 *
 * @param[in]	name		�ǂݍ��ރA�[�J�C�u�t�@�C����
 * @param[in]	index		�ǂݍ��ރf�[�^�̃A�[�J�C�u��̃C���f�b�N�X�i���o�[
 * @param[in]	heapID		���������m�ۂ���q�[�v�̈��ID
 *
 */
//============================================================================================
void	*GFL_ARC_DataLoadFilePathMalloc(const char *name,int datID,HEAPID heapID)
{
	return	ArchiveDataLoadIndexMalloc(name,datID,heapID,OFS_NO_SET,SIZE_NO_SET);
}

//============================================================================================
/**
 * �A�[�J�C�u�t�@�C���f�[�^�̃t�@�C�������擾
 *
 * @param[in]	arcID	�ǂݍ��ރA�[�J�C�u�t�@�C���̎�ރC���f�b�N�X�i���o�[�iarc_tool.h�ɋL�q�j
 * @param[in]	datID		�ǂݍ��ރf�[�^�̃A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 */
//============================================================================================
u16		GFL_ARC_DataFileCntGet(int arcID, int datID)
{
	FSFile		p_file;
	u32			size=0;
	u32			fat_top=0;
	u16			file_cnt=0;

	FS_InitFile(&p_file);
	FS_OpenFile(&p_file,(char *)ArchiveFileTable[arcID]);
	FS_SeekFile(&p_file,ARC_HEAD_SIZE_POS,FS_SEEK_SET);				///<�A�[�J�C�u�w�b�_�̃T�C�Y�i�[�ʒu�Ɉړ�
	FS_ReadFile(&p_file,&size,2);									///<�A�[�J�C�u�w�b�_�T�C�Y�����[�h
	fat_top=size;
	FS_SeekFile(&p_file,fat_top+SIZE_OFFSET,FS_SEEK_SET);			///<FAT�̃T�C�Y�i�[�ʒu�Ɉړ�
	FS_ReadFile(&p_file,&size,4);									///<FAT�T�C�Y�����[�h
	FS_ReadFile(&p_file,&file_cnt,2);								///<FileCount�����[�h

	return	file_cnt;
}

//============================================================================================
/**
 * �A�[�J�C�u�t�@�C���f�[�^�̃T�C�Y���擾
 *
 *	ArchiveDataLoad���g�p���鎞�A�m�ۂ��郁�����T�C�Y���擾����̂Ɏg�p���܂�
 *
 * @param[in]	arcID		�ǂݍ��ރA�[�J�C�u�t�@�C���̎�ރC���f�b�N�X�i���o�[�iarc_tool.h�ɋL�q�j
 * @param[in]	datID		�ǂݍ��ރf�[�^�̃A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 */
//============================================================================================
u32		GFL_ARC_DataSizeGet(int arcID,int datID)
{
	FSFile		p_file;
	u32			size=0;
	u32			fat_top=0;
	u32			fnt_top=0;
	u32			img_top=0;
	u32			top=0;
	u32			bottom=0;
	void		*data;
	u16			file_cnt=0;

	FS_InitFile(&p_file);
	FS_OpenFile(&p_file,(char *)ArchiveFileTable[arcID]);
	FS_SeekFile(&p_file,ARC_HEAD_SIZE_POS,FS_SEEK_SET);				///<�A�[�J�C�u�w�b�_�̃T�C�Y�i�[�ʒu�Ɉړ�
	FS_ReadFile(&p_file,&size,2);									///<�A�[�J�C�u�w�b�_�T�C�Y�����[�h
	fat_top=size;
	FS_SeekFile(&p_file,fat_top+SIZE_OFFSET,FS_SEEK_SET);			///<FAT�̃T�C�Y�i�[�ʒu�Ɉړ�
	FS_ReadFile(&p_file,&size,4);									///<FAT�T�C�Y�����[�h
	FS_ReadFile(&p_file,&file_cnt,2);								///<FileCount�����[�h
	GF_ASSERT_MSG(file_cnt>datID,"ServerArchiveDataLoadIndex:FileCnt<Index");
	fnt_top=fat_top+size;
	FS_SeekFile(&p_file,fnt_top+SIZE_OFFSET,FS_SEEK_SET);			///<FNT�̃T�C�Y�i�[�ʒu�Ɉړ�
	FS_ReadFile(&p_file,&size,4);									///<FNT�T�C�Y�����[�h
	img_top=fnt_top+size;
	
	FS_SeekFile(&p_file,fat_top+FAT_HEAD_SIZE+datID*8,FS_SEEK_SET);	///<���o������FAT�e�[�u���Ɉړ�
	FS_ReadFile(&p_file,&top,4);									///<FAT�e�[�u��top�����[�h
	FS_ReadFile(&p_file,&bottom,4);									///<FAT�e�[�u��bottom�����[�h

	FS_SeekFile(&p_file,img_top+IMG_HEAD_SIZE+top,FS_SEEK_SET);		///<���o������IMG�̐擪�Ɉړ�
	size=bottom-top;
	GF_ASSERT_MSG(size!=0,"ServerArchiveDataLoadIndex:ReadDataSize=0!");

	return	size;
}




//--------------------------------------------------------
/**
 * �A�[�J�C�u�t�@�C���̃n���h���Ǘ��\����
 */
//--------------------------------------------------------
struct _ARCHANDLE{
	FSFile  file;
	u32     fat_top;
	u32     img_top;
	u16		file_cnt;
};


//------------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^�̃t�@�C���n���h���I�[�v��
 *
 * @param   arcID		�A�[�J�C�u�f�[�^�C���f�b�N�X
 * @param   heapID		�Ǘ��p�q�[�v�h�c
 *
 * @retval  ARCHANDLE	�I�[�v�����ꂽ�n���h���̃|�C���^�i���s�Ȃ�NULL�j
 */
//------------------------------------------------------------------
ARCHANDLE* GFL_ARC_DataHandleOpen( u32 arcID, HEAPID heapID )
{
	ARCHANDLE* handle = GFL_HEAP_AllocMemory( heapID, sizeof(ARCHANDLE) );
	if( handle )
	{
		u32 fnt_top, tmp;

		// �Q�o�C�g��ǂݍ��񂾎��ɃS�~������Ȃ��悤�ɂ��Ă���
		handle->fat_top = 0;

		FS_InitFile( &(handle->file) );
		FS_OpenFile( &(handle->file), (char *)ArchiveFileTable[arcID] );
		FS_SeekFile( &(handle->file), ARC_HEAD_SIZE_POS, FS_SEEK_SET );
		FS_ReadFile( &(handle->file), &(handle->fat_top), 2 );
		FS_SeekFile( &(handle->file), handle->fat_top+SIZE_OFFSET, FS_SEEK_SET );
		FS_ReadFile( &(handle->file), &tmp, 4 );
		FS_ReadFile( &(handle->file), &(handle->file_cnt), 2 );
		fnt_top = handle->fat_top + tmp;
		FS_SeekFile( &(handle->file), fnt_top+SIZE_OFFSET, FS_SEEK_SET );
		FS_ReadFile( &(handle->file), &tmp, 4 );
		handle->img_top = fnt_top + tmp;

	}
	return handle;
}

//------------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^�̃n���h���N���[�Y
 *
 * @param   handle			�n���h���|�C���^
 *
 */
//------------------------------------------------------------------
void	GFL_ARC_DataHandleClose( ARCHANDLE* handle )
{
	FS_CloseFile( &(handle->file) );
	GFL_HEAP_FreeMemory( handle );
}

//------------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^�n���h�����g���ăf�[�^�擾�i�����ŃA���P�[�g�����������Ɂj
 *
 * @param   handle			�n���h���|�C���^
 * @param   datID			�A�[�J�C�u���̃f�[�^�C���f�b�N�X
 * @param   heapID			�������A���P�[�g�p�̃q�[�v�h�c
 *
 * @retval  u32				�f�[�^�T�C�Y�i�o�C�g�j
 */
//------------------------------------------------------------------
void* GFL_ARC_DataLoadAllocByHandle( ARCHANDLE* handle, u32 datId, HEAPID heapID )
{
	u32 top, bottom;
	void* buf;

	GF_ASSERT_MSG(handle->file_cnt>datId, "DatCount=%d, DatID=%d", handle->file_cnt, datId);

	FS_SeekFile( &(handle->file), handle->fat_top+FAT_HEAD_SIZE+datId*8, FS_SEEK_SET );
	FS_ReadFile( &(handle->file), &top, 4 );
	FS_ReadFile( &(handle->file), &bottom, 4 );
	FS_SeekFile( &(handle->file), handle->img_top+IMG_HEAD_SIZE+top, FS_SEEK_SET );

	buf = GFL_HEAP_AllocMemory(heapID, bottom-top);
	if( buf )
	{
		FS_ReadFile( &(handle->file), buf, bottom-top );
	}

	return buf;
}

//------------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^�n���h�����g���ăf�[�^�擾
 *
 * @param   handle		�n���h���|�C���^
 * @param   datId		�A�[�J�C�u���̃f�[�^�C���f�b�N�X
 * @param   buffer		�f�[�^�ǂݍ��ݐ�o�b�t�@
 *
 */
//------------------------------------------------------------------
void GFL_ARC_DataLoadByHandle( ARCHANDLE* handle, u32 datId, void* buffer )
{
	u32 top, bottom;
	
	GF_ASSERT_MSG(handle->file_cnt>datId, "DatCount=%d, DatID=%d", handle->file_cnt, datId);
	
	FS_SeekFile( &(handle->file), handle->fat_top+FAT_HEAD_SIZE+datId*8, FS_SEEK_SET );
	FS_ReadFile( &(handle->file), &top, 4 );
	FS_ReadFile( &(handle->file), &bottom, 4 );
	FS_SeekFile( &(handle->file), handle->img_top+IMG_HEAD_SIZE+top, FS_SEEK_SET );
	
	FS_ReadFile( &(handle->file), buffer, bottom-top );
}

//------------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^�n���h�����g���ăf�[�^�T�C�Y�擾
 *
 * @param   handle		�n���h���|�C���^
 * @param   datId		�A�[�J�C�u���̃f�[�^�C���f�b�N�X
 * @retval	u32			�C���f�b�N�X�̃f�[�^�T�C�Y
 *
 */
//------------------------------------------------------------------
u32 GFL_ARC_DataSizeGetByHandle( ARCHANDLE* handle, u32 datId )
{
	u32 top, bottom;
	
	GF_ASSERT_MSG(handle->file_cnt>datId, "DatCount=%d, DatID=%d", handle->file_cnt, datId);
	
	FS_SeekFile( &(handle->file), handle->fat_top+FAT_HEAD_SIZE+datId*8, FS_SEEK_SET );
	FS_ReadFile( &(handle->file), &top, 4 );
	FS_ReadFile( &(handle->file), &bottom, 4 );
	
	return( bottom-top );
}

//------------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^�n���h�����g���ăf�[�^�擾�i�ǂݍ��݊J�n�I�t�Z�b�g���T�C�Y�w��j
 *
 * @param   handle		�n���h���|�C���^
 * @param   datId		�A�[�J�C�u���̃f�[�^�C���f�b�N�X
 * @param   ofs			�ǂݍ��݊J�n�I�t�Z�b�g
 * @param   size		�ǂݍ��݃T�C�Y
 * @param   buffer		�f�[�^�ǂݍ��ݐ�o�b�t�@
 *
 */
//------------------------------------------------------------------
void GFL_ARC_DataLoadOfsByHandle( ARCHANDLE* handle, u32 datId, u32 ofs, u32 size, void* buffer )
{
	u32 top;

	GF_ASSERT_MSG(handle->file_cnt>datId, "DatCount=%d, DatID=%d", handle->file_cnt, datId);

	FS_SeekFile( &(handle->file), handle->fat_top+FAT_HEAD_SIZE+datId*8, FS_SEEK_SET );
	FS_ReadFile( &(handle->file), &top, 4 );
	FS_SeekFile( &(handle->file), handle->img_top+IMG_HEAD_SIZE+top+ofs, FS_SEEK_SET );

	FS_ReadFile( &(handle->file), buffer, size );
}

//------------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^�n���h�����g���ăC���[�W�f�[�^�I�t�Z�b�g�擾
 *
 * @param   handle		�n���h���|�C���^
 * @param   datId		�A�[�J�C�u���̃f�[�^�C���f�b�N�X
 * @param   offset		�I�t�Z�b�g�ǂݍ��ݐ�o�b�t�@
 *
 */
//------------------------------------------------------------------
void GFL_ARC_DataLoadImgofsByHandle( ARCHANDLE* handle, u32 datId, u32* offset )
{
	u32 top;

	GF_ASSERT_MSG(handle->file_cnt>datId, "DatCount=%d, DatID=%d", handle->file_cnt, datId);

	FS_SeekFile( &(handle->file), handle->fat_top+FAT_HEAD_SIZE+datId*8, FS_SEEK_SET );
	FS_ReadFile( &(handle->file), &top, 4 );
	
	*offset = handle->img_top+IMG_HEAD_SIZE+top;
}

//------------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^�n���h�����g���ăf�[�^�擾�i�t�@�C���n���h�������̑�������ǂݍ��݁j
 *
 * @param   handle		�n���h���|�C���^
 * @param   size		�ǂݍ��݃T�C�Y
 * @param   buffer		�f�[�^�ǂݍ��ݐ�o�b�t�@
 *
 */
//------------------------------------------------------------------
void GFL_ARC_DataLoadByHandleContinue( ARCHANDLE* handle, u32 size, void* buffer )
{
	FS_ReadFile( &(handle->file), buffer, size );
}

//------------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^�n���h������ăt�@�C���V�[�N
 *
 * @param   handle		�n���h���|�C���^
 * @param   size
 *
 */
//------------------------------------------------------------------
void GFL_ARC_DataSeekByHandle( ARCHANDLE* handle, u32 size )
{
	FS_SeekFile( &(handle->file), size, FS_SEEK_SET );
}

//============================================================================================
/**
 * �A�[�J�C�u�t�@�C���f�[�^�̃t�@�C�������擾(�n���h���g�p)
 *
 * @param	handle			�n���h���|�C���^
 * @retval	u16				�t�@�C����	
 */
//============================================================================================
u16		GFL_ARC_DataFileCntGetByHandle(ARCHANDLE* handle)
{
	return	handle->file_cnt;
}
