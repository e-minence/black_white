//============================================================================================
/**
 * @file	backup_system.c
 * @brief	�o�b�N�A�b�v�V�X�e��
 * @author	tamada	GAME FREAK Inc.
 * @date	2007.06.14
 *
 */
//============================================================================================

#include <nitro.h>
#include <nnsys.h>

#include "backup_system.h"
#include "savedata_local.h"

#include "gflib.h"
#include "heapsys.h"

#include "flash_access.h"



//============================================================================================
//============================================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define MIRROR1ST	(0)
#define	MIRROR2ND	(1)
#define	MIRRORERROR	(2)

typedef enum {
	SVLD_STAT_OK,
	SVLD_STAT_NG,
	SVLD_STAT_NULL,
	SVLD_STAT_ERROR,
}SVLD_STATUS;

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
	int save_seq;

}BACKUP_SYSTEM;

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�\����
 *
 * �Z�[�u�f�[�^���̂ƁA����p���[�N���܂Ƃ߂�����
 */
//---------------------------------------------------------------------------
struct _GFL_SAVEDATA {
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


static SVLD_STATUS NewSVLD_Load(GFL_SAVEDATA * sv);
static LOAD_RESULT NewCheckLoadData(GFL_SAVEDATA * sv);

static void			SaveAsync_Init(GFL_SAVEDATA * sv);
static SAVE_RESULT	SaveAsync_Main(GFL_SAVEDATA * sv);
static void			SaveAsync_End(GFL_SAVEDATA * sv, SAVE_RESULT result);
static void			SaveAsync_Cancel(GFL_SAVEDATA * sv);

static BOOL EraseFooter(const GFL_SAVEDATA * sv);


//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
void GFL_BACKUP_Init(u32 heap_save_id, u32 heap_temp_id)
{
	svsys = GFL_HEAP_AllocMemory(heap_save_id, sizeof(BACKUP_SYSTEM));
	GFL_STD_MemClear32(svsys, sizeof(BACKUP_SYSTEM));
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
 * @brief	�t���b�V�����݃t���O�̃`�F�b�N
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @return	BOOL		TRUE�̂Ƃ��A�t���b�V�������݂���
 */
//---------------------------------------------------------------------------
BOOL GFL_BACKUP_IsEnableFlash(void)
{
	GF_ASSERT(svsys != NULL);	//�V�X�e��������������Ă��Ȃ���΂Ȃ�Ȃ�
	return svsys->flash_exists;
}


//=============================================================================
//
//
//		�Z�[�u�f�[�^�A�N�Z�X�֘A
//
//
//=============================================================================
//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
GFL_SAVEDATA * GFL_SAVEDATA_Create(const GFL_SVLD_PARAM * sv_param)
{
	GFL_SAVEDATA * sv;

	GF_ASSERT(svsys != NULL);	//�V�X�e��������������Ă��Ȃ���΂Ȃ�Ȃ�

	sv = GFL_HEAP_AllocMemory(svsys->heap_save_id, sizeof(GFL_SAVEDATA));
	GFL_STD_MemClear32(sv, sizeof(GFL_SAVEDATA));

	//�������p�p�����[�^�ݒ�
	sv->start_ofs = sv_param->savearea_top_address;
	sv->savearea_size = sv_param->savearea_size;
	sv->magic_number = sv_param->magic_number;

	//�Z�[�u�f�[�^�\���e�[�u������
	sv->svdt = SVDT_Create(svsys->heap_save_id, sv_param->table, sv_param->table_max,
			sv->savearea_size, sizeof(SAVE_FOOTER));

	//���f�[�^�T�C�Y���v�Z
	sv->svdt_size = SVDT_GetWorkSize(sv->svdt);

	//�Z�[�u�f�[�^�p���������m��
	sv->svwk = GFL_HEAP_AllocMemory(svsys->heap_save_id, sv->savearea_size);
	GFL_STD_MemClear32(sv->svwk, sv->savearea_size);
	sv->data_exists = FALSE;			//�f�[�^�͑��݂��Ȃ�
	sv->global_counter = 0;
	sv->current_side = 0;

	sv->first_status = GFL_BACKUP_Load(sv);
	switch (sv->first_status) {
	case LOAD_RESULT_NULL:
	case LOAD_RESULT_BREAK:
		//�V�K or �f�[�^�j��Ȃ̂ŃN���A����
		GFL_SAVEDATA_Clear(sv);
	}

	return sv;
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
void GFL_SAVEDATA_Delete(GFL_SAVEDATA * sv)
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
void * GFL_SAVEDATA_Get(GFL_SAVEDATA * sv, GFL_SVDT_ID gmdataID)
{
	return &(sv->svwk[SVDT_GetPageOffset(sv->svdt, gmdataID)]);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
const void * GFL_SAVEDATA_GetReadOnly(const GFL_SAVEDATA * sv, GFL_SVDT_ID gmdataID)
{
	return GFL_SAVEDATA_Get((GFL_SAVEDATA *)sv, gmdataID);
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�̏�����
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 *
 * GFL_BACKUP_Erase�ƈႢ�A�t���b�V���ɏ������܂Ȃ��B
 * �Z�[�u�f�[�^�������ԂŁu�������傩��v�V�ԏꍇ�Ȃǂ̏���������
 */
//---------------------------------------------------------------------------
void GFL_SAVEDATA_Clear(GFL_SAVEDATA * sv)
{
	SVDT_ClearWork(sv->svwk, sv->svdt);
}

//---------------------------------------------------------------------------
/**
 * @brief	�ŏ��̓ǂݍ��݌��ʂ�Ԃ�
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @return	LOAD_RESULT	�ǂݍ��݌��ʁisavedata_def.h�Q�Ɓj
 */
//---------------------------------------------------------------------------
LOAD_RESULT GFL_SAVEDATA_GetLoadResult(const GFL_SAVEDATA * sv)
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
BOOL GFL_SAVEDATA_GetExistFlag(const GFL_SAVEDATA * sv)
{
	return sv->data_exists;
}



//============================================================================================
//
//
//		�o�b�N�A�b�v�A�N�Z�X�֘A
//
//
//============================================================================================
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
BOOL GFL_BACKUP_Erase(GFL_SAVEDATA * sv)
{
	u32 adrs;
	u32 work_size = 0x1000;		//�K��
	u8 * buf = GFL_HEAP_AllocMemory(- svsys->heap_temp_id, work_size);
    GFL_UI_SleepDisable(GFL_UI_SLEEP_SVLD);

	//�u���b�N�̃t�b�^�����������s���č폜����
	EraseFooter(sv);

	GFL_STD_MemFill32(buf, 0xffffffff, work_size);
	for (adrs = 0; adrs < sv->savearea_size; adrs += work_size) {
		GFL_FLASH_Save(adrs + sv->start_ofs, buf, work_size);
	}
	GFL_HEAP_FreeMemory(buf);
	GFL_SAVEDATA_Clear(sv);
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
 *
 * ���[�h�����̓t���b�V���A�N�Z�X���Ԃ��\���������߁A�����֐��͗p�ӂ��Ă��Ȃ�
 */
//---------------------------------------------------------------------------
LOAD_RESULT GFL_BACKUP_Load(GFL_SAVEDATA * sv)
{
	SVLD_STATUS result;
	if (!svsys->flash_exists) {
		return LOAD_RESULT_ERROR;
	}

	result = NewSVLD_Load(sv);

	switch ((SVLD_STATUS)result) {
	case SVLD_STAT_OK:
		sv->data_exists = TRUE;			//�f�[�^�͑��݂���
		return LOAD_RESULT_OK;
	case SVLD_STAT_NG:
		return LOAD_RESULT_BREAK;

	case SVLD_STAT_NULL:
		return LOAD_RESULT_NULL;
	case SVLD_STAT_ERROR:
		return LOAD_RESULT_ERROR;
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
SAVE_RESULT GFL_BACKUP_Save(GFL_SAVEDATA * sv)
{
	SAVE_RESULT result;

	if (!svsys->flash_exists) {
		return SAVE_RESULT_NG;
	}

	SaveAsync_Init(sv);

	do {
		result = SaveAsync_Main(sv);
	}while (result == SAVE_RESULT_CONTINUE || result == SAVE_RESULT_LAST);

	SaveAsync_End(sv, result);

	if (result == SAVE_RESULT_OK) {
		sv->data_exists = TRUE;			//�f�[�^�͑��݂���
	}
	return result;
}

//---------------------------------------------------------------------------
/**
 * @brief	�����Z�[�u������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 */
//---------------------------------------------------------------------------
void GFL_BACKUP_SAVEASYNC_Init(GFL_SAVEDATA * sv)
{
	SaveAsync_Init(sv);
}

//---------------------------------------------------------------------------
/**
 * @brief	�����Z�[�u���C������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @return	SAVE_RESULT
 */
//---------------------------------------------------------------------------
SAVE_RESULT GFL_BACKUP_SAVEASYNC_Main(GFL_SAVEDATA * sv)
{
	SAVE_RESULT result;
	result = SaveAsync_Main(sv);
	if (result != SAVE_RESULT_CONTINUE && result != SAVE_RESULT_LAST) {
		SaveAsync_End(sv, result);
	}
	return result;
}

//---------------------------------------------------------------------------
/**
 * @brief	�����Z�[�u�L�����Z������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 */
//---------------------------------------------------------------------------
void GFL_BACKUP_SAVEASYNC_Cancel(GFL_SAVEDATA * sv)
{
	SaveAsync_Cancel(sv);
}

//============================================================================================
//
//
//			�Z�[�u�f�[�^�������`�F�b�N
//
//
//============================================================================================
typedef struct {
	SVLD_STATUS IsCorrect;
	u32 g_count;
}CHK_INFO;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void _setDummyInfo(CHK_INFO * chkinfo)
{
	chkinfo->IsCorrect = SVLD_STAT_ERROR;
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
static u16 _calcFooterCrc(const void * start, u32 size)
{
	return MATH_CalcCRC16CCITT(&svsys->crc_table, start, size - sizeof(SAVE_FOOTER));
}
//---------------------------------------------------------------------------
/**
 * @brief	�o�b�N�A�b�v�̈�ւ̃A�h���X�I�t�Z�b�g�擾
 */
//---------------------------------------------------------------------------
static u32 _getBackupOffset(const GFL_SAVEDATA * sv)
{
	return sv->start_ofs;
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
static SAVE_FOOTER * _getFooterAddress(const GFL_SAVEDATA * sv, const u8 * svwk_adrs)
{
	const u8 * start_adr;

	start_adr = svwk_adrs + sv->svdt_size - sizeof(SAVE_FOOTER);
	return (SAVE_FOOTER *)start_adr;
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
static SVLD_STATUS _checkFooter(const GFL_SAVEDATA * sv, const u8 * svwk_adrs)
{
	SAVE_FOOTER * footer = _getFooterAddress(sv, svwk_adrs);

	if (footer->size != sv->svdt_size) {
		//�T�C�Y���Ⴄ���f�[�^�̑��݃`�F�b�N���ł��Ȃ�
		return SVLD_STAT_NULL;
	}
	if (footer->magic_number != sv->magic_number) {
		//�}�W�b�N�i���o�[���Ⴄ���f�[�^�̑��݃`�F�b�N���ł��Ȃ�
		return SVLD_STAT_NULL;
	}
	if (footer->crc != _calcFooterCrc(svwk_adrs, sv->svdt_size)) {
		//�b�q�b�͈Ⴄ�A�T�C�Y�E�}�W�b�N�i���o�[�͈ꏏ���f�[�^����������
		return SVLD_STAT_NG;
	}
	return SVLD_STAT_OK;
}

//---------------------------------------------------------------------------
/**
 */
//---------------------------------------------------------------------------
static void _setFooter(const GFL_SAVEDATA * sv, u8 * svwk_adrs)
{
	SAVE_FOOTER * footer = _getFooterAddress(sv, svwk_adrs);

	footer->g_count = sv->global_counter;
	footer->size = sv->svdt_size;
	footer->magic_number = sv->magic_number;
	footer->crc = _calcFooterCrc(svwk_adrs, sv->svdt_size);

}

#if 0
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

	if (chk1->IsCorrect == SVLD_STAT_OK && chk2->IsCorrect == SVLD_STAT_OK) {
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
	else if (chk1->IsCorrect == SVLD_STAT_OK && chk2->IsCorrect != SVLD_STAT_OK) {
		//�Е��̂ݐ���̏ꍇ
		*res1 = MIRROR1ST;
		*res2 = MIRRORERROR;
		return 1;
	}
	else if (!chk1->IsCorrect != SVLD_STAT_OK && chk2->IsCorrect == SVLD_STAT_OK) {
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
static void _setCurrentInfo(GFL_SAVEDATA * sv, const CHK_INFO * chkinfo, u32 n_ofs)
{
	sv->global_counter = chkinfo[n_ofs].g_count;
	sv->current_side = n_ofs;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void _checkBlockInfo(CHK_INFO * chkinfo, GFL_SAVEDATA * sv, u8 * svwk_adrs)
{
	SAVE_FOOTER * footer = _getFooterAddress(sv, svwk_adrs);
	chkinfo->IsCorrect = _checkFooter(sv, svwk_adrs);
	chkinfo->g_count = footer->g_count;
}

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�̃`�F�b�N
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 * @retval	LOAD_RESULT		�`�F�b�N����
 */
//---------------------------------------------------------------------------
static LOAD_RESULT NewCheckLoadData(GFL_SAVEDATA * sv)
{
	CHK_INFO chkinfo[2];
	u32 nres;
	u32 newer, older;


	if(GFL_FLASH_Load(sv->start_ofs, sv->svwk, sv->savearea_size)) {
		_checkBlockInfo(&chkinfo[MIRROR1ST], sv, sv->svwk);
	} else {
		_setDummyInfo(&chkinfo[MIRROR1ST]);
	}

	if(GFL_FLASH_Load(sv->start_ofs, sv->svwk, sv->savearea_size)) {
		_checkBlockInfo(&chkinfo[MIRROR2ND], sv, sv->svwk);
	} else {
		_setDummyInfo(&chkinfo[MIRROR2ND]);
	}

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
#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
SVLD_STATUS NewSVLD_Load(GFL_SAVEDATA * sv)
{
	u32 backup_pos;

	backup_pos = _getBackupOffset(sv);
	if (GFL_FLASH_Load(backup_pos, sv->svwk, sv->svdt_size) == FALSE) {
		return SVLD_STAT_ERROR;
	}

	return _checkFooter(sv, sv->svwk);
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
static u16 _saveAsyncStart_Body(GFL_SAVEDATA *sv)
{
	u32 backup_pos;
	u8 * svwk;

	_setFooter(sv, sv->svwk);
	backup_pos = _getBackupOffset(sv);
	svwk = sv->svwk;

	return GFL_FLASH_SAVEASYNC_Init(backup_pos, svwk, sv->svdt_size - LAST_DATA_SIZE);
}

//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�Z�b�g�F�t�b�^�����r��
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 */
//---------------------------------------------------------------------------
static u16 _saveAsyncStart_Footer(GFL_SAVEDATA *sv)
{
	u32 backup_pos;
	u8 * svwk;

	backup_pos = _getBackupOffset(sv) + sv->svdt_size - LAST_DATA_SIZE;
	svwk = sv->svwk + sv->svdt_size - LAST_DATA_SIZE;

	return GFL_FLASH_SAVEASYNC_Init(backup_pos, svwk, LAST_DATA_SIZE);
}

//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�Z�b�g�F�t�b�^�����Ō�
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 */
//---------------------------------------------------------------------------
static u16 _saveAsyncStart_Footer2(GFL_SAVEDATA *sv)
{
	u32 backup_pos;
	u8 * svwk;

	backup_pos = _getBackupOffset(sv) + sv->svdt_size - LAST_DATA_SIZE + LAST_DATA2_SIZE;
	svwk = sv->svwk + sv->svdt_size - LAST_DATA_SIZE + LAST_DATA2_SIZE;

	return GFL_FLASH_SAVEASYNC_Init(backup_pos, svwk, LAST_DATA2_SIZE);
}



//---------------------------------------------------------------------------
/**
 * @brief	�񓯊��Z�[�u�����F������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 */
//---------------------------------------------------------------------------
static void SaveAsync_Init(GFL_SAVEDATA * sv)
{
	svsys->save_seq = 0;

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
static SAVE_RESULT SaveAsync_Main(GFL_SAVEDATA * sv)
{
	BOOL result;

	switch (svsys->save_seq) {
	case 0:
		svsys->lock_id = _saveAsyncStart_Body(sv);
		svsys->save_seq ++;
		/* FALL THROUGH */
	case 1:
		//���C���f�[�^�����Z�[�u
		if (GFL_FLASH_SAVEASYNC_Main(svsys->lock_id, &result) == FALSE) {
			break;
		}
		if (!result) {
			return SAVE_RESULT_NG;
		}
		svsys->save_seq ++;
		/* FALL THROUGH */

	case 2:
		svsys->lock_id = _saveAsyncStart_Footer2(sv);
		svsys->save_seq ++;
		/* FALL THROUGH */

	case 3:
		//�t�b�^�����Z�[�u
		if (GFL_FLASH_SAVEASYNC_Main(svsys->lock_id, &result) == FALSE) {
			break;
		}
		if (!result) {
			return SAVE_RESULT_NG;
		}
		svsys->save_seq ++;
		//�Ō�̃u���b�N�̃Z�[�u�̏ꍇ�A������O���ɒm�点�邽�߂�
		//SAVE_RESULT_CONTINUE�łȂ�SAVE_RESULT_LAST��Ԃ�
		return SAVE_RESULT_LAST;

	case 4:
		svsys->lock_id = _saveAsyncStart_Footer(sv);
		svsys->save_seq ++;
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
static void SaveAsync_End(GFL_SAVEDATA * sv, SAVE_RESULT result)
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
static void SaveAsync_Cancel(GFL_SAVEDATA * sv)
{
	sv->global_counter = svsys->counter_backup;
	//�񓯊������L�����Z���̃��N�G�X�g
	GFL_FLASH_SAVEASYNC_Cancel(svsys->lock_id);

    GFL_UI_SleepEnable(GFL_UI_SLEEP_SVLD);
}

//---------------------------------------------------------------------------
/**
 * @brief	�w��u���b�N�̃t�b�^��������������
 * @param	sv			�Z�[�u�f�[�^�\���ւ̃|�C���^
 */
//---------------------------------------------------------------------------
static BOOL EraseFooter(const GFL_SAVEDATA * sv)
{
	u32 backup_pos;
	SAVE_FOOTER dmy_footer;

	GFL_STD_MemFill(&dmy_footer, 0xff, sizeof(SAVE_FOOTER));
	backup_pos = _getBackupOffset(sv) + sv->svdt_size - LAST_DATA_SIZE;

	return GFL_FLASH_Save(backup_pos, &dmy_footer, LAST_DATA_SIZE);
}


