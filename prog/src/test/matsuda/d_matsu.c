//==============================================================================
/**
 * @file	d_matsu.c
 * @brief	���c�f�o�b�O�\�[�X
 * @author	matsuda
 * @date	2008.08.26(��)
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>
#include "system\main.h"
#include <backup_system.h>
//#include "savedata/contest_savedata.h"
#include "savedata/save_tbl.h"
#include "savedata/save_control.h"


//==============================================================================
//	�\���̒�`
//==============================================================================
typedef struct {

	u16		seq;
	HEAPID	heapID;
	int debug_mode;
	
	//�Z�[�u�֘A
	GFL_SAVEDATA *sv_normal;	///<�m�[�}���Z�[�u�f�[�^�ւ̃|�C���^
}D_MATSU_WORK;


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static BOOL DebugMatsuda_SaveSystemTest(D_MATSU_WORK *wk);
static BOOL DebugMatsuda_SaveTest(D_MATSU_WORK *wk);



//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   proc		
 * @param   seq		
 * @param   pwk		
 * @param   mywk		
 *
 * @retval  
 */
//--------------------------------------------------------------
static GFL_PROC_RESULT DebugMatsudaMainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	static const GFL_DISP_VRAM vramBank = {
		GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_0_F,			// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_128_D,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_0123_E,			// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_128K,	// �T�uOBJ�}�b�s���O���[�h
	};

	D_MATSU_WORK* wk;

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MATSUDA_DEBUG, 0x70000 );
	wk = GFL_PROC_AllocWork( proc, sizeof(D_MATSU_WORK), HEAPID_MATSUDA_DEBUG );
	MI_CpuClear8(wk, sizeof(D_MATSU_WORK));
	wk->heapID = HEAPID_MATSUDA_DEBUG;

//	GFL_ARC_Init( arcFiles, NELEMS(arcFiles) );	gfl_use.c��1�񂾂��������ɕύX

	GFL_DISP_SetBank( &vramBank );

	// �e����ʃ��W�X�^������
	G2_BlendNone();

	// BGsystem������
	GFL_BG_Init( wk->heapID );
	GFL_BMPWIN_Init( wk->heapID );

	//�a�f���[�h�ݒ�
	{
		static const GFL_BG_SYS_HEADER sysHeader = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysHeader );
	}

	// �ʃt���[���ݒ�
	{
		static const GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x5800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};

		GFL_BG_SetBGControl( GFL_BG_FRAME0_M,   &bgcntText,   GFL_BG_MODE_TEXT );
		GFL_BG_SetBGControl( GFL_BG_FRAME0_S,   &bgcntText,   GFL_BG_MODE_TEXT );

		GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_ON );
		GFL_BG_SetVisible( GFL_BG_FRAME1_M,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME2_M,   VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME3_M,   VISIBLE_OFF );

//		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 0x20, 0x22, wk->heapID );
		GFL_BG_FillCharacter( GFL_BG_FRAME0_M, 0xff, 1, 0 );

//		void GFL_BG_FillScreen( u8 frmnum, u16 dat, u8 px, u8 py, u8 sx, u8 sy, u8 mode )
		GFL_BG_FillScreen( GFL_BG_FRAME0_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

		GFL_BG_FillCharacter( GFL_BG_FRAME0_S, 0x00, 1, 0 );
		GFL_BG_FillScreen( GFL_BG_FRAME0_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );
	}

	// �㉺��ʐݒ�
	GX_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );

	GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );

	return GFL_PROC_RES_FINISH;
}
//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugMatsudaMainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	D_MATSU_WORK* wk = mywk;
	BOOL ret = 0;
	
	wk->debug_mode = 1;
	switch(wk->debug_mode){
	case 0:		//�Z�[�u�V�X�e���̍쐬�e�X�g
		ret = DebugMatsuda_SaveSystemTest(wk);
		break;
	case 1:		//���C���ō쐬����Ă���Z�[�u�V�X�e�����g�p���ăZ�[�u�e�X�g
		ret = DebugMatsuda_SaveTest(wk);
		break;
	default:
		ret = TRUE;
	}
	
	if(ret == TRUE){
		return GFL_PROC_RES_FINISH;
	}

	return GFL_PROC_RES_CONTINUE;
}
//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugMatsudaMainProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_BG_Exit();
	GFL_BMPWIN_Exit();

	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap(HEAPID_MATSUDA_DEBUG);
	
	return GFL_PROC_RES_FINISH;
}



//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �Z�[�u�e�X�g(�Z�[�u�V�X�e�����̂��̂���鏊����̃e�X�g)
 *
 * @param   wk		
 *
 * @retval  TRUE:�I���B�@FALSE:�����p����
 */
//--------------------------------------------------------------
static BOOL DebugMatsuda_SaveSystemTest(D_MATSU_WORK *wk)
{
#if 0
	LOAD_RESULT load_ret;
	SAVE_RESULT save_ret;
	
	GF_ASSERT(wk);

	switch( wk->seq ){
	case 0:
		//�m�[�}���Z�[�u�̈�쐬
		OS_TPrintf("�m�[�}���Z�[�u�̈�쐬\n");
		wk->sv_normal = GFL_SAVEDATA_Create(&SaveParam_Normal, GFL_HEAPID_APP);
		wk->seq++;
		break;
	case 1:
		//�R���e�X�g�f�[�^�����������Ă��Ē��g��\��
		load_ret = GFL_BACKUP_Load(wk->sv_normal, GFL_HEAPID_APP);
		switch(load_ret){
		case LOAD_RESULT_NULL:		///<�f�[�^�Ȃ�
			OS_TPrintf("LOAD:�f�[�^�����݂��Ȃ�\n");
			GFL_SAVEDATA_Clear(wk->sv_normal);
			break;
		case LOAD_RESULT_OK:				///<�f�[�^����ǂݍ���
			OS_TPrintf("LOAD:����ǂݍ���\n");
			{
				CONTEST_DATA *condata;
				u16 value;
				
				condata = GFL_SAVEDATA_Get(wk->sv_normal, GMDATA_ID_CONTEST);
				value = CONDATA_GetValue(condata, 0,0);
				OS_TPrintf("value = %d\n", value);
			}
			break;
		case LOAD_RESULT_NG:				///<�f�[�^�ُ�
			OS_TPrintf("LOAD:�f�[�^�ُ�\n");
			GFL_SAVEDATA_Clear(wk->sv_normal);
			break;
		case LOAD_RESULT_BREAK:			///<�j��A�����s�\
			OS_TPrintf("LOAD:�f�[�^�j��\n");
			GFL_SAVEDATA_Clear(wk->sv_normal);
			break;
		case LOAD_RESULT_ERROR:			///<�@��̏�Ȃǂœǂݎ��s�\
			OS_TPrintf("LOAD:�ǂݎ��s�\\n");
			GFL_SAVEDATA_Clear(wk->sv_normal);
			break;
		default:
			GF_ASSERT("LOAD:��O�G���[�I");
			break;
		}
		wk->seq++;
		break;
	case 2:
		//�R���e�X�g�f�[�^�̒��g��ύX���ăZ�[�u���s
		//����N���������ɒ��g���ς���Ă��邩�m�F�I
		{
			CONTEST_DATA *condata;
			u16 value;
			
			condata = GFL_SAVEDATA_Get(wk->sv_normal, GMDATA_ID_CONTEST);
			value = CONDATA_GetValue(condata, 0,0);
			CONDATA_RecordAdd(condata, 0, 0);
			OS_TPrintf("�Z�[�u���s\n");
			OS_TPrintf("before value = %d, after value = %d\n", value, value+1);
			save_ret = GFL_BACKUP_Save(wk->sv_normal);
			switch(save_ret){
			case SAVE_RESULT_CONTINUE:		///<�Z�[�u�����p����
				OS_TPrintf("SAVE:�p����\n");
				break;
			case SAVE_RESULT_LAST:			///<�Z�[�u�����p�����A�Ō�̈�O
				OS_TPrintf("SAVE:�p���� �Ō��1�O\n");
				break;
			case SAVE_RESULT_OK:			///<�Z�[�u����I��
				OS_TPrintf("SAVE:����I��\n");
				break;
			case SAVE_RESULT_NG:			///<�Z�[�u���s�I��
				OS_TPrintf("SAVE:���s\n");
				break;
			}
		}
		wk->seq++;
		break;
	case 3:
		OS_TPrintf("�Z�[�u�V�X�e���j��\n");
		GFL_SAVEDATA_Delete(wk->sv_normal);
		
		return TRUE;
	}
#endif
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �Z�[�u�e�X�g(�Z�[�u�V�X�e���̓��C���ō���Ă�����̂��g�p)
 *
 * @param   wk		
 *
 * @retval  TRUE:�I���B�@FALSE:�����p����
 */
//--------------------------------------------------------------
static BOOL DebugMatsuda_SaveTest(D_MATSU_WORK *wk)
{
	return FALSE;
}


//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA DebugMatsudaMainProcData = {
	DebugMatsudaMainProcInit,
	DebugMatsudaMainProcMain,
	DebugMatsudaMainProcEnd,
};


