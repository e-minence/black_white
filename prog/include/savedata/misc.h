//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   misc.h
 *  @brief  �����ރZ�[�u�f�[�^�A�N�Z�X
 *  @author Toru=Nagihashi
 *  @date   2009.10.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "savedata/save_control.h"
#include "system/pms_data.h"

//=============================================================================
/**
 *          �萔�錾
*/
//=============================================================================
///����Ⴂ�ʐM�ł��������ꂽ�񐔂̍ő吔
#define CROSS_COMM_THANKS_RECV_COUNT_MAX    (99999)
///����Ⴂ�ʐM�����l���̍ő吔
#define CROSS_COMM_SURETIGAI_COUNT_MAX      (99999)

enum {
  MY_GOLD_MAX = 9999999,
};

#define NAMEIN_SAVEMODE_MAX (8)

///�^�C�g�����j���[�t���O��`
typedef enum {
  MISC_STARTMENU_FLAG_OPEN = 1,   ///<�\������
  MISC_STARTMENU_FLAG_VIEW = 2,   ///<�\���ς�
}MISC_STARTMENU_FLAG;

///�^�C�g�����j���[�t���O�ݒ��`
typedef enum {
  MISC_STARTMENU_TYPE_HUSHIGI = 0,  ///<�u�s�v�c�ȑ��蕨�v���j���[�\��
  MISC_STARTMENU_TYPE_BATTLE,       ///<�u�o�g�����v���j���[�\��
  MISC_STARTMENU_TYPE_GAMESYNC,     ///<�u�Q�[���V���N�ݒ�v���j���[�\��
  MISC_STARTMENU_TYPE_MACHINE,      ///<�u�]���}�V�����g���v���j���[�\��
}MISC_STARTMENU_TYPE;


//=============================================================================
/**
 *          �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// �����ރZ�[�u�f�[�^�s���S�^
//=====================================
typedef struct _MISC MISC;

//=============================================================================
/**
 *          �O���Q��
*/
//=============================================================================
//----------------------------------------------------------
//�Z�[�u�f�[�^�V�X�e���Ɉˑ�����֐�
//----------------------------------------------------------
extern int MISC_GetWorkSize( void );
extern void MISC_Init( MISC *p_msc );

//----------------------------------------------------------
//�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
extern const MISC * SaveData_GetMiscConst( const SAVE_CONTROL_WORK * cp_sv);
extern MISC * SaveData_GetMisc( SAVE_CONTROL_WORK * p_sv);

//----------------------------------------------------------
//���ꂼ��̎擾�֐�
//----------------------------------------------------------
//���O����
extern void MISC_SetNameInMode( MISC *p_misc, u32 mode, u8 input_type );
extern u8 MISC_GetNameInMode( const MISC *cp_misc, u32 mode );
//GDS
extern void MISC_SetFavoriteMonsno(MISC * misc, int monsno, int form_no, int egg_flag, int sex );
extern void MISC_GetFavoriteMonsno(const MISC * misc, int *monsno, int *form_no, int *egg_flag, int *p_sex );
//palpark
extern const u32  MISC_GetPalparkHighscore(const MISC *misc);
extern void  MISC_SetPalparkHighscore(MISC *misc , u32 score);
extern const u8  MISC_GetPalparkFinishState(const MISC *misc);
extern void  MISC_SetPalparkFinishState(MISC *misc , u8 state);
//����Ⴂ
extern u32 MISC_CrossComm_GetThanksRecvCount(const MISC *misc);
extern void MISC_CrossComm_SetThanksRecvCount(MISC *misc, u32 count );
extern u32 MISC_CrossComm_IncThanksRecvCount(MISC *misc);
extern u32 MISC_CrossComm_GetSuretigaiCount(const MISC *misc);
extern void MISC_CrossComm_SetSuretigaiCount(MISC *misc, u32 count );
extern u32 MISC_CrossComm_IncSuretigaiCount(MISC *misc);
extern const STRCODE * MISC_CrossComm_GetSelfIntroduction(const MISC *misc);
extern void MISC_CrossComm_SetSelfIntroduction(MISC *misc, const STRBUF *srcbuf);
extern const STRCODE * MISC_CrossComm_GetThankyouMessage(const MISC *misc);
extern void MISC_CrossComm_SetThankyouMessage(MISC *misc, const STRBUF *srcbuf);
extern int MISC_CrossComm_GetResearchTeamRank(const MISC *misc);
extern void MISC_CrossComm_SetResearchTeamRank(MISC *misc, int rank);
//�o�b�W  
extern BOOL MISC_GetBadgeFlag(const MISC *misc, int badge_id);
extern void MISC_SetBadgeFlag(MISC *misc, int badge_id);
extern int MISC_GetBadgeCount(const MISC *misc);
//���� 
extern u32 MISC_GetGold(const MISC *misc);
extern u32 MISC_SetGold(MISC *misc, u32 gold);
extern u32 MISC_AddGold(MISC *misc, u32 add);
extern u32 MISC_SubGold(MISC *misc, u32 sub);
//�W���������̃f�[�^�o�^
extern void MISC_SetGymVictoryInfo( MISC * misc, int gym_id, const u16 * monsnos );
extern void MISC_GetGymVictoryInfo( const MISC * misc, int gym_id, u16 * monsnos );
//�^�C�g�����j���[
extern void MISC_SetStartMenuFlag( MISC * misc, MISC_STARTMENU_TYPE type, MISC_STARTMENU_FLAG flg );
extern u8 MISC_GetStartMenuFlag( const MISC * misc, MISC_STARTMENU_TYPE type );
extern u8 MISC_GetStartMenuFlagAll( const MISC * misc );
extern void MISC_SetStartMenuFlagAll( MISC * misc, u8 flg );
// ����Ⴂ������
extern u8 MISC_GetResearchRequestID( const MISC* misc );
extern void MISC_SetResearchRequestID( MISC* misc, u8 id );
extern u8 MISC_GetResearchQuestionID( const MISC* misc, u8 idx );
extern void MISC_SetResearchQuestionID( MISC* misc, u8 idx, u8 id );
extern u16 MISC_GetResearchCount( const MISC* misc, u8 idx );
extern void MISC_SetResearchCount( MISC* misc, u8 idx, u16 count );
extern void MISC_AddResearchCount( MISC* misc, u8 idx, u16 add );
extern s64 MISC_GetResearchStartTimeBySecond( const MISC* misc );
extern void MISC_SetResearchStartTimeBySecond( MISC* misc, s64 time );
//�C�x���g���b�N
extern void MISC_SetEvtRockValue( MISC* misc, const int inNo, const u32 inValue );
extern u32 MISC_GetEvtRockValue( MISC* misc, const int inNo );
// �d���f����
extern u16 MISC_GetChampNewsMinutes( const MISC* misc );
extern void MISC_SetChampNewsMinutes( MISC* misc, u16 minutes );
extern void MISC_DecChampNewsMinutes( MISC* misc, u16 minutes );
//�o�g�����R�[�_�[
extern u8 MISC_GetBattleRecorderColor( const MISC * misc );
extern void MISC_SetBattleRecorderColor( MISC * misc, u8 color );
