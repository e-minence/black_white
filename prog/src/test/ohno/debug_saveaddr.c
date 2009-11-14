


//=============================================================================
/**
 * @file	  debug_saveaddr.c
 * @brief	  PDW�ŃA�N�Z�X���镨�̃A�h���X���X�g
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/04/30
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"
#include "arc_def.h"

#include "system/main.h"  //HEAPID
#include "message.naix"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "font/font.naix"
#include "print/str_tool.h"
#include "sound/pm_sndsys.h"
#include "system/wipe.h"
#include "net/network_define.h"
#include "savedata/wifilist.h"
#include "msg/msg_d_ohno.h"
#include "savedata/dreamworld_data.h"

typedef struct _SAVEADDR_WORK SAVEADDR_WORK;

typedef void (StateFunc)(SAVEADDR_WORK* pState);



static void _changeState(SAVEADDR_WORK* pWork,StateFunc* state);
static void _changeStateDebug(SAVEADDR_WORK* pWork,StateFunc* state, int line);

#define _NET_DEBUG (1)

#ifdef _NET_DEBUG
#define   _CHANGE_STATE(state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeState(pWork ,state)
#endif //_NET_DEBUG



struct _SAVEADDR_WORK {
	GFL_FONT		  *fontHandle;
	GFL_MSGDATA		*mm;
	GFL_BMPWIN		*win;
	GFL_BMP_DATA	*bmp;
	HEAPID heapID;
	STRBUF			*strbuf;
	STRBUF			*strbufEx;
  WORDSET			*WordSet;								// ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[

	SAVE_CONTROL_WORK* pSaveData;
	StateFunc* state;      ///< �n���h���̃v���O�������
	vu32 count;

	void* buffer;
	int bufferSize;
	int hz;
	int req;
	int getdataCount;
	BOOL bEnd;
//	DWCGHTTPPost post;
	u8 mac[6];
//	u8 buffer[0x102];
};



static void _msgPrint(SAVEADDR_WORK* pWork,int msg)
{
	GFL_BMP_Clear( pWork->bmp, 0xff );
	GFL_MSG_GetString(pWork->mm, msg, pWork->strbuf);
	PRINTSYS_Print( pWork->bmp, 0, 0, pWork->strbuf, pWork->fontHandle);
	GFL_BMPWIN_TransVramCharacter( pWork->win );
}

static void _msgPrintNo(SAVEADDR_WORK* pWork,int msg,int no)
{
	GFL_BMP_Clear( pWork->bmp, 0xff );
	GFL_MSG_GetString(pWork->mm, msg, pWork->strbufEx);

	WORDSET_RegisterNumber(pWork->WordSet, 0, no,
												 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
	WORDSET_ExpandStr( pWork->WordSet, pWork->strbuf,pWork->strbufEx );


	PRINTSYS_Print( pWork->bmp, 0, 0, pWork->strbuf, pWork->fontHandle);
	GFL_BMPWIN_TransVramCharacter( pWork->win );
}


//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _changeState(SAVEADDR_WORK* pWork,StateFunc state)
{
	pWork->state = state;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------
#ifdef _NET_DEBUG
static void _changeStateDebug(SAVEADDR_WORK* pWork,StateFunc state, int line)
{
#ifdef DEBUG_ONLY_FOR_ohno
	OS_TPrintf("ghttp: %d\n",line);
#endif
	_changeState(pWork, state);
}
#endif





//------------------------------------------------------------------------------
/**
 * @brief   �L�[���͂œ�����ς���
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _keyWait(SAVEADDR_WORK* pWork)
{
	switch(GFL_UI_KEY_GetTrg())
	{
	case PAD_BUTTON_X:
    {
      u32 size;
      u8* pAddr;
      u8* topAddr = (u8*)SaveControl_GetSaveWorkAdrs(pWork->pSaveData, &size);

      OS_TPrintf("SAVESIZE ,%x\n", size);
      
      {//DreamWorld�֘A
        DREAMWORLD_SAVEDATA* pDW = DREAMWORLD_SV_GetDreamWorldSaveData(pWork->pSaveData);
        pAddr = (u8*)DREAMWORLD_SV_GetSleepPokemon(pDW);

        OS_TPrintf("SLEEP POKEMON ,0x%x\n", (u32)pAddr-(u32)topAddr);
        

      }

      
    }
		break;
	case PAD_BUTTON_Y:
		break;
	case PAD_BUTTON_L:
		break;
	case PAD_BUTTON_B:
		_CHANGE_STATE(NULL);
		break;
	}
}




static const GFL_DISP_VRAM vramBank = {
		GX_VRAM_BG_128_A,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_0_F,			// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_0123_E,			// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_32K,	// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
};


static GFL_PROC_RESULT DebugSaveAddrProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PROC, 0x70000 );//�e�X�g

	{
		SAVEADDR_WORK* pWork = GFL_PROC_AllocWork( proc, sizeof(SAVEADDR_WORK), HEAPID_PROC );

		pWork->pSaveData = SaveControl_GetPointer();  //�f�o�b�O
		pWork->heapID = HEAPID_PROC;

		GFL_DISP_SetBank( &vramBank );

		//�o�b�N�O���E���h�̐F�����Ă���
		GFL_STD_MemFill16((void*)HW_BG_PLTT, 0x5ad6, 2);
	
		// �e����ʃ��W�X�^������
		G2_BlendNone();

		// BGsystem������
		GFL_BG_Init( pWork->heapID );
		GFL_BMPWIN_Init( pWork->heapID );
		GFL_FONTSYS_Init();

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

		pWork->win = GFL_BMPWIN_Create( GFL_BG_FRAME0_M, 4, 12, 24, 16, 0, GFL_BMP_CHRAREA_GET_F );
		pWork->bmp = GFL_BMPWIN_GetBmp( pWork->win);
		GFL_BMP_Clear( pWork->bmp, 0xff );
		GFL_BMPWIN_MakeScreen( pWork->win );
		GFL_BMPWIN_TransVramCharacter( pWork->win );

		GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );

		pWork->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
																			GFL_FONT_LOADTYPE_FILE, FALSE, pWork->heapID );

		pWork->mm = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_d_ohno_dat, pWork->heapID );
		pWork->strbuf = GFL_STR_CreateBuffer(64, pWork->heapID);
		pWork->strbufEx = GFL_STR_CreateBuffer(64, pWork->heapID);
		pWork->WordSet    = WORDSET_Create( pWork->heapID );

	//�t�H���g�p���b�g�]��
		GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
																	0, 0x20, HEAPID_PROC);

		
		_CHANGE_STATE(_keyWait);
	}
  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT DebugSaveAddrProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	SAVEADDR_WORK* pWork = mywk;
	StateFunc* state = pWork->state;

	if( state ){
		state( pWork );
		return GFL_PROC_RES_CONTINUE;
	}
	return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT DebugSaveAddrProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	SAVEADDR_WORK* pWork = mywk;

	GFL_STR_DeleteBuffer(pWork->strbuf);
	GFL_STR_DeleteBuffer(pWork->strbufEx);
	GFL_MSG_Delete(pWork->mm);
	GFL_FONT_Delete(pWork->fontHandle);

	WORDSET_Delete( pWork->WordSet );

	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap(HEAPID_PROC);  //�e�X�g
  return GFL_PROC_RES_FINISH;
}

// �v���Z�X��`�f�[�^
const GFL_PROC_DATA DebugSaveAddrProcData = {
  DebugSaveAddrProc_Init,
  DebugSaveAddrProc_Main,
  DebugSaveAddrProc_End,
};

