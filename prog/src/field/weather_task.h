//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather_task.h
 *	@brief		�P�V�C�@����Ǘ�
 *	@author		tomoya takahshi
 *	@date		2009.03.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifdef _cplusplus
extern "C"{
#endif

#ifndef __WEATHER_TASK_H__
#define __WEATHER_TASK_H__

#include <gflib.h>

#include "fieldmap.h"
#include "field_common.h"
#include "field_camera.h"

#include "field_light.h"
#include "field_fog.h"
#include "field_zonefog.h"
#include "field_3dbg.h"
#include "field_oam_pal.h"

#include "weather_snd.h"



//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	���������[�h
//=====================================
typedef enum {
	WEATHER_TASK_INIT_NORMAL,		// �ʏ�ǂݍ���
	WEATHER_TASK_INIT_DIV,			// �����ǂݍ���

	WEATHER_TASK_INIT_MODE_NUM,		// �V�X�e�������Ŏg�p
} WEATEHR_TASK_INIT_MODE;


//-------------------------------------
///	�^�X�N���
//=====================================
typedef enum {
	WEATHER_TASK_INFO_NONE,				// ���Ȃ�
	WEATHER_TASK_INFO_LOADING,			// �ǂݍ��ݒ�
	WEATHER_TASK_INFO_FADEIN,			// �t�F�[�h�C����
	WEATHER_TASK_INFO_MAIN,				// ���C�����쒆
	WEATHER_TASK_INFO_FADEOUT,			// �t�F�[�h�A�E�g��
	WEATHER_TASK_INFO_DEST,				// �j����


	WEATHER_TASK_INFO_NUM,				// �V�X�e�����Ŏg�p
} WEATHER_TASK_INFO;

//-------------------------------------
///	�Ǘ��֐��̖߂�l
//=====================================
typedef enum {
	WEATHER_TASK_FUNC_RESULT_CONTINUE,	// �Â�
	WEATHER_TASK_FUNC_RESULT_FINISH,	// ����


	WEATHER_TASK_FUNC_RESULT_NUM,		// �V�X�e�����Ŏg�p
} WEATHER_TASK_FUNC_RESULT;

//-------------------------------------
///	FOG�Ǘ��t���O
//=====================================
typedef enum {
	WEATHER_TASK_FOG_NONE,		// �t�H�O������s��Ȃ�
	WEATHER_TASK_FOG_USE,		// �V�����t�H�O��ݒ肷��
	WEATHER_TASK_FOG_WITH,		// �O�̐ݒ�������p���ݒ肷��

	WEATHER_TASK_FOG_MODE_MAX,	// �v���O�������Ŏg�p
} WEATHER_TASK_FOG_MODE;



//-------------------------------------
///	���\�[�X���
//=====================================
#define WEATHER_TASK_GRAPHIC_OAM_PLTT_NUM_MAX	( 1 )		// �p���b�g�̃T�C�Y
#define WEATHER_TASK_GRAPHIC_OAM_PLTT_OFS		( FLDOAM_PALNO_WEATHER*32 )	// �p���b�g�̓]���ʒu


//-------------------------------------
///	�V�C�I�u�W�F�ێ��A���[�N�T�C�Y
//=====================================
#define WEATHER_OBJ_WORK_USE_WORKSIZE	( sizeof(s32) * 10 )


//-------------------------------------
/// DP�@����ڐA�@��{���	
//=====================================
#define		WEATHER_FOG_DEPTH_DEFAULT	      ( 0x7880 )	// �t�H�O�f�t�H���g�l
#define		WEATHER_FOG_DEPTH_DEFAULT_START	( 0x7FFF )	// �t�H�O�f�t�H���g�t�F�[�h�J�n�l
//#define		WEATHER_FOG_DEPTH_DEFAULT	( 0x200 )	// �t�H�O�f�t�H���g�l
#define		WEATHER_FOG_SLOPE_DEFAULT	( 4 )



//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�C�^�X�N
//=====================================
typedef struct _WEATHER_TASK WEATHER_TASK;

//-------------------------------------
///	�V�C�P�I�u�W�F�i��̗��ȂǗp�@OAM�j
//=====================================
typedef struct _WEATHER_OBJ_WORK WEATHER_OBJ_WORK;


//-------------------------------------
///	�V�C�Ǘ��֐�
//=====================================
typedef WEATHER_TASK_FUNC_RESULT (WEATHER_TASK_FUNC)( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID );


//-------------------------------------
///	�P�I�u�W�F����֐�
//=====================================
typedef void (WEATHER_OBJ_WORK_FUNC)( WEATHER_OBJ_WORK* p_wk );

//-------------------------------------
///	�V�C�ǉ����
//=====================================
typedef struct {
	// �A�[�NID
	// BG��OAM���p
	u16	arc_id;				
	u16	use_oam;			// OAM���g�p���邩�H TRUE or FALSE
	u16	use_bg;			// BG���g�p���邩�H TRUE or FALSE
	
	// OAM���
	u16 oam_cg;				
	u16 oam_pltt;			// �p���b�g��1�{�ł��B
	u16 oam_cell;
	u16 oam_cellanm;
	
	// BG���
	// �i�Q�̓V�C�𓯎��ɊǗ�����ꍇ�ABG�̔��f�͌㏟���j
	u16 bg_tex;		// BGTEX
	u8 texwidth;	// GXTexSizeS
	u8 texheight;	// GXTexSizeT
	u8 repeat;		// GXTexRepeat
	u8 flip;		// GXTexFlip
	u8 texfmt;		// GXTexFmt
	u8 texpltt;		// GXTexPlttColor0
	
	// ���[�N�̈�T�C�Y
	u32 work_byte;
	
	// �Ǘ��֐�
	WEATHER_TASK_FUNC* p_f_init;			// ����������
	WEATHER_TASK_FUNC* p_f_fadein;			// �t�F�[�h�C������
	WEATHER_TASK_FUNC* p_f_nofade;			// �t�F�[�h�Ȃ��̏ꍇ�̏���
	WEATHER_TASK_FUNC* p_f_main;			// ���C������
	WEATHER_TASK_FUNC* p_f_init_fadeout;	// �t�F�[�h�A�E�g����������
	WEATHER_TASK_FUNC* p_f_fadeout;			// �t�F�[�h�A�E�g����
	WEATHER_TASK_FUNC* p_f_dest;			// �j������

	
	//  ����I�u�W�F����֐�
	WEATHER_OBJ_WORK_FUNC* p_f_objmove;	
	
} WEATHER_TASK_DATA;


//-------------------------------------
///	���`���샏�[�N
//=====================================
typedef struct {
	int x;
	int s_x;
	int d_x;

	u32 count_max;
} WT_MOVE_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *		WEATHER_TASK	�Ǘ��֐�
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	�V�X�e�������E�j���E���C��
//=====================================
extern WEATHER_TASK* WEATHER_TASK_Init( GFL_CLUNIT* p_clunit, const FIELD_CAMERA* cp_camera, FIELD_LIGHT* p_light, FIELD_FOG_WORK* p_fog, const FIELD_ZONEFOGLIGHT* cp_zonefog, FIELD_3DBG* p_3dbg, u32 heapID );
extern void WEATHER_TASK_Exit( WEATHER_TASK* p_wk );
extern void WEATHER_TASK_Main( WEATHER_TASK* p_wk, u32 heapID );

//-------------------------------------
///	�����E��~�Ǘ�
//=====================================
extern void WEATHER_TASK_Start( WEATHER_TASK* p_wk, const WEATHER_TASK_DATA* cp_data, WEATEHR_TASK_INIT_MODE init_mode, BOOL fade, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID );
extern void WEATHER_TASK_End( WEATHER_TASK* p_wk, BOOL fade, WEATHER_TASK_FOG_MODE fog_cont );
extern void WEATHER_TASK_ForceEnd( WEATHER_TASK* p_wk );

//-------------------------------------
///	���C�g����
//	Change	light_id�̃��C�g�ɕύX
//	Back	��{�̃��C�g�ɕύX
//=====================================
extern void WEATHER_TASK_LIGHT_Change( WEATHER_TASK* p_wk, u32 arcid, u32 dataid, u32 heapID );
extern void WEATHER_TASK_LIGHT_Back( WEATHER_TASK* p_wk, u32 heapID );

extern void WEATHER_TASK_LIGHT_StartColorFade( WEATHER_TASK* p_wk, GXRgb color, u16 insync, u16 outsync );
extern BOOL WEATHER_TASK_LIGHT_IsColorFade( const WEATHER_TASK* cp_wk );


//-------------------------------------
///	3DBG����
//=====================================
extern void WEATHER_TASK_3DBG_SetVisible( WEATHER_TASK* p_wk, BOOL visible );
extern BOOL WEATHER_TASK_3DBG_GetVisible( const WEATHER_TASK* cp_wk );
extern void WEATHER_TASK_3DBG_SetAlpha( WEATHER_TASK* p_wk, u8 alpha );
extern u8 WEATHER_TASK_3DBG_GetAlpha( const WEATHER_TASK* cp_wk );

extern void WEATHER_TASK_3DBG_SetScrollX( WEATHER_TASK* p_wk, s32 x );
extern void WEATHER_TASK_3DBG_SetScrollY( WEATHER_TASK* p_wk, s32 y );
extern s32 WEATHER_TASK_3DBG_GetScrollX( const WEATHER_TASK* cp_wk );
extern s32 WEATHER_TASK_3DBG_GetScrollY( const WEATHER_TASK* cp_wk );


//-------------------------------------
///	���̎擾
//=====================================
extern WEATHER_TASK_INFO WEATHER_TASK_GetInfo( const WEATHER_TASK* cp_wk );
extern void* WEATHER_TASK_GetWorkData( const WEATHER_TASK* cp_wk );		//WEATHER_TASK_DATA.work_byte���̃��[�N
extern u32 WEATHER_TASK_GetActiveObjNum( const WEATHER_TASK* cp_wk );


//-------------------------------------
///	����I�u�W�F�̒ǉ��E�j��
//=====================================
extern WEATHER_OBJ_WORK* WEATHER_TASK_CreateObj( WEATHER_TASK* p_wk, u32 heapID );
extern void WEATHER_TASK_DeleteObj( WEATHER_OBJ_WORK* p_obj );




//-------------------------------------
///	����I�u�W�F�̒ǉ��t�F�[�h����
//=====================================
typedef void (WEATHER_TASK_OBJADD_FUNC)( WEATHER_TASK* p_wk, int num, u32 heapID );
extern void WEATHER_TASK_ObjFade_Init( WEATHER_TASK* p_wk, s32 objAddNum, s32 objAddTmgMax, s32 obj_add_num_end, s32 obj_add_tmg_end, s32 obj_add_tmg_sum, s32 obj_add_num_sum_tmg, s32 obj_add_num_sum, WEATHER_TASK_OBJADD_FUNC* p_addfunc );
extern void WEATHER_TASK_ObjFade_SetOut( WEATHER_TASK* p_wk, s32 obj_add_num_end, s32 obj_add_tmg_end, s32 obj_add_tmg_sum, s32 obj_add_num_sum );
extern BOOL WEATHER_TASK_ObjFade_Main( WEATHER_TASK* p_wk, u32 heapID );

// �t�F�[�h�͂����A�I�u�W�F�̓����o�^�^�C�~���O�ŁA�������@�I�u�W�F��o�^���܂��B
extern BOOL WEATHER_TASK_ObjFade_NoFadeMain( WEATHER_TASK* p_wk, u32 heapID );

//-------------------------------------
///	����I�u�W�F�𗐐��ŕ��z������
//=====================================
extern void WEATHER_TASK_DustObj( WEATHER_TASK* p_wk, WEATHER_TASK_OBJADD_FUNC* p_add_func, int num, int dust_div_num, int dust_div_move, u32 heapID );

//-------------------------------------
///	�t�H�O�t�F�[�h����
//=====================================
extern void WEATHER_TASK_FogSet( WEATHER_TASK* p_wk, FIELD_FOG_SLOPE fog_slope, int fog_offs, WEATHER_TASK_FOG_MODE mode );
extern void WEATHER_TASK_FogClear( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE mode );
extern void WEATHER_TASK_FogFadeIn_Init( WEATHER_TASK* p_wk, FIELD_FOG_SLOPE fog_slope, int fog_offs, int timing, WEATHER_TASK_FOG_MODE mode );
extern void WEATHER_TASK_FogFadeOut_Init( WEATHER_TASK* p_wk, int fog_offs, int timing, WEATHER_TASK_FOG_MODE mode );
extern BOOL WEATHER_TASK_FogFade_IsFade( const WEATHER_TASK* cp_wk );


//-------------------------------------
///	�]�[���p�ݒ�̎擾
//=====================================
extern BOOL WEATHER_TASK_IsZoneFog( const WEATHER_TASK* cp_wk );
extern BOOL WEATHER_TASK_IsZoneLight( const WEATHER_TASK* cp_wk );
extern s32 WEATHER_TASK_GetZoneFogOffset( const WEATHER_TASK* cp_wk );
extern u32 WEATHER_TASK_GetZoneFogSlope( const WEATHER_TASK* cp_wk );
extern u32 WEATHER_TASK_GetZoneLight( const WEATHER_TASK* cp_wk );

//-------------------------------------
///	�t�H�O�u�����h����
//=====================================
extern void WEATHER_TASK_Fog_SetBlendMode( WEATHER_TASK* p_wk, FIELD_FOG_BLEND blend );
extern FIELD_FOG_BLEND WEATHER_TASK_Fog_GetBlendMode( const WEATHER_TASK* cp_wk );
extern void WEATHER_TASK_Fog_SetAlpha( WEATHER_TASK* p_wk, u8 alpha );
extern u8 WEATHER_TASK_Fog_GetAlpha( const WEATHER_TASK* cp_wk );



//-------------------------------------
///�@�X�N���[���Ǘ�
//=====================================
extern void WEATHER_TASK_InitScrollDist( WEATHER_TASK* p_wk );
extern void WEATHER_TASK_GetScrollDist( WEATHER_TASK* p_wk, int* p_x, int* p_y );
extern void WEATHER_TASK_ScrollObj( WEATHER_TASK* p_wk, int x, int y );
extern void WEATHER_TASK_ScrollBg( WEATHER_TASK* p_wk, int x, int y );

//-------------------------------------
///	�T�E���h�Ǘ�
//=====================================
extern void WEATHER_TASK_PlayLoopSnd( WEATHER_TASK* p_wk, int snd_no );
extern void WEATHER_TASK_StopLoopSnd( WEATHER_TASK* p_wk );




//-----------------------------------------------------------------------------
/**
 *		WEATHER_OBJ_WORK	�Ǘ��֐�
 *
 *		�V�C�I�u�W�F�̎����Ă��郏�[�N�T�C�Y�͌Œ�ł��B
 *		WEATHER_OBJ_WORK_USE_WORKSIZE
 */
//-----------------------------------------------------------------------------
extern void* WEATHER_OBJ_WORK_GetWork( const WEATHER_OBJ_WORK* cp_wk );
extern GFL_CLWK* WEATHER_OBJ_WORK_GetClWk( const WEATHER_OBJ_WORK* cp_wk );
extern const WEATHER_TASK* WEATHER_OBJ_WORK_GetParent( const WEATHER_OBJ_WORK* cp_wk );
extern void WEATHER_OBJ_WORK_GetPos( const WEATHER_OBJ_WORK* cp_wk, GFL_CLACTPOS* p_pos );
extern void WEATHER_OBJ_WORK_SetPos( WEATHER_OBJ_WORK* p_wk, const GFL_CLACTPOS* cp_pos );
extern void WEATHER_OBJ_WORK_SetPosNoTurn( WEATHER_OBJ_WORK* p_wk, const GFL_CLACTPOS* cp_pos );


//-----------------------------------------------------------------------------
/**
 *		����v�Z�c�[��
 */
//-----------------------------------------------------------------------------
extern void WT_MOVE_WORK_Init( WT_MOVE_WORK* p_wk, int s_x, int e_x, u32 count_max );
extern BOOL	WT_MOVE_WORK_Main( WT_MOVE_WORK* p_wk, u32 count );



#endif		// __WEATHER_TASK_H__

#ifdef _cplusplus
}/* extern "C" */
#endif

