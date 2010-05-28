//=============================================================================
/**
 *
 *	@file		d_hosaka.c
 *	@brief  �ۍ�f�o�b�O�p���j���[
 *	@author		hosaka genya
 *	@data		2010.05.28
 *
 */
//=============================================================================
#if PM_DEBUG

//lib
#include <gflib.h>
#include <wchar.h>	//for wcslen

//contant
#include "system/main.h"	//HEAPID
#include "system/gfl_use.h"

//module
#include "system/bmp_menulist.h"

//archive
#include "arc_def.h"
#include "font/font.naix"

//proc
#include "title/title.h"

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//-------------------------------------
///	
//=====================================
typedef enum 
{
	PROC_TYPE_NONE,
	PROC_TYPE_CALL,
	PROC_TYPE_NEXT,
} PROC_TYPE;

//--------------------------------------------------------------
///	���X�g
//==============================================================
typedef enum
{
  LIST_SELECT_NONE,
  LIST_SELECT_DECIDE,
  LIST_SELECT_CANCEL,
} LIST_SELECT;

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================
#define LISTDATA_STR_LENGTH (32)

//-------------------------------------
///	BG�֌W
//=====================================
typedef struct
{
	int dummy;
} GRAPHIC_BG_WORK;
//-------------------------------------
///	�`��֌W
//=====================================
typedef struct 
{
	GRAPHIC_BG_WORK		gbg;
	BOOL							is_init;

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

//--------------------------------------------------------------
///	���C�����[�N
//==============================================================
typedef struct
{ 
  GRAPHIC_WORK      grp;
	MSG_WORK					msg;
  LIST_WORK         list;
	GFL_BMPWIN				*p_bmpwin;
	BOOL	is_temp_modules;
  
  BOOL is_end;
	PROC_TYPE	proc_type;
	
  //Proc�؂�ւ��p
	FSOverlayID overlay_Id;
	const GFL_PROC_DATA *p_procdata;
	void	*p_proc_work;

} D_HOSAKA_WORK;

//-------------------------------------
///	���X�g�ݒ�e�[�u��
//=====================================
typedef BOOL (*LISTFUNC)( D_HOSAKA_WORK *work , const int idx );
typedef struct 
{	
	u16	str[LISTDATA_STR_LENGTH];
  LISTFUNC func;
} LIST_SETUP_TBL;

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================

//proc
static GFL_PROC_RESULT D_HOSAKA_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_parent, void *p_work );
static GFL_PROC_RESULT D_HOSAKA_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_parent, void *p_work );
static GFL_PROC_RESULT D_HOSAKA_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_parent, void *p_work );

static void D_HOSAKA_COMMAND_CallProc( D_HOSAKA_WORK * p_wk, FSOverlayID ov_id, const GFL_PROC_DATA *p_procdata, void *p_work );
static void D_HOSAKA_COMMAND_NextProc( D_HOSAKA_WORK * p_wk, FSOverlayID ov_id, const GFL_PROC_DATA *p_procdata, void *p_work );
static void D_HOSAKA_COMMAND_ChangeMenu( D_HOSAKA_WORK * p_wk, const LIST_SETUP_TBL *cp_tbl, u32 tbl_max );
static void D_HOSAKA_COMMAND_End( D_HOSAKA_WORK *p_wk );
static void CreateTemporaryModules( D_HOSAKA_WORK *p_wk, HEAPID heapID );
static void DeleteTemporaryModules( D_HOSAKA_WORK *p_wk );
static void MainTemporaryModules( D_HOSAKA_WORK *p_wk );
static void GRAPHIC_Init( GRAPHIC_WORK* p_wk, HEAPID heapID );
static void GRAPHIC_Exit( GRAPHIC_WORK* p_wk );
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK* p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void LIST_Init( LIST_WORK *p_wk, const LIST_SETUP_TBL *cp_tbl, u32 tbl_max, MSG_WORK *p_msg, GFL_BMPWIN*	p_bmpwin, HEAPID heapID );
static void LIST_Exit( LIST_WORK *p_wk );
static void LIST_Main( LIST_WORK *p_wk );
static LIST_SELECT LIST_IsDecide( const LIST_WORK *cp_wk, u32 *p_select );
static void MSG_Init( MSG_WORK *p_wk, HEAPID heapID );
static void MSG_Exit( MSG_WORK *p_wk );
static BOOL MSG_Main( MSG_WORK *p_wk );
static PRINT_UTIL * MSG_GetPrintUtil( MSG_WORK *p_wk, GFL_BMPWIN*	p_bmpwin );
static GFL_FONT*	MSG_GetFont( const MSG_WORK *cp_wk );
static PRINT_QUE* MSG_GetPrintQue( const MSG_WORK *cp_wk );
static BOOL TESTMODE_ITEM_SelectUNSelect( D_HOSAKA_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectCmmBtlDemoEnd( D_HOSAKA_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectCmmBtlDemoEndMulti( D_HOSAKA_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectWcsCmmBtlDemoEnd( D_HOSAKA_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectCmmBtlDemoStart( D_HOSAKA_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectWcsCmmBtlDemoStart( D_HOSAKA_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectCmmBtlDemoStartMulti( D_HOSAKA_WORK* work, const int idx );
static BOOL TESTMODE_ITEM_SelectPmsSelect( D_HOSAKA_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectPmsDirect( D_HOSAKA_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectPmsPeculiar( D_HOSAKA_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectPmsSingle( D_HOSAKA_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectPmsDouble( D_HOSAKA_WORK *work , const int idx );
static BOOL TESTMODE_ITEM_SelectMicTest( D_HOSAKA_WORK *work , const int idx );

//-------------------------------------
///	�O���Q��PROC�f�[�^
//=====================================
const GFL_PROC_DATA	DHosakaMainProcData = 
{
	D_HOSAKA_PROC_Init,
	D_HOSAKA_PROC_Main,
	D_HOSAKA_PROC_Exit,
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

// ���j���[���X�g
static const LIST_SETUP_TBL sc_list_data_home[] = 
{
//  {L"�C���g���f��",TESTMODE_ITEM_SelectIntro },
  {L"���A�t���A�I��",TESTMODE_ITEM_SelectUNSelect},
  {L"�v�b�r�o�g���O�f��",TESTMODE_ITEM_SelectWcsCmmBtlDemoStart},
  {L"�v�b�r�o�g����f��",TESTMODE_ITEM_SelectWcsCmmBtlDemoEnd},
  {L"�ʐM�o�g���O�f��",TESTMODE_ITEM_SelectCmmBtlDemoStart},
  {L"�ʐM�o�g���O�}���`",TESTMODE_ITEM_SelectCmmBtlDemoStartMulti},
  {L"�ʐM�o�g����f��",TESTMODE_ITEM_SelectCmmBtlDemoEnd},
  {L"�ʐM�o�g����}���`",TESTMODE_ITEM_SelectCmmBtlDemoEndMulti},
  {L"���񂢉�b�I��",TESTMODE_ITEM_SelectPmsSelect },
  {L"���񂢉�b���Ă�",TESTMODE_ITEM_SelectPmsDirect },
  {L"���񂢉�b�Œ�",TESTMODE_ITEM_SelectPmsPeculiar },
  {L"���񂢉�b��P��",TESTMODE_ITEM_SelectPmsSingle },
  {L"���񂢉�b��P��",TESTMODE_ITEM_SelectPmsDouble },
  {L"�}�C�N�e�X�g",TESTMODE_ITEM_SelectMicTest },
  
//  {L"���ǂ�"        ,TESTMODE_ITEM_BackTopMenu },
};


//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================
//
//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	GFL_PROC *p_proc
 *	@param	*p_seq
 *	@param	*p_parent
 *	@param	*p_work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT D_HOSAKA_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_parent, void *p_work )
{
  D_HOSAKA_WORK* p_wk;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_HOSAKA_DEBUG, 0x20000 );
  p_wk = GFL_PROC_AllocWork( p_proc, sizeof(D_HOSAKA_WORK), HEAPID_HOSAKA_DEBUG );
  GFL_STD_MemClear( p_wk, sizeof(D_HOSAKA_WORK) );

  CreateTemporaryModules( p_wk, HEAPID_HOSAKA_DEBUG );

  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	GFL_PROC *p_proc
 *	@param	*p_seq
 *	@param	*p_parent
 *	@param	*p_work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT D_HOSAKA_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_parent, void *p_work )
{
  D_HOSAKA_WORK* p_wk = p_work;

  DeleteTemporaryModules( p_wk );
  
	if( p_wk->proc_type == PROC_TYPE_NEXT )
	{	
		//����PROC�\��
		GFL_PROC_SysSetNextProc(
				p_wk->overlay_Id, p_wk->p_procdata, p_wk->p_proc_work );
	}
  else if( p_wk->is_end )
  { 

		//����PROC�\��
		GFL_PROC_SysSetNextProc(
				FS_OVERLAY_ID(title), &TitleProcData, NULL );
  }

  GFL_PROC_FreeWork( p_proc );
  GFL_HEAP_DeleteHeap( HEAPID_HOSAKA_DEBUG );
  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	GFL_PROC *p_proc
 *	@param	*p_seq
 *	@param	*p_parent
 *	@param	*p_work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT D_HOSAKA_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_parent, void *p_work )
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

  D_HOSAKA_WORK* p_wk = p_work;

  switch( *p_seq )
  {
	 case SEQ_INIT:
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

    case SEQ_MAIN :
      {
        u32 idx;
        LIST_SELECT select;
        LIST_Main( &p_wk->list );
        select  = LIST_IsDecide( &p_wk->list, &idx );
        if( select == LIST_SELECT_DECIDE )
        {	
          LISTFUNC func = (LISTFUNC)idx;
          func( p_wk, NULL );
        }
        if( select == LIST_SELECT_CANCEL )
        { 
          p_wk->is_end  = TRUE;
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
    HOSAKA_Printf( "ret!\n" );
    if( p_wk->p_proc_work != NULL )
    {
      GFL_HEAP_FreeMemory( p_wk->p_proc_work );
    }
		CreateTemporaryModules( p_wk, HEAPID_HOSAKA_DEBUG );
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

  default : GF_ASSERT(0);
  }

  MainTemporaryModules( p_wk );

  return GFL_PROC_RES_CONTINUE;
}

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//=============================================================================
/**
 *		����p
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�v���Z�X�؂�ւ�
 *
 *	@param	D_HOSAKA_WORK * p_wk	���[�N
 *	@param	ov_id												�I�[�o�[���CID
 *	@param	GFL_PROC_DATA *p_procdata		�v���Z�X�f�[�^
 *	@param	*p_work											�n�����
 *
 */
//-----------------------------------------------------------------------------
static void D_HOSAKA_COMMAND_CallProc( D_HOSAKA_WORK * p_wk, FSOverlayID ov_id, const GFL_PROC_DATA *p_procdata, void *p_work )
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
 *	@param	D_HOSAKA_WORK * p_wk		���[�N
 *	@param	ov_id											�I�[�o�[���CID
 *	@param	GFL_PROC_DATA *p_procdata	�v���Z�X�f�[�^
 *	@param	*p_work										�n�����
 */
//-----------------------------------------------------------------------------
static void D_HOSAKA_COMMAND_NextProc( D_HOSAKA_WORK * p_wk, FSOverlayID ov_id, const GFL_PROC_DATA *p_procdata, void *p_work )
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
 *	@param	D_HOSAKA_WORK * p_wk	���[�N
 *	@param	LIST_SETUP_TBL *cp_tbl			���X�g�p�e�[�u��
 *	@param	tbl_max											���X�g�p�e�[�u���̗v�f��
 *
 */
//-----------------------------------------------------------------------------
static void D_HOSAKA_COMMAND_ChangeMenu( D_HOSAKA_WORK * p_wk, const LIST_SETUP_TBL *cp_tbl, u32 tbl_max )
{
	LIST_Exit( &p_wk->list );
	LIST_Init( &p_wk->list, cp_tbl, tbl_max, &p_wk->msg, p_wk->p_bmpwin, HEAPID_HOSAKA_DEBUG );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I��
 *
 *	@param	D_HOSAKA_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void D_HOSAKA_COMMAND_End( D_HOSAKA_WORK *p_wk )
{	
	p_wk->is_end	= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ꎞ�I�ȃ��W���[�����쐬
 *
 *	@param	D_HOSAKA_WORK *p_wk	���[�N
 *	@param	heapID											�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void CreateTemporaryModules( D_HOSAKA_WORK *p_wk, HEAPID heapID )
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
 *	@param	D_HOSAKA_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void DeleteTemporaryModules( D_HOSAKA_WORK *p_wk )
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
 *	@param	D_HOSAKA_WORK *p_wk 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void MainTemporaryModules( D_HOSAKA_WORK *p_wk )
{	
	if( p_wk->is_temp_modules )
	{	
		MSG_Main( &p_wk->msg );
	}
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

	GRAPHIC_BG_Exit( &p_wk->gbg );

	p_wk->is_init	= FALSE;
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_WORK) );
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

			BmpMenuWork_ListAddString( &p_wk->p_list_data[i], p_strbuf, (u32)cp_tbl[i].func, heapID );
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
		header.label_x		= 0;
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
static LIST_SELECT LIST_IsDecide( const LIST_WORK *cp_wk, u32 *p_select )
{	
	switch( cp_wk->select)
  { 
  case BMPMENULIST_NULL:
    return LIST_SELECT_NONE;

	case BMPMENULIST_CANCEL:
    return LIST_SELECT_CANCEL;

  default:
		if( p_select )
		{	
			*p_select	= cp_wk->select;
		}
		return LIST_SELECT_DECIDE;
  }

	return LIST_SELECT_NONE;
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
    NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );

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


#include "app/un_select.h"

static BOOL TESTMODE_ITEM_SelectUNSelect( D_HOSAKA_WORK* work, const int idx )
{
  UN_SELECT_PARAM* initParam = GFL_HEAP_AllocClearMemory( HEAPID_HOSAKA_DEBUG, sizeof(UN_SELECT_PARAM) );

  //�e�X�g�f�[�^�Z�b�g
  {
    int i;
    //���֍��R�[�h���e�X�g�Z�b�g
    for( i=0; i<WIFI_COUNTRY_MAX-1; i++)
    {
      if ( GFUser_GetPublicRand0( 2 ) ) initParam->OpenCountryFlg[i] = 1;
      else initParam->OpenCountryFlg[i] = 0;
    }
    
    //�؍ݍ��R�[�h���e�X�g�Z�b�g
    for(i=0;i<FLOOR_MARKING_MAX;i++){
      int code;
      code = GFUser_GetPublicRand(UN_LIST_MAX) + 1;
      initParam->StayCountry[i] = code;
    }

    initParam->InFloor = 5;
  }

  D_HOSAKA_COMMAND_CallProc(work, FS_OVERLAY_ID(un_select), &UNSelectProcData, initParam);

  return TRUE;
}

#include "demo/comm_btl_demo.h"

// �f�o�b�O�p�̃p�����[�^�ݒ�
// ALLOC�����ςȂ�
// ���[�N����
static void debug_param( COMM_BTL_DEMO_PARAM* prm )
{ 
  int i;

  HOSAKA_Printf("in param type = %d \n", prm->type);
  
  prm->result = GFUser_GetPublicRand( COMM_BTL_DEMO_RESULT_MAX );

  HOSAKA_Printf( "result = %d \n", prm->result );

  for( i=0; i<COMM_BTL_DEMO_TRDATA_MAX; i++ )
  {
    prm->trainer_data[i].server_version = GFUser_GetPublicRand(2);

    {
      // ALLOC�����ςȂ�
      MYSTATUS* st = GFL_HEAP_AllocMemoryLo( HEAPID_HOSAKA_DEBUG, MyStatus_GetWorkSize() );

      MyStatus_Copy( SaveData_GetMyStatus( SaveControl_GetPointer() ), st );

      if( i == 0 )
      {
        STRCODE debugname[8] = L"A";
        debugname[1] = GFL_STR_GetEOMCode();
        MyStatus_SetMySex( st, PM_MALE );
        MyStatus_SetMyName( st, debugname);
      }
      else if ( i == 1 )
      {
        STRCODE debugname[8] = L"B";
        debugname[1] = GFL_STR_GetEOMCode();
        MyStatus_SetMySex( st, PM_FEMALE );
        MyStatus_SetMyName( st, debugname);
      }
      else if ( i == 2 )
      {
        STRCODE debugname[8] = L"C";
        debugname[1] = GFL_STR_GetEOMCode();
        MyStatus_SetMySex( st, PM_MALE );
        MyStatus_SetMyName( st, debugname);
      }
      else if ( i == 3 )
      {
        STRCODE debugname[8] = L"D";
        debugname[1] = GFL_STR_GetEOMCode();
        MyStatus_SetMySex( st, PM_FEMALE );
        MyStatus_SetMyName( st, debugname);
      }

      prm->trainer_data[i].mystatus = st;
    }

#if 0
 // �p�~�\��
//    prm->trainer_data[i].trsex = PM_FEMALE;//(GFUser_GetPublicRand(2)==0) ? PM_MALE : PM_FEMALE;  
    // �g���[�i�[��
    {
      //�I�[�R�[�h��ǉ����Ă���STRBUF�ɕϊ�
      STRCODE debugname[32] = L"�Ƃ���";
      
      debugname[3] = GFL_STR_GetEOMCode();

      prm->trainer_data[i].str_trname = GFL_STR_CreateBuffer( sizeof(STRCODE)*10, HEAPID_HOSAKA_DEBUG );
      GFL_STR_SetStringCode( prm->trainer_data[i].str_trname, debugname );
    }
#endif
    
    // �f�o�b�O�|�P�p�[�e�B�[
    {
      POKEPARTY *party;
      int poke_cnt;
      int p;

      party = PokeParty_AllocPartyWork( HEAPID_HOSAKA_DEBUG );

#if 0
      if( prm->type == COMM_BTL_DEMO_TYPE_NORMAL_END || prm->type == COMM_BTL_DEMO_TYPE_NORMAL_START )
      {
        Debug_PokeParty_MakeParty( party );
      }
      else
#endif
      {
        int pokemax = 3;
        PokeParty_Init(party, pokemax);
        for (p = 0; p < pokemax; p++) 
        {
          POKEMON_PARAM* pp = GFL_HEAP_AllocMemoryLo( HEAPID_HOSAKA_DEBUG , POKETOOL_GetWorkSize() );
          PP_Clear(pp);
          PP_Setup( pp, 392+p, 99, 0 );
          PokeParty_Add(party, pp);
          GFL_HEAP_FreeMemory(pp);
        }
      }
      
      prm->trainer_data[i].party = party;

      poke_cnt = PokeParty_GetPokeCount( prm->trainer_data[i].party );

      if( prm->type == COMM_BTL_DEMO_TYPE_NORMAL_END || prm->type == COMM_BTL_DEMO_TYPE_MULTI_END )
      {
        // �ΐ��̃|�P�����̏�Ԉُ�
        for( p=0; p<poke_cnt; p++ )
        {
          POKEMON_PARAM* pp = PokeParty_GetMemberPointer( party, p );
          switch( GFUser_GetPublicRand(3) )
          {
          case 0: PP_SetSick( pp, POKESICK_DOKU ); break; // ��
          case 1: PP_Put(pp, ID_PARA_hp, 0 ); break; // �m��
          }
          HOSAKA_Printf("poke [%d] condition=%d \n",p, PP_Get( pp, ID_PARA_condition, NULL ) );
        }
      }

      HOSAKA_Printf("[%d] server_version=%d trsex=%d poke_cnt=%d \n",i, 
          prm->trainer_data[i].server_version,
          MyStatus_GetMySex( prm->trainer_data[i].mystatus ),
          poke_cnt );
    }
  }
}


static BOOL TESTMODE_ITEM_SelectCmmBtlDemoEnd( D_HOSAKA_WORK* work, const int idx )
{
  COMM_BTL_DEMO_PARAM* initParam = GFL_HEAP_AllocClearMemory( HEAPID_HOSAKA_DEBUG, sizeof(COMM_BTL_DEMO_PARAM) );

  initParam->type = COMM_BTL_DEMO_TYPE_NORMAL_END;
  debug_param(initParam);

  D_HOSAKA_COMMAND_CallProc(work,FS_OVERLAY_ID(comm_btl_demo), &CommBtlDemoProcData, initParam);

  return TRUE;
}

static BOOL TESTMODE_ITEM_SelectCmmBtlDemoEndMulti( D_HOSAKA_WORK* work, const int idx )
{
  COMM_BTL_DEMO_PARAM* initParam = GFL_HEAP_AllocClearMemory( HEAPID_HOSAKA_DEBUG, sizeof(COMM_BTL_DEMO_PARAM) );

  initParam->type = COMM_BTL_DEMO_TYPE_MULTI_END;
  debug_param(initParam);

  D_HOSAKA_COMMAND_CallProc(work,FS_OVERLAY_ID(comm_btl_demo), &CommBtlDemoProcData, initParam);

  return TRUE;
}


static BOOL TESTMODE_ITEM_SelectWcsCmmBtlDemoEnd( D_HOSAKA_WORK* work, const int idx )
{
  COMM_BTL_DEMO_PARAM* initParam = GFL_HEAP_AllocClearMemory( HEAPID_HOSAKA_DEBUG, sizeof(COMM_BTL_DEMO_PARAM) );

  initParam->type = COMM_BTL_DEMO_TYPE_NORMAL_END;
  debug_param(initParam);
  initParam->wcs_flag = TRUE;

  D_HOSAKA_COMMAND_CallProc(work,FS_OVERLAY_ID(comm_btl_demo), &CommBtlDemoProcData, initParam);

  return TRUE;
}


static BOOL TESTMODE_ITEM_SelectCmmBtlDemoStart( D_HOSAKA_WORK* work, const int idx )
{ 
  COMM_BTL_DEMO_PARAM* initParam = GFL_HEAP_AllocClearMemory( HEAPID_HOSAKA_DEBUG, sizeof(COMM_BTL_DEMO_PARAM) );

  initParam->type = COMM_BTL_DEMO_TYPE_NORMAL_START;
  debug_param(initParam);

  D_HOSAKA_COMMAND_CallProc(work,FS_OVERLAY_ID(comm_btl_demo), &CommBtlDemoProcData, initParam);

  return TRUE;
}

static BOOL TESTMODE_ITEM_SelectWcsCmmBtlDemoStart( D_HOSAKA_WORK* work, const int idx )
{ 
  COMM_BTL_DEMO_PARAM* initParam = GFL_HEAP_AllocClearMemory( HEAPID_HOSAKA_DEBUG, sizeof(COMM_BTL_DEMO_PARAM) );

  initParam->type = COMM_BTL_DEMO_TYPE_NORMAL_START;
  debug_param(initParam);
  initParam->wcs_flag = TRUE;
  

  D_HOSAKA_COMMAND_CallProc(work,FS_OVERLAY_ID(comm_btl_demo), &CommBtlDemoProcData, initParam);

  return TRUE;
}

static BOOL TESTMODE_ITEM_SelectCmmBtlDemoStartMulti( D_HOSAKA_WORK* work, const int idx )
{
  COMM_BTL_DEMO_PARAM* initParam = GFL_HEAP_AllocClearMemory( HEAPID_HOSAKA_DEBUG, sizeof(COMM_BTL_DEMO_PARAM) );

  initParam->type = COMM_BTL_DEMO_TYPE_MULTI_START;
  debug_param(initParam);

  D_HOSAKA_COMMAND_CallProc(work,FS_OVERLAY_ID(comm_btl_demo), &CommBtlDemoProcData, initParam);

  return TRUE;
}


// �ȈՉ�b
#include "app/pms_select.h"
#include "app/pms_input.h"
FS_EXTERN_OVERLAY(pmsinput);

//#define TESTMODE_PMS_USE_PROC

#ifdef TESTMODE_PMS_USE_PROC
typedef struct
{
  HEAPID                parent_heap_id;
  LISTFUNC   func;
}
TESTMODE_PMS_PARAM;

typedef struct
{
  HEAPID                heap_id;
  GFL_PROCSYS*          local_procsys;
  void*                 local_proc_param;
}
TESTMODE_PMS_WORK;

static TESTMODE_PMS_PARAM*  TESTMODE_PMS_CreateParam( HEAPID heap_id, LISTFUNC func );
static void                 TESTMODE_PMS_DeleteParam( TESTMODE_PMS_PARAM* param );

static TESTMODE_PMS_PARAM*  TESTMODE_PMS_CreateParam( HEAPID heap_id, LISTFUNC func )
{
  TESTMODE_PMS_PARAM* param = GFL_HEAP_AllocClearMemory( heap_id, sizeof(TESTMODE_PMS_PARAM) );
  param->parent_heap_id   = heap_id;
  param->func             = func;
  return param;
}
static void                 TESTMODE_PMS_DeleteParam( TESTMODE_PMS_PARAM* param )
{
  GFL_HEAP_FreeMemory( param );
}

static GFL_PROC_RESULT TESTMODE_PMS_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT TESTMODE_PMS_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT TESTMODE_PMS_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
const GFL_PROC_DATA  TESTMODE_PMS_ProcData =
{
  TESTMODE_PMS_ProcInit,
  TESTMODE_PMS_ProcMain,
  TESTMODE_PMS_ProcExit,
};

static GFL_PROC_RESULT TESTMODE_PMS_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  TESTMODE_PMS_PARAM*  param    = (TESTMODE_PMS_PARAM*)pwk;
  TESTMODE_PMS_WORK*   work;
  
  GFL_HEAP_CreateHeap( param->parent_heap_id, HEAPID_UI_DEBUG, 0x10000 );
  work = GFL_PROC_AllocWork( proc, sizeof(TESTMODE_PMS_WORK), HEAPID_UI_DEBUG );
  
  work->heap_id = HEAPID_UI_DEBUG;
  work->local_procsys = GFL_PROC_LOCAL_boot( work->heap_id );
  
  return GFL_PROC_RES_FINISH;
}
static GFL_PROC_RESULT TESTMODE_PMS_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  TESTMODE_PMS_PARAM*  param    = (TESTMODE_PMS_PARAM*)pwk;
  TESTMODE_PMS_WORK*   work     = (TESTMODE_PMS_WORK*)mywk;

  GFL_PROC_LOCAL_Exit( work->local_procsys );

  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_UI_DEBUG );
  
  return GFL_PROC_RES_FINISH;
}
static GFL_PROC_RESULT TESTMODE_PMS_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  TESTMODE_PMS_PARAM*  param    = (TESTMODE_PMS_PARAM*)pwk;
  TESTMODE_PMS_WORK*   work     = (TESTMODE_PMS_WORK*)mywk;

  GFL_PROC_MAIN_STATUS  local_proc_status   =  GFL_PROC_LOCAL_Main( work->local_procsys );
  if( local_proc_status == GFL_PROC_MAIN_VALID ) return GFL_PROC_RES_CONTINUE;

  switch(*seq)
  {
  case 0:
    {
      if( param->func == TESTMODE_ITEM_SelectPmsSelect )
      {
        PMS_SELECT_PARAM* initParam = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(PMS_SELECT_PARAM) );
        initParam->save_ctrl = SaveControl_GetPointer();
        GFL_PROC_LOCAL_CallProc( work->local_procsys, FS_OVERLAY_ID(pmsinput), &ProcData_PMSSelect, initParam );
        work->local_proc_param = initParam;
      }
      else if( param->func == TESTMODE_ITEM_SelectPmsDirect )
      {
        PMS_DATA data;
        PMSI_PARAM* pmsi;
        PMSDAT_Init( &data, 0 );
        pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, NULL, TRUE, SaveControl_GetPointer(), work->heap_id );  // �f�R����OK
        //pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, NULL, FALSE, SaveControl_GetPointer(), work->heap_id );  // �f�R�����֎~
        GFL_PROC_LOCAL_CallProc( work->local_procsys, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );
        work->local_proc_param = pmsi;
      }
      else if( param->func == TESTMODE_ITEM_SelectPmsPeculiar )
      {
        PMS_DATA data;
        PMSI_PARAM* pmsi;
        PMSDAT_Init( &data, 0 );
        //pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, &data, TRUE, SaveControl_GetPointer(), work->heap_id );  // �f�R����OK
        pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, &data, FALSE, SaveControl_GetPointer(), work->heap_id );  // �f�R�����֎~
        GFL_PROC_LOCAL_CallProc( work->local_procsys, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );
        work->local_proc_param = pmsi;
      }
      else if( param->func == TESTMODE_ITEM_SelectPmsSingle )
      {
        PMS_DATA data;
        PMSI_PARAM* pmsi;
        PMSDAT_Init( &data, 0 );
        pmsi = PMSI_PARAM_Create( PMSI_MODE_SINGLE, PMSI_GUIDANCE_DEFAULT, NULL, FALSE, SaveControl_GetPointer(), work->heap_id );  // data�͑���Ȃ�
        GFL_PROC_LOCAL_CallProc( work->local_procsys, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );
        work->local_proc_param = pmsi;
      }
      else if( param->func == TESTMODE_ITEM_SelectPmsDouble )
      {
        PMS_DATA data;
        PMSI_PARAM* pmsi;
        PMSDAT_Init( &data, 0 );
        //pmsi = PMSI_PARAM_Create( PMSI_MODE_DOUBLE, PMSI_GUIDANCE_DEFAULT, &data, TRUE, SaveControl_GetPointer(), work->heap_id );  // data�𑗂��Ă݂�
        pmsi = PMSI_PARAM_Create( PMSI_MODE_DOUBLE, PMSI_GUIDANCE_DEFAULT, NULL, TRUE, SaveControl_GetPointer(), work->heap_id );  // �P�ꃂ�[�h�͕��͌Œ�ɖ��Ή��炵�� pmsi_param.c
        GFL_PROC_LOCAL_CallProc( work->local_procsys, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );
        work->local_proc_param = pmsi;
      }
      (*seq)++;
    }
    break;
  case 1:
    {
      if( local_proc_status != GFL_PROC_MAIN_VALID )
      {
        if( param->func == TESTMODE_ITEM_SelectPmsSelect )
        {
          PMS_SELECT_PARAM* initParam = (PMS_SELECT_PARAM*)work->local_proc_param;
          GFL_HEAP_FreeMemory( initParam );
        }
        else if( param->func == TESTMODE_ITEM_SelectPmsDirect )
        {
          PMSI_PARAM* pmsi = (PMSI_PARAM*)work->local_proc_param;
          PMSI_PARAM_Delete( pmsi );
        }
        else if( param->func == TESTMODE_ITEM_SelectPmsPeculiar )
        {
          PMSI_PARAM* pmsi = (PMSI_PARAM*)work->local_proc_param;
          PMSI_PARAM_Delete( pmsi );
        }
        else if( param->func == TESTMODE_ITEM_SelectPmsSingle )
        {
          PMSI_PARAM* pmsi = (PMSI_PARAM*)work->local_proc_param;
          PMSI_PARAM_Delete( pmsi );
        }
        else if( param->func == TESTMODE_ITEM_SelectPmsDouble )
        {
          PMSI_PARAM* pmsi = (PMSI_PARAM*)work->local_proc_param;
          PMSI_PARAM_Delete( pmsi );
        }
        (*seq)++;
      }
    }
    break;
  case 2:
    {
      return GFL_PROC_RES_FINISH;
    }
    break;
  }

  return GFL_PROC_RES_CONTINUE;
}
#endif  // #ifdef TESTMODE_PMS_USE_PROC

// �ȈՉ�b �I��
static BOOL TESTMODE_ITEM_SelectPmsSelect( D_HOSAKA_WORK *work , const int idx )
{
#ifndef TESTMODE_PMS_USE_PROC
  PMS_SELECT_PARAM* initParam = GFL_HEAP_AllocClearMemory( HEAPID_HOSAKA_DEBUG, sizeof(PMS_SELECT_PARAM) );

  initParam->save_ctrl = SaveControl_GetPointer();

  D_HOSAKA_COMMAND_CallProc(work,FS_OVERLAY_ID(pmsinput), &ProcData_PMSSelect, initParam);
  return TRUE;
#else
  TESTMODE_PMS_PARAM* param = TESTMODE_PMS_CreateParam( HEAPID_HOSAKA_DEBUG, TESTMODE_ITEM_SelectPmsSelect );
  D_HOSAKA_COMMAND_CallProc( work, FS_OVERLAY_ID(testmode), &TESTMODE_PMS_ProcData, param );
  return TRUE;
#endif
}


// �ȈՉ�b ���Ă�
static BOOL TESTMODE_ITEM_SelectPmsDirect( D_HOSAKA_WORK *work , const int idx )
{
#ifndef TESTMODE_PMS_USE_PROC
  PMS_DATA data;
  PMSI_PARAM* pmsi;

/*
  {
    PMSDAT_Init( &data, 0 );
    pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, NULL, TRUE, SaveControl_GetPointer(), HEAPID_HOSAKA_DEBUG );  // �f�R����OK
  }
*/
/*
  {
    PMSDAT_Init( &data, 0 );
    pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, NULL, FALSE, SaveControl_GetPointer(), HEAPID_HOSAKA_DEBUG );  // �f�R�����֎~
  }
*/
  {
    PMSDAT_Init( &data, PMS_TYPE_BATTLE_WON );  // �퓬�����̒�^���ŃX�^�[�g
    pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, NULL, FALSE, SaveControl_GetPointer(), HEAPID_HOSAKA_DEBUG );  // �f�R�����֎~
    PMSI_PARAM_SetInitializeDataSentence( pmsi, &data );  // �퓬�����̒�^���ŃX�^�[�g
  }

  D_HOSAKA_COMMAND_CallProc( work, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );

  return TRUE;
#else

  TESTMODE_PMS_PARAM* param = TESTMODE_PMS_CreateParam( HEAPID_HOSAKA_DEBUG, TESTMODE_ITEM_SelectPmsDirect );
  D_HOSAKA_COMMAND_CallProc( work, FS_OVERLAY_ID(testmode), &TESTMODE_PMS_ProcData, param );
  return TRUE;
#endif
}

// �ȈՉ�b �Œ�
#include "msg\msg_pmss_peculiar.h"
static BOOL TESTMODE_ITEM_SelectPmsPeculiar( D_HOSAKA_WORK *work , const int idx )
{
#ifndef TESTMODE_PMS_USE_PROC
  PMS_DATA data;
  PMSI_PARAM* pmsi;

  //PMSDAT_Init( &data, PMS_TYPE_PECULIAR );  // �ŗL��0�Ԗڂ̒�^���ŁA�^�O�͋�ŏ����������
  //PMSDAT_SetSentence( &data, PMS_TYPE_PECULIAR, pmss_peculiar_08 );  // �ŗL��7�Ԗڂ̒�^���ɂ���(�^�O�ɑ΂��Ă͉�������Ȃ�)
  PMSDAT_InitAndSetSentence( &data, PMS_TYPE_PECULIAR, pmss_peculiar_08 );  // �ŗL��7�Ԗڂ̒�^���ŁA�^�O�͋�ŏ���������
  pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, &data, TRUE, SaveControl_GetPointer(), HEAPID_HOSAKA_DEBUG );  // �f�R����OK
  //pmsi = PMSI_PARAM_Create( PMSI_MODE_SENTENCE, PMSI_GUIDANCE_DEFAULT, &data, FALSE, SaveControl_GetPointer(), HEAPID_HOSAKA_DEBUG );  // �f�R�����֎~

  D_HOSAKA_COMMAND_CallProc( work, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );

  return TRUE;
#else
  TESTMODE_PMS_PARAM* param = TESTMODE_PMS_CreateParam( HEAPID_HOSAKA_DEBUG, TESTMODE_ITEM_SelectPmsPeculiar );
  D_HOSAKA_COMMAND_CallProc( work, FS_OVERLAY_ID(testmode), &TESTMODE_PMS_ProcData, param );
  return TRUE;
#endif
}
  
// �ȈՉ�b ��P��
static BOOL TESTMODE_ITEM_SelectPmsSingle( D_HOSAKA_WORK *work , const int idx )
{
#ifndef TESTMODE_PMS_USE_PROC
  PMS_DATA data;
  PMSI_PARAM* pmsi;

  PMSDAT_Init( &data, 0 );
  pmsi = PMSI_PARAM_Create( PMSI_MODE_SINGLE, PMSI_GUIDANCE_DEFAULT, NULL, FALSE, SaveControl_GetPointer(), HEAPID_HOSAKA_DEBUG );  // data�͑���Ȃ�

  D_HOSAKA_COMMAND_CallProc( work, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );

  return TRUE;
#else

  TESTMODE_PMS_PARAM* param = TESTMODE_PMS_CreateParam( HEAPID_HOSAKA_DEBUG, TESTMODE_ITEM_SelectPmsSingle );
  D_HOSAKA_COMMAND_CallProc( work, FS_OVERLAY_ID(testmode), &TESTMODE_PMS_ProcData, param );
  return TRUE;
#endif
}

// �ȈՉ�b ��P��
static BOOL TESTMODE_ITEM_SelectPmsDouble( D_HOSAKA_WORK *work , const int idx )
{
#ifndef TESTMODE_PMS_USE_PROC
  PMS_DATA data;
  PMSI_PARAM* pmsi;

  PMSDAT_Init( &data, 0 );
  //pmsi = PMSI_PARAM_Create( PMSI_MODE_DOUBLE, PMSI_GUIDANCE_DEFAULT, &data, TRUE, SaveControl_GetPointer(), HEAPID_HOSAKA_DEBUG );  // data�𑗂��Ă݂�
  pmsi = PMSI_PARAM_Create( PMSI_MODE_DOUBLE, PMSI_GUIDANCE_DEFAULT, NULL, TRUE, SaveControl_GetPointer(), HEAPID_HOSAKA_DEBUG );  // �P�ꃂ�[�h�͕��͌Œ�ɖ��Ή��炵�� pmsi_param.c

  D_HOSAKA_COMMAND_CallProc( work, FS_OVERLAY_ID(pmsinput), &ProcData_PMSInput, pmsi );

  return TRUE;
#else
  TESTMODE_PMS_PARAM* param = TESTMODE_PMS_CreateParam( HEAPID_HOSAKA_DEBUG, TESTMODE_ITEM_SelectPmsDouble );
  D_HOSAKA_COMMAND_CallProc( work, FS_OVERLAY_ID(testmode), &TESTMODE_PMS_ProcData, param );
  return TRUE;
#endif
}


// �}�C�N�e�X�g
#include "app/mictest.h"
static BOOL TESTMODE_ITEM_SelectMicTest( D_HOSAKA_WORK *work , const int idx )
{
  D_HOSAKA_COMMAND_CallProc(work,FS_OVERLAY_ID(mictest), &TitleMicTestProcData, NULL );
  return TRUE;
}


#endif // PM_DEBUG
