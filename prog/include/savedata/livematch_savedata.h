//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		livematch_btlscore_data.h
 *	@brief  ���C�u�}�b�`��уZ�[�u�f�[�^
 *	@author	Toru=Nagihashi
 *	@date		2010.03.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�����̏��
//=====================================
typedef enum
{
  LIVEMATCH_MYDATA_PARAM_BTLCNT,
  LIVEMATCH_MYDATA_PARAM_WIN,
  LIVEMATCH_MYDATA_PARAM_LOSE,
  LIVEMATCH_MYDATA_PARAM_DUMMY, //�p�f�B���O

  LIVEMATCH_MYDATA_PARAM_MAX
} LIVEMATCH_MYDATA_PARAM;

//-------------------------------------
///	�ΐ푊��̏��
//=====================================
typedef enum
{
  LIVEMATCH_FOEDATA_PARAM_REST_POKE,
  LIVEMATCH_FOEDATA_PARAM_REST_HP,

  LIVEMATCH_FOEDATA_PARAM_MAX
} LIVEMATCH_FOEDATA_PARAM;

//-------------------------------------
///	�ΐ푊��̋L�^�ő�
//=====================================
#define LIVEMATCH_FOEDATA_MAX   (10)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�Z�[�u�f�[�^�s���S�^
//=====================================
typedef struct _LIVEMATCH_DATA LIVEMATCH_DATA;

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//----------------------------------------------------------
//�Z�[�u�f�[�^�V�X�e���Ɉˑ�����֐�
//----------------------------------------------------------
extern int LIVEMATCH_GetWorkSize( void );
extern void LIVEMATCH_Init( LIVEMATCH_DATA *p_wk );

//���[�N�擾�́B�o�g�����p�Z�[�u�f�[�^battlematch_savedata.h�ɂ���܂�

//----------------------------------------------------------
//�擾�A���삷��֐�
//----------------------------------------------------------
//�����Ɋ֌W����f�[�^
//�擾
extern void LIVEMATCH_DATA_GetMyMacAddr( const LIVEMATCH_DATA *cp_wk, u8* p_mac_addr );
extern int LIVEMATCH_DATA_GetMyParam( const LIVEMATCH_DATA *cp_wk, LIVEMATCH_MYDATA_PARAM param );
//�ݒ�
//extern void LIVEMATCH_DATA_SetMyMacAddr( const LIVEMATCH_DATA *cp_wk, const u8* cp_mac_addr );
extern void LIVEMATCH_DATA_SetMyParam( LIVEMATCH_DATA *p_wk, LIVEMATCH_MYDATA_PARAM param, int data );
extern void LIVEMATCH_DATA_AddMyParam( LIVEMATCH_DATA *p_wk, LIVEMATCH_MYDATA_PARAM param, int data );

//����Ɋ֌W����f�[�^
//�擾
extern void LIVEMATCH_DATA_GetFoeMacAddr( const LIVEMATCH_DATA *cp_wk, u32 index, u8* p_mac_addr );
extern int LIVEMATCH_DATA_GetFoeParam( const LIVEMATCH_DATA *cp_wk, u32 index, LIVEMATCH_FOEDATA_PARAM param );
//�ݒ�
extern void LIVEMATCH_DATA_SetFoeMacAddr( LIVEMATCH_DATA *p_wk, u32 index, const u8* cp_mac_addr );
extern void LIVEMATCH_DATA_SetFoeParam( LIVEMATCH_DATA *p_wk, u32 index, LIVEMATCH_FOEDATA_PARAM param, int data );

//�}�b�N�A�h���X������̃��m�����邩
extern BOOL LIVEMATCH_DATA_IsSameMacAddress( const LIVEMATCH_DATA *cp_wk, const u8 *cp_mac_addr );
