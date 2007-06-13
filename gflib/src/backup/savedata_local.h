//============================================================================================
/**
 * @file	savedata_local.h
 * @brief
 * @date	2006.04.17
 * @author	tamada GAME FREAK inc.
 */
//============================================================================================

#ifndef	__SAVEDATA_LOCAL_H__
#define	__SAVEDATA_LOCAL_H__

#include "heap.h"
#include "savedata.h"

#ifdef	PM_DEBUG
//���̒�`��L���ɂ���ƃt���b�V���̃G���[�`�F�b�N�������ɂȂ�
//�f�o�b�O���ԗp�Ȃ̂Ő��i�łł͔ɉh�����Ȃ����ƁI
//#define	DISABLE_FLASH_CHECK
#endif

//=============================================================================
//=============================================================================
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define	SECTOR_SIZE		(SAVE_SECTOR_SIZE)
#define SECTOR_MAX		(SAVE_PAGE_MAX)

#define	SAVEAREA_SIZE	(SECTOR_SIZE * SECTOR_MAX)
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef u32 (*FUNC_GET_SIZE)(void);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef void (*FUNC_INIT_WORK)(void *);

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^���e�f�[�^�p�̍\���̒�`
 */
//---------------------------------------------------------------------------
typedef struct {
	GMDATA_ID gmdataID;				///<�Z�[�u�f�[�^����ID
	SVBLK_ID	blockID;
	FUNC_GET_SIZE get_size;			///<�Z�[�u�f�[�^�T�C�Y�擾�֐�
	FUNC_INIT_WORK	init_work;		///<�Z�[�u�f�[�^�������֐�
}SAVEDATA_TABLE;

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^���ڂ��Ƃ̏���`
 */
//---------------------------------------------------------------------------
typedef struct {
	GMDATA_ID gmdataID;	///<�Z�[�u�f�[�^����ID
	u32 size;			///<�f�[�^�T�C�Y�i�[
	u32 address;		///<�f�[�^�J�n�ʒu
	u16 crc;			///<�G���[���o�pCRC�R�[�h�i�[
	u16 blockID;		///<�����u���b�N��ID
}SVPAGE_INFO;

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�u���b�N�f�[�^���
 */
//---------------------------------------------------------------------------
typedef struct {
	u32 id;				///<�u���b�N��`ID
	u32 start_sec;		///<�J�n�Z�N�^�ʒu
	u32 use_sec;			///<��L�Z�N�^��
	u32 start_ofs;		///<�Z�[�u�f�[�^�ł̊J�n�I�t�Z�b�g
	u32 size;			///<�Z�[�u�f�[�^�̐�L�T�C�Y
}SVBLK_INFO;


typedef struct _SVDT SVDT;

//=============================================================================
//=============================================================================

extern const SAVEDATA_TABLE SaveDataTable[];
extern const int SaveDataTableMax;


extern SVDT * SVDT_Create(HEAPID heap_id, const SAVEDATA_TABLE * table, u32 table_max, u32 footer_size);

//extern void SVDT_ClearWork(u8 * svwk, const SVPAGE_INFO * pageinfo, u32 savearea_size);
extern void SVDT_ClearWork(u8 * svwk, const SVDT * svdt, u32 savearea_size);
extern void SVDT_MakeBlockIndex(SVBLK_INFO * blkinfo, const SVPAGE_INFO * pageinfo, u32 footer_size);
extern u32 SVDT_GetPageOffset(SVDT * svdt, GMDATA_ID gmdataid);


#endif	/* __SAVEDATA_LOCAL_H__ */
