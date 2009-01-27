//==============================================================================================
/**
 * @file	snd_test.c
 * @brief	�T�E���h�e�X�g(�T�E���h�`�F�b�N)
 * @author	nohara
 * @date	2005.07.04
 *
 * ���T�E���h�e�X�g�̗�O�����ɂ��ā�
 *
 * �P�j
 * �T�E���h�q�[�v���g�p���āA�f�[�^�̃��[�h���s���悤�ɂ���ƁA
 * �Ȃ̍Đ��A��~���ƂɁA�m�ہA�J�����s��Ȃ��Ƃ����Ȃ��Ȃ�܂��B
 *
 * ���ꂾ�ƁABGM��SE�𓯎��ɍĐ����邱�Ƃ�����̂ŁA
 *
 * �q�[�v��S�ăN���A���āA
 * PLAYER_BGM��BGM�̃V�[�P���X�A�o���N�A�g�`�A�[�J�C�u���̃v���C���[�q�[�v���m�ہA
 *
 * BGM��PLAYER_BGM�̃v���C���[�q�[�v�ōĐ�����悤�ɂ��܂��B
 * ME��PLAYER_BGM�̃v���C���[�q�[�v�ōĐ�����悤�ɂ��܂��B
 *
 * �Q�j
 * SE�́A�Đ����邽�тɁA�V�[�P���X�A�o���N�A�g�`�A�[�J�C�u��
 * �T�E���h�q�[�v�Ń��[�h���Ȃ����悤�ɂ��܂��B
 *
 * �R�j
 * ��L�̗�O���������Ă���̂ŁA
 * �T�E���h�ݒ肪�A�Q�[�����Ƃ́A�S���قȂ�܂��B
 * ���̂��߁A�ʏ�̃T�E���h��������ʂ��������A
 * ���S�Ȃ̂ŁA�I�������ŁA�\�t�g���Z�b�g�������Ă��܂��B
 */
//==============================================================================================
#include <gflib.h>
#include "sound/snd_tool.h"
#include "snd_test.h"

#include "sound/snd_system.h"

#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"

#include "msg/msg_snd_test_str.h"
#include "msg/msg_snd_test_name.h"

#include "system/main.h"
#include "system/bmp_menuwork.h"
#include "system/bmp_menulist.h"
#include "system/bmp_winframe.h"
#include "msg/msg_debugname.h"

#include "print/str_tool.h"

//==============================================================================================
//
//	��`
//
//==============================================================================================
enum{
	BUF_BGM = 0,
	BUF_SE,
	BUF_PV,

	BUF_MAX,								//�o�b�t�@�̍ő吔
};

#define SND_TEST_BUF_SIZE		(48)		//�o�b�t�@�̃T�C�Y
//#define BG_PLANE_MENU (SND_TEST_BMPWIN_FRAME)
#define ITEM_NAME_MAX (32)
#define TESTMODE_PLT_FONT	(15)
#define LIST_DISP_MAX (5)
#define FBMP_COL_RED  (3)
#define FBMP_COL_WHITE (15)


//==============================================================================================
//
//	�\����
//
//==============================================================================================
//�T�E���h�e�X�g�\����
typedef	struct {
	BOOL end_flag;
	u8	seq;							//�����i���o�[
	s8  select;							//�I��Ώ�
	u16 dmy2;

	int	old_bgm;						//�Â�BGM�i���o�[
	int	bgm;							//BGM�i���o�[
	int	se;								//SE�i���o�[
	int	pv;								//�|�P���������i���o�[

	int bgm_lv;							//�K�w
	int se_lv[SE_HANDLE_MAX];			//�K�w
	int pv_lv;							//�K�w

	u8  bgm_play_flag;					//��x�ł��Đ��������t���O
	u8  se_play_flag[SE_HANDLE_MAX];	//��x�ł��Đ��������t���O
	u8  pv_play_flag;					//��x�ł��Đ��������t���O
	u8  dmy3;
	
	int pitch;							//����
	int tempo;							//�e���|
	int var;							//�V�[�P���X���[�J�����[�N�ɃZ�b�g����l

	u16* pMsg;

	GFL_BG_INI * bgl;

//	GFL_BMPWIN bmpwin;				//BMP�E�B���h�E�f�[�^

	STRBUF* msg_buf[BUF_MAX];

	GFL_MSGDATA* msgman;			//���b�Z�[�W�}�l�[�W���[


	BMP_MENULIST_DATA	*menuList;
	BMPMENULIST_WORK	*menuWork;
	PRINT_UTIL			printUtil;
	PRINT_QUE			*printQue;
	GFL_FONT 			*fontHandle;
	GFL_BMPWIN			*bmpWin;

	PRINT_UTIL			printUtilSub;
	PRINT_QUE			*printQueSub;
//	GFL_BMPWIN			*bmpWinSub;	//����ʗp�H

	HEAPID heapId;
    
}SND_TEST_WORK;


//==============================================================================================
//
//	�v���g�^�C�v�錾
//
//==============================================================================================
static void SndTestCall(SND_TEST_WORK * wk);

static void SndTestWorkInit( SND_TEST_WORK* wk );
static void SndTestSeqNext( SND_TEST_WORK* wk );

static void SndTestSeqInit( SND_TEST_WORK* wk );
static void SndTestSeqBgmFadeWait( SND_TEST_WORK* wk );
static void SndTestSeqKeyCheck( SND_TEST_WORK* wk );
static void SndTestSeqExit( SND_TEST_WORK* wk );

static void SndTestStop( SND_TEST_WORK* wk );
static void SndTestPlay( SND_TEST_WORK* wk );

//static void (* const SndTestTable[])() = {
static void (* const SndTestTable[])(SND_TEST_WORK*) = {
	SndTestSeqInit,						//������
	SndTestSeqBgmFadeWait,				//BGM�t�F�[�h�A�E�g�҂�
	SndTestSeqKeyCheck,					//�L�[�`�F�b�N
	SndTestSeqExit,						//�I��
};

static void SndTestNumMsgSet( SND_TEST_WORK* wk, int num, u8 x, u8 y );
static void SndTestNameMsgSet( SND_TEST_WORK* wk, const STRBUF* msg, u8 x, u8 y );
static void MsgRewrite( SND_TEST_WORK* wk, s8 select );
static void SndTestInit( SND_TEST_WORK* wk );
static void SndTestCursor( SND_TEST_WORK* wk );
static void SndTestSysMsgSet( SND_TEST_WORK* wk );
static void SndTestStrPrint( SND_TEST_WORK* wk,  u32 strID, u32 x, u32 y );
static void SndTestTrackMute( SND_TEST_WORK* wk );

//BG�ݒ�
static void SetBank();
static void BgSet( SND_TEST_WORK* wk );


//==================================================================================================
//
//	�f�[�^
//
//==================================================================================================
/*
static const BMPWIN_DAT	SndTestWinData = {
	SND_TEST_BMPWIN_FRAME,					//�E�C���h�E�g�p�t���[��
	SND_TEST_BMPWIN_PX1,SND_TEST_BMPWIN_PY1,//�E�C���h�E�̈�̍����X,Y���W�i�L�����P�ʂŎw��j
	SND_TEST_BMPWIN_SX,	SND_TEST_BMPWIN_SY,	//�E�C���h�E�̈��X,Y�T�C�Y�i�L�����P�ʂŎw��j
	SND_TEST_BMPWIN_PL,						//�E�C���h�E�̈�̃p���b�g�i���o�[	
	SND_TEST_BMPWIN_CH						//�E�C���h�E�L�����̈�̊J�n�L�����N�^�i���o�[
};
*/

//==================================================================================================
//
//	�֐�
//
//==================================================================================================

//--------------------------------------------------------------
/**
 * @brief	�T�E���h�e�X�g�Ăяo��
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void SndTestCall(SND_TEST_WORK * wk)
{
#if 0
	if( wk == NULL ){
		OS_Printf("snd_test.c Alloc ERROR!");
		return;
	}

	wk->bgl = GF_BGL_BglIniAlloc( HEAPID_BASE_DEBUG );

	//BG�ݒ�
	GF_Disp_GX_VisibleControlInit();	//���C����ʂ̊e�ʂ̕\���R���g���[��������
	SetBank();							//VRAM�o���N�ݒ�
	BgSet(wk);							//BG�ݒ�
	GF_Disp_DispOn();					//�\���L���ݒ�
	MSG_PrintInit();					//�����\���������֐�

	OS_Printf( "\n\n//==========================================\n" );
	OS_Printf( "//\t�T�E���h�e�X�g�@�X�^�[�g\n" );
	OS_Printf( "//==========================================\n" );

	wk->end_flag = FALSE;
	wk->seq			= 0;									//�����i���o�[
	SndTestWorkInit(wk);									//���[�N������

	//���b�Z�[�W�f�[�^�}�l�[�W���[�쐬
//	wk->msgman = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, 
//									NARC_msg_snd_test_name_dat, HEAPID_BASE_DEBUG );

	//app������(fld_debug.c)
	//FieldBitMapWinCgxSet();
	
	//SystemFontPaletteLoad( PALTYPE_MAIN_BG, 0, HEAPID_BASE_DEBUG );

	GF_BGL_BmpWinAddEx( wk->bgl, &wk->bmpwin, &SndTestWinData );		//�r�b�g�}�b�v�ǉ�
	GF_BGL_BmpWinDataFill( &wk->bmpwin, FBMP_COL_WHITE );				//�h��Ԃ�

	SndTestSysMsgSet( wk );									//�K�v�ȏ�񃁃b�Z�[�W
	SndTestCursor( wk );									//�J�[�\���X�V

	//�Ȗ����b�Z�[�W�\��
	MsgRewrite( wk, SND_TEST_TYPE_BGM );					//BGM�i���o�[
	MsgRewrite( wk, SND_TEST_TYPE_SE );						//SE�i���o�[
	MsgRewrite( wk, SND_TEST_TYPE_PV );						//PV�i���o�[

	SndTestNumMsgSet( wk, wk->pitch, ST_TYPE_X+88, ST_BGM_MSG_Y );//����
	SndTestNumMsgSet( wk, wk->tempo, ST_TYPE_X+168, ST_BGM_MSG_Y );//�e���|

	GF_BGL_BmpWinOn( &wk->bmpwin );
#endif

	GX_SetMasterBrightness(0);	
	GXS_SetMasterBrightness(-16);
	GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
	GFL_DISP_GXS_SetVisibleControlDirect(0);
	SetBank();							//VRAM�o���N�ݒ�
    GFL_BG_Init(wk->heapId );
	BgSet(wk);							//BG�ݒ�
    GFL_BMPWIN_Init( wk->heapId );

    
	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , TESTMODE_PLT_FONT * 32, 16*2, wk->heapId );
	wk->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , wk->heapId );

	wk->bmpWin = GFL_BMPWIN_Create( SND_TEST_BMPWIN_FRAME , 1,1,30,LIST_DISP_MAX*2,TESTMODE_PLT_FONT,GFL_BMP_CHRAREA_GET_B );
	GFL_BMPWIN_MakeScreen( wk->bmpWin );
	GFL_BMPWIN_TransVramCharacter( wk->bmpWin );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->bmpWin), 15 );
	GFL_BG_LoadScreenReq( SND_TEST_BMPWIN_FRAME );
	wk->printQue = PRINTSYS_QUE_Create( wk->heapId );
	PRINT_UTIL_Setup( &wk->printUtil , wk->bmpWin );

	wk->msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
									NARC_message_snd_test_name_dat, wk->heapId );
    
//	wk->printQueSub = PRINTSYS_QUE_Create( wk->heapId );
//	PRINT_UTIL_Setup( &wk->printUtilSub , wk->bmpWinSub );
    
	return;
}

//--------------------------------------------------------------
/**
 * @brief	���[�N������
 *
 * @param	wk		SND_TEST_WORK�ւ̃|�C���^
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void SndTestWorkInit( SND_TEST_WORK* wk )
{
	int i;

	wk->select		= SND_TEST_TYPE_BGM;		//�I��Ώ�

	wk->bgm			= SND_TEST_BGM_START_NO;	//BGM�X�^�[�g�i���o�[
	wk->se			= SND_TEST_SE_START_NO;		//SE�X�^�[�g�i���o�[
	wk->pv			= SND_TEST_PV_START_NO;		//�|�P���������X�^�[�g�i���o�[

	//�����K�w
	wk->bgm_lv		= 1;			
	wk->pv_lv		= 1;		

	for( i=0; i < SE_HANDLE_MAX ;i++ ){
		wk->se_lv[i] = 1;		
	}

	//��x�ł��Đ��������t���O
	wk->bgm_play_flag	= 0;					
	wk->pv_play_flag	= 0;

	for( i=0; i < SE_HANDLE_MAX ;i++ ){
		wk->se_play_flag[i]	= 0;
	}

	wk->pMsg = NULL;

	wk->pitch		= 0;	//����
	wk->tempo		= 256;	//�e���|

	// �o�b�t�@�쐬
	for(i=0; i<BUF_MAX; i++)
	{
		wk->msg_buf[i] = GFL_STR_CreateBuffer(SND_TEST_BUF_SIZE*2, wk->heapId);
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	���̏�����
 *
 * @param	wk		SND_TEST_WORK�ւ̃|�C���^
 *
 * @retval	none
 *
 * �K�v�ȏ�����ǉ����Ă���(�T�u�V�[�P���X�̃N���A�Ȃ�)
 */
//--------------------------------------------------------------
static void SndTestSeqNext( SND_TEST_WORK* wk)
{
	wk->seq++;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	������
 *
 * @param	wk		SND_TEST_WORK�ւ̃|�C���^
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void SndTestSeqInit( SND_TEST_WORK* wk )
{
	//Snd_BgmFadeOut( 60 );				//BGM�t�F�[�h�A�E�g
	Snd_Stop();							//�S��~
	Snd_BgmChannelSetAndReverbSet( 0 );	//�g�p�\�`�����l������A���o�[�u�ݒ�(�N���A)
	SndTestSeqNext(wk);
	return;
}

//--------------------------------------------------------------
/**
 * @brief	BGM�t�F�[�h�A�E�g�҂�
 *
 * @param	wk		SND_TEST_WORK�ւ̃|�C���^
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void SndTestSeqBgmFadeWait( SND_TEST_WORK* wk )
{
	int ret;

	if( Snd_FadeCheck() ){						//BGM�t�F�[�h�A�E�g�҂�
		return;
	}

	Snd_HeapStateClear();						//�������[�h���Ă��Ȃ���Ԃɂ���(�풓�����������)
	
	//�x�[�V�b�N�o���N�A�g�`���[�h
	Snd_DebugLoadBank( BANK_BASIC );
	
	//�v���C���[�q�[�v�쐬
	ret = Snd_PlayerHeapCreate( PLAYER_BGM, 70000 );
	//Snd_UseHeapSizeOsPrint();					//�ǉ������T�E���h�q�[�v�̗e�ʂ��o�͂���
	//OS_Printf( "player_bgm heap create = %d\n", ret );

	Snd_HeapSaveState( Snd_GetParamAdrs(SND_W_ID_HEAP_SAVE_SE) );	//�K�w�ۑ�

	SndTestSeqNext(wk);
	return;
}

//--------------------------------------------------------------
/**
 * @brief	�L�[�`�F�b�N
 *
 * @param	wk		SND_TEST_WORK�ւ̃|�C���^
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void SndTestSeqKeyCheck( SND_TEST_WORK* wk )
{
	int ret, i, spd;

	//�����ύX
	if( GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT ){
		if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP ){
			wk->pitch++;
		}else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
			wk->pitch--;
		}
		SndTestNumMsgSet( wk, wk->pitch, ST_TYPE_X+88, ST_BGM_MSG_Y );
		Snd_PlayerSetTrackPitch( SND_HANDLE_BGM, 0xffff, (wk->pitch*64) );
//		GF_BGL_BmpWinOn( &wk->bmpwin );
		return;
	}

	//�V�[�P���X���[�J�����[�N��ύX
	if( (GFL_UI_KEY_GetCont() & PAD_BUTTON_R) && (GFL_UI_KEY_GetRepeat() & PAD_KEY_UP) ){

		wk->var++;
		if( wk->var >= 128 ){
			wk->var = 0;
		}
		return;
	}

	if( (GFL_UI_KEY_GetCont() & PAD_BUTTON_R) && (GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN) ){

		wk->var--;
		if( wk->var < 0 ){
			wk->var = 127;
		}
		return;
	}

	//�e���|�ύX
	if( GFL_UI_KEY_GetCont() & PAD_BUTTON_START ){
		if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP ){
			wk->tempo+=32;
		}else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN ){
			wk->tempo-=32;
		}
		SndTestNumMsgSet( wk, wk->tempo, ST_TYPE_X+168, ST_BGM_MSG_Y );
		Snd_PlayerSetTempoRatio( SND_HANDLE_BGM, wk->tempo );
//		GF_BGL_BmpWinOn( &wk->bmpwin );
		return;
	}

	//�Ώۂ�ύX(��BGM��PV��SE��)
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN ){
		wk->select++;
		if( wk->select >= SND_TEST_TYPE_MAX ){
			wk->select = SND_TEST_TYPE_BGM;
		}

		SndTestCursor( wk );					//�J�[�\���X�V
//		GF_BGL_BmpWinOn( &wk->bmpwin );
	}

	//�Ώۂ�ύX(��BGM��SE��PV��)
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP ){
		wk->select--;
		if( wk->select < SND_TEST_TYPE_BGM ){
			wk->select = (SND_TEST_TYPE_MAX-1);
		}

		SndTestCursor( wk );					//�J�[�\���X�V
//		GF_BGL_BmpWinOn( &wk->bmpwin );
	}

	//�V�[�P���X�i���o�[��i�߂�
	if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT ){

		if( (GFL_UI_KEY_GetCont() & PAD_BUTTON_R) || (GFL_UI_KEY_GetCont() & PAD_BUTTON_L) ){
			spd = 10;												//10����
		}else{
			spd = 1;												//1����
		}

		switch( wk->select ){
		case SND_TEST_TYPE_BGM:
			wk->old_bgm = wk->bgm;
			wk->bgm+=spd;
			if( wk->bgm >= SND_TEST_BGM_END_NO ){
				wk->bgm = SND_TEST_BGM_START_NO;
			}

			MsgRewrite( wk, SND_TEST_TYPE_BGM );					//BGM�i���o�[
//			GF_BGL_BmpWinOn( &wk->bmpwin );
			break;
		case SND_TEST_TYPE_SE:
			wk->se+=spd;
			if( wk->se >= SND_TEST_SE_END_NO ){
				wk->se = SND_TEST_SE_START_NO;
			}

			MsgRewrite( wk, SND_TEST_TYPE_SE );						//SE�i���o�[
//			GF_BGL_BmpWinOn( &wk->bmpwin );
			break;
		case SND_TEST_TYPE_PV:
			wk->pv+=spd;
			if( wk->pv >= SND_TEST_PV_END_NO ){
				wk->pv = SND_TEST_PV_START_NO;
			}

			MsgRewrite( wk, SND_TEST_TYPE_PV );						//PV�i���o�[
//			GF_BGL_BmpWinOn( &wk->bmpwin );
			break;
		};
	}

	//�V�[�P���X�i���o�[��߂�
	if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT ){

		if( (GFL_UI_KEY_GetCont() & PAD_BUTTON_R) || (GFL_UI_KEY_GetCont() & PAD_BUTTON_L) ){
			spd = 10;												//10����
		}else{
			spd = 1;												//1����
		}

		switch( wk->select ){
		case SND_TEST_TYPE_BGM:
			wk->old_bgm = wk->bgm;
			wk->bgm-=spd;
			if( wk->bgm < SND_TEST_BGM_START_NO ){
				wk->bgm = (SND_TEST_BGM_END_NO);
			}

			MsgRewrite( wk, SND_TEST_TYPE_BGM );					//BGM�i���o�[
//			GF_BGL_BmpWinOn( &wk->bmpwin );
			break;
		case SND_TEST_TYPE_SE:
			wk->se-=spd;
			if( wk->se < SND_TEST_SE_START_NO ){
				wk->se = (SND_TEST_SE_END_NO);
			}

			MsgRewrite( wk, SND_TEST_TYPE_SE );						//SE�i���o�[
//			GF_BGL_BmpWinOn( &wk->bmpwin );
			break;
		case SND_TEST_TYPE_PV:
			wk->pv-=spd;
			if( wk->pv < SND_TEST_PV_START_NO ){
				wk->pv = (SND_TEST_PV_END_NO);
			}

			MsgRewrite( wk, SND_TEST_TYPE_PV );						//PV�i���o�[
//			GF_BGL_BmpWinOn( &wk->bmpwin );
			break;
		};
	}

	//��~
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
		SndTestStop(wk);
	}

	//�Đ�(��~�����Ă���Đ�)
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
		SndTestStop(wk);
		SndTestPlay(wk);

#if 1
		//�V�[�P���X���[�J�����[�N����
		//�T�E���h�n���h�����֘A�t����ꂽ��ɑ��삷��I
		Snd_PlayerWriteVariable( wk->var );
		SndTestNumMsgSet( wk, wk->var, ST_TYPE_X+168, ST_BGM_MSG_Y+16 );
//		GF_BGL_BmpWinOn( &wk->bmpwin );
#endif

	}

	//������
	//if( GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT ){
	if( GFL_UI_KEY_GetCont() & PAD_BUTTON_Y ){
		Snd_Stop();						//�S�Ē�~
		SndTestInit( wk );				//���[�N�A�f�[�^�ȂǃN���A
//		GF_BGL_BmpWinOn( &wk->bmpwin );
	}

	//�I��
	//if( GFL_UI_KEY_GetCont() & PAD_BUTTON_START ){
	if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X ){
		Snd_Stop();						//�S�Ē�~
		SndTestSeqNext(wk);
		return;
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	�I��
 *
 * @param	wk		SND_TEST_WORK�ւ̃|�C���^
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void SndTestSeqExit( SND_TEST_WORK* wk )
{
	int i;

	//BMP�E�B���h�EOFF
//	GF_BGL_BmpWinOff( &wk->bmpwin );
	
	//BMP���X�g�j��
	//BmpListExit( wp->bmplistContID,&DebugList,&DebugCursor );
	//BmpListExit( wk->blwin, NULL, NULL );

	//BMPWindow����
//	GF_BGL_BmpWinDel( &wk->bmpwin );


	//�J������
	PRINTSYS_QUE_Clear( wk->printQue );
	PRINTSYS_QUE_Delete( wk->printQue );
	GFL_BMPWIN_Delete( wk->bmpWin );
	GFL_FONT_Delete( wk->fontHandle );
	GFL_BMPWIN_Exit();
	GFL_BG_Exit();


	GX_SetMasterBrightness(0);
	GXS_SetMasterBrightness(0);



    
	GFL_MSG_Delete( wk->msgman );


	//�����o�b�t�@�폜
	for(i=0; i<BUF_MAX; i++)
	{
		if(wk->msg_buf[i]){
			GFL_STR_DeleteBuffer(wk->msg_buf[i]);
		}
	}



    
	wk->end_flag = TRUE;

}

//--------------------------------------------------------------
/**
 * @brief	�T�E���h��~����
 *
 * @param	wk		SND_TEST_WORK�ւ̃|�C���^
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void SndTestStop( SND_TEST_WORK* wk )
{
	int i,type;

	switch( wk->select ){
	case SND_TEST_TYPE_BGM:
		//Snd_BgmStop(Snd_NowBgmNoGet(), 0);
		//Snd_BgmStop( wk->old_bgm, 0 );
		NNS_SndPlayerStopSeqBySeqNo( wk->old_bgm, 0 );
		break;
	case SND_TEST_TYPE_SE:
		//Snd_SeStopBySeqNo( wk->se, 0 );
		Snd_SeStop( SND_HANDLE_SE_1, 0 );
		break;
	case SND_TEST_TYPE_PV:
		Snd_PMVoiceStop(0);
		break;
	};

}

//--------------------------------------------------------------
/**
 * @brief	�T�E���h�Đ�����
 *
 * @param	wk		SND_TEST_WORK�ւ̃|�C���^
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void SndTestPlay( SND_TEST_WORK* wk )
{
	int type,ret;

	switch( wk->select ){
	case SND_TEST_TYPE_BGM:
        if(Snd_ArcPlayerStartSeqEx( SND_HANDLE_BGM, PLAYER_BGM, wk->bgm )){
            OS_TPrintf("SND_TEST_TYPE_BGMSTART\n");
        }
		break;

	case SND_TEST_TYPE_SE:
		//�V�[�P���X�A�o���N�A�g�`�A�[�J�C�u���[�h(�q�[�v���x���͓K���ł�)
		Snd_HeapLoadState( Snd_GetHeapSaveLv(SND_HEAP_SAVE_SE) );		//�J��
		Snd_ArcLoadSeq( wk->se );										//���[�h
		//Snd_HeapSaveState( Snd_GetParamAdrs(SND_W_ID_HEAP_SAVE_SE) );	//�K�w�ۑ�
		ret = Snd_ArcPlayerStartSeqEx( SND_HANDLE_SE_1, PLAYER_SE_1, wk->se );
		//OS_Printf( "se_play = %d\n", ret );
		break;

	case SND_TEST_TYPE_PV:
		Snd_PMVoicePlay( wk->pv, 0 );
		break;
	};

	return;
}


//==============================================================================================
//
//	�ݒ�֐�
//
//==============================================================================================

//--------------------------------------------------------------
/**
 * @brief	VRAM�o���N�ݒ�
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void SetBank(void)
{
	//display.c
	
	GFL_DISP_VRAM vramSetTable = {	//VRAM�ݒ�\����
		//GX_VRAM_BG_256_AB,			//���C��2D�G���W����BG
		GX_VRAM_BG_128_C,				//���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			//���C��2D�G���W����BG�g���p���b�g

		GX_VRAM_SUB_BG_32_H,			//�T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		//�T�u2D�G���W����BG�g���p���b�g

		//GX_VRAM_OBJ_64_E,				//���C��2D�G���W����OBJ
		GX_VRAM_OBJ_16_F,				//���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		//���C��2D�G���W����OBJ�g���p���b�g

		//GX_VRAM_SUB_OBJ_NONE,			//�T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJ_16_I,			//�T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	//�T�u2D�G���W����OBJ�g���p���b�g

		GX_VRAM_TEX_01_AB,			//�e�N�X�`���C���[�W�X���b�g
		//GX_VRAM_TEX_0_A,				//�e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_0123_E			//�e�N�X�`���p���b�g�X���b�g
		//GX_VRAM_TEX_NONE,				//�e�N�X�`���C���[�W�X���b�g
		//GX_VRAM_TEXPLTT_NONE			//�e�N�X�`���p���b�g�X���b�g
		//GX_VRAM_TEXPLTT_0123_E		//�e�N�X�`���p���b�g�X���b�g
	};

	GFL_DISP_SetBank( &vramSetTable );
}

//--------------------------------------------------------------
/**
 * @brief	BG�ݒ�
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void BgSet( SND_TEST_WORK* wk )
{
	//bg_system.c
	
#if 1
	{
		GFL_BG_SYS_HEADER BGsys_data = {	//BG�V�X�e���\����
			GX_DISPMODE_GRAPHICS,			//�\�����[�h�w�� 
			GX_BGMODE_0,					//�a�f���[�h�w��(���C���X�N���[��)
			GX_BGMODE_0,					//�a�f���[�h�w��(�T�u�X�N���[��)
			GX_BG0_AS_2D,					//�a�f�O�̂Q�c�A�R�c���[�h�I��
			//GX_BG0_AS_3D,					//�a�f�O�̂Q�c�A�R�c���[�h�I��
		};

		//���C���A�T�u�̃O���t�B�b�N�X�G���W���̕\�����[�h�ݒ�
		//���C���A�T�u�̉�ʂ̊e�ʂ̕\���R���g���[��������(display.c)
		//�r�b�g�}�b�v�E�B���h�E�ݒ�\���̏�����
		GFL_BG_SetBGMode( &BGsys_data );
	}
#endif

	{	//BMP�E�B���h�E
		GFL_BG_BGCNT_HEADER BmpWinBgCntData = {	//BG�R���g���[���ݒ�\����
	0, 0, 0x800, 0,
	GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
	GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
	GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};

		GFL_BG_SetBGControl(SND_TEST_BMPWIN_FRAME, 
							&BmpWinBgCntData, GFL_BG_MODE_TEXT );
        GFL_BG_SetVisible( SND_TEST_BMPWIN_FRAME, VISIBLE_ON );
        GFL_BG_ClearFrame( SND_TEST_BMPWIN_FRAME );
        GFL_BG_LoadScreenReq( SND_TEST_BMPWIN_FRAME );
        
	}

	return;
}

//--------------------------------------------------------------
/**
 * @brief	�������b�Z�[�W�\��
 *
 * @param	win_index	�r�b�g�}�b�vINDEX
 * @param	num			���l
 * @param	x			�\���ʒuX
 * @param	y			�\���ʒuY
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void SndTestNumMsgSet( SND_TEST_WORK* wk, int num, u8 x, u8 y )
{
	const u16 strLen = 64;
#if 0
    STRBUF* buf = STRBUF_Create( 6, HEAPID_BASE_DEBUG );

	STRBUF_SetNumber( buf, num, 4, NUMBER_DISPTYPE_SPACE, NUMBER_CODETYPE_DEFAULT );
	GF_STR_PrintSimple( win, FONT_SYSTEM, buf, x, y, MSG_NO_PUT, NULL );
	STRBUF_Delete(buf);
#endif
    STRBUF* msg = GFL_STR_CreateBuffer( strLen , wk->heapId );

	STRTOOL_SetNumber( msg, num, 4, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
    
    PRINT_UTIL_Print(&wk->printUtil , wk->printQue , x, y, msg, wk->fontHandle);

	GFL_STR_DeleteBuffer(msg);
    
}

//--------------------------------------------------------------
/**
 * @brief	�T�E���h�l�[�����b�Z�[�W�\��
 *
 * @param	win_index	�r�b�g�}�b�vINDEX
 * @param	num			���l
 * @param	x			�\���ʒuX
 * @param	y			�\���ʒuY
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void SndTestNameMsgSet( SND_TEST_WORK* wk, const STRBUF* msg, u8 x, u8 y )
{
//	GF_STR_PrintSimple( win, FONT_SYSTEM, msg, x, y, MSG_NO_PUT, NULL );

    PRINT_UTIL_Print(&wk->printUtil , wk->printQue , x, y, msg, wk->fontHandle);

}

//------------------------------------------------------------------
/**
 * �Œ蕶����o��
 *
 * @param   win			BitmapWindow
 * @param   fontID		�t�H���g
 * @param   strID		������h�c
 * @param   x			�`��w���W
 * @param   y			�`��x���W
 * @param   wait		�E�F�C�g
 * @param   callback	�R�[���o�b�N
 *
 */
//------------------------------------------------------------------
static void SndTestStrPrint( SND_TEST_WORK* wk, u32 strID, u32 x, u32 y )
{
	STRBUF *strbuf;
	const u16 strLen = 64;
	GFL_MSGDATA *msgMng = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_snd_test_str_dat, wk->heapId);

    strbuf = GFL_STR_CreateBuffer( strLen , wk->heapId );
    GFL_MSG_GetString(msgMng, strID, strbuf);
    PRINT_UTIL_Print(&wk->printUtil , wk->printQue , x, y, strbuf, wk->fontHandle);
	GFL_STR_DeleteBuffer(strbuf);
	GFL_MSG_Delete(msgMng);
}

//--------------------------------------------------------------
/**
 * @brief	BGM���b�Z�[�W�ĕ\��
 *
 * @param	wk		SND_TEST_WORK�ւ̃|�C���^
 * @param	select	�I��Ώ�
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void MsgRewrite( SND_TEST_WORK* wk, s8 select )
{
	int type;
	u32 msg_h_id;

	switch( select ){
	case SND_TEST_TYPE_BGM:
		//wk->bgm,se,pv�ɂ́A�V�[�P���X�i���o�[�������Ă���̂ŁA
		//�X�^�[�g�i���o�[�������A�I�����Ă���l�ɂ��āA
		//���b�Z�[�WID�̃X�^�[�g�i���o�[�𑫂����b�Z�[�W��\������
		msg_h_id = wk->bgm - SND_TEST_BGM_START_NO + msg_seq_pv_end + 1;

		//�w��͈͂�h��Ԃ�
		GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->bmpWin), ST_TYPE_X, ST_BGM_MSG_Y+16, 8*24, 16*1 , 
							FBMP_COL_WHITE);

		SndTestStrPrint( wk,  msg_BGM, 
						ST_TYPE_X, ST_BGM_MSG_Y );
		SndTestNumMsgSet( wk, wk->bgm, ST_TYPE_X, ST_BGM_MSG_Y+16 );

		GFL_MSG_GetString( wk->msgman, msg_h_id, wk->msg_buf[BUF_BGM] );
		SndTestNameMsgSet( wk, wk->msg_buf[BUF_BGM], ST_NAME_X, ST_BGM_MSG_Y+16 );

		SndTestStrPrint( wk,  msg_ME, 
						ST_TYPE_X+32, ST_BGM_MSG_Y );

		SndTestStrPrint( wk,  msg_KEY, 
						ST_TYPE_X+56, ST_BGM_MSG_Y );

		SndTestStrPrint( wk,  msg_TMP, 
						ST_TYPE_X+136, ST_BGM_MSG_Y );

		break;

	case SND_TEST_TYPE_SE:
		msg_h_id = wk->se - SND_TEST_SE_START_NO + msg_seq_bgm_end + 1;

		//�w��͈͂�h��Ԃ�
		GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->bmpWin)
							, ST_TYPE_X, ST_SE_MSG_Y+16, 8*24, 16*1,FBMP_COL_WHITE );

		//�V�[�P���X�i���o�[����ASE�̃v���C���[�i���o�[���擾
		type = Snd_GetPlayerNo(wk->se);
		type -= PLAYER_SE_1;

		//SE�̂݃v���C���[�i���o�[�\��
		SndTestStrPrint(wk,  msg_PLAYER, 
						ST_TYPE_X+32, ST_SE_MSG_Y );
		SndTestNumMsgSet( wk, type, ST_TYPE_X+32+56, ST_SE_MSG_Y );

		SndTestStrPrint( wk,  msg_SE, 
						ST_TYPE_X, ST_SE_MSG_Y );

		SndTestNumMsgSet( wk, wk->se, ST_TYPE_X, ST_SE_MSG_Y+16 );

		GFL_MSG_GetString( wk->msgman, msg_h_id, wk->msg_buf[BUF_SE] );
		SndTestNameMsgSet( wk, wk->msg_buf[BUF_SE], ST_NAME_X, ST_SE_MSG_Y+16 );
		break;

	case SND_TEST_TYPE_PV:
		msg_h_id = wk->pv - SND_TEST_PV_START_NO + msg_seq_pv001;

		//�w��͈͂�h��Ԃ�
		GFL_BMP_Fill(GFL_BMPWIN_GetBmp(wk->bmpWin)
							, ST_TYPE_X, ST_PV_MSG_Y+16, 8*24, 16*1,FBMP_COL_WHITE );

		SndTestStrPrint( wk,  msg_PV, 
						ST_TYPE_X, ST_PV_MSG_Y );
		SndTestNumMsgSet( wk, wk->pv, ST_TYPE_X, ST_PV_MSG_Y+16 );

		GFL_MSG_GetString( wk->msgman, msg_h_id, wk->msg_buf[BUF_PV] );
		SndTestNameMsgSet( wk, wk->msg_buf[BUF_PV], ST_NAME_X, ST_PV_MSG_Y+16 );
		break;
	};

	return;
}

//--------------------------------------------------------------
/**
 * @brief	������Ԃɂ���(X_BUTTON�����������̏������I)
 *
 * @param	wk		SND_TEST_WORK�ւ̃|�C���^
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void SndTestInit( SND_TEST_WORK* wk )
{
	//���[�N������
	SndTestWorkInit( wk );

	//��������
	MsgRewrite( wk, SND_TEST_TYPE_BGM );		//BGM�i���o�[
	MsgRewrite( wk, SND_TEST_TYPE_SE );			//SE�i���o�[
	MsgRewrite( wk, SND_TEST_TYPE_PV );			//PV�i���o�[

	SndTestCursor( wk );						//�J�[�\���X�V

	return;
}

//--------------------------------------------------------------
/**
 * @brief	�J�[�\��
 *
 * @param	wk		SND_TEST_WORK�ւ̃|�C���^
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void SndTestCursor( SND_TEST_WORK* wk )
{
    u16 y;

	switch( wk->select ){
	case SND_TEST_TYPE_BGM:
		y = ST_BGM_MSG_Y;
		break;
	case SND_TEST_TYPE_SE:
		y = ST_SE_MSG_Y;
		break;
	case SND_TEST_TYPE_PV:
		y = ST_PV_MSG_Y;
		break;
	};

	//�w��͈͂�h��Ԃ�
	GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->bmpWin), ST_CURSOR_X, ST_BGM_MSG_Y, 8*1, 16*8,FBMP_COL_WHITE );
	GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->bmpWin), ST_CURSOR_X, y, 8*1, 16*1, FBMP_COL_RED );

	return;
}

//--------------------------------------------------------------
/**
 * @brief	�V�X�e�����b�Z�[�W
 *
 * @param	wk		SND_TEST_WORK�ւ̃|�C���^
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void SndTestSysMsgSet( SND_TEST_WORK* wk )
{
	SndTestStrPrint( wk,  msg_SOUND_TEST, 
				ST_TITLE_X, ST_TITLE_Y );			//SOUND TEST
	SndTestStrPrint( wk,  msg_A_PLAY, 
				ST_KEY_X, ST_KEY_Y );				//A PLAY
	SndTestStrPrint( wk,  msg_B_STOP, 
				ST_KEY_X+64, ST_KEY_Y );			//B STOP
	SndTestStrPrint( wk,  msg_X_END, 
				ST_KEY_X, ST_KEY_Y+16 );			//X END
	SndTestStrPrint( wk,  msg_Y_INIT, 
				ST_KEY_X+64, ST_KEY_Y+16 );		//Y INIT

	return;
}


//==============================================================================================
//
//	PROC�֘A
//
//==============================================================================================

//--------------------------------------------------------------
//--------------------------------------------------------------
static GFL_PROC_RESULT SoundTestProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	SND_TEST_WORK* wk = NULL;

	wk = GFL_PROC_AllocWork(proc, sizeof(SND_TEST_WORK), GFL_HEAPID_APP);

    GFL_STD_MemClear( wk,  sizeof(SND_TEST_WORK) );
    wk->heapId = GFL_HEAPID_APP;

	SndTestCall(wk);
	return GFL_PROC_RES_FINISH;
		
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GFL_PROC_RESULT SoundTestProc_Main(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	SND_TEST_WORK *wk;
	wk = mywk;

	SndTestTable[wk->seq](wk);

	PRINTSYS_QUE_Main( wk->printQue );
	PRINT_UTIL_Trans( &wk->printUtil ,wk->printQue );

    
	if (wk->end_flag) {
		return GFL_PROC_RES_FINISH;
	} else {
		return GFL_PROC_RES_CONTINUE;
	}
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GFL_PROC_RESULT SoundTestProc_End(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	GFL_PROC_FreeWork(proc);
	//Main_SetNextProc(NO_OVERLAY_ID, &TitleProcData);

	//�\�t�g���Z�b�g
	//�ڍׂ̓\�[�X�擪�́u�T�E���h�e�X�g�̗�O�����ɂ��āv���Q�Ƃ��ĉ�����
	//OS_InitReset();
//	OS_ResetSystem(0);									//�\�t�g���Z�b�g
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
const GFL_PROC_DATA DebugSoundProcData = {
	SoundTestProc_Init,
	SoundTestProc_Main,
	SoundTestProc_End,
};


