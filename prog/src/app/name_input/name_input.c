//======================================================================
/**
 * @file	name_input.c
 * @brief	���O����
 * @author	ariizumi
 * @data	09/01/08
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "buflen.h"
#include "app/name_input.h"
#include "savedata/save_control.h"
#include "savedata/mystatus.h"
#include "title/game_start.h"
#include "title/startmenu.h"
#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#define BG_PLANE_SKB GFL_BG_FRAME1_S

//======================================================================
//	enum
//======================================================================
typedef enum
{
	NIS_FADE_IN,
	NIS_INPUT_NAME,
	NIS_FADE_OUT,
}NAME_INPUT_STATE;


//======================================================================
//	typedef struct
//======================================================================
typedef struct
{
	HEAPID	heapId_;
	
	NAME_INPUT_STATE state_;
	int		inputMode_;
	
	GFL_SKB	*gflSkb_;
	
}NAME_INPUT_WORK;


//======================================================================
//	proto
//======================================================================

static void	NAME_INPUT_InitGraphic( NAME_INPUT_WORK *work );
static void	NAME_INPUT_InitBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane );

//Proc�f�[�^
static GFL_PROC_RESULT NAME_INPUT_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT NAME_INPUT_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT NAME_INPUT_ProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

const GFL_PROC_DATA NameInputProcData = {
	NAME_INPUT_ProcInit,
	NAME_INPUT_ProcMain,
	NAME_INPUT_ProcEnd,
};

//--------------------------------------------------------------
static GFL_PROC_RESULT NAME_INPUT_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	static GFL_SKB_SETUP skbData= {
		GFL_SKB_STRLEN_MAX, GFL_SKB_STRTYPE_STRBUF,
		GFL_SKB_MODE_HIRAGANA, TRUE, 0,
		GFL_DISPUT_BGID_S1, GFL_DISPUT_PALID_14, GFL_DISPUT_PALID_15,
	};

	NAMEIN_PARAM *param = pwk;
	NAME_INPUT_WORK *work;

		//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_NAME_INPUT, 0x80000 );
	work = GFL_PROC_AllocWork( proc, sizeof(NAME_INPUT_WORK), HEAPID_NAME_INPUT );
	GFL_STD_MemClear(work, sizeof(NAME_INPUT_WORK));

	work->heapId_ = HEAPID_NAME_INPUT;
	work->inputMode_ = param->mode;
	GF_ASSERT_MSG( work->inputMode_ < NAMEIN_MAX , "Invalid input type [%d]\n",work->inputMode_);
	work->state_ = NIS_FADE_IN;
	
	NAME_INPUT_InitGraphic(work);

	//SKB�̏�����
	skbData.strlen = param->wordmax;
	
	work->gflSkb_ = GFL_SKB_Create( param->strbuf, &skbData, work->heapId_ );
	
	GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB , 16 , 0 , ARI_FADE_SPD );
	//�w�i�F
	*((u16 *)HW_DB_BG_PLTT) = 0x7d8c;//RGB(12, 12, 31);
	
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
static GFL_PROC_RESULT NAME_INPUT_ProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	NAMEIN_PARAM *param = pwk;
	NAME_INPUT_WORK *work = mywk;
	
	//TODO �{���͍ŏ��Ɠ������ǂ����̃`�F�b�N���v��
	if( GFL_STR_GetBufferLength( param->strbuf ) == 0 )
	{
		param->cancel = 0;
	}
	else
	{
		param->cancel = 1;
	}
	
	GFL_SKB_Delete( work->gflSkb_ );

	GFL_BMPWIN_Exit();

	GFL_BG_FreeBGControl(BG_PLANE_SKB);
	GFL_BG_Exit();
	
	GFL_PROC_FreeWork( proc );

	GFL_HEAP_DeleteHeap( HEAPID_NAME_INPUT );

	//Proc�̕ύX�𒆂ł���Ă�
//	GameStart_Beginning();
	//���͂���Ă邩�m�F�p�ɃX�^�[�g���j���[��
//	GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &StartMenuProcData, NULL);
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
static GFL_PROC_RESULT NAME_INPUT_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	NAME_INPUT_WORK *work = mywk;

	switch( work->state_ )
	{
	case NIS_FADE_IN:
		if( GFL_FADE_CheckFade() == FALSE )
		{
			work->state_ = NIS_INPUT_NAME;
		}
		break;
		
	case NIS_INPUT_NAME:
		{
			const BOOL ret = GFL_SKB_Main( work->gflSkb_ );
			if( ret == FALSE )
			{
				work->state_ = NIS_FADE_OUT;
				GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB , 0 , 16 , ARI_FADE_SPD );
			}
		}
		break;
		
	case NIS_FADE_OUT:
		if( GFL_FADE_CheckFade() == FALSE )
		{
			return GFL_PROC_RES_FINISH;
		}
		break;
	}
	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------
//	�`����菉����
//--------------------------------------------------------------------------
static void	NAME_INPUT_InitGraphic( NAME_INPUT_WORK *work )
{
	static const GFL_DISP_VRAM vramBank = {
		GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_128_D,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_NONE,			// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_32K,		// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
	};

	static const GFL_BG_SYS_HEADER sysHeader = {
		GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
	};
	
	static const GFL_BG_BGCNT_HEADER bgCont_Skb = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	};

	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	GX_SetMasterBrightness(-16);	
	GXS_SetMasterBrightness(-16);
	GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
	GFL_DISP_GXS_SetVisibleControlDirect(0);

	GFL_DISP_SetBank( &vramBank );
	GFL_BG_Init( work->heapId_ );
	GFL_BG_SetBGMode( &sysHeader );	

	NAME_INPUT_InitBgFunc( &bgCont_Skb , BG_PLANE_SKB );
	
	GFL_BMPWIN_Init( work->heapId_ );
	
}

//--------------------------------------------------------------------------
//	Bg������ �@�\��
//--------------------------------------------------------------------------
static void	NAME_INPUT_InitBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane )
{
	GFL_BG_SetBGControl( bgPlane, bgCont, GFL_BG_MODE_TEXT );
	GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
	GFL_BG_ClearFrame( bgPlane );
	GFL_BG_LoadScreenReq( bgPlane );
}


//wifi_note�̈ڐA�ŕK�v����������
//���[�N�̏������E�J�������ڐA
//==============================================================================
/**
 * ���O���͂ɓn���p�����[�^���m�ۂ���i���O���͂̌��ʂ�����̂ŁA�Ăяo������HEAPID���K�v�j
 *
 * @param   HeapId		�Ăяo������HEAPID
 * @param   mode		���O���̓��[�h(NAMEIN_MYNAME,NAMEIN_BOX,NAMEIN_POKEMON)
 * @param   info		NAMEIN_MYNAME�̎���0=�j1=��, NAMEIN_POKEMON�̎��͊J��NO
 * @param   wordmax		���͕����ő吔�̎w��
 *
 * @retval  NAMEIN_PARAM *		�m�ۂ��ꂽ���O���̓p�����[�^�̃|�C���^
 */
//==============================================================================
NAMEIN_PARAM *NameIn_ParamAllocMake(int heapId, int mode, int info, int wordmax, CONFIG *config )
{
	NAMEIN_PARAM *param;
	
	param = (NAMEIN_PARAM*)GFL_HEAP_AllocMemory(heapId, sizeof(NAMEIN_PARAM));

	param->mode    = mode;		// ���̓��[�h�i�����̖��O�A�|�P�����A�{�b�N�X�j
	param->info    = info;		// �i�j�E���A�|�P�����̊J��NO)
	param->wordmax = wordmax;	// ���͕����ő吔
	param->cancel  = 0;			// �L�����Z���t���O
#ifdef USE_PARAM_STR
	param->str[0]  = EOM_;
#endif

	// ������ő咷�{EOM�ŕ�����o�b�t�@���쐬
	param->strbuf = GFL_STR_CreateBuffer( wordmax+1, heapId );


	// �|�P�����ߊl�̎��Ƀ{�b�N�X�]���������������ɂ͎g�p����郁���o
	param->get_msg_id  = 0;		// �|�P�����ߊl���b�Z�[�WID
	param->boxdata     = NULL;	// �|�P�����{�b�N�X�f�[�^�ւ̃|�C���^
	param->sex		   = 0;
	param->config      = config;
	param->form		   = 0;

	return param;
}

//==============================================================================
/**
 * ���O���̓p�����[�^�̉���i���O���͌�Ƀ��[�U�[�����O�擾�����������Ă��炤�j
 *
 * @param   param		
 *
 * @retval  none		
 */
//==============================================================================
void NameIn_ParamDelete(NAMEIN_PARAM *param)
{
	GF_ASSERT((param->strbuf)!=NULL);
	GF_ASSERT((param)!=NULL);
	
	GFL_STR_DeleteBuffer(param->strbuf);
	GFL_HEAP_FreeMemory(param);
}
