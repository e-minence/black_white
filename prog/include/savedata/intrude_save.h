//==============================================================================
/**
 * @file    intrude_save.h
 * @brief   �N���Z�[�u�f�[�^
 * @author  matsuda
 * @date    2009.10.18(��)
 */
//==============================================================================
#pragma once

#include "gamesystem/gamedata_def.h"  //GAMEDATA
#include "field/intrude_common.h"
#include "buflen.h"
#include "field/gpower_id.h"
#include "savedata/save_control.h"


//==============================================================================
//  �萔��`
//==============================================================================
enum {
  OCCUPY_ID_MAX = FIELD_COMM_MEMBER_MAX + 1,

  OCCUPY_ID_MINE = OCCUPY_ID_MAX - 1,   ///<�������g�̐苒���ID
};

///�苒���x���̍ő�l
#define OCCUPY_LEVEL_MAX    (999)

///�~�b�V�����N���A��
enum{
  MISSION_CLEAR_NONE,       ///<�N���A���Ă��Ȃ�
  MISSION_CLEAR_WHITE,      ///<���ŃN���A�B��
  MISSION_CLEAR_BLACK,      ///<���ŃN���A�B��
};

///G�p���[�z�z��Mbit�̃��[�N��
#define INTRUDE_SAVE_DISTRIBUTION_BIT_WORK_MAX  (2)

//==============================================================================
//  �\���̒�`
//==============================================================================
///�N���Z�[�u�\���̂̕s���S�^
typedef struct _INTRUDE_SAVE_WORK INTRUDE_SAVE_WORK;

//--------------------------------------------------------------
//  �苒
//--------------------------------------------------------------
///�~�b�V�������X�g��
#define MISSION_LIST_MAX   (6)  //MISSION_TYPE_MAX�Ɠ����l�ł���K�v������܂�

///�~�b�V�������X�g�X�e�[�^�X
typedef struct{
  u8 mission_no[MISSION_LIST_MAX];      ///<�~�b�V�����ԍ�
  u8 mission_clear[MISSION_LIST_MAX];   ///<�~�b�V�����N���A��(MISSION_CLEAR_xxx)
}MISSION_LIST_STATUS;

///�苒���
typedef struct{
  MISSION_LIST_STATUS mlst;              ///<�~�b�V�������X�g�X�e�[�^�X
  u16 white_level;                       ///<�����x��
  u16 black_level;                       ///<�����x��
}OCCUPY_INFO;

//--------------------------------------------------------------
//  �B���A�C�e��
//--------------------------------------------------------------
///�N���B���A�C�e���Z�[�u���[�N
typedef struct{
  STRCODE name[PERSON_NAME_SIZE + EOM_SIZE];  ///<�B�����l�̖��O
  u16 item;                                   ///<�A�C�e���ԍ�
  u8 tbl_no;                                  ///<�B����Ă���ꏊ�������e�[�u���ԍ�
  u8 padding;
}INTRUDE_SECRET_ITEM_SAVE;


//==============================================================================
//  
//==============================================================================
extern INTRUDE_SAVE_WORK * SaveData_GetIntrude( SAVE_CONTROL_WORK * p_sv);


//============================================================================================
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief   OCCUPY_INFO�ւ̃|�C���^�擾(�v���C���[ID�w��)
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @param   player_id   �v���C���[ID
 * @retval  OCCUPY_INFO�ւ̃|�C���^
 * @note
 * ���Ԃ�src/gamesystem/game_data.c�ɂ��邪�A���B����
 * �l�����ăA�N�Z�X�֐��Q�������w�b�_�ɔz�u���Ă���
 */
//--------------------------------------------------------------
extern OCCUPY_INFO * GAMEDATA_GetOccupyInfo(GAMEDATA * gamedata, u32 player_id);
//--------------------------------------------------------------
/**
 * @brief   OCCUPY_INFO�ւ̃|�C���^�擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @retval  OCCUPY_INFO�ւ̃|�C���^
 * @note
 * ���Ԃ�src/gamesystem/game_data.c�ɂ��邪�A���B����
 * �l������BGM_INFO_SYS�������w�b�_�ɔz�u���Ă���
 */
//--------------------------------------------------------------
extern OCCUPY_INFO * GAMEDATA_GetMyOccupyInfo(GAMEDATA * gamedata);

//==============================================================================
//  �O���֐��錾
//==============================================================================
extern u32 IntrudeSave_GetWorkSize( void );
extern void IntrudeSave_WorkInit(void *work);

//--------------------------------------------------------------
//  �苒���
//--------------------------------------------------------------
extern void OccupyInfo_WorkInit(OCCUPY_INFO *occupy);
extern u16 OccupyInfo_GetWhiteLevel(const OCCUPY_INFO *occupy);
extern u16 OccupyInfo_GetBlackLevel(const OCCUPY_INFO *occupy);
extern void SaveData_OccupyInfoUpdate(SAVE_CONTROL_WORK * sv, const OCCUPY_INFO *occupy);
extern void SaveData_OccupyInfoLoad(SAVE_CONTROL_WORK * sv, OCCUPY_INFO *dest_occupy);
extern void OccupyInfo_LevelUpWhite(OCCUPY_INFO *occupy, int add_level);
extern void OccupyInfo_LevelUpBlack(OCCUPY_INFO *occupy, int add_level);

//--------------------------------------------------------------
//  �B���A�C�e��
//--------------------------------------------------------------
extern void ISC_SAVE_SetItem(INTRUDE_SAVE_WORK *intsave, const INTRUDE_SECRET_ITEM_SAVE *src);

//--------------------------------------------------------------
//  G�p���[
//--------------------------------------------------------------
extern void ISC_SAVE_SetGPowerID(INTRUDE_SAVE_WORK *intsave, GPOWER_ID gpower_id);
extern GPOWER_ID ISC_SAVE_GetGPowerID(INTRUDE_SAVE_WORK *intsave);
extern void ISC_SAVE_SetDistributionGPower(INTRUDE_SAVE_WORK *intsave, GPOWER_ID gpower_id);
extern BOOL ISC_SAVE_GetDistributionGPower(INTRUDE_SAVE_WORK *intsave, GPOWER_ID gpower_id);
extern void ISC_SAVE_GetDistributionGPower_Array(INTRUDE_SAVE_WORK *intsave, u8 *dest_array, int array_num);

//--------------------------------------------------------------
//  �p���X
//--------------------------------------------------------------
extern u32 ISC_SAVE_GetMissionClearCount(INTRUDE_SAVE_WORK *intsave);
extern s64 ISC_SAVE_GetPalaceSojournTime(INTRUDE_SAVE_WORK *intsave);
extern void ISC_SAVE_SetPalaceSojournTime(INTRUDE_SAVE_WORK *intsave, s64 second);
