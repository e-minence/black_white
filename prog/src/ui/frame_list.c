//============================================================================================
/**
 * @file		frame_list.c
 * @brief		�a�f�t���[���S�̂��g�p�������X�g����
 * @author	Hiroyuki Nakamura
 * @date		10.01.23
 *
 *	���W���[�����FFRAMELIST
 *
 *	�E�P��ʂɂ��A�a�f�P�ʂ��L���܂��i�X�N���[�����邽�߁j
 *	�E������BMPWIN���m�ۂ��Ă��邽�߁ABMPWIN�̏�������Ƀ��X�g���쐬���Ă�������
 */
//============================================================================================
#include <gflib.h>

#include "system/bgwinfrm.h"
#include "system/blink_palanm.h"
#include "system/scroll_bar.h"
#include "sound/pm_sndsys.h"

#include "ui/frame_list.h"


//============================================================================================
//	�萔��`
//============================================================================================

// ���ڃf�[�^
typedef struct {
	u32	type;					// �w�i�ԍ�
	u32	param;				// �߂�l
}FL_ITEM;

// �w�i�f�[�^
typedef struct {
	u16 * scrn;
}FL_GRA;

// PRINT_UTIL
typedef struct {
	PRINT_UTIL	util;
}FL_PRINT_UTIL;

// �a�f�t���[�����X�g���[�N
struct _FRAMELIST_WORK {
	FRAMELIST_HEADER	hed;				// �w�b�_�[�f�[�^

	FL_ITEM * item;								// ���ڃf�[�^

	GFL_UI_TP_HITTBL * touch;			// �^�b�`�f�[�^

	BLINKPALANM_WORK * blink;			// �_�Ńp���b�g�A�j��

	PRINT_QUE * que;							// �v�����g�L���[
	FL_PRINT_UTIL * printUtil;		// PRINT_UTIL
	BGWINFRM_WORK * wfrm;					// �E�B���h�E�t���[��
	FL_GRA * wfrmGra;							// �w�i�ۑ��ꏊ
	u8	printMax;									// �t���[����
	u8	printSubPos;							// ���ʂɕ\������t���[���ԍ��J�n�ʒu
	s8	putFrameMain;							// �z�u��t���[���i���C���j
	s8	putFrameSub;							// �z�u��t���[���i�T�u�j
	u8	putFrameMaxMain;					// ���ۂɔz�u�����t���[�����i���C���j
	u8	putFrameMaxSub;						// ���ۂɔz�u�����t���[�����i�T�u�j

	s8	subFramePy;				// �T�u��ʂɔz�u����t���[���̂x�␳�l

	u16	listMax;					// ���X�g�o�^��

	u16	mainSeq;					// ���C���V�[�P���X
	u16	nextSeq;					// ���A�V�[�P���X

	u32	nowTpy;						// ���݂̃^�b�`�x���W
	u32	oldTpy;						// �O��̃^�b�`�x���W

	s16	listPos;					// �J�[�\���\���ʒu
	u16	listPosMax;				// �J�[�\���ړ��ő�l
	s16	listOldPos;				// �O��̃J�[�\���ʒu

	s16	listScroll;				// ���X�g�X�N���[���l
	u16	listScrollMax;		// ���X�g�X�N���[���ő�l

	s8	listBgScoll;				// �a�f�X�N���[���l
	s8	listBgScrollSpeed;	// �a�f�X�N���[�����x
	u8	listBgScrollMax;		// �a�f�X�N���[����
	u8	listBgScrollCount;	// �a�f�X�N���[���J�E���^

	BOOL scrollSE;					// �X�N���[�����̂r�d�Đ��t���O

	BOOL autoScroll;				// �����X�N���[���t���O

	GFL_UI_TP_HITTBL	railHit[2];		// ���[���ړ��p�^�b�`�e�[�u��
	u8	railHitPos;									// ���[���^�b�`�e�[�u���ʒu
	u8	railTop;										// ���[���ŏ㕔�̂x���W
	u8	railBottom;									// ���[���ŉ����̂x���W

	u8	slidePos;						// �X���C�h����ڈʒu
	u8	slideCnt;						// �X���C�h�����e�[�u���ʒu

	u8	keyRepeat;					// �L�[���s�[�g�J�E���^
	u8	keyRepPos;					// �L�[���s�[�g�����e�[�u���ʒu
	u8	listWait;						// �L�[���s�[�g�E�F�C�g

	HEAPID	heapID;								// �q�[�v�h�c
};

// ���C���V�[�P���X
enum {
	MAINSEQ_MAIN = 0,
	MAINSEQ_SCROLL,
	MAINSEQ_RAIL,
	MAINSEQ_SLIDE,
	MAINSEQ_WAIT,
};

// �R�}���h
enum {
	COMMAND_CURSOR_ON = 0,					// �J�[�\���\��
	COMMAND_CURSOR_MOVE,						// �J�[�\���ړ�

	COMMAND_SCROLL_UP,							// �X�N���[���F��
	COMMAND_SCROLL_DOWN,						// �X�N���[���F��

	COMMAND_SCROLL_UP_1P,						// �X�N���[���F�P�y�[�W��
	COMMAND_SCROLL_DOWN_1P,					// �X�N���[���F�P�y�[�W��

	COMMAND_LIST_TOP,								// ���X�g��ԏ�܂ňړ�
	COMMAND_LIST_BOTTOM,						// ���X�g��ԉ��܂ňړ�

	COMMAND_RAIL,										// ���[���ړ�

	COMMAND_SLIDE,									// �X���C�h������

	COMMAND_SELECT,									// �I��

	COMMAND_ERROR,									// �R�}���h����
	COMMAND_NONE,										// ����Ȃ�
};


// �a�f�X�N���[���L�����T�C�Y�i��ʕ\���͈́j
#define	BGSCRN_CHR_SIZE_X		( 32 )
#define	BGSCRN_CHR_SIZE_Y		( 24 )

#define	PALCHG_NONE		( 0xff )			// �p���b�g�`�F���W����

#define	KEYWAIT_LIST_MOVE					( 8 )			// �L�[���͌�Ɏ��̓��͂�������E�F�C�g�i�J�[�\���ړ���j
#define	KEYWAIT_LIST_SCROLL				( 0 )			// �L�[���͌�Ɏ��̓��͂�������E�F�C�g�i�X�N���[����j
#define	KEYWAIT_LIST_PAGE_MOVE		( 4 )			// �L�[���͌�Ɏ��̓��͂�������E�F�C�g�i�y�[�W�ړ���j

#define	RAIL_AREA_X				( 0 )				// ���[�������̃^�b�`�͈͂w�␳�l
#define	RAIL_AREA_Y				( 32 )			// ���[�������̃^�b�`�͈͂x�␳�l
#define	RAIL_AREA_X_MAX		( 255 )			// ���[�������̃^�b�`�͈͂w�ő�l
#define	RAIL_AREA_Y_MAX		( 191 )			// ���[�������̃^�b�`�͈͂x�ő�l

// �L�[�E�^�b�`�؂�ւ��ɑΉ������{�^��
#define	KEY_TOUCH_CHG_TRG		( PAD_KEY_UP|PAD_KEY_DOWN|PAD_KEY_LEFT|PAD_KEY_RIGHT|PAD_BUTTON_DECIDE|PAD_BUTTON_L|PAD_BUTTON_R )


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void InitListDraw( FRAMELIST_WORK * wk, BOOL flg );
static void InitListData( FRAMELIST_WORK * wk );

static u32 MoveListMain( FRAMELIST_WORK * wk );
static void SetListScrollSpeed( FRAMELIST_WORK * wk );

static void InitListScroll( FRAMELIST_WORK * wk, s8 vec, u8 max, s16 next, BOOL se );
static BOOL MainListScroll( FRAMELIST_WORK * wk );

static void InitRailMove( FRAMELIST_WORK * wk, int pos );
static BOOL MainRailMove( FRAMELIST_WORK * wk );

static void InitSlideMove( FRAMELIST_WORK * wk, int pos );
static BOOL MainSlideMove( FRAMELIST_WORK * wk );

static void WriteItemFrame( FRAMELIST_WORK * wk, u32 itemNum, u32 frmNum );
static void PutItemFrame( FRAMELIST_WORK * wk, u32 idx, s8 py );
static s8 GetItemFramePosY( FRAMELIST_WORK * wk, s8 vec );
static s8 GetNextFrameNum( s8 now, s8 max, s8 vec );
static void DrawListItemMain( FRAMELIST_WORK * wk, s32 pos, s8 py );
static void DrawListItemSub( FRAMELIST_WORK * wk, s32 pos, s8 py );
static void ChangeCursorPosPalette( FRAMELIST_WORK * wk, u16 now, u16 old );
static void PrintTransMain( FRAMELIST_WORK * wk );


//============================================================================================
//	�O���[�o��
//============================================================================================

// �I�[�g�X�N���[���J�E���^
static const u8 AutoScrollCount[] = {
	128, 64, 32, 16, 8, 4
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�t���[�����X�g�쐬
 *
 * @param		hed			�w�b�_�[�f�[�^
 * @param		heapID	�q�[�v�h�c
 *
 * @return	���[�N
 */
//--------------------------------------------------------------------------------------------
FRAMELIST_WORK * FRAMELIST_Create( FRAMELIST_HEADER * hed, HEAPID heapID )
{
	FRAMELIST_WORK * wk;
	u32	i;
	
	wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FRAMELIST_WORK) );

	wk->hed    = *hed;
	wk->heapID = heapID;

	wk->nowTpy = 0xffffffff;
	wk->oldTpy = 0xffffffff;

	// �v�����g�L���[�쐬
//	wk->que  = PRINTSYS_QUE_Create( heapID );
	wk->que  = PRINTSYS_QUE_CreateEx( 2048, heapID );
	// �_�ŃA�j���쐬
	wk->blink = BLINKPALANM_Create( wk->hed.selPal*16, 16, wk->hed.mainBG, wk->heapID );

	// ���ڃf�[�^�̈�m��
	wk->item = GFL_HEAP_AllocClearMemory( wk->heapID, sizeof(FL_ITEM)*wk->hed.itemMax );

	// �w�i�O���t�B�b�N�̈�m��
	if( wk->hed.graMax != 0 ){
		u32	i;
		wk->wfrmGra = GFL_HEAP_AllocMemory( wk->heapID, sizeof(FL_GRA)*wk->hed.graMax );
		for( i=0; i<wk->hed.graMax; i++ ){
			wk->wfrmGra[i].scrn = GFL_HEAP_AllocMemory( wk->heapID, wk->hed.itemSizX*wk->hed.itemSizY*2 );
		}
	}

	// BGWINFRM�쐬 & BMPWIN�쐬
	wk->printMax = BGSCRN_CHR_SIZE_Y / wk->hed.itemSizY + 2;		// �ő�\���t���[�����{�␳���{����������
	wk->printSubPos = wk->printMax;
	if( wk->hed.subBG != FRAMELIST_SUB_BG_NONE ){
		wk->printMax *= 2;
	}
	wk->wfrm = BGWINFRM_Create( BGWINFRM_TRANS_VBLANK, wk->printMax, wk->heapID );

	wk->printUtil = GFL_HEAP_AllocMemory( wk->heapID, sizeof(FL_PRINT_UTIL)*wk->printMax );

	for( i=0; i<wk->printMax; i++ ){
		if( wk->hed.subBG != FRAMELIST_SUB_BG_NONE && wk->printSubPos <= i ){
			BGWINFRM_Add( wk->wfrm, i, wk->hed.subBG, wk->hed.itemSizX, wk->hed.itemSizY );
			wk->printUtil[i].util.win = GFL_BMPWIN_Create(
																		wk->hed.subBG,
																		wk->hed.bmpPosX, wk->hed.bmpPosY,
																		wk->hed.bmpSizX, wk->hed.bmpSizY,
																		wk->hed.bmpPal, GFL_BMP_CHRAREA_GET_B );
		}else{
			BGWINFRM_Add( wk->wfrm, i, wk->hed.mainBG, wk->hed.itemSizX, wk->hed.itemSizY );
			wk->printUtil[i].util.win = GFL_BMPWIN_Create(
																		wk->hed.mainBG,
																		wk->hed.bmpPosX, wk->hed.bmpPosY,
																		wk->hed.bmpSizX, wk->hed.bmpSizY,
																		wk->hed.bmpPal, GFL_BMP_CHRAREA_GET_B );
		}
	}

	wk->subFramePy = BGSCRN_CHR_SIZE_Y;
	while( 1 ){
		wk->subFramePy -= wk->hed.itemSizY;
		if( wk->subFramePy <= 0 ){
			break;
		}
	}

	// �^�b�`�f�[�^�쐬
	if( wk->hed.touch != NULL ){
		u32	max = 0;
		i = 0;
		while( 1 ){
			if( wk->hed.touch[max].tbl.rect.top == GFL_UI_TP_HIT_END ){
				max++;
				break;
			}
			max++;
		}
		wk->touch = GFL_HEAP_AllocMemory( wk->heapID, sizeof(GFL_UI_TP_HITTBL)*max );
		for( i=0; i<max; i++ ){
			GFL_STD_MemCopy32( &wk->hed.touch[i].tbl, &wk->touch[i], sizeof(GFL_UI_TP_HITTBL) );
		}
	}

	return wk;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�t���[�����X�g�폜
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_Exit( FRAMELIST_WORK * wk )
{
	u32	i;

  GF_ASSERT( wk );

	// �^�b�`�f�[�^�폜
	GFL_HEAP_FreeMemory( wk->touch );

	// BMPWIN�폜
	for( i=0; i<wk->printMax; i++ ){
		GFL_BMPWIN_Delete( wk->printUtil[i].util.win );
	}
	GFL_HEAP_FreeMemory( wk->printUtil );

	// BGWINFRM�폜
	BGWINFRM_Exit( wk->wfrm );

	// �w�i�O���t�B�b�N�폜
	if( wk->hed.graMax != 0 ){
		for( i=0; i<wk->hed.graMax; i++ ){
			GFL_HEAP_FreeMemory( wk->wfrmGra[i].scrn );
		}
		GFL_HEAP_FreeMemory( wk->wfrmGra );
	}

	// ���ڃf�[�^�폜
	GFL_HEAP_FreeMemory( wk->item );

	// �_�ŃA�j���폜
	BLINKPALANM_Exit( wk->blink );
	// �v�����g�L���[�폜
	PRINTSYS_QUE_Delete( wk->que );

	// �a�f�t���[�����X�g���[�N�폜
	GFL_HEAP_FreeMemory( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���ڒǉ�
 *
 * @param		wk			���[�N
 * @param		type		�w�i�ԍ�
 * @param		param		�p�����[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_AddItem( FRAMELIST_WORK * wk, u32 type, u32 param )
{
  GF_ASSERT( wk );
  GF_ASSERT( wk->hed.itemMax > wk->listMax );
  GF_ASSERT( wk->hed.graMax > type );

	wk->item[wk->listMax].type  = type;
	wk->item[wk->listMax].param = param;
	wk->listMax++;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�w�i�O���t�B�b�N�ǂݍ���
 *
 * @param		wk				���[�N
 * @param		ah				�A�[�J�C�u�n���h��
 * @param		dataIdx		�f�[�^�C���f�b�N�X
 * @param		comp			���k�t���O TRUE = ���k
 * @param		frameNum	�w�i�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_LoadFrameGraphicAH( FRAMELIST_WORK * wk, ARCHANDLE * ah, u32 dataIdx, BOOL comp, u32 frameNum )
{
	NNSG2dScreenData * scrn;
	void * buf;

  GF_ASSERT( wk );
  GF_ASSERT( wk->hed.graMax > frameNum );

	buf = GFL_ARCHDL_UTIL_LoadScreen( ah, dataIdx, comp, &scrn, wk->heapID );

	GFL_STD_MemCopy16(
		(const void *)scrn->rawData,
		wk->wfrmGra[frameNum].scrn,
		wk->hed.itemSizX*wk->hed.itemSizY*2 );

	GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���p���b�g�A�j���p�f�[�^�ǂݍ���
 *
 * @param		wk				���[�N
 * @param		ah				�A�[�J�C�u�n���h��
 * @param		dataIdx		�f�[�^�C���f�b�N�X
 * @param		startPal	�p���b�g�P 0�`15
 * @param		endPal		�p���b�g�Q 0�`15
 *
 * @return	���[�N
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_LoadBlinkPalette( FRAMELIST_WORK * wk, ARCHANDLE * ah, u32 dataIdx, u32 startPal, u32 endPal )
{
  GF_ASSERT( wk );

	BLINKPALANM_SetPalBufferArcHDL( wk->blink, ah, dataIdx, startPal*16, endPal*16 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�t���[�����X�g�����`��
 *
 * @param		wk		���[�N
 *
 * @retval	"TRUE = ��������"
 * @retval	"FALSE = �������I��"
 */
//--------------------------------------------------------------------------------------------
BOOL FRAMELIST_Init( FRAMELIST_WORK * wk )
{
  GF_ASSERT( wk );

	switch( wk->mainSeq ){
	case 0:
		InitListData( wk );
		InitListDraw( wk, FALSE );
		wk->mainSeq++;
		break;

	case 1:
	  if( PRINTSYS_QUE_IsFinished( wk->que ) == TRUE ){
			wk->mainSeq = 0;
			return FALSE;
		}
		break;

  default:
		GF_ASSERT(0);
	}

	PrintTransMain( wk );

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�f�t���[�����X�g���C������
 *
 * @param		wk		���[�N
 *
 * @return	���쌋��
 */
//--------------------------------------------------------------------------------------------
u32 FRAMELIST_Main( FRAMELIST_WORK * wk )
{
	u32	ret;
	u32	x;
  
  GF_ASSERT( wk );
	
	ret = FRAMELIST_RET_NONE;

	if( GFL_UI_TP_GetPointCont( &x, &wk->nowTpy ) == FALSE ){
		wk->nowTpy = 0xffffffff;
	}

	switch( wk->mainSeq ){
	case MAINSEQ_MAIN:
		ret = MoveListMain( wk );
		break;

	case MAINSEQ_SCROLL:
		if( MainListScroll( wk ) == TRUE ){
			ret = FRAMELIST_RET_SCROLL;
		}
		break;

	case MAINSEQ_RAIL:
		if( MainRailMove( wk ) == TRUE ){
			ret = FRAMELIST_RET_RAIL;
		}
		break;

	case MAINSEQ_SLIDE:
		if( MainSlideMove( wk ) == TRUE ){
			ret = FRAMELIST_RET_SLIDE;
		}
		break;

	case MAINSEQ_WAIT:
		// �`�{�^���������ꂽ��I����Ԃ�
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY ){
			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
				ret = wk->listPos;
			}
		}
		if( wk->listWait == 0 ){
			wk->mainSeq = MAINSEQ_MAIN;
		}else{
			wk->listWait--;
		}
		break;

  default:
		GF_ASSERT(0);
	}

	wk->oldTpy = wk->nowTpy;

//	BGWINFRM_MoveMain( wk->wfrm );
	BLINKPALANM_Main( wk->blink );
	PrintTransMain( wk );

	return ret;
}


//============================================================================================
//	�����`��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g�f�[�^������
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitListData( FRAMELIST_WORK * wk )
{
	// �J�[�\���ړ��ő�l�ݒ�
	if( wk->hed.posMax > wk->listMax ){
		wk->listPosMax = wk->listMax;
	}else{
		wk->listPosMax = wk->hed.posMax;
	}

	// �X�N���[���ő�l�ݒ�
	if( wk->listMax < wk->hed.posMax ){
		wk->listScrollMax = 0;
	}else{
		wk->listScrollMax = wk->listMax - wk->hed.posMax;
	}

	// �J�[�\���ʒu�␳
	if( wk->hed.initPos > wk->listPosMax ||
			wk->hed.initScroll > wk->listScrollMax ){
		wk->listPos    = 0;
		wk->listScroll = 0;
	}else{
		wk->listPos    = wk->hed.initPos;
		wk->listScroll = wk->hed.initScroll;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g�����`��
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitListDraw( FRAMELIST_WORK * wk, BOOL flg )
{
	s16	pos;
	s8	py;

	GFL_BG_SetScrollReq( wk->hed.mainBG, GFL_BG_SCROLL_Y_SET, 0 );
	wk->listBgScoll = 0;

	pos = wk->listScroll;
	py  = wk->hed.itemPosY;
	while( 1 ){
		if( pos == 0 || py <= 0 ){
			break;
		}
		pos--;
		py -= wk->hed.itemSizY;
	}

	wk->putFrameMain = 0;

	while( 1 ){
//		if( py >= BGSCRN_CHR_SIZE_Y || pos >= wk->listMax ){
		if( py >= BGSCRN_CHR_SIZE_Y ){
			break;
		}

		DrawListItemMain( wk, pos, py );
		wk->putFrameMain++;

		pos++;
		py += wk->hed.itemSizY;
	}

	wk->putFrameMaxMain = wk->putFrameMain;

	// �T�u���
	if( wk->hed.subBG == FRAMELIST_SUB_BG_NONE ){ return; }

	GFL_BG_SetScrollReq( wk->hed.subBG, GFL_BG_SCROLL_Y_SET, 0 );

	py  = BGSCRN_CHR_SIZE_Y + wk->hed.itemPosY - wk->hed.itemSizY;
	pos = wk->listScroll - 1;
	while( 1 ){
		if( py <= 0 ){
			break;
		}
		pos--;
		py -= wk->hed.itemSizY;
	}

	wk->putFrameSub = 0;

	while( 1 ){
		if( py >= BGSCRN_CHR_SIZE_Y ){
			break;
		}

		DrawListItemSub( wk, pos, py );
		wk->putFrameSub++;

		pos++;
		py += wk->hed.itemSizY;
	}

	wk->putFrameMaxSub = wk->putFrameSub;

	ChangeCursorPosPalette( wk, wk->listPos, PALCHG_NONE );
	wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, flg );
}


//============================================================================================
//	���C���R���g���[��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�b�`�`�F�b�N
 *
 * @param		wk		���[�N
 *
 * @return	�^�b�`�ɑΉ������R�}���h
 */
//--------------------------------------------------------------------------------------------
static int MoveListTouch( FRAMELIST_WORK * wk )
{
	int	ret;

	if( wk->touch == NULL ){
		return COMMAND_NONE;
	}

	ret = GFL_UI_TP_HitTrg( wk->touch );

	if( ret == GFL_UI_TP_HIT_NONE ){
		return COMMAND_NONE;
	}

	GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );

	switch( wk->hed.touch[ret].prm ){
	case FRAMELIST_TOUCH_PARAM_ITEM:					// ����
		if( ret >= wk->listPosMax ){
			return COMMAND_ERROR;					// �R�}���h����
		}
		wk->listOldPos = wk->listPos;
		wk->listPos = ret;
//		return COMMAND_CURSOR_MOVE;		// �J�[�\���ړ�
		return COMMAND_SELECT;					// �I��

	case FRAMELIST_TOUCH_PARAM_SLIDE:
		if( ret >= wk->listPosMax ){
			return COMMAND_ERROR;					// �R�}���h����
		}
		wk->listOldPos = wk->listPos;
		wk->listPos = ret;
		return COMMAND_SLIDE;

	case FRAMELIST_TOUCH_PARAM_RAIL:					// ���[��
		wk->railHitPos = ret;
		wk->listOldPos = wk->listPos;
//		wk->listPos = 0;
		return COMMAND_RAIL;

	case FRAMELIST_TOUCH_PARAM_UP:						// ��ړ�
		if( wk->listScroll != 0 ){
			wk->keyRepPos = 0;
			return COMMAND_SCROLL_UP;		// �X�N���[���F��
		}
		break;

	case FRAMELIST_TOUCH_PARAM_DOWN:					// ���ړ�
		if( wk->listScroll < wk->listScrollMax ){
			wk->keyRepPos = 0;
			return COMMAND_SCROLL_DOWN;	// �X�N���[���F��
		}
		break;

	case FRAMELIST_TOUCH_PARAM_PAGE_UP:				// �y�[�W��ړ�
		if( wk->listScroll != 0 ){
			return COMMAND_SCROLL_UP_1P;	// �X�N���[���F�P�y�[�W��
		}else if( wk->listPos != 0 ){
			wk->listOldPos = wk->listPos;
			wk->listPos = 0;
			return COMMAND_CURSOR_MOVE;		// �J�[�\���ړ�
		}
		break;

	case FRAMELIST_TOUCH_PARAM_PAGE_DOWN:			// �y�[�W���ړ�
		if( wk->listScroll < wk->listScrollMax ){
			return COMMAND_SCROLL_DOWN_1P;	// �X�N���[���F�P�y�[�W��
		}else if( wk->listPos < (wk->listPosMax-1) ){
			wk->listOldPos = wk->listPos;
			wk->listPos = wk->listPosMax - 1;
			return COMMAND_CURSOR_MOVE;		// �J�[�\���ړ�
		}
		break;

	case FRAMELIST_TOUCH_PARAM_LIST_TOP:			// ���X�g�ŏ�ʂ�
		if( wk->listPos != 0 || wk->listScroll != 0 ){
			return COMMAND_LIST_TOP;		// ���X�g��ԏ�܂ňړ�
		}
		break;

	case FRAMELIST_TOUCH_PARAM_LIST_BOTTOM:		// ���X�g�ŉ��ʂ�
		if( wk->listPos != (wk->listPosMax-1) || wk->listScroll != wk->listScrollMax ){
			return COMMAND_LIST_BOTTOM;		// ���X�g��ԉ��܂ňړ�
		}
		break;

  default:
		GF_ASSERT(0);
	}

	return COMMAND_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�L�[�`�F�b�N
 *
 * @param		wk		���[�N
 *
 * @return	�L�[�ɑΉ������R�}���h
 */
//--------------------------------------------------------------------------------------------
static int MoveListKey( FRAMELIST_WORK * wk )
{
	if( GFL_UI_KEY_GetTrg() & KEY_TOUCH_CHG_TRG ){
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
			GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
			wk->listOldPos = PALCHG_NONE;
			wk->listWait = KEYWAIT_LIST_MOVE;
			return COMMAND_CURSOR_ON;	// �J�[�\���\��
		}
	}

	if( !(GFL_UI_KEY_GetCont()) ){
		wk->keyRepeat = 0;
		wk->keyRepPos = 6;
	}

	if( GFL_UI_KEY_GetCont() & PAD_KEY_UP  ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		if( wk->listPos != 0 ){
			wk->listOldPos = wk->listPos;
			wk->listPos--;
			wk->listWait = KEYWAIT_LIST_MOVE;
			return COMMAND_CURSOR_MOVE;	// �J�[�\���ړ�
		}else if( wk->listScroll != 0 ){
			wk->listWait = KEYWAIT_LIST_SCROLL;
			SetListScrollSpeed( wk );
			return COMMAND_SCROLL_UP;		// �X�N���[���F��
		}
		return COMMAND_NONE;					// ����Ȃ�
	}

	if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		if( wk->listPos < (wk->listPosMax-1) ){
			wk->listOldPos = wk->listPos;
			wk->listPos++;
			wk->listWait = KEYWAIT_LIST_MOVE;
			return COMMAND_CURSOR_MOVE;	// �J�[�\���ړ�
		}else if( wk->listScroll < wk->listScrollMax ){
			wk->listWait = KEYWAIT_LIST_SCROLL;
			SetListScrollSpeed( wk );
			return COMMAND_SCROLL_DOWN;	// �X�N���[���F��
		}
		return COMMAND_NONE;					// ����Ȃ�
	}

	if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		if( wk->listScroll != 0 ){
			wk->listWait = KEYWAIT_LIST_PAGE_MOVE;
			return COMMAND_SCROLL_UP_1P;	// �X�N���[���F�P�y�[�W��
		}else if( wk->listPos != 0 ){
			wk->listOldPos = wk->listPos;
			wk->listPos = 0;
			wk->listWait = KEYWAIT_LIST_PAGE_MOVE;
			return COMMAND_CURSOR_MOVE;		// �J�[�\���ړ�
		}
		return COMMAND_NONE;						// ����Ȃ�
	}

	if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		if( wk->listScroll < wk->listScrollMax ){
			wk->listWait = KEYWAIT_LIST_PAGE_MOVE;
			return COMMAND_SCROLL_DOWN_1P;	// �X�N���[���F�P�y�[�W��
		}else if( wk->listPos < (wk->listPosMax-1) ){
			wk->listOldPos = wk->listPos;
			wk->listPos = wk->listPosMax - 1;
			wk->listWait = KEYWAIT_LIST_PAGE_MOVE;
			return COMMAND_CURSOR_MOVE;		// �J�[�\���ړ�
		}
		return COMMAND_NONE;						// ����Ȃ�
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
		return COMMAND_SELECT;					// �I��
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		if( wk->listPos != 0 || wk->listScroll != 0 ){
			return COMMAND_LIST_TOP;		// ���X�g��ԏ�܂ňړ�
		}
		return COMMAND_NONE;					// ����Ȃ�
	}

	// ���X�g�̈�ԉ��ֈړ�
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		if( wk->listPos != (wk->listPosMax-1) || wk->listScroll != wk->listScrollMax ){
			return COMMAND_LIST_BOTTOM;		// ���X�g��ԉ��܂ňړ�
		}
		return COMMAND_NONE;						// ����Ȃ�
	}

	return COMMAND_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C���R���g���[��
 *
 * @param		wk		���[�N
 *
 * @return	�O���֖߂��l
 */
//--------------------------------------------------------------------------------------------
static u32 MoveListMain( FRAMELIST_WORK * wk )
{
	int	ret;

	if( PRINTSYS_QUE_IsFinished( wk->que ) == FALSE ){
		return FRAMELIST_RET_NONE;
	}

	ret = MoveListTouch( wk );
	if( ret == COMMAND_NONE ){
		ret = MoveListKey( wk );
	}

	switch( ret ){
	case COMMAND_CURSOR_ON:				// �J�[�\���\��
		PMSND_PlaySE( SEQ_SE_SELECT1 );
		ChangeCursorPosPalette( wk, wk->listPos, wk->listOldPos );
		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );
		wk->mainSeq = MAINSEQ_WAIT;
		return FRAMELIST_RET_CURSOR_ON;

	case COMMAND_CURSOR_MOVE:			// �J�[�\���ړ�
		PMSND_PlaySE( SEQ_SE_SELECT1 );
		ChangeCursorPosPalette( wk, wk->listPos, wk->listOldPos );
		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );
		wk->mainSeq = MAINSEQ_WAIT;
		return FRAMELIST_RET_MOVE;

	case COMMAND_SCROLL_UP:				// �X�N���[���F��
//		PMSND_PlaySE( SEQ_SE_SELECT1 );
		InitListScroll( wk, -wk->hed.scrollSpeed[wk->keyRepPos], 1, MAINSEQ_WAIT, TRUE );
		return FRAMELIST_RET_SCROLL;

	case COMMAND_SCROLL_DOWN:			// �X�N���[���F��
//		PMSND_PlaySE( SEQ_SE_SELECT1 );
		InitListScroll( wk, wk->hed.scrollSpeed[wk->keyRepPos], 1, MAINSEQ_WAIT, TRUE );
		return FRAMELIST_RET_SCROLL;

	case COMMAND_SCROLL_UP_1P:		// �X�N���[���F�P�y�[�W��
		{
			s16	cnt = wk->listPosMax;
			wk->listOldPos = wk->listPos;
			if( ( wk->listScroll - cnt ) < 0 ){
				wk->listPos = 0;
				cnt = wk->listScroll;
			}
			ChangeCursorPosPalette( wk, PALCHG_NONE, wk->listOldPos );
			InitListScroll( wk, -wk->hed.scrollSpeed[0], cnt, MAINSEQ_WAIT, FALSE );
		}
		PMSND_PlaySE( SEQ_SE_SELECT1 );
		return FRAMELIST_RET_PAGE_UP;

	case COMMAND_SCROLL_DOWN_1P:	// �X�N���[���F�P�y�[�W��
		{
			s16	cnt = wk->listPosMax;
			wk->listOldPos = wk->listPos;
			if( ( wk->listScroll + cnt ) > wk->listScrollMax ){
				wk->listPos = wk->listPosMax - 1;
				cnt = wk->listScrollMax - wk->listScroll;
			}
			ChangeCursorPosPalette( wk, PALCHG_NONE, wk->listOldPos );
			InitListScroll( wk, wk->hed.scrollSpeed[0], cnt, MAINSEQ_WAIT, FALSE );
		}
		PMSND_PlaySE( SEQ_SE_SELECT1 );
		return FRAMELIST_RET_PAGE_DOWN;

	case COMMAND_LIST_TOP:				// ���X�g��ԏ�܂ňړ�
		wk->listPos = 0;
		wk->listScroll = 0;
		InitListDraw( wk, TRUE );
//		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll );
		PMSND_PlaySE( SEQ_SE_SELECT1 );
		return FRAMELIST_RET_JUMP_TOP;

	case COMMAND_LIST_BOTTOM:			// ���X�g��ԉ��܂ňړ�
		wk->listPos = wk->listPosMax-1;
		wk->listScroll = wk->listScrollMax;
		InitListDraw( wk, TRUE );
//		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll );
		PMSND_PlaySE( SEQ_SE_SELECT1 );
		return FRAMELIST_RET_JUMP_BOTTOM;

	case COMMAND_RAIL:						// ���[���ړ�
		ChangeCursorPosPalette( wk, PALCHG_NONE, wk->listOldPos );
//		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll );
		InitRailMove( wk, wk->railHitPos );
		PMSND_PlaySE( SEQ_SE_SYS_06 );
		return FRAMELIST_RET_RAIL;

	case COMMAND_SLIDE:
		ChangeCursorPosPalette( wk, PALCHG_NONE, wk->listOldPos );
		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );
		InitSlideMove( wk, wk->listPos );
		PMSND_PlaySE( SEQ_SE_SYS_06 );
		return FRAMELIST_RET_SLIDE;

	case COMMAND_SELECT:					// �I��
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
//			PMSND_PlaySE( SEQ_SE_DECIDE1 );
			ChangeCursorPosPalette( wk, wk->listPos, wk->listOldPos );
			wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );
		}
		return wk->listPos;

	case COMMAND_ERROR:						// �R�}���h����
	case COMMAND_NONE:						// ����Ȃ�
		break;
  
  default:
		GF_ASSERT(0);
	}

	return FRAMELIST_RET_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X�N���[�����x�Z�b�g
 *
 * @param		wk		���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetListScrollSpeed( FRAMELIST_WORK * wk )
{
	if( wk->keyRepeat < 20 ){
		wk->keyRepeat++;
	}
	if( wk->keyRepeat == 20 ){
		wk->keyRepPos = 0;
	}else if( wk->keyRepeat >= 16 ){
		wk->keyRepPos = 1;
	}else if( wk->keyRepeat >= 12 ){
		wk->keyRepPos = 2;
	}else if( wk->keyRepeat >= 8 ){
		wk->keyRepPos = 3;
	}else if( wk->keyRepeat >= 4 ){
		wk->keyRepPos = 4;
	}else{
		wk->keyRepPos = 5;
	}
}


//============================================================================================
//	�X�N���[���֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X�N���[���t���[�����쐬
 *
 * @param		wk			���[�N
 * @param		speed		�X�N���[�����x
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static u8 MakeScrollCount( FRAMELIST_WORK * wk, s8 speed )
{
	return ( wk->hed.itemSizY * 8 / GFL_STD_Abs(speed) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���݂̈ʒu����̎c��̍ő�X�N���[���l���擾
 *
 * @param		wk			���[�N
 * @param		speed		�X�N���[�����x
 *
 * @return	�c��̍ő�X�N���[���l
 */
//--------------------------------------------------------------------------------------------
static u32 GetListScrollCount( FRAMELIST_WORK * wk, s8 speed )
{
	if( speed < 0 ){
		return wk->listScroll;
	}
	return ( wk->listScrollMax - wk->listScroll );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X�N���[�������ݒ�
 *
 * @param		wk			���[�N
 * @param		speed		�X�N���[�����x
 * @param		max			�X�N���[����
 * @param		next		�X�N���[����̃V�[�P���X
 * @param		se			�r�d�Đ��t���O
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitListScroll( FRAMELIST_WORK * wk, s8 speed, u8 max, s16 next, BOOL se )
{
	wk->listBgScrollSpeed = speed;
	wk->listBgScrollMax   = max;
	wk->listBgScrollCount = 0;

	wk->nextSeq = next;
	wk->mainSeq = MAINSEQ_SCROLL;

	wk->scrollSE = se;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X�N���[�����C������
 *
 * @param		wk			���[�N
 *
 * @retval	"TRUE = �X�N���[����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
static BOOL MainListScroll( FRAMELIST_WORK * wk )
{
	if( PRINTSYS_QUE_IsFinished( wk->que ) == FALSE ){
		return TRUE;
	}

	if( wk->listBgScrollCount == 0 ){
		// �w��񐔃X�N���[���I��
		if( wk->listBgScrollMax == 0 ){
			ChangeCursorPosPalette( wk, wk->listPos, PALCHG_NONE );
			wk->mainSeq = wk->nextSeq;
			return FALSE;
		}

		ChangeCursorPosPalette( wk, PALCHG_NONE, wk->listPos );

		if( wk->nextSeq == MAINSEQ_SLIDE ){
			if( wk->autoScroll == FALSE ){
				if( GFL_UI_TP_GetCont() == FALSE ){
					u32	max = GetListScrollCount( wk, wk->listBgScrollSpeed );
					u32	abs = GFL_STD_Abs( wk->listBgScrollSpeed );
					wk->autoScroll = TRUE;
					for( wk->slideCnt=0; wk->slideCnt<FRAMELIST_SPEED_MAX; wk->slideCnt++ ){
						if( abs == wk->hed.scrollSpeed[wk->slideCnt] ){
							wk->listBgScrollMax = AutoScrollCount[wk->slideCnt];
							break;
						}
					}
					if( wk->listBgScrollMax > max ){
						wk->listBgScrollMax = max;
					}
				}else{
					if( wk->listBgScrollSpeed < 0 ){
						wk->listPos++;
					}else{
						wk->listPos--;
					}
				}
			}else{
//				if( GFL_UI_TP_GetCont() == TRUE ){
				int	ret = GFL_UI_TP_HitCont( wk->touch );
				if( ret != GFL_UI_TP_HIT_NONE ){
					if( wk->hed.touch[ret].prm == FRAMELIST_TOUCH_PARAM_SLIDE ){
						wk->listPos = ret;
						wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );
						wk->mainSeq = wk->nextSeq;
						return FALSE;
					}
				}
				if( wk->listBgScrollMax == 1 ){
					u32	max = GetListScrollCount( wk, wk->listBgScrollSpeed );
					u32	abs = GFL_STD_Abs( wk->listBgScrollSpeed );
					wk->slideCnt++;
					if( wk->slideCnt != FRAMELIST_SPEED_MAX ){
						if( wk->listBgScrollSpeed < 0 ){
							wk->listBgScrollSpeed = -wk->hed.scrollSpeed[wk->slideCnt];
						}else{
							wk->listBgScrollSpeed = wk->hed.scrollSpeed[wk->slideCnt];
						}
						wk->listBgScrollMax = AutoScrollCount[wk->slideCnt];
					}
					if( wk->listBgScrollMax > max ){
						wk->listBgScrollMax = max;
					}
				}
			}
		}

		wk->listBgScrollCount = MakeScrollCount( wk, wk->listBgScrollSpeed );
		wk->listBgScrollMax--;

		if( wk->listBgScrollSpeed < 0 ){
			wk->listScroll--;
			DrawListItemMain( wk, wk->listScroll, GetItemFramePosY(wk,-1) );
			wk->putFrameMain = GetNextFrameNum( wk->putFrameMain, wk->putFrameMaxMain, -1 );
			DrawListItemSub( wk, wk->listScroll-wk->putFrameMaxSub, GetItemFramePosY(wk,-1)+wk->subFramePy );
			wk->putFrameSub = GetNextFrameNum( wk->putFrameSub, wk->putFrameMaxSub, -1 );
		}else{
			wk->listScroll++;
			DrawListItemMain( wk, wk->listScroll+(wk->putFrameMaxMain-1), GetItemFramePosY(wk,1) );
			wk->putFrameMain = GetNextFrameNum( wk->putFrameMain, wk->putFrameMaxMain, 1 );
			DrawListItemSub( wk, wk->listScroll-1, GetItemFramePosY(wk,1)+wk->subFramePy );
			wk->putFrameSub = GetNextFrameNum( wk->putFrameSub, wk->putFrameMaxSub, 1 );
		}
		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );

		if( wk->scrollSE == TRUE ){
			if( wk->nextSeq == MAINSEQ_SLIDE || wk->nextSeq == MAINSEQ_RAIL ){
				PMSND_PlaySE( SEQ_SE_SYS_06 );
			}else{
				PMSND_PlaySE( SEQ_SE_SELECT1 );
			}
		}
	}

	wk->listBgScoll += wk->listBgScrollSpeed;
	GFL_BG_SetScrollReq( wk->hed.mainBG, GFL_BG_SCROLL_Y_SET, wk->listBgScoll );
	if( wk->hed.subBG != FRAMELIST_SUB_BG_NONE ){
		GFL_BG_SetScrollReq( wk->hed.subBG, GFL_BG_SCROLL_Y_SET, wk->listBgScoll );
	}
	wk->hed.cbFunc->scroll( wk->hed.cbWork, wk->listBgScrollSpeed );

	wk->listBgScrollCount--;

	return TRUE;
}


//============================================================================================
//	���[���֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		���[���ړ�������
 *
 * @param		wk			���[�N
 * @param		pos			�^�b�`�f�[�^�ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitRailMove( FRAMELIST_WORK * wk, int pos )
{
	wk->railTop    = wk->touch[pos].rect.top;
	wk->railBottom = wk->touch[pos].rect.bottom;

	wk->railHit[0] = wk->touch[pos];
	if( wk->railHit[0].rect.top >= RAIL_AREA_Y ){
		wk->railHit[0].rect.top = wk->railHit[0].rect.top - RAIL_AREA_Y;
	}else{
		wk->railHit[0].rect.top = 0;
	}
	if( ( wk->railHit[0].rect.bottom + RAIL_AREA_Y ) <= RAIL_AREA_Y_MAX  ){
		wk->railHit[0].rect.bottom = wk->railHit[0].rect.bottom + RAIL_AREA_Y;
	}else{
		wk->railHit[0].rect.bottom = RAIL_AREA_Y_MAX;
	}
	if( wk->railHit[0].rect.left >= RAIL_AREA_X ){
		wk->railHit[0].rect.left = wk->railHit[0].rect.left - RAIL_AREA_X;
	}else{
		wk->railHit[0].rect.left = 0;
	}
	if( ( wk->railHit[0].rect.right + RAIL_AREA_X ) <= RAIL_AREA_X_MAX  ){
		wk->railHit[0].rect.right = wk->railHit[0].rect.right + RAIL_AREA_X;
	}else{
		wk->railHit[0].rect.right = RAIL_AREA_X_MAX;
	}

	wk->railHit[1].rect.top = GFL_UI_TP_HIT_END;

	wk->mainSeq = MAINSEQ_RAIL;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�b�`�ʒu����X�N���[���l���擾
 *
 * @param		wk			���[�N
 *
 * @return	�X�N���[���l
 */
//--------------------------------------------------------------------------------------------
static u32 GetRailScroll( FRAMELIST_WORK * wk )
{
	u32	x, y;

	GFL_UI_TP_GetPointCont( &x, &y );

	return SCROLLBAR_GetCount( wk->listScrollMax, y, wk->railTop, wk->railBottom, wk->hed.barSize );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X�N���[���o�[�̕\���x���W���擾
 *
 * @param		wk			���[�N
 *
 * @return	�x���W
 */
//--------------------------------------------------------------------------------------------
u32 FRAMELIST_GetScrollBarPY( FRAMELIST_WORK * wk )
{
	u8	ty, by;
	u8	i;

	i = 0;
	while( 1 ){
		if( wk->hed.touch[i].tbl.rect.top == GFL_UI_TP_HIT_END ){
			return 0;
		}
		if( wk->hed.touch[i].prm == FRAMELIST_TOUCH_PARAM_RAIL ){
			ty = wk->hed.touch[i].tbl.rect.top;
			by = wk->hed.touch[i].tbl.rect.bottom;
			break;
		}
		i++;
	}

	return SCROLLBAR_GetPosY( wk->listScrollMax, wk->listScroll, ty, by, wk->hed.barSize );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���[���������C��
 *
 * @param		wk			���[�N
 *
 * @retval	"TRUE = ���쒆"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
static BOOL MainRailMove( FRAMELIST_WORK * wk )
{
	u32	scroll;
	u32	max;

	if( PRINTSYS_QUE_IsFinished( wk->que ) == FALSE ){
		return TRUE;
	}

	if( GFL_UI_TP_HitCont( wk->railHit ) == GFL_UI_TP_HIT_NONE ){
		wk->mainSeq = MAINSEQ_MAIN;
		return FALSE;
	}

	scroll = GetRailScroll( wk );
	max = GFL_STD_Abs( wk->listScroll-scroll );
	if( max > wk->putFrameMaxMain ){
		max = wk->putFrameMaxMain;
	}

	if( wk->listScroll > scroll ){
		wk->listScroll = scroll+max;
		InitListScroll( wk, -wk->hed.scrollSpeed[0], max, MAINSEQ_RAIL, TRUE );
	}else if( wk->listScroll < scroll ){
		wk->listScroll = scroll-max;
		InitListScroll( wk, wk->hed.scrollSpeed[0], max, MAINSEQ_RAIL, TRUE );
	}

	return TRUE;
}


//============================================================================================
//	�X���C�h����
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X���C�h����������
 *
 * @param		wk			���[�N
 * @param		pos			�^�b�`�������ڈʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitSlideMove( FRAMELIST_WORK * wk, int pos )
{
	wk->slidePos = pos;
	wk->mainSeq  = MAINSEQ_SLIDE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���[���������C��
 *
 * @param		wk			���[�N
 *
 * @retval	"TRUE = ���쒆"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
static BOOL MainSlideMove( FRAMELIST_WORK * wk )
{
	int	ret;
	u32	x, y;

	if( PRINTSYS_QUE_IsFinished( wk->que ) == FALSE ){
		return TRUE;
	}

	ret = GFL_UI_TP_HitCont( wk->touch );
	
	if( ret == GFL_UI_TP_HIT_NONE ){
		wk->autoScroll = FALSE;
		wk->mainSeq = MAINSEQ_MAIN;
		return FALSE;
	}
	if( wk->hed.touch[ret].prm != FRAMELIST_TOUCH_PARAM_SLIDE ){
		wk->autoScroll = FALSE;
		wk->mainSeq = MAINSEQ_MAIN;
		return FALSE;
	}

	if( wk->autoScroll == TRUE ){
		wk->autoScroll = FALSE;
		wk->slidePos = ret;
	}

	if( wk->nowTpy != 0xffffffff && wk->oldTpy != 0xffffffff ){
		u32	y;
		u32	cnt, cntMax;
		s8	speed;

		y = GFL_STD_Abs(  wk->nowTpy -  wk->oldTpy );

		if( wk->listPos == 0 && wk->nowTpy < wk->oldTpy ){
			speed = 0;
		}else if( wk->listPos == (wk->listPosMax-1) && wk->nowTpy > wk->oldTpy ){
			speed = 0;
		}else if( y >= 12 ){
			speed = wk->hed.scrollSpeed[0];
		}else if( y >= 8 ){
			speed = wk->hed.scrollSpeed[1];
		}else if( y >= 4 ){
			speed = wk->hed.scrollSpeed[2];
		}else{
			speed = 0;
		}

		if( wk->nowTpy > wk->oldTpy ){
			speed *= -1;
		}

		cntMax = GetListScrollCount( wk, speed );
		if( cntMax != 0 ){
			cnt = y / wk->hed.itemSizY;
			if( cnt == 0 ){
				cnt = 1;
			}else if( cntMax < cnt ){
				cnt = cntMax;
			}

			if( speed != 0 ){
				InitListScroll( wk, speed, cnt, MAINSEQ_SLIDE, TRUE );
				wk->slidePos = ret;
				return TRUE;
			}
		}
	}

	if( wk->slidePos != ret ){
		u32	cnt;
		u32	cntMax;
		s8	speed;
		if( ret > wk->slidePos ){
			speed = -wk->hed.scrollSpeed[3];
			cnt = ret - wk->slidePos;
		}else if( ret < wk->slidePos ){
			speed = wk->hed.scrollSpeed[3];
			cnt = wk->slidePos - ret;
		}
		cntMax = GetListScrollCount( wk, speed );
		if( cntMax != 0 ){
			if( cntMax < cnt ){
				cnt = cntMax;
			}
			InitListScroll( wk, speed, cnt, MAINSEQ_SLIDE, TRUE );
			wk->slidePos = ret;
			return TRUE;
		}
	}

	return TRUE;
}


//============================================================================================
//	�`��֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�u�q�`�l�擾�p�̂a�f�t���[���p�����[�^���擾
 *
 * @param		bgFrame		�a�f�t���[���ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static GFL_DISPUT_BGID GetDispBGID( u8 bgFrame )
{
	if( bgFrame == GFL_BG_FRAME0_M ){
		return GFL_DISPUT_BGID_M0;
	}else if( bgFrame == GFL_BG_FRAME1_M ){
		return GFL_DISPUT_BGID_M1;
	}else if( bgFrame == GFL_BG_FRAME2_M ){
		return GFL_DISPUT_BGID_M2;
	}else if( bgFrame == GFL_BG_FRAME3_M ){
		return GFL_DISPUT_BGID_M3;
	}else if( bgFrame == GFL_BG_FRAME0_S ){
		return GFL_DISPUT_BGID_S0;
	}else if( bgFrame == GFL_BG_FRAME1_S ){
		return GFL_DISPUT_BGID_S1;
	}else if( bgFrame == GFL_BG_FRAME2_S ){
		return GFL_DISPUT_BGID_S2;
	}else if( bgFrame == GFL_BG_FRAME3_S ){
		return GFL_DISPUT_BGID_S3;
	}
	return GFL_DISPUT_BGID_M0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���ڔw�i��������
 *
 * @param		itemNum		���ڔԍ�
 * @param		frmNum		�v�����g�t���[���ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void WriteItemFrame( FRAMELIST_WORK * wk, u32 itemNum, u32 frmNum )
{
	GFL_BMPWIN * win;
	u16 * scrn;
	u8 * bmp;
	u8 * cgx;
	u8	bmp_px, bmp_py, bmp_sx, bmp_sy;
	u8	frm_sx, frm_sy;
	u8	i, j;

	win  = wk->printUtil[frmNum].util.win;
	scrn = wk->wfrmGra[ wk->item[itemNum].type ].scrn;
	cgx  = GFL_DISPUT_GetCgxPtr( GetDispBGID(BGWINFRM_BGFrameGet(wk->wfrm,frmNum)) );

	// �t���[���ɔw�i���Z�b�g
	BGWINFRM_FrameSet( wk->wfrm, frmNum, scrn );

	// �t���[����BMPWIN���Z�b�g
	BGWINFRM_BmpWinOn( wk->wfrm, frmNum, win );

	// BMPWIN�ɔw�i��`��
	frm_sx = wk->hed.itemSizX;
	frm_sy = wk->hed.itemSizY;
	bmp_sx = GFL_BMPWIN_GetScreenSizeX( win );
	bmp_sy = GFL_BMPWIN_GetScreenSizeY( win );
	bmp_px = GFL_BMPWIN_GetPosX( win );
	bmp_py = GFL_BMPWIN_GetPosY( win );
	bmp = GFL_BMP_GetCharacterAdrs( GFL_BMPWIN_GetBmp(win) );
	for( i=0; i<bmp_sy; i++ ){
		for( j=0; j<bmp_sx; j++ ){
			GFL_STD_MemCopy32(
				&cgx[(scrn[(i+bmp_py)*frm_sx+j+bmp_px]&0x3ff)*0x20],
				&bmp[(i*bmp_sx+j)*0x20],
				0x20 );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���ڔz�u
 *
 * @param		idx			���ڃC���f�b�N�X
 * @param		py			�z�u�x���W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PutItemFrame( FRAMELIST_WORK * wk, u32 idx, s8 py )
{
	u16 * buf;
	u16	frm;
	u16	i;

	buf = BGWINFRM_FrameBufGet( wk->wfrm, idx );
	frm = BGWINFRM_BGFrameGet( wk->wfrm, idx );

	for( i=0; i<wk->hed.itemSizY; i++ ){
		if( py < 0 ){
			py += 32;
		}else if( py >= 32 ){
			py -= 32;
		}
		GFL_BG_WriteScreenExpand(
			frm,
			wk->hed.itemPosX, py,
			wk->hed.itemSizX, 1,
			buf,
			0, i,
			wk->hed.itemSizX, wk->hed.itemSizY );
		py++;
	}
	GFL_BG_LoadScreenV_Req( frm );
}

static void ClearItemFrame( FRAMELIST_WORK * wk, u8 frm, s8 py )
{
	u32	i;

	for( i=0; i<wk->hed.itemSizY; i++ ){
		if( py < 0 ){
			py += 32;
		}else if( py >= 32 ){
			py -= 32;
		}
		GFL_BG_FillScreen( frm, 0, wk->hed.itemPosX, py, wk->hed.itemSizX, 1, 0 );
		py++;
	}
	GFL_BG_LoadScreenV_Req( frm );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���ڔz�u�x���W���擾
 *
 * @param		wk		���[�N
 * @param		vec		�z�u����
 *
 * @return	�z�u�x���W
 */
//--------------------------------------------------------------------------------------------
static s8 GetItemFramePosY( FRAMELIST_WORK * wk, s8 vec )
{
	s8	py;
	s8	prm;
	
	py  = wk->hed.itemPosY;
	prm = wk->hed.itemSizY * vec;
	
	while( 1 ){
		py += prm;
		if( py < 0 || py >= BGSCRN_CHR_SIZE_Y ){
			break;
		}
	}
	py = py + ( wk->listBgScoll / 8 );
	return py;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���̍��ڃt���[���ԍ����擾
 *
 * @param		now		���݂̍��ڃt���[���ԍ�
 * @param		max		�t���[���ő吔
 * @param		vec		�z�u����
 *
 * @return	�t���[���ԍ�
 */
//--------------------------------------------------------------------------------------------
static s8 GetNextFrameNum( s8 now, s8 max, s8 vec )
{
	now += vec;
	if( now < 0 ){
		now = max;
	}else if( now > max ){
		now = 0;
	}

	return now;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���C����ʂ̍��ڕ`��
 *
 * @param		wk			���[�N
 * @param		pos			���ڔԍ�
 * @param		py			�`��x���W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void DrawListItemMain( FRAMELIST_WORK * wk, s32 pos, s8 py )
{
	if( pos >= 0 && pos < wk->listMax ){
		WriteItemFrame( wk, pos, wk->putFrameMain );
		PutItemFrame( wk, wk->putFrameMain, py );
		{
			s32	drawPy = py;
			wk->hed.cbFunc->draw(
				wk->hed.cbWork,
				pos,
				&wk->printUtil[wk->putFrameMain].util,
				drawPy * 8 - wk->listBgScoll,
				TRUE );
		}
	}else{
		ClearItemFrame( wk, wk->hed.mainBG, py );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�T�u��ʂ̍��ڕ`��
 *
 * @param		wk			���[�N
 * @param		pos			���ڔԍ�
 * @param		py			�`��x���W
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void DrawListItemSub( FRAMELIST_WORK * wk, s32 pos, s8 py )
{
	if( wk->hed.subBG != FRAMELIST_SUB_BG_NONE ){
//		py += wk->subFramePy;
		if( pos >= 0 ){
			u32	subFrame = wk->putFrameSub + wk->printSubPos;
			WriteItemFrame( wk, pos, subFrame );
			PutItemFrame( wk, subFrame, py );
			{
				s32	drawPy = py;
				wk->hed.cbFunc->draw(
					wk->hed.cbWork,
					pos,
					&wk->printUtil[subFrame].util,
					drawPy * 8 - wk->listBgScoll,
					FALSE );
			}
		}else{
			ClearItemFrame( wk, wk->hed.subBG, py );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �w��ʒu�̍��ڂ̃p���b�g��ύX	
 *
 * @param		wk		���[�N
 * @param		pos		�J�[�\���ʒu
 * @param		pal		�p���b�g�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_ChangePosPalette( FRAMELIST_WORK * wk, u16 pos, u16 pal )
{
	s16	py;
	u16	i;

	py = wk->hed.itemPosY + pos * wk->hed.itemSizY + wk->listBgScoll / 8;

	for( i=0; i<wk->hed.itemSizY; i++ ){
		if( py < 0 ){
			py += 32;
		}else if( py >= 32 ){
			py -= 32;
		}
		GFL_BG_ChangeScreenPalette(
			wk->hed.mainBG,
			wk->hed.itemPosX, py,
			wk->hed.itemSizX, 1,
			pal );
		py++;
	}
	GFL_BG_LoadScreenV_Req( wk->hed.mainBG );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ʒu�̃p���b�g�ύX
 *
 * @param		wk			���[�N
 * @param		now			���݂̃J�[�\���ʒu
 * @param		old			�O��̃J�[�\���ʒu
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ChangeCursorPosPalette( FRAMELIST_WORK * wk, u16 now, u16 old )
{
	if( old < wk->listPosMax ){
		u16 * scrn = wk->wfrmGra[wk->item[wk->listScroll+old].type].scrn;
		FRAMELIST_ChangePosPalette( wk, old, scrn[0] >> 12 );
	}
	if( now < wk->listPosMax && GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY ){
		BLINKPALANM_InitAnimeCount( wk->blink );
		FRAMELIST_ChangePosPalette( wk, now, wk->hed.selPal );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�l�o�]��
 *
 * @param		wk			���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PrintTransMain( FRAMELIST_WORK * wk )
{
	u32	i;

	PRINTSYS_QUE_Main( wk->que );
	for( i=0; i<wk->printMax; i++ ){
		PRINT_UTIL_Trans( &wk->printUtil[i].util, wk->que );
	}
}


//============================================================================================
//	���[�N����֘A
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�v�����g�L���[�擾
 *
 * @param		wk				���[�N
 *
 * @return	�v�����g�L���[
 */
//--------------------------------------------------------------------------------------------
PRINT_QUE * FRAMELIST_GetPrintQue( FRAMELIST_WORK * wk )
{
	return wk->que;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���ڃp�����[�^�擾
 *
 * @param		wk				���[�N
 * @param		itemIdx		���ڃC���f�b�N�X
 *
 * @return	���ڃp�����[�^
 */
//--------------------------------------------------------------------------------------------
u32 FRAMELIST_GetItemParam( FRAMELIST_WORK * wk, u32 itemIdx )
{
	return wk->item[itemIdx].param;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g�ʒu�擾
 *
 * @param		wk				���[�N
 *
 * @return	���X�g�ʒu
 */
//--------------------------------------------------------------------------------------------
u32 FRAMELIST_GetListPos( FRAMELIST_WORK * wk )
{
	return ( wk->listPos + wk->listScroll );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ʒu�擾
 *
 * @param		wk				���[�N
 *
 * @return	���ڃp�����[�^
 */
//--------------------------------------------------------------------------------------------
u32 FRAMELIST_GetCursorPos( FRAMELIST_WORK * wk )
{
	return wk->listPos;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g�X�N���[���J�E���^���擾
 *
 * @param		wk		���[�N
 *
 * @return	�X�N���[���J�E���^
 */
//--------------------------------------------------------------------------------------------
u32 FRAMELIST_GetScrollCount( FRAMELIST_WORK * wk )
{
	return wk->listScroll;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g�����ɃX�N���[���ł��邩
 *
 * @param		wk		���[�N
 *
 * @retval	"TRUE = ��"
 * @retval	"FALSE = �s��"
 */
//--------------------------------------------------------------------------------------------
BOOL FRAMELIST_CheckScrollMax( FRAMELIST_WORK * wk )
{
	if( wk->listScroll < wk->listScrollMax ){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ʒu��ݒ�
 *
 * @param		wk					���[�N
 * @param		pos					�J�[�\���ʒu
 *
 * @return	none
 *
 *	�R�[���o�b�N[move]���Ă΂�܂�
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_SetCursorPos( FRAMELIST_WORK * wk, u32 pos )
{
	wk->listOldPos = wk->listPos;
	wk->listPos = pos;
	ChangeCursorPosPalette( wk, wk->listPos, wk->listOldPos );
	wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ʒu�̃p���b�g�����点��
 *
 * @param		wk					���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_SetCursorPosPalette( FRAMELIST_WORK * wk )
{
	ChangeCursorPosPalette( wk, wk->listPos, PALCHG_NONE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�J�[�\���ʒu�̃p���b�g��߂�
 *
 * @param		wk					���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_SetCursorPosPaletteOff( FRAMELIST_WORK * wk )
{
	ChangeCursorPosPalette( wk, PALCHG_NONE, wk->listPos );
}
