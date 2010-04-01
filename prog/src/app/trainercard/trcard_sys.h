//============================================================================================
/**
 * @file  trcard_bmp.h
 * @bfief �g���[�i�[�J�[�h��ʃV�X�e���w�b�_
 * @author  Akito Mori / Nozomu Saito (WB�ڐA�͗L�򂭂�)
 * @date  10.03.09
 */
//============================================================================================
#ifndef _TRCARD_SYS_H_
#define _TRCARD_SYS_H_

#include "system/bmp_menu.h"
#include "system/pms_data.h"
#include "app/trainer_card.h"
#include "trcard_obj.h"
#include "trcard_snd.h"
#include "system/pms_draw.h"

#define TRC_FRAME_RATE (60)

#define TRC_SCREEN_WIDTH  (32)
//#define TR_CARD_WIN_MAX   (14)
#define TR_CARD_PARAM_MAX (7)
#define TR_CPRM_STR_MAX   (17)

#define TR_CARD_BADGE_LV_MAX  (4)   //�o�b�W�̍ő僌�x��
#define TR_BADGE_NUM_MAX  (16)


#define CALL_NONE     ( 0 ) ///< �g���[�i�[�J�[�h�ʏ�I��
#define CALL_PMSINPUT ( 1 ) ///< �g���[�i�J�[�h����ȈՉ�b�Ăяo��
#define CALL_BADGE    ( 2 ) ///< �g���[�i�[�J�[�h����o�b�a�m�F��ʌĂяo��
#define CALL_CARD     ( 3 ) ///< �g���[�i�[�J�[�h����o�b�a�m�F��ʌĂяo��

//�p���b�g�g�p���X�g
enum{
  CASE_BD_PAL   = 0,
  YNWIN_PAL     = 11, //2�{�g��
  TALKWIN_PAL   = 13,
  SYS_FONT_PAL  = 14,
  TR_FONT_PAL   = 15,
  BACK_FONT_PAL = 15,
};

//=====================================
// BMPWIN�z��w��p��`
//=====================================
typedef enum{
  // �\
  TRC_BMPWIN_TR_NAME=0,     // �g���[�i�[��
  TRC_BMPWIN_TRAINER_TYPE,// �����őI���ł���g���[�i�[�^�C�v��
  TRC_BMPWIN_PERSONALITY, // �����őI���ł��邹��������
  TRC_BMPWIN_BOOK,        // ������
  TRC_BMPWIN_PLAY_TIME,   // �v���C����
  TRC_BMPWIN_PMSWORD,      // �ȈՉ�b

  // ��
  TRC_BMPWIN_SCORE_LIST, // �X�N���[���ł���e����E�C���h�E

  // ���b�Z�[�W
  TRC_BMPWIN_MSG,         // �T�C����ʃE�C���h�E
  TR_CARD_WIN_MAX,
}TRC_BMPWIN;

enum{
  TR_CARD_CREDIT_KAI=0,
  TR_CARD_CREDIT_YEN,
  TR_CARD_CREDIT_YYMMDD,
  TR_CREDIT_STR_MAX,
};


typedef struct TR_SCRUCH_SND_tag
{
  s8 OldDirX;
  s8 OldDirY;
  s8 DirX;
  s8 DirY;
  u8 Snd;
}TR_SCRUCH_SND;

// �^�b�`�p�l�����\����
typedef struct{
  u8  x;      // �ő�8�񕪂̃^�b�`���W
  u8  y;      // 
  u8  brush;  // �T���v�����O������
  u8  on;     // ���͂��肩�ǂ���
}TOUCH_INFO;


typedef struct TR_CARD_WORK_tag
{
  HEAPID heapId;
  GFL_BMPWIN  *win[TR_CARD_WIN_MAX];      // BMP�E�B���h�E�f�[�^�i�ʏ�j

  TRCARD_CALL_PARAM* tcp; ///<�J�[�h�Ăяo���p�����[�^
  TR_CARD_DATA *TrCardData;
  u8 TrSignData[SIGN_SIZE_X*SIGN_SIZE_Y*64];  //�]������̂łS�o�C�g���E�̈ʒu�ɒu������
  void *TrArcData;              //�g���[�i�[�O���t�B�b�N�A�[�J�C�u�f�[�^
  NNSG2dCharacterData* TrCharData;      //�g���[�i�[�L�����f�[�^�|�C���^
  BOOL is_back;
  BOOL brushOK;
  
  fx32 CardScaleX;
  fx32 CardScaleY;
  fx32 CoverScaleY;
  int  RotateCount;
  int  CardCenterX;
  int  CardCenterY;
  int  CardOffsetX;
  int  CardOffsetY;
  u16  ScaleSide;     // �J�[�h�̍��E�ǂ������g�債�Ă��邩�H( 0:�� 1:�E�j
  u16  ScaleMode;     // �g��k�����[�h���

//  int touch;
  int  key_mode;
  BOOL rc_cover;
  int  BeforeX;
  int  BeforeY;
  TR_CARD_OBJ_WORK ObjWork;
//  int RevSpeed;
  int  Scroll;
  BOOL ButtonPushFlg;
  SND_BADGE_WORK SndBadgeWork;
  TR_SCRUCH_SND  ScruchSnd;
  
  void             *TrScrnArcData;
  NNSG2dScreenData *ScrnData;
  void             *pScrnBCase;
  NNSG2dScreenData *pSBCase;
  
//  TOUCH_SW_SYS *ynbtn_wk;   // YesNo�I���{�^�����[�N
  GFL_TCB      *vblankFunc;
  GFL_TCBLSYS  *vblankTcblSys;

  GFL_FONT     *fontHandle;
  GFL_MSGDATA  *msgMan;
  WORDSET      *wordset;
  
  STRBUF  *PlayTimeBuf;   //�v���C���ԗp������o�b�t�@
  STRBUF  *SecBuf;        //�v���C���ԃR�����p������o�b�t�@
  STRBUF  *TmpBuf;        //�e���|����������
  STRBUF  *DigitBuf;      //�J�[�h���l�p�����[�^�W�J�p������o�b�t�@
  STRBUF  *ExpBuf[3];     //<�������b�Z�[�W������o�b�t�@
  STRBUF  *SignBuf[2];    //�T�C���������b�Z�[�W������o�b�t�@
  STRBUF  *CPrmBuf[TR_CPRM_STR_MAX];  //�J�[�h�p�����[�^�W�J�p������o�b�t�@
  STRBUF  *CreditBuf[TR_CREDIT_STR_MAX];  // �J�[�h�p�����[�^�W�J�p(�񐔒P�ʕ\���p�j������o�b�t�@

  PMS_DATA      pms;          ///< �ȈՉ�b
  PMS_DRAW_WORK *PmsDrawWork; ///< �ȈՉ�b�`�惏�[�N

  u8 badge[TR_BADGE_NUM_MAX];
  u8 IsOpen;
  u8 Counter;
  u8 sub_seq;
  
  u8 SignAnimeWait;   // �g���[�i�[�T�C���A�j���p���[�N
  u8 SignAnimePat;    // �g���[�i�[�T�C���A�j���̃p�^�[���w��iSIGN_ALL,SIGN_LEFT,SIGN_RIGHT)
  u8 SecCount;        // �b�\���̂��߂̃J�E���^
  u8 isClear:1;       // �a�����肵�Ă��邩�ۂ��H
  u8 isComm:1;        // �ʐM�����ۂ��H
  u8 aff_req:1;       // �A�t�B���ϊ����s���N�G�X�g
  u8 drug_f:1;        // �h���b�O�t���O
  u8 Dummy:5;         // �p�f�B���O
  
  PRINT_QUE     *printQue;
  PRINT_STREAM  *printHandle;
  u8  win_type;  ///<�E�B���h�E�^�C�v
  u8  msg_spd;   ///<���b�Z�[�W�X�s�[�h
  u8  scrl_ct;   ///<�w�i�X�N���[���J�E���^
  u32 tp_x;
  u32 tp_y;
  u8  draw_seq;

  s16 scrol_point;      // �X�R�A�\���X�N���[���|�C���g
  s16 old_scrol_point;  // �X�N���[���|�C���g�ߋ��ʒu
  s16 touch_sy;         // �^�b�`�X�^�[�gY���W
  s16 scrol_start;      // �X�N���[���X�^�[�g�ʒu

  TOUCH_INFO      MyTouchResult;  
  TOUCH_INFO      AllTouchResult; // �ʐM�Ŏ擾�����T���v�����O���ʁi���̃f�[�^�ŕ`�悷��
  TOUCH_INFO      OldTouch;       // �O�񂩂�̃|�C���g����
  u8  pen;                        // �T�C���̐F(0:�� 1:���j
  u8  card_list_col;              // �J�[�h���ʂ̃��X�g�`��F�����F�ɂ���i0:�� 1:���F�j

}TR_CARD_WORK;

extern GFL_PROC_RESULT TrCardProc_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
extern GFL_PROC_RESULT TrCardProc_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk );
extern GFL_PROC_RESULT TrCardProc_End( GFL_PROC * proc, int * seq , void *pwk, void *mywk );

#endif //_TRCARD_SYS_H_


