//============================================================================================
/**
 * @file	saveload_system.c
 * @brief	�Z�[�u���[�h�V�X�e��
 * @author	tamada	GAME FREAK Inc.
 * @date	2005.10.12
 *
 * @li		2006.04.17	savedata.c���番��
 */
//============================================================================================

//#include "common.h"
#include <nitro.h>
#include <nnsys.h>
//#include "gflib/system.h"

#include "savedata.h"
#include "savedata_local.h"

#include "gflib.h"
#include "heapsys.h"

#include "flash_access.h"

//#include "application/backup.h"	//SaveErrorWarningCall


//=============================================================================
//=============================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define MAGIC_NUMBER	(0x20060623)
#define	SECTOR_SIZE		(SAVE_SECTOR_SIZE)
#define SECTOR_MAX		(SAVE_PAGE_MAX)

#define	FIRST_MIRROR_START	(0)
#define	SECOND_MIRROR_START	(64)

#define MIRROR1ST	(0)
#define	MIRROR2ND	(1)
#define	MIRRORERROR	(2)

#define HEAPID_SAVE_TEMP	(HEAPID_BASE_APP)


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define	SEC_DATA_SIZE		SECTOR_SIZE


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef struct {
	u32 g_count;		///<�O���[�o���J�E���^�iMY�f�[�^�ABOX�f�[�^���L�j
	u32 b_count;		///<�u���b�N���J�E���^�iMY�f�[�^��BOX�f�[�^�ƂœƗ��j
	u32 size;			///<�f�[�^�T�C�Y�i�t�b�^�T�C�Y�܂ށj
	u32 magic_number;	///<�}�W�b�N�i���o�[
	u32 blk_id;			///<�Ώۂ̃u���b�N�w��ID
	u16 crc;			///<�f�[�^�S�̂�CRC�l
}SAVE_FOOTER;

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
 * @brief	�Z�[�u���[�N�\����
 *
 * ���ۂ̃Z�[�u����镔���̍\��
 */
//---------------------------------------------------------------------------
typedef struct {
	u8 data[SECTOR_SIZE * SECTOR_MAX];	///<���ۂ̃f�[�^�ێ��̈�
}SAVEWORK;


//---------------------------------------------------------------------------
/**
 * @brief	�����]������p���[�N
 */
//---------------------------------------------------------------------------
typedef struct {
	BOOL total_save_mode;
	u32 block_start;
	u32 block_current;
	u32 block_end;
	u16 lock_id;
	int div_seq;
	u32 g_backup;
	u32 b_backup[SVBLK_ID_MAX];
}NEWDIVSV_WORK;


//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�\����
 *
 * �Z�[�u�f�[�^���̂ƁA����p���[�N���܂Ƃ߂�����
 */
//---------------------------------------------------------------------------
struct _SAVEDATA {
	BOOL flash_exists;			///<�o�b�N�A�b�vFLASH�����݂��邩�ǂ���
	BOOL data_exists;			///<�f�[�^�����݂��邩�ǂ���
	BOOL new_data_flag;			///<�u�������傩��v�̃f�[�^���ǂ���
	BOOL total_save_flag;		///<�S�̃Z�[�u���K�v���ǂ����̃t���O
	LOAD_RESULT first_status;	///<��ԍŏ��̃Z�[�u�f�[�^�`�F�b�N����

	u32 heap_save_id;
	u32 heap_temp_id;
	MATHCRC16Table crc_table;	///<CRC�Z�o�p�e�[�u��

	SAVEWORK svwk;				///<�Z�[�u�f�[�^�{��

	u32 global_counter;
	u32 current_counters[SVBLK_ID_MAX];
	u32 current_side[SVBLK_ID_MAX];

	///�Z�[�u���ڃf�[�^���
	SVPAGE_INFO pageinfo[GMDATA_ID_MAX];

	///�Z�[�u�u���b�N�f�[�^���
	SVBLK_INFO blkinfo[SVBLK_ID_MAX];

	///�����]������p���[�N
	NEWDIVSV_WORK ndsw;

	int dendou_sector_switch;
	u32 dendou_counter;
};


//============================================================================================
//
//
//			�ϐ�
//
//
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�\���̂ւ̃|�C���^
 *
 * ���̃t�@�C���ŗB��ÓI�m�ۂ����ϐ��B
 * �Q�[���J�n���ɏ����������B
 */
//---------------------------------------------------------------------------
static SAVEDATA * SvPointer = NULL;


//============================================================================================
//
//
//			���J�֐�
//
//
//============================================================================================
static void SVDT_Init(SAVEWORK * svwk, const SVPAGE_INFO * pageinfo);
static void SVDT_MakeIndex(SVPAGE_INFO * pageinfo);
static void SVDT_MakeBlockIndex(SVBLK_INFO * blkinfo, const SVPAGE_INFO * pageinfo);


static SAVE_RESULT NewSVLD_Save(SAVEDATA * sv);
static BOOL NewSVLD_Load(SAVEDATA * sv);
static LOAD_RESULT NewCheckLoadData(SAVEDATA * sv);
static void NEWSVLD_DivSaveInit(SAVEDATA * sv, NEWDIVSV_WORK * ndsw, u32 block_id);
static SAVE_RESULT NEWSVLD_DivSaveMain(SAVEDATA * sv, NEWDIVSV_WORK * ndsw);
static void NEWSVLD_DivSaveEnd(SAVEDATA * sv, NEWDIVSV_WORK * ndsw, SAVE_RESULT result);
static void NEWSVLD_DivSaveCancel(SAVEDATA * sv, NEWDIVSV_WORK * ndsw);

static BOOL EraseFlashFooter(const SAVEDATA * sv, u32 block_id, u32 mirror_side);

static int GetTotalSector(const SAVEDATA * sv);

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�\���̏�����
 */
//---------------------------------------------------------------------------
SAVEDATA * SaveData_System_Init(u32 heap_save_id, u32 heap_temp_id)
{
	SAVEDATA * sv;


	sv = GFL_HEAP_AllocMemory(heap_save_id, sizeof(SAVEDATA));
	MI_CpuClearFast(sv, sizeof(SAVEDATA));
	SvPointer = sv;
	sv->heap_save_id = heap_save_id;
	sv->heap_temp_id = heap_temp_id;
	sv->flash_exists = GFL_FLASH_Init();
	sv->data_exists = FALSE;			//�f�[�^�͑��݂��Ȃ�
	sv->new_data_flag = TRUE;			//�V�K�f�[�^�ɂȂ�
	sv->total_save_flag = TRUE;			//�S�̃Z�[�u�̕K�v������
	MATH_CRC16CCITTInitTable(&sv->crc_table);
	SVDT_MakeIndex(sv->pageinfo);
	SVDT_MakeBlockIndex(sv->blkinfo, sv->pageinfo);
	MI_CpuClearFast(sv->current_counters, sizeof(sv->current_counters));


	//�f�[�^���݃`�F�b�N���s���Ă���
	sv->first_status = NewCheckLoadData(sv);
	switch (sv->first_status) {
	case LOAD_RESULT_OK:
		sv->total_save_flag = FALSE;	//�S�̃Z�[�u�̕K�v�͂Ȃ�
										//NG�̏ꍇ��TRUE�̂܂܂Ȃ̂őS�̃Z�[�u�ɂȂ�
		/* FALL THROUGH */
	case LOAD_RESULT_NG:
		//�܂Ƃ��ȃf�[�^������悤�Ȃ̂œǂݍ���
		(void)NewSVLD_Load(sv);
		sv->data_exists = TRUE;			//�f�[�^�͑��݂���
		sv->new_data_flag = FALSE;		//�V�K�f�[�^�ł͂Ȃ�
		break;
	case LOAD_RESULT_NULL:
	case LOAD_RESULT_BREAK:
		//�V�K or �f�[�^�j��Ȃ̂ŃN���A����
		SaveData_ClearData(sv);
		break;
	}

#ifdef	PM_DEBUG
	{
		int i, rest;
		for (i = 0; i < SVBLK_ID_MAX; i++) {
			OS_TPrintf("[%d] ",i);
			OS_TPrintf("(%04x x %02d) - %05x = %05x\n",
					SEC_DATA_SIZE,sv->blkinfo[i].use_sec,
					sv->blkinfo[i].size,
					SEC_DATA_SIZE * sv->blkinfo[i].use_sec - sv->blkinfo[i].size);
		}
		rest = SECTOR_MAX - GetTotalSector(sv);
		if (rest > 0) {
			OS_TPrintf("%2d sector(0x%05x) left.\n", rest, 0x1000 * rest);
		}
	}
#endif

	return sv;
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�ւ̃|�C���^�擾
 * @return	SAVEDATA	�Z�[�u�f�[�^�\���ւ̃|�C���^
 *
 * ��{�I�ɂ̓Z�[�u�f�[�^�ւ̃O���[�o���Q�Ƃ͔��������B���̂��߁A���̊֐���
 * �g�p����ӏ��͌��d�ɐ�������Ȃ���΂Ȃ�Ȃ��B�ł���΃v���O���}���[�_�[��
 * �����Ȃ���Ύg�p�ł��Ȃ��悤�ɂ������B
 * �ςȃA�N�Z�X��������C����������܂��B�g�p���@�ɂ͒��ӂ��Ă��������B
 */
//---------------------------------------------------------------------------
SAVEDATA * SaveData_GetPointer(void)
{
	GF_ASSERT(SvPointer != NULL);
	return SvPointer;
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�i�����j�̃|�C���^���擾����
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	gmdataID	�擾�������Z�[�u�f�[�^��ID
 * @return	�K�v�ȃZ�[�u�̈�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
void * SaveData_Get(SAVEDATA * sv, GMDATA_ID gmdataID)
{
	GF_ASSERT(gmdataID < GMDATA_ID_MAX);
	return &(sv->svwk.data[sv->pageinfo[gmdataID].address]);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const void * SaveData_GetReadOnlyData(const SAVEDATA * sv, GMDATA_ID gmdataID)
{
	return SaveData_Get((SAVEDATA *)sv, gmdataID);
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�̏�������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @retval	TRUE		�������ݐ���
 * @retval	FALSE		�������ݎ��s
 *
 * �Z�[�u�f�[�^���N���A������Ńt���b�V���ɏ������ށB
 */
//---------------------------------------------------------------------------
BOOL SaveData_Erase(SAVEDATA * sv)
{
	u32 i;
	u8 * buf = GFL_HEAP_AllocMemory(- sv->heap_temp_id, SECTOR_SIZE);
    GFL_UI_SleepDisable(GFL_UI_SLEEP_SVLD);

	//�e�u���b�N�̃t�b�^�����������s���č폜����
	(void)EraseFlashFooter(sv, SVBLK_ID_NORMAL, !sv->current_side[SVBLK_ID_NORMAL]);
	(void)EraseFlashFooter(sv, SVBLK_ID_BOX, !sv->current_side[SVBLK_ID_BOX]);
	(void)EraseFlashFooter(sv, SVBLK_ID_NORMAL, sv->current_side[SVBLK_ID_NORMAL]);
	(void)EraseFlashFooter(sv, SVBLK_ID_BOX, sv->current_side[SVBLK_ID_BOX]);

	MI_CpuFillFast(buf, 0xffffffff, SECTOR_SIZE);
	for (i = 0; i < SECTOR_MAX * 2; i++) {
		(void)GFL_FLASH_Save(SECTOR_SIZE * (i + FIRST_MIRROR_START), buf, SECTOR_SIZE);
		(void)GFL_FLASH_Save(SECTOR_SIZE * (i + SECOND_MIRROR_START), buf, SECTOR_SIZE);
	}
	GFL_HEAP_FreeMemory(buf);
	SaveData_ClearData(sv);
	sv->data_exists = FALSE;
    GFL_UI_SleepEnable(GFL_UI_SLEEP_SVLD);

	return TRUE;
}

//---------------------------------------------------------------------------
/**
 * @brief	���[�h����
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @retval	TRUE		�ǂݍ��ݐ���
 * @retval	FALSE		�ǂݍ��ݎ��s
 */
//---------------------------------------------------------------------------
BOOL SaveData_Load(SAVEDATA * sv)
{
	BOOL result;
	if (!sv->flash_exists) {
		return FALSE;
	}

	result = NewSVLD_Load(sv);

	if (result) {
		sv->data_exists = TRUE;			//�f�[�^�͑��݂���
		sv->new_data_flag = FALSE;		//�V�K�f�[�^�ł͂Ȃ�
		return TRUE;
	} else {
		return FALSE;
	}
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u����
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @retval	TRUE		�������ݐ���
 * @retval	FALSE		�������ݎ��s
 */
//---------------------------------------------------------------------------
SAVE_RESULT SaveData_Save(SAVEDATA * sv)
{
	SAVE_RESULT result;

	if (!sv->flash_exists) {
#ifdef	DISABLE_FLASH_CHECK		//�o�b�N�A�b�v�t���b�V���Ȃ��ł�����
		return SAVE_RESULT_OK;
#else
		return SAVE_RESULT_NG;
#endif
	}

	result = NewSVLD_Save(sv);

	if (result == SAVE_RESULT_OK) {
		sv->data_exists = TRUE;			//�f�[�^�͑��݂���
		sv->new_data_flag = FALSE;		//�V�K�f�[�^�ł͂Ȃ�
	}
	return result;
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�����i�����j
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	id			�Z�[�u�Ώۂ̃u���b�NID
 * @retval	TRUE		�������ݐ���
 * @retval	FALSE		�������ݎ��s
 *
 * id�@��SVBLK_ID_MAX���w�肷��ƑS�̃Z�[�u�ƂȂ�
 */
//---------------------------------------------------------------------------
SAVE_RESULT SaveData_SaveParts(SAVEDATA * sv, SVBLK_ID id)
{
	SAVE_RESULT result;

	GF_ASSERT(id < SVBLK_ID_MAX);
	GF_ASSERT(sv->new_data_flag == FALSE);
	GF_ASSERT(sv->data_exists == TRUE);
	SaveData_DivSave_Init(sv, id);
	do {
		result = SaveData_DivSave_Main(sv);
	}while(result == SAVE_RESULT_CONTINUE || result == SAVE_RESULT_LAST);
	return result;
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�̏�����
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 *
 * SaveData_Erase�ƈႢ�A�t���b�V���ɏ������܂Ȃ��B
 * �Z�[�u�f�[�^�������ԂŁu�������傩��v�V�ԏꍇ�Ȃǂ̏���������
 */
//---------------------------------------------------------------------------
void SaveData_ClearData(SAVEDATA * sv)
{
	sv->new_data_flag = TRUE;				//�V�K�f�[�^�ł���
	sv->total_save_flag = TRUE;				//�S�̃Z�[�u����K�v������
	SVDT_Init(&sv->svwk, sv->pageinfo);
}

//---------------------------------------------------------------------------
/**
 * @brief	�t���b�V�����݃t���O�̃`�F�b�N
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @return	BOOL		TRUE�̂Ƃ��A�t���b�V�������݂���
 */
//---------------------------------------------------------------------------
BOOL SaveData_GetFlashExistsFlag(const SAVEDATA * sv)
{
	return sv->flash_exists;
}

//---------------------------------------------------------------------------
/**
 * @brief	�ŏ��̓ǂݍ��݌��ʂ�Ԃ�
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @return	LOAD_RESULT	�ǂݍ��݌��ʁisavedata_def.h�Q�Ɓj
 */
//---------------------------------------------------------------------------
LOAD_RESULT SaveData_GetLoadResult(const SAVEDATA * sv)
{
	return sv->first_status;
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^���݃t���O���擾
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @return	BOOL		TRUE�̂Ƃ��A�Z�[�u�f�[�^�����݂���
 */
//---------------------------------------------------------------------------
BOOL SaveData_GetExistFlag(const SAVEDATA * sv)
{
	return sv->data_exists;
}

//---------------------------------------------------------------------------
/**
 * @brief	�V�K�Q�[����Ԃ��擾
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	BOOL		TRUE�̂Ƃ��A�V�K�Q�[���i�ł܂��Z�[�u���Ă��Ȃ��j
 */
//---------------------------------------------------------------------------
BOOL SaveData_GetNewDataFlag(const SAVEDATA * sv)
{
	return sv->new_data_flag;
}

//---------------------------------------------------------------------------
/**
 * @brief	�f�[�^�㏑���`�F�b�N
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @retval	TRUE		���ɂ���f�[�^�ɕʂ̃f�[�^���㏑�����悤�Ƃ��Ă���
 * @retval	FALSE		�f�[�^���Ȃ����A�����f�[�^�ł���
 */
//---------------------------------------------------------------------------
BOOL SaveData_IsOverwritingOtherData(const SAVEDATA * sv)
{
	if (SaveData_GetNewDataFlag(sv) && SaveData_GetExistFlag(sv)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

//---------------------------------------------------------------------------
/**
 * @brief	�S�̃Z�[�u���K�v�ȏ�Ԃ��ǂ����𔻒肷��
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @return	BOOL		TRUE�̂Ƃ��A�S�̃Z�[�u���K�v�i�{�b�N�X���X�V����Ă���j
 */
//---------------------------------------------------------------------------
BOOL SaveData_GetTotalSaveFlag(const SAVEDATA * sv)
{
	return sv->total_save_flag;
}

//---------------------------------------------------------------------------
/**
 * @brief	�S�̃Z�[�u��v������
 */
//---------------------------------------------------------------------------
void SaveData_RequestTotalSave(void)
{
	SvPointer->total_save_flag = TRUE;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static int GetTotalSector(const SAVEDATA * sv)
{
	int i;
	int total = 0;
	const SVBLK_INFO * blkinfo = sv->blkinfo;
	for (i = 0; i < SVBLK_ID_MAX; i++) {
		total += blkinfo[i].use_sec;
	}
	GF_ASSERT(total == blkinfo[SVBLK_ID_MAX-1].start_sec + blkinfo[SVBLK_ID_MAX-1].use_sec);
	return total;
}

//============================================================================================
//
//
//		���C���f�[�^�Z�[�u����
//
//
//============================================================================================

//---------------------------------------------------------------------------
/**
 * @brief	�����Z�[�u������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 */
//---------------------------------------------------------------------------
void SaveData_DivSave_Init(SAVEDATA * sv, u32 BlockID)
{
	NEWSVLD_DivSaveInit(sv, &sv->ndsw, BlockID);
}

//---------------------------------------------------------------------------
/**
 * @brief	�����Z�[�u���C������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @return	SAVE_RESULT
 */
//---------------------------------------------------------------------------
SAVE_RESULT SaveData_DivSave_Main(SAVEDATA * sv)
{
	SAVE_RESULT result;
	result = NEWSVLD_DivSaveMain(sv, &sv->ndsw);
	if (result != SAVE_RESULT_CONTINUE && result != SAVE_RESULT_LAST) {
		NEWSVLD_DivSaveEnd(sv, &sv->ndsw, result);
	}
	return result;
}

//---------------------------------------------------------------------------
/**
 * @brief	�����Z�[�u�L�����Z������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 */
//---------------------------------------------------------------------------
void SaveData_DivSave_Cancel(SAVEDATA * sv)
{
	NEWSVLD_DivSaveCancel(sv, &sv->ndsw);
}

//============================================================================================
//
//
//			�Z�[�u�f�[�^�������`�F�b�N
//
//
//============================================================================================
typedef struct {
	BOOL IsCorrect;
	u32 g_count;
	u32 b_count;
}CHK_INFO;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�̎�肦���Ԃɂ���
 *
 * ���肦��p�^�[���ɂ��āI
 * �㉺�͐i�s�f�[�^�ƃ{�b�N�X�f�[�^�A���E�̓~���[�̑g�B���퉻�ǂ�����4�r�b�g����̂�
 * 16�ʂ�̃p�^�[���ƂȂ�
 *
 *	�O		�P		�Q		�R
 *	����	����	����	����
 *	����	���~	�~��	�~�~
 *
 *	�S		�T		�U		�V
 *	���~	���~	���~	���~
 *	����	���~	�~��	�~�~
 *
 *	�W		�X		�P�O	�P�P
 *	�~��	�~��	�~��	�~��
 *	����	���~	�~��	�~�~
 *
 *	�P�Q	�P�R	�P�S	�P�T
 *	�~�~	�~�~	�~�~	�~�~
 *	����	���~	�~��	�~�~
 */

#ifdef	DEBUG_ONLY_FOR_tamada
//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
static void _DebugPutFooter(const SAVE_FOOTER * footer)
{
	OS_TPrintf("FOOTER:adrs:%08x",footer);
	OS_TPrintf(" size:%05x\n",footer->size);
	OS_TPrintf("FOOTER:GCNT:%08x",footer->g_count);
	OS_TPrintf(" BCNT:%08x\n",footer->b_count);
	//OS_TPrintf("FOOTER:MGNR:%08x\n",footer->magic_number);
	OS_TPrintf("FOOTER:BKID:%02d",footer->blk_id);
	OS_TPrintf(" CRC :%04x\n",footer->crc);
}
static void _DebugPutOX(BOOL flag)
{
	if (flag) {
		OS_PutString(" O");
	} else {
		OS_PutString(" X");
	}
}
#define	DEBUG_FOOTER_PUT(footer)	_DebugPutFooter(footer)
#define	DEBUG_OX_PUT(value)			_DebugPutOX(value)
#else
#define	DEBUG_FOOTER_PUT(footer)	/* DO NOTHING */
#define	DEBUG_OX_PUT(value)			/* DO NOTHING */
#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void _setDummyInfo(CHK_INFO * chkinfo)
{
	chkinfo->IsCorrect = FALSE;
	chkinfo->g_count = 0;
	chkinfo->b_count = 0;
}
//---------------------------------------------------------------------------
/**
 * @brief	CRC�̌v�Z
 * @param	sv		
 * @param	start	�Z�[�u�f�[�^�̊J�n�A�h���X
 * @param	size	�Z�[�u�f�[�^�̃T�C�Y�i�t�b�^�����܂ށj
 */
//---------------------------------------------------------------------------
static u16 _calcFooterCrc(const SAVEDATA * sv, void * start, u32 size)
{
	return MATH_CalcCRC16CCITT(&sv->crc_table, start, size - sizeof(SAVE_FOOTER));
}
//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�t���b�V���ւ̃A�h���X�I�t�Z�b�g�擾
 * @param	mirror_id	�ΏۂƂ���~���[�̎w��i0 or 1)
 * @param	blkinfo		�u���b�N���ւ̃|�C���^
 */
//---------------------------------------------------------------------------
static u32 _getFlashOffset(u32 mirror_id, const SVBLK_INFO * blkinfo)
{
	u32 base_ofs;
	if (mirror_id == MIRROR1ST) {
		base_ofs = FIRST_MIRROR_START * SECTOR_SIZE;
	} else {
		base_ofs = SECOND_MIRROR_START * SECTOR_SIZE;
	}
	base_ofs += blkinfo->start_ofs;
	return base_ofs;
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
static SAVE_FOOTER * _getFooterAddress(SAVEDATA * sv, u32 svwk_adrs, u32 blk_id)
{
	u32 start_adr;
	const SVBLK_INFO * blkinfo = &sv->blkinfo[blk_id];

	start_adr = svwk_adrs + blkinfo->start_ofs;
	GF_ASSERT(blkinfo->size);
	start_adr += blkinfo->size;
	start_adr -= sizeof(SAVE_FOOTER);
	return (SAVE_FOOTER *)start_adr;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static BOOL _checkSaveFooter(SAVEDATA * sv, u32 svwk_adrs, u32 blk_id)
{
	const SVBLK_INFO * blkinfo = &sv->blkinfo[blk_id];
	SAVE_FOOTER * footer = _getFooterAddress(sv, svwk_adrs, blk_id);
	u32 start_adr = svwk_adrs + blkinfo->start_ofs;

	DEBUG_FOOTER_PUT(footer);
	if (footer->size != blkinfo->size) {
		return FALSE;
	}
	if (footer->magic_number != MAGIC_NUMBER) {
		return FALSE;
	}
	if (footer->blk_id != blk_id) {
		return FALSE;
	}
	if (footer->crc != _calcFooterCrc(sv, (void *)start_adr, blkinfo->size)) {
		return FALSE;
	}
	return TRUE;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void _checkBlockInfo(CHK_INFO * chkinfo, SAVEDATA * sv, u32 svwk_adrs, u32 blk_id)
{
	SAVE_FOOTER * footer = _getFooterAddress(sv, svwk_adrs, blk_id);
	chkinfo->IsCorrect = _checkSaveFooter(sv, svwk_adrs, blk_id);
	chkinfo->g_count = footer->g_count;
	chkinfo->b_count = footer->b_count;
}
//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
static void _setSaveFooter(SAVEDATA * sv, u32 svwk_adrs, u32 blk_id)
{
	const SVBLK_INFO * blkinfo = &sv->blkinfo[blk_id];
	SAVE_FOOTER * footer = _getFooterAddress(sv, svwk_adrs, blk_id);
	u32 start_adr = svwk_adrs + blkinfo->start_ofs;

	footer->g_count = sv->global_counter;
	footer->b_count = sv->current_counters[blk_id];
	footer->size = blkinfo->size;
	footer->magic_number = MAGIC_NUMBER;
	footer->blk_id = blk_id;
	footer->crc = _calcFooterCrc(sv, (void *)start_adr, blkinfo->size);

	DEBUG_FOOTER_PUT(footer);
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
static int _diffCounter(u32 counter1, u32 counter2)
{
	if (counter1 == 0xffffffff && counter2 == 0) {
		return -1;
	}else if (counter1 == 0 && counter2 == 0xffffffff) {
		return 1;
	} else if (counter1 > counter2) {
		return 1;
	} else if (counter1 < counter2) {
		return -1;
	}
	return 0;
}

//---------------------------------------------------------------------------
/**
 * @brief	�f�[�^�̃J�E���^�𒲂ׂ�
 * @param	chk1	���ׂ�f�[�^���̂P
 * @param	chk2	���ׂ�f�[�^���̂Q
 * @param	res1	�ŐV�f�[�^�ւ̃I�t�Z�b�g
 * @param	res2	�Â��f�[�^�ւ̃I�t�Z�b�g
 * @return	int		����ȃf�[�^�̐�
 */
//---------------------------------------------------------------------------
static u32 _getNewerData(const CHK_INFO * chk1, const CHK_INFO * chk2, u32 *res1, u32 *res2)
{
	int global, block;
	global = _diffCounter(chk1->g_count, chk2->g_count);
	block = _diffCounter(chk1->b_count, chk2->b_count);

	if (chk1->IsCorrect && chk2->IsCorrect) {
		//�����Ƃ�����̏ꍇ
		if (global > 0) {
			//GLOBAL���Ⴄ�ꍇ���S�̃Z�[�u����
			GF_ASSERT(block > 0);
			*res1 = MIRROR1ST;
			*res2 = MIRROR2ND;
		}
		else if (global < 0) {
			//GLOBAL���Ⴄ�ꍇ���S�̃Z�[�u����
			GF_ASSERT(block < 0);
			*res1 = MIRROR2ND;
			*res2 = MIRROR1ST;
		}
		else if (block > 0) {
			//GLOBAL�������ꍇ�������Z�[�u��
			*res1 = MIRROR1ST;
			*res2 = MIRROR2ND;
		} else if (block < 0) {
			*res1 = MIRROR2ND;
			*res2 = MIRROR1ST;
		} else {
			//GLOBAL�������Ńu���b�N�J�E���^������
			//�����肦�Ȃ��͂�����,�QM�t���b�V���̂��߂ɂƂ肠�����@�\������
			//GF_ASSERT(0);
			*res1 = MIRROR1ST;
			*res2 = MIRROR2ND;
		}
		return 2;
	}
	else if (chk1->IsCorrect && !chk2->IsCorrect) {
		//�Е��̂ݐ���̏ꍇ
		*res1 = MIRROR1ST;
		*res2 = MIRRORERROR;
		return 1;
	}
	else if (!chk1->IsCorrect && chk2->IsCorrect) {
		*res1 = MIRROR2ND;
		*res2 = MIRRORERROR;
		return 1;
	}
	else {
		*res1 = MIRRORERROR;
		*res2 = MIRRORERROR;
		return 0;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void _setCurrentInfo(SAVEDATA * sv, const CHK_INFO * ndata, const CHK_INFO * bdata,
		u32 n_ofs, u32 b_ofs)
{
	sv->global_counter = ndata[n_ofs].g_count;
	sv->current_counters[SVBLK_ID_NORMAL] = ndata[n_ofs].b_count;
	sv->current_counters[SVBLK_ID_BOX] = bdata[b_ofs].b_count;
	sv->current_side[SVBLK_ID_NORMAL] = n_ofs;
	sv->current_side[SVBLK_ID_BOX] = b_ofs;
#ifdef	DEBUG_ONLY_FOR_tamada
	OS_Printf("CURRENT NORMAL NML %d  BOX %d\n",n_ofs, b_ofs);
	OS_Printf("CURRENT COUNTER NML %08x BOX %08x\n", ndata[n_ofs].b_count, bdata[b_ofs].b_count);
	OS_Printf("CURRENT GLOBAL %08x\n", sv->global_counter);
#endif
}
//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�̃`�F�b�N
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @retval	LOAD_RESULT		�`�F�b�N����
 */
//---------------------------------------------------------------------------
static LOAD_RESULT NewCheckLoadData(SAVEDATA * sv)
{
	CHK_INFO ndata[2];
	CHK_INFO bdata[2];
	u8 * buffer1;
	u8 * buffer2;
	u32 nres, bres;
	u32 n_main, b_main, n_sub, b_sub;

	buffer1 = GFL_HEAP_AllocMemory(- sv->heap_temp_id, SECTOR_SIZE * SECTOR_MAX);
	buffer2 = GFL_HEAP_AllocMemory(- sv->heap_temp_id, SECTOR_SIZE * SECTOR_MAX);

	if(GFL_FLASH_Load(FIRST_MIRROR_START * SECTOR_SIZE, buffer1, SECTOR_SIZE * SECTOR_MAX)) {
		_checkBlockInfo(&ndata[MIRROR1ST], sv, (u32)buffer1, SVBLK_ID_NORMAL);
		_checkBlockInfo(&bdata[MIRROR1ST], sv, (u32)buffer1, SVBLK_ID_BOX);
	} else {
		_setDummyInfo(&ndata[MIRROR1ST]);
		_setDummyInfo(&bdata[MIRROR1ST]);
	}
	if(GFL_FLASH_Load(SECOND_MIRROR_START * SECTOR_SIZE, buffer2, SECTOR_SIZE * SECTOR_MAX)) {
		_checkBlockInfo(&ndata[MIRROR2ND], sv, (u32)buffer2, SVBLK_ID_NORMAL);
		_checkBlockInfo(&bdata[MIRROR2ND], sv, (u32)buffer2, SVBLK_ID_BOX);
	} else {
		_setDummyInfo(&ndata[MIRROR2ND]);
		_setDummyInfo(&bdata[MIRROR2ND]);
	}

	GFL_HEAP_FreeMemory(buffer1);
	GFL_HEAP_FreeMemory(buffer2);

	nres = _getNewerData(&ndata[MIRROR1ST], &ndata[MIRROR2ND], &n_main, &n_sub);
	bres = _getNewerData(&bdata[MIRROR1ST], &bdata[MIRROR2ND], &b_main, &b_sub);
	{
		OS_PutString("SAVE NORMAL:");
		DEBUG_OX_PUT(ndata[MIRROR1ST].IsCorrect);
		DEBUG_OX_PUT(ndata[MIRROR2ND].IsCorrect);
		OS_TPrintf("...OK=%d\n",nres);
		OS_PutString("SAVE BOX   :");
		DEBUG_OX_PUT(bdata[MIRROR1ST].IsCorrect);
		DEBUG_OX_PUT(bdata[MIRROR2ND].IsCorrect);
		OS_TPrintf("...OK=%d\n",bres);
	}
	if (nres == 0 && bres == 0) {
		//�ʏ�f�[�^���{�b�N�X�f�[�^�����Ă��遨�P�Ƀf�[�^���Ȃ��Ƃ݂Ȃ�
		return LOAD_RESULT_NULL;		//�p�^�[���P�T
	}
	if (nres == 0 || bres == 0) {
		//�ʏ�f�[�^���{�b�N�X�f�[�^���̂ǂ��炩�����Ă��遨�j��
		return LOAD_RESULT_BREAK;	//�p�^�[���R�C�V�C�P�P�A�P�Q�C�P�R�C�P�S
	}

	if (nres == 2 && bres == 2){
		// ����f�[�^��2�g	
		if (ndata[n_main].g_count == bdata[b_main].g_count) {
			_setCurrentInfo(sv, ndata, bdata, n_main, b_main);
			return LOAD_RESULT_OK;	//�p�^�[���O
		} else {
			//�i�s�f�[�^�ƃ{�b�N�X�f�[�^�̃Z�[�u�̂��傤�ǊԂ�
			//�d���ؒf�����ꍇ
			_setCurrentInfo(sv, ndata, bdata, n_sub, b_main);
			return LOAD_RESULT_NG;	//�p�^�[���O
		}
	}
	if (nres == 1 && bres == 2) {
		//�����A���邢�͑S�̏������ݒ��f�ɂ��i�s�f�[�^�j��
		if (ndata[n_main].g_count == bdata[b_main].g_count) {
			_setCurrentInfo(sv, ndata, bdata, n_main, b_main);
			return LOAD_RESULT_NG;	//�p�^�[���S�A�W
		} else if (ndata[n_main].g_count == bdata[b_sub].g_count) {
			_setCurrentInfo(sv, ndata, bdata, n_main, b_sub);
			return LOAD_RESULT_NG;	//�p�^�[���S�A�W
		}
		//GF_ASSERT(ndata[n_main].g_count == bdata[b_main].g_count);
		//_setCurrentInfo(sv, ndata, bdata, n_main, b_main);
		return LOAD_RESULT_BREAK;	//�p�^�[���S�A�W
	}
	if (nres == 2 && bres == 1) {
		if (ndata[n_main].g_count == bdata[b_main].g_count) {
			//����Z�[�u���x���S�̃Z�[�u���s���Ă��Ȃ��ꍇ
			_setCurrentInfo(sv, ndata, bdata, n_main, b_main);
			return LOAD_RESULT_OK;	//�p�^�[���P�C�Q
		} else {
			//�S�̏������ݒ��f�ɂ��{�b�N�X�f�[�^�j��
			//�f�[�^�̊����߂�����������
			_setCurrentInfo(sv, ndata, bdata, n_sub, b_main);
			return LOAD_RESULT_NG;	//�p�^�[���P�C�Q
		}
	}
	if (nres == 1 && bres == 1 && n_main == b_main) {
		// ����f�[�^����g ������
		GF_ASSERT(ndata[n_main].g_count == bdata[b_main].g_count);
		_setCurrentInfo(sv, ndata, bdata, n_main, b_main);
		return LOAD_RESULT_OK;	//�p�^�[���T�A���邢�͂P�O��
	} else {
		//����Z�[�u�A2,3��ڂ�i�s�f�[�^�݂̂ɂ���3��ڂ𒆒f�Ŏ��s�����ꍇ
		GF_ASSERT(ndata[n_main].g_count == bdata[b_main].g_count);
		_setCurrentInfo(sv, ndata, bdata, n_main, b_main);
		return LOAD_RESULT_NG;	//�p�^�[���U�A�X
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static BOOL _loadBlock(u32 mirror_id, const SVBLK_INFO * blkinfo, u8 * svwk)
{
	u32 base_ofs;

	base_ofs = _getFlashOffset(mirror_id, blkinfo);
	svwk += blkinfo->start_ofs;
	return GFL_FLASH_Load(base_ofs, svwk, blkinfo->size);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL NewSVLD_Load(SAVEDATA * sv)
{
	u32 i;
	//u32 base_ofs;
	const SVBLK_INFO * blkinfo = sv->blkinfo;

	for (i = 0; i < SVBLK_ID_MAX; i++) {
		if (_loadBlock(sv->current_side[i], &sv->blkinfo[i], sv->svwk.data) == FALSE) {
			return FALSE;
		}
		if (_checkSaveFooter(sv, (u32)sv->svwk.data, i) == FALSE) {
			return FALSE;
		}
	}
	return TRUE;
}

//============================================================================================
//
//
//			�����Z�[�u
//
//
//============================================================================================
#define	LAST_DATA_SIZE	sizeof(SAVE_FOOTER)
//#define	LAST_DATA_SIZE	(8)
#define	LAST_DATA2_SIZE	(8)

//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�Z�b�g�F�f�[�^���C������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	block_id	�Z�[�u����u���b�N�w��ID
 * @param	mirror_side	�Z�[�u����~���[�̎w��i0 or 1)
 */
//---------------------------------------------------------------------------
static u16 _saveDivStartMain(SAVEDATA *sv, u32 block_id, u8 mirror_side)
{
	u32 base_ofs;
	//BOOL result;
	u8 * svwk;
	const SVBLK_INFO * blkinfo = &sv->blkinfo[block_id];

	_setSaveFooter(sv, (u32)sv->svwk.data, block_id);
	base_ofs = _getFlashOffset(mirror_side, blkinfo);
	svwk = sv->svwk.data + blkinfo->start_ofs;

	return GFL_FLASH_SAVEASYNC_Init(base_ofs, svwk, blkinfo->size - LAST_DATA_SIZE);
}

//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�Z�b�g�F�t�b�^����
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	block_id	�Z�[�u����u���b�N�w��ID
 * @param	mirror_side	�Z�[�u����~���[�̎w��i0 or 1)
 */
//---------------------------------------------------------------------------
static u16 _saveDivStartFooter(SAVEDATA *sv, u32 block_id, u8 mirror_side)
{
	u32 base_ofs;
	//BOOL result;
	u8 * svwk;
	const SVBLK_INFO * blkinfo = &sv->blkinfo[block_id];

	//_setSaveFooter(sv, (u32)sv->svwk.data, block_id);
	base_ofs = _getFlashOffset(mirror_side, blkinfo) + blkinfo->size - LAST_DATA_SIZE;
	svwk = sv->svwk.data + blkinfo->start_ofs + blkinfo->size - LAST_DATA_SIZE;

	return GFL_FLASH_SAVEASYNC_Init(base_ofs, svwk, LAST_DATA_SIZE);
}

//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�Z�b�g�F�t�b�^����
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	block_id	�Z�[�u����u���b�N�w��ID
 * @param	mirror_side	�Z�[�u����~���[�̎w��i0 or 1)
 */
//---------------------------------------------------------------------------
static u16 _saveDivStartFooter2(SAVEDATA *sv, u32 block_id, u8 mirror_side)
{
	u32 base_ofs;
	//BOOL result;
	u8 * svwk;
	const SVBLK_INFO * blkinfo = &sv->blkinfo[block_id];

	//_setSaveFooter(sv, (u32)sv->svwk.data, block_id);
	base_ofs = _getFlashOffset(mirror_side, blkinfo) + blkinfo->size - LAST_DATA_SIZE + LAST_DATA2_SIZE;
	svwk = sv->svwk.data + blkinfo->start_ofs + blkinfo->size - LAST_DATA_SIZE + LAST_DATA2_SIZE;

	return GFL_FLASH_SAVEASYNC_Init(base_ofs, svwk, LAST_DATA2_SIZE);
}



//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�����F������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	ndsw		�񓯊��Z�[�u���䃏�[�N�ւ̃|�C���^
 * @param	block_id	�Z�[�u����u���b�N�w��ID
 */
//---------------------------------------------------------------------------
static void NEWSVLD_DivSaveInit(SAVEDATA * sv, NEWDIVSV_WORK * ndsw, u32 block_id)
{
	int i;

	for (i = 0; i < SVBLK_ID_MAX; i++) {
		ndsw->b_backup[i] = sv->current_counters[i];
		sv->current_counters[i] ++;
	}

	ndsw->div_seq = 0;
	ndsw->total_save_mode = FALSE;

	if (block_id == SVBLK_ID_MAX) {
		if (sv->total_save_flag) {
			ndsw->total_save_mode = TRUE;
			ndsw->g_backup = sv->global_counter;
			sv->global_counter ++;

			ndsw->block_start = 0;
			ndsw->block_current = 0;
			ndsw->block_end = SVBLK_ID_MAX;
		} else {
			ndsw->block_start = SVBLK_ID_NORMAL;
			ndsw->block_current = SVBLK_ID_NORMAL;
			ndsw->block_end = SVBLK_ID_NORMAL + 1;
		}
	} else {
		ndsw->block_start = block_id;
		ndsw->block_current = block_id;
		ndsw->block_end = block_id + 1;
	}
    GFL_UI_SleepDisable(GFL_UI_SLEEP_SVLD);
}

//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�����F���C��
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	ndsw		�񓯊��Z�[�u���䃏�[�N�ւ̃|�C���^
 * @retval	SAVE_RESULT_CONTINUE	�Z�[�u�p����
 * @retval	SAVE_RESULT_LAST		�Z�[�u�p�����A�Ō�̕���
 * @retval	SAVE_RESULT_OK			�Z�[�u�I���A����
 * @retval	SAVE_RESULT_NG			�Z�[�u�I���A���s
 */
//---------------------------------------------------------------------------
static SAVE_RESULT NEWSVLD_DivSaveMain(SAVEDATA * sv, NEWDIVSV_WORK * ndsw)
{
	BOOL result;

	switch (ndsw->div_seq) {
	case 0:
		OS_TPrintf("DIV SAVE:BLOCK %d SIDE %d\n",
				ndsw->block_current, !sv->current_side[ndsw->block_current]);
		ndsw->lock_id = _saveDivStartMain(sv, ndsw->block_current, !sv->current_side[ndsw->block_current]);
		ndsw->div_seq ++;
		/* FALL THROUGH */
	case 1:
		//���C���f�[�^�����Z�[�u
		if (GFL_FLASH_SAVEASYNC_Main(ndsw->lock_id, &result) == FALSE) {
			break;
		}
#ifndef	DISABLE_FLASH_CHECK		//�o�b�N�A�b�v�t���b�V���Ȃ��ł�����
		if (!result) {
			OS_TPrintf("DIV SAVE ERROR!!!!\n");
			return SAVE_RESULT_NG;
		}
#endif
		ndsw->div_seq ++;
		/* FALL THROUGH */

	case 2:
		ndsw->lock_id = _saveDivStartFooter2(sv, ndsw->block_current, !sv->current_side[ndsw->block_current]);
		ndsw->div_seq ++;
		/* FALL THROUGH */

	case 3:
		//�t�b�^�����Z�[�u
		if (GFL_FLASH_SAVEASYNC_Main(ndsw->lock_id, &result) == FALSE) {
			break;
		}
#ifndef	DISABLE_FLASH_CHECK		//�o�b�N�A�b�v�t���b�V���Ȃ��ł�����
		if (!result) {
			OS_TPrintf("DIV SAVE ERROR!!!!\n");
			return SAVE_RESULT_NG;
		}
#endif
		ndsw->div_seq ++;
		if (ndsw->block_current + 1 == ndsw->block_end) {
			//�Ō�̃u���b�N�̃Z�[�u�̏ꍇ�A������O���ɒm�点�邽�߂�
			//SAVE_RESULT_CONTINUE�łȂ�SAVE_RESULT_LAST��Ԃ�
			return SAVE_RESULT_LAST;
		}
		/* FALL THROUGH */

	case 4:
		ndsw->lock_id = _saveDivStartFooter(sv, ndsw->block_current, !sv->current_side[ndsw->block_current]);
		ndsw->div_seq ++;
		/* FALL THROUGH */

	case 5:
		//�t�b�^�����Z�[�u
		if (GFL_FLASH_SAVEASYNC_Main(ndsw->lock_id, &result) == FALSE) {
			break;
		}
#ifndef	DISABLE_FLASH_CHECK		//�o�b�N�A�b�v�t���b�V���Ȃ��ł�����
		if (!result) {
			OS_TPrintf("DIV SAVE ERROR!!!!\n");
			return SAVE_RESULT_NG;
		}
#endif
		ndsw->block_current ++;
		if (ndsw->block_current == ndsw->block_end) {
			OS_TPrintf("DIV SAVE OK!\n");
			return SAVE_RESULT_OK;
		}
		ndsw->div_seq = 0;
		break;
	}
	return SAVE_RESULT_CONTINUE;
}

//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�����F�I������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	ndsw		�񓯊��Z�[�u���䃏�[�N�ւ̃|�C���^
 * @param	result		�Z�[�u����
 */
//---------------------------------------------------------------------------
static void NEWSVLD_DivSaveEnd(SAVEDATA * sv, NEWDIVSV_WORK * ndsw, SAVE_RESULT result)
{
	u32 i;
	if (result == SAVE_RESULT_NG) {
		//�Z�[�u���s�̏ꍇ
		if (ndsw->total_save_mode) {
			sv->global_counter = ndsw->g_backup;
		}
		for (i = 0; i < SVBLK_ID_MAX; i++) {
			sv->current_counters[i] = ndsw->b_backup[i];
		}
	} else {
		//����Z�[�u�����u���b�N�̎Q�ƃ~���[���t�]���Ă���
		for (i = ndsw->block_start; i < ndsw->block_end; i++) {
			sv->current_side[i] = !sv->current_side[i];
		}
		sv->data_exists = TRUE;			//�f�[�^�͑��݂���
		sv->new_data_flag = FALSE;		//�V�K�f�[�^�ł͂Ȃ�
		sv->total_save_flag = FALSE;	//�S�̃Z�[�u�͕K�v�Ȃ�
	}
    GFL_UI_SleepEnable(GFL_UI_SLEEP_SVLD);
}

//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�����F�L�����Z������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	ndsw		�񓯊��Z�[�u���䃏�[�N�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
static void NEWSVLD_DivSaveCancel(SAVEDATA * sv, NEWDIVSV_WORK * ndsw)
{
	int i;
	if (ndsw->total_save_mode) {
		sv->global_counter = ndsw->g_backup;
	}
	for (i = 0; i < SVBLK_ID_MAX; i++) {
		sv->current_counters[i] = ndsw->b_backup[i];
	}
    if(!CARD_TryWaitBackupAsync()){
        CARD_CancelBackupAsync();		//�񓯊������L�����Z���̃��N�G�X�g

        CARD_UnlockBackup(ndsw->lock_id);
        OS_ReleaseLockID(ndsw->lock_id);
    }
    GFL_UI_SleepEnable(GFL_UI_SLEEP_SVLD);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
SAVE_RESULT NewSVLD_Save(SAVEDATA * sv)
{
	SAVE_RESULT result;
	NEWDIVSV_WORK ndsw;

	NEWSVLD_DivSaveInit(sv, &ndsw, SVBLK_ID_MAX);

	do {
		result = NEWSVLD_DivSaveMain(sv, &ndsw);
	}while (result == SAVE_RESULT_CONTINUE || result == SAVE_RESULT_LAST);
	NEWSVLD_DivSaveEnd(sv, &ndsw, result);
	return result;
}

//---------------------------------------------------------------------------
/**
 * @brief	�w��u���b�N�̃t�b�^��������������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	block_id	��������u���b�N�w��ID
 * @param	mirror_side	��������~���[�̎w��i0 or 1)
 */
//---------------------------------------------------------------------------
static BOOL EraseFlashFooter(const SAVEDATA * sv, u32 block_id, u32 mirror_side)
{
	u32 base_ofs;
	SAVE_FOOTER dmy_footer;
	const SVBLK_INFO * blkinfo = &sv->blkinfo[block_id];

	MI_CpuFill8(&dmy_footer, 0xff, sizeof(SAVE_FOOTER));
	base_ofs = _getFlashOffset(mirror_side, blkinfo) + blkinfo->size - LAST_DATA_SIZE;

	return GFL_FLASH_Save(base_ofs, &dmy_footer, LAST_DATA_SIZE);
}


//============================================================================================
//
//
//
//============================================================================================
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
static void SVDT_MakeIndex(SVPAGE_INFO * pageinfo)
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
			savedata_total_size += sizeof(SAVE_FOOTER);
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
	GF_ASSERT(savedata_total_size <= SECTOR_SIZE * SECTOR_MAX);
}


//---------------------------------------------------------------------------
/**
 * @brief	�u���b�N�Ɋւ��鎫�����쐬����
 * @param	blkinfo		�u���b�N���ڕێ����[�N
 * @param	pageinfo	�Z�[�u���ڕێ����[�N
 */
//---------------------------------------------------------------------------
static void SVDT_MakeBlockIndex(SVBLK_INFO * blkinfo, const SVPAGE_INFO * pageinfo)
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
		blkinfo[i].size += sizeof(SAVE_FOOTER);
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


//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�̃N���A
 * @param	svwk	�Z�[�u���[�N�ւ̃|�C���^
 * @param	pageinfo	�Z�[�u���ڕێ����[�N
 */
//---------------------------------------------------------------------------
static void SVDT_Init(SAVEWORK * svwk, const SVPAGE_INFO * pageinfo)
{
	const SAVEDATA_TABLE * table = SaveDataTable;
	int i;
	u32 size;
	void * page;
	u32 adrs;


	MI_CpuClearFast(svwk->data, sizeof(svwk->data));

	for (i = 0; i <SaveDataTableMax; i++) {
		adrs = pageinfo[i].address;
		page = &svwk->data[adrs];
		size = pageinfo[i].size;
		MI_CpuClearFast(page, size);
		table[i].init_work(page);
	}

}

