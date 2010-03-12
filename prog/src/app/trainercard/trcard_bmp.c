//============================================================================================
/**
 * @file  trcard_bmp.c
 * @bfief �g���[�i�[�J�[�h���BMP�֘A
 * @author  Nozomu Saito
 * @date  05.11.15
 */
//============================================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/str_tool.h"

#include "gamesystem\msgspeed.h"
#include "savedata/playtime.h"
#include "trcard_bmp.h"
#include "trcard_cgx_def.h"

#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"
#include "msg/msg_trainercard.h"
#include "msg/msg_trtype.h"


#define FONT_BG (GFL_BG_FRAME3_S)
//#define FONT_BG (GFL_BG_FRAME0_S)
#define MSG_BG  (GFL_BG_FRAME0_S)

enum{
  EXPMSG_BACK,
  EXPMSG_TOUCH,
  EXPMSG_SIGN,
};

#define HOUR_DISP_MAX (999)


#define BMP_CSX_TYPE0 (12)
#define BMP_CSX_TYPE1 (13)
#define BMP_CSX_TYPE2 (17)
#define BMP_CSX_TYPE3 (27)
#define BMP_CSX_TYPE4 (28)

////////////////////////////////////////////////////////////////
//�\��
////////////////////////////////////////////////////////////////
//���O
#define WIN_NAME_PX (3)
#define WIN_NAME_PY (4)
#define WIN_NAME_SX (BMP_CSX_TYPE1)
#define WIN_NAME_SY (2)

//�g���[�i�[�^�C�v
#define WIN_TRTYPE_PX  (3)
#define WIN_TRTYPE_PY  (6)
#define WIN_TRTYPE_SX  (BMP_CSX_TYPE2)
#define WIN_TRTYPE_SY  (2)

//��������
#define WIN_PERSONALITY_PX  (3)
#define WIN_PERSONALITY_PY  (8)
#define WIN_PERSONALITY_SX  (BMP_CSX_TYPE2)
#define WIN_PERSONALITY_SY  (2)

//������            
#define WIN_BOOK_PX ( 3)
#define WIN_BOOK_PY (10)
#define WIN_BOOK_SX (BMP_CSX_TYPE2)
#define WIN_BOOK_SY ( 2)

//�v���C����          :56
#define WIN_TIME_PX ( 3)
#define WIN_TIME_PY (13)
#define WIN_TIME_SX (BMP_CSX_TYPE3)
#define WIN_TIME_SY (2)

//�ȈՉ�b
#define WIN_PMS_PX  ( 2)
#define WIN_PMS_PY  (15)
#define WIN_PMS_SX  (BMP_CSX_TYPE3)
#define WIN_PMS_SY  ( 4)
//////////////////////////////////////////////////////////////
//����
//////////////////////////////////////////////////////////////

//�X�R�A�\��
#define WIN_SCORE_PX  (2)
#define WIN_SCORE_PY  (2)
#define WIN_SCORE_SX  (BMP_CSX_TYPE4)
#define WIN_SCORE_SY  (8)

//�������          :56
#define WIN_COMM_PX (2)
#define WIN_COMM_PY (6)
#define WIN_COMM_SX (BMP_CSX_TYPE3)
#define WIN_COMM_SY (2)
#define WIN_COMM_CGX  (WIN_CLEAR_CGX+WIN_CLEAR_SX*WIN_CLEAR_SY)

//�ʐM�ΐ�            :56
#define WIN_BATTLE_PX (2)
#define WIN_BATTLE_PY (8)
#define WIN_BATTLE_SX (BMP_CSX_TYPE3)
#define WIN_BATTLE_SY (2)
#define WIN_BATTLE_CGX  (WIN_COMM_CGX+WIN_COMM_SX*WIN_COMM_SY)

//�ʐM����            :56
#define WIN_TRADE_PX  (2)
#define WIN_TRADE_PY  (10)
#define WIN_TRADE_SX  (BMP_CSX_TYPE3)
#define WIN_TRADE_SY  (2)
#define WIN_TRADE_CGX (WIN_BATTLE_CGX+WIN_BATTLE_SX*WIN_BATTLE_SY)


//////////////////////////////////////////////////////////////
//���b�Z�[�W�̈�
//////////////////////////////////////////////////////////////
//TalkWindow�L�����̈�
//#define TALKWIN_CGX (MSG_BG_BASE-TALK_WIN_CGX_SIZ)
#define TALKWIN_CGX (1+2*24)  //BG������
//YesnoWindow�L�����̈�
//#define YNWIN_CGX (TALKWIN_CGX-TOUCH_SW_USE_CHAR_NUM)
#define YNWIN_CGX (TALKWIN_CGX+TALK_WIN_CGX_SIZ)
#define YNWIN_PX  (26)
#define YNWIN_PY  (6)

#define WIN_MSG01_PX  (2)
#define WIN_MSG01_PY  (1)
#define WIN_MSG01_SX  (27)
#define WIN_MSG01_SY  (4)
#define WIN_MSG01_CGX (YNWIN_CGX-WIN_MSG01_SX*WIN_MSG01_SY)

// �u�^�b�`�ł߂���v�u�T�C���������v
#define WIN_MSG02_PX  (1)
#define WIN_MSG02_PY  (21)
#define WIN_MSG02_SX  (12)
#define WIN_MSG02_SY  (3)
#define WIN_MSG02_CGX (WIN_MSG01_CGX-WIN_MSG02_SX*WIN_MSG02_SY)

//�߂�{�^��
#define WIN_MSG03_PX  (25)
#define WIN_MSG03_PY  (21)
#define WIN_MSG03_SX  (6)
#define WIN_MSG03_SY  (3)
#define WIN_MSG03_CGX (WIN_MSG02_CGX-WIN_MSG03_SX*WIN_MSG03_SY)



//=============================================================
#define TR_MSGCOLOR     (GF_PRINTCOLOR_MAKE( 1, 2, FBMP_COL_NULL ))
#define SIGN_MSGCOLOR   (GF_PRINTCOLOR_MAKE( FBMP_COL_BLACK, FBMP_COL_BLK_SDW, FBMP_COL_WHITE ))

#define BACK_MSGCOLOR (GF_PRINTCOLOR_MAKE( 5, 4, 0 ))

#define BMP_WIDTH_TYPE0 (8*BMP_CSX_TYPE0)
#define BMP_WIDTH_TYPE1 (8*BMP_CSX_TYPE1)
#define BMP_WIDTH_TYPE2 (8*BMP_CSX_TYPE2)
#define BMP_WIDTH_TYPE3 (8*BMP_CSX_TYPE3)

#define SEC_DISP_OFS  (2)   //�K���B���������Ɍ�����l�ŁB
#define YEN_OFS     (8*1) //�u�~�v�\�����E�X�y�[�X
#define HIKI_OFS    (8*2) //�u�Ђ��v�\�����E�X�y�[�X

#define SEC_DISP_POS_X  (8*24)
#define MINITE_DISP_POS_X (8*26)
#define HOUR_DISP_POS_X (8*21)
#define MINITE_DISP_W (8*2)
#define HOUR_DISP_W (8*4)

///�r�b�g�}�b�v���X�g�p�f�[�^�\����
typedef struct{
  u8  frm_num;  ///<�E�C���h�E�g�p�t���[��
  u8  pos_x;    ///<�E�C���h�E�̈�̍����X���W�i�L�����P�ʂŎw��j
  u8  pos_y;    ///<�E�C���h�E�̈�̍����Y���W�i�L�����P�ʂŎw��j
  u8  siz_x;    ///<�E�C���h�E�̈��X�T�C�Y�i�L�����P�ʂŎw��j
  u8  siz_y;    ///<�E�C���h�E�̈��Y�T�C�Y�i�L�����P�ʂŎw��j
  u8  palnum;   ///<�E�C���h�E�̈�̃p���b�g�i���o�[
}TRC_BMPWIN_DAT;


static const TRC_BMPWIN_DAT TrCardBmpData[] =
{
  
  { // 0:���O
    FONT_BG, WIN_NAME_PX, WIN_NAME_PY,
    WIN_NAME_SX, WIN_NAME_SY, TR_FONT_PAL, 
  },
  { // 1:�g���[�i�[�^�C�v
    FONT_BG, WIN_TRTYPE_PX, WIN_TRTYPE_PY,
    WIN_TRTYPE_SX, WIN_TRTYPE_SY, TR_FONT_PAL, 
  },
  { // 2:��������
    FONT_BG, WIN_PERSONALITY_PX, WIN_PERSONALITY_PY,
    WIN_PERSONALITY_SX, WIN_PERSONALITY_SY, TR_FONT_PAL, 
  },
  
  { // 3:������
    FONT_BG, WIN_BOOK_PX, WIN_BOOK_PY,
    WIN_BOOK_SX, WIN_BOOK_SY, TR_FONT_PAL, 
  },
  { // 4:�v���C����
    FONT_BG, WIN_TIME_PX, WIN_TIME_PY,
    WIN_TIME_SX, WIN_TIME_SY, TR_FONT_PAL, 
  },
  { // 5:�ȈՉ�b
    FONT_BG, WIN_PMS_PX, WIN_PMS_PY,
    WIN_PMS_SX, WIN_PMS_SY, TR_FONT_PAL, 
  },
  
  { // 6:�e��X�R�A�\��
    FONT_BG, WIN_SCORE_PX, WIN_SCORE_PY,
    WIN_SCORE_SX, WIN_SCORE_SY, TR_FONT_PAL, 
  },

  { // 7:���b�Z�[�W�E�B���h�E
    MSG_BG, WIN_MSG01_PX, WIN_MSG01_PY,
    WIN_MSG01_SX, WIN_MSG01_SY, SYS_FONT_PAL, 
  },
};

static void WriteNumData( TR_CARD_WORK* wk,
              GFL_BMPWIN *inWin,
              const u32 inBmpWidth,
              const u32 inRightSpace,
              const u32 inStartY,
              STRBUF *buff,
              const u32 inNum,
              const u8 inDigit,
              const StrNumberDispType inDisptype,
              const u32 wait);
static void WriteNumDataWithCredit( TR_CARD_WORK* wk,
              GFL_BMPWIN *inWin,
              const u32 inBmpWidth,
              const u32 inRightSpace,
              const u32 inStartY,
              STRBUF *buff,
              const u32 inNum,
              const u8 inDigit,
              const StrNumberDispType inDisptype,
              const u32 credit);
static void WriteNumDateYYMMDD( TR_CARD_WORK* wk,
              GFL_BMPWIN *inWin,
              const u32 inBmpWidth,
              const u32 inRightSpace,
              const u32 inStartY,
              STRBUF *buff,
              const StrNumberDispType inDisptype);


static void WriteNumDataFill( TR_CARD_WORK* wk,
                GFL_BMPWIN *inWin,
                const u32 inBmpWidth,
                const u32 inRightSpace,
                const u32 inStartY,
                STRBUF *buff,
                const u32 inNum,
                const u8 inDigit,
                const StrNumberDispType inDisptype,
                const u32 inFillSatrt,
                const u32 inFillWidth,
                const u32 wait);
static void WriteStrData( TR_CARD_WORK* wk,
              GFL_BMPWIN *inWin,
              const u32 inBmpWidth,
              const u32 inRightSpace,
              const u32 inStartY,
              const STRBUF *buff);
//--------------------------------------------------------------------------------------------
/**
 * BMP�E�B���h�E�ǉ�
 *
 * @param bgl   bgl
 * @param win   BmpWin
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void TRCBmp_AddTrCardBmp( TR_CARD_WORK* wk )
{
  u8 i;
  const TRC_BMPWIN_DAT * dat =TrCardBmpData;
  
  GFL_BMPWIN_Init( wk->heapId );
  
  for(i=0;i<TR_CARD_WIN_MAX;i++){
    wk->win[i] = GFL_BMPWIN_Create( dat[i].frm_num , dat[i].pos_x , dat[i].pos_y 
              , dat[i].siz_x , dat[i].siz_y 
              , dat[i].palnum , GFL_BMP_CHRAREA_GET_B );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[i]), 0 );
  }

  //�擪�L�������N���A�i�X�N���[���N���A����Ă�Ƃ���́A���̃L�����Ŗ��܂�j
  GFL_BG_FillCharacter( FONT_BG, 0, 1, 0 );
//  GFL_BG_FillCharacter( MSG_BG, 0, 1, 0 );

  //��b�E�B���h�E�p���\�[�X�W�J
  BmpWinFrame_GraphicSet(MSG_BG,
    TALKWIN_CGX, TALKWIN_PAL, wk->win_type, wk->heapId);
  //�V�X�e���p�t�H���g�p���b�g�]��
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_SUB_BG , SYS_FONT_PAL*32, 16*2, wk->heapId );

  //YesNo�{�^���V�X�e�����[�N�m��
  wk->ynbtn_wk = TOUCH_SW_AllocWork(wk->heapId);
  GFL_BG_LoadScreenV_Req(FONT_BG);
  GFL_BG_LoadScreenV_Req(MSG_BG);

  wk->printQue = PRINTSYS_QUE_Create( wk->heapId );
  
}

//--------------------------------------------------------------------------------------------
/**
 * BMP�E�B���h�E�j��
 *
 * @param win   BmpWin
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void TRCBmp_ExitTrCardBmpWin( TR_CARD_WORK* wk)
{
  u16 i;

  PRINTSYS_QUE_Delete( wk->printQue );

  //YesNo�{�^���V�X�e�����[�N���
  TOUCH_SW_FreeWork( wk->ynbtn_wk);

  for( i=0; i<TR_CARD_WIN_MAX; i++ ){
    GFL_BMPWIN_Delete( wk->win[i] );
  }
  GFL_BMPWIN_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * ���\���i�\�ʁj
 *
 * @param win       BmpWin
 * @param inTrCardData  �g���[�i�[�J�[�h�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void TRCBmp_WriteTrWinInfo(TR_CARD_WORK* wk, GFL_BMPWIN *win[], const TR_CARD_DATA *inTrCardData )
{
  u8 i;
  GFL_MSGDATA* man = wk->msgMan;

  for(i=0;i<5;i++){
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win[i]), 0 );
    //������̂ݎ����Ă���Ƃ������\��
    if ( (i!=3) || (i==3)&&(inTrCardData->PokeBookFlg) ){  
        PRINTSYS_Print( GFL_BMPWIN_GetBmp(win[i]) , 0, 0, wk->CPrmBuf[i], wk->fontHandle );
    }
  }
  {
    STRBUF* str = wk->DigitBuf;
    
    //�Ȃ܂�
    GFL_STR_SetStringCode( wk->TmpBuf, inTrCardData->TrainerName );
    WriteStrData( wk, win[TRC_BMPWIN_TR_NAME],
            BMP_WIDTH_TYPE1, 0, 0, wk->TmpBuf);

    //������
    if (inTrCardData->PokeBookFlg){ //�\���t���O�������Ă���Ƃ��̂ݕ\��  
      WriteNumData( wk, win[TRC_BMPWIN_BOOK],
              BMP_WIDTH_TYPE2, HIKI_OFS, 0, str, inTrCardData->PokeBook, MONS_NUM_DIGIT,
              STR_NUM_DISP_SPACE,0);
    }
    
    
    // �g���[�i�[�^�C�v(���j�I�������ڂ̎擾�E���f)
    TRCBmp_PrintTrainerType( wk, inTrCardData->UnionTrNo, 0 );
    
    // ��������
    TRCBmp_PrintPersonality( wk, inTrCardData->Personality, 0 );   // �Z�[�u�f�[�^�ɐ��i�f�[�^�ۑ����Ȃ�
    
    //�v���C����
    if ( inTrCardData->TimeUpdate ){  //�ʏ�
      WriteNumData( wk, win[TRC_BMPWIN_PLAY_TIME],
            BMP_WIDTH_TYPE3, 0, 0, str, PLAYTIME_GetMinute(inTrCardData->PlayTime),
            TIME_M_DIGIT,
            STR_NUM_DISP_ZERO,0);   //��
      WriteNumData( wk, win[TRC_BMPWIN_PLAY_TIME],
            BMP_WIDTH_TYPE3, 3*8, 0, str, PLAYTIME_GetHour(inTrCardData->PlayTime),
            TIME_H_DIGIT,
            STR_NUM_DISP_SPACE,0);    //��
    }else{            //���ԍX�V���Ȃ��ꍇ�̂݁A�Œ�Łu�F�v�\��
      WriteNumData( wk, win[TRC_BMPWIN_PLAY_TIME],
              BMP_WIDTH_TYPE3, 0, 0, str, inTrCardData->PlayTime_m, TIME_M_DIGIT,
              STR_NUM_DISP_ZERO,0);   //��
      WriteNumData( wk, win[TRC_BMPWIN_PLAY_TIME],
              BMP_WIDTH_TYPE3, 3*8, 0, str, inTrCardData->PlayTime_h, TIME_H_DIGIT,
              STR_NUM_DISP_SPACE,0);    //��
      
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(win[TRC_BMPWIN_PLAY_TIME]) , SEC_DISP_POS_X+SEC_DISP_OFS, 0, wk->CPrmBuf[MSG_TCARD_12], wk->fontHandle );
//      GF_STR_PrintColor(win[TRC_BMPWIN_PLAY_TIME], FONT_SYSTEM, wk->CPrmBuf[MSG_TCARD_12],
//          SEC_DISP_POS_X+SEC_DISP_OFS, 0, 0, TR_MSGCOLOR, NULL);
    }
    
    // �ȈՉ�b�`��
    {
//      STRBUF *str = PMSDAT_ToString( &wk->TrCardData->Pms, wk->heapId );
//      PRINTSYS_Print( GFL_BMPWIN_GetBmp(win[TRC_BMPWIN_PMSWORD]) , 0, 0, str, wk->fontHandle );
//      GFL_STR_DeleteBuffer( str );

        PMS_DRAW_SetNullColorPallet( wk->PmsDrawWork, 0 );
        PMS_DRAW_Print( wk->PmsDrawWork, win[TRC_BMPWIN_PMSWORD], &wk->TrCardData->Pms, 0 );
    }
    
    //�X�^�[�g����
#if 0
    WriteNumData( wk, win[TRC_BMPWIN_START_TIME],
            BMP_WIDTH_TYPE3, 1*8, 0, str, inTrCardData->Start_d, DAY_DIGIT,
            STR_NUM_DISP_ZERO,0); //��
    WriteNumData( wk, win[TRC_BMPWIN_START_TIME],
            BMP_WIDTH_TYPE3, 4*8, 0, str, inTrCardData->Start_m, MONTH_DIGIT,
            STR_NUM_DISP_ZERO,0); //��
    WriteNumData( wk, win[TRC_BMPWIN_START_TIME],
            BMP_WIDTH_TYPE3, 7*8, 0, str, inTrCardData->Start_y, YEAR_DIGIT,
            STR_NUM_DISP_ZERO,0); //�N
#endif
  }
}

// �g���[�i�[�J�[�h�ɕ\�����郆�j�I�������ڕ�����
static const trtype_name_tbl[]={
  // �j�q
  MSG_TRTYPE_ELITEM,        // �G���[�g�g���[�i�[
  MSG_TRTYPE_RANGERM,       // �|�P���������W���[��
  MSG_TRTYPE_BREEDERM,      // �|�P�����u���[�_�[��
  MSG_TRTYPE_SCIENTISTM,    // ���񂫂イ����
  MSG_TRTYPE_TANPAN,        // ����ς񂱂���
  MSG_TRTYPE_MOUNT,         // ��܂��Ƃ�
  MSG_TRTYPE_HEADS,         // �X�L���w�b�Y
  MSG_TRTYPE_KINDERGARTENM, // �悤������

  // ���q
  MSG_TRTYPE_MINI,          // �~�j�X�J�[�g
  MSG_TRTYPE_ELITEW,        // �G���[�g�g���[�i�[��
  MSG_TRTYPE_RANGERW,       // �|�P���������W���[��
  MSG_TRTYPE_BREEDERW,      // �|�P�����u���[�_�[��
  MSG_TRTYPE_SCIENTISTW,    // ���񂫂イ����
  MSG_TRTYPE_NURSE,         // �i�[�X
  MSG_TRTYPE_PARASOL,       // �p���\�����˂�����
  MSG_TRTYPE_KINDERGARTENW, // �悤������
};

//----------------------------------------------------------------------------------
/**
 * @brief �g���[�i�[�^�C�v�̕`��
 *
 * @param   wk       
 * @param   trtype   �g���[�i�[�^�C�v
 * @param   trans_sw �]���X�C�b�`(0:����  1:�]������j
 */
//----------------------------------------------------------------------------------
void TRCBmp_PrintTrainerType( TR_CARD_WORK *wk, int trtype, int trans_sw )
{
  GFL_MSGDATA *msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                                         NARC_message_trtype_dat, wk->heapId );
  STRBUF *str = GFL_MSG_CreateString( msgdata, trtype_name_tbl[trtype] );
  int length = PRINTSYS_GetStrWidth( str, wk->fontHandle, 0 );

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[TRC_BMPWIN_TRAINER_TYPE]), 0 );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->win[TRC_BMPWIN_TRAINER_TYPE]),
                   WIN_TRTYPE_SX*8-length, 0, str, wk->fontHandle );
  GFL_STR_DeleteBuffer( str );
  GFL_MSG_Delete( msgdata );

  if(trans_sw){
    GFL_BMPWIN_MakeScreen( wk->win[TRC_BMPWIN_TRAINER_TYPE] );
    GFL_BMPWIN_TransVramCharacter( wk->win[TRC_BMPWIN_TRAINER_TYPE] );
    GFL_BG_LoadScreenV_Req(FONT_BG);
  }
}

//=============================================================================================
/**
 * @brief ���i�̕`��
 *
 * @param   wk            
 * @param   personality   ���i
 * @param   trans_sw      �]���X�C�b�`(0:����  1:�]������j
 */
//=============================================================================================
void TRCBmp_PrintPersonality( TR_CARD_WORK *wk, int personality, int trans_sw )
{
  STRBUF *str = GFL_MSG_CreateString( wk->msgMan, MSG_TCARD_SEIKAKU_000+personality );
  int length = PRINTSYS_GetStrWidth( str, wk->fontHandle, 0 );

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[TRC_BMPWIN_PERSONALITY]), 0 );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->win[TRC_BMPWIN_PERSONALITY]), 
                  0, 0, wk->CPrmBuf[2], wk->fontHandle );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->win[TRC_BMPWIN_PERSONALITY]),
                   WIN_TRTYPE_SX*8-length, 0, str, wk->fontHandle );
  GFL_STR_DeleteBuffer( str );

  if(trans_sw){
    GFL_BMPWIN_MakeScreen( wk->win[TRC_BMPWIN_PERSONALITY] );
    GFL_BMPWIN_TransVramCharacter( wk->win[TRC_BMPWIN_PERSONALITY] );
    GFL_BG_LoadScreenV_Req(FONT_BG);
  }
  
}

static const u8 tr_on_table[]={ 1,1,1,1,1,1,1, };

//=============================================================================================
/**
 * @brief  BMPWIN�]��
 *
 * @param   wk      TR_CARD_WORK*
 * @param   win[]   �\�p��BMP�����p��BMP�̔z��̐擪
 */
//=============================================================================================
void TRCBmp_TransTrWinInfo(TR_CARD_WORK* wk,GFL_BMPWIN  *win[])
{
  int i;
  
  // �E���ʕ`��
  if(wk->is_back){
    for(i = 6;i < TR_CARD_PARAM_MAX;i++){
      GFL_BMPWIN_MakeScreen( win[i] );
      GFL_BMPWIN_TransVramCharacter( win[i] );
    }

  // �I���e�ʕ`��
  }else{
    for(i = 0;i <= TRC_BMPWIN_PMSWORD;i++){
      if(tr_on_table[i]){
        GFL_BMPWIN_MakeScreen( win[i] );
        GFL_BMPWIN_TransVramCharacter( win[i] );
      }
    }
  }
  GFL_BG_LoadScreenV_Req(FONT_BG);
  GFL_BG_LoadScreenV_Req(MSG_BG);
}

//--------------------------------------------------------------------------------------------
/**
 * ���\���i���ʁj
 *
 * @param win       BmpWin
 * @param inTrCardData  �g���[�i�[�J�[�h�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void TRCBmp_WriteTrWinInfoRev(TR_CARD_WORK* wk,GFL_BMPWIN *win[], const TR_CARD_DATA *inTrCardData  )
{
  u8 i;
  GFL_MSGDATA* man = wk->msgMan;

  for(i=7;i<TR_CARD_PARAM_MAX;i++){
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win[i]), 0 );
//    GF_STR_PrintColor(&win[i], FONT_SYSTEM, wk->CPrmBuf[i], 0, 0, 0, TR_MSGCOLOR, NULL);
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(win[i]) , 0, 0, wk->CPrmBuf[i], wk->fontHandle );
  }
  
  TRCBmp_WriteScoreListWin( wk, wk->scrol_point, 0 );

#if 0
  {
    STRBUF* str = wk->DigitBuf;
    //�a������

    if (inTrCardData->Clear_m != 0){  //����0���łȂ���΁A�N���A�����Ƃ݂Ȃ�
      WriteNumData( wk, win[TRC_BMPWIN_CLEAR_TIME],
              BMP_WIDTH_TYPE3, 8*1, 0, str, inTrCardData->Clear_d, DAY_DIGIT,
              STR_NUM_DISP_ZERO,0);   //��
      WriteNumData( wk, win[TRC_BMPWIN_CLEAR_TIME],
              BMP_WIDTH_TYPE3, 8*4, 0, str, inTrCardData->Clear_m, MONTH_DIGIT,
              STR_NUM_DISP_ZERO,0);   //��
      WriteNumData( wk, win[TRC_BMPWIN_CLEAR_TIME],
              BMP_WIDTH_TYPE3, 8*7, 0, str, inTrCardData->Clear_y, YEAR_DIGIT,
              STR_NUM_DISP_ZERO,0);   //�N
      WriteNumData( wk, win[TRC_BMPWIN_CLEAR_TIME],
              BMP_WIDTH_TYPE3, 0, 16, str, inTrCardData->ClearTime_m, TIME_M_DIGIT,
              STR_NUM_DISP_ZERO,0);   //��
      WriteNumData( wk, win[TRC_BMPWIN_CLEAR_TIME],
              BMP_WIDTH_TYPE3, 8*3, 16, str, inTrCardData->ClearTime_h, TIME_H_DIGIT,
              STR_NUM_DISP_SPACE,0);  //��
    }else{
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(win[TRC_BMPWIN_CLEAR_TIME]) 
            , BMP_WIDTH_TYPE3-(8*9), 0, wk->CPrmBuf[MSG_TCARD_13], wk->fontHandle );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(win[TRC_BMPWIN_CLEAR_TIME]) 
            , BMP_WIDTH_TYPE3-(8*6), 0, wk->CPrmBuf[MSG_TCARD_13], wk->fontHandle );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(win[TRC_BMPWIN_CLEAR_TIME]) 
            , BMP_WIDTH_TYPE3-(8*3), 0, wk->CPrmBuf[MSG_TCARD_13], wk->fontHandle );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(win[TRC_BMPWIN_CLEAR_TIME]) 
            , BMP_WIDTH_TYPE3-(8*5), 16, wk->CPrmBuf[MSG_TCARD_13], wk->fontHandle );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(win[TRC_BMPWIN_CLEAR_TIME]) 
            , BMP_WIDTH_TYPE3-(8*2), 16, wk->CPrmBuf[MSG_TCARD_13], wk->fontHandle );
    }
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(win[TRC_BMPWIN_CLEAR_TIME]) 
          , SEC_DISP_POS_X+SEC_DISP_OFS, 16, wk->CPrmBuf[MSG_TCARD_12], wk->fontHandle );
    
    //�ʐM��
    WriteNumData( wk, win[TRC_BMPWIN_COMM_INFO],
            BMP_WIDTH_TYPE3, 8*2, 0, str, inTrCardData->CommNum, COMM_DIGIT,
            STR_NUM_DISP_SPACE,0);
    
    //�ʐM�ΐ�
    WriteNumData( wk, win[TRC_BMPWIN_BATTLE_INFO],
            BMP_WIDTH_TYPE3, 0, 0, str, inTrCardData->CommBattleLose, BATTLE_DIGIT,
            STR_NUM_DISP_SPACE,0);
    WriteNumData( wk, win[TRC_BMPWIN_BATTLE_INFO],
            BMP_WIDTH_TYPE3, 8*8, 0, str, inTrCardData->CommBattleWin, BATTLE_DIGIT,
            STR_NUM_DISP_SPACE,0);
    
    //�ʐM����
    WriteNumData( wk, win[TRC_BMPWIN_TRADE_INFO],
            BMP_WIDTH_TYPE3, 8*2, 0, str, inTrCardData->CommTrade, TRADE_DIGIT,
            STR_NUM_DISP_SPACE,0);
  }
#endif
}


//----------------------------------------------------------------------------------
/**
 * @brief �X�R�A���X�g��1�s���Ăяo�������\������
 *
 * @param   wk    
 * @param   win   
 * @param   line    
 * @param   y   
 */
//----------------------------------------------------------------------------------
static void print_score_list_line( TR_CARD_WORK *wk, GFL_BMPWIN *win, int line, int y )
{
  TR_CARD_DATA *inTrCardData = wk->TrCardData;
  STRBUF *str = wk->DigitBuf;

  switch(line){
  case SCORE_LINE_ID: // ID
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(win), 0, y, wk->CPrmBuf[MSG_TCARD_06], wk->fontHandle );
    WriteNumData( wk, win, BMP_WIDTH_TYPE0, 0, y, str, inTrCardData->TrainerID, 
                  TR_ID_DIGIT, STR_NUM_DISP_ZERO,0);
    break;
  case SCORE_LINE_MONEY:  // �����Â���
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(win), 0, y, wk->CPrmBuf[MSG_TCARD_06+1], wk->fontHandle );
    WriteNumDataWithCredit( wk, win,
            BMP_WIDTH_TYPE2, YEN_OFS, y, str, inTrCardData->Money, MONEY_DIGIT,
            STR_NUM_DISP_SPACE,1);

    break;
  case SCORE_LINE_START_DATE_1: // �͂��߂��ЂP�s��
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(win), 0, y, wk->CPrmBuf[MSG_TCARD_06+2], wk->fontHandle );
    break;
  case SCORE_LINE_START_DATE_2: // �͂��߂��ЂQ�s��
    WriteNumDateYYMMDD( wk, win, BMP_WIDTH_TYPE3, 0, y, str, STR_NUM_DISP_ZERO );
    break;

#if 0
    WriteNumData( wk, win,
            BMP_WIDTH_TYPE3, 1*8, y, str, inTrCardData->Start_d, DAY_DIGIT,
            STR_NUM_DISP_ZERO,0); //��
    WriteNumData( wk, win,
            BMP_WIDTH_TYPE3, 4*8, y, str, inTrCardData->Start_m, MONTH_DIGIT,
            STR_NUM_DISP_ZERO,0); //��
    WriteNumData( wk, win,
            BMP_WIDTH_TYPE3, 7*8, y, str, inTrCardData->Start_y, YEAR_DIGIT,
            STR_NUM_DISP_ZERO,0); //�N
#endif
    break;
  case SCORE_LINE_FIRST_CHAMPION_1: // �͂��߂Ă̂ł�ǂ�����1�s��
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(win), 0, y, wk->CPrmBuf[MSG_TCARD_06+3], wk->fontHandle );
    if (inTrCardData->Clear_m != 0){  //����0���łȂ���΁A�N���A�����Ƃ݂Ȃ�
      WriteNumData( wk, win,
              BMP_WIDTH_TYPE3, 8*1, y, str, inTrCardData->Clear_d, DAY_DIGIT,
              STR_NUM_DISP_ZERO,0);   //��
      WriteNumData( wk, win,
              BMP_WIDTH_TYPE3, 8*4, y, str, inTrCardData->Clear_m, MONTH_DIGIT,
              STR_NUM_DISP_ZERO,0);   //��
      WriteNumData( wk, win,
              BMP_WIDTH_TYPE3, 8*7, y, str, inTrCardData->Clear_y, YEAR_DIGIT,
              STR_NUM_DISP_ZERO,0);   //�N
    }else{
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(win) 
            , BMP_WIDTH_TYPE3-(8*9), y, wk->CPrmBuf[MSG_TCARD_13], wk->fontHandle );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(win) 
            , BMP_WIDTH_TYPE3-(8*6), y, wk->CPrmBuf[MSG_TCARD_13], wk->fontHandle );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(win) 
            , BMP_WIDTH_TYPE3-(8*3), y, wk->CPrmBuf[MSG_TCARD_13], wk->fontHandle );
    }

    break;
  case SCORE_LINE_FIRST_CHAMPION_2: // �͂��߂Ă̂ł�ǂ�����2�s��
    if (inTrCardData->Clear_m != 0){  //����0���łȂ���΁A�N���A�����Ƃ݂Ȃ�
      WriteNumData( wk, win,
              BMP_WIDTH_TYPE3, 0, y, str, inTrCardData->ClearTime_m, TIME_M_DIGIT,
              STR_NUM_DISP_ZERO,0);   //��
      WriteNumData( wk, win,
              BMP_WIDTH_TYPE3, 8*3, y, str, inTrCardData->ClearTime_h, TIME_H_DIGIT,
              STR_NUM_DISP_SPACE,0);  //��
    }else{
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(win) 
            , BMP_WIDTH_TYPE3-(8*5), y, wk->CPrmBuf[MSG_TCARD_13], wk->fontHandle );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp(win) 
            , BMP_WIDTH_TYPE3-(8*2), y, wk->CPrmBuf[MSG_TCARD_13], wk->fontHandle );
    }
    break;
  case SCORE_LINE_COMM_NUM: // �����񂵂���
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(win), 0, y, wk->CPrmBuf[MSG_TCARD_06+4], wk->fontHandle );
    //�ʐM��
    WriteNumDataWithCredit( wk, win,
            BMP_WIDTH_TYPE3, 8*2, y, str, inTrCardData->CommNum, COMM_DIGIT,
            STR_NUM_DISP_SPACE,0);

    break;
  case SCORE_LINE_COMM_BATTLE_NUM:  // �ʐM�ΐ��
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(win), 0, y, wk->CPrmBuf[MSG_TCARD_06+5], wk->fontHandle );
    //�ʐM��
    WriteNumDataWithCredit( wk, win,
            BMP_WIDTH_TYPE3, 8*2, y, str, inTrCardData->CommBattleNum, COMM_DIGIT,
            STR_NUM_DISP_SPACE,0);
    break;
  case SCORE_LINE_COMM_WIN_NUM:
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(win), 0, y, wk->CPrmBuf[MSG_TCARD_06+6], wk->fontHandle );
    WriteNumDataWithCredit( wk, win,
            BMP_WIDTH_TYPE3, 8*2, y, str, inTrCardData->CommBattleWin, BATTLE_DIGIT,
            STR_NUM_DISP_SPACE,0);
    break;
  case SCORE_LINE_COMM_LOSE_NUM:  // �ʐM�ΐ�ŏ�������
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(win), 0, y, wk->CPrmBuf[MSG_TCARD_06+7], wk->fontHandle );
    WriteNumDataWithCredit( wk, win,
            BMP_WIDTH_TYPE3, 8*2, y, str, inTrCardData->CommBattleLose, BATTLE_DIGIT,
            STR_NUM_DISP_SPACE,0);
    break;
  case SCORE_LINE_COMM_TRADE_NUM:  // �ʐM�ΐ�ŕ�������
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(win), 0, y, wk->CPrmBuf[MSG_TCARD_06+8], wk->fontHandle );
    WriteNumDataWithCredit( wk, win,
            BMP_WIDTH_TYPE3, 8*2, y, str, inTrCardData->CommTrade, TRADE_DIGIT,
            STR_NUM_DISP_SPACE,0);
    break;
  }


}



//=============================================================================================
/**
 * @brief �X�R�A�\�����X�N���[�����郊�X�g���ōs��
 *
 * @param   wk    
 * @param   scrol_point   
 * @param   trans_sw
 */
//=============================================================================================
void TRCBmp_WriteScoreListWin( TR_CARD_WORK *wk, int scrol_point, int trans_sw )
{
  GFL_BMPWIN *win = wk->win[TRC_BMPWIN_SCORE_LIST];
  int start,y,i;

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win), 0 );

  start = -(scrol_point/16);
  y     = (scrol_point%16);

  OS_Printf("scrol=%d start=%d y=%d\n",scrol_point, start, y);

  for(i=0;i<5;i++){
    print_score_list_line( wk, win,start+i, y+16*i );
  }
  
  if(trans_sw){
    GFL_BMPWIN_MakeTransWindow_VBlank( win );
  }
}


//--------------------------------------------------------------------------------------------
/**
 * BMP�E�B���h�E��\��
 *
 * @param win       BmpWin
 * @param start     �ΏۃE�B���h�E�J�n�C���f�b�N�X
 * @param end       �ΏۃE�B���h�E�ŏI�C���f�b�N�X
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void TRCBmp_NonDispWinInfo(GFL_BMPWIN *win[], const u8 start, const u8 end)
{
  u8 i;
  for(i=start;i<=end;i++){
//    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win[i]), 0 );
    GFL_BMPWIN_ClearScreen( win[i] );
//    GFL_BMPWINOff( &win[i] );
  }
//  GFL_BG_ClearScreen( MSG_BG );
}

//--------------------------------------------------------------------------------------------
/**
 * �����\��
 *
 * @param win       BmpWin
 * @param inBmpWidth    ��
 * @param inRightSpace  �E��
 * @param inStartY    �\���J�nY�ʒu
 * @param buff      �o�b�t�@
 * @param inNum     ����
 * @param inDigit     ����
 * @param inDispType    �\���^�C�v
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WriteNumData( TR_CARD_WORK* wk,
              GFL_BMPWIN *inWin,
              const u32 inBmpWidth,
              const u32 inRightSpace,
              const u32 inStartY,
              STRBUF *buff,
              const u32 inNum,
              const u8 inDigit,
              const StrNumberDispType inDisptype,
              const u32 wait)
{
  u32 len;

  STRTOOL_SetNumber( buff, inNum, inDigit, inDisptype, STR_NUM_CODE_DEFAULT );
  len = PRINTSYS_GetStrWidth(buff,wk->fontHandle,0);

//  GF_STR_PrintColor(  inWin, FONT_SYSTEM, buff,
//            inBmpWidth-(len+inRightSpace), inStartY,
//            0, TR_MSGCOLOR, NULL);  
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(inWin) 
        , inBmpWidth-(len+inRightSpace), inStartY
        , buff, wk->fontHandle );
}



//--------------------------------------------------------------------------------------------
/**
 * �����\��(�P�ʂ��ꏏ�ɕ\���u��v�E�u�~�v
 *
 * @param win       BmpWin
 * @param inBmpWidth    ��
 * @param inRightSpace  �E��
 * @param inStartY    �\���J�nY�ʒu
 * @param buff      �o�b�t�@
 * @param inNum     ����
 * @param inDigit     ����
 * @param inDispType    �\���^�C�v
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WriteNumDataWithCredit( TR_CARD_WORK* wk,
              GFL_BMPWIN *inWin,
              const u32 inBmpWidth,
              const u32 inRightSpace,
              const u32 inStartY,
              STRBUF *buff,
              const u32 inNum,
              const u8 inDigit,
              const StrNumberDispType inDisptype,
              const u32 credit)
{
  u32 len;

  WORDSET_RegisterNumber( wk->wordset, 0, inNum, inDigit, inDisptype, STR_NUM_DISP_LEFT );
  WORDSET_ExpandStr( wk->wordset, buff, wk->CreditBuf[credit] );
  len = PRINTSYS_GetStrWidth(buff,wk->fontHandle,0);

  PRINTSYS_Print( GFL_BMPWIN_GetBmp(inWin) 
        , inBmpWidth-(len+inRightSpace), inStartY
        , buff, wk->fontHandle );
}


//--------------------------------------------------------------------------------------------
/**
 * �����\��
 *
 * @param win       BmpWin
 * @param inBmpWidth    ��
 * @param inRightSpace  �E��
 * @param inStartY    �\���J�nY�ʒu
 * @param buff      �o�b�t�@
 * @param inNum     ����
 * @param inDigit     ����
 * @param inDispType    �\���^�C�v
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WriteNumDateYYMMDD( TR_CARD_WORK* wk,
              GFL_BMPWIN *inWin,
              const u32 inBmpWidth,
              const u32 inRightSpace,
              const u32 inStartY,
              STRBUF *buff,
              const StrNumberDispType inDisptype)
{
  u32 len;

  WORDSET_RegisterNumber( wk->wordset, 0, wk->TrCardData->Start_y, 2, inDisptype, STR_NUM_CODE_ZENKAKU );
  WORDSET_RegisterNumber( wk->wordset, 1, wk->TrCardData->Start_m, 2, inDisptype, STR_NUM_CODE_ZENKAKU );
  WORDSET_RegisterNumber( wk->wordset, 2, wk->TrCardData->Start_d, 2, inDisptype, STR_NUM_CODE_ZENKAKU );
  WORDSET_ExpandStr( wk->wordset, buff, wk->CreditBuf[TR_CARD_CREDIT_YYMMDD] );
  len = PRINTSYS_GetStrWidth(buff,wk->fontHandle,0);

  PRINTSYS_Print( GFL_BMPWIN_GetBmp(inWin) 
        , inBmpWidth-(len+inRightSpace), inStartY
        , buff, wk->fontHandle );
}

//--------------------------------------------------------------------------------------------
/**
 * �����\��(�㏑���p)
 *
 * @param win       BmpWin
 * @param inBmpWidth    ��
 * @param inRightSpace  �E��
 * @param inStartY    �\���J�nY�ʒu
 * @param buff      �o�b�t�@
 * @param inNum     ����
 * @param inDigit     ����
 * @param inDispType    �\���^�C�v
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void 
WriteNumDataFill( TR_CARD_WORK* wk,
          GFL_BMPWIN *inWin,
          const u32 inBmpWidth,
          const u32 inRightSpace,
          const u32 inStartY,
          STRBUF *buff,
          const u32 inNum,
          const u8 inDigit,
          const StrNumberDispType inDisptype,
          const u32 inFillSatrt,
          const u32 inFillWidth,
          const u32 wait)
{
  u32 len;
  
  STRTOOL_SetNumber( buff, inNum, inDigit, inDisptype, STR_NUM_CODE_DEFAULT );
  len = PRINTSYS_GetStrWidth(buff,wk->fontHandle,0);

  GFL_BMP_Fill( GFL_BMPWIN_GetBmp(inWin),inFillSatrt,  0, inFillWidth, 2*8, 0 );

  PRINTSYS_Print( GFL_BMPWIN_GetBmp(inWin) 
        , inBmpWidth-(len+inRightSpace), inStartY
        , buff, wk->fontHandle );
}


//--------------------------------------------------------------------------------------------
/**
 * ������\��
 *
 * @param win       BmpWin
 * @param inBmpWidth    ��
 * @param inRightSpace  �E��
 * @param inStartY    �\���J�nY�ʒu
 * @param buff      �o�b�t�@
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void WriteStrData( TR_CARD_WORK* wk,
              GFL_BMPWIN *inWin,
              const u32 inBmpWidth,
              const u32 inRightSpace,
              const u32 inStartY,
              const STRBUF *buff)
{
  u32 len;
  len = PRINTSYS_GetStrWidth(buff,wk->fontHandle,0);
//  GF_STR_PrintColor(  inWin, FONT_SYSTEM, buff,
//            inBmpWidth-(len+inRightSpace), inStartY,
//            0, TR_MSGCOLOR, NULL);
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(inWin) 
        , inBmpWidth-(len+inRightSpace), inStartY
        , buff, wk->fontHandle );
}

//--------------------------------------------------------------------------------------------
/**
 * �v���C���ԕ`��
 *
 * @param win       BmpWin
 * @param inTrCardData  �g���[�i�[�J�[�h�f�[�^
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void TRCBmp_WritePlayTime(TR_CARD_WORK* wk, GFL_BMPWIN  *win[], const TR_CARD_DATA *inTrCardData, STRBUF *str)
{
  int hour;
//  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win[TRC_BMPWIN_PLAY_TIME]), 0 );
  
  GF_ASSERT(inTrCardData->PlayTime!=NULL&&"ERROR:PlayTimeData is NULL!!");

  // ���ԕ\�L���������N���A����
  GFL_BMP_Fill( GFL_BMPWIN_GetBmp(win[TRC_BMPWIN_PLAY_TIME]), 18*8, 0, WIN_TIME_SX*8-18*8, 16, 0 );

  // �v���C���ԏ�������
  hour = PLAYTIME_GetHour(inTrCardData->PlayTime);

  if (hour>HOUR_DISP_MAX){
    hour = HOUR_DISP_MAX;
  }

  WriteNumDataFill( wk, win[TRC_BMPWIN_PLAY_TIME],
            BMP_WIDTH_TYPE3, 0, 0, str,
            PLAYTIME_GetMinute(inTrCardData->PlayTime),
            TIME_M_DIGIT, STR_NUM_DISP_ZERO,
            MINITE_DISP_POS_X,
            MINITE_DISP_W,0);   //��
  WriteNumDataFill( wk, win[TRC_BMPWIN_PLAY_TIME],
            BMP_WIDTH_TYPE3, 3*8, 0, str,
            hour,
            TIME_H_DIGIT, STR_NUM_DISP_SPACE,
            HOUR_DISP_POS_X,
            HOUR_DISP_W,0);   //��
  //�����������A���^�C���X�V�����邩��
  GFL_BMPWIN_TransVramCharacter(win[TRC_BMPWIN_PLAY_TIME]);
}

//--------------------------------------------------------------------------------------------
/**
 * �b�\��
 *
 * @param win       BmpWin
 * @param inDisp      �\���t���O
 * @param inSecBuf    ������i�R�����j
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void TRCBmp_WriteSec(TR_CARD_WORK* wk,GFL_BMPWIN  *win, const BOOL inDisp, STRBUF *inSecBuf)
{
  if (inDisp){
//    GF_STR_PrintColor(win, FONT_SYSTEM, inSecBuf, SEC_DISP_POS_X+SEC_DISP_OFS, 0, MSG_ALLPUT, TR_MSGCOLOR, NULL);
    PRINTSYS_Print( GFL_BMPWIN_GetBmp(win) 
          , SEC_DISP_POS_X+SEC_DISP_OFS, 0
          , inSecBuf, wk->fontHandle );
  }else{
    GFL_BMP_Fill( GFL_BMPWIN_GetBmp(win), SEC_DISP_POS_X, 0, 8, 2*8, 0 );
//    GFL_BMPWINOn( win );
  }
  GFL_BMPWIN_TransVramCharacter(win);
  GFL_BG_LoadScreenV_Req( FONT_BG);
}


//--------------------------------------------------------------------------------------------
/**
 * �T�C���A�v���Ăяo�� �m�F���b�Z�[�W�ǉ�
 *
 * @param wk        ���[�N
 * @param on_f      �\����ԃt���O
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void TRCBmp_SignDrawMsgPut(TR_CARD_WORK* wk,const u8 pat)
{
  if(pat == 0){
    BmpWinFrame_Write( wk->win[TRC_BMPWIN_MSG],WINDOW_TRANS_OFF, TALKWIN_CGX, TALKWIN_PAL );
  }
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->win[TRC_BMPWIN_MSG]),15/*FBMP_COL_WHITE*/);

//  wk->msgIdx = GF_STR_PrintColor(&wk->win[TRC_BMPWIN_MSG], FONT_TALK, 
//      wk->SignBuf[pat], 0, 0, wk->msg_spd, SIGN_MSGCOLOR, NULL);  
  wk->printHandle = PRINTSYS_PrintStream( wk->win[TRC_BMPWIN_MSG], 0,0
      ,wk->SignBuf[pat] ,wk->fontHandle ,MSGSPEED_GetWait() 
      ,wk->vblankTcblSys , 0 , wk->heapId , 0);
  
  GFL_BMPWIN_MakeScreen(wk->win[TRC_BMPWIN_MSG]);
  GFL_BG_LoadScreenV_Req( MSG_BG);
}

//--------------------------------------------------------------------------------------------
/**
 * �T�C���A�v���Ăяo�� YesNo�{�^���\��
 *
 * @param wk        ���[�N
 * @param on_f      �\����ԃt���O
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void TRCBmp_SignDrawYesNoCall(TR_CARD_WORK* wk,const u8 pat)
{
  TOUCH_SW_PARAM param;

  MI_CpuClear8(&param,sizeof(TOUCH_SW_PARAM));

  param.bg_frame = MSG_BG;
  param.char_offs = YNWIN_CGX;
  param.pltt_offs = YNWIN_PAL;
  param.x   = YNWIN_PX;
  param.y   = YNWIN_PY;
  param.kt_st = wk->key_mode;
  param.key_pos = 0;
  param.type = TOUCH_SW_TYPE_S;
  TOUCH_SW_Init( wk->ynbtn_wk, &param);

  GFL_BG_LoadScreenV_Req( MSG_BG);
}

//--------------------------------------------------------------------------------------------
/**
 * �T�C���A�v���Ăяo�� YesNo�{�^���҂�
 *
 * @param wk        ���[�N
 * @param on_f      �\����ԃt���O
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
int TRCBmp_SignDrawYesNoWait(TR_CARD_WORK* wk,const u8 pat)
{
  int ret;

  ret = TOUCH_SW_Main( wk->ynbtn_wk );
  switch(ret){
  case TOUCH_SW_RET_YES:
    ret = TRUE;
    break;
  case TOUCH_SW_RET_NO:
    ret = FALSE;
    break;
  default:
    return -1;
  }
  //���݂̑��샂�[�h���擾
  wk->key_mode = TOUCH_SW_GetKTStatus(wk->ynbtn_wk);
  TOUCH_SW_Reset( wk->ynbtn_wk);

  if(ret == FALSE || pat){
    BmpWinFrame_Clear( wk->win[TRC_BMPWIN_MSG],WINDOW_TRANS_ON);
    GFL_BG_LoadScreenV_Req( MSG_BG);
  }
  return ret;
}

