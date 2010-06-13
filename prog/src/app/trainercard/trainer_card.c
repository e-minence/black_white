//============================================================================================
/**
 * @file    trainer_card.c
 * @brief   �g���[�i�[�J�[�h
 * @author  Saito Nozomu
 * @date    05.11.15
 */
//============================================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "gamesystem/msgspeed.h"
#include "savedata/trainercard_data.h"
#include "sound/pm_sndsys.h"
#include "app/app_menu_common.h"
#include "arc_def.h"
#include "font/font.naix"

#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "message.naix"
#include "trainer_case.naix"
#include "msg/msg_trainercard.h"

#include "trcard_sys.h"

#include "trcard_snd_def.h"
#include "trcard_bmp.h"
#include "trcard_obj.h"
#include "trcard_cgx_def.h"
#include "trainer_case.naix"
#include "trc_union.naix"

#include "savedata/config.h"
#include "msg/msg_trainercard.h"
#include "message.naix"
#include "system/wipe.h"
#include "system/pms_draw.h"
#include "test/ariizumi/ari_debug.h"


#define MIN_SCRUCH  (3)
#define MAX_SCRUCH  (40)
#define REV_SPEED (FX32_SHIFT - wk->RevSpeed)

#define CGX_WIDE    (32)
#define BUTTON_CGX_POS  (4*CGX_WIDE)
#define MAIN_BUTTON_CGX_SIZE  (4)
#define MAIN_BUTTON_POS_Y (19)
#define MAIN_BUTTON_POS_X (14)

#define NOBODY_CGX_POS  (5*CGX_WIDE+26)
#define GYM_READER_CGX_SIZE (5)
#define COVER_CGX_WIDTH (16)

#define BADGE_BAD_PLTT_NO (3)
#define UNI_TRAINER_PLTT_NO (4)

#define TRC_BG_TRAINER  (GFL_BG_FRAME3_S)
#define TRC_BG_FONT     (GFL_BG_FRAME3_S)
#define TRC_BG_SIGN     (GFL_BG_FRAME3_S)
#define TRC_BG_MSG      (GFL_BG_FRAME0_S)
#define TRC_BG_BACK     (GFL_BG_FRAME1_S)
#define TRC_BG_CARD     (GFL_BG_FRAME2_S)
#define TRC_BG_BADGE_CASE (GFL_BG_FRAME2_M)
#define TRC_BG_BADGE_BACK (GFL_BG_FRAME3_M)

#define UNION_TR_MAX  (16)

enum {
  SEQ_IN,
  SEQ_MAIN,
  SEQ_RETURN_FADEOUT,
  SEQ_END_FADEOUT,
  SEQ_OUT,
  SEQ_REVERSE,
  SEQ_PMSINPUT,
  SEQ_BADGE_VIEW_CALL,
  SEQ_COVER,
  SEQ_SCALING,
};

typedef enum {
  TRC_KEY_REQ_NONE = 0,
  TRC_KEY_REQ_TP_BEAT,
  TRC_KEY_REQ_TP_BRUSH,
  TRC_KEY_REQ_REV_BUTTON,
  TRC_KEY_REQ_RETURN_BUTTON,
  TRC_KEY_REQ_END_BUTTON,
  TRC_KEY_REQ_TRAINER_TYPE,
  TRC_KEY_REQ_PERSONALITY,
  TRC_KEY_REQ_SCALE_BUTTON,
  TRC_KEY_REQ_PMS_CALL,
  TRC_KEY_REQ_BADGE_CALL,
}TRC_KEY_REQ;

typedef enum {
  COVER_CLOSE = 0,
  COVER_OPEN = 1,
}COVER_STATE;

enum{
  SCREEN_SIGN_LEFT=0,
  SCREEN_SIGN_RIGHT,
  SCREEN_SIGN_ALL,
};


// �߂�{�^�����W
#define TP_RETURN_X   (  228 )
#define TP_RETURN_Y   ( 21*8 )
#define TP_RETURN_W   (  3*8 )
#define TP_RETURN_H   (  3*8 )

// �I���{�^�����W
#define TP_END_X  (  204 )
#define TP_END_Y  ( 21*8 )
#define TP_END_W  (  3*8 )
#define TP_END_H  (  3*8 )

// �J�[�h�Ђ�����Ԃ��{�^��
#define TP_CARD_X  (  100 )
#define TP_CARD_Y  ( 21*8 )
#define TP_CARD_W  (  8*8 )
#define TP_CARD_H  (  3*8 )

// �o�b�W��ʃ{�^��(�J�[�h�߂�{�^���ł�����A�A�j���J�n�E��~�{�^���ł�����)
#define TP_BADGE_X  (    4 )
#define TP_BADGE_Y  ( 21*8 )
#define TP_BADGE_W  (  3*8 )
#define TP_BADGE_H  (  3*8 )

// �����������݃{�^��
#define TP_LOUPE_X  (   36 )
#define TP_LOUPE_Y  ( 21*8 )
#define TP_LOUPE_W  (  3*8 )
#define TP_LOUPE_H  (  3*8 )

// �y����{�^��
#define TP_PEN_X  (   68 )
#define TP_PEN_Y  ( 21*8 )
#define TP_PEN_W  (  3*8 )
#define TP_PEN_H  (  3*8 )

// �u�b�N�}�[�N�{�^��
#define TP_BOOKMARK_X  (  180 )
#define TP_BOOKMARK_Y  ( 21*8 )
#define TP_BOOKMARK_W  (  3*8 )
#define TP_BOOKMARK_H  (  3*8 )

// �g���[�i�[�^�C�v
#define TP_TRTYPE_X  (   24 )
#define TP_TRTYPE_Y  (   48 )
#define TP_TRTYPE_W  (  136 )
#define TP_TRTYPE_H  (  2*8 )

// ��������
#define TP_PERSONAL_X  (   24 )
#define TP_PERSONAL_Y  (   64 )
#define TP_PERSONAL_W  (  136 )
#define TP_PERSONAL_H  (  2*8 )

// �ȈՉ�b
#define TP_PMS_X  (   16 )
#define TP_PMS_Y  (  120 )
#define TP_PMS_W  (  223 )
#define TP_PMS_H  (   31 )

// �E�ֈړ��i�g�厞�j
#define TP_MOVERIGHT_X  ( 255-24 )
#define TP_MOVERIGHT_Y  (      0 )
#define TP_MOVERIGHT_W  (     24 )
#define TP_MOVERIGHT_H  ( 192-24 )

// ���ֈړ��i�g�厞�j
#define TP_MOVELEFT_X  (      0 )
#define TP_MOVELEFT_Y  (      0 )
#define TP_MOVELEFT_W  (     24 )
#define TP_MOVELEFT_H  ( 192-24 )


// ���ֈړ��i�g�厞�j

//============================================
// ���\�L���^�b�`�͈͒�`
//============================================

// �X�R�A�X�N���[���̈�
#define TP_SCORE_SCROL_X  (   16 )
#define TP_SCORE_SCROL_Y  (   16 )
#define TP_SCORE_SCROL_W  (  224 )
#define TP_SCORE_SCROL_H  (   64 )

// �������̈�i�ȈՔŁj
#define TP_PAINTS_X  (   24 )
#define TP_PAINTS_Y  (   88 )
#define TP_PAINTS_W  (  208 )
#define TP_PAINTS_H  (   64 )

// �������̈�i�����Łj
#define TP_PAINTL_X  (   24 )
#define TP_PAINTL_Y  (   24 )
#define TP_PAINTL_W  (  208 )
#define TP_PAINTL_H  (  128 )

// �J�[�h�A�t�B�����W�����l
#define TRCARD_CENTER_POSX  ( 128 )
#define TRCARD_CENTER_POSY  (  96 )
#define TRCARD_OFFSET_POSX  (   0 )
#define TRCARD_OFFSET_POSY  (   0 )
#define TRCARD_LEFT_SCALE_CENTER_POSX   (  32 ) // �����g�厞
#define TRCARD_RIGHT_SCALE_CENTER_POSX  ( 224 ) // �E���g�厞
#define TRCARD_SCALE_CENTER_POSY        ( 152 ) // �g�厞Y���W����

#define SCALE_SIDE_LEFT_MAX_X (96)  // �g��A�������ŏ�������
#define SCALE_SIDE_RIGHT_MIN_X (95) // �g��A�E�����ŏ�������

//���j�I�����[���g���[�i�[�\���e�[�u��
static const int UniTrTable[UNION_TR_MAX][2] =
{
  //�j
  { NARC_trc_union_01_NCGR, NARC_trc_union_01_NCLR },
  { NARC_trc_union_02_NCGR, NARC_trc_union_02_NCLR },
  { NARC_trc_union_03_NCGR, NARC_trc_union_03_NCLR },
  { NARC_trc_union_04_NCGR, NARC_trc_union_04_NCLR },
  { NARC_trc_union_05_NCGR, NARC_trc_union_05_NCLR },
  { NARC_trc_union_06_NCGR, NARC_trc_union_06_NCLR },
  { NARC_trc_union_07_NCGR, NARC_trc_union_07_NCLR },
  { NARC_trc_union_08_NCGR, NARC_trc_union_08_NCLR },
  //��              
  { NARC_trc_union_09_NCGR, NARC_trc_union_09_NCLR },
  { NARC_trc_union_10_NCGR, NARC_trc_union_10_NCLR },
  { NARC_trc_union_11_NCGR, NARC_trc_union_11_NCLR },
  { NARC_trc_union_12_NCGR, NARC_trc_union_12_NCLR },
  { NARC_trc_union_13_NCGR, NARC_trc_union_13_NCLR },
  { NARC_trc_union_14_NCGR, NARC_trc_union_14_NCLR },
  { NARC_trc_union_15_NCGR, NARC_trc_union_15_NCLR },
  { NARC_trc_union_16_NCGR, NARC_trc_union_16_NCLR },
};

static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_128_A,       // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g

  GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g

  GX_VRAM_OBJ_128_B,        // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g

  GX_VRAM_SUB_OBJ_128_D,     // �T�u2D�G���W����OBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // �T�u2D�G���W����OBJ�g���p���b�g

  GX_VRAM_TEX_NONE,       // �e�N�X�`���C���[�W�X���b�g
  GX_VRAM_TEXPLTT_NONE,     // �e�N�X�`���p���b�g�X���b�g

  GX_OBJVRAMMODE_CHAR_1D_128K,  // ���C��OBJ�}�b�s���O���[�h
  GX_OBJVRAMMODE_CHAR_1D_32K,   // �T�uOBJ�}�b�s���O���[�h
};

//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================
static void AllocStrBuf( TR_CARD_WORK * wk );
static void FreeStrBuf( TR_CARD_WORK * wk );

static void SetTrCardBg( void );
static void SetTrCardBgGraphic( TR_CARD_WORK * wk );

static void TrCardBgExit( void );

static void CardDesignDraw(TR_CARD_WORK* wk);
static void CardDataDraw(TR_CARD_WORK* wk);
static BOOL CardRev(TR_CARD_WORK * wk );
static void CardRevAffineSet(TR_CARD_WORK* wk);
static int  CheckInput(TR_CARD_WORK *wk);
static void JumpInputResult( TR_CARD_WORK *wk, int req, int *seq );
static void SetCardPalette(TR_CARD_WORK *wk ,u8 inCardRank, const u8 inPokeBookHold);
static void SetUniTrainerPalette(TR_CARD_WORK *wk ,const u8 inTrainerNo);

static void VBlankFunc( GFL_TCB *tcb, void *work );
static void DispTrainer(TR_CARD_WORK *wk);
static void ClearTrainer(TR_CARD_WORK *wk);

static void ResetAffinePlane(void);

static void DecodeSignData(const u8 *inRawData, u8 *outData);
static void EncodeSignData( const u8 *inBmpData, u8 *outData );

static void TransSignData(const int inFrame, const u8 *inSignData, int flag);
static void DispTouchBarObj( TR_CARD_WORK *wk, int is_back );
static void UpdateSignAnime( TR_CARD_WORK *wk );
static void Trans_SignScreen( const int inFrame, const int flag );
static BOOL CardScaling( TR_CARD_WORK *wk );
static void Change_SignAnimeButton( TR_CARD_WORK *wk, int flag, int OnOff );
static void Start_SignAnimeButton( TR_CARD_WORK *wk, int flag );
static void UpdateTextBlink( TR_CARD_WORK *wk );


static void tp_sign_end_check( TR_CARD_WORK *wk );
static void Stock_TouchPoint( TR_CARD_WORK *wk, int scale_mode );
static void _BmpWinPrint_Rap(     GFL_BMPWIN * win, void * src,
      int src_x, int src_y, int src_dx, int src_dy,
      int win_x, int win_y, int win_dx, int win_dy );
static int DrawPoint_to_Line(  GFL_BMPWIN *win, 
  const u8 *brush, 
  int px, int py, int *sx, int *sy, 
  int count, int flag, int scale_mode, int scale_side );
static void Stock_OldTouch( TOUCH_INFO *all, TOUCH_INFO *stock );
static  int DrawBrushLine( GFL_BMPWIN *win, TOUCH_INFO *all, TOUCH_INFO *old, int draw, u8 *SignData, u8 sign_mode, u8 scale_side );
static void SetBookMark( TR_CARD_WORK *wk);


//============================================================================================
//  �O���[�o���ϐ�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F������
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT TrCardProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  TR_CARD_WORK * wk;

  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );
  WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
  WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
  WIPE_ResetWndMask(WIPE_DISP_MAIN);
  WIPE_ResetWndMask(WIPE_DISP_SUB);

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TR_CARD, 0x50000 );

  wk = GFL_PROC_AllocWork( proc, sizeof(TR_CARD_WORK), HEAPID_TR_CARD );
  GFL_STD_MemFill( wk, 0, sizeof(TR_CARD_WORK) );

  wk->heapId = HEAPID_TR_CARD;
  wk->ObjWork.heapId = HEAPID_TR_CARD;
  //���p���p�����[�^�擾
  wk->tcp = pwk;
  wk->TrCardData = wk->tcp->TrCardData;
  wk->key_mode = GFL_UI_CheckTouchOrKey();

  wk->msg_spd = MSGSPEED_GetWait();
  wk->win_type = 0;

  if(wk->TrCardData->Clear_m != 0){ //����0���łȂ���΁A�N���A�����Ƃ݂Ȃ�
    wk->isClear = TRUE;
  }else{
    wk->isClear = FALSE;
  }
  {
    int i;
    u16 flag  = 1;
    for(i=0;i<TR_BADGE_NUM_MAX;i++){
      if(wk->TrCardData->BadgeFlag & flag){
        wk->badge[i] = 1;
      }else{
        wk->badge[i] = 0;
      }
      flag <<= 1;
    }
  }

  //�ʐM�����ǂ����H
  if(GFL_NET_GetConnectNum() > 0){
    wk->isComm = TRUE;
  }

  GFL_BG_Init( wk->heapId );

  AllocStrBuf(wk);

  GFL_DISP_SetBank( &vramBank );
  SetTrCardBg();
  SetTrCardBgGraphic( wk );

  InitTRCardCellActor( &wk->ObjWork , &vramBank );

  // ���ʕ\�����ڐݒ�
  TRCBmp_MakeScoreList( wk, wk->tcp->TrCardData );

  // ����ʃZ���A�N�^�[�o�^
  SetTrCardActorSub( &wk->ObjWork);

  //Bmp�E�B���h�E������
  TRCBmp_AddTrCardBmp( wk );

  // �ȈՉ�b�V�X�e��������
  wk->PmsDrawWork = PMS_DRAW_Init( wk->ObjWork.cellUnit, CLSYS_DRAW_SUB, wk->printQue, 
                                   wk->fontHandle, 8, 1, HEAPID_TR_CARD );

  //�R�����`��
  TRCBmp_WriteSec(wk,wk->win[TRC_BMPWIN_PLAY_TIME], TRUE, wk->SecBuf);

  // �\�E���ݒ�i�V���[�g�J�b�g�ŗ��ʎw��̍ۂ̂ݗ��Ɂj
  if(wk->tcp->mode==TRCARD_SHORTCUT_BACK){
    wk->tcp->mode = TRCARD_SHORTCUT_NONE; // ��񔽉f��������N���A
    wk->is_back = TRUE;
  }else{
    wk->is_back = FALSE;
  }

  // �p�����[�^������
  wk->tcp->value = FALSE;
  wk->IsOpen = COVER_CLOSE;     //�P�[�X�͕�����Ԃ���X�^�[�g

  wk->ButtonPushFlg = FALSE;    //�{�^��������ĂȂ�
  if(wk->TrCardData->SignAnimeOn){  // �T�C���A�j�����Ă��邩�H
    wk->SignAnimePat = SCREEN_SIGN_LEFT;    
  }else{
    wk->SignAnimePat = SCREEN_SIGN_ALL;    
  }
  wk->CardCenterX = TRCARD_CENTER_POSX;
  wk->CardCenterY = TRCARD_CENTER_POSY;
  wk->CardOffsetX = TRCARD_OFFSET_POSX;
  wk->CardOffsetY = TRCARD_OFFSET_POSY;

  
  //�g�k�ʂ����Z�b�g
  ResetAffinePlane();

  //�J�[�h�̏�����ԕ\��
  CardDataDraw(wk);

  wk->vblankFunc = GFUser_VIntr_CreateTCB(VBlankFunc, wk , 1);  // VBlank�Z�b�g
  wk->vblankTcblSys = GFL_TCBL_Init( wk->heapId , wk->heapId , 3 , 0 );
  //�ʐM�A�C�R���Z�b�g
  GFL_NET_ReloadIconTopOrBottom( TRUE, wk->heapId );

  WIPE_SYS_Start( WIPE_PATTERN_FMAS, WIPE_TYPE_SHUTTERIN_DOWN,
          WIPE_TYPE_SHUTTERIN_DOWN, WIPE_FADE_BLACK,
          WIPE_DEF_DIV, WIPE_DEF_SYNC, wk->heapId );

  return GFL_PROC_RES_FINISH;
}

#if 0
//----------------------------------------------------------------------------------
/**
 * @brief �J�[�h�̊g��k���|�C���g�̂������t���邽�߂̃f�o�b�O���[�`��
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void debug_scale( TR_CARD_WORK *wk )
{
  if(GFL_UI_KEY_GetCont()&PAD_BUTTON_L){
    wk->CardScaleX -= 0x100;
    wk->CardScaleY -= 0x100;
    OS_Printf("scaleX=%d, scaleY=%d, cx=%d, cy=%d, ox=%d, oy=%d\n", 
               wk->CardScaleX, wk->CardScaleY,wk->CardCenterX, wk->CardCenterY,wk->CardOffsetX, wk->CardOffsetY);
    //�A�t�B���ϊ����s���N�G�X�g
    wk->aff_req = TRUE;
  }else if(GFL_UI_KEY_GetCont()&PAD_BUTTON_R){
    wk->CardScaleX += 0x100;
    wk->CardScaleY += 0x100;
    OS_Printf("scaleX=%d, scaleY=%d, cx=%d, cy=%d, ox=%d, oy=%d\n", 
               wk->CardScaleX, wk->CardScaleY,wk->CardCenterX, wk->CardCenterY,wk->CardOffsetX, wk->CardOffsetY);
    //�A�t�B���ϊ����s���N�G�X�g
    wk->aff_req = TRUE;
  }else if(GFL_UI_KEY_GetCont()&PAD_KEY_UP){
    if(GFL_UI_KEY_GetCont()&PAD_BUTTON_X){
      wk->CardOffsetY -= 1;
    }else{
      wk->CardCenterY -= 1;
    }
    OS_Printf("scaleX=%d, scaleY=%d, cx=%d, cy=%d, ox=%d, oy=%d\n", 
               wk->CardScaleX, wk->CardScaleY,wk->CardCenterX, wk->CardCenterY,wk->CardOffsetX, wk->CardOffsetY);
    //�A�t�B���ϊ����s���N�G�X�g
    wk->aff_req = TRUE;
  }else if(GFL_UI_KEY_GetCont()&PAD_KEY_DOWN){
    if(GFL_UI_KEY_GetCont()&PAD_BUTTON_X){
      wk->CardOffsetY += 1;
    }else{
      wk->CardCenterY += 1;
    }
    OS_Printf("scaleX=%d, scaleY=%d, cx=%d, cy=%d, ox=%d, oy=%d\n", 
               wk->CardScaleX, wk->CardScaleY,wk->CardCenterX, wk->CardCenterY,wk->CardOffsetX, wk->CardOffsetY);
    //�A�t�B���ϊ����s���N�G�X�g
    wk->aff_req = TRUE;
  }else if(GFL_UI_KEY_GetCont()&PAD_KEY_LEFT){
    if(GFL_UI_KEY_GetCont()&PAD_BUTTON_X){
      wk->CardOffsetX -= 1;
    }else{
      wk->CardCenterX -= 1;
    }
    OS_Printf("scaleX=%d, scaleY=%d, cx=%d, cy=%d, ox=%d, oy=%d\n", 
               wk->CardScaleX, wk->CardScaleY,wk->CardCenterX, wk->CardCenterY,wk->CardOffsetX, wk->CardOffsetY);
    //�A�t�B���ϊ����s���N�G�X�g
    wk->aff_req = TRUE;
  }else if(GFL_UI_KEY_GetCont()&PAD_KEY_RIGHT){
    if(GFL_UI_KEY_GetCont()&PAD_BUTTON_X){
      wk->CardOffsetX += 1;
    }else{
      wk->CardCenterX += 1;
    }
    OS_Printf("scaleX=%d, scaleY=%d, cx=%d, cy=%d, ox=%d, oy=%d\n", 
               wk->CardScaleX, wk->CardScaleY,wk->CardCenterX, wk->CardCenterY,wk->CardOffsetX, wk->CardOffsetY);
    //�A�t�B���ϊ����s���N�G�X�g
    wk->aff_req = TRUE;
  }
}
#endif

#define TRAINER_TYPE_GENDER_MAX   ( 8 )

//----------------------------------------------------------------------------------
/**
 * @brief �j��(0-7) ����(8-15)�Ԃ̃g���[�i�[�^�C�v�𑫂�����
 *
 * @param   trCard    
 */
//----------------------------------------------------------------------------------
static void _add_UnionTrNo( TR_CARD_DATA *trCard )
{
  if(++trCard->UnionTrNo > (TRAINER_TYPE_GENDER_MAX*(trCard->TrSex+1))-1){
    trCard->UnionTrNo = TRAINER_TYPE_GENDER_MAX*trCard->TrSex;
  }

}


#define SIGN_SE_LINES_LIMIT   ( 5 )

static void _sign_se( TR_CARD_WORK *wk, int lines )
{
  //OS_Printf("lines=%d, sewait=%d\n", lines, wk->sign_se_wait);
  if(wk->sign_se_wait!=0){
    wk->sign_se_wait--;
  }else{
    if(lines>SIGN_SE_LINES_LIMIT){
      wk->sign_se_wait=5;
      if(wk->pen==0){
        PMSND_PlaySE(SND_TRCARD_SIGN);
      }else{
        PMSND_PlaySE(SND_TRCARD_KESHI);
      }
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F���C��
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT TrCardProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  TR_CARD_WORK * wk  = mywk;
  
  switch( *seq ){
  case SEQ_IN:
    if( WIPE_SYS_EndCheck() ){
      *seq = SEQ_MAIN;
    }
    break;

  case SEQ_MAIN:
    {
      int req;
      req = CheckInput(wk);           // ���̓`�F�b�N
      JumpInputResult(wk, req, seq);  // ���͂ŕ���
      
      UpdateSignAnime(wk);
//      UpdateTextBlink(wk);

      if(wk->tcp->TrCardData->EditPossible){    // �ҏW�\�Ȃ�
        int line = DrawBrushLine( (GFL_BMPWIN*)wk->TrSignData, &wk->AllTouchResult, 
                                   &wk->OldTouch, 1, wk->TrSignData, wk->ScaleMode, wk->ScaleSide );
        _sign_se(wk, line);
      }
    }
    break;

  case SEQ_RETURN_FADEOUT:
    if(GFL_CLACT_WK_CheckAnmActive(wk->ObjWork.ClActWorkS[ACTS_BTN_BACK])==FALSE){
      WIPE_SYS_Start( WIPE_PATTERN_FSAM, WIPE_TYPE_SHUTTEROUT_UP,
              WIPE_TYPE_SHUTTEROUT_UP, WIPE_FADE_BLACK,
              WIPE_DEF_DIV, WIPE_DEF_SYNC, wk->heapId );
      *seq = SEQ_OUT;
    }
    break;
  case SEQ_END_FADEOUT:
    if(GFL_CLACT_WK_CheckAnmActive(wk->ObjWork.ClActWorkS[ACTS_BTN_END])==FALSE){
      WIPE_SYS_Start( WIPE_PATTERN_FSAM, WIPE_TYPE_SHUTTEROUT_UP,
              WIPE_TYPE_SHUTTEROUT_UP, WIPE_FADE_BLACK,
              WIPE_DEF_DIV, WIPE_DEF_SYNC, wk->heapId );
      *seq = SEQ_OUT;
    }
    break;
  case SEQ_OUT:
    if( WIPE_SYS_EndCheck() ){
      return GFL_PROC_RES_FINISH;
    }
    break;

  case SEQ_REVERSE: //���o�[�X����
    if ( CardRev(wk) ){
      *seq = SEQ_MAIN;
    }
    break;
  case SEQ_SCALING:
    if( CardScaling(wk)){
      *seq = SEQ_MAIN;
    }
    break;
  case SEQ_PMSINPUT:  //�ȈՉ�b�������Ă�
    WIPE_SYS_Start( WIPE_PATTERN_FSAM, WIPE_TYPE_SHUTTEROUT_UP,
            WIPE_TYPE_SHUTTEROUT_UP, WIPE_FADE_BLACK,
            WIPE_DEF_DIV, WIPE_DEF_SYNC, wk->heapId );
    wk->tcp->value = CALL_PMSINPUT;
    *seq = SEQ_OUT;
    break;
  case SEQ_BADGE_VIEW_CALL: // �o�b�W��ʑJ��
    if(GFL_CLACT_WK_CheckAnmActive(wk->ObjWork.ClActWorkS[ACTS_BTN_CHANGE])==FALSE){
      WIPE_SYS_Start( WIPE_PATTERN_FSAM, WIPE_TYPE_SHUTTEROUT_UP,
              WIPE_TYPE_SHUTTEROUT_UP, WIPE_FADE_BLACK,
              WIPE_DEF_DIV, WIPE_DEF_SYNC, wk->heapId );
      wk->tcp->value = CALL_BADGE;
      *seq = SEQ_OUT;
    }
    break;

  }
  
  if(++wk->scrl_ct >= 256){
    wk->scrl_ct = 0;
  }
  GFL_CLACT_SYS_Main();
  PMS_DRAW_Main( wk->PmsDrawWork );
  PRINTSYS_QUE_Main( wk->printQue );
  if( wk->vblankTcblSys != NULL ){
    GFL_TCBL_Main( wk->vblankTcblSys );
  }
  
  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �v���Z�X�֐��F�I��
 *
 * @param proc  �v���Z�X�f�[�^
 * @param seq   �V�[�P���X
 *
 * @return  ������
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT TrCardProc_End( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
  TR_CARD_WORK * wk  = mywk;
  TRCARD_CALL_PARAM *param = pwk;

  // �����̃T�C�������Ă��鎞�̓T�C���f�[�^�𔽉f������
  if(wk->TrCardData->SignDisenable==0){
    //�T�C�����k
    EncodeSignData( wk->TrSignData, wk->TrCardData->SignRawData);
  
    {
      u8 *SignBuf;
      // �T�C�������o���o�b�t�@�|�C���^�擾
      SignBuf = (u8*)TRCSave_GetSignDataPtr(GAMEDATA_GetTrainerCardPtr(wk->tcp->gameData));
      MI_CpuCopy8( wk->TrCardData->SignRawData, SignBuf, SIGN_SIZE_X*SIGN_SIZE_Y*8);
    }
  }
  // �ҏW�\�������ꍇ�̓��j�I�������ڂƐ��i��ύX����
  if(param->edit_possible){
    MYSTATUS *my = GAMEDATA_GetMyStatus(wk->tcp->gameData);
    TR_CARD_SV_PTR tr = GAMEDATA_GetTrainerCardPtr(wk->tcp->gameData);
    OS_Printf("�����ڂ�%d�ɁA���i��%d�ɕύX\n", wk->TrCardData->UnionTrNo,wk->TrCardData->Personality);
    MyStatus_SetTrainerView( my, wk->TrCardData->UnionTrNo );
    TRCSave_SetPersonarity(  tr, wk->TrCardData->Personality );
    TRCSave_SetSignAnime(    tr, wk->TrCardData->SignAnimeOn );
  }


  PMS_DRAW_Exit( wk->PmsDrawWork );

  //�g�p�����g�k�ʂ����ɖ߂�
  ResetAffinePlane();

  FreeStrBuf( wk );         //��������

  RereaseCellObject(&wk->ObjWork);    //2D�I�u�W�F�N�g�֘A�̈���

  GFL_HEAP_FreeMemory( wk->TrArcData ); //�g���[�i�[�L�����A�[�J�C�u�f�[�^���

  GFL_HEAP_FreeMemory( wk->TrScrnArcData );//�g���[�i�[�X�N���[�����

  // �����_�Ńp���b�g���
  GFL_HEAP_FreeMemory( wk->pPalBuf );

  TRCBmp_ExitTrCardBmpWin( wk );      // BMP�E�B���h�E�J��
  TrCardBgExit();     // BGL�폜
//  StopTP();             // �^�b�`�p�l���I��
  GFL_TCB_DeleteTask( wk->vblankFunc );   // VBlank�Z�b�g
  GFL_TCBL_Exit( wk->vblankTcblSys );
  //���샂�[�h�����߂�
  GFL_UI_SetTouchOrKey(wk->key_mode);

  GFL_PROC_FreeWork( proc );        // ���[�N�J��

  GFL_HEAP_DeleteHeap( HEAPID_TR_CARD );

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * ������o�b�t�@�̊m��
 *
 * @param wk    ��ʂ̃��[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void AllocStrBuf( TR_CARD_WORK * wk )
{
  int i;

  //�t�H���g�ǂݍ���
  wk->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , wk->heapId );

  wk->msgMan = GFL_MSG_Create(GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_trainercard_dat, wk->heapId);

  //�Œ�̃o�b�t�@�쐬
  wk->PlayTimeBuf = GFL_STR_CreateBuffer(TIME_H_DIGIT+1, wk->heapId);
  wk->TmpBuf = GFL_STR_CreateBuffer(TR_STRING_LEN, wk->heapId);
  wk->DigitBuf = GFL_STR_CreateBuffer(TR_DIGIT_LEN, wk->heapId);
  wk->SecBuf =  GFL_STR_CreateBuffer(5, wk->heapId);

  //�Œ蕶����擾
  GFL_MSG_GetString(wk->msgMan, MSG_TCARD_12, wk->SecBuf);

  for(i = 0;i < 3;i++){
    wk->ExpBuf[i] = GFL_MSG_CreateString(wk->msgMan, MSG_TCARD_BACK+i);
  }
  for(i = 0;i < 2;i++){
    wk->SignBuf[i] = GFL_MSG_CreateString(wk->msgMan, MSG_TCARD_SIGN01+i);
  }
  for(i = 0;i < TR_CPRM_STR_MAX;i++){
    wk->CPrmBuf[i] = GFL_MSG_CreateString(wk->msgMan, MSG_TCARD_01+i);
  }
  for(i=0;i<TR_CREDIT_STR_MAX;i++){
    wk->CreditBuf[i] = GFL_MSG_CreateString(wk->msgMan, MSG_TCARD_NUM_KAI+i);
  }
  
  // WordSet������
  wk->wordset = WORDSET_Create( wk->heapId );
}

//--------------------------------------------------------------------------------------------
/**
 * ������o�b�t�@�̉��
 *
 * @param wk    ��ʂ̃��[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void FreeStrBuf( TR_CARD_WORK * wk )
{
  int i;
  // WordSet���
  WORDSET_Delete( wk->wordset );

  for(i=0;i<TR_CREDIT_STR_MAX;i++){
    GFL_STR_DeleteBuffer( wk->CreditBuf[i] );
  }

  for(i = 0;i < TR_CPRM_STR_MAX;i++){
    GFL_STR_DeleteBuffer( wk->CPrmBuf[i] );
  }
  GFL_STR_DeleteBuffer( wk->SignBuf[1] );
  GFL_STR_DeleteBuffer( wk->SignBuf[0] );

  GFL_STR_DeleteBuffer( wk->ExpBuf[2] );
  GFL_STR_DeleteBuffer( wk->ExpBuf[1] );
  GFL_STR_DeleteBuffer( wk->ExpBuf[0] );

  GFL_STR_DeleteBuffer( wk->SecBuf );
  GFL_STR_DeleteBuffer( wk->DigitBuf );
  GFL_STR_DeleteBuffer( wk->TmpBuf );
  GFL_STR_DeleteBuffer( wk->PlayTimeBuf );

  GFL_MSG_Delete( wk->msgMan );

  GFL_FONT_Delete(wk->fontHandle);
}

// �J�[�h�ʃp���b�g���v�a�őւ��邽�߂̃e�[�u��
static int card_palette_table[][2]={
  {    NARC_trainer_case_card_w0_NCLR, NARC_trainer_case_card_0_NCLR, },
  {    NARC_trainer_case_card_w1_NCLR, NARC_trainer_case_card_1_NCLR, },
  {    NARC_trainer_case_card_w2_NCLR, NARC_trainer_case_card_2_NCLR, },
  {    NARC_trainer_case_card_w3_NCLR, NARC_trainer_case_card_3_NCLR, },
  {    NARC_trainer_case_card_w4_NCLR, NARC_trainer_case_card_4_NCLR, },
  {    NARC_trainer_case_card_w5_NCLR, NARC_trainer_case_card_5_NCLR, },
  {    NARC_trainer_case_card_6_NCLR,  NARC_trainer_case_card_6_NCLR, },  // �o�[�W�����Ⴂ
};

//----------------------------------------------------------------------------------
/**
 * @brief �J�[�h�̃p���b�g�]���������N�EWB�ɑΉ����čs��
 *
 * @param   wk    
 * @param   inCardRank    
 * @param   inVersion
 */
//----------------------------------------------------------------------------------
static void SetCardPalette(TR_CARD_WORK *wk ,u8 inCardRank, const u8 inPokeBookHold)
{
  u32 palette_index;
  int Version = 1;

  // �v�a�̃o�[�W�����ɂ��킹�ăo���b�g�؂�ւ��iWB�ȊO�͑S�Đj
  if(wk->TrCardData->Version==VERSION_WHITE){
    Version = 0;
  }else if(wk->TrCardData->Version==VERSION_BLACK){
    Version = 1;
  }else{
    inCardRank = 6;
  }
  
  OS_Printf("rank=%d, version=%d", inCardRank, Version);

  // �v�a�ɂ��킹�ăp���b�g�ǂݍ���
  palette_index = card_palette_table[inCardRank][Version];

  // �p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette( ARCID_TRAINERCARD, palette_index,
                                 PALTYPE_SUB_BG, 0, 2*16*16 ,wk->heapId );
  
  // �_�ŗp�Ƀp���b�g1��ڂ�ۑ����Ă���
  wk->pPalBuf = GFL_ARC_UTIL_LoadPalette( ARCID_TRAINERCARD, palette_index, 
                                          &wk->pPalData, wk->heapId );
}


//--------------------------------------------------------------------------------------------
/**
 * ���j�I���g���[�i�[�p���b�g�Z�b�g
 *
 * @param inTrainerNo
 *
 * @return  �Ȃ�
 */
//--------------------------------------------------------------------------------------------
static void SetUniTrainerPalette(TR_CARD_WORK *wk ,const u8 inTrainerNo)
{
  // ���j�I�����[���p�̃O���t�B�b�N�͂O�s�ڂ̃p���b�g��]������
  // ���A�O���t�B�b�N���̂��̂̃p���b�g�s�͂S�s�ڂ��Q�Ƃ���Ă���̂ł����ɓ]������
  GFL_ARC_UTIL_TransVramPalette( ARCID_TRC_UNION, UniTrTable[inTrainerNo][1], 
                                 PALTYPE_SUB_BG, 32*UNI_TRAINER_PLTT_NO, 32, wk->heapId );
}

//--------------------------------------------------------------------------------------------
/**
 * BG�ݒ�
 *
 * @param ini   BGL�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void SetTrCardBg( void )
{

  GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);

  { // BG SYSTEM
    GFL_BG_SYS_HEADER BGsys_data = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_5, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &BGsys_data );
  }

  { // FONT (BMP) BG3
    GFL_BG_BGCNT_HEADER  ExAffineBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256, 
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x10000, 0x10000, GX_BG_EXTPLTT_01,
      1, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( TRC_BG_FONT, &ExAffineBgCntDat, GFL_BG_MODE_256X16 );
    GFL_BG_ClearFrame( TRC_BG_FONT );
    GFL_BG_SetVisible( TRC_BG_FONT, VISIBLE_ON );
  }

  { // BG (CASE CHAR,MSG_CHAR) BG0 BG1
    GFL_BG_BGCNT_HEADER  TextBgCntDat[] = {
     {  0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000, 0x6000, GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE},
     {  0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x04000, 0x6000, GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE},
    };
    GFL_BG_SetBGControl( TRC_BG_MSG,  &TextBgCntDat[0], GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( TRC_BG_MSG );
    GFL_BG_SetVisible( TRC_BG_MSG, VISIBLE_ON );
    GFL_BG_SetBGControl( TRC_BG_BACK, &TextBgCntDat[1], GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( TRC_BG_BACK );
    GFL_BG_SetVisible( TRC_BG_BACK, VISIBLE_ON );
  }

  { // BG (CARD CHAR)
    GFL_BG_BGCNT_HEADER  AffineBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x08000, 0x8000, GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( TRC_BG_CARD, &AffineBgCntDat, GFL_BG_MODE_256X16 );
    GFL_BG_ClearFrame( TRC_BG_CARD );
    GFL_BG_SetVisible( TRC_BG_CARD, VISIBLE_ON );
  }

  { // BG (BADGE_CASE CHAR)
    GFL_BG_BGCNT_HEADER  TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, 0x8000, GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( TRC_BG_BADGE_CASE, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( TRC_BG_BADGE_CASE );
    GFL_BG_SetVisible( TRC_BG_BADGE_CASE, VISIBLE_OFF );
  }

  { // BG (BADGE_BACK CHAR)
    GFL_BG_BGCNT_HEADER  TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, 0x8000, GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( TRC_BG_BADGE_BACK, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( TRC_BG_BADGE_BACK );
    GFL_BG_SetVisible( TRC_BG_BADGE_BACK, VISIBLE_ON );
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief �J�[�h�O���t�B�b�N��VERSION�œǂݕ����邽�߂�ID�ύX����
 *
 * @param   version   
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int _get_card_ncgr( int version )
{
  if(version==VERSION_WHITE){
    return NARC_trainer_case_card_w_NCGR;
  }else{
    return NARC_trainer_case_card_NCGR;
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief �J�[�h�X�N���[����(�\�j��VERSION�œǂݕ����邽�߂�ID�ύX����
 *
 * @param   version   
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int _get_card_face_nscr( int version )
{
  if(version==VERSION_WHITE){
    return NARC_trainer_case_card_faca_w_NSCR;
  }else{
    return NARC_trainer_case_card_faca_NSCR;
  }

}

//--------------------------------------------------------------------------------------------
/**
 * �O���t�B�b�N�f�[�^�Z�b�g
 *
 * @param wk    ��ʂ̃��[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void SetTrCardBgGraphic( TR_CARD_WORK * wk )
{
  // TRAINER_PALETTE(UP_DISPLAY)
  // CARD PALETTE
#ifdef PM_DEBUG
  if(GFL_UI_KEY_GetCont()&PAD_BUTTON_L){
    wk->TrCardData->PokeBookFlg = 0;
  }
#endif

  SetCardPalette(wk,wk->TrCardData->CardRank, wk->TrCardData->PokeBookFlg);

  //TRAINER
  if (wk->TrCardData->OtherTrCard==FALSE){
    wk->TrArcData = GFL_ARC_UTIL_LoadBGCharacter( ARCID_TRAINERCARD, NARC_trainer_case_card_trainer_NCGR,
                    FALSE, &wk->TrCharData, wk->heapId);
    if (wk->TrCardData->TrSex == PM_MALE){
      //�j
      wk->TrScrnArcData = GFL_ARC_UTIL_LoadScreen( ARCID_TRAINERCARD, NARC_trainer_case_card_trainer01_NSCR,
                          0, &wk->ScrnData, wk->heapId);
    }else{
      //��
      wk->TrScrnArcData = GFL_ARC_UTIL_LoadScreen( ARCID_TRAINERCARD, NARC_trainer_case_card_trainer02_NSCR,
                          0, &wk->ScrnData, wk->heapId);
    }
  }else{
    //���j�I�����[���ő��̐l�̃f�[�^�����鎞
    {
      wk->TrArcData = GFL_ARC_UTIL_LoadBGCharacter( ARCID_TRC_UNION, UniTrTable[wk->TrCardData->UnionTrNo][0],
                    FALSE, &wk->TrCharData, wk->heapId);

      wk->TrScrnArcData = GFL_ARC_UTIL_LoadScreen( ARCID_TRC_UNION, NARC_trc_union_card_test256_NSCR,
                          0, &wk->ScrnData, wk->heapId);
      //�g���[�i�[�p���b�g�ύX
      SetUniTrainerPalette(wk,wk->TrCardData->UnionTrNo);
    }
  }
  DispTrainer(wk);

  //CARD
  GFL_ARC_UTIL_TransVramBgCharacter(
      ARCID_TRAINERCARD, _get_card_ncgr( wk->TrCardData->Version ), TRC_BG_CARD, 0, 0, 0, wk->heapId );
  GFL_ARC_UTIL_TransVramScreen(
      ARCID_TRAINERCARD, _get_card_face_nscr( wk->TrCardData->Version), TRC_BG_CARD, 0, 0, 0, wk->heapId );

  //CARD BACKGROUND
  GFL_ARC_UTIL_TransVramBgCharacter(
      ARCID_TRAINERCARD, NARC_trainer_case_card_case_NCGR, TRC_BG_BACK, 0, 0, 0, wk->heapId );
  GFL_ARC_UTIL_TransVramScreen(
      ARCID_TRAINERCARD, NARC_trainer_case_card_case_NSCR, TRC_BG_BACK, 0, 0, 0, wk->heapId );

  //UPLCD BACKGROUND
  GFL_ARC_UTIL_TransVramBgCharacter(
      ARCID_TRAINERCARD, NARC_trainer_case_card_case_NCGR, TRC_BG_BADGE_BACK, 0, 0, 0, wk->heapId );
  GFL_ARC_UTIL_TransVramScreen(
      ARCID_TRAINERCARD, NARC_trainer_case_card_case_NSCR, TRC_BG_BADGE_BACK, 0, 0, 0, wk->heapId );
    GFL_ARC_UTIL_TransVramPalette( ARCID_TRAINERCARD, NARC_trainer_case_card_w0_NCLR,
          PALTYPE_MAIN_BG , 0 , 16*2 ,wk->heapId );


//  GFL_ARC_UTIL_TransVramScreen( ARCID_TRAINERCARD, NARC_trainer_case_card_case2bg_NSCR,
//    TRC_BG_BADGE_BACK, 0, 0, 0, wk->heapId );

  //�T�C���W�J
  DecodeSignData( wk->TrCardData->SignRawData, wk->TrSignData );

  // �^�b�`�o�[����
  GFL_ARC_UTIL_TransVramBgCharacter( APP_COMMON_GetArcId(),
                                     APP_COMMON_GetBarCharArcIdx(),
                                     TRC_BG_MSG, 0, 0, 0, wk->heapId );
  GFL_ARC_UTIL_TransVramScreen( APP_COMMON_GetArcId(),
                                APP_COMMON_GetBarScrnArcIdx(),
                                TRC_BG_MSG,0,0,0,wk->heapId);
                                
  GFL_BG_ChangeScreenPalette( TRC_BG_MSG, 0, 21, 32, 3, 12 );
  GFL_BG_LoadScreenReq( TRC_BG_MSG );
}

//--------------------------------------------------------------------------------------------
/**
 * BG���
 *
 * @param ini   BGL�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void TrCardBgExit( void )
{

  
  GFL_DISP_GX_SetVisibleControl(
    GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 |
    GX_PLANEMASK_BG3 | GX_PLANEMASK_OBJ, VISIBLE_OFF );
  GFL_DISP_GXS_SetVisibleControl(
    GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 |
    GX_PLANEMASK_BG3 | GX_PLANEMASK_OBJ, VISIBLE_OFF );

  GFL_BG_SetScroll( TRC_BG_BACK, GFL_BG_SCROLL_X_SET, 0);
  GFL_BG_SetScroll( TRC_BG_BACK, GFL_BG_SCROLL_Y_SET, 0);
  GFL_BG_SetScroll( TRC_BG_BADGE_BACK, GFL_BG_SCROLL_X_SET, 0);
  GFL_BG_SetScroll( TRC_BG_BADGE_BACK, GFL_BG_SCROLL_Y_SET, 0);

  GFL_BG_FreeBGControl( TRC_BG_FONT );
  GFL_BG_FreeBGControl( TRC_BG_CARD );
  GFL_BG_FreeBGControl( TRC_BG_MSG );
  GFL_BG_FreeBGControl( TRC_BG_BACK );
  GFL_BG_FreeBGControl( TRC_BG_BADGE_CASE );
  GFL_BG_FreeBGControl( TRC_BG_BADGE_BACK );

  GFL_BG_Exit();
}


//----------------------------------------------------------------------------------
/**
 * @brief �n�[�h�E�F�A�E�C���h�E�̐ݒ�
 *
 * @param   flag    
 */
//----------------------------------------------------------------------------------
static void HardWareWindow_Set( int flag )
{
  if(flag){
    G2S_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_OBJ,0);
    G2S_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_BG3|GX_WND_PLANEMASK_OBJ,0);
    G2S_SetWnd0Position( 24, 0, 232, 16);
    GXS_SetVisibleWnd(GX_WNDMASK_W0);
  }else{
    GXS_SetVisibleWnd(GX_WNDMASK_NONE);
    
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief ���ʃX�N���[����ւ���
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void Trans_CardBackScreen( TR_CARD_WORK *wk, int version )
{
  // �A�j��ON��OFF���œ]������X�N���[����ւ���
  const int card_back_screen[][2]={
    { NARC_trainer_case_card_back_w_NSCR, NARC_trainer_case_card_back_NSCR,},    // �T�C���A�j��OFF
    { NARC_trainer_case_card_back2_w_NSCR, NARC_trainer_case_card_back2_NSCR,}, // �T�C���A�j��ON
  };
  if(version==VERSION_WHITE){
    version = 0;
  }else{
    version = 1;
  }

  GFL_ARC_UTIL_TransVramScreen( ARCID_TRAINERCARD, 
                                card_back_screen[wk->TrCardData->SignAnimeOn][version],
                                TRC_BG_CARD, 0, 0, 0, wk->heapId );
/*
  if(wk->TrCardData->SignAnimeOn){
    GFL_ARC_UTIL_TransVramScreen(
      ARCID_TRAINERCARD, NARC_trainer_case_card_back2_NSCR, TRC_BG_CARD, 0, 0, 0, wk->heapId );
  }else{
    GFL_ARC_UTIL_TransVramScreen(
      ARCID_TRAINERCARD, NARC_trainer_case_card_back_NSCR, TRC_BG_CARD, 0, 0, 0, wk->heapId );
  }
*/
}


//--------------------------------------------------------------------------------------------
/**
 *  @brief  wk->is_back�ϐ��̏�Ԃɂ���āA�J�[�h�̕\or����View���N���A����������
 */
//--------------------------------------------------------------------------------------------
static void CardDesignDraw(TR_CARD_WORK* wk)
{
  if (wk->is_back == FALSE){
    GFL_ARC_UTIL_TransVramScreen(
        ARCID_TRAINERCARD, _get_card_face_nscr( wk->TrCardData->Version), TRC_BG_CARD, 0, 0, 0, wk->heapId );
    //�X�N���[���N���A
    GFL_BG_ClearScreen( TRC_BG_TRAINER );
//    HardWareWindow_Set( 0 );
  }else{
    // ���ʃJ�[�h�X�N���[���]��
    Trans_CardBackScreen(wk, wk->TrCardData->Version);
    //�g���[�i�[����
    ClearTrainer(wk);
    //�X�N���[���N���A
    GFL_BG_ClearScreen( TRC_BG_SIGN );
//    HardWareWindow_Set( 1 );
  }
}


//--------------------------------------------------------------------------------------------
/**
 *  @brief  wk->is_back�ϐ��̏�Ԃɂ���āA�J�[�h�̕\or����View���쐬
 */
//--------------------------------------------------------------------------------------------
static void CardDataDraw(TR_CARD_WORK* wk)
{
  //�w�i�f�U�C����������
  CardDesignDraw(wk);
  TRCBmp_WriteTrWinInfo(wk, wk->win, wk->TrCardData );
  PMS_DRAW_Main( wk->PmsDrawWork );

  if (wk->is_back == FALSE){  //�\
    TRCBmp_WriteScoreListWin( wk, wk->scrol_point, 0, 0 );
    //�g���[�i�[�\��
    DispTrainer(wk);
  }else{  //���ʕ\��
    TRCBmp_WriteTrWinInfoRev(wk, wk->win, wk->TrCardData );
    TransSignData(TRC_BG_SIGN, wk->TrSignData, wk->SignAnimePat);
  }
  
  // �^�b�`�o�[OBJ����̍ĕ`��
  DispTouchBarObj( wk, wk->is_back );

  TRCBmp_TransTrWinInfo(wk,wk->win);
}


//----------------------------------------------------------------------------------
/**
 * @brief �����̃J�[�h��ҏW�\��ԂŌ���Ƃ��̃{�^���\���E��\������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void EditOK_MineCardAppear( TR_CARD_WORK *wk, int is_back )
{
  BOOL flag = FALSE;
 // �\
  if(is_back==0){  
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_BACK,  APP_COMMON_BARICON_RETURN,TRUE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_LOUPE, ANMS_LOUPE_L,             FALSE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_PEN,   ANMS_BLACK_PEN_L,         FALSE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_CHANGE,ANMS_BADGE_L,             TRUE);

    // �V���[�g�J�b�g�o�^���Ă��邩
    flag = GAMEDATA_GetShortCut( wk->tcp->gameData, SHORTCUT_ID_TRCARD_FRONT );
    SetSActDrawSt(&wk->ObjWork,ACTS_BTN_BOOKMARK, APP_COMMON_BARICON_CHECK_OFF+flag, TRUE);

  // ��
  }else{          
    // �ʏ�\��
    if(wk->ScaleMode==0){
      SetSActDrawSt( &wk->ObjWork, ACTS_BTN_BACK,  APP_COMMON_BARICON_RETURN,TRUE);
      SetSActDrawSt( &wk->ObjWork, ACTS_BTN_END,   APP_COMMON_BARICON_EXIT,  TRUE);
      SetSActDrawSt( &wk->ObjWork, ACTS_BTN_TURN,  ANMS_TURN_L,              TRUE);

      SetSActDrawSt( &wk->ObjWork, ACTS_BTN_LOUPE, ANMS_LOUPE_L,           wk->TrCardData->SignAnimeOn^1);
      SetSActDrawSt(&wk->ObjWork,ACTS_BTN_PEN, ANMS_WHITE_PEN_L-wk->pen*2, wk->TrCardData->SignAnimeOn^1);

      // �T�C���A�j������
      Change_SignAnimeButton( wk, wk->TrCardData->SignAnimeOn, TRUE);

      // �V���[�g�J�b�g�o�^���Ă��邩
      flag = GAMEDATA_GetShortCut( wk->tcp->gameData, SHORTCUT_ID_TRCARD_BACK );
      SetSActDrawSt(&wk->ObjWork,ACTS_BTN_BOOKMARK, APP_COMMON_BARICON_CHECK_OFF+flag, TRUE);

    // �g��\��
    }else{
      SetSActDrawSt( &wk->ObjWork, ACTS_BTN_BACK,   APP_COMMON_BARICON_RETURN_OFF, TRUE);
      SetSActDrawSt( &wk->ObjWork, ACTS_BTN_END,    APP_COMMON_BARICON_EXIT_OFF,   TRUE);
      SetSActDrawSt( &wk->ObjWork, ACTS_BTN_LOUPE,  ANMS_SIMPLE_L,                 TRUE);
      SetSActDrawSt( &wk->ObjWork, ACTS_BTN_PEN,    ANMS_WHITE_PEN_L-wk->pen*2,    TRUE);
      SetSActDrawSt( &wk->ObjWork, ACTS_BTN_TURN,   ANMS_TURN_L,                   FALSE);
      SetSActDrawSt( &wk->ObjWork, ACTS_BTN_CHANGE, ANMS_ANIME_L,                  FALSE);
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * @brief �����̃J�[�h��ҏW�\��ԂŌ���Ƃ��̃{�^���\���E��\������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void EditNG_MineCardAppear( TR_CARD_WORK *wk, int is_back )
{
  BOOL flag = FALSE;
 // �\
  if(is_back==0){  
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_BACK,  APP_COMMON_BARICON_RETURN,TRUE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_LOUPE, ANMS_LOUPE_L,             FALSE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_PEN,   ANMS_BLACK_PEN_L,         FALSE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_CHANGE,ANMS_BADGE_L,             TRUE);

    // �V���[�g�J�b�g�o�^���Ă��邩
    flag = GAMEDATA_GetShortCut( wk->tcp->gameData, SHORTCUT_ID_TRCARD_FRONT );
    SetSActDrawSt(&wk->ObjWork,ACTS_BTN_BOOKMARK, APP_COMMON_BARICON_CHECK_OFF+flag, TRUE);

  // ��
  }else{          
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_BACK,  APP_COMMON_BARICON_RETURN,TRUE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_END,   APP_COMMON_BARICON_EXIT,  TRUE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_LOUPE, ANMS_LOUPE_L,             FALSE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_TURN,  ANMS_TURN_L,              TRUE);

    SetSActDrawSt(&wk->ObjWork,ACTS_BTN_PEN, ANMS_WHITE_PEN_L-wk->pen*2, FALSE);

    // �T�C���A�j������
    Change_SignAnimeButton( wk, wk->TrCardData->SignAnimeOn, FALSE);

    // �V���[�g�J�b�g�o�^���Ă��邩
    flag = GAMEDATA_GetShortCut( wk->tcp->gameData, SHORTCUT_ID_TRCARD_BACK );
    SetSActDrawSt(&wk->ObjWork,ACTS_BTN_BOOKMARK, APP_COMMON_BARICON_CHECK_OFF+flag, TRUE);
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief ���l�̃J�[�h��ҏW�\��ԂŌ���Ƃ��̃{�^���\���\���E��\������
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void OtherCardAppear( TR_CARD_WORK *wk, int is_back )
{
 // �\
  if(is_back==0){  
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_BACK,  APP_COMMON_BARICON_RETURN,     TRUE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_END,   APP_COMMON_BARICON_EXIT_OFF,   FALSE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_CHANGE,ANMS_BADGE_L,                  FALSE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_PEN,   ANMS_BLACK_PEN_L,              FALSE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_BOOKMARK,APP_COMMON_BARICON_CHECK_OFF,FALSE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_LOUPE, 0,FALSE);

  // ��
  }else{          
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_BACK,  APP_COMMON_BARICON_RETURN,TRUE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_END,   APP_COMMON_BARICON_EXIT,  FALSE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_LOUPE, ANMS_LOUPE_L,             FALSE);
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_TURN,  ANMS_TURN_L,              TRUE);
    SetSActDrawSt(&wk->ObjWork,ACTS_BTN_PEN, ANMS_WHITE_PEN_L-wk->pen*2, FALSE);
    Change_SignAnimeButton( wk, wk->TrCardData->SignAnimeOn, FALSE);
    SetSActDrawSt(&wk->ObjWork,ACTS_BTN_BOOKMARK, APP_COMMON_BARICON_CHECK_OFF, FALSE);
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief �\���̐؂�ւ�莞�Ƀ^�b�`�o�[��OBJ�\����Ԃ�ύX����
 *
 * @param   wk       TR_CARD_WORK
 * @param   is_back  �\�E��
 */
//----------------------------------------------------------------------------------
static void DispTouchBarObj( TR_CARD_WORK *wk, int is_back )
{

  // �l�̃J�[�h�����Ă���
  if(wk->TrCardData->OtherTrCard==TRUE){
    OtherCardAppear( wk, is_back );
  // �����̃J�[�h�����ҏW�s���
  }else if(wk->tcp->edit_possible==FALSE){
    EditNG_MineCardAppear(wk, is_back);
  // �ҏW�\�Ȏ����̃J�[�h
  }else{
    EditOK_MineCardAppear(wk, is_back);
  }
  
 
}


#define ROTATE_SPEED (0x400)    //90�x�̃J�E���g�A�b�v
#define ROTATE_MAX (0x4000)   //360�x = 0x10000�@90�x = 0x4000 = 16384
#define START_SCALE ( FX32_ONE + FX32_CONST( 0.015f ) ) //���E��2�s�N�Z���g�傳����{��
#define CARD_SCALE_MIN  ( 8.0f )  //�Œ���\����������(�s�N�Z��)
#define CARD_SCALE_MIN_FX ( FX32_CONST(CARD_SCALE_MIN/256.0f) ) //�Œ���\����������


//--------------------------------------------------------------------------------------------
/**
 * �J�[�h���Ђ�����Ԃ�
 *
 * @param wk    ��ʂ̃��[�N
 *
 * @return  BOOL  TRUE:�I�� FALSE�F������
 */
//--------------------------------------------------------------------------------------------
static BOOL CardRev( TR_CARD_WORK *wk )
{
  BOOL rc;
  MtxFx22 mtx;
    fx32 rScale_x,rScale_y;
  rc = FALSE;
  switch(wk->sub_seq){
  case 0:
    wk->RotateCount = 0;
    //��������
    //������Ɗg�債��
    wk->CardScaleX = START_SCALE;
    wk->CardScaleY = START_SCALE;
    PMSND_PlaySE( SND_TRCARD_REV );   //�Ђ�����Ԃ���
    if(wk->is_back==0){
      PMS_DRAW_SetPmsObjVisible( wk->PmsDrawWork, 0, FALSE );
    }
    wk->sub_seq++;
    break;
  case 1:
    //�����k�߂�
    wk->CardScaleX = FX_Mul(FX_CosIdx(wk->RotateCount),START_SCALE);
    if (wk->CardScaleX <= CARD_SCALE_MIN_FX){//�J�[�h�X�N���[���ύX
      wk->CardScaleX = CARD_SCALE_MIN_FX; //�o�O�\���h�~(�l�̓J�[�h�̌��݂��ێ��ł���ʂ̖ڕ���)
      wk->sub_seq++;
    }

    wk->RotateCount += ROTATE_SPEED;
    if( wk->RotateCount >= ROTATE_MAX )
    {
      wk->RotateCount = ROTATE_MAX;
    }
    break;
  case 2:
    wk->is_back ^= 1;
    CardDesignDraw(wk);
    DispTouchBarObj( wk, wk->is_back ); // �^�b�`�o�[OBJ����̍ĕ`��

    wk->sub_seq++;
    break;
  case 3:
    if (wk->is_back == FALSE){
      //�g���[�i�[�\��
      DispTrainer(wk);
    }else{
      TransSignData(TRC_BG_SIGN, wk->TrSignData, wk->SignAnimePat);
    }
    wk->sub_seq++;
    break;
  case 4:
    TRCBmp_TransTrWinInfo(wk,wk->win);
    wk->sub_seq++;
    break;
  case 5:
    //�����L����
    wk->RotateCount -= ROTATE_SPEED;
    if( wk->RotateCount <= 0 )
      wk->RotateCount = 0;

    wk->CardScaleX = FX_Mul(FX_CosIdx(wk->RotateCount),START_SCALE);
    if (wk->CardScaleX <= CARD_SCALE_MIN_FX){//�J�[�h�X�N���[���ύX
      wk->CardScaleX = CARD_SCALE_MIN_FX; //�o�O�\���h�~(�l�̓J�[�h�̌��݂��ێ��ł���ʂ̖ڕ���)
    }
    if( wk->RotateCount == 0 )
    {
      //���̃T�C�Y��
      wk->CardScaleX = 1 << FX32_SHIFT;
      wk->CardScaleY = 1 << FX32_SHIFT;
      rc = TRUE;
      if(wk->is_back==0){
        PMS_DRAW_VisibleSet( wk->PmsDrawWork, 0, TRUE );
        PMS_DRAW_SetPmsObjVisible( wk->PmsDrawWork, 0, TRUE );
      }
    }
    break;
  }
  //�A�t�B���ϊ����s���N�G�X�g
  wk->aff_req = TRUE;
  return rc;
}

//----------------------------------------------------------------------------------
/**
 * @brief �n�[�h�E�F�A�E�C���h�E�Ɣ�������ݒ肷��
 *
 * @param   side    
 */
//----------------------------------------------------------------------------------
static void _blend_win_set( int side )
{
  // �������ƃE�C���h�E�ݒ�OFF
  if(side<0){
    G2S_BlendNone();
    GXS_SetVisibleWnd(GX_WNDMASK_NONE);
  // �E�����E�C���h�E�ݒ�
  }else if(side==0){
    // �P�x�_�E��
    G2S_SetBlendBrightness(
      GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3, -8);
  
    // �E�C���h�E�̐ݒ�
    G2S_SetWnd0Position( 256-32, 0, 256, 192-24);
    G2S_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_BG3|
                            GX_WND_PLANEMASK_OBJ, 1 );
    G2S_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|
                            GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_BG3|
                            GX_WND_PLANEMASK_OBJ,0 );
    GXS_SetVisibleWnd(GX_WNDMASK_W0);

  // �������E�C���h�E�ݒ�
  }else if(side==1){
    // �P�x�_�E��
    G2S_SetBlendBrightness(
      GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3, -8);
  
    // �E�C���h�E�̐ݒ�
    G2S_SetWnd0Position( 0, 0, 32, 192-24);
    G2S_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_BG3|
                            GX_WND_PLANEMASK_OBJ, 1 );
    G2S_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0|GX_WND_PLANEMASK_BG1|
                            GX_WND_PLANEMASK_BG2|GX_WND_PLANEMASK_BG3|
                            GX_WND_PLANEMASK_OBJ,0 );
    GXS_SetVisibleWnd(GX_WNDMASK_W0);
  
  }
}


enum{
  CARDSCALE_WIDE_START=0,
  CARDSCALE_WIDE_CALC,
  CARDSCALE_WIDE_END,
  CARDSCALE_SMALL_START,
  CARDSCALE_SMALL_CALC,
  CARDSCALE_SMALL_END,
  CARDSCALE_MOVE_LEFT_START,
  CARDSCALE_MOVE_LEFT_CALC,
  CARDSCALE_MOVE_LEFT_END,
  CARDSCALE_MOVE_RIGHT_START,
  CARDSCALE_MOVE_RIGHT_CALC,
  CARDSCALE_MOVE_RIGHT_END,
};

//----------------------------------------------------------------------------------
/**
 * @brief �g��T�C����ʂɕύX����(�s���E�߂�j
 *
 * @param   wk    
 *
 * @retval  BOOL    TRUE:�I��
 */
//----------------------------------------------------------------------------------
static BOOL CardScaling( TR_CARD_WORK *wk )
{
  switch(wk->sub_seq){

  // �g���ʂֈڍs
  case CARDSCALE_WIDE_START:
    if(wk->ScaleSide==0){
      wk->CardCenterX = TRCARD_LEFT_SCALE_CENTER_POSX;
      wk->CardCenterY = TRCARD_SCALE_CENTER_POSY;
    }else{
      wk->CardCenterX = TRCARD_RIGHT_SCALE_CENTER_POSX;
      wk->CardCenterY = TRCARD_SCALE_CENTER_POSY;
    }
    wk->sub_seq = 1;
    break;
  case CARDSCALE_WIDE_CALC:
    wk->CardScaleX+=0x100;
    wk->CardScaleY+=0x100;
    if(wk->CardScaleX >= 0x2000){
      wk->CardScaleX = 0x2000;
      wk->CardScaleY = 0x2000;
      wk->sub_seq = 2;
    }
    break;
  case CARDSCALE_WIDE_END:
    wk->ScaleMode = 1;
    DispTouchBarObj( wk, wk->is_back ); // �^�b�`�o�[OBJ����̍ĕ`��
    _blend_win_set(wk->ScaleSide);
    return TRUE;
    break;

  // �ʏ��ʂֈڍs
  case CARDSCALE_SMALL_START:
    wk->sub_seq = 4;
    break;
  case CARDSCALE_SMALL_CALC:
    wk->CardScaleX-=0x100;
    wk->CardScaleY-=0x100;
    if(wk->CardScaleX <= 0x1000){
      wk->CardScaleX = 0x1000;
      wk->CardScaleY = 0x1000;
      wk->sub_seq = 5;
    }
    break;
  case CARDSCALE_SMALL_END:
    wk->CardCenterX = TRCARD_CENTER_POSX;
    wk->CardCenterY = TRCARD_CENTER_POSY;
    wk->ScaleMode = 0;
    DispTouchBarObj( wk, wk->is_back ); // �^�b�`�o�[OBJ����̍ĕ`��
    _blend_win_set(-1);
    return TRUE;
    break;

  // �g���Ԃ̍ۂɍ����̊g��ʂɈړ����鏈��
  case CARDSCALE_MOVE_LEFT_START:
    wk->sub_seq = CARDSCALE_MOVE_LEFT_CALC;
    break;
  case CARDSCALE_MOVE_LEFT_CALC:
    wk->CardCenterX-=8;
    if( wk->CardCenterX < TRCARD_LEFT_SCALE_CENTER_POSX){
      wk->CardCenterX = TRCARD_LEFT_SCALE_CENTER_POSX;
      wk->sub_seq = CARDSCALE_MOVE_LEFT_END;
    }
    break;
  case CARDSCALE_MOVE_LEFT_END:
    wk->ScaleSide ^= 1;
    _blend_win_set(wk->ScaleSide);
    return TRUE;
    break;

  // �g���Ԃ̍ۂɉE���̊g��ʂɈړ����鏈��
  case CARDSCALE_MOVE_RIGHT_START:
    wk->sub_seq = CARDSCALE_MOVE_RIGHT_CALC;
    break;
  case CARDSCALE_MOVE_RIGHT_CALC:
    wk->CardCenterX+=8;
    if( wk->CardCenterX > TRCARD_RIGHT_SCALE_CENTER_POSX){
      wk->CardCenterX = TRCARD_RIGHT_SCALE_CENTER_POSX;
      wk->sub_seq = CARDSCALE_MOVE_RIGHT_END;
    }
    break;
  case CARDSCALE_MOVE_RIGHT_END:
    wk->ScaleSide ^= 1;
    _blend_win_set(wk->ScaleSide);
    return TRUE;
    break;

  }
  
  //�A�t�B���ϊ����s���N�G�X�g
  wk->aff_req = TRUE;
  return FALSE;
}


//----------------------------------------------------------------------------------
/**
 * @brief BG�A�t�B���ϊ����N�G�X�g
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void CardRevAffineSet(TR_CARD_WORK* wk)
{
  MtxFx22 mtx;

  mtx._00 = FX_Inv(wk->CardScaleX);
  mtx._01 = 0;
  mtx._10 = 0;
  mtx._11 = FX_Inv(wk->CardScaleY);
//  SVC_WaitVBlankIntr();          // Waiting the end of VBlank interrup
  G2S_SetBG2Affine(&mtx,          // a matrix for rotation and scaling
          wk->CardCenterX, wk->CardCenterY,      // the center of rotation
          wk->CardOffsetX, wk->CardOffsetY       // the reference point before rotation and scaling applied
      );
  G2S_SetBG3Affine(&mtx,          // a matrix for rotation and scaling
          wk->CardCenterX, wk->CardCenterY,      // the center of rotation
          wk->CardOffsetX, wk->CardOffsetY       // the reference point before rotation and scaling applied
      );
  wk->aff_req = FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �L�[�ƃ^�b�`�̐؂�ւ�����
 *
 * @param wk    ��ʂ̃��[�N
 *
 * @return  int   ���N�G�X�g
 */
//--------------------------------------------------------------------------------------------
static int CheckKTStatus(TR_CARD_WORK* wk)
{
  if(wk->key_mode == GFL_APP_KTST_TOUCH){
    if(GFL_UI_TP_GetCont() != 0){ //
      return 0;
    }
    if(GFL_UI_KEY_GetCont() != 0){
      wk->key_mode = GFL_APP_KTST_KEY;
      return 1;
    }
  }else{
    if(GFL_UI_KEY_GetCont() != 0){
      return 0;
    }
    if(GFL_UI_TP_GetCont() != 0){
      wk->key_mode = GFL_APP_KTST_TOUCH;
      return 0;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------
/**
 * @brief �L�[����
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int CheckKey(TR_CARD_WORK* wk)
{
  const int keyTrg = GFL_UI_KEY_GetTrg();
  if( keyTrg & PAD_BUTTON_CANCEL )
  {
    if(wk->ScaleMode==0){
      SetSActDrawSt( &wk->ObjWork, ACTS_BTN_BACK, 9, TRUE);
      PMSND_PlaySE( SND_TRCARD_CANCEL );   //�I����
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      return TRC_KEY_REQ_RETURN_BUTTON;
    }
  }
  else if(keyTrg & PAD_BUTTON_X )
  {
    if(wk->ScaleMode==0 && wk->TrCardData->OtherTrCard==FALSE){
      SetSActDrawSt( &wk->ObjWork, ACTS_BTN_END, 8, TRUE);
      PMSND_PlaySE( SND_TRCARD_END );   //�I����
      return TRC_KEY_REQ_END_BUTTON;
    }
    
  }
  else if(keyTrg & (PAD_KEY_LEFT|PAD_KEY_RIGHT))
  {
    // �g�債�Ă��Ȃ�
    if(wk->ScaleMode==0){
      SetSActDrawSt( &wk->ObjWork, ACTS_BTN_TURN, ANMS_TURN_G ,TRUE);
      return TRC_KEY_REQ_REV_BUTTON;
    }
  }
  else if(keyTrg & PAD_BUTTON_Y)
  {
    // �u�b�N�}�[�N�iY�{�^�����j���[�j�o�^
    if(wk->TrCardData->OtherTrCard==FALSE){ // �����̃J�[�h�ł���΃o�b�W��ʂ�
      SetBookMark( wk );
    }
  }
  else if(keyTrg & PAD_BUTTON_DECIDE)
  {
    // �����̃J�[�h�ŕ\�Ŋg�僂�[�h�łȂ���΂���΃o�b�W��ʂ�
    if(wk->TrCardData->OtherTrCard==FALSE && wk->is_back==0){ 
      PMSND_PlaySE( SND_TRCARD_DECIDE );
      SetSActDrawSt( &wk->ObjWork, ACTS_BTN_CHANGE, ANMS_BADGE_G, TRUE);
      return TRC_KEY_REQ_BADGE_CALL;
    }
  }

  return TRC_KEY_REQ_NONE;
}

//----------------------------------------------------------------------------------
/**
 * @brief �g���[�i�[�J�[�h�̃u�b�N�}�[�N����
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void SetBookMark( TR_CARD_WORK *wk)
{
  PMSND_PlaySE( SND_TRCARD_BOOKMARK );

  if(wk->is_back==0){   // �\
    BOOL flag = GAMEDATA_GetShortCut( wk->tcp->gameData, SHORTCUT_ID_TRCARD_FRONT );
    flag ^=1;
    SetSActDrawSt(&wk->ObjWork,ACTS_BTN_BOOKMARK, APP_COMMON_BARICON_CHECK_OFF+flag, TRUE);
    GAMEDATA_SetShortCut( wk->tcp->gameData, SHORTCUT_ID_TRCARD_FRONT, flag );
  }else{                // ��
    BOOL flag = GAMEDATA_GetShortCut( wk->tcp->gameData, SHORTCUT_ID_TRCARD_BACK );
    flag ^=1;
    SetSActDrawSt(&wk->ObjWork,ACTS_BTN_BOOKMARK, APP_COMMON_BARICON_CHECK_OFF+flag, TRUE);
    GAMEDATA_SetShortCut( wk->tcp->gameData, SHORTCUT_ID_TRCARD_BACK, flag );
  
  }

}


//----------------------------------------------------------------------------------
/**
 * @brief �A�j���{�^���̕\����؂�ւ���
 *
 * @param   wk    
 *
 * @retval  static    
 */
//----------------------------------------------------------------------------------
static void Change_SignAnimeButton( TR_CARD_WORK *wk, int flag, int OnOff )
{
  if(flag==0){  // �Đ��{�^��
    SetSActDrawSt(&wk->ObjWork,ACTS_BTN_CHANGE, ANMS_ANIME_L, OnOff);
  }else{
    SetSActDrawSt(&wk->ObjWork,ACTS_BTN_CHANGE, ANMS_STOP_L,  OnOff);
  }

}

//----------------------------------------------------------------------------------
/**
 * @brief �A�j���{�^���̕\����؂�ւ���
 *
 * @param   wk    
 *
 * @retval  none
 */
//----------------------------------------------------------------------------------
static void Start_SignAnimeButton( TR_CARD_WORK *wk, int flag )
{
  if(flag==0){  // �Đ��{�^���L��
    SetSActDrawSt(&wk->ObjWork,ACTS_BTN_CHANGE, ANMS_ANIME_G, TRUE);
    GFL_CLACT_WK_SetDrawEnable( wk->ObjWork.ClActWorkS[ACTS_BTN_LOUPE], FALSE );
    GFL_CLACT_WK_SetDrawEnable( wk->ObjWork.ClActWorkS[ACTS_BTN_PEN], FALSE );
  }else{        // ��~�{�^���L��
    SetSActDrawSt(&wk->ObjWork,ACTS_BTN_CHANGE, ANMS_STOP_G,  TRUE);
    GFL_CLACT_WK_SetDrawEnable( wk->ObjWork.ClActWorkS[ACTS_BTN_LOUPE], TRUE );
    GFL_CLACT_WK_SetDrawEnable( wk->ObjWork.ClActWorkS[ACTS_BTN_PEN], TRUE );
  }

}


//----------------------------------------------------------------------------------
/**
 * @brief �T�C���A�j������������
 *
 * @param   wk    
 * @param   flag    
 */
//----------------------------------------------------------------------------------
static void Change_SignAnime( TR_CARD_WORK *wk, int flag )
{
  if(flag){
    wk->SignAnimePat  = SCREEN_SIGN_LEFT;
    wk->SignAnimeWait = 0;
  }else{
    wk->SignAnimePat  = SCREEN_SIGN_ALL;
  }
  Trans_SignScreen( TRC_BG_SIGN, wk->SignAnimePat );
}


//----------------------------------------------------------------------------------
/**
 * @brief �^�b�`���ꂽ�{�^���͋@�\���邩�H
 *
 * @param   wk    TR_CARD_WORK
 * @param   hitNo �{�^���i���o�[
 *
 * @retval  BOOL  TRUE:�N��������EFALSE:�����Ȃ�
 */
//----------------------------------------------------------------------------------
static BOOL _sign_eneble_check( TR_CARD_WORK *wk, int hitNo )
{
  // ����ł��Ȃ����[�h���H
  if(wk->TrCardData->SignDisenable){
    // �ł��Ȃ����[�h�̎��̓G�f�B�b�g�֘A�̃{�^���͖���
    if( hitNo>=3 && hitNo<=9 ){
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------------
/**
 * @brief �ʏ펞�^�b�`����
 *
 * @param   wk    
 * @param   hitNo   
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int normal_touch_func( TR_CARD_WORK *wk, int hitNo )
{
  // �T�C���s���[�h�̎��̓G�f�B�b�g�֘A�͋@�\�����Ȃ�
  if(_sign_eneble_check(wk, hitNo)==TRUE){
    return TRC_KEY_REQ_NONE;
  }

  switch(hitNo){
  case 0:     // �߂�
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_BACK, 9, TRUE);
    PMSND_PlaySE( SND_TRCARD_CANCEL );
    GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
    return TRC_KEY_REQ_RETURN_BUTTON;
    break;
  case 1:     // �I��
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_END, 8, TRUE);
    PMSND_PlaySE( SND_TRCARD_END );
    return TRC_KEY_REQ_END_BUTTON;
    break;
  case 2:     // �J�[�h���Ԃ��{�^��
    GFL_UI_TP_GetPointCont( &wk->tp_x , &wk->tp_y );
    SetSActDrawSt( &wk->ObjWork, ACTS_BTN_TURN, ANMS_TURN_G ,TRUE);
    return TRC_KEY_REQ_REV_BUTTON;
    break;
  case 3:     // �o�b�W��ʃ{�^���E�A�j��ON/OFF�{�^��
    if(wk->is_back){
      // �A�j��ON�^OFF
      if(wk->tcp->TrCardData->EditPossible){    // �ҏW�\�Ȃ�
        Start_SignAnimeButton( wk, wk->TrCardData->SignAnimeOn );
        wk->TrCardData->SignAnimeOn ^=1;
        Trans_CardBackScreen(wk, wk->TrCardData->Version);
        Change_SignAnime( wk, wk->TrCardData->SignAnimeOn );
        OS_Printf("SignAnime = %d\n", wk->TrCardData->SignAnimeOn);
        PMSND_PlaySE( SND_TRCARD_ANIME );
      }
    }else{  
      // �o�b�W��ʂ�
      PMSND_PlaySE( SND_TRCARD_DECIDE );
      SetSActDrawSt( &wk->ObjWork, ACTS_BTN_CHANGE, ANMS_BADGE_G, TRUE);
      return TRC_KEY_REQ_BADGE_CALL;
    }
    break;
  case 4:     // �����`��{�^��
    if(wk->is_back && wk->TrCardData->SignAnimeOn==0){
      if(wk->tcp->TrCardData->EditPossible){    // �ҏW�\�Ȃ�
          if(wk->ScaleMode==0){
            SetSActDrawSt(&wk->ObjWork,ACTS_BTN_LOUPE, ANMS_LOUPE_G, TRUE);
            PMSND_PlaySE( SND_TRCARD_SCALEUP );
            wk->sub_seq = 0;
          }else{
            SetSActDrawSt(&wk->ObjWork,ACTS_BTN_LOUPE, ANMS_SIMPLE_G, TRUE);
            PMSND_PlaySE( SND_TRCARD_SCALEDOWN );
            wk->sub_seq = 3;
          }
          return TRC_KEY_REQ_SCALE_BUTTON;
      }
    }
    break;
  case 5:     // �y����{�^��
    if(wk->is_back){
      // �ҏW�\ & �A�j��������Ȃ�������
      if(wk->tcp->TrCardData->EditPossible && wk->TrCardData->SignAnimeOn==0){    
        PMSND_PlaySE( SND_TRCARD_PEN );
        SetSActDrawSt(&wk->ObjWork,ACTS_BTN_PEN, ANMS_WHITE_PEN_L-wk->pen*2+1, TRUE);
        wk->pen ^=1;
        OS_Printf("pen touch\n");
      }
    }
    break;
  case 6:     // �u�b�N�}�[�N�{�^��
    SetBookMark( wk );
    break;
  case 7:     // �g���[�i�[�^�C�v
    if(wk->is_back==0){
      return TRC_KEY_REQ_TRAINER_TYPE;
    }
    break;
  case 8:     // ���i
    if(wk->is_back==0){
      return TRC_KEY_REQ_PERSONALITY;
    }
    break;
  case 9:     // �ȈՉ�b
    if(wk->is_back==0){
      return TRC_KEY_REQ_PMS_CALL;
    }
    break;
  }
  return TRC_KEY_REQ_NONE;
}

//----------------------------------------------------------------------------------
/**
 * @brief �g�僂�[�h���^�b�`����(���Ȃ�̏��������������j
 *
 * @param   wk    
 * @param   hitNo   
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int large_touch_func( TR_CARD_WORK *wk, int hitNo )
{
  switch(hitNo){
  case 0:     // �߂�
  case 1:     // �I��
  case 2:     // �J�[�h���Ԃ��{�^��
  case 3:     // �o�b�W��ʃ{�^���E�A�j��ON/OFF�{�^��
  case 7:     // �g���[�i�[�^�C�v
  case 8:     // ���i
    break;
  case 4:     // �����`��{�^��
    if(wk->is_back){
      SetSActDrawSt(&wk->ObjWork,ACTS_BTN_LOUPE, ANMS_SIMPLE_G, TRUE);
      PMSND_PlaySE( SND_TRCARD_SCALEDOWN );
      wk->sub_seq = 3;
      return TRC_KEY_REQ_SCALE_BUTTON;
    }
    break;
  case 5:     // �y����{�^��
    if(wk->is_back){
      PMSND_PlaySE( SND_TRCARD_PEN );
      SetSActDrawSt(&wk->ObjWork,ACTS_BTN_PEN, ANMS_WHITE_PEN_L-wk->pen*2+1, TRUE);
      wk->pen ^=1;
      OS_Printf("pen touch\n");
    }
    break;
  case 6:     // �u�b�N�}�[�N�{�^��
    SetBookMark( wk );
    break;
  case 10:     // �E�ֈړ�
    if(wk->ScaleSide==0){
      PMSND_PlaySE( SND_TRCARD_ANIME );
      wk->sub_seq = CARDSCALE_MOVE_RIGHT_START;
      return TRC_KEY_REQ_SCALE_BUTTON;
    }
    break;
  case 11:    // ���ֈړ�
    if(wk->ScaleSide==1){
      PMSND_PlaySE( SND_TRCARD_ANIME );
      wk->sub_seq = CARDSCALE_MOVE_LEFT_START;
      return TRC_KEY_REQ_SCALE_BUTTON;
    }
    break;
  }
  return TRC_KEY_REQ_NONE;
}


// �X�N���[��
static const GFL_UI_TP_HITTBL sc_Scrol_TpRect[] = {
  { TP_SCORE_SCROL_Y, TP_SCORE_SCROL_Y+TP_SCORE_SCROL_H, TP_SCORE_SCROL_X, TP_SCORE_SCROL_X+TP_SCORE_SCROL_W }, // �X�N���[��
  {GFL_UI_TP_HIT_END,0,0,0}
};

//-----------------------------------------------------------------------------
/**
 *  @brief  �J�[�h���ʁF�^�b�`�X�N���[���I�����聕����
 *
 *  @param  TR_CARD_WORK *wk 
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
static void tp_scroll_end_check( TR_CARD_WORK *wk )
{
  if( wk->is_back && GFL_UI_TP_HitCont( sc_Scrol_TpRect ) != 0 )
  {
    if(wk->card_list_col==1)
    {
      TRCBmp_WriteScoreListWin( wk, wk->scrol_point, 1, 0 );
      wk->card_list_col = 0;
      wk->scroll_se_wait = 0;
      wk->b_touch_scroll = FALSE;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �J�[�h���ʁF�^�b�`�X�N���[������
 *
 *  @param  TR_CARD_WORK *wk 
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
static void tp_scroll( TR_CARD_WORK *wk )
{
  u32 x,y;
  
  if(GFL_UI_TP_HitTrg( sc_Scrol_TpRect )==0){
    wk->b_touch_scroll = TRUE;
  }
  
  if( wk->b_touch_scroll && GFL_UI_TP_HitCont( sc_Scrol_TpRect )==0){
    if(GFL_UI_TP_GetTrg()){
      GFL_UI_TP_GetPointTrg( &x, &y );
      wk->touch_sy    = y;
      wk->scrol_start = wk->scrol_point;
    }
    if(GFL_UI_TP_GetCont()){
      GFL_UI_TP_GetPointCont( &x, &y );
      wk->scrol_point = wk->scrol_start - (wk->touch_sy-y);
//      OS_Printf("sy=%d,y=%d,start=%d,point=%d\n",wk->touch_sy,y,wk->scrol_start,wk->scrol_point);
    }
    // �͈͊O�`�F�b�N
    if(wk->scrol_point<-(wk->score_max-4)*16){
      wk->scrol_point = -(wk->score_max-4)*16;
    }else if(wk->scrol_point>0){
      wk->scrol_point = 0;
    }

    // �X�N���[���l�̕ύX������ꍇ�̓��R�[�h���X�g�ĕ`��
    if(wk->old_scrol_point!=wk->scrol_point){
      wk->card_list_col = 1;
      TRCBmp_WriteScoreListWin( wk, wk->scrol_point, 1, 1 );
      PMSND_PlaySE(SND_TRCARD_SLIDE);
    }
    wk->old_scrol_point=wk->scrol_point;
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief �^�b�`����
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int CheckTouch(TR_CARD_WORK* wk)
{
  int ret;
  u16 pat;
  int hitNo;
  // �^�b�`�o�[
  static const GFL_UI_TP_HITTBL Btn_TpRect[] = {
    { TP_RETURN_Y,   TP_RETURN_Y+TP_RETURN_H,    TP_RETURN_X,   TP_RETURN_X+TP_RETURN_W }, // �߂�
    { TP_END_Y,      TP_END_Y+TP_END_H,          TP_END_X,      TP_END_X+TP_END_W },       // �I��
    { TP_CARD_Y,     TP_CARD_Y+TP_CARD_H,        TP_CARD_X,     TP_CARD_X+TP_CARD_W },     // �J�[�h���Ԃ��{�^��
    { TP_BADGE_Y,    TP_BADGE_Y+TP_BADGE_H,      TP_BADGE_X,    TP_BADGE_X+TP_BADGE_W },   // �o�b�W��ʃ{�^��
    { TP_LOUPE_Y,    TP_LOUPE_Y+TP_LOUPE_H,      TP_LOUPE_X,    TP_LOUPE_X+TP_LOUPE_W },   // �����`��{�^��
    { TP_PEN_Y,      TP_PEN_Y+TP_PEN_H,          TP_PEN_X,      TP_PEN_X+TP_PEN_W },       // �y����{�^��
    { TP_BOOKMARK_Y, TP_BOOKMARK_Y+TP_BOOKMARK_H,TP_BOOKMARK_X, TP_BOOKMARK_X+TP_BOOKMARK_W },    //�u�b�N�}�[�N�{�^��
    { TP_TRTYPE_Y, TP_TRTYPE_Y+TP_TRTYPE_H,TP_TRTYPE_X, TP_TRTYPE_X+TP_TRTYPE_W },                //�g���[�i�[�^�C�v
    { TP_PERSONAL_Y, TP_PERSONAL_Y+TP_PERSONAL_H,TP_PERSONAL_X, TP_PERSONAL_X+TP_PERSONAL_W },    //���i
    { TP_PMS_Y,      TP_PMS_Y+TP_PMS_H,          TP_PMS_X,      TP_PMS_X+TP_PMS_W },              //�ȈՉ�b
    { TP_MOVERIGHT_Y,TP_MOVERIGHT_Y+TP_MOVERIGHT_H ,TP_MOVERIGHT_X,TP_MOVERIGHT_X+TP_MOVERIGHT_W},//�E�ֈړ�(�g�厞�j
    { TP_MOVELEFT_Y,TP_MOVELEFT_Y+TP_MOVELEFT_H ,TP_MOVELEFT_X,TP_MOVELEFT_X+TP_MOVELEFT_W},      //���ֈړ�(�g�厞�j
    { GFL_UI_TP_HIT_END, 0, 0, 0 },
  };
  static const GFL_UI_TP_HITTBL Btn_TpRectComm[] = {
    { TP_RETURN_Y,   TP_RETURN_Y+TP_RETURN_H,    TP_RETURN_X,   TP_RETURN_X+TP_RETURN_W }, // �߂�
    { TP_RETURN_Y,   TP_RETURN_Y+TP_RETURN_H,    TP_RETURN_X,   TP_RETURN_X+TP_RETURN_W }, // �߂�
    { TP_CARD_Y,     TP_CARD_Y+TP_CARD_H,        TP_CARD_X,     TP_CARD_X+TP_CARD_W },     // �J�[�h���Ԃ��{�^��
    { GFL_UI_TP_HIT_END,0,0,0}
  };


  // �^�b�`���o
  if(wk->TrCardData->OtherTrCard==FALSE){
    hitNo = GFL_UI_TP_HitTrg( Btn_TpRect );
  }else{
    hitNo = GFL_UI_TP_HitTrg( Btn_TpRectComm );
  }
  // �g���Ԃ��ǂ���
  if(wk->ScaleMode==0){
    ret = normal_touch_func( wk, hitNo );
  }else{
    ret = large_touch_func( wk, hitNo );
  }
  if(ret!=TRC_KEY_REQ_NONE){
    return ret;
  }

  // ���ʗp����
  if(wk->is_back){
    // �g�債�ĂȂ��ăA�j�����ĂȂ�
    if(wk->ScaleMode==0){
      // �X�N���[���E�T�C������
      tp_scroll(wk);
      Stock_TouchPoint( wk, 0 );
    }else{
      // �T�C���ʏ��擾����
      Stock_TouchPoint( wk, 1 );
    }

  }

  return TRC_KEY_REQ_NONE;
}


//----------------------------------------------------------------------------------
/**
 * @brief ���͌��o�i�L�[�E�^�b�`�j
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int CheckInput(TR_CARD_WORK *wk)
{
  int key_req;
  BOOL tp_key_flg;
  tp_key_flg = FALSE;
  key_req = TRC_KEY_REQ_NONE;

//  wk->touch = RECT_HIT_NONE;

  //�L�[�ƃ^�b�`�̐ؑ֊Ď�
  CheckKTStatus(wk);

  //�^�b�`�X�N���[���I������
  tp_scroll_end_check(wk);
  //�T�C���I������
  tp_sign_end_check(wk);

  if(wk->key_mode == GFL_APP_KTST_TOUCH){
    return CheckTouch(wk);
  }else{
    return CheckKey(wk);
  }
  return key_req;
}


//----------------------------------------------------------------------------------
/**
 * @brief ���͓��e�ɂ���ăV�[�P���X�𕪊򂳂���
 *
 * @param   wk    
 * @param   req   ���͌��ʂ̃��N�G�X�g���e
 */
//----------------------------------------------------------------------------------
static void JumpInputResult( TR_CARD_WORK *wk, int req, int *seq )
{
  switch( req ){
  //�J�[�h�Ђ�����Ԃ�
  case TRC_KEY_REQ_REV_BUTTON:
    wk->sub_seq = 0;    //�T�u�V�[�P���X������
    *seq = SEQ_REVERSE; //�J�[�h��]
    break;
  // �g��k��
  case TRC_KEY_REQ_SCALE_BUTTON:
    if(wk->tcp->TrCardData->EditPossible){    // �ҏW�\�Ȃ�
      *seq = SEQ_SCALING;
    }
    break;
  // �ȈՉ�b�Ăяo��
  case TRC_KEY_REQ_PMS_CALL:
    if(wk->tcp->TrCardData->EditPossible){    // �ҏW�\�Ȃ�
      PMSND_PlaySE( SND_TRCARD_PMS );
      wk->sub_seq = 0;
      *seq = SEQ_PMSINPUT;
    }
    break;
  // �ʏ�I��
  case TRC_KEY_REQ_RETURN_BUTTON:
    *seq = SEQ_RETURN_FADEOUT;
    wk->tcp->value = CALL_NONE;
    wk->tcp->next_proc = TRAINERCARD_NEXTPROC_RETURN;
    break;
  // �t�B�[���h���ڏI��
  case TRC_KEY_REQ_END_BUTTON:
    *seq = SEQ_END_FADEOUT;
    wk->tcp->value = CALL_NONE;
    wk->tcp->next_proc = TRAINERCARD_NEXTPROC_EXIT;
    break;
  // �g���[�i�[�^�C�v�؂�ւ�
  case TRC_KEY_REQ_TRAINER_TYPE:
    if(wk->tcp->TrCardData->EditPossible){    // �ҏW�\�Ȃ�
      TR_CARD_SV_PTR trsave = GAMEDATA_GetTrainerCardPtr(wk->tcp->gameData);

      TRCSave_SetTrainerViewChange( trsave );   //�ύX�����t���O�𗧂Ă�
      PMSND_PlaySE( SND_TRCARD_TRTYPE );
      _add_UnionTrNo( wk->TrCardData );
      TRCBmp_PrintTrainerType( wk, wk->TrCardData->UnionTrNo, 1 );
    }
    break;
  // ���i�؂�ւ�
  case TRC_KEY_REQ_PERSONALITY:
    if(wk->tcp->TrCardData->EditPossible){    // �ҏW�\�Ȃ�
      PMSND_PlaySE( SND_TRCARD_PERSONALITY );
      if(++wk->TrCardData->Personality>(TRCARD_PERSONARITY_MAX-1)){
        wk->TrCardData->Personality = 0;
      }
      TRCBmp_PrintPersonality( wk, wk->TrCardData->Personality, 1 );
    }
    break;
  // �o�b�W�{����ʂ�
  case TRC_KEY_REQ_BADGE_CALL:
    wk->sub_seq = 0;
    *seq = SEQ_BADGE_VIEW_CALL;
    break;
  }
}


//--------------------------------------------------------------------------------------------
/**
 * VBlank�֐�
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void VBlankFunc( GFL_TCB *tcb, void *work )
{
  TR_CARD_WORK* wk = (TR_CARD_WORK*)work;
  int scr;

  // �X�N���[���e�L�X�g�_��
  UpdateTextBlink( wk );


  //�w�i�X�N���[��
  scr = -(wk->scrl_ct/2);
  GFL_BG_SetScroll( TRC_BG_BACK, GFL_BG_SCROLL_X_SET,scr );
  GFL_BG_SetScroll( TRC_BG_BACK, GFL_BG_SCROLL_Y_SET,scr );
  GFL_BG_SetScroll( TRC_BG_BADGE_BACK, GFL_BG_SCROLL_X_SET, scr);
  GFL_BG_SetScroll( TRC_BG_BADGE_BACK, GFL_BG_SCROLL_Y_SET, scr);

  if(wk->aff_req){
    CardRevAffineSet(wk);
  }
  GFL_BG_VBlankFunc();

  GFL_CLACT_SYS_VBlankFunc();


//  OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------------------------------------
/**
 * �g�k�ʃ��Z�b�g
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void ResetAffinePlane(void)
{
  MtxFx22 mtx;
    fx32 rScale_x,rScale_y;

  rScale_x = FX_Inv((1 << FX32_SHIFT));
  rScale_y = FX_Inv((1 << FX32_SHIFT));
  mtx._00 = rScale_x;
  mtx._01 = 0;
  mtx._10 = 0;
  mtx._11 = rScale_y;
  SVC_WaitVBlankIntr();          // Waiting the end of VBlank interrup
  G2S_SetBG2Affine(&mtx,          // a matrix for rotation and scaling
            128, 96,      // the center of rotation
            0, 0           // the reference point before rotation and scaling applied
      );

  G2S_SetBG3Affine(&mtx,          // a matrix for rotation and scaling
            128, 96,      // the center of rotation
            0, 0           // the reference point before rotation and scaling applied
      );
  SVC_WaitVBlankIntr();          // Waiting the end of VBlank interrup
  G2_SetBG3Affine(&mtx,          // a matrix for rotation and scaling
            128, 0,      // the center of rotation
            0, 0           // the reference point before rotation and scaling applied
      );
}

#define TRAINER_SCREEN_PASTE_X  ( 21 )
#define TRAINER_SCREEN_PASTE_Y  (  2 )
#define TRAINER_SCREEN_PASTE_W  ( 10 )
#define TRAINER_SCREEN_PASTE_H  ( 11 )
#define TRAINER_SCREEN_SRC_X    ( 21 )
#define TRAINER_SCREEN_SRC_Y    (  2 )

//--------------------------------------------------------------------------------------------
/**
 * �g���[�i�[�X�N���[���]��
 *
 * @param wk      ��ʂ̃��[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void DispTrainer(TR_CARD_WORK *wk)
{
  u32 transSize;

  transSize = wk->ScrnData->szByte;

  //TRAINER
  GFL_BG_LoadCharacter(TRC_BG_TRAINER, wk->TrCharData->pRawData, wk->TrCharData->szByte, 0);

  GFL_BG_WriteScreenExpand(TRC_BG_TRAINER,
    TRAINER_SCREEN_PASTE_X,TRAINER_SCREEN_PASTE_Y,
    TRAINER_SCREEN_PASTE_W,TRAINER_SCREEN_PASTE_H,
    wk->ScrnData->rawData,
    TRAINER_SCREEN_SRC_X,
    TRAINER_SCREEN_SRC_Y,
    wk->ScrnData->screenWidth/8,wk->ScrnData->screenHeight/8);
  GFL_BG_LoadScreenReq( TRC_BG_TRAINER );

}

//--------------------------------------------------------------------------------------------
/**
 * �g���[�i�[�X�N���[���N���A
 *
 * @param wk      ��ʂ̃��[�N
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void ClearTrainer(TR_CARD_WORK *wk)
{
  GFL_BG_FillScreen(TRC_BG_TRAINER, 0, 20, 6, 6, 9, GFL_BG_SCRWRT_PALNL);
}


//--------------------------------------------------------------------------------------------
/**
 * �T�C���f�[�^�W�J�ibit�f�[�^��BMP256�f�[�^�ɓW�J�j
 *
 * @param *inRawData  �r�b�g�P�ʂŎ����Ă���T�C���f�[�^
 * @param *outData  �T�C���f�[�^�i�[�ꏊ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void DecodeSignData( const u8 *inRawData, u8 *outData )
{

  int dot,raw_dot;
  u8 raw_line;  //0�`7
  u8 shift;   //0�`7
  for(dot=0;dot<SIGN_SIZE_X*SIGN_SIZE_Y*64;dot++){
    raw_dot = dot/64;
    raw_line = (dot/8)%8;
    shift = (dot%8);
    outData[dot] = ( 0x01 & (inRawData[(raw_dot*8)+raw_line]>>shift) );
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief �T�C���f�[�^���k�iBMP256�f�[�^��1bit�f�[�^�Ɉ��k�j
 *
 * @param   inBmpData   
 * @param   outData   
 */
//----------------------------------------------------------------------------------
static void EncodeSignData( const u8 *inBmpData, u8 *outData )
{
  int i,r,tmp;

  // 1bit�P�ʂŊi�[����̂�1/8�̃��[�v�ł��ނ͂�
  for(i=0;i<SIGN_SIZE_X*SIGN_SIZE_Y*8;i++){
    tmp = 0;
    for(r=0;r<8;r++){
      tmp |= (inBmpData[i*8+r]<<r);
    }
    outData[i] = tmp;
  }
}

#define SIGN_HALF_SIZE    ( SIGN_SX/2 )

//----------------------------------------------------------------------------------
/**
 * @brief �T�C����ʂ̃X�N���[���f�[�^���H�E�]�����s��
 *
 * @param   inFrame   �w��X�N���[��
 * @param   flag      SCREEN_SIGN_ALL: SCREEN_SIGN_LEFT:  SCREEN_SIGN_RIGHT
 */
//----------------------------------------------------------------------------------
static void Trans_SignScreen( const int inFrame, const int flag )
{
  u16 *buf;
  u8 x,y;
  u16 count;

  // �X�N���[���f�[�^�̉��H
  buf   = (u16 *)GFL_BG_GetScreenBufferAdrs( inFrame );
  
  // �S�N���A 
  for( y=0; y<SIGN_SY; y++ ){
    for( x=0; x<SIGN_SX; x++ ){
      buf[ (SIGN_PY+y)*TRC_SCREEN_WIDTH + x+SIGN_PX ]=0;
    }
  }

  // ���[�h�ɂ��킹�ăX�N���[�������H  
  count = 0;
  switch(flag){
  case SCREEN_SIGN_ALL:   // �S��
    for( y=0; y<SIGN_SY; y++ ){
      for( x=0; x<SIGN_SX; x++ ){
        buf[ (SIGN_PY+y)*TRC_SCREEN_WIDTH + x+SIGN_PX ] = (SIGN_CGX+count);
        count++;
      }
    }
    break;
  case SCREEN_SIGN_LEFT:  // ������
    for( y=0; y<SIGN_ANIME_SY; y++ ){
      for( x=0; x<SIGN_ANIME_SX; x++ ){
        buf[ (SIGN_ANIME_PY+y)*TRC_SCREEN_WIDTH + x+SIGN_ANIME_PX ] = (SIGN_CGX+count);
        count++;
      }
      count+=SIGN_HALF_SIZE;
    }
    break;
  case SCREEN_SIGN_RIGHT: // �E����
    count+=SIGN_HALF_SIZE;
    for( y=0; y<SIGN_ANIME_SY; y++ ){
      for( x=0; x<SIGN_ANIME_SX; x++ ){
        buf[ (SIGN_ANIME_PY+y)*TRC_SCREEN_WIDTH + x+SIGN_ANIME_PX ] = (SIGN_CGX+count);
        count++;
      }
      count+=SIGN_HALF_SIZE;
    }
    break;
  }
  
  // �X�N���[���]��
  GFL_BG_LoadScreenV_Req( inFrame );

}

//--------------------------------------------------------------------------------------------
/**
 * �T�C���f�[�^�]��
 *
 * @param bgl       bgl
 * @param inFrame     �Ώۃt���[��
 * @param inSignData    �T�C���f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void TransSignData(const int inFrame, const u8 *inSignData, const int flag)
{

  // �T�C���O���t�B�b�N�f�[�^��VRAM�ɓ]��
  GFL_BG_LoadCharacter( inFrame, inSignData, SIGN_BYTE_SIZE, SIGN_CGX );

  // �X�N���[���f�[�^�̉��H�E�]�����s��
  Trans_SignScreen( inFrame, flag );
}


#define SIGN_ANIME_SWITCH_FRAME   ( 20 )


//----------------------------------------------------------------------------------
/**
 * @brief �T�C����ʃA�j���̍Đ�����
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void UpdateSignAnime( TR_CARD_WORK *wk )
{
  // ���ʂŃA�j���t���O�������Ă���Ȃ�T�C���A�j�����s��
  if(wk->is_back && wk->TrCardData->SignAnimeOn ){
    wk->SignAnimeWait++;
    if(wk->SignAnimeWait>SIGN_ANIME_SWITCH_FRAME){
      wk->SignAnimePat^=1;
      Trans_SignScreen( TRC_BG_SIGN, wk->SignAnimePat );
      wk->SignAnimeWait = 0;
    }
  }
}

#define BLINK_COUNT_MAX       ( 8 )  ///< �_�ŃJ�E���^�̍ő�l
#define BLINK_PAL_DES_OFFSET  (  3 )  ///< �]����I�t�Z�b�g
#define BLINK_PAL_SRC_OFFSET  (  3 )  ///< �]�����I�t�Z�b�g

//----------------------------------------------------------------------------------
/**
 * @brief ���R�[�h�f�[�^�X�N���[�����p���b�g�̓_�ŏ���
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void UpdateTextBlink( TR_CARD_WORK *wk )
{
  u16 *pal = wk->pPalData->pRawData;
  if(++wk->blink_count > BLINK_COUNT_MAX){
    wk->blink_count = 0;
  }
  if(wk->blink_count==0){
    GXS_LoadBGPltt( &pal[BLINK_PAL_SRC_OFFSET], BLINK_PAL_DES_OFFSET*2, 4 );
  }else if(wk->blink_count==(BLINK_COUNT_MAX/2)){
    GXS_LoadBGPltt( &pal[BLINK_PAL_SRC_OFFSET+2], BLINK_PAL_DES_OFFSET*2, 4 );
  }
}


//---------------------------------------------------------
// �u���V�p�^�[��
//---------------------------------------------------------

// �ǂ����Ă��p���b�g�f�[�^��3bit�ɏk�߂Ă��܂������̂ŁA�����F��8�ɂ���
// �F�w�莩�̂�0-7�ɓ��ĂĂ���B0�͓����F����BD�ʂ����ɂ��č���������悤�ɂ��Ă���

// BMP�f�[�^�͍Œቡ8dot���K�v�Ȃ̂ŁA4x4�̃h�b�g�f�[�^����肽������
// �Q�o�C�g���ƂɎQ�Ƃ���Ȃ��f�[�^�������Q�o�C�g�K�v
static const u8 sign_brush[2][2][16]={
  {
    {
      0,1,1,0, 
      1,1,1,1,  
      1,1,1,1,  
      0,1,1,0,  
    },
    {
      0,2,2,0, 
      2,2,2,2, 
      2,2,2,2,
      0,2,2,0,  
    },
  },
  {
    {
      0,0,0,0, 
      0,1,1,0,  
      0,1,1,0,  
      0,0,0,0,  
    },
    {
      0,0,0,0, 
      0,2,2,0, 
      0,2,2,0,
      0,0,0,0,  
    },
  },    
};                                                                    

// ���������{�[�hBMP�i����ʁj
#define OEKAKI_BOARD_POSX  (  4 )
#define OEKAKI_BOARD_POSY  ( 11 )
#define OEKAKI_BOARD_W     ( 24 )
#define OEKAKI_BOARD_H     (  8 )

#define LARGE_BOARD_POSX  (  4 )
#define LARGE_BOARD_POSY  (  3 )
#define LARGE_BOARD_W     ( 24 )
#define LARGE_BOARD_H     ( 16 )

static const GFL_UI_TP_HITTBL sign_tbl[]={
  {OEKAKI_BOARD_POSY*8,(OEKAKI_BOARD_POSY+OEKAKI_BOARD_H)*8, OEKAKI_BOARD_POSX*8, (OEKAKI_BOARD_POSX+OEKAKI_BOARD_W)*8+1},
  {GFL_UI_TP_HIT_END,0,0,0},
};

static const GFL_UI_TP_HITTBL large_sign_tbl[]={
  {LARGE_BOARD_POSY*8,(LARGE_BOARD_POSY+LARGE_BOARD_H)*8, LARGE_BOARD_POSX*8, (LARGE_BOARD_POSX+LARGE_BOARD_W)*8},
  {GFL_UI_TP_HIT_END,0,0,0},
};

// ���T�C���I������p �[��h��Ԃ����ׂ̈Ɉ���傫��
static const GFL_UI_TP_HITTBL sign_release_tbl[]={
  {OEKAKI_BOARD_POSY*8-16,(OEKAKI_BOARD_POSY+OEKAKI_BOARD_H)*8+16, OEKAKI_BOARD_POSX*8-16, (OEKAKI_BOARD_POSX+OEKAKI_BOARD_W)*8+16},
  {GFL_UI_TP_HIT_END,0,0,0},
};

static const GFL_UI_TP_HITTBL large_sign_release_tbl[]={
  {LARGE_BOARD_POSY*8-16,(LARGE_BOARD_POSY+LARGE_BOARD_H)*8+16, LARGE_BOARD_POSX*8-16, (LARGE_BOARD_POSX+LARGE_BOARD_W)*8+16},
  {GFL_UI_TP_HIT_END,0,0,0},
};

//-----------------------------------------------------------------------------
/**
 *  @brief  �T�C���I������ �͈͊O�ɏo����܂��V���Ƀ^�b�`���Ȃ����Ȃ��Ə����Ȃ�
 *
 *  @param  TR_CARD_WORK *wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void tp_sign_end_check( TR_CARD_WORK *wk )
{
  if( ( wk->is_back ) &&
      ( wk->ScaleMode == 0 && GFL_UI_TP_HitCont( sign_release_tbl ) != 0 ) ||
      ( wk->ScaleMode == 1 && GFL_UI_TP_HitCont( large_sign_release_tbl ) != 0 ) )
  {
      wk->b_touch_sign = FALSE;
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief �T�C���ʂ̃^�b�`�p�l�����擾
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void Stock_TouchPoint( TR_CARD_WORK *wk, int scale_mode )
{
  //  �T���v�����O�����擾���Ċi�[
  u32 x,y;

  // �T�C�����������s��(�ҏW�~���T�C���A�j�����j��������I��
  if(wk->TrCardData->SignDisenable || wk->TrCardData->SignAnimeOn==1){
    return;
  }

  if(GFL_UI_TP_GetPointCont( &x, &y )){

    // �ʏ�
    if(scale_mode==0){
      if( GFL_UI_TP_HitTrg( sign_tbl ) == 0 ){
        wk->b_touch_sign = TRUE;
      }
      
      if( wk->b_touch_sign && GFL_UI_TP_HitSelf( sign_tbl, x, y )==0){
        wk->MyTouchResult.x = x;
        wk->MyTouchResult.y = y;
        wk->MyTouchResult.brush = wk->pen;
        wk->MyTouchResult.on    = 1;
  
        // �g�傷��ۂ̍��E�����߂�
        if(x<128){
          wk->ScaleSide=0;
        }else{
          wk->ScaleSide=1;
        }
      }else{
        wk->MyTouchResult.on    = 0;
      }

    // ����
    }else{
      if( GFL_UI_TP_HitTrg( large_sign_tbl ) == 0 ){
        wk->b_touch_sign = TRUE;
      }

      if( wk->b_touch_sign && GFL_UI_TP_HitSelf( large_sign_tbl, x, y )==0){
        wk->MyTouchResult.x = OEKAKI_BOARD_POSX*8+(x-LARGE_BOARD_POSX*8+wk->ScaleSide*(24*8))/2;
        wk->MyTouchResult.y = OEKAKI_BOARD_POSY*8+(y-LARGE_BOARD_POSY*8)/2;
        wk->MyTouchResult.brush = wk->pen;
        wk->MyTouchResult.on    = 1;
      }else{
        wk->MyTouchResult.on    = 0;
      }
    }
  }else{
    wk->MyTouchResult.on    = 0;
  }

  wk->AllTouchResult = wk->MyTouchResult;

}

//----------------------------------------------------------------------------------
/**
 * @brief 256�F��BMPWIN�ɒ��ړ_���������ރ��[�`��
 *
 * @param   sign    �T�C���ʂ̉摜�z��
 * @param   brush   �u���V�f�[�^
 * @param   x   �T�C���ʂɏ������ޓ_X
 * @param   y   �T�C���ʂɏ������ޓ_Y
 * @param   ww  �T�C���ʂ̉���
 * @param   hh  �T�C���ʂ̏c��
 */
//----------------------------------------------------------------------------------
static void draw_pen( u8 *sign, u8* brush, int x, int y, int ww, int hh, int scale_mode, int scale_side )
{
  int bx,by,px,py,wx,wy;
  int i,offset,w,h,count=0;
  
  // �������ނ̂�4x4�Ɍ���
  for(h=0;h<4;h++){
    wy = y+h;
    by = wy / 8;
    py = wy % 8;
    for(w=0;w<4;w++){
      wx = x+w;
      if(wx<0 || wy<0){
//        HOSAKA_Printf("count=%d wx=%d wy=%d \n",count,wx,wy);
        count++;
        continue;
      }
      if(wx >= ww || wy >= hh){
        continue;
      }
      // �g�厞�A�����ւ̏������݂����E���z���Ĕ��Α��ɏ����Ă��܂��̂ŋK��l�ōi���݁B
      if( scale_mode == 1 )
      {
        if( ( scale_side == 0 && wx >= SCALE_SIDE_LEFT_MAX_X ) ||
            ( scale_side == 1 && wx <= SCALE_SIDE_RIGHT_MIN_X ) )
        {
          count++;
          continue;
        }
      }

      bx = wx / 8;
      px = wx % 8;
      offset = (by*OEKAKI_BOARD_W+bx)*64 + py*8+px;
//      HOSAKA_Printf("w=%d h=%d bx=%d by=%d px=%d py=%d wx=%d wy=%d offset=%d\n", w,h,bx,by,px,py,wx,wy,offset );
      if(brush[count]==1){
        sign[offset] = 1;
      }else if(brush[count]==2){
        sign[offset] = 0;
      }
#if 0
      else if( ( bx == 0 && px == 0 ) || ( by == 0 && py == 0 ) ){
        // �[�Ȃ�u���V�̒����œh��
        if(brush[5]==1){
          sign[offset] = 1;
        }else if(brush[5]==2){
          sign[offset] = 0;
        }
      }
#endif
      count++;
    }
  }
}

//==============================================================================
/**
 * @brief   �`��J�n�ʒu���}�C�i�X�����ɂ����Ă��`��ł���BmpWinPrint���b�p�[
 * @retval  none    
 */
//==============================================================================
#if 0
static void _BmpWinPrint_Rap(
      GFL_BMPWIN * win, void * src,
      int src_x, int src_y, int src_dx, int src_dy,
      int win_x, int win_y, int win_dx, int win_dy )
{

  if(win_x < 0){
    int diff;
    diff = win_x*-1;
    if(diff>win_dx){
      diff = win_dx;
    }
    win_x   = 0;
    src_x  += diff;
    src_dx -= diff;
    win_dx -= diff;
  }

  if(win_y < 0){
    int diff;
    diff = win_y*-1;
    if(diff>win_dy){
      diff = win_dy;
    }
    win_y   = 0;
    src_y  += diff;
    src_dy -= diff;
    win_dy -= diff;
  }

//  GF_BGL_BmpWinPrint( win, src, src_x, src_y, src_dx, src_dy, win_x, win_y, win_dx, win_dy );
/*
  GFL_BMP_Print( src, win, src_x, src_y, src_dx, src_dy, win_x, win_y, 0 );
*/
  {
    u8 *sign = (u8*)win;
    draw_pen( sign, src, win_x, win_y, OEKAKI_BOARD_W*8, OEKAKI_BOARD_H*8 );
  }
}
#endif


//------------------------------------------------------------------
/**
 * @brief   ���C���`��
 *
 * @param   win   
 * @param   brush   
 * @param   px    
 * @param   py    
 * @param   sx    
 * @param   sy    
 * @param   count   
 * @param   flag    
 *
 * @retval  lines 
 */
//------------------------------------------------------------------
static int DrawPoint_to_Line( 
  GFL_BMPWIN *win, 
  const u8 *brush, 
  int px, int py, int *sx, int *sy, 
  int count, int flag, int scale_mode, int scale_side )
{
  int dx, dy, s, step;
  int x1 = *sx;
  int y1 = *sy;
  int x2 = px;
  int y2 = py;
  int lines = 0;

  // ����͌��_�ۑ��̂�
  if(count==0 && flag == 0){
    *sx = px;   *sy = py;
    return lines;
  }
  

  dx = MATH_IAbs(x2 - x1);  dy = MATH_IAbs(y2 - y1);
  if (dx > dy) {
    if (x1 > x2) {
      step = (y1 > y2) ? 1 : -1;
      s = x1;  x1 = x2;  x2 = s;  y1 = y2;
    } else step = (y1 < y2) ? 1: -1;

    draw_pen( (u8*)win, (void*)brush, x1, y1, OEKAKI_BOARD_W*8, OEKAKI_BOARD_H*8, scale_mode, scale_side );
    //_BmpWinPrint_Rap( win, (void*)brush,  0, 0, 4, 4, x1, y1, 4, 4 );
    lines++;
    s = dx >> 1;
    while (++x1 <= x2) {
      if ((s -= dy) < 0) {
        s += dx;  y1 += step;
      };
      draw_pen( (u8*)win, (void*)brush, x1, y1, OEKAKI_BOARD_W*8, OEKAKI_BOARD_H*8, scale_mode, scale_side );
      //_BmpWinPrint_Rap( win, (void*)brush,  0, 0, 4, 4, x1, y1, 4, 4 );
      lines++;
    }
  } else {
    if (y1 > y2) {
      step = (x1 > x2) ? 1 : -1;
      s = y1;  y1 = y2;  y2 = s;  x1 = x2;
    } else step = (x1 < x2) ? 1 : -1;
    draw_pen( (u8*)win, (void*)brush, x1, y1, OEKAKI_BOARD_W*8, OEKAKI_BOARD_H*8, scale_mode, scale_side );
    //_BmpWinPrint_Rap( win, (void*)brush,  0, 0, 4, 4, x1, y1, 4, 4 );
    lines++;
    s = dy >> 1;
    while (++y1 <= y2) {
      if ((s -= dx) < 0) {
        s += dy;  x1 += step;
      }
      draw_pen( (u8*)win, (void*)brush, x1, y1, OEKAKI_BOARD_W*8, OEKAKI_BOARD_H*8, scale_mode, scale_side );
      //_BmpWinPrint_Rap( win, (void*)brush,  0, 0, 4, 4, x1, y1, 4, 4 );
      lines++;
    }
  }
  
  
  *sx = px;     *sy = py;

  return lines;
}

static void Stock_OldTouch( TOUCH_INFO *all, TOUCH_INFO *stock )
{
  *stock = *all;

/*
  int i;
  stock[i].size = all[i].size;
  if(all[i].size!=0){
    stock[i].x = all[i].x[all[i].size-1];
    stock[i].y = all[i].y[all[i].size-1];
  }
*/
}



//----------------------------------------------------------------------------------
/**
 * @brief �擾���������^�b�`�p�l���̌��ʃf�[�^�����ɕ`�悷��
 *
 * @param   win   ��������BMPWIN
 * @param   all   �^�b�`���_�i�[�z��
 * @param   old   1sync�O�̃^�b�`���_�i�[�z��
 * @param   draw    ��������ōs����CGX�ύX��]�����邩�H(0:���Ȃ�  1:����j
 * @param   SignData    ��������̃T�C���f�[�^
 * @param   sign_mode   �g�僂�[�h���ǂ����i0:�ʏ�  1:�g�僂�[�h�j
 * @param   scale_side  �g�僂�[�h���̍��E���[�h�i0:���@1:�E�j
 *
 * @retval  line        �h�b�g����������
 */
//----------------------------------------------------------------------------------
static  int DrawBrushLine( GFL_BMPWIN *win, TOUCH_INFO *all, TOUCH_INFO *old, int draw, u8 *SignData, u8 sign_mode, u8 scale_side )
{
  int px,py,i,r,flag=0, sx, sy, centerling;
  int line = 0;

  //OS_Printf("id0=%d,id1=%d,id2=%d,id3=%d,id4=%d\n",all[0].size,all[1].size,all[2].size,all[3].size,all[4].size);
  if(sign_mode==0){
    centerling = 4;
  }else{
    centerling = 2;
  }

  if(all->on!=0){
    if(old->on){
      sx = old->x-OEKAKI_BOARD_POSX*8-centerling;
      sy = old->y-OEKAKI_BOARD_POSY*8-2;
    }
    px = all->x - OEKAKI_BOARD_POSX*8-centerling;
    py = all->y - OEKAKI_BOARD_POSY*8-2;
      
//    OS_Printf("sx=%d, sy=%d, px=%d, py=%d\n", sx,sy,px,py);

    // BG1�ʗpBMP�i���G�����摜�j�E�C���h�E�m��
    line = DrawPoint_to_Line(win, sign_brush[sign_mode][all->brush], px, py, &sx, &sy, 0, old->on, sign_mode, scale_side );
    
    flag = 1;
    
  }
  if(flag && draw){

    // �T�C���O���t�B�b�N�f�[�^��VRAM�ɓ]��
    GFL_BG_LoadCharacter( TRC_BG_SIGN, SignData, SIGN_BYTE_SIZE, SIGN_CGX );
    
    //OS_Printf("write board %d times\n",debug_count++);
    
  }
  
  // ����̍ŏI���W�̃o�b�N�A�b�v�����   
    Stock_OldTouch(all, old);
//  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    all->on = 0;    // ��x�`�悵������W���͎̂Ă�
//  }
  
  return line;
}

