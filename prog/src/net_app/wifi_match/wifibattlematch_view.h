//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_view.h
 *	@brief	�����_���}�b�`�\���֌W
 *	@author	Toru=Nagihashi
 *	@date		2009.11.04
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include "net_app/wifibattlematch.h"
#include "system/pms_data.h"
#include "pm_define.h"
#include "pokeicon/pokeicon.h"
#include "system/pms_draw.h"

//-------------------------------------
///	�f�o�b�O
//=====================================
#ifdef PM_DEBUG
#define DEBUG_DATA_CREATE
#endif //PM_DEBUG


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�v���C���[���\��
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//=============================================================================
/**
 *					�萔
*/
//=============================================================================
#define PLAYERINFO_BG_FRAME	(GFL_BG_FRAME0_S)
#define PLAYERINFO_PLT_BG_FONT	(15)
#define PLAYERINFO_PLT_OBJ_POKEICON	(0)
#define PLAYERINFO_PLT_OBJ_TRAINER	(PLAYERINFO_PLT_OBJ_POKEICON+POKEICON_PAL_MAX)

#define PLAYERINFO_POKEICON_START_X		(32)
#define PLAYERINFO_POKEICON_DIFF_X		(32)
#define PLAYERINFO_POKEICON_Y					(176)

#define PLAYERINFO_TRAINER_CUP_X			(40)
#define PLAYERINFO_TRAINER_CUP_Y			(112)
#define PLAYERINFO_TRAINER_RND_X			(40)
#define PLAYERINFO_TRAINER_RND_Y			(64)
//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�v���C���[���\�����[�N
//=====================================
typedef struct _PLAYERINFO_WORK PLAYERINFO_WORK;

//-------------------------------------
///	�����_���}�b�`��\������Ƃ��ɕK�v�ȃf�[�^
//=====================================
typedef struct
{
	BtlRule								btl_rule;				//���̃o�g����
	u16										rate;						//���[�e�B���O
	u32										trainerID;			//�����̌�����
	u16										btl_cnt;				//�ΐ��
	u16										win_cnt;				//������
	u16										lose_cnt;				//������
} PLAYERINFO_RANDOMMATCH_DATA;
//-------------------------------------
///	WIFI���̏��ʂɕ\������f�[�^
//=====================================
typedef struct
{
	STRCODE								cup_name[72+1];	//��������
	GFDATE								start_date;			//���J�n
	GFDATE								end_date;				//���I��
	u32										trainerID;			//�����̌�����
	u16										rate;						//���[�e�B���O
	u16										btl_cnt;				//�ΐ��
	u16										btl_max;				//�ΐ�񐔏��
	u16										poke[TEMOTI_POKEMAX];				//�g�p�|�P����
	u16										form[TEMOTI_POKEMAX];				//�g�p�|�P�����̃t�H����
} PLAYERINFO_WIFICUP_DATA;
//-------------------------------------
///	���C�u���̏��ʂɕ\������f�[�^
//=====================================
typedef struct
{
	STRCODE								cup_name[72+1];	//��������
	GFDATE								start_date;			//���J�n
	GFDATE								end_date;				//���I��
	u32										trainerID;			//�����̌�����
	u16										win_cnt;				//������
	u16										lose_cnt;				//������
	u16										btl_cnt;				//�ΐ��
	u16										btl_max;				//�ΐ�񐔏��
	u16										poke[TEMOTI_POKEMAX];				//�g�p�|�P����
	u16										form[TEMOTI_POKEMAX];				//�g�p�|�P�����̃t�H����
} PLAYERINFO_LIVECUP_DATA;

//-------------------------------------
///	�
//=====================================
typedef union
{	
	PLAYERINFO_RANDOMMATCH_DATA	rnd;
	PLAYERINFO_WIFICUP_DATA			wifi;
	PLAYERINFO_LIVECUP_DATA			live;
} PLAYERINFO_DATA;

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
extern PLAYERINFO_WORK *PLAYERINFO_Init( WIFIBATTLEMATCH_MODE mode, const void *cp_data_adrs, const MYSTATUS* p_my, GFL_CLUNIT *p_unit, GFL_FONT	*p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, HEAPID heapID );
extern void PLAYERINFO_Exit( PLAYERINFO_WORK *p_wk );
extern BOOL PLAYERINFO_PrintMain( PLAYERINFO_WORK * p_wk, PRINT_QUE *p_que );

#ifdef DEBUG_DATA_CREATE
extern void PLAYERINFO_DEBUG_CreateData( WIFIBATTLEMATCH_MODE mode, void *p_data_adrs );
#else
#define PLAYERINFO_DEBUG_CreateData(a,b)		/*  */
#endif //DEBUG_DATA_CREATE

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�ΐ�ҏ��\��
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//=============================================================================
/**
 *					�萔
*/
//=============================================================================
#define MATCHINFO_BG_FRAME	(GFL_BG_FRAME0_M)
#define MATCHINFO_PLT_BG_FONT	(15)
#define MATCHINFO_PLT_OBJ_PMS	(0)
#define MATCHINFO_PLT_OBJ_TRAINER	(MATCHINFO_PLT_OBJ_PMS+PMS_DRAW_OBJ_PLTT_NUM)

#define MATCHINFO_TRAINER_X			(28)
#define MATCHINFO_TRAINER_Y			(20)
//=============================================================================
/**
 *					�\���̐錾
 */
//=============================================================================
//-------------------------------------
///	�ΐ�ҏ��\�����[�N
//=====================================
typedef struct _MATCHINFO_WORK MATCHINFO_WORK;

//-------------------------------------
///	�ΐ�ҏ��\���f�[�^
//=====================================
typedef struct 
{	
	STRCODE								name[7+1];	
	u32										trainerID;
	u16										rate;
	u8										area;
	u8										country;
	PMS_DATA							pms;
} MATCHINFO_DATA;

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
extern MATCHINFO_WORK	* MATCHINFO_Init( const MATCHINFO_DATA *cp_data, GFL_CLUNIT *p_unit, GFL_FONT	*p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, HEAPID heapID );
extern void MATCHINFO_Exit( MATCHINFO_WORK *p_wk );
extern BOOL MATCHINFO_PrintMain( MATCHINFO_WORK *p_wk, PRINT_QUE *p_que );

#ifdef DEBUG_DATA_CREATE
extern void MATCHINFO_DEBUG_CreateData( MATCHINFO_DATA *p_data );
#else
#define MATCHINFO_DEBUG_CreateData(a)		/*  */
#endif //DEBUG_DATA_CREATE
