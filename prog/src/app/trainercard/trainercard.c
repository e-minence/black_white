//======================================================================
/**
 * @file	trainercard.c
 * @brief	�g���[�i�[�J�[�h��
 * @author	ariizumi
 * @data	08/11/25
 */
//======================================================================
#include <gflib.h>
#include <textprint.h>
#include "system/gfl_use.h"
#include "procsys.h"
#include "system/main.h"
#include "gamesystem/game_init.h"

#include "trainercard.h"
#include "test/ariizumi/ari_debug.h"
#include "print/printsys.h"
#include "field/field_comm/field_comm_data.h"
#include "savedata/save_control.h"
#include "savedata/mystatus.h"

#include "arc_def.h"
#include "test_graphic/d_taya.naix"
#include "test_graphic/wb_trainercard.naix"
//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
//���W�e�[�u��(�E���ɗ��镪�͉E��̍��W���w��
//X���W�EY���W�E�E�񂹃t���O
//�L�����P��(8)�̐��l�Ŕ������͌��
static const u8 CARD_MESSAGE_POSITION[CMI_MAX][3] ={
	{8*2 ,8* 4+2,0},
	{8*19,8* 4+2,1},
	{8*2 ,8* 6+2,0},
	{8*19,8* 6+2,1},
	{8*2 ,8* 9+2,0},
	{8*17,8* 9+2,1},
	{8*2 ,8*12+2,0},
	{8*16,8*12+2,1},
	{8*2 ,8*15+2,0},
	{8*19,8*15+2,1},
	{8*2 ,8*18+2,0},
	{8*30,8*18+2,1},
	{8*2 ,8*20+2,0},
	{8*23,8*20+2,1},
	{8*26,8*20+2,1},
	{8*29,8*20+2,1},
};
//======================================================================
//	typedef struct
//======================================================================
struct _TRAINER_CARD_WORK
{
	HEAPID heapID_;
	u8	seq_;

	GFL_BMPWIN	*bmpWinFont_;
	PRINT_QUE	*printQue_;		
	PRINT_UTIL	printUtil_[1];
	GFL_FONT	*fontHandle_;
};

//======================================================================
//	proto
//======================================================================
TRAINER_CARD_WORK* TRAINER_CARD_Init( HEAPID heapID );
void TRAINER_CARD_Term( TRAINER_CARD_WORK *work );
const BOOL TRAINER_CARD_Loop( TRAINER_CARD_WORK *work );
static void TRAINER_CARD_InitGraphic( TRAINER_CARD_WORK *work );
static void TRAINER_CARD_TermGraphic( TRAINER_CARD_WORK *work );
static void TRAINER_CARD_DrawString( STRBUF* str , u16 posX , u16 posY , BOOL isRight , TRAINER_CARD_WORK *work );
static void TRAINER_CARD_GetCardString_Partner( STRBUF* str , const u8 idx , TRAINER_CARD_WORK *work );
//--------------------------------------------------------------
//	
//--------------------------------------------------------------
TRAINER_CARD_WORK* TRAINER_CARD_Init( HEAPID heapID )
{
	TRAINER_CARD_WORK *work;
	work = GFL_HEAP_AllocMemory( heapID , sizeof( TRAINER_CARD_WORK ) );
	work->heapID_ = heapID;
	work->seq_ = 0;

	TRAINER_CARD_InitGraphic( work );
	return work;
}

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
void TRAINER_CARD_Term( TRAINER_CARD_WORK *work )
{
	TRAINER_CARD_TermGraphic( work );
	GFL_HEAP_FreeMemory( work );
}

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
const BOOL TRAINER_CARD_Loop( TRAINER_CARD_WORK *work )
{
	PRINTSYS_QUE_Main( work->printQue_ );
	if( PRINT_UTIL_Trans( work->printUtil_ , work->printQue_ ) == FALSE )
	{}
	switch( work->seq_ )
	{
	case 0:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB ,
									 16 ,0 ,ARI_FADE_SPD );
		work->seq_++;
		break;
	case 1:
		if( GFL_FADE_CheckFade() == FALSE )
		{
			work->seq_++;
		}
		break;
	case 2:
		if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
		{
			GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB ,
										 0 ,16 ,ARI_FADE_SPD );
			work->seq_++;
		}
		break;
	case 3:
		if( GFL_FADE_CheckFade() == FALSE )
		{
			return TRUE;
		}
		break;

	}
	return FALSE;
}

//--------------------------------------------------------------
//	�`���菉�����p��`
//--------------------------------------------------------------
#define ACC_BGPLANE_FONT (GFL_BG_FRAME0_M)
#define ACC_BGPLANE_CARD_BASE (GFL_BG_FRAME2_M)
#define ACC_BGPLANE_CARD_CHARA (GFL_BG_FRAME3_M)

#define ACC_BGPLANE_CARD_DOWN (GFL_BG_FRAME3_S)
static const GFL_BG_SYS_HEADER bgsysHeader = {
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_1,GX_BG0_AS_2D 
};	
static const GFL_BG_BGCNT_HEADER bgCont_CardBase = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x00000, 0x2000 ,
	GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
};
static const GFL_BG_BGCNT_HEADER bgCont_CardChara = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x10000, 0x2000 ,
	GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
};
static const GFL_BG_BGCNT_HEADER bgCont_Font = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x2800, GX_BG_CHARBASE_0x08000, 0x8000,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
};

static const GFL_BG_BGCNT_HEADER bgCont_CardDown = {
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
	GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x08000, 0x8000 ,
	GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
};

//--------------------------------------------------------------
//	�`���菉����
//--------------------------------------------------------------
static void TRAINER_CARD_InitGraphic( TRAINER_CARD_WORK *work )
{
	u8 i;
	//VRAM�n������
	//BG�V�X�e���N�� 
	GFL_BG_Init( work->heapID_ );

	//VRAM�ݒ�
	GX_SetBankForBG( GX_VRAM_BG_128_A );
	GX_SetBankForSubBG( GX_VRAM_BG_128_C );

	//BG���[�h�ݒ�
	GFL_BG_SetBGMode( &bgsysHeader );

	//�a�f�R���g���[���ݒ�
	GFL_BG_SetBGControl( ACC_BGPLANE_FONT, &bgCont_Font, GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( ACC_BGPLANE_CARD_BASE, &bgCont_CardBase, GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( ACC_BGPLANE_CARD_CHARA, &bgCont_CardChara, GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( ACC_BGPLANE_CARD_DOWN, &bgCont_CardDown, GFL_BG_MODE_AFFINE );
	GFL_BG_SetVisible( ACC_BGPLANE_FONT, VISIBLE_ON );
	GFL_BG_SetVisible( ACC_BGPLANE_CARD_BASE, VISIBLE_ON );
	GFL_BG_SetVisible( ACC_BGPLANE_CARD_CHARA, VISIBLE_ON );
	GFL_BG_SetVisible( ACC_BGPLANE_CARD_DOWN, VISIBLE_ON );

	//Font��Bg�̃N���A 
	GFL_BG_FillCharacter( ACC_BGPLANE_FONT, 0x00, 1, 0 );
	GFL_BG_FillScreen( ACC_BGPLANE_FONT ,0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_LoadScreenReq( ACC_BGPLANE_FONT );
	
	//BG�ǂݍ��݊J�n
	//�w�i
	GFL_ARC_UTIL_TransVramBgCharacter( ARCID_WB_TRAINERCARD , NARC_wb_trainercard_card_NCGR ,
			ACC_BGPLANE_CARD_BASE , 0 , 0 , FALSE , work->heapID_ );
	GFL_ARC_UTIL_TransVramScreen( ARCID_WB_TRAINERCARD , NARC_wb_trainercard_card_faca_NSCR ,
			ACC_BGPLANE_CARD_BASE , 0 , 0 , FALSE , work->heapID_ );
	GFL_ARC_UTIL_TransVramPalette( ARCID_WB_TRAINERCARD , NARC_wb_trainercard_card_0_NCLR ,
			PALTYPE_MAIN_BG , 0 , 0 , work->heapID_ );
	//�L����
	GFL_ARC_UTIL_TransVramBgCharacter( ARCID_WB_TRAINERCARD , NARC_wb_trainercard_card_trainer_NCGR ,
			ACC_BGPLANE_CARD_CHARA , 0 , 0 , FALSE , work->heapID_ );
	GFL_ARC_UTIL_TransVramScreen( ARCID_WB_TRAINERCARD , NARC_wb_trainercard_card_trainer01_NSCR ,
			ACC_BGPLANE_CARD_CHARA , 0 , 0 , FALSE , work->heapID_ );

	//�w�i
	GFL_ARC_UTIL_TransVramBgCharacter( ARCID_WB_TRAINERCARD , NARC_wb_trainercard_card_futa_NCGR ,
			ACC_BGPLANE_CARD_DOWN , 0 , 0 , FALSE , work->heapID_ );
	GFL_ARC_UTIL_TransVramScreen( ARCID_WB_TRAINERCARD , NARC_wb_trainercard_card_futa_NSCR ,
			ACC_BGPLANE_CARD_DOWN , 0 , 0 , FALSE , work->heapID_ );
	GFL_ARC_UTIL_TransVramPalette( ARCID_WB_TRAINERCARD , NARC_wb_trainercard_card_futa_NCLR ,
			PALTYPE_SUB_BG , 0 , 0 , work->heapID_ );

	//�r�b�g�}�b�v�E�C���h�E�V�X�e���̋N��
	GFL_BMPWIN_Init( work->heapID_ );
	
	//�t�H���g�pBMPWIN�n
	work->bmpWinFont_= GFL_BMPWIN_Create( ACC_BGPLANE_FONT ,0, 0,32,24,0,GFL_BG_CHRAREA_GET_F );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->bmpWinFont_ ), 0x0000 );
	GFL_BMPWIN_MakeScreen( work->bmpWinFont_ );
	GFL_BMPWIN_TransVramCharacter( work->bmpWinFont_ );
	GFL_BG_LoadScreenReq( ACC_BGPLANE_FONT );

	//�t�H���g�ǂݍ���
	work->fontHandle_ = GFL_FONT_Create( ARCID_D_TAYA , 
					NARC_d_taya_lc12_2bit_nftr , GFL_FONT_LOADTYPE_FILE ,
					FALSE , work->heapID_ );
	GFL_ARC_UTIL_TransVramPalette( ARCID_D_TAYA , NARC_d_taya_default_nclr ,
					PALTYPE_MAIN_BG, 0xf*32, 32, work->heapID_ );
	
	work->printQue_ = PRINTSYS_QUE_Create( work->heapID_ );
	PRINT_UTIL_Setup( work->printUtil_ , work->bmpWinFont_ );
	
	GFL_FONTSYS_Init();
	GFL_FONTSYS_SetColor( 0x1,0x9,0 );

	//������\���e�X�g
	{
		BOOL isCommMode = FALSE;
		u8 i;
		if( FIELD_COMM_DATA_GetUserDataType() == FCUT_TRAINERCARD )
		{
			isCommMode = TRUE;
		}
		for( i=0;i<CMI_MAX;i++ )
		{
			STRBUF *str;
			const BOOL isRight = (CARD_MESSAGE_POSITION[i][2]==0?FALSE:TRUE);

			str = GFL_STR_CreateBuffer( 128 , work->heapID_ );
			if( isCommMode == TRUE )
			{
				TRAINER_CARD_GetCardString_Partner( str , i , work );
			}
			else
			{
				TRAINER_CARD_GetCardString( str , i , work );
			}
			TRAINER_CARD_DrawString( str , CARD_MESSAGE_POSITION[i][0] , CARD_MESSAGE_POSITION[i][1], isRight , work );
			
			GFL_STR_DeleteBuffer( str );
		}
	}

	//�������S�`��
	while( PRINTSYS_QUE_IsFinished(work->printQue_) == FALSE )
	{
		PRINTSYS_QUE_Main( work->printQue_ );
		if( PRINT_UTIL_Trans( work->printUtil_ , work->printQue_ ) == FALSE )
		{}
	}
}

//--------------------------------------------------------------
//	�`����J��
//--------------------------------------------------------------
static void TRAINER_CARD_TermGraphic( TRAINER_CARD_WORK *work )
{
	u8 i;
	GFL_FONT_Delete( work->fontHandle_ );
	PRINTSYS_QUE_Delete( work->printQue_ );
	GFL_BMPWIN_Delete( work->bmpWinFont_ );
	
	GFL_BMPWIN_Exit();
	GFL_BG_FreeBGControl( ACC_BGPLANE_FONT );
	GFL_BG_FreeBGControl( ACC_BGPLANE_CARD_BASE );
	GFL_BG_FreeBGControl( ACC_BGPLANE_CARD_CHARA );
	GFL_BG_FreeBGControl( ACC_BGPLANE_CARD_DOWN );
	GFL_BG_Exit();

}

//--------------------------------------------------------------
//	������`��
//--------------------------------------------------------------
static void TRAINER_CARD_DrawString( STRBUF* str , u16 posX , u16 posY , BOOL isRight , TRAINER_CARD_WORK *work )
{
	int	workPosX = posX;
	
	if( isRight == TRUE )
	{
		workPosX -= PRINTSYS_GetStrWidth( str , work->fontHandle_ , 1 );
	}

	PRINT_UTIL_Print( work->printUtil_ , work->printQue_ , workPosX , posY ,
					(void*)str , work->fontHandle_ );

}
 
//--------------------------------------------------------------
//	������擾(����
//--------------------------------------------------------------
void TRAINER_CARD_GetCardString( STRBUF* str , const u8 idx , TRAINER_CARD_WORK *work )
{
	SAVE_CONTROL_WORK *saveWork = SaveControl_GetPointer();
	
	switch( idx )
	{
	case CMI_ID_TITLE:		//ID
		GFL_STR_SetStringCodeOrderLength( str , L"IDNo.\0" , 6 );
//		STD_CopyString( str , "IDNo." );
		break;
	case CMI_ID_USER:
		{
			GFL_STR_SetStringCodeOrderLength( str , L"65535\0" , 6 );
		}
		break;
	case CMI_NAME_TITLE:	//���O
		GFL_STR_SetStringCodeOrderLength( str , L"�Ȃ܂�\0" , 4 );
//		STD_CopyString( str , "�Ȃ܂�" );
		break;
	case CMI_NAME_USER:
		{
			MYSTATUS *mystatus = SaveData_GetMyStatus( saveWork );
			if( MyStatus_CheckNameClear( mystatus ) == FALSE )
			{
				MyStatus_CopyNameString( mystatus , str );
			}
			else
			{
				//TODO �O�̂��ߖ��O�������ĂȂ��Ƃ��ɗ����Ȃ��悤�ɂ��Ă���
				GFL_STR_SetStringCodeOrderLength( str , L"NoName\0" , 6 );
			}
		}
		break;
	case CMI_MONEY_TITLE:	//������
		GFL_STR_SetStringCodeOrderLength( str , L"�����Â���\0" , 6 );
//		STD_CopyString( str , "�����Â���" );
		break;
	case CMI_MONEY_USER:
		GFL_STR_SetStringCodeOrderLength( str , L"12980\0" , 6 );
//		STD_CopyString( str , "12980" );
		break;
	case CMI_BOOK_TITLE:	//�}�ӕC��
		GFL_STR_SetStringCodeOrderLength( str , L"�|�P����������\0" , 8 );
//		STD_CopyString( str , "�|�P����������" );
		break;
	case CMI_BOOK_USER:
		GFL_STR_SetStringCodeOrderLength( str , L"255\0" , 4 );
//		STD_CopyString( str , "255" );
		break;
	case CMI_SCORE_TITLE:	//�X�R�A
		GFL_STR_SetStringCodeOrderLength( str , L"SCORE\0" , 6 );
//		STD_CopyString( str , "SCORE" );
		break;
	case CMI_SCORE_USER:	
		GFL_STR_SetStringCodeOrderLength( str , L"98765\0" , 6 );
//		STD_CopyString( str , "98765" );
		break;
	case CMI_TIME_TITLE:	//�v���C����
		GFL_STR_SetStringCodeOrderLength( str , L"�v���C������\0" , 7 );
//		STD_CopyString( str , "�v���C������" );
		break;
	case CMI_TIME_USER:
		GFL_STR_SetStringCodeOrderLength( str , L"10:10\0" , 6 );
//		STD_CopyString( str , "10:10" );
		break;
	case CMI_DAY_TITLE:		//�v���C�J�n��
		GFL_STR_SetStringCodeOrderLength( str , L"�ڂ�������@�͂��߂��Ƃ�\0" , 13 );
//		STD_CopyString( str , "�ڂ�������@�͂��߂��Ƃ�" );
		break;
	case CMI_DAY_USER_YEAR:	//�N
		GFL_STR_SetStringCodeOrderLength( str , L"08\0" , 3 );
//		STD_CopyString( str , "08" );
		break;
	case CMI_DAY_USER_MONTH://��
		GFL_STR_SetStringCodeOrderLength( str , L"11\0" , 3 );
//		STD_CopyString( str , "11" );
		break;
	case CMI_DAY_USER_DAY:	//��
		GFL_STR_SetStringCodeOrderLength( str , L"26\0" , 3 );
//		STD_CopyString( str , "26" );
		break;
	}
}

//--------------------------------------------------------------
//	������擾(�ʐM����
//--------------------------------------------------------------
static void TRAINER_CARD_GetCardString_Partner( STRBUF* str , const u8 idx , TRAINER_CARD_WORK *work )
{
	switch( idx )
	{
	case CMI_ID_USER:
		{
			FIELD_COMM_USERDATA_TRAINERCARD *cardData;
			cardData = FIELD_COMM_DATA_GetSelfUserData( FCUT_TRAINERCARD );
			GFL_STR_SetStringCode( str , cardData->id_ );
		}
		break;
	case CMI_NAME_USER:
		{
			FIELD_COMM_USERDATA_TRAINERCARD *cardData;
			cardData = FIELD_COMM_DATA_GetSelfUserData( FCUT_TRAINERCARD );
			GFL_STR_SetStringCode( str , cardData->name_ );
		}
		break;
	case CMI_ID_TITLE:		//ID
	case CMI_NAME_TITLE:	//���O
	case CMI_MONEY_TITLE:	//������
	case CMI_MONEY_USER:
	case CMI_BOOK_TITLE:	//�}�ӕC��
	case CMI_BOOK_USER:
	case CMI_SCORE_TITLE:	//�X�R�A
	case CMI_SCORE_USER:	
	case CMI_TIME_TITLE:	//�v���C����
	case CMI_TIME_USER:
	case CMI_DAY_TITLE:		//�v���C�J�n��
	case CMI_DAY_USER_YEAR:	//�N
	case CMI_DAY_USER_MONTH://��
	case CMI_DAY_USER_DAY:	//��
		TRAINER_CARD_GetCardString( str , idx , work );
		break;
	}
}
//---------------------------------------------------------------
//
//	�ȉ�Proc�n
//
//---------------------------------------------------------------
TRAINER_CARD_WORK *trainerCardWork = NULL;

//------------------------------------------------------------------
//  �f�o�b�O�p�������֐�
//------------------------------------------------------------------
static GFL_PROC_RESULT TrainerCardProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ARIIZUMI_DEBUG, 0x200000 );

	trainerCardWork = GFL_HEAP_AllocMemory( HEAPID_ARIIZUMI_DEBUG , sizeof(TRAINER_CARD_WORK));
	trainerCardWork = TRAINER_CARD_Init( HEAPID_ARIIZUMI_DEBUG );
	
	return GFL_PROC_RES_FINISH;
}


//------------------------------------------------------------------
/**  �f�o�b�O�pExit
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT TrainerCardProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	TRAINER_CARD_Term( trainerCardWork );
	GFL_HEAP_FreeMemory( trainerCardWork );
	trainerCardWork = NULL;

	GFL_HEAP_DeleteHeap( HEAPID_ARIIZUMI_DEBUG );
	
	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**   �f�o�b�O�p���C��
 */  
//------------------------------------------------------------------
static GFL_PROC_RESULT TrainerCardProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{

	if( TRAINER_CARD_Loop( trainerCardWork ) == TRUE ){
		return GFL_PROC_RES_FINISH;
	}

	return GFL_PROC_RES_CONTINUE;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
const GFL_PROC_DATA TrainerCardProcData = {
	TrainerCardProcInit,
	TrainerCardProcMain,
	TrainerCardProcEnd,
};


