//============================================================================================
/**
 * @file    leader_board_def.h
 * @brief   ���[�_�[�{�[�h��ʁiWifi�o�g���T�u�E�F�C�_�E�����[�h�f�[�^�j��`���
 * @author  Akito Mori
 * @date    10.02.22
 */
//============================================================================================
#ifndef __LEADER_BOARD_DEF_H__
#define __LEADER_BOARD_DEF_H__


//--------------------------------------------------------------------------------------------
// �萔��`
//--------------------------------------------------------------------------------------------

// �P��ʂɕ\������l��
#define LB_1PAGE_NUM        ( 10 )

// �^�b�`�o�[�̊e����`
#define TOUCHBAR_X    (  0 )
#define TOUCHBAR_Y    ( 21 )
#define TOUCHBAR_W    ( 32 )
#define TOUCHBAR_H    (  3 )
#define TOUCHBAR_PAL  ( 14 )

// ���C����ʂ̃p���b�g��`
#define TALKFONT_PAL          ( 15 )
#define TALKFONT_PAL_OFFSET   ( TALKFONT_PAL*0x20 )

// �v���[�g�̊e���
#define LB_PLATE_ID   (  0 )
#define LB_PLATE0_X   (  1 )
#define LB_PLATE1_X   ( 17 )
#define LB_PLATE0_Y   (  0 )
#define LB_PLATE2_Y   (  4 )
#define LB_PLATE4_Y   (  8 )
#define LB_PLATE6_Y   ( 12 )
#define LB_PLATE8_Y   ( 16 )
#define LB_PLATE_W    ( 14 )
#define LB_PLATE_H    (  4 )




// �Z���A�N�^�[
enum{
  LB_OBJ_CURSOR=0,
  LB_OBJ_END,
  LB_OBJ_LEFT,
  LB_OBJ_RIGHT,
  LB_OBJ_MAX,
};

// �Z���A�N�^�[���\�[�X
enum{
  LB_RES_PLTT=0,
  LB_RES_CHR,
  LB_RES_CELL,
  LB_RES_COMMON_PLTT,
  LB_RES_COMMON_CHR,
  LB_RES_COMMON_CELL,
  LB_RES_MAX,
};


// BMPWIN
enum{
  LB_BMPWIN_M_NAME0=0,    // ���[�_�[��0
  LB_BMPWIN_M_NAME1,
  LB_BMPWIN_M_NAME2,
  LB_BMPWIN_M_NAME3,
  LB_BMPWIN_M_NAME4,
  LB_BMPWIN_M_NAME5,
  LB_BMPWIN_M_NAME6,
  LB_BMPWIN_M_NAME7,
  LB_BMPWIN_M_NAME8,
  LB_BMPWIN_M_NAME9,

  LB_BMPWIN_S_REKIDAI_NO_TOREINAA,  // �u�ꂫ�����̃g���[�i�[�v
  LB_BMPWIN_S_RANK,                 // �����N����
  LB_BMPWIN_S_TRAIN_NO,             // �g���C���i���o�[����
  LB_BMPWIN_S_NAME,         // ���ʃ��[�_�[��
  LB_BMPWIN_S_SUNDERUBASHO, // ���ʁu�Z��ł�ꏊ�v
  LB_BMPWIN_S_COUTRY,       // ��
  LB_BMPWIN_S_JIKOSHOUKAI,  // ���ʁu���ȏЉ�v
  LB_BMPWIN_S_PMS,          // �ȈՉ�b�̈�

  // �ȈՉ�b��PMS_DRAW�ŕ`��

  LB_BMPWIN_ID_MAX,  
};

// STRBUF�w��p
enum{
  LB_STR_REKIDAI=0,
  LB_STR_RANk,
  LB_STR_TRAINNO,
  LB_STR_SUNDERU,
  LB_STR_COUNTRY,
  LB_STR_LOCAL,
  LB_STR_JIKOSHOUKAI,
  LB_STR_MAX,
};

// �^�b�`�o�[���o���̋@�\
enum{
  FUNC_LEFT_PAGE=0,
  FUNC_RIGHT_PAGE,
  FUNC_END,
};

#define LB_CLACT_NUM        ( LB_OBJ_MAX ) // OBJ�o�^�ő吔
#define LB_PMSDRAW_OBJ_NUM  ( 2 )          // �ȈՉ�b�\���ɂ�����


// �T�u��ʂ�BMPWIN�̐�
#define SUB_BMPWIN_NUM  ( LB_BMPWIN_ID_MAX - LB_BMPWIN_S_REKIDAI_NO_TOREINAA)
// ���C����ʂ�BMPWIN�̐�
#define MAIN_BMPWIN_NUM ( LB_BMPWIN_M_NAME9+1 )
// �v���[�g�N���A�p���b�g�f�[�^
#define NAME_PLATE_CLEAR_COL  ( 0x0404 )


  
#endif