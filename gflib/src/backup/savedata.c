//============================================================================================
/**
 */
//============================================================================================

#include <nitro.h>
#include <nnsys.h>

#include "gflib.h"

#include "assert.h"
#include "savedata_local.h"
//============================================================================================
//============================================================================================
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
struct _SVDT {
	const SAVEDATA_TABLE * table;
	u32 table_max;
	u32 footer_size;
	SVPAGE_INFO pageinfo[GMDATA_ID_MAX];
	SVBLK_INFO blkinfo[SVBLK_ID_MAX];
};

//============================================================================================
//============================================================================================
static void SVDT_MakeIndex(SVPAGE_INFO * pageinfo, u32 footer_size);
static u32 GetWorkSize(int id);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
SVDT * SVDT_Create(HEAPID heap_id, const SAVEDATA_TABLE * table, u32 table_max, u32 footer_size)
{
	SVDT * svdt;
	svdt = GFL_HEAP_AllocMemory(heap_id, sizeof(SVDT));
	GFL_STD_MemClear32(svdt, sizeof(SVDT));
	svdt->table = table;
	svdt->table_max = table_max;
	svdt->footer_size = footer_size;
	SVDT_MakeIndex(svdt->pageinfo, svdt->footer_size);
	SVDT_MakeBlockIndex(svdt->blkinfo, svdt->pageinfo, svdt->footer_size);

	return svdt;
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�̃N���A
 * @param	svwk	�Z�[�u���[�N�ւ̃|�C���^
 * @param	pageinfo	�Z�[�u���ڕێ����[�N
 * @param	savearea_size	�Z�[�u���[�N�̃T�C�Y
 */
//---------------------------------------------------------------------------
void SVDT_ClearWork(u8 * svwk, const SVDT * svdt, u32 savearea_size)
{
	const SAVEDATA_TABLE * table = SaveDataTable;
	int i;
	u32 size;
	void * page;
	u32 adrs;


	MI_CpuClearFast(svwk, savearea_size);

	for (i = 0; i <SaveDataTableMax; i++) {
		adrs = svdt->pageinfo[i].address;
		page = &svwk[adrs];
		size = svdt->pageinfo[i].size;
		MI_CpuClearFast(page, size);
		table[i].init_work(page);
	}

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static u32 GetWorkSize(int id)
{
	u32 size;
	const SAVEDATA_TABLE * table = SaveDataTable;
	GF_ASSERT(id < SaveDataTableMax);
	size = table[id].get_size();
	size += 4 - (size % 4);
	return size;
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u���ڂɊւ��鎫�����쐬����
 * @param	pageinfo	�Z�[�u���ڕێ����[�N
 */
//---------------------------------------------------------------------------
static void SVDT_MakeIndex(SVPAGE_INFO * pageinfo, u32 footer_size)
{
	const SAVEDATA_TABLE * table = SaveDataTable;
	int i;
	u32 savedata_total_size = 0;


	GF_ASSERT(SaveDataTableMax == GMDATA_ID_MAX);

	for (i = 0; i < SaveDataTableMax; i++) {
		GF_ASSERT(table[i].gmdataID == i);
		pageinfo[i].gmdataID = table[i].gmdataID;
		pageinfo[i].size = GetWorkSize(i);
		pageinfo[i].address = savedata_total_size;
		pageinfo[i].crc = 0;
		pageinfo[i].blockID = table[i].blockID;
		savedata_total_size += pageinfo[i].size;
		if (i == SaveDataTableMax - 1 || table[i].blockID != table[i+1].blockID) {
			//��ԍŌ�A���邢�̓u���b�N���؂�ւ��Ƃ���ł�
			//�t�b�^�p�̈�̕��A�h���X�����߂Ă���
			savedata_total_size += footer_size;
		}
#ifdef	DEBUG_ONLY_FOR_tamada
		if (i % 4 == 0) {
			OS_TPrintf("SVDT:");
		}
		OS_TPrintf("%02d:%02d:%05x(%05x) ", pageinfo[i].gmdataID, pageinfo[i].blockID,
				pageinfo[i].address, pageinfo[i].size);
		if (i % 4 == 3) {
			OS_PutString("\n");
		}
#endif
	}
	if (i % 4 != 0) {
		OS_PutString("\n");
	}
	GF_ASSERT(savedata_total_size <= SAVEAREA_SIZE);
}


//============================================================================================
//
//
//
//============================================================================================
#define	SEC_DATA_SIZE		SECTOR_SIZE
//---------------------------------------------------------------------------
/**
 * @brief	�u���b�N�Ɋւ��鎫�����쐬����
 * @param	blkinfo		�u���b�N���ڕێ����[�N
 * @param	pageinfo	�Z�[�u���ڕێ����[�N
 */
//---------------------------------------------------------------------------
void SVDT_MakeBlockIndex(SVBLK_INFO * blkinfo, const SVPAGE_INFO * pageinfo, u32 footer_size)
{
	int blk_count = 0;
	u32 total_sec, address;
	u32 i, page;

	total_sec = 0;
	address = 0;
	page = 0;
	for (i = 0; i < SVBLK_ID_MAX; i ++) {
		blkinfo[i].id = i;
		//�f�[�^�T�C�Y���v�Z
		blkinfo[i].size = 0;
		for (; pageinfo[page].blockID == i && page < SaveDataTableMax; page++) {
			blkinfo[i].size += pageinfo[page].size;
		}
		blkinfo[i].size += footer_size;
		//�Z�N�^�J�n�i���o�[����
		blkinfo[i].start_sec = total_sec;
		//�Z�[�u���[�N�ł̊J�n�A�h���X����
		blkinfo[i].start_ofs = address;
		//�g�p�Z�N�^���i�؂�グ�j����
		blkinfo[i].use_sec = (blkinfo[i].size + SEC_DATA_SIZE - 1) / SEC_DATA_SIZE;

		total_sec += blkinfo[i].use_sec;
		address += blkinfo[i].size;
#ifdef	DEBUG_ONLY_FOR_tamada
		OS_TPrintf("SVBLK:%d:%08x(%02x) size:%05x(%02x)\n",i,
				blkinfo[i].start_ofs, blkinfo[i].start_sec,
				blkinfo[i].size, blkinfo[i].use_sec);
#endif
	}

	GF_ASSERT(total_sec == blkinfo[SVBLK_ID_MAX-1].start_sec + blkinfo[SVBLK_ID_MAX-1].use_sec);
	GF_ASSERT(total_sec <= SECTOR_MAX);
}


extern u32 SVDT_GetPageOffset(SVDT * svdt, GMDATA_ID gmdataid)
{
	GF_ASSERT(gmdataid < GMDATA_ID_MAX);
	return svdt->pageinfo[gmdataid].address;
}
