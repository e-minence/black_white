//============================================================================================
/**
 * @file  trainer_card.c
 * @bfief �g���[�i�[�J�[�h
 * @author  Nozomu Saito
 * @date  05.11.15
 */
//============================================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "gamesystem/msgspeed.h"
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

#define TRC_BG_TRAINER    (GFL_BG_FRAME3_S)
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
  SEQ_OUT,
  SEQ_REV,
  SEQ_SIGN_CALL,
  SEQ_SIGN,
  SEQ_COVER,
};

typedef enum {
  TRC_KEY_REQ_NONE = 0,
  TRC_KEY_REQ_TP_BEAT,
  TRC_KEY_REQ_TP_BRUSH,
  TRC_KEY_REQ_REV_BUTTON,
  TRC_KEY_REQ_SIGN_CALL,
  TRC_KEY_REQ_END_BUTTON,
}TRC_KEY_REQ;

typedef enum {
  COVER_CLOSE = 0,
  COVER_OPEN = 1,
}COVER_STATE;

#if 0
enum {
  CASE_BUTTON,
  BADGE0,
  BADGE1,
  BADGE2,
  BADGE3,
  BADGE4,
  BADGE5,
  BADGE6,
  BADGE7,
};
#endif

enum {
  ANIME_NOTHING,
  ANIME_BUTTON_PUSH,
  ANIME_BUTTON_PULL
};

#define TP_BTN_PY (21*8)
#define TP_BTN_SY (3*8)
#define TP_BTN_SPX  (0*8)
#define TP_BTN_SSX  (14*8)
#define TP_BTN_BPX  (25*8)
#define TP_BTN_BSX  (8*8)
#define TP_CARD_PX  (0)
#define TP_CARD_PY  (0)
#define TP_CARD_SX  (255)
#define TP_CARD_SY  (21*8)

#if 0
typedef struct {
  const RECT_HIT_TBL *const CoverTpRect[2]; //0:�J�o�[���Ă�Ƃ� 1:�J�o�[�J���Ă�Ƃ�
}RECT_HIT_TBL_LIST;
#endif

//���j�I�����[���g���[�i�[�\���e�[�u��
static const int UniTrTable[UNION_TR_MAX] =
{
  //�j
  NARC_trc_union_trdp_schoolb256_NCGR,
  NARC_trc_union_trdp_mushi256_NCGR,
  NARC_trc_union_trdp_elitem256_NCGR,
  NARC_trc_union_trdp_heads256_NCGR,
  NARC_trc_union_trdp_iseki256_NCGR,
  NARC_trc_union_trdp_karate256_NCGR,
  NARC_trc_union_trdp_prince256_NCGR,
  NARC_trc_union_trdp_espm256_NCGR,
  //��
  NARC_trc_union_trdp_mini256_NCGR,
  NARC_trc_union_trdp_battleg256_NCGR,
  NARC_trc_union_trdp_sister256_NCGR,
  NARC_trc_union_trdp_elitew256_NCGR,
  NARC_trc_union_trdp_idol256_NCGR,
  NARC_trc_union_trdp_madam256_NCGR,
  NARC_trc_union_trdp_cowgirl256_NCGR,
  NARC_trc_union_trdp_princess256_NCGR,
};

static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_128_A,       // ���C��2D�G���W����BG
  GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g

  GX_VRAM_SUB_BG_128_C,     // �T�u2D�G���W����BG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // �T�u2D�G���W����BG�g���p���b�g

  GX_VRAM_OBJ_128_B,        // ���C��2D�G���W����OBJ
  GX_VRAM_OBJEXTPLTT_NONE,    // ���C��2D�G���W����OBJ�g���p���b�g

  GX_VRAM_SUB_OBJ_16_I,     // �T�u2D�G���W����OBJ
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
static int CheckInput(TR_CARD_WORK *wk);
static void SetCardPalette(TR_CARD_WORK *wk ,const u8 inCardRank, const u8 inPokeBookHold);
static void SetCasePalette(TR_CARD_WORK *wk ,const u8 inVersion);
static void SetUniTrainerPalette(TR_CARD_WORK *wk ,const u8 inTrainerNo);

static void VBlankFunc( GFL_TCB *tcb, void *work );
static void DispTrainer(TR_CARD_WORK *wk);
static void ClearTrainer(TR_CARD_WORK *wk);

static void ResetAffinePlane(void);
static const u8 GetBadgeLevel(const int inCount);

static void MakeSignData(const u8 *inRawData, u8 *outData);
static void TransSignData(const int inFrame, const u8 *inSignData);
static void UpdatePlayTime(TR_CARD_WORK *wk, const u8 inUpdateFlg);

static int SignCall( TR_CARD_WORK *wk );

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
  CONFIG* configSave; ///<�R���t�B�O�Z�[�u�f�[�^

  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );
  WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
  WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
  WIPE_ResetWndMask(WIPE_DISP_MAIN);
  WIPE_ResetWndMask(WIPE_DISP_SUB);

  GFL_UI_KEY_SetRepeatSpeed( 4, 8 );
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TR_CARD, 0x50000 );

  wk = GFL_PROC_AllocWork( proc, sizeof(TR_CARD_WORK), HEAPID_TR_CARD );
  GFL_STD_MemFill( wk, 0, sizeof(TR_CARD_WORK) );

  wk->heapId = HEAPID_TR_CARD;
  wk->ObjWork.heapId = HEAPID_TR_CARD;
  //���p���p�����[�^�擾
  wk->tcp = pwk;
  wk->TrCardData = wk->tcp->TrCardData;
  wk->key_mode = GFL_UI_CheckTouchOrKey();

  configSave = SaveData_GetConfig(SaveControl_GetPointer());
  wk->msg_spd = MSGSPEED_GetWait();
  wk->win_type = CONFIG_GetWindowType(configSave);

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
  //FullOpen!
#if DEB_ARI&0
  {
    u8 i;
    wk->isClear = TRUE;
    for(i=0;i<TR_BADGE_NUM_MAX;i++)wk->badge[i] = 1;
  }
#endif

  //�ʐM�����ǂ����H
//  if( CommIsConnect(u16 netID) ){
  if(GFL_NET_GetConnectNum() > 0){
    wk->isComm = TRUE;
  }

  GFL_BG_Init( wk->heapId );

  AllocStrBuf(wk);

  GFL_DISP_SetBank( &vramBank );
  SetTrCardBg();
  SetTrCardBgGraphic( wk );

//  InitTPSystem();           // �^�b�`�p�l���V�X�e��������
//  InitTPNoBuff(4);

  //���֘A������
#if TRC_USE_SND
  Snd_DataSetByScene( SND_SCENE_SUB_TRCARD, 0, 0 ); //�T�E���h�f�[�^���[�h(BGM���p��)
//  Snd_BadgeWorkInit( &wk->SndBadgeWork );
  Snd_SePlay( SND_TRCARD_CALL );    //�Ăяo����
#endif

  InitTRCardCellActor( &wk->ObjWork , &vramBank );

  SetTrCardActor( &wk->ObjWork, wk->badge ,wk->isClear);
  SetTrCardActorSub( &wk->ObjWork);

  //Bmp�E�B���h�E������
  TRCBmp_AddTrCardBmp( wk );

  //�R�����`��
  TRCBmp_WriteSec(wk,wk->win[TRC_BMPWIN_PLAY_TIME], TRUE, wk->SecBuf);

  if(wk->tcp->value){
    wk->is_back = TRUE;
  }else{
    wk->is_back = FALSE;    //�\�ʂ���X�^�[�g
  }
  wk->tcp->value = FALSE;
  wk->IsOpen = COVER_CLOSE;     //�P�[�X�͕�����Ԃ���X�^�[�g

//  wk->touch = RECT_HIT_NONE;      //�^�b�`�p�l���͉�����Ă��Ȃ�

  wk->ButtonPushFlg = FALSE;      //�{�^��������ĂȂ�
  wk->AnimeType = ANIME_NOTHING;    //�{�^���A�j������

  //�g�k�ʂ����Z�b�g
  ResetAffinePlane();

  //�J�[�h�̏�����ԕ\��
  CardDataDraw(wk);

  //�߂�{�^���`��
  TRCBmp_WriteBackButton(wk,FALSE);

  wk->vblankFunc = GFUser_VIntr_CreateTCB(VBlankFunc, wk , 1);  // VBlank�Z�b�g
  wk->vblankTcblSys = GFL_TCBL_Init( wk->heapId , wk->heapId , 3 , 0 );
  //�ʐM�A�C�R���Z�b�g
  // ��M���x�A�C�R����ʐM�ڑ����Ȃ�\�����邵�A
  // ���j�I���̂悤�ɒʐM��H�͓��삵�Ă��邪�ڑ��͂��Ă��Ȃ���ԂȂ�o���Ȃ�
  //WirelessIconEasyUnion();

  //BGM�����ڂ�
  //Snd_BgmFadeOut( BGM_VOL_TR_CASE, BGM_FADE_TR_CASE_TIME );
#if TRC_USE_SND
  Snd_PlayerSetPlayerVolume( PLAYER_FIELD, SND_PLAYER_VOL_TR_CASE );
#endif

  WIPE_SYS_Start( WIPE_PATTERN_FSAM, WIPE_TYPE_SHUTTERIN_DOWN,
          WIPE_TYPE_SHUTTERIN_DOWN, WIPE_FADE_BLACK,
          WIPE_DEF_DIV, WIPE_DEF_SYNC, wk->heapId );

  return GFL_PROC_RES_FINISH;
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
      req = CheckInput(wk);

#if 0
      if (wk->TrCardData->BrushValid){
        ClearScruchSnd(&wk->ScruchSnd);
      }
#endif
      if (req == TRC_KEY_REQ_REV_BUTTON){
        //�J�[�h�Ђ�����Ԃ�
        wk->sub_seq = 0;  //�T�u�V�[�P���X������
        *seq = SEQ_REV; //�J�[�h��]
      }else if(req == TRC_KEY_REQ_SIGN_CALL){
        SetSActDrawSt(&wk->ObjWork,ACTS_BTN_SIGN,ANMS_SIGN_ON,TRUE);
        SetEffActDrawSt(&wk->ObjWork, ACTS_BTN_SIGN ,TRUE);
        wk->sub_seq = 0;
        *seq = SEQ_SIGN_CALL;
      }else if (req == TRC_KEY_REQ_END_BUTTON){
        //�I��
        SetSActDrawSt(&wk->ObjWork,ACTS_BTN_BACK,ANMS_BACK_ON,TRUE);
        SetEffActDrawSt(&wk->ObjWork, ACTS_BTN_BACK ,TRUE);
        WIPE_SYS_Start( WIPE_PATTERN_FMAS, WIPE_TYPE_SHUTTEROUT_UP,
                WIPE_TYPE_SHUTTEROUT_UP, WIPE_FADE_BLACK,
                WIPE_DEF_DIV, WIPE_DEF_SYNC, wk->heapId );
        *seq = SEQ_OUT;
      }
      UpdatePlayTime(wk, wk->TrCardData->TimeUpdate);
    }
    break;

  case SEQ_OUT:
    if( WIPE_SYS_EndCheck() ){
      return GFL_PROC_RES_FINISH;
    }
    break;

  case SEQ_REV: //���o�[�X����
    if ( CardRev(wk) ){
      *seq = SEQ_MAIN;
    }
    break;
  case SEQ_SIGN_CALL: //�T�C���������Ăяo�����H
    {
      int ret;
      ret = SignCall(wk);
      switch(ret){
      case 1:
        *seq = SEQ_MAIN;
        break;
      case 2:
        *seq = SEQ_SIGN;
        break;
      }
    }
    break;
  case SEQ_SIGN:  //�T�C���������Ă�
    WIPE_SYS_Start( WIPE_PATTERN_FMAS, WIPE_TYPE_SHUTTEROUT_UP,
            WIPE_TYPE_SHUTTEROUT_UP, WIPE_FADE_BLACK,
            WIPE_DEF_DIV, WIPE_DEF_SYNC, wk->heapId );
    wk->tcp->value = TRUE;
    *seq = SEQ_OUT;
    break;
  }
  if(++wk->scrl_ct >= 128){
    wk->scrl_ct = 0;
  }
  GFL_CLACT_SYS_Main();
  if( wk->vblankTcblSys != NULL )
    GFL_TCBL_Main( wk->vblankTcblSys );

#if DEB_ARI
  {
    int x,y;
    u8 bgPlane = TRC_BG_FONT;
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X )
      bgPlane = TRC_BG_MSG;
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L )
    {
      for( y=0;y<24;y++ )
      {
        for( x=0;x<32;x++ )
        {
          GFL_BG_FillScreen( bgPlane , x+y*32 , x,y,1,1 ,GFL_BG_SCRWRT_PALIN );
        }
      }
      GFL_BG_LoadScreenV_Req( bgPlane);
    }
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R )
    {
      for( y=0;y<24;y++ )
      {
        for( x=0;x<32;x++ )
        {
          GFL_BG_FillScreen( bgPlane , x+y*32+32*24 , x,y,1,1 ,GFL_BG_SCRWRT_PALIN );
        }
      }
      GFL_BG_LoadScreenV_Req( bgPlane);
    }
  }

#endif
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

  //�g�p�����g�k�ʂ����ɖ߂�
  ResetAffinePlane();

  FreeStrBuf( wk );         //��������

  RereaseCellObject(&wk->ObjWork);    //2D�I�u�W�F�N�g�֘A�̈���

  GFL_HEAP_FreeMemory( wk->TrArcData ); //�g���[�i�[�L�����A�[�J�C�u�f�[�^���

  GFL_HEAP_FreeMemory( wk->pScrnBCase );  //�o�b�W�P�[�X�N���[�����
  GFL_HEAP_FreeMemory( wk->TrScrnArcData );//�g���[�i�[�X�N���[�����

  TRCBmp_ExitTrCardBmpWin( wk );      // BMP�E�B���h�E�J��
  TrCardBgExit();     // BGL�폜
//  StopTP();             // �^�b�`�p�l���I��
  GFL_TCB_DeleteTask( wk->vblankFunc );   // VBlank�Z�b�g
  GFL_TCBL_Exit( wk->vblankTcblSys );
  //���샂�[�h�����߂�
  GFL_UI_SetTouchOrKey(wk->key_mode);

  GFL_PROC_FreeWork( proc );        // ���[�N�J��

  GFL_HEAP_DeleteHeap( HEAPID_TR_CARD );

  //BGM�����̉��ʂɖ߂�
  //Snd_BgmFadeIn( BGM_VOL_MAX, BGM_FADE_TR_CASE_TIME, BGM_FADEIN_START_VOL_NOW );
#if TRC_USE_SND
  Snd_PlayerSetPlayerVolume( PLAYER_FIELD, SND_PLAYER_DEFAULT_VOL );
#endif

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

//--------------------------------------------------------------------------------------------
/**
 * �J�[�h�p���b�g�ݒ�
 *
 * @param inCardRank    �J�[�h�����N
 * @param inPokeBookHold  �}�ӏ����t���O
 *
 * @return  �Ȃ�
 */
//--------------------------------------------------------------------------------------------
static void SetCardPalette(TR_CARD_WORK *wk ,const u8 inCardRank, const u8 inPokeBookHold)
{
  {
    void *buf;
    NNSG2dPaletteData *dat;

    if (inPokeBookHold){
      switch(inCardRank){
      case 0:
        buf = GFL_ARC_UTIL_LoadPalette(
          ARCID_TRAINERCARD, NARC_trainer_case_card_0_NCLR, &dat, wk->heapId );
        break;
      case 1:
        buf = GFL_ARC_UTIL_LoadPalette(
          ARCID_TRAINERCARD, NARC_trainer_case_card_1_NCLR, &dat, wk->heapId );
        break;
      case 2:
        buf = GFL_ARC_UTIL_LoadPalette(
          ARCID_TRAINERCARD, NARC_trainer_case_card_2_NCLR, &dat, wk->heapId );
        break;
      case 3:
        buf = GFL_ARC_UTIL_LoadPalette(
          ARCID_TRAINERCARD, NARC_trainer_case_card_3_NCLR, &dat, wk->heapId );
        break;
      case 4:
        buf = GFL_ARC_UTIL_LoadPalette(
          ARCID_TRAINERCARD, NARC_trainer_case_card_4_NCLR, &dat, wk->heapId );
        break;
      case 5:
        buf = GFL_ARC_UTIL_LoadPalette(
          ARCID_TRAINERCARD, NARC_trainer_case_card_5_NCLR, &dat, wk->heapId );
        break;
      }
    }else{
      buf = GFL_ARC_UTIL_LoadPalette(
          ARCID_TRAINERCARD, NARC_trainer_case_card_6_NCLR, &dat, wk->heapId );
    }

    {
      u16 * adr;
      DC_FlushRange( dat->pRawData, 2*16*16 );  //16�{�����t���b�V��
      adr = dat->pRawData;
      //16�p���b�g�P�Ԗڂ���8�{�����[�h
      GXS_LoadBGPltt( &adr[16], 2*16, 2*16*8 );
      //f�Ԗڂ̃p���b�g�P�{�����[�h
      GXS_LoadBGPltt( &adr[16*15], 2*16*15, 2*16 );
      GFL_HEAP_FreeMemory(buf);
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * �P�[�X�p���b�g�Z�b�g
 *
 * @param inVersion �o�[�W����
 *
 * @return  �Ȃ�
 */
//--------------------------------------------------------------------------------------------
static void SetCasePalette(TR_CARD_WORK *wk ,const u8 inVersion)
{
  void *buf;
  NNSG2dPaletteData *dat;
/*
  switch(inVersion){
  case VERSION_DIAMOND:   //�_�C��
    buf = GFL_ARC_UTIL_LoadPalette(
        ARCID_TRAINERCARD, NARC_trainer_case_card_case_d_NCLR, &dat, wk->heapId );
    break;
  case VERSION_PEARL:   //�p�[��
    buf = GFL_ARC_UTIL_LoadPalette(
        ARCID_TRAINERCARD, NARC_trainer_case_card_case_p_NCLR, &dat, wk->heapId );
    break;
  case VERSION_GOLD:
  case VERSION_SILVER:
    buf = GFL_ARC_UTIL_LoadPalette(
        ARCID_TRAINERCARD, NARC_trainer_case_card_case_g_NCLR, &dat, wk->heapId );
    break;
  case VERSION_PLATINUM:
  default:  //�ʃo�[�W����
    buf = GFL_ARC_UTIL_LoadPalette(
        ARCID_TRAINERCARD, NARC_trainer_case_card_case_x_NCLR, &dat, wk->heapId );
    break;
  }
*/
  //�Ƃ肠����GOLD�̓���ɓ���
  buf = GFL_ARC_UTIL_LoadPalette(
      ARCID_TRAINERCARD, NARC_trainer_case_card_case_g_NCLR, &dat, wk->heapId );


  DC_FlushRange( dat->pRawData, 2*16 );
  GX_LoadBGPltt( dat->pRawData, CASE_BD_PAL*32, 2*16 );
  GXS_LoadBGPltt( dat->pRawData, CASE_BD_PAL*32, 2*16 );
  GFL_HEAP_FreeMemory(buf);
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
  void *buf;
  u8 *addr;
  NNSG2dPaletteData *dat;
  buf = GFL_ARC_UTIL_LoadPalette(
      ARCID_TRC_UNION, NARC_trc_union_trdp_union_card_NCLR, &dat, wk->heapId );

  addr = (u8*)(dat->pRawData);

  DC_FlushRange(&addr[2*16*inTrainerNo] , 2*16 );
  GXS_LoadBGPltt( &addr[2*16*inTrainerNo], 2*16*UNI_TRAINER_PLTT_NO, 2*16 );
  GFL_HEAP_FreeMemory(buf);
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

  { // FONT (BMP)
    GFL_BG_BGCNT_HEADER  ExAffineBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256, //TODO 256����ς��ĕ��C�H
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x10000, 0x10000, GX_BG_EXTPLTT_01,
      1, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( TRC_BG_FONT, &ExAffineBgCntDat, GFL_BG_MODE_256X16 );
    GFL_BG_ClearFrame( TRC_BG_FONT );
    GFL_BG_SetVisible( TRC_BG_FONT, VISIBLE_ON );
  }

  { // BG (CASE CHAR,MSG_CHAR)
    GFL_BG_BGCNT_HEADER  TextBgCntDat[] = {
     {  0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000, 0x6000, GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE},
     {  0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x00000, 0x6000, GX_BG_EXTPLTT_01,
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
    GFL_BG_SetVisible( TRC_BG_BADGE_CASE, VISIBLE_ON );
  }

  { // BG (BADGE_BACK CHAR)
    GFL_BG_BGCNT_HEADER  TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x00000, 0x8000, GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( TRC_BG_BADGE_BACK, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( TRC_BG_BADGE_BACK );
    GFL_BG_SetVisible( TRC_BG_BADGE_BACK, VISIBLE_ON );
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
  {
  /*
    void *buf;
    NNSG2dPaletteData *dat;
    buf = ArcUtil_PalDataGet(
        ARCID_TRAINERCARD, NARC_trainer_case_card_0_NCLR, &dat, wk->heapId );
    DC_FlushRange( dat->pRawData, 2*16*16 );
    GXS_LoadBGPltt( dat->pRawData, 0, 2*16*16 );
    sys_FreeMemoryEz(buf);
  */
    GFL_ARC_UTIL_TransVramPalette( ARCID_TRAINERCARD, NARC_trainer_case_card_0_NCLR,
          PALTYPE_SUB_BG , 0 , 2*16*16 ,wk->heapId );
  }

  // CARD PALETTE
  SetCardPalette(wk,wk->TrCardData->CardRank, wk->TrCardData->PokeBookFlg);

  // UNSER_CASE_COVER PALETTE(UNDER_DISPLAY)
  {
  /*
    void *buf;
    NNSG2dPaletteData *dat;
    buf = ArcUtil_PalDataGet(
        ARCID_TRAINERCARD, NARC_trainer_case_card_case_g_NCLR, &dat, wk->heapId );
    DC_FlushRange( dat->pRawData, 16*2*16 );
    GX_LoadBGPltt( dat->pRawData, 0, 16*2*16 );
    sys_FreeMemoryEz(buf);
  */
    GFL_ARC_UTIL_TransVramPalette( ARCID_TRAINERCARD, NARC_trainer_case_card_case_g_NCLR,
          PALTYPE_MAIN_BG , 0 , 16*2*16 ,wk->heapId );
  }
  // CASE PALETTE
  SetCasePalette(wk,wk->TrCardData->Version);

  //TRAINER
  if (wk->TrCardData->UnionTrNo == UNION_TR_NONE){
    {
      /*
      BOOL rc;
      //�A�[�J�C�u�f�[�^�擾
      wk->TrArcData = ArcUtil_Load( ARCID_TRAINERCARD, NARC_trainer_case_card_trainer_NCGR,
                      FALSE, wk->heapId, ALLOC_TOP );
      GF_ASSERT(wk->TrArcData!=NULL);
      rc = NNS_G2dGetUnpackedBGCharacterData( wk->TrArcData, &wk->TrCharData);
      GF_ASSERT(rc);
      */
      wk->TrArcData = GFL_ARC_UTIL_LoadBGCharacter( ARCID_TRAINERCARD, NARC_trainer_case_card_trainer_NCGR,
                    FALSE, &wk->TrCharData, wk->heapId);
    }

    if (wk->TrCardData->TrSex == PM_MALE){
      //�j
      wk->TrScrnArcData = GFL_ARC_UTIL_LoadScreen(ARCID_TRAINERCARD, NARC_trainer_case_card_trainer01_NSCR,
                          0, &wk->ScrnData, wk->heapId);
    }else{
      //��
      wk->TrScrnArcData = GFL_ARC_UTIL_LoadScreen(ARCID_TRAINERCARD, NARC_trainer_case_card_trainer02_NSCR,
                          0, &wk->ScrnData, wk->heapId);
    }
  }else{
    //���j�I�����[���ő��̐l�̃f�[�^�����鎞
    {
      /*
      BOOL rc;
      //�A�[�J�C�u�f�[�^�擾
      wk->TrArcData = ArcUtil_Load( ARCID_TRAINERCARD, UniTrTable[wk->TrCardData->UnionTrNo],
                      FALSE, wk->heapId, ALLOC_TOP );
      GF_ASSERT(wk->TrArcData!=NULL);
      rc = NNS_G2dGetUnpackedBGCharacterData( wk->TrArcData, &wk->TrCharData);
      GF_ASSERT(rc);
      */
      wk->TrArcData = GFL_ARC_UTIL_LoadBGCharacter( ARCID_TRC_UNION, UniTrTable[wk->TrCardData->UnionTrNo],
                    FALSE, &wk->TrCharData, wk->heapId);

      wk->TrScrnArcData = GFL_ARC_UTIL_LoadScreen(ARCID_TRC_UNION, NARC_trc_union_card_test256_NSCR,
                          0, &wk->ScrnData, wk->heapId);
      //�g���[�i�[�p���b�g�ύX
      SetUniTrainerPalette(wk,wk->TrCardData->UnionTrNo);
    }
  }
  DispTrainer(wk);

  //CARD
  GFL_ARC_UTIL_TransVramBgCharacter(
      ARCID_TRAINERCARD, NARC_trainer_case_card_NCGR, TRC_BG_CARD, 0, 0, 0, wk->heapId );
  GFL_ARC_UTIL_TransVramScreen(
      ARCID_TRAINERCARD, NARC_trainer_case_card_faca_NSCR, TRC_BG_CARD, 0, 0, 0, wk->heapId );
  //CASE
  GFL_ARC_UTIL_TransVramBgCharacter(
      ARCID_TRAINERCARD, NARC_trainer_case_card_case_NCGR, TRC_BG_BACK, 0, 0, 0, wk->heapId );
  GFL_ARC_UTIL_TransVramScreen(
      ARCID_TRAINERCARD, NARC_trainer_case_card_case_NSCR, TRC_BG_BACK, 0, 0, 0, wk->heapId );

  //BADGE_CASE & BADGE_BACK
  GFL_ARC_UTIL_TransVramBgCharacter( ARCID_TRAINERCARD, NARC_trainer_case_card_case2_NCGR,
    TRC_BG_BADGE_CASE, 0, 0, 0, wk->heapId );

  //�X�N���[���f�[�^�擾
  wk->pScrnBCase = GFL_ARC_UTIL_LoadScreen(ARCID_TRAINERCARD, NARC_trainer_case_card_case4_NSCR,
              0, &wk->pSBCase, wk->heapId);

  if(wk->isClear){
    int i;
    GFL_ARC_UTIL_TransVramScreen( ARCID_TRAINERCARD, NARC_trainer_case_card_case3_NSCR,
      TRC_BG_BADGE_CASE, 0, 0, 0, wk->heapId );

    for(i = 0;i < 8;i++){
      if(wk->badge[i+8] == 0){
        GFL_BG_WriteScreenExpand(TRC_BG_BADGE_CASE,
          LEADER_ICON_X+(LEADER_ICON_OX*(i%4)),LEADER_ICON_KY+(LEADER_ICON_OY*(i/4)),
          LEADER_ICON_SX,LEADER_ICON_SY,wk->pSBCase->rawData,0,LEADER_ICON_PY,
          wk->pSBCase->screenWidth/8,wk->pSBCase->screenHeight/8);
      }
    }
  }else{
    int i;
    GFL_ARC_UTIL_TransVramScreen( ARCID_TRAINERCARD, NARC_trainer_case_card_case2_NSCR,
      TRC_BG_BADGE_CASE, 0, 0, 0, wk->heapId );

    for(i = 0;i < 8;i++){
      if(wk->badge[i] == 0){
        GFL_BG_WriteScreenExpand(TRC_BG_BADGE_CASE,
          LEADER_ICON_X+(LEADER_ICON_OX*(i%4)),LEADER_ICON_JY+(LEADER_ICON_OY*(i/4)),
          LEADER_ICON_SX,LEADER_ICON_SY,wk->pSBCase->rawData,0,LEADER_ICON_PY,
          wk->pSBCase->screenWidth/8,wk->pSBCase->screenHeight/8);
      }
    }
  }
  GFL_ARC_UTIL_TransVramScreen( ARCID_TRAINERCARD, NARC_trainer_case_card_case2bg_NSCR,
    TRC_BG_BADGE_BACK, 0, 0, 0, wk->heapId );

  //�a������}�[�N�\��
  if(!wk->isClear && wk->TrCardData->BadgeFlag >= 0x00FF){
    GFL_BG_WriteScreenExpand(TRC_BG_BADGE_CASE,
      0,7,7,9,wk->pSBCase->rawData,0,0,
      wk->pSBCase->screenWidth/8,wk->pSBCase->screenHeight/8);
  }else if(wk->TrCardData->BadgeFlag == 0xFFFF){
    GFL_BG_WriteScreenExpand(TRC_BG_BADGE_CASE,
      0,14,7,9,wk->pSBCase->rawData,7,0,
      wk->pSBCase->screenWidth/8,wk->pSBCase->screenHeight/8);
  }
  GFL_BG_LoadScreenReq(TRC_BG_BADGE_CASE);
  //�T�C���W�J
  MakeSignData(wk->TrCardData->SignRawData, wk->TrSignData);

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

//--------------------------------------------------------------------------------------------
/**
 *  @brief  wk->is_back�ϐ��̏�Ԃɂ���āA�J�[�h�̕\or����View���N���A����������
 */
//--------------------------------------------------------------------------------------------
static void CardDesignDraw(TR_CARD_WORK* wk)
{
  if (wk->is_back == FALSE){
    GFL_ARC_UTIL_TransVramScreen(
        ARCID_TRAINERCARD, NARC_trainer_case_card_faca_NSCR, TRC_BG_CARD, 0, 0, 0, wk->heapId );
    //���ʂ�����
    TRCBmp_NonDispWinInfo(wk->win, TRC_BMPWIN_CLEAR_TIME, TRC_BMPWIN_TRADE_INFO);
    //�X�N���[���N���A
    GFL_BG_ClearScreen( TRC_BG_TRAINER );
  }else{
    GFL_ARC_UTIL_TransVramScreen(
        ARCID_TRAINERCARD, NARC_trainer_case_card_back_NSCR, TRC_BG_CARD, 0, 0, 0, wk->heapId );
    //�\�ʂ�����
    TRCBmp_NonDispWinInfo(wk->win, TRC_BMPWIN_TR_ID, TRC_BMPWIN_START_TIME);
    //�g���[�i�[����
    ClearTrainer(wk);
    //�X�N���[���N���A
    GFL_BG_ClearScreen( TRC_BG_SIGN );
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

  //�߂�{�^���\��
  SetSActDrawSt( &wk->ObjWork,ACTS_BTN_BACK,ANMS_BACK_OFF ,TRUE);

  if (wk->is_back == FALSE){  //�\
    //�g���[�i�[�\��
    DispTrainer(wk);
    TRCBmp_WriteExpWin(wk,EXPWIN_PAT_TOUCH);
  }else{  //���ʕ\��
    TransSignData(TRC_BG_SIGN, wk->TrSignData);
    TRCBmp_WriteExpWin(wk,EXPWIN_PAT_SIGN);
  }
  TRCBmp_WriteTrWinInfo(wk, wk->win, wk->TrCardData );
  TRCBmp_WriteTrWinInfoRev(wk, wk->win, wk->TrCardData );
  TRCBmp_TransTrWinInfo(wk,wk->win);
}

//--------------------------------------------------------------------------------------------
/**
 * �T�C���A�v�����ĂԂ��m�F
 *
 * @param wk    ��ʂ̃��[�N
 *
 * @return  BOOL  TRUE:�I�� FALSE�F������
 */
//--------------------------------------------------------------------------------------------
static int SignCall( TR_CARD_WORK *wk )
{
  int ret = 0;

  switch(wk->sub_seq){
  case 0: //�T�C���������܂����H
    TRCBmp_SignDrawMsgPut(wk,0);
    wk->sub_seq++;
    break;
  case 1:
    //�`��I���҂�
//    if( GF_MSG_PrintEndCheck( wk->msgIdx ))
    if( PRINTSYS_PrintStreamGetState( wk->printHandle ) != PRINTSTREAM_STATE_DONE )
    {
      return 0;
    }
    PRINTSYS_PrintStreamDelete( wk->printHandle );
    TRCBmp_SignDrawYesNoCall(wk,0);
    wk->sub_seq++;
    break;
  case 2: //�I��҂�
    ret = TRCBmp_SignDrawYesNoWait(wk,0);
    if(ret < 0){
      break;
    }
    if(ret == 0){
      return 1;
    }
    if(wk->TrCardData->MySignValid){
      TRCBmp_SignDrawMsgPut(wk,1);
    }else{
      return 2; //�T�C��������
    }
    wk->sub_seq++;
    break;
  case 3:
//    if( GF_MSG_PrintEndCheck( wk->msgIdx ))
    if( PRINTSYS_PrintStreamGetState( wk->printHandle ) != PRINTSTREAM_STATE_DONE )
    {
      return 0;
    }
    PRINTSYS_PrintStreamDelete( wk->printHandle );
    TRCBmp_SignDrawYesNoCall(wk,1);
    wk->sub_seq++;
    break;
  case 4: //���̃f�[�^�������Ă������ł����H
    ret = TRCBmp_SignDrawYesNoWait(wk,1);
    switch(ret){
    case 0:
      return 1;
    case 1:
      return 2;
    }
    break;
  }
  return 0;
}

#define ROTATE_SPEED (0x300)    //90�x�̃J�E���g�A�b�v
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
#if TRC_USE_SND
    Snd_SePlay( SND_TRCARD_REV );   //�Ђ�����Ԃ���
#endif
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
    wk->sub_seq++;
    break;
  case 3:
    if (wk->is_back == FALSE){
      //�g���[�i�[�\��
      DispTrainer(wk);
      TRCBmp_WriteExpWin(wk,EXPWIN_PAT_TOUCH);
    }else{
      TransSignData(TRC_BG_SIGN, wk->TrSignData);
      if(!wk->isComm){  //�ʐM���͏������Ȃ�
        TRCBmp_WriteExpWin(wk,EXPWIN_PAT_SIGN);
      }
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
    }
    break;
  }
  //�A�t�B���ϊ����s���N�G�X�g
  wk->aff_req = TRUE;
  return rc;
}

static void CardRevAffineSet(TR_CARD_WORK* wk)
{
  MtxFx22 mtx;

  mtx._00 = FX_Inv(wk->CardScaleX);
  mtx._01 = 0;
  mtx._10 = 0;
  mtx._11 = FX_Inv(wk->CardScaleY);
//  SVC_WaitVBlankIntr();          // Waiting the end of VBlank interrup
  G2S_SetBG2Affine(&mtx,          // a matrix for rotation and scaling
          128, 96,      // the center of rotation
          0, 0           // the reference point before rotation and scaling applied
      );
  G2S_SetBG3Affine(&mtx,          // a matrix for rotation and scaling
          128, 96,      // the center of rotation
          0, 0           // the reference point before rotation and scaling applied
      );
  wk->aff_req = FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �L�[����
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

static int CheckKey(TR_CARD_WORK* wk)
{
  const int keyTrg = GFL_UI_KEY_GetTrg();
  if ( keyTrg & PAD_BUTTON_DECIDE )
  {
    if(wk->is_back && (!wk->isComm))
    {
      //�T�C��������
#if TRC_USE_SND
      Snd_SePlay( SND_TRCARD_SIGN );
#endif
      return TRC_KEY_REQ_SIGN_CALL;
    }
  }
  else if( keyTrg & PAD_BUTTON_CANCEL )
  {
#if TRC_USE_SND
    Snd_SePlay( SND_TRCARD_END );   //�I����
#endif
    return TRC_KEY_REQ_END_BUTTON;
  }

  if(keyTrg & (PAD_KEY_LEFT|PAD_KEY_RIGHT))
  {
    return TRC_KEY_REQ_REV_BUTTON;
  }
  return TRC_KEY_REQ_NONE;
}
static int CheckTouch(TR_CARD_WORK* wk)
{
  int ret;
  u16 pat;
  static const GFL_UI_TP_HITTBL Btn_TpRect[] = {
//    {0,191,0,255}, ty,by,lx,rx
    {TP_BTN_PY,TP_BTN_PY+TP_BTN_SY,TP_BTN_BPX,TP_BTN_BPX+TP_BTN_BSX},
    {TP_BTN_PY,TP_BTN_PY+TP_BTN_SY,TP_BTN_SPX,TP_BTN_SPX+TP_BTN_SSX},
    {TP_CARD_PY,TP_CARD_PY+TP_CARD_SY,TP_CARD_PX,TP_CARD_PX+TP_CARD_SX},
    {GFL_UI_TP_HIT_END,0,0,0}
  };

  const int hitNo = GFL_UI_TP_HitCont( Btn_TpRect );
  if(GFL_UI_TP_GetTrg() == 0){
    return TRC_KEY_REQ_NONE;
  }

  //�߂�{�^�����o
  if( hitNo == 0 ){
#if TRC_USE_SND
    Snd_SePlay( SND_TRCARD_END );   //�I����
#endif
    return TRC_KEY_REQ_END_BUTTON;
  }

  //�T�C���������{�^�����o
  if(wk->is_back && (!wk->isComm)){
    if( hitNo == 1 ){
#if TRC_USE_SND
      Snd_SePlay( SND_TRCARD_SIGN );
#endif
      return TRC_KEY_REQ_SIGN_CALL;
    }
  }

  //���݂̍��W���擾
  if( hitNo == 2 ){
    GFL_UI_TP_GetPointCont( &wk->tp_x , &wk->tp_y );
    SetEffActDrawSt(&wk->ObjWork,ACTS_BTN_EFF,TRUE);
    return TRC_KEY_REQ_REV_BUTTON;
  }
  return TRC_KEY_REQ_NONE;
}
static int CheckInput(TR_CARD_WORK *wk)
{
  int key_req;
  BOOL tp_key_flg;
  tp_key_flg = FALSE;
  key_req = TRC_KEY_REQ_NONE;

//  wk->touch = RECT_HIT_NONE;

  //�L�[�ƃ^�b�`�̐ؑ֊Ď�
  CheckKTStatus(wk);

  if(wk->key_mode == GFL_APP_KTST_TOUCH){
    return CheckTouch(wk);
  }else{
    return CheckKey(wk);
  }
  return key_req;
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

  //�w�i�X�N���[��
  scr = -(wk->scrl_ct);
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

#if 0
  if( GFL_BG_ScreenAdrsGet( TRC_BG_TRAINER ) != NULL ){
    GFL_BG_ScreenBufSet( TRC_BG_TRAINER, wk->ScrnData->rawData, transSize );
  }

  GFL_BG_LoadScreen( TRC_BG_TRAINER, wk->ScrnData->rawData, transSize, 0 );
#else
  GFL_BG_WriteScreenExpand(TRC_BG_TRAINER,
    21,7,10,11,wk->ScrnData->rawData,21,7,
    wk->ScrnData->screenWidth/8,wk->ScrnData->screenHeight/8);
  GFL_BG_LoadScreenReq( TRC_BG_TRAINER );
#endif

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
 * �o�b�W���x���擾
 *
 * @param inCount     ������
 *
 * @return  u8        ���x��
 */
//--------------------------------------------------------------------------------------------
static const u8 GetBadgeLevel(const int inCount)
{
  u8 lv;
  if ((0<=inCount)&&(inCount<COUNT_LV0)){
    lv = 0;
  }else if(inCount<COUNT_LV1){
    lv =1;
  }else if (inCount<COUNT_LV2){
    lv = 2;
  }else if (inCount<COUNT_LV3){
    lv = 3;
  }else if (inCount<COUNT_LV4){
    lv = 4;
  }else{
    GF_ASSERT(0&&"BadgeLevelOver");
    lv = 0;
  }
  return lv;
}

//--------------------------------------------------------------------------------------------
/**
 * �T�C���f�[�^�쐬
 *
 * @param *inRawData  �r�b�g�P�ʂŎ����Ă���T�C���f�[�^
 * @param *outData  �T�C���f�[�^�i�[�ꏊ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void MakeSignData(const u8 *inRawData, u8 *outData)
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
static void TransSignData(const int inFrame, const u8 *inSignData)
{
  u16 *buf;
  u8 x,y;
  u16 count;

  GFL_BG_LoadCharacter( inFrame, inSignData, SIGN_BYTE_SIZE, SIGN_CGX );

  buf   = (u16 *)GFL_BG_GetScreenBufferAdrs( inFrame );

  count = 0;
  for( y=0; y<SIGN_SY; y++ ){
        for( x=0; x<SIGN_SX; x++ ){
            buf[ (SIGN_PY+y)*TRC_SCREEN_WIDTH + x+SIGN_PX ] = (SIGN_CGX+count);
      count++;
        }
    }
  GFL_BG_LoadScreenV_Req( inFrame );
}

//--------------------------------------------------------------------------------------------
/**
 * �v���C���ԍX�V
 *
 * @param wk        ��ʂ̃��[�N
 * @param inUpdateFlg   ���ԍX�V�t���O
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void UpdatePlayTime(TR_CARD_WORK *wk, const u8 inUpdateFlg)
{
  //�X�V�t���O�������Ă��邩���`�F�b�N
  if (!inUpdateFlg){
    return;
  }

  if (!wk->is_back){  //�\�ʂ̏ꍇ�̂ݕ`��
    if (wk->SecCount == 0){
      TRCBmp_WritePlayTime(wk,wk->win, wk->TrCardData, wk->PlayTimeBuf);
      //�R�����`��
      TRCBmp_WriteSec(wk,wk->win[TRC_BMPWIN_PLAY_TIME], TRUE, wk->SecBuf);
    }else if(wk->SecCount == TRC_FRAME_RATE/2){
      //�R��������
      TRCBmp_WriteSec(wk,wk->win[TRC_BMPWIN_PLAY_TIME], FALSE, wk->SecBuf);
    }
  }
  //�J�E���g�A�b�v
  wk->SecCount++;   //  1/TRC_FRAME_RATE�Ȃ̂�
  if( wk->SecCount >= TRC_FRAME_RATE )
    wk->SecCount -= TRC_FRAME_RATE;
}

