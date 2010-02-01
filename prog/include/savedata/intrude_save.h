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


//==============================================================================
//  �萔��`
//==============================================================================
enum {
  OCCUPY_ID_MAX = FIELD_COMM_MEMBER_MAX + 1,

  OCCUPY_ID_MINE = OCCUPY_ID_MAX - 1,   ///<�������g�̐苒���ID
};

//==============================================================================
//  �\���̒�`
//==============================================================================
///�N���Z�[�u�\���̂̕s���S�^
typedef struct _INTRUDE_SAVE_WORK INTRUDE_SAVE_WORK;

//--------------------------------------------------------------
//  �苒
//--------------------------------------------------------------
///�苒���F�X
typedef struct{
  ///�X�̐苒�l   (WHITE ---- OCCUPY_TOWN_NEUTRALITY ---- OCCUPY_TOWN_BLACK)
  u16 town_occupy[INTRUDE_TOWN_MAX];
}OCCUPY_INFO_TOWN;

///�苒���
typedef struct{
  OCCUPY_INFO_TOWN town;                 ///<�X�̐苒���
  u16 intrude_point;                     ///<�N���|�C���g
  u8 intrude_level;                      ///<�N�����x��
  u8 padding;
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
extern void SaveData_OccupyInfoUpdate(SAVE_CONTROL_WORK * sv, const OCCUPY_INFO *occupy);
extern void SaveData_OccupyInfoLoad(SAVE_CONTROL_WORK * sv, OCCUPY_INFO *dest_occupy);
extern u32 OccupyInfo_GetIntrudeLevel(const OCCUPY_INFO *occupy);

//--------------------------------------------------------------
//  �B���A�C�e��
//--------------------------------------------------------------
extern void ISC_SAVE_SetItem(INTRUDE_SAVE_WORK *intsave, const INTRUDE_SECRET_ITEM_SAVE *src);
