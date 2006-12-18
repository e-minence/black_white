//===================================================================
/**
 * @file	testmode.c
 */
//===================================================================
#include "gflib.h"
#include "main.h"

#include "ui.h"
#include "textprint.h"

typedef struct {
	int						seq;
	u16						listPosition;
	void*					chrbuf;
	GFL_BMP_DATA*			bmpHeader;
	GFL_TEXT_PRINTPARAM*	textParam;

}TESTMODE_WORK;

TESTMODE_WORK* testmode;

void	TestModeInit(void);
void	TestModeMain(void);

static BOOL	TestModeControl( void );

#define XYSIZE (32*32)	//�X�N���[���L�����N�^�T�C�Y

//------------------------------------------------------------------
/**
 * @brief		������
 */
//------------------------------------------------------------------
void	TestModeInit(void)
{
	testmode = GFL_HEAP_AllocMemoryClear(GFL_HEAPID_APP,sizeof(TESTMODE_WORK));
	GFL_TEXT_sysInit( "gfl_graphic/gfl_font.dat" );
}

//------------------------------------------------------------------
/**
 * @brief		���C��
 */
//------------------------------------------------------------------
void	TestModeMain(void)
{
	if( TestModeControl() == TRUE ){
		GFL_HEAP_FreeMemory( testmode );
	}
}


//------------------------------------------------------------------
/**
 * @brief		�a�f�ݒ聕�f�[�^�]��
 */
//------------------------------------------------------------------
static void	bgscr_trans( void* scr )
{
	//GFL_STD_MemCopy32(scr,(void*)G2_GetBG3ScrPtr(),XYSIZE*2);
	DC_FlushRange( scr, XYSIZE*2 );
	GX_LoadBG3Scr( scr, 0, XYSIZE*2 ); 
}

static void	bgchr_trans( void* chr )
{
	//GFL_STD_MemCopy32(chr,(void*)(u32)G2_GetBG3CharPtr(),XYSIZE*0x20);
	DC_FlushRange( chr, XYSIZE*0x20 );
	GX_LoadBG3Char( chr, 0, XYSIZE*0x20 ); 
}

static void	bgplt_trans( void* plt )
{
	//GFL_STD_MemCopy16(plt,(void*)HW_BG_PLTT,16*2);
	DC_FlushRange( plt, 16*2 );
	GX_LoadBGPltt( plt, 0, 16*2 );
}

static void	bg_init( void )
{
	GX_SetBankForBG(GX_VRAM_BG_64_E);
	GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D); 
	G2_SetBG3ControlText(	GX_BG_SCRSIZE_TEXT_256x256,GX_BG_COLORMODE_16,
							GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000 );
	GX_SetVisiblePlane(GX_PLANEMASK_BG3);
	G2_SetBG3Priority(0);
	{//�X�N���[���쐬���]��
		int	i;
		u16* scr = GFL_HEAP_AllocMemoryLowClear(GFL_HEAPID_APP,XYSIZE*2);

		for(i=0;i<XYSIZE;i++){
			scr[i] = i; 	//�L�����i���o�[0���疄�߂�
		}
		bgscr_trans(scr);
		GFL_HEAP_FreeMemory(scr);
	}
	{//�L�����N�^�[�o�b�t�@�쐬���]��
		u16* chr = GFL_HEAP_AllocMemoryClear(GFL_HEAPID_APP,XYSIZE*0x20);
		bgchr_trans(chr);
		testmode->chrbuf = chr;	//�L�����]���p�ɕۑ�
	}
	{//�p���b�g�쐬���]��
		u16* plt = GFL_HEAP_AllocMemoryLowClear(GFL_HEAPID_APP,16*2);

		plt[0] = 0x5041;	//��
		plt[1] = 0x7fff;	//��
		plt[2] = 0x001f;	//��
		bgplt_trans(plt);
		GFL_HEAP_FreeMemory(plt);
	}
	{//�r�b�g�}�b�v�w�b�_�쐬
		GFL_BMP_DATA* bmpchr = GFL_HEAP_AllocMemoryLowClear(GFL_HEAPID_APP,sizeof(GFL_BMP_DATA));

		bmpchr->adrs = testmode->chrbuf;
		bmpchr->size_x = 32*8;
		bmpchr->size_y = 32*8;
		testmode->bmpHeader = bmpchr;
	}
	{//�����\���p�����[�^���[�N�쐬
		GFL_TEXT_PRINTPARAM* param = GFL_HEAP_AllocMemoryLowClear
										(GFL_HEAPID_APP,sizeof(GFL_TEXT_PRINTPARAM));
		param->bmp = testmode->bmpHeader;
		param->writex = 16;
		param->writey = 8;
		param->spacex = 1;
		param->spacey = 1;
		param->colorF = 1;
		param->colorB = 0;
		param->mode = GFL_TEXT_WRITE_16;
		testmode->textParam = param;
	}
	GX_DispOn(); 
}

static void	bg_exit( void )
{
	GFL_HEAP_FreeMemory(testmode->bmpHeader);
	GFL_HEAP_FreeMemory(testmode->textParam);
	GFL_HEAP_FreeMemory(testmode->chrbuf);
}

//------------------------------------------------------------------
/**
 * @brief	�v�����g����
 */
//------------------------------------------------------------------
static const char	test_index0[] = 
{"�Q�[���t���[�N�@�v���O�������C�u�����@�e�X�g���[�h\nGame Freak Libraries TestMode"};
static const char	test_index1[] = {"http://www.gamefreak.co.jp"};

static const char	test_select0[] = {"�킽�Ȃׁ@�Ă�\nTetsuya Watanabe"};
static const char	test_select1[] = {"���܂��@��������\nSousuke Tamada"};
static const char	test_select2[] = {"�����ׁ@�Ђ���\nHisashi Sogabe"};
static const char	test_select3[] = {"�����́@����\nKatsumi Ohno"};
static const char	test_select4[] = {"�݂͂�@���Ƃ�\nSatoshi Mitsuhara"};

typedef struct {
	const char*	name;
	u8 writex;
	u8 writey;
}TESTMODE_PRINTLIST;

static const TESTMODE_PRINTLIST indexList[] = {
	{ test_index0, 16, 8},
	{ test_index1, 16, 180},
};

static const TESTMODE_PRINTLIST selectList[] = {
	{ test_select0, 32, (0*25)+40},
	{ test_select1, 32, (1*25)+40},
	{ test_select2, 32, (2*25)+40},
	{ test_select3, 32, (3*25)+40},
	{ test_select4, 32, (4*25)+40},
};

static BOOL	TestModeControl( void )
{
	BOOL return_flag = FALSE;
	int i;

	switch( testmode->seq ){

	case 0:
		bg_init();
		testmode->listPosition = 0;
		testmode->seq++;
		break;

	case 1:
		for(i=0;i<NELEMS(indexList);i++){
			testmode->textParam->writex = indexList[i].writex;
			testmode->textParam->writey = indexList[i].writey;
			GFL_TEXT_PrintSjisCode( indexList[i].name, testmode->textParam );
		}
		testmode->seq++;
		break;

	case 2:
		for(i=0;i<NELEMS(selectList);i++){
			testmode->textParam->writex = selectList[i].writex;
			testmode->textParam->writey = selectList[i].writey;
			if( testmode->listPosition == i ){
				testmode->textParam->colorF = 2;
			} else {
				testmode->textParam->colorF = 1;
			}
			GFL_TEXT_PrintSjisCode( selectList[i].name, testmode->textParam );
		}
		bgchr_trans( testmode->chrbuf );
		testmode->seq++;
		break;

	case 3:
		if( GFL_UI_KeyGetTrg() == PAD_KEY_UP ){
			if( testmode->listPosition > 0 ){
				testmode->listPosition--;
				testmode->seq--;
			}
		} else if( GFL_UI_KeyGetTrg() == PAD_KEY_DOWN ){
			if( testmode->listPosition < NELEMS(selectList)-1 ){
				testmode->listPosition++;
				testmode->seq--;
			}
		}
		break;

	case 4:
		bg_exit();
		break;
	}
	return return_flag;
}


