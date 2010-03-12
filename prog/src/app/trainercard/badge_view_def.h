//============================================================================================
/**
 * @file    badge_view_def.h
 * @brief   �o�b�W�{�����
 * @author  Akito Mori
 * @date    10.03.02
 */
//============================================================================================
#ifndef __BADGE_VIEW_DEF_H__
#define __BADGE_VIEW_DEF_H__


//--------------------------------------------------------------------------------------------
// �萔��`
//--------------------------------------------------------------------------------------------

// �P��ʂɕ\������l��
#define BV_1PAGE_NUM        ( 10 )

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
#define BV_PLATE_ID   (  0 )
#define BV_PLATE0_X   (  1 )
#define BV_PLATE1_X   ( 17 )
#define BV_PLATE0_Y   (  0 )
#define BV_PLATE2_Y   (  4 )
#define BV_PLATE4_Y   (  8 )
#define BV_PLATE6_Y   ( 12 )
#define BV_PLATE8_Y   ( 16 )
#define BV_PLATE_W    ( 14 )
#define BV_PLATE_H    (  4 )




// �Z���A�N�^�[
enum{
  BV_OBJ_CARD=0,   ///< �J�[�h�ɖ߂�{�^��
  BV_OBJ_RETURN,   ///< �߂�uU�v
  BV_OBJ_END,      ///< ��߂�u�~�v
  BV_OBJ_BOOKMARK, ///< �u�b�N�}�[�N�Y
  BV_OBJ_BADGE0,   ///< �o�b�W�P
  BV_OBJ_BADGE1,
  BV_OBJ_BADGE2,
  BV_OBJ_BADGE3,
  BV_OBJ_BADGE4,
  BV_OBJ_BADGE5,
  BV_OBJ_BADGE6,
  BV_OBJ_BADGE7,  ///< �o�b�W�W
  BV_OBJ_KIRAKIRA0_0, // �L���L��OBJ�i�o�b�W�ЂƂ�3��8�j
  BV_OBJ_KIRAKIRA0_1,
  BV_OBJ_KIRAKIRA0_2,
  BV_OBJ_KIRAKIRA1_0,
  BV_OBJ_KIRAKIRA1_1,
  BV_OBJ_KIRAKIRA1_2,
  BV_OBJ_KIRAKIRA2_0,
  BV_OBJ_KIRAKIRA2_1,
  BV_OBJ_KIRAKIRA2_2,
  BV_OBJ_KIRAKIRA3_0,
  BV_OBJ_KIRAKIRA3_1,
  BV_OBJ_KIRAKIRA3_2,
  BV_OBJ_KIRAKIRA4_0,
  BV_OBJ_KIRAKIRA4_1,
  BV_OBJ_KIRAKIRA4_2,
  BV_OBJ_KIRAKIRA5_0,
  BV_OBJ_KIRAKIRA5_1,
  BV_OBJ_KIRAKIRA5_2,
  BV_OBJ_KIRAKIRA6_0,
  BV_OBJ_KIRAKIRA6_1,
  BV_OBJ_KIRAKIRA6_2,
  BV_OBJ_KIRAKIRA7_0,
  BV_OBJ_KIRAKIRA7_1,
  BV_OBJ_KIRAKIRA7_2,
  BV_OBJ_MAX,
};

// �Z���A�N�^�[���\�[�X
enum{
  BV_RES_PLTT=0,
  BV_RES_CHR,
  BV_RES_CELL,
  BV_RES_COMMON_PLTT,
  BV_RES_COMMON_CHR,
  BV_RES_COMMON_CELL,
  BV_RES_MAX,
};


// BMPWIN
enum{
  BV_BMPWIN_M_NAME0=0,    // ���[�_�[��0
  BV_BMPWIN_M_NAME1,
  BV_BMPWIN_M_NAME2,
  BV_BMPWIN_M_NAME3,
  BV_BMPWIN_M_NAME4,
  BV_BMPWIN_M_NAME5,
  BV_BMPWIN_M_NAME6,
  BV_BMPWIN_M_NAME7,
  BV_BMPWIN_M_NAME8,
  BV_BMPWIN_M_NAME9,

  BV_BMPWIN_S_REKIDAI_NO_TOREINAA,  // �u�ꂫ�����̃g���[�i�[�v
  BV_BMPWIN_S_RANK,                 // �����N����
  BV_BMPWIN_S_TRAIN_NO,             // �g���C���i���o�[����
  BV_BMPWIN_S_NAME,         // ���ʃ��[�_�[��
  BV_BMPWIN_S_SUNDERUBASHO, // ���ʁu�Z��ł�ꏊ�v
  BV_BMPWIN_S_COUTRY,       // ��
  BV_BMPWIN_S_JIKOSHOUKAI,  // ���ʁu���ȏЉ�v
  BV_BMPWIN_S_PMS,          // �ȈՉ�b�̈�

  // �ȈՉ�b��PMS_DRAW�ŕ`��

  BV_BMPWIN_ID_MAX,  
};

// STRBUF�w��p
enum{
  BV_STR_REKIDAI=0,
  BV_STR_RANk,
  BV_STR_TRAINNO,
  BV_STR_SUNDERU,
  BV_STR_COUNTRY,
  BV_STR_LOCAL,
  BV_STR_JIKOSHOUKAI,
  BV_STR_MAX,
};

// �^�b�`�o�[���o���̋@�\
enum{
  FUNC_LEFT_PAGE=0,
  FUNC_RIGHT_PAGE,
  FUNC_END,
};

#define BV_CLACT_NUM        ( BV_OBJ_MAX ) // OBJ�o�^�ő吔
#define BV_PMSDRAW_OBJ_NUM  ( 2 )          // �ȈՉ�b�\���ɂ�����


// �T�u��ʂ�BMPWIN�̐�
#define SUB_BMPWIN_NUM  ( BV_BMPWIN_ID_MAX - BV_BMPWIN_S_REKIDAI_NO_TOREINAA)
// ���C����ʂ�BMPWIN�̐�
#define MAIN_BMPWIN_NUM ( BV_BMPWIN_M_NAME9+1 )
// �v���[�g�N���A�p���b�g�f�[�^
#define NAME_PLATE_CLEAR_COL  ( 0x0404 )


  
#endif //__BADGE_VIEW_DEF_H__