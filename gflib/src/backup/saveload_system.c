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

#include <nitro.h>
#include <nnsys.h>

#include "savedata.h"
#include "savedata_local.h"

#include "gflib.h"
#include "heapsys.h"

#include "flash_access.h"



//=============================================================================
//=============================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define MIRROR1ST	(0)
#define	MIRROR2ND	(1)
#define	MIRRORERROR	(2)

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef struct {
	u32 g_count;		///<�O���[�o���J�E���^
	u32 size;			///<�f�[�^�T�C�Y�i�t�b�^�T�C�Y�܂ށj
	u32 magic_number;	///<�}�W�b�N�i���o�[
	u16 crc;			///<�f�[�^�S�̂�CRC�l
}SAVE_FOOTER;

//---------------------------------------------------------------------------
/**
 * @brief	�o�b�N�A�b�v����\����
 */
//---------------------------------------------------------------------------
typedef struct {
	BOOL flash_exists;			///<�o�b�N�A�b�vFLASH�����݂��邩�ǂ���
	u32 heap_save_id;			///<�Z�[�u�f�[�^�p�Ɏg�p����q�[�v�̎w��
	u32 heap_temp_id;			///<�e���|�����Ɏg�p����q�[�v�̎w��
	MATHCRC16Table crc_table;	///<CRC�Z�o�p�e�[�u��

	///�����]������p���[�N
	u32 counter_backup;
	u16 lock_id;
	int div_seq;

}BACKUP_SYSTEM;

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�\����
 *
 * �Z�[�u�f�[�^���̂ƁA����p���[�N���܂Ƃ߂�����
 */
//---------------------------------------------------------------------------
struct _SAVEDATA {
	u32 start_ofs;
	u32 savearea_size;
	u32 magic_number;

	BOOL data_exists;			///<�f�[�^�����݂��邩�ǂ���
	LOAD_RESULT first_status;	///<��ԍŏ��̃Z�[�u�f�[�^�`�F�b�N����
	u32 global_counter;
	u32 current_side;

	///�Z�[�u�f�[�^�\����`�ւ̃|�C���^
	SVDT * svdt;
	u32 svdt_size;

	u8 * svwk;	///<���ۂ̃f�[�^�ێ��̈�
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
 * @brief	�o�b�N�A�b�v�V�X�e���̐���\����
 */
//---------------------------------------------------------------------------
static BACKUP_SYSTEM * svsys;


//============================================================================================
//
//
//			���J�֐�
//
//
//============================================================================================


static SAVE_RESULT NewSVLD_Save(SAVEDATA * sv);
static BOOL NewSVLD_Load(SAVEDATA * sv);
static LOAD_RESULT NewCheckLoadData(SAVEDATA * sv);

static void NEWSVLD_DivSaveInit(SAVEDATA * sv);
static SAVE_RESULT NEWSVLD_DivSaveMain(SAVEDATA * sv);
static void NEWSVLD_DivSaveEnd(SAVEDATA * sv, SAVE_RESULT result);
static void NEWSVLD_DivSaveCancel(SAVEDATA * sv);

static BOOL EraseFlashFooter(const SAVEDATA * sv);


//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
void GFL_BACKUP_Init(u32 heap_save_id, u32 heap_temp_id)
{
	svsys = GFL_HEAP_AllocMemory(heap_save_id, sizeof(BACKUP_SYSTEM));
	GFL_STD_MemClear32(svsys, sizeof(SAVEDATA));
	svsys->heap_save_id = heap_save_id;
	svsys->heap_temp_id = heap_temp_id;
	MATH_CRC16CCITTInitTable(&svsys->crc_table);
	//�t���b�V���A�N�Z�X�֘A������
	svsys->flash_exists = GFL_FLASH_Init();
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
void GFL_BACKUP_Exit(void)
{
	GFL_HEAP_FreeMemory(svsys);
	svsys = NULL;
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
SAVEDATA * GFL_SVLD_Create(const GFL_SVLD_PARAM * sv_param)
{
	SAVEDATA * sv;

	sv = GFL_HEAP_AllocMemory(svsys->heap_save_id, sizeof(SAVEDATA));
	GFL_STD_MemClear32(sv, sizeof(SAVEDATA));

	//�������p�p�����[�^�ݒ�
	sv->start_ofs = sv_param->savearea_top_address;
	sv->savearea_size = sv_param->savearea_size;
	sv->magic_number = sv_param->magic_number;

	//�Z�[�u�f�[�^�\���e�[�u������
	sv->svdt = SVDT_Create(svsys->heap_save_id, sv_param->table, sv_param->table_max,
			sv->savearea_size, sizeof(SAVE_FOOTER));

	//�Z�[�u�f�[�^�p���������m��
	sv->svwk = GFL_HEAP_AllocMemory(svsys->heap_save_id, sv->savearea_size);
	GFL_STD_MemClear32(sv->svwk, sv->savearea_size);
	sv->data_exists = FALSE;			//�f�[�^�͑��݂��Ȃ�

	//���f�[�^�T�C�Y���v�Z
	sv->svdt_size = SVDT_GetWorkSize(sv->svdt);


	//�f�[�^���݃`�F�b�N���s���Ă���
	sv->first_status = NewCheckLoadData(sv);
	switch (sv->first_status) {
	case LOAD_RESULT_OK:
	case LOAD_RESULT_NG:
		//�܂Ƃ��ȃf�[�^������悤�Ȃ̂œǂݍ���
		(void)NewSVLD_Load(sv);
		sv->data_exists = TRUE;			//�f�[�^�͑��݂���
		break;
	case LOAD_RESULT_NULL:
	case LOAD_RESULT_BREAK:
		//�V�K or �f�[�^�j��Ȃ̂ŃN���A����
		SaveData_ClearData(sv);
		break;
	}

	return sv;
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
void GFL_SVLD_Delete(SAVEDATA * sv)
{
	SVDT_Delete(sv->svdt);
	GFL_HEAP_FreeMemory(sv->svwk);
	GFL_HEAP_FreeMemory(sv);
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�i�����j�̃|�C���^���擾����
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	gmdataID	�擾�������Z�[�u�f�[�^��ID
 * @return	�K�v�ȃZ�[�u�̈�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
void * SaveData_Get(SAVEDATA * sv, DATA_ID gmdataID)
{
	return &(sv->svwk[SVDT_GetPageOffset(sv->svdt, gmdataID)]);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const void * SaveData_GetReadOnlyData(const SAVEDATA * sv, DATA_ID gmdataID)
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
	u32 adrs;
	u32 work_size = 0x1000;		//�K��
	u8 * buf = GFL_HEAP_AllocMemory(- svsys->heap_temp_id, work_size);
    GFL_UI_SleepDisable(GFL_UI_SLEEP_SVLD);

	//�e�u���b�N�̃t�b�^�����������s���č폜����
	EraseFlashFooter(sv);

	GFL_STD_MemFill32(buf, 0xffffffff, work_size);
	for (adrs = 0; adrs < sv->savearea_size; adrs += work_size) {
		GFL_FLASH_Save(adrs + sv->start_ofs, buf, work_size);
		//GFL_FLASH_Save(adrs + sv->start_ofs + sv->savearea_size, buf, work_size);
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
	if (!svsys->flash_exists) {
		return FALSE;
	}

	result = NewSVLD_Load(sv);

	if (result) {
		sv->data_exists = TRUE;			//�f�[�^�͑��݂���
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

	if (!svsys->flash_exists) {
		return SAVE_RESULT_NG;
	}

	result = NewSVLD_Save(sv);

	if (result == SAVE_RESULT_OK) {
		sv->data_exists = TRUE;			//�f�[�^�͑��݂���
	}
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
	SVDT_ClearWork(sv->svwk, sv->svdt);
}

//---------------------------------------------------------------------------
/**
 * @brief	�t���b�V�����݃t���O�̃`�F�b�N
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @return	BOOL		TRUE�̂Ƃ��A�t���b�V�������݂���
 */
//---------------------------------------------------------------------------
BOOL GFL_BACKUP_IsEnableFlash(const SAVEDATA * sv)
{
	return svsys->flash_exists;
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
void SaveData_DivSave_Init(SAVEDATA * sv)
{
	NEWSVLD_DivSaveInit(sv);
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
	result = NEWSVLD_DivSaveMain(sv);
	if (result != SAVE_RESULT_CONTINUE && result != SAVE_RESULT_LAST) {
		NEWSVLD_DivSaveEnd(sv, result);
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
	NEWSVLD_DivSaveCancel(sv);
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
}CHK_INFO;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void _setDummyInfo(CHK_INFO * chkinfo)
{
	chkinfo->IsCorrect = FALSE;
	chkinfo->g_count = 0;
}
//---------------------------------------------------------------------------
/**
 * @brief	CRC�̌v�Z
 * @param	sv		
 * @param	start	�Z�[�u�f�[�^�̊J�n�A�h���X
 * @param	size	�Z�[�u�f�[�^�̃T�C�Y�i�t�b�^�����܂ށj
 */
//---------------------------------------------------------------------------
static u16 _calcFooterCrc(void * start, u32 size)
{
	return MATH_CalcCRC16CCITT(&svsys->crc_table, start, size - sizeof(SAVE_FOOTER));
}
//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�t���b�V���ւ̃A�h���X�I�t�Z�b�g�擾
 */
//---------------------------------------------------------------------------
static u32 _getFlashOffset(const SAVEDATA * sv)
{
	return sv->start_ofs;
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
static SAVE_FOOTER * _getFooterAddress(SAVEDATA * sv, u8 * svwk_adrs)
{
	u8 * start_adr;

	start_adr = svwk_adrs + sv->svdt_size - sizeof(SAVE_FOOTER);
	return (SAVE_FOOTER *)start_adr;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static BOOL _checkSaveFooter(SAVEDATA * sv, u8 * svwk_adrs)
{
	SAVE_FOOTER * footer = _getFooterAddress(sv, svwk_adrs);

	if (footer->size != sv->svdt_size) {
		return FALSE;
	}
	if (footer->magic_number != sv->magic_number) {
		return FALSE;
	}
	if (footer->crc != _calcFooterCrc(svwk_adrs, sv->svdt_size)) {
		return FALSE;
	}
	return TRUE;
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
static void _setSaveFooter(SAVEDATA * sv, u8 * svwk_adrs)
{
	SAVE_FOOTER * footer = _getFooterAddress(sv, svwk_adrs);

	footer->g_count = sv->global_counter;
	footer->size = sv->svdt_size;
	footer->magic_number = sv->magic_number;
	footer->crc = _calcFooterCrc(svwk_adrs, sv->svdt_size);

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
	int global;
	global = _diffCounter(chk1->g_count, chk2->g_count);

	if (chk1->IsCorrect && chk2->IsCorrect) {
		//�����Ƃ�����̏ꍇ
		if (global > 0) {
			*res1 = MIRROR1ST;
			*res2 = MIRROR2ND;
		}
		else if (global < 0) {
			*res1 = MIRROR2ND;
			*res2 = MIRROR1ST;
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
		//�Е��̂ݐ���̏ꍇ
		*res1 = MIRROR2ND;
		*res2 = MIRRORERROR;
		return 1;
	}
	else {
		//�����Ƃ��ُ�̏ꍇ
		*res1 = MIRRORERROR;
		*res2 = MIRRORERROR;
		return 0;
	}
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void _setCurrentInfo(SAVEDATA * sv, const CHK_INFO * chkinfo, u32 n_ofs)
{
	sv->global_counter = chkinfo[n_ofs].g_count;
	sv->current_side = n_ofs;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void _checkBlockInfo(CHK_INFO * chkinfo, SAVEDATA * sv, u8 * svwk_adrs)
{
	SAVE_FOOTER * footer = _getFooterAddress(sv, svwk_adrs);
	chkinfo->IsCorrect = _checkSaveFooter(sv, svwk_adrs);
	chkinfo->g_count = footer->g_count;
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
	CHK_INFO chkinfo[2];
	u8 * buffer1;
	u8 * buffer2;
	u32 nres;
	u32 newer, older;

	buffer1 = GFL_HEAP_AllocMemory(- svsys->heap_temp_id, sv->savearea_size);
	buffer2 = GFL_HEAP_AllocMemory(- svsys->heap_temp_id, sv->savearea_size);

	if(GFL_FLASH_Load(sv->start_ofs, buffer1, sv->savearea_size)) {
		_checkBlockInfo(&chkinfo[MIRROR1ST], sv, buffer1);
	} else {
		_setDummyInfo(&chkinfo[MIRROR1ST]);
	}

	if(GFL_FLASH_Load(sv->start_ofs, buffer2, sv->savearea_size)) {
		_checkBlockInfo(&chkinfo[MIRROR2ND], sv, buffer2);
	} else {
		_setDummyInfo(&chkinfo[MIRROR2ND]);
	}

	GFL_HEAP_FreeMemory(buffer1);
	GFL_HEAP_FreeMemory(buffer2);

	nres = _getNewerData(&chkinfo[MIRROR1ST], &chkinfo[MIRROR2ND], &newer, &older);

	if (nres == 2) {
		// ��������f�[�^
		_setCurrentInfo(sv, chkinfo, newer);
		return LOAD_RESULT_OK;
	}
	if (nres == 1) {
		//�ǂ��炩���Ă���
		_setCurrentInfo(sv, chkinfo, newer);
		return LOAD_RESULT_NG;
	}
	//if (nres == 0) 
	//�f�[�^���Ă��遨�P�Ƀf�[�^���Ȃ��Ƃ݂Ȃ�
	return LOAD_RESULT_NULL;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL NewSVLD_Load(SAVEDATA * sv)
{
	u32 flash_pos;

	flash_pos = _getFlashOffset(sv);
	if (GFL_FLASH_Load(flash_pos, sv->svwk, sv->svdt_size) == FALSE) {
		return FALSE;
	}
	if (_checkSaveFooter(sv, sv->svwk) == FALSE) {
		return FALSE;
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
#define	LAST_DATA2_SIZE	(8)

//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�Z�b�g�F�f�[�^���C������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 */
//---------------------------------------------------------------------------
static u16 _saveDivStart_Body(SAVEDATA *sv)
{
	u32 flash_pos;
	u8 * svwk;

	_setSaveFooter(sv, sv->svwk);
	flash_pos = _getFlashOffset(sv);
	svwk = sv->svwk;

	return GFL_FLASH_SAVEASYNC_Init(flash_pos, svwk, sv->svdt_size - LAST_DATA_SIZE);
}

//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�Z�b�g�F�t�b�^�����r��
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 */
//---------------------------------------------------------------------------
static u16 _saveDivStart_Footer(SAVEDATA *sv)
{
	u32 flash_pos;
	u8 * svwk;

	flash_pos = _getFlashOffset(sv) + sv->svdt_size - LAST_DATA_SIZE;
	svwk = sv->svwk + sv->svdt_size - LAST_DATA_SIZE;

	return GFL_FLASH_SAVEASYNC_Init(flash_pos, svwk, LAST_DATA_SIZE);
}

//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�Z�b�g�F�t�b�^�����Ō�
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 */
//---------------------------------------------------------------------------
static u16 _saveDivStart_Footer2(SAVEDATA *sv)
{
	u32 flash_pos;
	u8 * svwk;

	flash_pos = _getFlashOffset(sv) + sv->svdt_size - LAST_DATA_SIZE + LAST_DATA2_SIZE;
	svwk = sv->svwk + sv->svdt_size - LAST_DATA_SIZE + LAST_DATA2_SIZE;

	return GFL_FLASH_SAVEASYNC_Init(flash_pos, svwk, LAST_DATA2_SIZE);
}



//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�����F������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 */
//---------------------------------------------------------------------------
static void NEWSVLD_DivSaveInit(SAVEDATA * sv)
{
	svsys->div_seq = 0;

	svsys->counter_backup = sv->global_counter;
	sv->global_counter ++;

    GFL_UI_SleepDisable(GFL_UI_SLEEP_SVLD);
}

//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�����F���C��
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @retval	SAVE_RESULT_CONTINUE	�Z�[�u�p����
 * @retval	SAVE_RESULT_LAST		�Z�[�u�p�����A�Ō�̕���
 * @retval	SAVE_RESULT_OK			�Z�[�u�I���A����
 * @retval	SAVE_RESULT_NG			�Z�[�u�I���A���s
 */
//---------------------------------------------------------------------------
static SAVE_RESULT NEWSVLD_DivSaveMain(SAVEDATA * sv)
{
	BOOL result;

	switch (svsys->div_seq) {
	case 0:
		svsys->lock_id = _saveDivStart_Body(sv);
		svsys->div_seq ++;
		/* FALL THROUGH */
	case 1:
		//���C���f�[�^�����Z�[�u
		if (GFL_FLASH_SAVEASYNC_Main(svsys->lock_id, &result) == FALSE) {
			break;
		}
		if (!result) {
			return SAVE_RESULT_NG;
		}
		svsys->div_seq ++;
		/* FALL THROUGH */

	case 2:
		svsys->lock_id = _saveDivStart_Footer2(sv);
		svsys->div_seq ++;
		/* FALL THROUGH */

	case 3:
		//�t�b�^�����Z�[�u
		if (GFL_FLASH_SAVEASYNC_Main(svsys->lock_id, &result) == FALSE) {
			break;
		}
		if (!result) {
			return SAVE_RESULT_NG;
		}
		svsys->div_seq ++;
		//�Ō�̃u���b�N�̃Z�[�u�̏ꍇ�A������O���ɒm�点�邽�߂�
		//SAVE_RESULT_CONTINUE�łȂ�SAVE_RESULT_LAST��Ԃ�
		return SAVE_RESULT_LAST;

	case 4:
		svsys->lock_id = _saveDivStart_Footer(sv);
		svsys->div_seq ++;
		/* FALL THROUGH */

	case 5:
		//�t�b�^�����Z�[�u
		if (GFL_FLASH_SAVEASYNC_Main(svsys->lock_id, &result) == FALSE) {
			break;
		}
		if (!result) {
			return SAVE_RESULT_NG;
		}
		return SAVE_RESULT_OK;
		break;
	}
	return SAVE_RESULT_CONTINUE;
}

//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�����F�I������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @param	result		�Z�[�u����
 */
//---------------------------------------------------------------------------
static void NEWSVLD_DivSaveEnd(SAVEDATA * sv, SAVE_RESULT result)
{
	if (result == SAVE_RESULT_NG) {
		//�Z�[�u���s�̏ꍇ
		sv->global_counter = svsys->counter_backup;
	} else {
		//����Z�[�u�����u���b�N�̎Q�ƃ~���[���t�]���Ă���
		sv->current_side = !sv->current_side;
		sv->data_exists = TRUE;			//�f�[�^�͑��݂���
	}
    GFL_UI_SleepEnable(GFL_UI_SLEEP_SVLD);
}

//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�����F�L�����Z������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 */
//---------------------------------------------------------------------------
static void NEWSVLD_DivSaveCancel(SAVEDATA * sv)
{
	sv->global_counter = svsys->counter_backup;
	//�񓯊������L�����Z���̃��N�G�X�g
	GFL_FLASH_SAVEASYNC_Cancel(svsys->lock_id);

    GFL_UI_SleepEnable(GFL_UI_SLEEP_SVLD);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
SAVE_RESULT NewSVLD_Save(SAVEDATA * sv)
{
	SAVE_RESULT result;

	NEWSVLD_DivSaveInit(sv);

	do {
		result = NEWSVLD_DivSaveMain(sv);
	}while (result == SAVE_RESULT_CONTINUE || result == SAVE_RESULT_LAST);
	NEWSVLD_DivSaveEnd(sv, result);
	return result;
}

//---------------------------------------------------------------------------
/**
 * @brief	�w��u���b�N�̃t�b�^��������������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 */
//---------------------------------------------------------------------------
static BOOL EraseFlashFooter(const SAVEDATA * sv)
{
	u32 flash_pos;
	SAVE_FOOTER dmy_footer;

	GFL_STD_MemFill(&dmy_footer, 0xff, sizeof(SAVE_FOOTER));
	flash_pos = _getFlashOffset(sv) + sv->svdt_size - LAST_DATA_SIZE;

	return GFL_FLASH_Save(flash_pos, &dmy_footer, LAST_DATA_SIZE);
}


