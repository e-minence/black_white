//============================================================================================
/**
 * @file  item.h
 * @brief �A�C�e���f�[�^����
 * @author  Hiroyuki Nakamura
 * @date  05.09.06
 */
//============================================================================================

#pragma once

#include <gflib.h>
#include "item/itemsym.h"
#include "item/itemequip.h"


//============================================================================================
//  �萔��`
//============================================================================================
typedef struct _ITEM_DATA ITEMDATA;   // �A�C�e���f�[�^

// ����A�C�e��ID�i�A�C�R���擾�ȂǂɎg�p�j
#define ITEM_DUMMY_ID ( 0 )       // �_�~�[ID
#define ITEM_RETURN_ID  ( 0xffff )      // �߂�


// ���[���A�C�e���̃X�^�[�g�i���o�[
#define MAIL_START_ITEMNUM  (ITEM_HAZIMETEMEERU)
#define MAIL_END_ITEMNUM  (MAIL_START_ITEMNUM+ITEM_MAIL_MAX-1)

//�؂̎��E���₵�A�C�e���̃X�^�[�g�i���o�[��
#define NUTS_START_ITEMNUM  ( ITEM_KURABONOMI )   // �A�C�e���ԍ��̍ŏ��̖؂̎�
#define NUTS_END_ITEMNUM  ( NUTS_START_ITEMNUM+ITEM_NUTS_MAX-1 )  //�A�C�e���ԍ��̍Ō�̖؂̎�
#define COMPOST_START_ITEMNUM (ITEM_SUKUSUKUKOYASI)
#define ITEM_COMPOST_MAX    (ITEM_NEBANEBAKOYASI-ITEM_SUKUSUKUKOYASI+1)
#define ITEMNO_TO_KINOMINO(no)  ((no)-NUTS_START_ITEMNUM)
#define KINOMINO_TO_ITEMNO(no)  ((no)+NUTS_START_ITEMNUM)



// �C���f�b�N�X�擾��`
enum {
  ITEM_GET_DATA = 0,    // �A�C�e���f�[�^
  ITEM_GET_ICON_CGX,    // �A�C�R���L����
  ITEM_GET_ICON_PAL,    // �A�C�R���p���b�g
  ITEM_GET_AGB_NUM    // AGB�̃A�C�e���ԍ�
};

// �p�����[�^�擾��`
enum {
  ITEM_PRM_PRICE,         // ���l
  ITEM_PRM_EQUIP,         // ��������
  ITEM_PRM_ATTACK,        // �З�
  ITEM_PRM_EVENT,         // �d�v
  ITEM_PRM_CNV,         // �֗��{�^��
  ITEM_PRM_POCKET,        // �ۑ���i�|�P�b�g�ԍ��j
  ITEM_PRM_FIELD,         // field�@�\
  ITEM_PRM_BATTLE,        // battle�@�\
  ITEM_PRM_TUIBAMU_EFF,     // ���΂ތ���
  ITEM_PRM_NAGETUKERU_EFF,    // �Ȃ��������
  ITEM_PRM_NAGETUKERU_ATC,    // �Ȃ�����З�
  ITEM_PRM_SIZENNOMEGUMI_ATC,   // ������̂߂��݈З�
  ITEM_PRM_SIZENNOMEGUMI_TYPE,  // ������̂߂��݃^�C�v
  ITEM_PRM_BTL_POCKET,      // �퓬�ۑ���i�|�P�b�g�ԍ��j
  ITEM_PRM_W_TYPE,        // ���[�N�^�C�v
  ITEM_PRM_ITEM_TYPE,     // �A�C�e�����
  ITEM_PRM_ITEM_SPEND,    // ����邩

  ITEM_PRM_SLEEP_RCV,       // �����
  ITEM_PRM_POISON_RCV,      // �ŉ�
  ITEM_PRM_BURN_RCV,        // �Ώ���
  ITEM_PRM_ICE_RCV,       // �X��
  ITEM_PRM_PARALYZE_RCV,      // ��჉�
  ITEM_PRM_PANIC_RCV,       // ������
  ITEM_PRM_MEROMERO_RCV,      // ����������
  ITEM_PRM_ABILITY_GUARD,     // �\�̓K�[�h
  ITEM_PRM_DEATH_RCV,       // �m����
  ITEM_PRM_ALL_DEATH_RCV,     // �S���m����
  ITEM_PRM_LV_UP,         // ���x���A�b�v
  ITEM_PRM_EVOLUTION,       // �i��
  ITEM_PRM_ATTACK_UP,       // �U���̓A�b�v
  ITEM_PRM_DEFENCE_UP,      // �h��̓A�b�v
  ITEM_PRM_SP_ATTACK_UP,      // ���U�A�b�v
  ITEM_PRM_SP_DEFENCE_UP,     // ���h�A�b�v
  ITEM_PRM_AGILITY_UP,      // �f�����A�b�v
  ITEM_PRM_HIT_UP,        // �������A�b�v
  ITEM_PRM_CRITICAL_UP,     // �N���e�B�J�����A�b�v
  ITEM_PRM_PP_UP,         // PP�A�b�v
  ITEM_PRM_PP_3UP,        // PP�A�b�v�i�R�i�K�j
  ITEM_PRM_PP_RCV,        // PP��
  ITEM_PRM_ALL_PP_RCV,      // PP�񕜁i�S�Ă̋Z�j
  ITEM_PRM_HP_RCV,        // HP��
  ITEM_PRM_HP_EXP,        // HP�w�͒l�A�b�v
  ITEM_PRM_POWER_EXP,       // �U���w�͒l�A�b�v
  ITEM_PRM_DEFENCE_EXP,     // �h��w�͒l�A�b�v
  ITEM_PRM_AGILITY_EXP,     // �f�����w�͒l�A�b�v
  ITEM_PRM_SP_ATTACK_EXP,     // ���U�w�͒l�A�b�v
  ITEM_PRM_SP_DEFENCE_EXP,    // ���h�w�͒l�A�b�v
  ITEM_PRM_FRIEND1,       // �Ȃ��x�P
  ITEM_PRM_FRIEND2,       // �Ȃ��x�Q
  ITEM_PRM_FRIEND3,       // �Ȃ��x�R
  ITEM_PRM_HP_EXP_POINT,      // HP�w�͒l�̒l
  ITEM_PRM_POWER_EXP_POINT,   // �U���w�͒l�̒l
  ITEM_PRM_DEFENCE_EXP_POINT,   // �h��w�͒l�̒l
  ITEM_PRM_AGILITY_EXP_POINT,   // �f�����w�͒l�̒l
  ITEM_PRM_SP_ATTACK_EXP_POINT, // ���U�w�͒l�̒l
  ITEM_PRM_SP_DEFENCE_EXP_POINT,  // ���h�w�͒l�̒l
  ITEM_PRM_HP_RCV_POINT,      // HP�񕜒l�̒l
  ITEM_PRM_PP_RCV_POINT,      // pp�񕜒l�̒l
  ITEM_PRM_FRIEND1_POINT,     // �Ȃ��x1�̒l
  ITEM_PRM_FRIEND2_POINT,     // �Ȃ��x2�̒l
  ITEM_PRM_FRIEND3_POINT,     // �Ȃ��x3�̒l
};

// ���[�N�^�C�v�̎��
enum {
  ITEM_WKTYPE_DUMMY = 0,    // �_�~�[
  ITEM_WKTYPE_POKEUSE,    // �|�P�����Ɏg��
};


///�{�[��ID��`
typedef enum{
 BALLID_NULL,     // NULL
 BALLID_MASUTAABOORU, //01 �}�X�^�[�{�[��
 BALLID_HAIPAABOORU,  //02 �n�C�p�[�{�[��
 BALLID_SUUPAABOORU,  //03 �X�[�p�[�{�[��
 BALLID_MONSUTAABOORU,  //04 �����X�^�[�{�[��
 BALLID_SAFARIBOORU,  //05 �T�t�@���{�[��
 BALLID_NETTOBOORU,   //06 �l�b�g�{�[��
 BALLID_DAIBUBOORU,   //07 �_�C�u�{�[��
 BALLID_NESUTOBOORU,    //08 �l�X�g�{�[��
 BALLID_RIPIITOBOORU, //09 ���s�[�g�{�[��
 BALLID_TAIMAABOORU,    //10 �^�C�}�[�{�[��
 BALLID_GOOZYASUBOORU,  //11 �S�[�W���X�{�[��
 BALLID_PUREMIABOORU, //12 �v���~�A�{�[��
 BALLID_DAAKUBOORU,   //13 �_�[�N�{�[��
 BALLID_HIIRUBOORU,   //14 �q�[���{�[��
 BALLID_KUIKKUBOORU,    //15 �N�C�b�N�{�[��
 BALLID_PURESYASUBOORU, //16 �v���V���X�{�[��

 BALLID_SUPIIDOBOORU, //17 �X�s�[�h�{�[��
 BALLID_REBERUBOORU,    //18 ���x���{�[��
 BALLID_RUAABOORU,    //19 ���A�[�{�[��
 BALLID_HEBIIBOORU,   //20 �w�r�[�{�[��
 BALLID_RABURABUBOORU,  //21 ���u���u�{�[��
 BALLID_HURENDOBOORU, //22 �t�����h�{�[��
 BALLID_MUUNBOORU,    //23 ���[���{�[��
 BALLID_KONPEBOORU,   //24 �R���y�{�[��
 BALLID_MAX = BALLID_KONPEBOORU,  //�p�[�N�{�[���͎��̂̂Ȃ��{�[���Ȃ̂�ID�Ɋ܂߂܂���
}BALL_ID;


// HP �񕜗ʒ�` (���L�ȊO�Ȃ�A���̒l�������񕜂���j
enum {
  ITEM_RECOVER_HP_FULL = 255,   ///< �S��
  ITEM_RECOVER_HP_HALF = 254,   ///< MAX�̔�����
  ITEM_RECOVER_HP_QUOT = 253,   ///< MAX��1/4��
};

// PP �񕜗ʒ�` (���L�ȊO�Ȃ�A���̒l�������񕜂���j
enum {
  ITEM_RECOVER_PP_FULL = 127,   ///< �S��
};

//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �w��A�C�e�����w��ʒu�ɑ}��
 *
 * @param item  �A�C�e���f�[�^
 * @param pos1  �w��A�C�e���ʒu
 * @param pos2  �}���ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void ItemPutIn( void * item, u16 pos1, u16 pos2 );

//============================================================================================
//============================================================================================
//  �f�[�^�擾
//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �f�[�^�C���f�b�N�X�擾
 *
 * @param item    �A�C�e���ԍ�
 * @param type    �擾�f�[�^
 *
 * @return  �w��f�[�^
 *
 * @li  type = ITEM_GET_DATA    : �A�C�e���f�[�^�̃A�[�J�C�u�C���f�b�N�X
 * @li  type = ITEM_GET_ICON_CGX  : �A�C�R���L�����̃A�[�J�C�u�C���f�b�N�X
 * @li  type = ITEM_GET_ICON_PAL  : �A�C�R���p���b�g�̃A�[�J�C�u�C���f�b�N�X
 * @li  type = ITEM_GET_AGB_NUM   : AGB�̃A�C�e���ԍ�
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_GetIndex( u16 item, u16 type );

//--------------------------------------------------------------------------------------------
/**
 * AGB�̃A�C�e����DP�̃A�C�e���ɕϊ�
 *
 * @param agb   AGB�̃A�C�e���ԍ�
 *
 * @retval  "ITEM_DUMMY_ID = DP�ɂȂ��A�C�e��"
 * @retval  "ITEM_DUMMY_ID != DP�̃A�C�e���ԍ�"
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_CnvAgbItem( u16 agb );

//--------------------------------------------------------------
/**
 * @brief   �A�C�e���A�C�R���̃A�[�J�C�uID�擾
 *
 * @retval  �A�C�e���A�C�R���̃A�[�J�C�uID
 */
//--------------------------------------------------------------
extern u16 ITEM_GetIconArcID(void);

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���̃Z���A�[�J�C�u�C���f�b�N�X�擾
 *
 * @param none
 *
 * @return  �Z���A�[�J�C�u�C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_GetIconCell(void);

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���̃Z���A�j���A�[�J�C�u�C���f�b�N�X�擾
 *
 * @param none
 *
 * @return  �Z���A�j���A�[�J�C�u�C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_GetIconCellAnm(void);

//--------------------------------------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^���[�h
 *
 * @param item    �A�C�e���ԍ�
 * @param type    �擾�f�[�^
 * @param heap_id   �q�[�vID
 *
 * @return  �w��f�[�^
 *
 * @li  type = ITEM_GET_DATA    : �A�C�e���f�[�^
 * @li  type = ITEM_GET_ICON_CGX  : �A�C�R���̃L�����f�[�^
 * @li  type = ITEM_GET_ICON_PAL  : �A�C�R���̃p���b�g�f�[�^
 */
//--------------------------------------------------------------------------------------------
extern void * ITEM_GetItemArcData( u16 item, u16 type, HEAPID heap_id );

//--------------------------------------------------------------------------------------------
/**
 * ���O�擾
 *
 * @param buf     �i�[�ꏊ
 * @param item    �A�C�e���ԍ�
 * @param heap_id   �q�[�vID
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void ITEM_GetItemName( STRBUF* buf, u16 item, HEAPID heap_id );

//--------------------------------------------------------------------------------------------
/**
 * �����擾
 *
 * @param   buf     �A�C�e�����i�[��o�b�t�@
 * @param item    �A�C�e���ԍ�
 * @param heap_id   �q�[�vID
 *
 * @return  ����
 */
//--------------------------------------------------------------------------------------------
extern void ITEM_GetInfo( STRBUF * buf, u16 item, HEAPID heap_id );

//--------------------------------------------------------------------------------------------
/**
 * �p�����[�^�擾�i�A�C�e���ԍ��w��j
 *
 * @param item  �A�C�e���ԍ�
 * @param param �擾�p�����[�^��`
 * @param heap_id   �q�[�vID
 *
 * @return  �w��p�����[�^
 */
//--------------------------------------------------------------------------------------------
extern s32 ITEM_GetParam( u16 item, u16 param, HEAPID heap_id );

//--------------------------------------------------------------------------------------------
/**
 * �p�����[�^�擾�i�A�C�e���f�[�^�w��j
 *
 * @param item  �A�C�e���f�[�^
 * @param param �擾�p�����[�^��`
 *
 * @return  �w��p�����[�^
 */
//--------------------------------------------------------------------------------------------
extern s32 ITEM_GetBufParam( ITEMDATA * item, u16 param );

//--------------------------------------------------------------------------------------------
/**
 * @brief   �Z�}�V�����ǂ���
 *
 * @param   item  �A�C�e���ԍ�
 *
 * @retval  "TRUE = �Z�}�V��"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ITEM_CheckWazaMachine( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * �Z�}�V���Ŋo������Z���擾
 *
 * @param item  �A�C�e���ԍ�
 *
 * @return  �Z�ԍ�
 */
//--------------------------------------------------------------------------------------------
extern const u16 ITEM_GetWazaNo( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * @brief �I�����ꂽ�Z���|�P���������ɓK���Ȃ��Z���ǂ����i��`�Z���ǂ����j
 *
 * @param waza  �Z�ԍ�
 *
 * @retval  "TRUE = �����ɓK���Ȃ��Z(��`�Z"
 * @retval  "FALSE = �����ɓK����Z(��`�Z�ȊO"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ITEM_CheckHidenWaza( u16 waza );

//--------------------------------------------------------------------------------------------
/**
 * �Z�}�V���ԍ��擾
 *
 * @param item  �A�C�e���ԍ�
 *
 * @return  �Z�}�V���ԍ�
 *
 * @li  ��`�}�V���͒ʏ�̋Z�}�V���̌�ɃJ�E���g�����
 */
//--------------------------------------------------------------------------------------------
extern u8 ITEM_GetWazaMashineNo( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * ��`�}�V���ԍ��擾
 *
 * @param item  �A�C�e���ԍ�
 *
 * @return  ��`�}�V���ԍ�
 */
//--------------------------------------------------------------------------------------------
//extern u8 ITEM_GetHidenMashineNo( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e�������[�����ǂ������`�F�b�N
 *
 * @param item  �A�C�e���ԍ�
 *
 * @retval  "TRUE = ���[��"
 * @retval  "FALSE = ���[���ȊO"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ITEM_CheckMail( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���ԍ����烁�[���̃f�U�C�����擾
 *
 * @param item  �A�C�e���ԍ�
 *
 * @return  �f�U�C��ID
 */
//--------------------------------------------------------------------------------------------
extern u8 ITEM_GetMailDesign( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * ���[���̃f�U�C������A�C�e���ԍ����擾
 *
 * @param design    �f�U�C��ID
 *
 * @return  �A�C�e���ԍ�
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_MailNumGet( u8 design );

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e�����؂̎����ǂ������`�F�b�N
 *
 * @param item  �A�C�e���ԍ�
 *
 * @retval  "TRUE = �؂̎�"
 * @retval  "FALSE = �؂̎��ȊO"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ITEM_CheckNuts( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * �؂̎��̔ԍ����擾
 *
 * @param item  �A�C�e���ԍ�
 *
 * @return  �؂̎��ԍ�
 */
//--------------------------------------------------------------------------------------------
extern u8 ITEM_GetNutsNo( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * �w��ID�̖؂̎��̃A�C�e���ԍ����擾
 *
 * @param id    �؂̎���ID
 *
 * @return  �A�C�e���ԍ�
 */
//--------------------------------------------------------------------------------------------
extern u16 ITEM_GetNutsID( u8 id );

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���������򂩂ǂ������`�F�b�N
 *
 * @param item  �A�C�e���ԍ�
 *
 * @retval  "TRUE = ������"
 * @retval  "FALSE = ������"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ITEM_CheckKanpouyaku( u16 item );

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���e�[�u�������ׂă������ɓW�J
 *
 * @param heapID  �W�J���郁�����q�[�vID
 *
 * @retval  �m�ۂ����������̃A�h���X
 */
//--------------------------------------------------------------------------------------------
extern  void  *ITEM_LoadDataTable(HEAPID heapID);

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���e�[�u������C�ӂ̃A�h���X���擾
 *
 * @param item  �A�C�e���e�[�u��
 * @param index �Ƃ肾���C���f�b�N�X�i���o�[
 *
 * @retval  �擾�����A�h���X
 */
//--------------------------------------------------------------------------------------------
extern  ITEMDATA  *ITEM_GetDataPointer(ITEMDATA *item,u16 index);

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e�����|�P�����Ɏ��������邩
 *
 * @param item  �A�C�e���ԍ�
 *
 * @retval  "TRUE = ��"
 * @retval  "FALSE = �s��"
 *
 *  ��{�A����Œǉ����ꂽ����
 */
//--------------------------------------------------------------------------------------------
extern BOOL ITEM_CheckPokeAdd( u16 item );


//--------------------------------------------------------------------------------------------
/**
 * �Â��A�C�e���ō��͎g���Ă��Ȃ����ǂ�����������
 * @param item  �A�C�e���ԍ�
 * @retval  TRUE  = ������
 * @retval  FALSE = �����Ȃ�
 */
//--------------------------------------------------------------------------------------------
extern BOOL ITEM_CheckEnable( u16 item );


//--------------------------------------------------------------------------------------------
/**
 * �A�C�e��ID -> �{�[��ID �ϊ�
 *
 * @param   itemID    �A�C�e��ID
 *
 * @retval  BALL_ID   �{�[��ID�i�w�肳�ꂽ�A�C�e��ID���{�[���ȊO�̏ꍇ�ABALLID_NULL�j
 */
//--------------------------------------------------------------------------------------------
extern BALL_ID ITEM_GetBallID( u16 itemID );


