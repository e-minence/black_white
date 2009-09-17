//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		d_nagi.c
 *	@brief	���؋��p�f�o�b�O
 *	@author	Toru=Nagihashi
 *	@data		2009.05.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

//lib
#include <gflib.h>
#include <wchar.h>	//wcslen

//contant
#include "system/main.h"	//HEAPID
#include "system/gfl_use.h"

//module
#include "system/bmp_menulist.h"

//archive
#include "arc_def.h"
#include "font/font.naix"

//proc
#include "net_app/irc_aura.h"
#include "net_app/irc_rhythm.h"
#include "net_app/irc_result.h"
#include "net_app/irc_compatible.h"
#include "net_app/compatible_irc_sys.h"
#include "debug_irc_name.h"
#include "net_app/irc_ranking.h"
#include "app/townmap.h"
#include "net_app/worldtrade.h"
#include "app/config_panel.h"

#include "savedata/irc_compatible_savedata.h"


//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	���X�g�̍ő啶����
//=====================================
#define LISTDATA_STR_LENGTH	(32)


//-------------------------------------
///	�G�N�X�^�[��
//=====================================
FS_EXTERN_OVERLAY( compatible_irc_sys );

//-------------------------------------
///	��
//=====================================
#define CIRCLE_MAX		(10)

//-------------------------------------
///	
//=====================================
typedef enum 
{
	PROC_TYPE_NONE,
	PROC_TYPE_CALL,
	PROC_TYPE_NEXT,
} PROC_TYPE;


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	BG�֌W
//=====================================
typedef struct
{
	int dummy;
} GRAPHIC_BG_WORK;
//-------------------------------------
///	3D�`���
//=====================================
typedef struct {
	GFL_G3D_CAMERA		*p_camera;
} GRAPHIC_3D_WORK;
//-------------------------------------
///	�~
//=====================================
enum {	
	CIRCLE_VTX_MAX	=	60,
};
typedef struct 
{
	VecFx16	vtx[CIRCLE_VTX_MAX];
	u16 r;				//���a	
	u16 red:5;		//��
	u16	green:5;	//��
	u16 blue:5;		//��
	u16	dummy1:1;

	u16 add_r;
	u16 add_red:5;
	u16 add_green:5;
	u16 add_blue:5;
	u16 dummy2:1;
} CIRCLE_WORK;
//-------------------------------------
///	�`��֌W
//=====================================
typedef struct 
{
	GRAPHIC_BG_WORK		gbg;
	GRAPHIC_3D_WORK		g3d;
	CIRCLE_WORK				c[CIRCLE_MAX];
	BOOL							is_init;

	GFL_TCB *p_tcb;
} GRAPHIC_WORK;
//-------------------------------------
///	���X�g
//=====================================
typedef struct 
{
	BMPMENULIST_WORK	*p_list;
	BMP_MENULIST_DATA *p_list_data;
	u32		select;
} LIST_WORK;
//-------------------------------------
///	���b�Z�[�W
//=====================================
typedef struct 
{
	GFL_FONT*				  p_font;
  PRINT_QUE*        p_print_que;
  PRINT_UTIL        print_util;
} MSG_WORK;

//-------------------------------------
///	���C�����[�N
//=====================================
typedef struct
{
	GRAPHIC_WORK			grp;
	MSG_WORK					msg;
	LIST_WORK					list;
	GFL_BMPWIN				*p_bmpwin;
	BOOL	is_temp_modules;

	BOOL	is_end;
	PROC_TYPE	proc_type;

	//Proc�؂�ւ��p
	FSOverlayID overlay_Id;
	const GFL_PROC_DATA *p_procdata;
	void	*p_proc_work;

	IRC_RESULT_PARAM			result_param;
	IRC_COMPATIBLE_PARAM	compatible_param;
	IRC_AURA_PARAM				aura_param;
	IRC_RHYTHM_PARAM			rhythm_param;
	TOWNMAP_PARAM					townmap_param;
	WORLDTRADE_PARAM			gts_param;
} DEBUG_NAGI_MAIN_WORK;

//-------------------------------------
///	���X�g�ݒ�e�[�u��
//=====================================
typedef struct 
{	
	u16	str[LISTDATA_STR_LENGTH];
	u32	param;
} LIST_SETUP_TBL;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//proc
static GFL_PROC_RESULT DEBUG_PROC_NAGI_Init( GFL_PROC *p_proc, int *p_seq, void *p_parent, void *p_work );
static GFL_PROC_RESULT DEBUG_PROC_NAGI_Exit( GFL_PROC *p_proc, int *p_seq, void *p_parent, void *p_work );
static GFL_PROC_RESULT DEBUG_PROC_NAGI_Main( GFL_PROC *p_proc, int *p_seq, void *p_parent, void *p_work );
//�ėp
static void DEBUG_NAGI_COMMAND_CallProc( DEBUG_NAGI_MAIN_WORK * p_wk, FSOverlayID ov_id, const GFL_PROC_DATA *p_procdata, void *p_work );
static void DEBUG_NAGI_COMMAND_NextProc( DEBUG_NAGI_MAIN_WORK * p_wk, FSOverlayID ov_id, const GFL_PROC_DATA *p_procdata, void *p_work );
static void DEBUG_NAGI_COMMAND_ChangeMenu( DEBUG_NAGI_MAIN_WORK * p_wk, const LIST_SETUP_TBL *cp_tbl, u32 tbl_max );
static void DEBUG_NAGI_COMMAND_End( DEBUG_NAGI_MAIN_WORK *p_wk );

static void CreateTemporaryModules( DEBUG_NAGI_MAIN_WORK *p_wk, HEAPID heapID );
static void DeleteTemporaryModules( DEBUG_NAGI_MAIN_WORK *p_wk );
static void MainTemporaryModules( DEBUG_NAGI_MAIN_WORK *p_wk );

//grp
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk );
static void GRAPHIC_Draw( GRAPHIC_WORK *p_wk );
static void Graphic_Tcb_Capture( GFL_TCB *p_tcb, void *p_work );

//BG
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
//LIST
static void LIST_Init( LIST_WORK *p_wk, const LIST_SETUP_TBL *cp_tbl, u32 tbl_max, MSG_WORK *p_msg, GFL_BMPWIN*	p_bmpwin, HEAPID heapID );
static void LIST_Exit( LIST_WORK *p_wk );
static void LIST_Main( LIST_WORK *p_wk );
static BOOL LIST_IsDecide( const LIST_WORK *cp_wk, u32 *p_select );

//MSG_WORK
static void MSG_Init( MSG_WORK *p_wk, HEAPID heapID );
static void MSG_Exit( MSG_WORK *p_wk );
static BOOL MSG_Main( MSG_WORK *p_wk );
static PRINT_UTIL * MSG_GetPrintUtil( MSG_WORK *p_wk, GFL_BMPWIN*	p_bmpwin );
static GFL_FONT*	MSG_GetFont( const MSG_WORK *cp_wk );
static PRINT_QUE* MSG_GetPrintQue( const MSG_WORK *cp_wk );
//LISTFUNC
typedef void (*LISTDATA_FUNCTION)( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_CallProcAura( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_CallProcRhythm( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_CallProcResult( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_CallProcCompatible( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_CallProcCompatibleDebug( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_CallProcNameDebug( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_CallProcRankingDebug( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_CallProcGts( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_CallProcConfig( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_AddRankData( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_FullRankData( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_CallProcTownMap( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_CallProcSkyJump( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_Return( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_NextListHome( DEBUG_NAGI_MAIN_WORK *p_wk );
static void LISTDATA_NextListPage1( DEBUG_NAGI_MAIN_WORK *p_wk );
//3d
static void GRAPHIC_3D_Init( GRAPHIC_3D_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_3D_Exit( GRAPHIC_3D_WORK *p_wk );
static void GRAPHIC_3D_StartDraw( GRAPHIC_3D_WORK *p_wk );
static void GRAPHIC_3D_EndDraw( GRAPHIC_3D_WORK *p_wk );
static void Graphic_3d_SetUp( void );
//�~�v���~�e�B�u
static void CIRCLE_Init( CIRCLE_WORK *p_wk, u16 r, GXRgb color );
static void CIRCLE_Draw( CIRCLE_WORK *p_wk );
static void CIRCLE_SetAddR( CIRCLE_WORK *p_wk, u16 add_r );
static void CIRCLE_SetAddColor( CIRCLE_WORK *p_wk, u16 add_color );
static void Circle_DrawLine( VecFx16 *p_start, VecFx16 *p_end );

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	�O���Q��PROC�f�[�^
//=====================================
const GFL_PROC_DATA	DebugNagiMainProcData	= 
{
	DEBUG_PROC_NAGI_Init,
	DEBUG_PROC_NAGI_Main,
	DEBUG_PROC_NAGI_Exit,
};

//-------------------------------------
///	BG�V�X�e��
//=====================================
typedef enum 
{
	GRAPHIC_BG_FRAME_TEXT,
	GRAPHIC_BG_FRAME_MAX
} GRAPHIC_BG_FRAME;
static const u32 sc_bgcnt_frame[ GRAPHIC_BG_FRAME_MAX ] = 
{
	GFL_BG_FRAME0_S,
};
static const GFL_BG_BGCNT_HEADER sc_bgcnt_data[ GRAPHIC_BG_FRAME_MAX ] = 
{
	// GRAPHIC_BG_FRAME_TEXT
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	},
};

//-------------------------------------
///	���X�g�Ŏ��s�������
//=====================================
enum
{	
	LISTDATA_SEQ_PROC_AURA_DEBUG,
	LISTDATA_SEQ_PROC_RHYTHM_DEBUG,
	LISTDATA_SEQ_PROC_RESULT,
	LISTDATA_SEQ_PROC_COMPATIBLE,
	LISTDATA_SEQ_PROC_COMPATIBLE_DEBUG,
	LISTDATA_SEQ_RETURN,
	LISTDATA_SEQ_PROC_NAME_DEBUG,
	LISTDATA_SEQ_PROC_RANKING_DEBUG,
	LISTDATA_SEQ_RANKDATA_ONE,
	LISTDATA_SEQ_RANKDATA_FULL,
	LISTDATA_SEQ_PROC_TOWNMAP,
	LISTDATA_SEQ_PROC_SKYJUMP,
	LISTDATA_SEQ_NEXT_HOME,
	LISTDATA_SEQ_NEXT_PAGE1,
	LISTDATA_SEQ_PROC_GTS,
	LISTDATA_SEQ_PROC_CONFIG,

	LISTDATA_SEQ_MAX,
};
static const LISTDATA_FUNCTION	sc_list_funciton[]	= 
{	
	LISTDATA_CallProcAura,
	LISTDATA_CallProcRhythm,
	LISTDATA_CallProcResult,
	LISTDATA_CallProcCompatible,
	LISTDATA_CallProcCompatibleDebug,
	LISTDATA_Return,
	LISTDATA_CallProcNameDebug,
	LISTDATA_CallProcRankingDebug,
	LISTDATA_AddRankData,
	LISTDATA_FullRankData,
	LISTDATA_CallProcTownMap,
	LISTDATA_CallProcSkyJump,
	LISTDATA_NextListHome,
	LISTDATA_NextListPage1,
	LISTDATA_CallProcGts,
	LISTDATA_CallProcConfig,
};

//-------------------------------------
///	���X�g�f�[�^
//=====================================
static const LIST_SETUP_TBL sc_list_data_home[]	=
{	
	{	
		L"�R���t�B�O", LISTDATA_SEQ_PROC_CONFIG
	},
	{	
		L"�����f�f��ʂ�", LISTDATA_SEQ_PROC_COMPATIBLE
	},
	{	
		L"GTS", LISTDATA_SEQ_PROC_GTS
	},
	{	
		L"����", LISTDATA_SEQ_PROC_RESULT,
	},

#if 0
	{	
		L"�����f�f�i�ЂƂ�j", LISTDATA_SEQ_PROC_COMPATIBLE_DEBUG
	},
	{	
		L"�I�[���i�ЂƂ�j", LISTDATA_SEQ_PROC_AURA_DEBUG
	},
	{	
		L"���Y���i�ЂƂ�j", LISTDATA_SEQ_PROC_RHYTHM_DEBUG
	},
	{	
		L"�^���l�`�F�b�N", LISTDATA_SEQ_PROC_NAME_DEBUG,
	},
#endif
#if 1
	{	
		L"�����L���O", LISTDATA_SEQ_PROC_RANKING_DEBUG,
	},
	{	
		L"�����N�f�[�^�P",	LISTDATA_SEQ_RANKDATA_ONE,
	},
	{	
		L"�����N�f�[�^�t��",	LISTDATA_SEQ_RANKDATA_FULL,
	},
#endif
#if 1
	{	
		L"�^�E���}�b�v", LISTDATA_SEQ_PROC_TOWNMAP,
	},
	{	
		L"������Ƃ�", LISTDATA_SEQ_PROC_SKYJUMP,
	},
	{	
		L"���ǂ�", LISTDATA_SEQ_RETURN
	},
#endif
};

static const LIST_SETUP_TBL sc_list_data_page1[]	=
{	
	{	
		L"�O��", LISTDATA_SEQ_NEXT_HOME
	}
};

//----------------------------------------------------------------------------
/**
 *	@brief	���C���v���b�N	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_parent					�e���[�N
 *	@param	*p_work						�����̃��[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_PROC_NAGI_Init( GFL_PROC *p_proc, int *p_seq, void *p_parent, void *p_work )
{	
	DEBUG_NAGI_MAIN_WORK	*p_wk;
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_NAGI_DEBUG, 0x20000 );
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(DEBUG_NAGI_MAIN_WORK), HEAPID_NAGI_DEBUG );
	GFL_STD_MemClear( p_wk, sizeof(DEBUG_NAGI_MAIN_WORK) );

	CreateTemporaryModules( p_wk, HEAPID_NAGI_DEBUG );

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C���v���b�N	�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_parent					�e���[�N
 *	@param	*p_work						�����̃��[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_PROC_NAGI_Exit( GFL_PROC *p_proc, int *p_seq, void *p_parent, void *p_work )
{	
	DEBUG_NAGI_MAIN_WORK	*p_wk;

	p_wk	= p_work;

	if( p_wk->proc_type == PROC_TYPE_NEXT )
	{	
		//����PROC�\��
		GFL_PROC_SysSetNextProc(
				p_wk->overlay_Id, p_wk->p_procdata, p_wk->p_proc_work );
	}

	DeleteTemporaryModules( p_wk );

	GFL_PROC_FreeWork( p_proc );
	GFL_HEAP_DeleteHeap( HEAPID_NAGI_DEBUG );

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���C���v���b�N	���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_parent					�e���[�N
 *	@param	*p_work						�����̃��[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_PROC_NAGI_Main( GFL_PROC *p_proc, int *p_seq, void *p_parent, void *p_work )
{	
	enum
	{	
		SEQ_INIT,
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
		SEQ_MAIN,
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_EXIT,

		SEQ_PROC_FADEIN_START,
		SEQ_PROC_FADEIN_WAIT,
		SEQ_CALL_PROC,
		SEQ_RET_PROC,
	};

	DEBUG_NAGI_MAIN_WORK	*p_wk;

	p_wk	= p_work;


	switch( *p_seq )
	{	
	case SEQ_INIT:
//		*p_seq	= SEQ_FADEOUT_START;
			*p_seq	= SEQ_MAIN;
		break;

	case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_MAIN;
		}
		break;

	case SEQ_MAIN:
		{
			u32 select;
			LIST_Main( &p_wk->list );
			if( LIST_IsDecide( &p_wk->list, &select ) )
			{	
				sc_list_funciton[ select ]( p_wk );
			}

			//�I������
			if( p_wk->is_end )
			{	
				*p_seq	= SEQ_FADEIN_START;
			}

			//PROC����
			switch( p_wk->proc_type )
			{	
			case PROC_TYPE_CALL:
				p_wk->proc_type	= PROC_TYPE_NONE;
				*p_seq	= SEQ_PROC_FADEIN_START;
				break;
			case PROC_TYPE_NEXT:
				*p_seq	= SEQ_FADEIN_START;
				break;
			}
		}
		break;

	case SEQ_PROC_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
		*p_seq	= SEQ_PROC_FADEIN_WAIT;
		break;

	case SEQ_PROC_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_CALL_PROC;
		}
		break;

	case SEQ_CALL_PROC:
		DeleteTemporaryModules( p_wk );
		GFL_PROC_SysCallProc( p_wk->overlay_Id, p_wk->p_procdata, p_wk->p_proc_work );
		*p_seq	= SEQ_RET_PROC;
		break;

	case SEQ_RET_PROC:
		CreateTemporaryModules( p_wk, HEAPID_NAGI_DEBUG );
		*p_seq	= SEQ_FADEOUT_START;
		break;

	case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
		*p_seq	= SEQ_FADEIN_WAIT;
		break;

	case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_EXIT;
		}
		break;
	
	case SEQ_EXIT:
		return GFL_PROC_RES_FINISH;

	default:
		GF_ASSERT_MSG( 0, "DEBUG_PROC_NAGI_Main��SEQ�G���[ %d", *p_seq );
	}

	MainTemporaryModules( p_wk );

	return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *		����p
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�v���Z�X�؂�ւ�
 *
 *	@param	DEBUG_NAGI_MAIN_WORK * p_wk	���[�N
 *	@param	ov_id												�I�[�o�[���CID
 *	@param	GFL_PROC_DATA *p_procdata		�v���Z�X�f�[�^
 *	@param	*p_work											�n�����
 *
 */
//-----------------------------------------------------------------------------
static void DEBUG_NAGI_COMMAND_CallProc( DEBUG_NAGI_MAIN_WORK * p_wk, FSOverlayID ov_id, const GFL_PROC_DATA *p_procdata, void *p_work )
{	
	p_wk->overlay_Id	= ov_id;
	p_wk->p_procdata	= p_procdata;
	p_wk->p_proc_work	= p_work;
//	p_wk->is_end	= TRUE;

	p_wk->proc_type	= PROC_TYPE_CALL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���Z�X�\��
 *
 *	@param	DEBUG_NAGI_MAIN_WORK * p_wk		���[�N
 *	@param	ov_id											�I�[�o�[���CID
 *	@param	GFL_PROC_DATA *p_procdata	�v���Z�X�f�[�^
 *	@param	*p_work										�n�����
 */
//-----------------------------------------------------------------------------
static void DEBUG_NAGI_COMMAND_NextProc( DEBUG_NAGI_MAIN_WORK * p_wk, FSOverlayID ov_id, const GFL_PROC_DATA *p_procdata, void *p_work )
{	
	p_wk->overlay_Id	= ov_id;
	p_wk->p_procdata	= p_procdata;
	p_wk->p_proc_work	= p_work;
//	p_wk->is_end	= TRUE;

	p_wk->proc_type	= PROC_TYPE_NEXT;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[��؂�ւ�
 *
 *	@param	DEBUG_NAGI_MAIN_WORK * p_wk	���[�N
 *	@param	LIST_SETUP_TBL *cp_tbl			���X�g�p�e�[�u��
 *	@param	tbl_max											���X�g�p�e�[�u���̗v�f��
 *
 */
//-----------------------------------------------------------------------------
static void DEBUG_NAGI_COMMAND_ChangeMenu( DEBUG_NAGI_MAIN_WORK * p_wk, const LIST_SETUP_TBL *cp_tbl, u32 tbl_max )
{
	LIST_Exit( &p_wk->list );
	LIST_Init( &p_wk->list, cp_tbl, tbl_max, &p_wk->msg, p_wk->p_bmpwin, HEAPID_NAGI_DEBUG );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I��
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void DEBUG_NAGI_COMMAND_End( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	p_wk->is_end	= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ꎞ�I�ȃ��W���[�����쐬
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 *	@param	heapID											�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void CreateTemporaryModules( DEBUG_NAGI_MAIN_WORK *p_wk, HEAPID heapID )
{	
	GRAPHIC_Init( &p_wk->grp, heapID );

	MSG_Init( &p_wk->msg, heapID );

	p_wk->p_bmpwin	= GFL_BMPWIN_Create( sc_bgcnt_frame[GRAPHIC_BG_FRAME_TEXT],
			1, 1, 30, 10, 0, GFL_BMP_CHRAREA_GET_B );
	GFL_BMPWIN_MakeScreen( p_wk->p_bmpwin );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(p_wk->p_bmpwin) );

	LIST_Init( &p_wk->list, sc_list_data_home, NELEMS(sc_list_data_home), 
			&p_wk->msg, p_wk->p_bmpwin, heapID );

	p_wk->is_temp_modules	= TRUE;

}
//----------------------------------------------------------------------------
/**
 *	@brief	�ꎞ�I�ȃ��W���[����j��
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void DeleteTemporaryModules( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	LIST_Exit( &p_wk->list );
	GFL_BMPWIN_Delete( p_wk->p_bmpwin );
	MSG_Exit( &p_wk->msg );
	GRAPHIC_Exit( &p_wk->grp );

	p_wk->is_temp_modules	= FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void MainTemporaryModules( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	if( p_wk->is_temp_modules )
	{	
		MSG_Main( &p_wk->msg );
		GRAPHIC_Draw( &p_wk->grp );
	}
}
//=============================================================================
/**
 *	���X�g�֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	ProcChange
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
FS_EXTERN_OVERLAY(irc_aura);
static void LISTDATA_CallProcAura( DEBUG_NAGI_MAIN_WORK *p_wk )
{
	GFL_STD_MemClear( &p_wk->aura_param, sizeof(IRC_AURA_PARAM) );
	p_wk->aura_param.is_only_play	= TRUE;
	DEBUG_NAGI_COMMAND_CallProc( p_wk, FS_OVERLAY_ID(irc_aura), &IrcAura_ProcData, &p_wk->aura_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	ProcChange
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
FS_EXTERN_OVERLAY(irc_rhythm);
static void LISTDATA_CallProcRhythm( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	GFL_STD_MemClear( &p_wk->rhythm_param, sizeof(IRC_RHYTHM_PARAM) );
	p_wk->rhythm_param.is_only_play	= TRUE;
	DEBUG_NAGI_COMMAND_CallProc( p_wk, FS_OVERLAY_ID(irc_rhythm), &IrcRhythm_ProcData, &p_wk->rhythm_param );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ProcChange
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
FS_EXTERN_OVERLAY(irc_result);
static void LISTDATA_CallProcResult( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	GFL_STD_MemClear( &p_wk->result_param, sizeof(IRC_RESULT_PARAM) );
	p_wk->result_param.score	= 80;
	DEBUG_NAGI_COMMAND_CallProc( p_wk, FS_OVERLAY_ID(irc_result), &IrcResult_ProcData, &p_wk->result_param );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ProcChange
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
FS_EXTERN_OVERLAY(irc_compatible);
static void LISTDATA_CallProcCompatible( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	p_wk->compatible_param.p_gamesys	= NULL;
	p_wk->compatible_param.is_only_play	= FALSE;
	DEBUG_NAGI_COMMAND_CallProc( p_wk, FS_OVERLAY_ID(irc_compatible), &IrcCompatible_ProcData, &p_wk->compatible_param );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ProcChange
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void LISTDATA_CallProcCompatibleDebug( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	p_wk->compatible_param.p_gamesys	= NULL;
	p_wk->compatible_param.is_only_play	= TRUE;
	DEBUG_NAGI_COMMAND_CallProc( p_wk, FS_OVERLAY_ID(irc_compatible), &IrcCompatible_ProcData, &p_wk->compatible_param );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I��
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void LISTDATA_CallProcNameDebug( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	DEBUG_NAGI_COMMAND_CallProc( p_wk, NO_OVERLAY_ID, &DebugIrcName_ProcData, NULL );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����L���OPROC�ւ̑J��
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk���[�N
 *
 */
//-----------------------------------------------------------------------------
FS_EXTERN_OVERLAY(irc_ranking);
static void LISTDATA_CallProcRankingDebug( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	DEBUG_NAGI_COMMAND_CallProc( p_wk, FS_OVERLAY_ID(irc_ranking), &IrcRanking_ProcData, NULL );
}
//----------------------------------------------------------------------------
/**
 *	@brief	GTSPROC�ւ̈ړ�
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
FS_EXTERN_OVERLAY(worldtrade);
static void LISTDATA_CallProcGts( DEBUG_NAGI_MAIN_WORK *p_wk )
{
	GAMEDATA * p_gamedata	= GAMEDATA_Create(HEAPID_NAGI_DEBUG);

	p_wk->gts_param.savedata				= GAMEDATA_GetSaveControlWork( p_gamedata );
	p_wk->gts_param.worldtrade_data	= SaveData_GetWorldTradeData(p_wk->gts_param.savedata);
	p_wk->gts_param.systemdata			= SaveData_GetSystemData(p_wk->gts_param.savedata);
	p_wk->gts_param.myparty					= SaveData_GetTemotiPokemon(p_wk->gts_param.savedata);
	p_wk->gts_param.mybox						= SaveData_GetBoxData(p_wk->gts_param.savedata);
	p_wk->gts_param.zukanwork				= NULL;
	p_wk->gts_param.wifilist				= SaveData_GetWifiListData(p_wk->gts_param.savedata);
	p_wk->gts_param.wifihistory			= SaveData_GetWifiHistory(p_wk->gts_param.savedata);
	p_wk->gts_param.mystatus				= SaveData_GetMyStatus(p_wk->gts_param.savedata);
	p_wk->gts_param.config					= SaveData_GetConfig(p_wk->gts_param.savedata);
	p_wk->gts_param.record					= SaveData_GetRecord(p_wk->gts_param.savedata);
	p_wk->gts_param.myitem					= NULL;
	
	p_wk->gts_param.zukanmode				= 0;
	p_wk->gts_param.profileId				= 0;
	p_wk->gts_param.contestflag			= FALSE;
	p_wk->gts_param.connect					= 0;


	DEBUG_NAGI_COMMAND_CallProc( p_wk, FS_OVERLAY_ID(worldtrade), &WorldTrade_ProcData, &p_wk->gts_param );

	GAMEDATA_Delete( p_gamedata );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�R���t�B�OPROC�ւ̈ړ�
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void LISTDATA_CallProcConfig( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	DEBUG_NAGI_COMMAND_CallProc( p_wk, FS_OVERLAY_ID(config_panel), &ConfigPanelProcData, NULL );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����L���O�f�[�^�P�}��
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void LISTDATA_AddRankData( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	IRC_COMPATIBLE_SAVEDATA *p_sv	= IRC_COMPATIBLE_SV_GetSavedata( SaveControl_GetPointer() );
	{	
		STRCODE	str[128];
		u16	strlen;
		u16 idx;
		static const struct
		{	
			u16 * p_str;
			u32 ID;
		} scp_debug_rank_data[]	=
		{	
			{	
				L"���̂�",
				0x573,
			},
			{	
				L"�A���C�Y�~",
				0x785,
			},
			{	
				L"�L�^����",
				0x123,
			},
			{	
				L"���킨����",
				0x987,
			},
			{	
				L"���C�R",
				0x782
			},
			{	
				L"�C�P�C�P",
				0x191,
			},
			{	
				L"�؂�",
				0x232
			},
			{	
				L"�}�i",
				0x595,
			},
			{	
				L"�����݂�",
				0x999,
			},
			{	
				L"�v���b�g",
				0x1234,
			},
			{	
				L"�v���b�g",
				0x2345,
			},
			{	
				L"�v���b�g",
				0x3456,
			},
		};
		u16 *p_str;

		idx	= GFUser_GetPublicRand( NELEMS(scp_debug_rank_data) );

		p_str	= scp_debug_rank_data[idx].p_str;

		strlen	= wcslen(p_str);
		GFL_STD_MemCopy(p_str, str, strlen*2);
		str[strlen]	= GFL_STR_GetEOMCode();

		IRC_COMPATIBLE_SV_AddRanking( p_sv, str, GFUser_GetPublicRand( 101 ), scp_debug_rank_data[idx].ID );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����L���O�f�[�^��Full
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void LISTDATA_FullRankData( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	IRC_COMPATIBLE_SAVEDATA *p_sv	= IRC_COMPATIBLE_SV_GetSavedata( SaveControl_GetPointer() );
	while( IRC_COMPATIBLE_SV_GetRankNum( p_sv ) < IRC_COMPATIBLE_SV_RANKING_MAX )
	{	
		LISTDATA_AddRankData( p_wk );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�^�E���}�b�vPROC��
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
FS_EXTERN_OVERLAY(townmap);
static void LISTDATA_CallProcTownMap( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	GFL_STD_MemClear( &p_wk->townmap_param, sizeof(TOWNMAP_PARAM) );
	p_wk->townmap_param.mode			= TOWNMAP_MODE_MAP;
	DEBUG_NAGI_COMMAND_CallProc( p_wk, FS_OVERLAY_ID(townmap), &TownMap_ProcData, &p_wk->townmap_param );
}
static void LISTDATA_CallProcSkyJump( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	GFL_STD_MemClear( &p_wk->townmap_param, sizeof(TOWNMAP_PARAM) );
	p_wk->townmap_param.mode			= TOWNMAP_MODE_DEBUGSKY;
	DEBUG_NAGI_COMMAND_CallProc( p_wk, FS_OVERLAY_ID(townmap), &TownMap_ProcData, &p_wk->townmap_param );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I��
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void LISTDATA_Return( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	DEBUG_NAGI_COMMAND_End( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ŏ��̃��X�g��
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void LISTDATA_NextListHome( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	DEBUG_NAGI_COMMAND_ChangeMenu( p_wk, sc_list_data_home, NELEMS(sc_list_data_home) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	1�y�[�W�֔��
 *
 *	@param	DEBUG_NAGI_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void LISTDATA_NextListPage1( DEBUG_NAGI_MAIN_WORK *p_wk )
{	
	DEBUG_NAGI_COMMAND_ChangeMenu( p_wk, sc_list_data_page1, NELEMS(sc_list_data_page1) );
}
//=============================================================================
/**
 *				GRAPHIC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�`��֌W������
 *
 *	@param	GRAPHIC_WORK* p_wk	���[�N
 *	@param	heapID					�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Init( GRAPHIC_WORK* p_wk, HEAPID heapID )
{
	static const GFL_DISP_VRAM sc_vramSetTable =
	{
		GX_VRAM_BG_128_A,						// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,				// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_NONE,						// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,       // �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_0_D,						// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_01_FG,			// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_128K,		
		GX_OBJVRAMMODE_CHAR_1D_128K,		
	};

	//���[�N�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_WORK) );

	//VRAM�N���A�[
	GFL_DISP_ClearVRAM( 0 );

	// VRAM�o���N�ݒ�
	GFL_DISP_SetBank( &sc_vramSetTable );
	GX_SetBankForLCDC(GX_VRAM_LCDC_B);	//Capture�p

	// �f�B�X�v���CON
	GFL_DISP_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );
	GFL_DISP_SetDispOn();

	//�\��
	GFL_DISP_GX_InitVisibleControl();

	//	�t�H���g������
	GFL_FONTSYS_Init();

	//�`�惂�W���[��
	GRAPHIC_BG_Init( &p_wk->gbg, heapID );
	GRAPHIC_3D_Init( &p_wk->g3d, heapID );

	{	
		int i;
		for( i = 0; i < CIRCLE_MAX; i++ )
		{	
			CIRCLE_Init( &p_wk->c[i], 0, GX_RGB(31,16,0) );
			CIRCLE_SetAddR( &p_wk->c[i], 0x8F+0x1F*i );
			CIRCLE_SetAddColor( &p_wk->c[i], GX_RGB(0,0,1) );
		}
	}

	p_wk->p_tcb	= GFUser_VIntr_CreateTCB(Graphic_Tcb_Capture, p_wk, 0 );
	
	p_wk->is_init	= TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�`��֌W�j��
 *
 *	@param	GRAPHIC_WORK* p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Exit( GRAPHIC_WORK* p_wk )
{
	GF_ASSERT( p_wk->is_init );

	GFL_TCB_DeleteTask( p_wk->p_tcb );
	GX_ResetCapture();

	GRAPHIC_3D_Exit( &p_wk->g3d );
	GRAPHIC_BG_Exit( &p_wk->gbg );

	p_wk->is_init	= FALSE;
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`��֌W�`�揈��
 *
 *	@param	GRAPHIC_WORK* p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Draw( GRAPHIC_WORK* p_wk )
{
	if( p_wk->is_init )
	{	
		GRAPHIC_3D_StartDraw( &p_wk->g3d );
		//NNS�n��3D�`��
		//�Ȃ�
		NNS_G3dGeFlushBuffer();
		//SDK�n��3D�`��

		//�~�̕`��
		{	
			int i;
			for( i = 0; i < CIRCLE_MAX; i++ )
			{	
				CIRCLE_Draw( &p_wk->c[i] );
			}
		}
		GRAPHIC_3D_EndDraw( &p_wk->g3d );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	V�u�����N�^�X�N
 *
 *	@param	GFL_TCB *p_tcb
 *	@param	*p_work 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void Graphic_Tcb_Capture( GFL_TCB *p_tcb, void *p_work )
{	
	GFL_BG_VBlankFunc();

	GX_SetCapture(GX_CAPTURE_SIZE_256x192,  // Capture size
                      GX_CAPTURE_MODE_AB,			   // Capture mode
                      GX_CAPTURE_SRCA_3D,						 // Blend src A
                      GX_CAPTURE_SRCB_VRAM_0x00000,     // Blend src B
                      GX_CAPTURE_DEST_VRAM_B_0x00000,   // Output VRAM
                      14,             // Blend parameter for src A
                      14);            // Blend parameter for src B
}

//=============================================================================
/**
 *					GRAPHIC_BG
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BG�`��	������
 *
 *	@param	GRAPHIC_BG_WORK* p_wk	���[�N
 *	@param	heapID						�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK* p_wk, HEAPID heapID )
{
	int i;

	//�a�f�V�X�e���N��
	GFL_BG_Init( heapID );
	GFL_BMPWIN_Init( heapID );

	//�a�f���[�h�ݒ�
	{
		static const GFL_BG_SYS_HEADER sc_bg_sys_header = 
		{
		//	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
				GX_DISPMODE_VRAM_B,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
		};	
		GFL_BG_SetBGMode( &sc_bg_sys_header );
	}

	//�a�f�R���g���[���ݒ�
	{
		int i;

		for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
		{
			GFL_BG_SetBGControl( sc_bgcnt_frame[i], &sc_bgcnt_data[i], GFL_BG_MODE_TEXT );
			GFL_BG_ClearFrame( sc_bgcnt_frame[i] );
			GFL_BG_SetVisible( sc_bgcnt_frame[i], VISIBLE_ON );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG�`��	�j��
 *
 *	@param	GRAPHIC_BG_WORK* p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK* p_wk )
{	
	int i;

	// BG�R���g���[���j��
	for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
	{
		GFL_BG_FreeBGControl( sc_bgcnt_frame[i] );
	}

	// BG�V�X�e���j��
	GFL_BMPWIN_Exit();
	GFL_BG_Exit();
}
//=============================================================================
/**
 *					GRAPHIC_3D
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�RD���̏�����
 *
 *	@param	p_wk			���[�N
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_3D_Init( GRAPHIC_3D_WORK *p_wk, HEAPID heapID )
{
	static const VecFx32 sc_CAMERA_PER_POS		= { 0,0,FX32_CONST(5) };
	static const VecFx32 sc_CAMERA_PER_UP			= { 0,FX32_ONE,0 };
	static const VecFx32 sc_CAMERA_PER_TARGET	= { 0,0,FX32_CONST( 0 ) };

	enum{	
		CAMERA_PER_FOVY	=	(40),
		CAMERA_PER_ASPECT =	(FX32_ONE * 4 / 3),
		CAMERA_PER_NEAR	=	(FX32_ONE * 1),
		CAMERA_PER_FER	=	(FX32_ONE * 800),
		CAMERA_PER_SCALEW	=(0),
	};

	GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX128K,
			GFL_G3D_VMANLNK, GFL_G3D_PLT32K, 0, heapID, Graphic_3d_SetUp );

	p_wk->p_camera = GFL_G3D_CAMERA_CreatePerspective( CAMERA_PER_FOVY, CAMERA_PER_ASPECT,
				CAMERA_PER_NEAR, CAMERA_PER_FER, CAMERA_PER_SCALEW, 
				&sc_CAMERA_PER_POS, &sc_CAMERA_PER_UP, &sc_CAMERA_PER_TARGET, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�RD���̔j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_3D_Exit( GRAPHIC_3D_WORK *p_wk )
{
	GFL_G3D_CAMERA_Delete( p_wk->p_camera );
	GFL_G3D_Exit();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`��J�n
 *	
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_3D_StartDraw( GRAPHIC_3D_WORK *p_wk )
{	
	GFL_G3D_DRAW_Start();
	GFL_G3D_CAMERA_Switching( p_wk->p_camera );
	GFL_G3D_DRAW_SetLookAt();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`��I��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_3D_EndDraw( GRAPHIC_3D_WORK *p_wk )
{	
	GFL_G3D_DRAW_End();
}

//----------------------------------------------------------------------------
/**
 *	@brief	���Z�b�g�A�b�v�R�[���o�b�N�֐�
 */
//-----------------------------------------------------------------------------
static void Graphic_3d_SetUp( void )
{
	// �R�c�g�p�ʂ̐ݒ�(�\�����v���C�I���e�B�[)
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	G2_SetBG0Priority(0);

	// �e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
	G3X_SetShading( GX_SHADING_HIGHLIGHT );
	G3X_AntiAlias( FALSE );
	G3X_AlphaTest( FALSE, 0 );	// �A���t�@�e�X�g�@�@�I�t
	G3X_AlphaBlend( TRUE );		// �A���t�@�u�����h�@�I��
	G3X_EdgeMarking( FALSE );
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

	// �N���A�J���[�̐ݒ�
	G3X_SetClearColor(GX_RGB(0,0,0),31,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog
	// �r���[�|�[�g�̐ݒ�
	G3_ViewPort(0, 0, 255, 191);

	// ���C�g�ݒ�
	{
		static const GFL_G3D_LIGHT sc_GFL_G3D_LIGHT[] = {
			{
				{ 0, -FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
			{
				{ 0, FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
			{
				{ 0, -FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
			{
				{ 0, -FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
		};
		int i;
		
		for( i=0; i<NELEMS(sc_GFL_G3D_LIGHT); i++ ){
			GFL_G3D_SetSystemLight( i, &sc_GFL_G3D_LIGHT[i] );
		}
	}

	//�����_�����O�X���b�v�o�b�t�@
	GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
}


//=============================================================================
/**
 *					LIST
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���X�g�V�X�e���@������
 *
 *	@param	LIST_WORK *p_wk	���[�N
 *	@param	*p_tbl					�ݒ�e�[�u��
 *	@param	tbl_max					�ݒ�e�[�u����
 *	@param	*p_msg					���b�Z�[�W���[�N
 *	@param	HEAPID heapID		�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void LIST_Init( LIST_WORK *p_wk, const LIST_SETUP_TBL *cp_tbl, u32 tbl_max, MSG_WORK *p_msg, GFL_BMPWIN*	p_bmpwin, HEAPID heapID )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(LIST_WORK));


	//LISTDATA create
	{	
		int i;
		STRBUF	*p_strbuf;
		STRCODE	str[128];
		u16	strlen;

		p_wk->p_list_data	= BmpMenuWork_ListCreate( tbl_max, heapID );

		p_strbuf = GFL_STR_CreateBuffer( LISTDATA_STR_LENGTH+1, heapID);

		for( i = 0; i < tbl_max; i++ )
		{	
			//
			strlen	= wcslen(cp_tbl[i].str);
			GFL_STD_MemCopy(cp_tbl[i].str, str, strlen*2);
			str[strlen]	= GFL_STR_GetEOMCode();
			GFL_STR_SetStringCode( p_strbuf, str);

			BmpMenuWork_ListAddString( &p_wk->p_list_data[i], p_strbuf, cp_tbl[i].param, heapID );
		}

		GFL_STR_DeleteBuffer( p_strbuf );
	}

	//LIST create
	{	
		BMPMENULIST_HEADER	header;

		GFL_STD_MemClear( &header, sizeof(BMPMENULIST_HEADER));
		header.list				= p_wk->p_list_data;
		header.win				= p_bmpwin;
		header.count			= tbl_max;
		header.line				= 5;
		header.rabel_x		= 0;
		header.data_x			= 16;
		header.cursor_x		= 0;
		header.line_y			= 2;
		header.f_col			= 1;
		header.b_col			= 15;
		header.s_col			= 2;
		header.msg_spc		= 0;
		header.line_spc		= 0;
		header.page_skip	= BMPMENULIST_LRKEY_SKIP;
		header.font				= 0;
		header.c_disp_f		= 0;
		header.work				= NULL;
		header.font_size_x= 16;
		header.font_size_y= 16;
		header.msgdata		= NULL;
		header.print_util	= MSG_GetPrintUtil( p_msg, p_bmpwin );
		header.print_que	= MSG_GetPrintQue( p_msg );
		header.font_handle= MSG_GetFont( p_msg );

		p_wk->p_list	= BmpMenuList_Set( &header, 0, 0, heapID );
		BmpMenuList_SetCursorBmp( p_wk->p_list, heapID );
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	���X�g�V�X�e��	�j��
 *
 *	@param	LIST_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void LIST_Exit( LIST_WORK *p_wk )
{
	BmpMenuList_Exit( p_wk->p_list, NULL, NULL );
	BmpMenuWork_ListDelete( p_wk->p_list_data );
	GFL_STD_MemClear( p_wk, sizeof(LIST_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���X�g�V�X�e��	���C������
 *
 *	@param	LIST_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void LIST_Main( LIST_WORK *p_wk )
{	
	p_wk->select	= BmpMenuList_Main(	p_wk->p_list );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���X�g����
 *
 *	@param	const LIST_WORK *cp_wk	���[�N
 *	@param	*p_select								���X�g�̃f�[�^
 *
 *	@retval	TRUE�Ȃ�Ό���
 *	@retval	FALSE�Ȃ�ΑI��
 */
//-----------------------------------------------------------------------------
static BOOL LIST_IsDecide( const LIST_WORK *cp_wk, u32 *p_select )
{	
	if( cp_wk->select != BMPMENULIST_NULL
		&& cp_wk->select != BMPMENULIST_CANCEL )
	{	
		if( p_select )
		{	
			*p_select	= cp_wk->select;
		}
		return TRUE;
	}

	return FALSE;
}

//=============================================================================
/**
 *					MSG
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	MSG�֌W��ݒ�
 *
 *	@param	MSG_WORK *p_wk	���[�N
 *	@param	heapID					�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void MSG_Init( MSG_WORK *p_wk, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(MSG_WORK) );

	p_wk->p_font	= GFL_FONT_Create( ARCID_FONT,
    NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );

	p_wk->p_print_que = PRINTSYS_QUE_Create( heapID );

	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 0, 0, heapID );
	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, 0, 0, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	MSG�֌W��j��
 *
 *	@param	MSG_WORK *p_wk	���[�N
 *	
 */
//-----------------------------------------------------------------------------
static void MSG_Exit( MSG_WORK *p_wk )
{	
	PRINTSYS_QUE_Delete( p_wk->p_print_que );

	GFL_FONT_Delete( p_wk->p_font );
	GFL_STD_MemClear( p_wk, sizeof(MSG_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	MSG�֌W	���C������
 *
 *	@param	MSG_WORK *p_wk	���[�N
 *
 * @retval  BOOL	�������I�����Ă����TRUE�^����ȊO��FALSE
 *
 */
//-----------------------------------------------------------------------------
static BOOL MSG_Main( MSG_WORK *p_wk )
{	
	if( PRINTSYS_QUE_Main( p_wk->p_print_que ) )
	{
		return PRINT_UTIL_Trans( &p_wk->print_util, p_wk->p_print_que );
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PRINT_UTIL��ݒ肵�擾
 *
 *	@param	MSG_WORK *p_wk	���[�N
 *	@param	BMPWIN
 *
 *	@return	PRINT_UTIL
 */
//-----------------------------------------------------------------------------
static PRINT_UTIL * MSG_GetPrintUtil( MSG_WORK *p_wk, GFL_BMPWIN*	p_bmpwin )
{	
	PRINT_UTIL_Setup( &p_wk->print_util, p_bmpwin );
	return &p_wk->print_util;
}

//----------------------------------------------------------------------------
/**
 *	@brief	FONT���擾
 *
 *	@param	const MSG_WORK *cp_wk		���[�N
 *
 *	@return	FONT
 */
//-----------------------------------------------------------------------------
static GFL_FONT*	MSG_GetFont( const MSG_WORK *cp_wk )
{	
	return cp_wk->p_font;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PRINTQUE���擾
 *
 *	@param	const MSG_WORK *cp_wk		���[�N
 *
 *	@return	PRINTQUE
 */
//-----------------------------------------------------------------------------
static PRINT_QUE* MSG_GetPrintQue( const MSG_WORK *cp_wk )
{	
	return cp_wk->p_print_que;
}
//=============================================================================
/**
 *				�~�v���~�e�B�u
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�~�v���~�e�B�u���쐬
 *
 *	@param	CIRCLE_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_Init( CIRCLE_WORK *p_wk, u16 r, GXRgb color )
{	
	GFL_STD_MemClear( p_wk, sizeof(CIRCLE_WORK) );
	p_wk->r			= r;
	p_wk->red		= (color&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
	p_wk->green	= (color&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
	p_wk->blue	= (color&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;

	//�~�̒��_�쐬
	{	
		int i;
		for( i = 0; i < CIRCLE_VTX_MAX; i++ )
		{	
			p_wk->vtx[i].x	=	FX_CosIdx( ( i * 360 / CIRCLE_VTX_MAX) * 0xFFFF / 360 );
			p_wk->vtx[i].y	=	FX_SinIdx( ( i * 360 / CIRCLE_VTX_MAX) * 0xFFFF / 360 );
			p_wk->vtx[i].z	=	0;
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�~��`��
 *
 *	@param	CIRCLE_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_Draw( CIRCLE_WORK *p_wk )
{	
	//�}�e���A���ݒ�
	G3_MaterialColorDiffAmb(GX_RGB(16, 16, 16), GX_RGB(16, 16, 16), FALSE );
	G3_MaterialColorSpecEmi(GX_RGB( 16, 16, 16 ), GX_RGB( 31,31,31 ), FALSE );
	G3_PolygonAttr( GX_LIGHTMASK_0123,GX_POLYGONMODE_MODULATE,GX_CULL_BACK,0,31,0 );

	//�T�C�Y�ݒ�
	G3_Scale( FX_SinIdx(p_wk->r), FX_SinIdx(p_wk->r), FX_SinIdx(p_wk->r));

	//�`��
	G3_Begin( GX_BEGIN_TRIANGLES );
	{
		int i;
		G3_Color(GX_RGB( p_wk->red, p_wk->green, p_wk->blue ));
		for( i = 0; i < CIRCLE_VTX_MAX - 1; i++ )
		{	
			Circle_DrawLine( &p_wk->vtx[i], &p_wk->vtx[i+1] );
		}
		Circle_DrawLine( &p_wk->vtx[i], &p_wk->vtx[0] );
	}
	G3_End();

	//����
	p_wk->r				+= p_wk->add_r;
	p_wk->red			+= p_wk->add_red;
	p_wk->green		+= p_wk->add_green;
	p_wk->blue		+= p_wk->add_blue;
	p_wk->red			%= 31;
	p_wk->green		%= 31;
	p_wk->blue		%= 31;

}

//----------------------------------------------------------------------------
/**
 *	@brief	���a���Z�l���Z�b�g
 *
 *	@param	CIRCLE_WORK *p_wk	���[�N
 *	@param	add_r							���a���Z�l	
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_SetAddR( CIRCLE_WORK *p_wk, u16 add_r )
{	
	p_wk->add_r			= add_r;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�F���Z�l���Z�b�g
 *
 *	@param	CIRCLE_WORK *p_wk	���[�N
 *	@param	add_color					�F���Z�l
 *
 */
//-----------------------------------------------------------------------------
static void CIRCLE_SetAddColor( CIRCLE_WORK *p_wk, u16 add_color )
{	
	p_wk->add_red		= (add_color&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
	p_wk->add_green	= (add_color&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
	p_wk->add_blue	= (add_color&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;
}
//----------------------------------------------------------------------------
/**
 *	@brief	����`��
 *
 *	@param	VecFx16 *p_start		�J�n���W
 *	@param	*p_end							�I�����W
 *
 */
//-----------------------------------------------------------------------------
static void Circle_DrawLine( VecFx16 *p_start, VecFx16 *p_end )
{	
	G3_Vtx( p_start->x, p_start->y, p_start->z);
	G3_Vtx( p_end->x, p_end->y, p_end->z);
	G3_Vtx( p_start->x, p_start->y, p_start->z);
}
