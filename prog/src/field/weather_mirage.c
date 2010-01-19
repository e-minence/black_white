//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		weather_mirage.c
 *	@brief		凋C�O
 *	@author		tomoya takahshi
 *	@date		2009.04.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"
#include "calctool.h"

#include "arc/arc_def.h"
#include "arc/field_weather.naix"

#include "system/laster.h"

#include "weather_mirage.h"

//-----------------------------------------------------------------------------
/**
 *					�R�[�f�B���O�K��
 *		���֐���
 *				�P�����ڂ͑啶������ȍ~�͏������ɂ���
 *		���ϐ���
 *				�E�ϐ�����
 *						const�ɂ� c_ ��t����
 *						static�ɂ� s_ ��t����
 *						�|�C���^�ɂ� p_ ��t����
 *						�S�č��킳��� csp_ �ƂȂ�
 *				�E�O���[�o���ϐ�
 *						�P�����ڂ͑啶��
 *				�E�֐����ϐ�
 *						�������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	凋C�OBG��
//=====================================
#define WEATRHER_MIRAGE_BG_FRAME	( GFL_BG_FRAME2_M )
#define WEATRHER_MIRAGE_BG_PLTT		( 0x5 )


//-------------------------------------
///	FOG
//=====================================
#define	WEATHER_MIRAGE_FOG_TIMING		(300)							// �ɂP��t�H�O�e�[�u���𑀍�
#define	WEATHER_MIRAGE_FOG_TIMING_END	(100)							// �ɂP��t�H�O�e�[�u���𑀍�
#define WEATHER_MIRAGE_FOG_START		(1)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_MIRAGE_FOG_START_END	(1)						// ���̃J�E���g�����Ă���t�H�O�e�[�u���𑀍�
#define WEATHER_MIRAGE_FOG_OFS			(0xf00)
#define WEATHER_MIRAGE_FOG_OFS_START	(0x1000)

#define WEATHER_MIRAGE_FOG_ALPHA		( 0 )
#define WEATHER_MIRAGE_FOG_SLOPE		( 10 )

//-------------------------------------
///	凋C�O���X�^�[
//=====================================
#define WEATHER_MIRAGE_LASTER_ADDR	( GFL_CALC_GET_ROTA_NUM(8) )
#define WEATHER_MIRAGE_LASTER_R_W	( 1.5*FX32_ONE )
#define WEATHER_MIRAGE_LASTER_SCRL	( 1 )
#define WEATHER_MIRAGE_LASTER_BG	( LASTER_SCROLL_MBG2 )
#define WEATHER_MIRAGE_LASTER_INIT	( 0 )


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	凋C�O���[�N
//=====================================
typedef struct {
	BOOL fog_init;

	GFL_TCB*			p_vblank;
	GFL_TCB*			p_hblank;

	s16 scroll[192];
	s16 scroll_index;
	
} WEATHER_MIRAGE_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//-------------------------------------
/// 凋C�O	
//=====================================
static WEATHER_TASK_FUNC_RESULT WEATHER_MIRAGE_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_MIRAGE_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_MIRAGE_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_MIRAGE_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_MIRAGE_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_MIRAGE_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 
static WEATHER_TASK_FUNC_RESULT WEATHER_MIRAGE_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID ); 

static void WEATHER_MIRAGE_VBlank( GFL_TCB* p_tcb, void* p_work );
static void WEATHER_MIRAGE_HBlank( GFL_TCB* p_tcb, void* p_work );





//-----------------------------------------------------------------------------
/**
 *			�V�C�f�[�^
 */
//-----------------------------------------------------------------------------
// 凋C�O
WEATHER_TASK_DATA c_WEATHER_TASK_DATA_MIRAGE = {
	//	�O���t�B�b�N���
	ARCID_FIELD_WEATHER,			// �A�[�NID
	FALSE,		// OAM���g�p���邩�H
	WEATHER_TASK_3DBG_USE_NONE,		// BG���g�p���邩�H
	0,			// OAM CG
	0,			// OAM PLTT
	0,			// OAM CELL
	0,			// OAM CELLANM
  {
    {
      0,		// BGTEX
      0,		// GXTexSizeS
      0,		// GXTexSizeT
      0,		// GXTexRepeat
      0,		// GXTexFlip
      0,		// GXTexFmt
      0,		// GXTexPlttColor0
    },
    {
      0,		// BGTEX
      0,		// GXTexSizeS
      0,		// GXTexSizeT
      0,		// GXTexRepeat
      0,		// GXTexFlip
      0,		// GXTexFmt
      0,		// GXTexPlttColor0
    },
  },

	// ���[�N�T�C�Y
	sizeof(WEATHER_MIRAGE_WORK),

	// �Ǘ��֐�
	WEATHER_MIRAGE_Init,		// ������
	WEATHER_MIRAGE_FadeIn,		// �t�F�[�h�C��
	WEATHER_MIRAGE_NoFade,		// �t�F�[�h�Ȃ�
	WEATHER_MIRAGE_Main,		// ���C������
	WEATHER_MIRAGE_InitFadeOut,	// �t�F�[�h�A�E�g
	WEATHER_MIRAGE_FadeOut,		// �t�F�[�h�A�E�g
	WEATHER_MIRAGE_Exit,		// �j��

	// �I�u�W�F����֐�
	NULL,
};







//-----------------------------------------------------------------------------
/**
 *			�v���C�x�[�g�֐��S
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief		凋C�O������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_MIRAGE_Init( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_MIRAGE_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );


	// BG�ʂ̐���
	{
		static const GFL_BG_BGCNT_HEADER header_main2 = {
			0, 0, 0x800, 0,	// scrX, scrY, scrbufSize, scrbufofs,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x08000,0x8000,
			GX_BG_EXTPLTT_01, 3, 1, 0, FALSE	// pal, pri, areaover, dmy, mosaic
		};

		GFL_BG_FreeBGControl( WEATRHER_MIRAGE_BG_FRAME );
		GFL_BG_SetBGControl( WEATRHER_MIRAGE_BG_FRAME, &header_main2, GFL_BG_MODE_TEXT );
		GFL_BG_ClearFrame( WEATRHER_MIRAGE_BG_FRAME );
		GFL_BG_SetVisible( WEATRHER_MIRAGE_BG_FRAME, VISIBLE_ON );
	}

	// �O���t�B�b�N�W�J
	{
		ARCHANDLE* p_handle;

		p_handle = GFL_ARC_OpenDataHandle( ARCID_FIELD_WEATHER, heapID );
		
		// �p���b�g����]��
		GFL_ARCHDL_UTIL_TransVramPalette(
			p_handle, NARC_field_weather_mirage_NCLR,
			PALTYPE_MAIN_BG, WEATRHER_MIRAGE_BG_PLTT*32, 32, heapID );

		// �L�����N�^���]��
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_field_weather_mirage_NCGR, WEATRHER_MIRAGE_BG_FRAME, 0, 0x4000, FALSE, heapID );

		// �X�N���[�����]��
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_field_weather_mirage_NSCR, WEATRHER_MIRAGE_BG_FRAME, 0, 0, FALSE, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}
	
	// ���X�^�[�V�X�e������
	{
		LASTER_ScrollMakeSinTbl( p_local_wk->scroll, 192, WEATHER_MIRAGE_LASTER_ADDR, WEATHER_MIRAGE_LASTER_R_W );
		
		p_local_wk->p_hblank = GFUser_HIntr_CreateTCB( WEATHER_MIRAGE_HBlank, p_local_wk, 0 );
		p_local_wk->p_vblank = GFUser_VIntr_CreateTCB( WEATHER_MIRAGE_VBlank, p_local_wk, 10 );
	}

	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT + WEATHER_MIRAGE_FOG_OFS_START, fog_cont );

	//
	WEATHER_TASK_Fog_SetBlendMode( p_wk, FIELD_FOG_BLEND_ALPHA );
	WEATHER_TASK_Fog_SetAlpha( p_wk, WEATHER_MIRAGE_FOG_ALPHA );

	G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG0, GX_BLEND_PLANEMASK_BG2, 0, 31 );

	p_local_wk->fog_init = FALSE;

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief		凋C�O�@�t�F�[�h�C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_MIRAGE_FadeIn( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_MIRAGE_WORK* p_local_wk;
	BOOL result;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	if( p_local_wk->fog_init == FALSE ){
		WEATHER_TASK_FogFadeIn_Init( p_wk,
				WEATHER_FOG_SLOPE_DEFAULT, 
				WEATHER_FOG_DEPTH_DEFAULT + WEATHER_MIRAGE_FOG_OFS, 
				WEATHER_MIRAGE_FOG_TIMING, fog_cont );
		p_local_wk->fog_init = TRUE;
	}

	result = WEATHER_TASK_FogFade_IsFade( p_wk );

	// �^�C�~���O���ŏ��ɂȂ����烁�C����
	if( result ){		// �t�F�[�h���U���g�������Ȃ�΃��C����
		return WEATHER_TASK_FUNC_RESULT_FINISH;
	}

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief		凋C�O�@�t�F�[�h�Ȃ�
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_MIRAGE_NoFade( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	// �t�H�O�̐ݒ�
	WEATHER_TASK_FogSet( p_wk, WEATHER_FOG_SLOPE_DEFAULT, WEATHER_FOG_DEPTH_DEFAULT + WEATHER_MIRAGE_FOG_OFS, fog_cont );


	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief		凋C�O�@���C��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_MIRAGE_Main( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_MIRAGE_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );


	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief		凋C�O�@�t�F�[�h�A�E�g������
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_MIRAGE_InitFadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_MIRAGE_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	WEATHER_TASK_FogFadeOut_Init( p_wk,
			WEATHER_FOG_DEPTH_DEFAULT + WEATHER_MIRAGE_FOG_OFS_START, 
			WEATHER_MIRAGE_FOG_TIMING_END, fog_cont );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief		凋C�O�@�t�F�[�h�A�E�g
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_MIRAGE_FadeOut( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_MIRAGE_WORK* p_local_wk;
	BOOL result;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	result = WEATHER_TASK_FogFade_IsFade( p_wk );

	// �^�C�~���O���ŏ��ɂȂ����烁�C����
	if( result ){		// �t�F�[�h���U���g�������Ȃ�΃��C����
		return WEATHER_TASK_FUNC_RESULT_FINISH;
	}

	return WEATHER_TASK_FUNC_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief		凋C�O�@�j��
 */
//-----------------------------------------------------------------------------
static WEATHER_TASK_FUNC_RESULT WEATHER_MIRAGE_Exit( WEATHER_TASK* p_wk, WEATHER_TASK_FOG_MODE fog_cont, u32 heapID )
{
	WEATHER_MIRAGE_WORK* p_local_wk;

	// ���[�J�����[�N�擾
	p_local_wk = WEATHER_TASK_GetWorkData( p_wk );

	// ���X�^�[�V�X�e���j��
	{
		GFL_TCB_DeleteTask( p_local_wk->p_hblank );
		GFL_TCB_DeleteTask( p_local_wk->p_vblank );
	}

	GFL_BG_FreeBGControl( WEATRHER_MIRAGE_BG_FRAME );

	GFL_BG_SetVisible( WEATRHER_MIRAGE_BG_FRAME, VISIBLE_OFF );

	// FOG�I��
	WEATHER_TASK_FogClear( p_wk, fog_cont );

	return WEATHER_TASK_FUNC_RESULT_FINISH;
}


//----------------------------------------------------------------------------
/**
 *	@brief	凋C�O�@VBLANK
 */
//-----------------------------------------------------------------------------
static void WEATHER_MIRAGE_VBlank( GFL_TCB* p_tcb, void* p_work )
{
	WEATHER_MIRAGE_WORK* p_wk;
	p_wk = p_work;

	p_wk->scroll_index += WEATHER_MIRAGE_LASTER_SCRL;
	if( p_wk->scroll_index < 0 ){
		p_wk->scroll_index += 192;
	}else{
		p_wk->scroll_index %= 192;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	凋C�O�@HBLANK
 */
//-----------------------------------------------------------------------------
static void WEATHER_MIRAGE_HBlank( GFL_TCB* p_tcb, void* p_work )
{
	WEATHER_MIRAGE_WORK* p_wk;
	int v_c;
	p_wk = p_work;

	v_c = GX_GetVCount();

	v_c += (p_wk->scroll_index + 1);
	v_c %= 192;

	if( GX_IsHBlank() ){
		G2_SetBG2Offset( p_wk->scroll[v_c], 0 );
	}
}


