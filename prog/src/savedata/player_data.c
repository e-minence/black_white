//============================================================================================
/**
 * @file	player_data.c
 * @date	2006.06.01
 * @author	tamada
 * @brief	�v���C���[�֘A�Z�[�u�f�[�^
 */
//============================================================================================
#include <gflib.h>
#include "savedata/config.h"
#include "savedata/playerdata_local.h"
#include "savedata/mystatus.h"
#include "mystatus_local.h"
#include "playtime_local.h"
#include "savedata/playtime.h"
#include "savedata/save_tbl.h"
#include "gamesystem/playerwork.h"
#include "savedata/player_data.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�O���Z�[�u�̑��ݗL���������}�W�b�N�L�[
#define EXTRA_SAVE_MAGIC_KEY    (0xc21e)

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�v���C���[�Z�[�u�f�[�^�̒�`
 */
//------------------------------------------------------------------
struct PLAYER_DATA {
	CONFIG config;
	MYSTATUS mystatus;
	PLAYTIME playtime;
	u8 now_save_mode_setup;    ///<TRUE:����Z�b�g�A�b�v�������Ă��Ȃ����(FALSE:���K�̃Z�[�u�ڍs�͂�����FALSE���)
	u8 padding[3];
	
	u16 extra_magic_key[SAVE_EXTRA_ID_MAX]; ///<�O���Z�[�u�̃}�W�b�N�L�[
	u32 extra_link[SAVE_EXTRA_ID_MAX];      ///<�O���Z�[�u�ւ̃����N���
};

//============================================================================================
//
//		��ɃZ�[�u�V�X�e������Ă΂��֐�
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�T�C�Y�̎擾
 * @return	int		�v���C���[�Z�[�u�f�[�^�̃T�C�Y
 */
//------------------------------------------------------------------
int PLAYERDATA_GetWorkSize(void)
{
	return sizeof(PLAYER_DATA);
}

//------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^������
 * @param	pd		�v���C���[�Z�[�u�f�[�^�ւ̃|�C���^
 */
//------------------------------------------------------------------
void PLAYERDATA_Init(PLAYER_DATA * pd)
{
	GFL_STD_MemClear32(pd, sizeof(PLAYER_DATA));
	/* �ȉ��Ɍʕ����̏��������������� */
	CONFIG_Init(&pd->config);
	MyStatus_Init(&pd->mystatus);
	PLAYTIME_Init(&pd->playtime);
	pd->now_save_mode_setup = FALSE;
}

//============================================================================================
//
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	������ԃf�[�^�ւ̃|�C���^�擾
 * @param	sv			�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @return	MYSTATUS	������ԕێ����[�N�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
MYSTATUS * SaveData_GetMyStatus(SAVE_CONTROL_WORK * sv)
{
	PLAYER_DATA * pd;
	pd = SaveControl_DataPtrGet(sv, GMDATA_ID_PLAYER_DATA);
	return &pd->mystatus;
}

//---------------------------------------------------------------------------
/**
 * @brief	�ݒ���f�[�^�ւ̃|�C���^�擾
 * @param	sv			�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @return	CONFIG		�ݒ���ێ����[�N�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
CONFIG * SaveData_GetConfig(SAVE_CONTROL_WORK * sv)
{
	PLAYER_DATA * pd;
	pd = SaveControl_DataPtrGet(sv, GMDATA_ID_PLAYER_DATA);
	return &pd->config;
}

//---------------------------------------------------------------------------
/**
 * @brief	�v���C���ԃf�[�^�ւ̃|�C���^���擾
 * @param	sv			�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @return	playtime	�v���C���ԃf�[�^�ւ̃|�C���^
 */
//---------------------------------------------------------------------------
PLAYTIME * SaveData_GetPlayTime(SAVE_CONTROL_WORK * sv)
{
	PLAYER_DATA * pd;
	pd = SaveControl_DataPtrGet(sv, GMDATA_ID_PLAYER_DATA);
	return &pd->playtime;
}

//==================================================================
/**
 * ���݂̃Z�[�u��Ԃ�����Z�b�g�A�b�v��������Ă��Ȃ��󋵂Ȃ̂����ׂ�
 *
 * @param   sv		
 *
 * @retval  BOOL		TRUE:����Z�b�g�A�b�v��������Ă��Ȃ�
 *                      ������ڍs�A��x�ł��Z�[�u����ΈȌ�A������FALSE�ɂȂ�
 */
//==================================================================
BOOL SaveData_GetNowSaveModeSetup(SAVE_CONTROL_WORK *sv)
{
	PLAYER_DATA * pd = SaveControl_DataPtrGet(sv, GMDATA_ID_PLAYER_DATA);
	return pd->now_save_mode_setup;
}

//==================================================================
/**
 * ���݂̃Z�[�u��Ԃ��u����Z�b�g�A�b�v��������Ă��Ȃ��v�󋵂ɂ���
 *
 * @param   sv		
 */
//==================================================================
void SaveData_SetNowSaveModeSetupON(SAVE_CONTROL_WORK *sv)
{
	PLAYER_DATA * pd = SaveControl_DataPtrGet(sv, GMDATA_ID_PLAYER_DATA);
	pd->now_save_mode_setup = TRUE;
}

//==================================================================
/**
 * ����Z�b�g�A�b�v�t���O��OFF�ɂ��A���݂̃Z�[�u��Ԃ𕁒ʂɂ���
 *
 * @param   sv		
 */
//==================================================================
void SaveData_SetNowSaveModeSetupOFF(SAVE_CONTROL_WORK *sv)
{
	PLAYER_DATA * pd = SaveControl_DataPtrGet(sv, GMDATA_ID_PLAYER_DATA);
	pd->now_save_mode_setup = FALSE;
}

//==================================================================
/**
 * �w�肵���O���Z�[�u�̑��݃`�F�b�N
 *
 * @param   sv		
 * @param   extra_id		�O���Z�[�uID
 *
 * @retval  BOOL		�O���Z�[�u�����݂���
 * @retval  BOOL		�O���Z�[�u�����݂��Ȃ�
 */
//==================================================================
BOOL SaveData_CheckExtraMagicKey(SAVE_CONTROL_WORK *sv, SAVE_EXTRA_ID extra_id)
{
	PLAYER_DATA * pd = SaveControl_DataPtrGet(sv, GMDATA_ID_PLAYER_DATA);
	if(pd->extra_magic_key[extra_id] != EXTRA_SAVE_MAGIC_KEY){
    return FALSE;
  }
  return TRUE;
}

//==================================================================
/**
 * �w�肵���O���Z�[�u�̑��݃`�F�b�N
 *
 * @param   sv		
 * @param   extra_id		�O���Z�[�uID
 *
 * @retval  BOOL		�O���Z�[�u�����݂���
 * @retval  BOOL		�O���Z�[�u�����݂��Ȃ�
 */
//==================================================================
void SaveData_SetExtraMagicKey(SAVE_CONTROL_WORK *sv, SAVE_EXTRA_ID extra_id)
{
	PLAYER_DATA * pd = SaveControl_DataPtrGet(sv, GMDATA_ID_PLAYER_DATA);
	pd->extra_magic_key[extra_id] = EXTRA_SAVE_MAGIC_KEY;
}

//==================================================================
/**
 * �w�肵���O���Z�[�u�̃����N���o�b�t�@�ւ̃|�C���^���擾
 *
 * @param   sv		
 * @param   extra_id		�O���Z�[�uID
 *
 * @retval  u16     �O���Z�[�u��CRC
 */
//==================================================================
u32 * SaveData_GetExtraLinkPtr(SAVE_CONTROL_WORK *sv, SAVE_EXTRA_ID extra_id)
{
	PLAYER_DATA * pd = SaveControl_DataPtrGet(sv, GMDATA_ID_PLAYER_DATA);
	return &pd->extra_link[extra_id];
}



//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   PLAYER_WORK�\���̂̃f�[�^������PLAYER_DATA���X�V����
 *
 * @param   pw		PLAYER_DATA�\����
 *
 * �t�B�[���h�̃Z�[�u�O�Ȃǂɂ�����Ăяo���K�v������
 */
//--------------------------------------------------------------
void SaveData_PlayerDataUpdate(SAVE_CONTROL_WORK * sv, const PLAYER_WORK *pw)
{
	PLAYER_DATA * pd = SaveControl_DataPtrGet(sv, GMDATA_ID_PLAYER_DATA);

	pd->mystatus = pw->mystatus;
}

//--------------------------------------------------------------
/**
 * @brief   �Z�[�u�f�[�^����PLAYER_WORK�\���̂̏������[�h����
 *
 * @param   pw		PLAYER_DATA�\����
 */
//--------------------------------------------------------------
void SaveData_PlayerDataLoad(SAVE_CONTROL_WORK * sv, PLAYER_WORK *pw)
{
	PLAYER_DATA * pd = SaveControl_DataPtrGet(sv, GMDATA_ID_PLAYER_DATA);

	pw->mystatus = pd->mystatus;
}
