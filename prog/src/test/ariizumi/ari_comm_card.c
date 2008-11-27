//======================================================================
/**
 * @file	ari_comm_card.c
 * @brief	�t�B�[���h�ʐM�e�X�g�p�p�[�g
 * @author	ariizumi
 * @data	08/11/25
 */
//======================================================================
#include <gflib.h>
#include <textprint.h>
#include "system/gfl_use.h"

#include "ari_comm_card.h"
#include "print/printsys.h"

#include "arc_def.h"
#include "test_graphic/d_taya.naix"
#include "test_graphic/wb_trainercard.naix"
//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
typedef enum
{
	CMI_ID_TITLE,		//ID
	CMI_ID_USER,
	CMI_NAME_TITLE,		//���O
	CMI_NAME_USER,
	CMI_MONEY_TITLE,	//������
	CMI_MONEY_USER,
	CMI_BOOK_TITLE,		//�}�ӕC��
	CMI_BOOK_USER,
	CMI_SCORE_TITLE,	//�X�R�A
	CMI_SCORE_USER,	
	CMI_TIME_TITLE,		//�v���C����
	CMI_TIME_USER,
	CMI_DAY_TITLE,		//�v���C�J�n��
	CMI_DAY_USER_YEAR,		//�N
	CMI_DAY_USER_MONTH,		//��
	CMI_DAY_USER_DAY,		//��
	CMI_MAX,
}CARD_MESSAGE_INDEX;

//���W�e�[�u��(�E���ɗ��镪�͉E��̍��W���w��
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
struct _ARI_COMM_CARD_WORK
{
	HEAPID heapID_;

	GFL_BMPWIN	*bmpWinFont_;
	PRINT_QUE	*printQue_;		
	PRINT_UTIL	printUtil_[1];
	GFL_FONT	*fontHandle_;
};

//======================================================================
//	proto
//======================================================================
static void ARI_COMM_CARD_InitGraphic( ARI_COMM_CARD_WORK *work );
static void ARI_COMM_CARD_TermGraphic( ARI_COMM_CARD_WORK *work );
static void ARI_COMM_CARD_DrawString( char* str , u16 posX , u16 posY , BOOL isRight , ARI_COMM_CARD_WORK *work );
static void ARI_COMM_CARD_GetCardString( char* str , const u8 idx , ARI_COMM_CARD_WORK *work );
//--------------------------------------------------------------
//	
//--------------------------------------------------------------
ARI_COMM_CARD_WORK* ARI_COMM_CARD_Init( HEAPID heapID )
{
	ARI_COMM_CARD_WORK *work;
	work = GFL_HEAP_AllocMemory( heapID , sizeof( ARI_COMM_CARD_WORK ) );
	work->heapID_ = heapID;

	ARI_COMM_CARD_InitGraphic( work );
	return work;
}

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
void ARI_COMM_CARD_Term( ARI_COMM_CARD_WORK *work )
{
	ARI_COMM_CARD_TermGraphic( work );
	GFL_HEAP_FreeMemory( work );
}

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
const BOOL ARI_COMM_CARD_Loop( ARI_COMM_CARD_WORK *work )
{
	PRINTSYS_QUE_Main( work->printQue_ );
	if( PRINT_UTIL_Trans( work->printUtil_ , work->printQue_ ) )
	{}
	if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
	{
		return TRUE;
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
static void ARI_COMM_CARD_InitGraphic( ARI_COMM_CARD_WORK *work )
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
	
	//�t�H���g�pBMPWIN�n	GFL_FONTSYS_SetColor(4, 0xb, 7);
	
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
		u8 i;
		for( i=0;i<CMI_MAX;i++ )
		{
			char str[128];
			const BOOL isRight = (CARD_MESSAGE_POSITION[i][2]==0?FALSE:TRUE);
			ARI_COMM_CARD_GetCardString( str , i , work );
			ARI_COMM_CARD_DrawString( str , CARD_MESSAGE_POSITION[i][0] , CARD_MESSAGE_POSITION[i][1], isRight , work );
		}
	}

	while( PRINTSYS_QUE_IsFinished(work->printQue_) == FALSE )
	{
		PRINTSYS_QUE_Main( work->printQue_ );
		if( PRINT_UTIL_Trans( work->printUtil_ , work->printQue_ ) )
		{}
	}
}

//--------------------------------------------------------------
//	�`����J��
//--------------------------------------------------------------
static void ARI_COMM_CARD_TermGraphic( ARI_COMM_CARD_WORK *work )
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
static void ARI_COMM_CARD_DrawString( char* str , u16 posX , u16 posY , BOOL isRight , ARI_COMM_CARD_WORK *work )
{
	STRBUF *strBuf;
	u16	strArr[128];
	int arrLen = 128;
	int	strBaseLen = STD_StrLen( str );
	int	workPosX = posX;
	strBuf = GFL_STR_CreateBuffer( 128 , work->heapID_ );
	STD_ConvertStringSjisToUnicode( strArr , &arrLen , str , &strBaseLen , NULL );
	
	GFL_STR_SetStringCodeOrderLength( strBuf , strArr , arrLen+1 );
	
	if( isRight == TRUE )
	{
		workPosX -= PRINTSYS_GetStrWidth( strBuf , work->fontHandle_ , 1 );
	}

	PRINT_UTIL_Print( work->printUtil_ , work->printQue_ , workPosX , posY ,
					(void*)strBuf , work->fontHandle_ );

	GFL_STR_DeleteBuffer( strBuf );
}
 
//--------------------------------------------------------------
//	������擾
//--------------------------------------------------------------
static void ARI_COMM_CARD_GetCardString( char* str , const u8 idx , ARI_COMM_CARD_WORK *work )
{
	switch( idx )
	{
	case CMI_ID_TITLE:		//ID
		STD_CopyString( str , "IDNo." );
		break;
	case CMI_ID_USER:
		STD_CopyString( str , "012345" );
		break;
	case CMI_NAME_TITLE:	//���O
		STD_CopyString( str , "�Ȃ܂�" );
		break;
	case CMI_NAME_USER:
		STD_CopyString( str , "�e�X�g" );
		break;
	case CMI_MONEY_TITLE:	//������
		STD_CopyString( str , "�����Â���" );
		break;
	case CMI_MONEY_USER:
		STD_CopyString( str , "12980" );
		break;
	case CMI_BOOK_TITLE:	//�}�ӕC��
		STD_CopyString( str , "�|�P����������" );
		break;
	case CMI_BOOK_USER:
		STD_CopyString( str , "255" );
		break;
	case CMI_SCORE_TITLE:	//�X�R�A
		STD_CopyString( str , "SCORE" );
		break;
	case CMI_SCORE_USER:	
		STD_CopyString( str , "98765" );
		break;
	case CMI_TIME_TITLE:	//�v���C����
		STD_CopyString( str , "�v���C������" );
		break;
	case CMI_TIME_USER:
		STD_CopyString( str , "10:10" );
		break;
	case CMI_DAY_TITLE:		//�v���C�J�n��
		STD_CopyString( str , "�ڂ�������@�͂��߂��Ƃ�" );
		break;
	case CMI_DAY_USER_YEAR:	//�N
		STD_CopyString( str , "08" );
		break;
	case CMI_DAY_USER_MONTH://��
		STD_CopyString( str , "11" );
		break;
	case CMI_DAY_USER_DAY:	//��
		STD_CopyString( str , "26" );
		break;
	}
}

