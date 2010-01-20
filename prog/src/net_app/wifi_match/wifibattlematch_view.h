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
#include "wifibattlematch_data.h"
//-------------------------------------
///	�f�o�b�O
//=====================================
#ifdef PM_DEBUG
//#define DEBUG_DATA_CREATE
#endif //PM_DEBUGa

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�\���֌W�̒萔
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	BG�t���[��
//=====================================
enum
{
	//���C�����
	BG_FRAME_M_TEXT	=	GFL_BG_FRAME0_M, 
	BG_FRAME_M_FONT	=	GFL_BG_FRAME1_M, 
	BG_FRAME_M_CARD	=	GFL_BG_FRAME2_M, 
	BG_FRAME_M_BACK	=	GFL_BG_FRAME3_M, 

	//�T�u���
	BG_FRAME_S_TEXT	=	GFL_BG_FRAME0_S, 
	BG_FRAME_S_FONT	=	GFL_BG_FRAME1_S, 
	BG_FRAME_S_CARD	=	GFL_BG_FRAME2_S, 
	BG_FRAME_S_BACK	=	GFL_BG_FRAME3_S, 
} ;

//-------------------------------------
///	�p���b�g
//=====================================
enum
{
	//���C�����BG
	PLT_BG_M	  =	0,
  PLT_LIST_M  = 13,
  PLT_TEXT_M  = 14,
	PLT_FONT_M	= 15,

	//�T�u���BG
	PLT_BG_S	  = 0,
	PLT_FONT_S	= 15,

	//���C�����OBJ
  PLT_OBJ_M   = 0,
  PLT_OBJ_PMS_M = 5,
	
	//�T�u���OBJ
  PLT_OBJ_S   = 0,
} ;

//-------------------------------------
///	�L����
//=====================================
enum
{ 
  //���C�����  BG_FRAME_M_TEXT
  CGR_OFS_M_CLEAR = 0,
  CGR_OFS_M_LIST  = 1,
  CGR_OFS_M_TEXT  = 10,
};

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					���\�[�X�ǂݍ���
 *					  ���ʑf�ނ�ǂݍ��݂܂�
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�萔
//=====================================
typedef enum
{ 
  WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_PLT_M,
  WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CHR_M,
  WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CEL_M,
  WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_PLT_S,
  WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CHR_S,
  WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_CEL_S,
  WIFIBATTLEMATCH_VIEW_RES_TYPE_OBJ_MAX,
}WIFIBATTLEMATCH_VIEW_RES_TYPE;

//-------------------------------------
///	���\�[�X�Ǘ����[�N
//=====================================
typedef struct _WIFIBATTLEMATCH_VIEW_RESOURCE WIFIBATTLEMATCH_VIEW_RESOURCE;

//-------------------------------------
///	�p�u���b�N
//=====================================
extern WIFIBATTLEMATCH_VIEW_RESOURCE *WIFIBATTLEMATCH_VIEW_LoadResource( GFL_CLUNIT *p_unit, WIFIBATTLEMATCH_MODE mode, HEAPID heapID );
extern void WIFIBATTLEMATCH_VIEW_UnLoadResource( WIFIBATTLEMATCH_VIEW_RESOURCE *p_wk );

extern u32 WIFIBATTLEMATCH_VIEW_GetResource( const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_wk, WIFIBATTLEMATCH_VIEW_RES_TYPE type );

extern void WIFIBATTLEMATCH_VIEW_Main( WIFIBATTLEMATCH_VIEW_RESOURCE *p_wk );

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
#define PLAYERINFO_BG_FRAME	        (BG_FRAME_S_FONT)
#define PLAYERINFO_PLT_BG_FONT	    (15)
#define PLAYERINFO_PLT_OBJ_POKEICON	(0)
#define PLAYERINFO_PLT_OBJ_TRAINER	(PLAYERINFO_PLT_OBJ_POKEICON+POKEICON_PAL_MAX)

#define PLAYERINFO_POKEICON_START_X		(32)
#define PLAYERINFO_POKEICON_DIFF_X		(32)
#define PLAYERINFO_POKEICON_Y					(176)

#define PLAYERINFO_TRAINER_CUP_X			(40)
#define PLAYERINFO_TRAINER_CUP_Y			(112-6)
#define PLAYERINFO_TRAINER_RND_X			(32)
#define PLAYERINFO_TRAINER_RND_Y			(74-6)

#define PLAYERINFO_ALPHA_EV1           (15)
#define PLAYERINFO_ALPHA_EV2           (3)

#define PLAYERINFO_STR_COLOR_BLACK    (PRINTSYS_LSB_Make( 1,2,0 ))
#define PLAYERINFO_STR_COLOR_WHITE    (PRINTSYS_LSB_Make( 0xF,2,0 ))

#define WBM_CARD_WIN_BLACK  (500)
#define WBM_CARD_WIN_GOLD   (100)
#define WBM_CARD_WIN_SILVER (50)
#define WBM_CARD_WIN_BRONZE (20)
#define WBM_CARD_WIN_COPPER (10)
#define WBM_CARD_WIN_NORMAL (0)

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
	WIFIBATTLEMATCH_BTLRULE		btl_rule;				//���̃o�g����
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

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	�����_���}�b�`
//=====================================
extern PLAYERINFO_WORK *PLAYERINFO_RND_Init( const PLAYERINFO_RANDOMMATCH_DATA *cp_data, BOOL is_rate, const MYSTATUS* p_my, GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, GFL_FONT	*p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, HEAPID heapID );
extern void PLAYERINFO_RND_Exit( PLAYERINFO_WORK *p_wk );
//-------------------------------------
///	WIFI���
//=====================================
extern PLAYERINFO_WORK *PLAYERINFO_WIFI_Init( const PLAYERINFO_WIFICUP_DATA *cp_data, BOOL is_limit, const MYSTATUS* p_my, GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, GFL_FONT	*p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, HEAPID heapID );
extern void PLAYERINFO_WIFI_Exit( PLAYERINFO_WORK *p_wk );
//-------------------------------------
///	LIVE���
//=====================================
extern PLAYERINFO_WORK *PLAYERINFO_LIVE_Init( const PLAYERINFO_LIVECUP_DATA *cp_data, const MYSTATUS* p_my, GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, GFL_FONT *p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, HEAPID heapID );
extern void PLAYERINFO_LIVE_Exit( PLAYERINFO_WORK *p_wk );
//-------------------------------------
///	���ʊ֐�
//=====================================
extern BOOL PLAYERINFO_PrintMain( PLAYERINFO_WORK * p_wk, PRINT_QUE *p_que );
extern BOOL PLAYERINFO_MoveMain( PLAYERINFO_WORK * p_wk );

#ifdef DEBUG_DATA_CREATE
extern void PLAYERINFO_DEBUG_CreateRndData( WIFIBATTLEMATCH_MODE mode, void *p_data_adrs );
extern void PLAYERINFO_DEBUG_CreateWifiData( WIFIBATTLEMATCH_MODE mode, void *p_data_adrs );
extern void PLAYERINFO_DEBUG_CreateLiveData( WIFIBATTLEMATCH_MODE mode, void *p_data_adrs );
#else
#define PLAYERINFO_DEBUG_CreateRndData(a,b)		/*  */
#define PLAYERINFO_DEBUG_CreateWifiData(a,b)		/*  */
#define PLAYERINFO_DEBUG_CreateLiveData(a,b)		/*  */
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
#define MATCHINFO_BG_FRAME	  (BG_FRAME_M_FONT)
#define MATCHINFO_PLT_BG_FONT	(PLT_FONT_M)
#define MATCHINFO_PLT_OBJ_PMS	(PLT_OBJ_PMS_M)
#define MATCHINFO_PLT_OBJ_TRAINER	(MATCHINFO_PLT_OBJ_PMS+PMS_DRAW_OBJ_PLTT_NUM)

#define MATCHINFO_TRAINER_X			(32)
#define MATCHINFO_TRAINER_Y			(48 - 6)
//=============================================================================
/**
 *					�\���̐錾
 */
//=============================================================================
//-------------------------------------
///	�ΐ�ҏ��\�����[�N
//=====================================
typedef struct _MATCHINFO_WORK MATCHINFO_WORK;

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
extern MATCHINFO_WORK	* MATCHINFO_Init( const WIFIBATTLEMATCH_ENEMYDATA *cp_data, GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, GFL_FONT	*p_font, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, WORDSET *p_word, HEAPID heapID );
extern void MATCHINFO_Exit( MATCHINFO_WORK *p_wk );
extern BOOL MATCHINFO_PrintMain( MATCHINFO_WORK *p_wk, PRINT_QUE *p_que );
extern BOOL MATCHINFO_MoveMain( MATCHINFO_WORK * p_wk );

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/**
 *					�ҋ@��OBJ
*/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//-------------------------------------
///	�ΐ�ҏ��\�����[�N
//=====================================
typedef struct _WBM_WAITICON_WORK WBM_WAITICON_WORK;

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
extern WBM_WAITICON_WORK	* WBM_WAITICON_Init( GFL_CLUNIT *p_unit, const WIFIBATTLEMATCH_VIEW_RESOURCE *cp_res, HEAPID heapID );
extern void WBM_WAITICON_Exit( WBM_WAITICON_WORK *p_wk );
extern void WBM_WAITICON_Main( WBM_WAITICON_WORK *p_wk );
extern void WBM_WAITICON_SetDrawEnable( WBM_WAITICON_WORK *p_wk, BOOL on_off );

#ifdef DEBUG_DATA_CREATE
//extern void MATCHINFO_DEBUG_CreateData( MATCHINFO_DATA *p_data );
#else
#define MATCHINFO_DEBUG_CreateData(a)		/*  */
#endif //DEBUG_DATA_CREATE
