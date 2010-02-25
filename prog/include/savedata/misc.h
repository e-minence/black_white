//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		misc.h
 *	@brief	�����ރZ�[�u�f�[�^�A�N�Z�X
 *	@author	Toru=Nagihashi
 *	@date		2009.10.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "savedata/save_control.h"
#include "system/pms_data.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
///����Ⴂ�ʐM�ł��������ꂽ�񐔂̍ő吔
#define CROSS_COMM_THANKS_RECV_COUNT_MAX    (99999)
///����Ⴂ�ʐM�����l���̍ő吔
#define CROSS_COMM_SURETIGAI_COUNT_MAX      (99999)

enum {
  MY_GOLD_MAX = 999999,
};

#define NAMEIN_SAVEMODE_MAX (8)


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�����ރZ�[�u�f�[�^�s���S�^
//=====================================
typedef struct _MISC MISC;

//=============================================================================
/**
 *					�O���Q��
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
extern void MISC_SetFavoriteMonsno(MISC * misc, int monsno, int form_no, int egg_flag);
extern void MISC_GetFavoriteMonsno(const MISC * misc, int *monsno, int *form_no, int *egg_flag);
//palpark
extern const u32  MISC_GetPalparkHighscore(const MISC *misc);
extern void  MISC_SetPalparkHighscore(MISC *misc , u32 score);
extern const u8  MISC_GetPalparkFinishState(const MISC *misc);
extern void  MISC_SetPalparkFinishState(MISC *misc , u8 state);
//����Ⴂ
extern u32 MISC_CrossComm_GetThanksRecvCount(const MISC *misc);
extern u32 MISC_CrossComm_IncThanksRecvCount(MISC *misc);
extern u32 MISC_CrossComm_GetSuretigaiCount(const MISC *misc);
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


