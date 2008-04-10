//============================================================================================
/**
 * @file	backup_data.c
 * @brief	�o�b�N�A�b�v�f�[�^�̓����\���A�N�Z�X
 * @author	tamada GAME FREAK inc.
 * @date	2007.06.12
 */
//============================================================================================

#include <nitro.h>
#include <nnsys.h>

#include "gflib.h"

#include "assert.h"
#include "backup_system.h"
#include "savedata_local.h"


//============================================================================================
//
//			��`
//
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^���ڂ��Ƃ̏���`
 */
//---------------------------------------------------------------------------
typedef struct {
	GFL_SVDT_ID gmdataID;	///<�Z�[�u�f�[�^����ID
	u32 size;				///<�f�[�^�T�C�Y�i�[
	u32 address;			///<�f�[�^�J�n�ʒu
}SVPAGE_INFO;

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^���ڊǗ��p�̍\���̒�`
 */
//---------------------------------------------------------------------------
struct _SVDT {
	const GFL_SAVEDATA_TABLE * table;	///<�Z�[�u�f�[�^�\����`�f�[�^�ւ̃|�C���^
	u32 table_max;						///<�Z�[�u�f�[�^�\����`�f�[�^�̗v�f��
	u32 footer_size;					///<�Z�[�u�f�[�^�ɕt������t�b�^�̃T�C�Y
	u32 total_size;						///<�Z�[�u�f�[�^�̎��ۂ̃T�C�Y
	u32 savearea_size;					///<���蓖�Ă��Ă���o�b�N�A�b�v�̈�̃T�C�Y
	SVPAGE_INFO * pageinfo;				///<�Z�[�u�f�[�^���ږ��̃f�[�^�ւ̃|�C���^
};

//============================================================================================
//
//			�֐�
//
//============================================================================================
static void SVDT_MakeIndex(SVDT * svdt);

static u32 GetWorkSize(const SVDT * svdt, int id);

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�\���Ǘ��p�f�[�^�𐶐�����
 * @return	svdt	�Z�[�u�f�[�^�\���Ǘ��p�f�[�^�ւ̃|�C���^
 *
 * �^����ꂽ�������f�[�^�Ɋ�Â��ăZ�[�u�f�[�^�Ǘ��p�̃f�[�^�𐶐�����B
 */
//---------------------------------------------------------------------------
SVDT * SVDT_Create(HEAPID heap_id, const GFL_SAVEDATA_TABLE * table, u32 table_max,
		u32 savearea_size, u32 footer_size)
{
	SVDT * svdt;
	svdt = GFL_HEAP_AllocMemory(heap_id, sizeof(SVDT));
	GFL_STD_MemClear32(svdt, sizeof(SVDT));
	svdt->table = table;
	svdt->table_max = table_max;
	svdt->savearea_size = savearea_size;
	svdt->footer_size = footer_size;
	svdt->pageinfo = GFL_HEAP_AllocMemory(heap_id, sizeof(SVPAGE_INFO) * table_max);
	SVDT_MakeIndex(svdt);

	return svdt;
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�\���Ǘ��p�f�[�^�̉������
 * @param	svdt	�Z�[�u�f�[�^�\���Ǘ��p�f�[�^�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
void SVDT_Delete(SVDT * svdt)
{
	GFL_HEAP_FreeMemory(svdt->pageinfo);
	GFL_HEAP_FreeMemory(svdt);
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�̃N���A
 * @param	svwk	�Z�[�u���[�N�ւ̃|�C���^
 * @param	svdt	�Z�[�u�f�[�^�\���Ǘ��p�f�[�^�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
void SVDT_ClearWork(u8 * svwk, const SVDT * svdt)
{
	const GFL_SAVEDATA_TABLE * table = svdt->table;
	u32 table_max = svdt->table_max;
	int i;
	u32 size;
	void * page;
	u32 adrs;


	GFL_STD_MemClear32(svwk, svdt->savearea_size);

	for (i = 0; i <table_max; i++) {
		adrs = svdt->pageinfo[i].address;
		page = &svwk[adrs];
		size = svdt->pageinfo[i].size;
		GFL_STD_MemClear32(page, size);
		table[i].init_work(page);
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static u32 GetWorkSize(const SVDT * svdt, int id)
{
	u32 size;
	const GFL_SAVEDATA_TABLE * table = svdt->table;
	GF_ASSERT(id < svdt->table_max);
	size = table[id].get_size();
	size += 4 - (size % 4);
	return size;
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u���ڂɊւ��鎫�����쐬����
 * @param	svdt	�Z�[�u�f�[�^�\���Ǘ��p�f�[�^�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
static void SVDT_MakeIndex(SVDT * svdt)
{
	const GFL_SAVEDATA_TABLE * table = svdt->table;
	SVPAGE_INFO * pageinfo = svdt->pageinfo;
	int i;
	u32 table_max = svdt->table_max;
	u32 savedata_total_size = 0;


	for (i = 0; i < table_max; i++) {
		//�����_�ł̓Z�[�u�f�[�^�̕��т�ID�̕��т���v���邱�Ƃ�z�肵�Ă���̂�
		//������ꍇ��ASSERT�Œ�~����
		GF_ASSERT(table[i].gmdataID == i);
		pageinfo[i].gmdataID = table[i].gmdataID;
		pageinfo[i].size = GetWorkSize(svdt, i);
		pageinfo[i].address = savedata_total_size;
		savedata_total_size += pageinfo[i].size;
	}
	svdt->total_size = savedata_total_size + svdt->footer_size;
	GF_ASSERT_MSG(svdt->total_size <= svdt->savearea_size,
		"SaveDataSize=%08xbytes, SaveAreaSize=%08xbytes", svdt->total_size, svdt->savearea_size );
}


//============================================================================================
//
//
//
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	�e�Z�[�u���ڂ̑��ΊJ�n�ʒu���擾
 * @param	svdt	�Z�[�u�f�[�^�\���Ǘ��p�f�[�^�ւ̃|�C���^
 * @param	gmdataid
 *
 * �Z�[�u�f�[�^���ł̊e���ڂ̊J�n�A�h���X���擾����
 */
//---------------------------------------------------------------------------
u32 SVDT_GetPageOffset(SVDT * svdt, GFL_SVDT_ID gmdataid)
{
	GF_ASSERT(gmdataid < svdt->table_max);
	return svdt->pageinfo[gmdataid].address;
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�S�̂̑傫�����擾����
 * @param	svdt	�Z�[�u�f�[�^�\���Ǘ��p�f�[�^�ւ̃|�C���^
 *
 * �e���ڂ���L����̈�{�t�b�^�̈�iCRC�Ȃǁj���܂߂����Z�[�u�f�[�^�̃T�C�Y��Ԃ�
 */
//---------------------------------------------------------------------------
u32 SVDT_GetWorkSize(const SVDT * svdt)
{
	return svdt->total_size;
}

