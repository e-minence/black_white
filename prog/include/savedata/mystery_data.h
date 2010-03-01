//============================================================================================
/**
 * @file	  mystery_data.h
 * @date	  2009.12.04
 * @author	k.ohno
 * @brief	  �ӂ����ʐM�p�Z�[�u�f�[�^�p�w�b�_
 */
//============================================================================================

#pragma once

#include "include/buflen.h"
#include "savedata/save_control.h"
#include "poke_tool/poke_tool.h"
#include "savedata/regulation.h"

//============================================================================================
//
//			��`
//
//============================================================================================

//------------------------------------------------------------------
///		���蕨�f�[�^�̌�
//------------------------------------------------------------------
#define	GIFT_DATA_MAX		(12)	//�J�[�h12
#define MYSTERY_DATA_MAX_EVENT		(2048)  //�C�x���g�ő�

//------------------------------------------------------------------
///		�J�[�h�֌W�̃T�C�Y��`
//------------------------------------------------------------------
#define GIFT_DATA_CARD_TITLE_MAX	(36)    //�z�z�^�C�g��

#define GIFT_DATA_CARD_TEXT_MAX		(252)   //�����e�L�X�g


//------------------------------------------------------------------
///		���蕨�f�[�^�\����
//------------------------------------------------------------------
#define MYSTERYGIFT_TYPE_NONE		(0)	// ��������
#define MYSTERYGIFT_TYPE_POKEMON	(1)	// �|�P����
#define MYSTERYGIFT_TYPE_ITEM		(2)	// �ǂ���
#define MYSTERYGIFT_TYPE_POWER		(3)	// �p���[
#define MYSTERYGIFT_TYPE_RULE  (4)   //�폜�\��
#define MYSTERYGIFT_TYPE_NUTSET  (5) //�폜�\��
#define MYSTERYGIFT_TYPE_MAX	(6)	// ����

#define MYSTERYGIFT_TYPE_CLEAR		(255)	// �ӂ����̈�̋����N���A


//------------------------------------------------------------------
///		��M����
//------------------------------------------------------------------
#define MYSTERYGIFT_DATE_MASK_YEAR    0xFFFF
#define MYSTERYGIFT_DATE_MASK_MONTH   0xFF
#define MYSTERYGIFT_DATE_MASK_DAY     0xFF
#define MYSTERYGIFT_DATE_SHIFT_YEAR   16
#define MYSTERYGIFT_DATE_SHIFT_MONTH  8
#define MYSTERYGIFT_DATE_SHIFT_DAY    0


// �T�C�Y�Œ�p�\����
typedef struct {
  u8 data[256];
} GIFT_PRESENT_ALL;

// �|�P����
typedef struct {
	u32	id_no;			// �e��ID
  u32 version;    // ROM�o�[�W����
  u32 rnd;        // �ő̗���
  u16 ribbon_no;        //���{���r�b�g 16�{
  u16 get_ball;  //�߂܂����{�[��
  u16 item;      //��������
  u16 waza1;     //�Z1
  u16 waza2;     //�Z2
  u16 waza3;     //�Z3
  u16 waza4;     //�Z4
  u16 mons_no;   //�����X�^�[No
  u8 form_no;    //�t�H����No
  u8 country_code;  //���R�[�h
	STRCODE	nickname[MONS_NAME_SIZE+EOM_SIZE];	//16h	�j�b�N�l�[��(MONS_NAME_SIZE=10)+(EOM_SIZE=1)=11  �ŏ���EOM�Ȃ�f�t�H���g
  u8 seikaku;          //���i
  u8 sex;              //����
  u8 speabino;         //�Ƃ�����
  u8 rare;             //���A�ɂ��邩�ǂ��� 0=�����_�����A�Ȃ�  1=�����_�� 2=���A
  u8 get_place;        //�߂܂����ꏊ
  u8 birth_place;      //���܂ꂽ�ꏊ
  u8 get_level;        //�߂܂���LEVEL
  u8 style;                    //�������悳
  u8 beautiful;                //��������
  u8 cute;                     //���킢��
  u8 clever;                   //��������
  u8 strong;                   //�����܂���
  u8 fur;                      //�щ�
  u8 hp_rnd;               //HP����
  u8 pow_rnd;              //�U���͗���
  u8 def_rnd;              //�h��͗���
  u8 agi_rnd;              //�f��������
  u8 spepow_rnd;             //���U����
  u8 spedef_rnd;             //���h����
  STRCODE	oyaname[PERSON_NAME_SIZE+EOM_SIZE];	//10h	�e�̖��O(PERSON_NAME_SIZE=7)+(EOM_SIZE_=1)=8*2(STRCODE=u16)
  u8 oyasex;      //�e�̐��� 0 1 2 
  u8 level;     //�|�P�������x��
  u8 egg;       //�^�}�S���ǂ��� TRUE�����܂�
  u8 dummy;
} GIFT_PRESENT_POKEMON;


// �ǂ���
typedef struct {
  int itemNo;        ///�ǂ����ԍ�
  int movieflag;    ///�f��z�M���ǂ���
} GIFT_PRESENT_ITEM;


// �f�p���[
typedef struct {
  int type;        ///�J������p���[�^�C�v����
} GIFT_PRESENT_POWER;



// �ӂ����Ȃ�������̋����N���A
typedef struct {
  int dummy;
} GIFT_PRESENT_REMOVE;

typedef union {
  GIFT_PRESENT_POKEMON	pokemon;
  GIFT_PRESENT_ITEM		  item;
  GIFT_PRESENT_POWER		gpower;
  GIFT_PRESENT_REMOVE		remove;
} GIFT_PRESENT;



// �ۑ�����f�[�^
typedef struct{
  GIFT_PRESENT data;
  STRCODE event_name[GIFT_DATA_CARD_TITLE_MAX+EOM_SIZE];	// �C�x���g�^�C�g��
  s32 recv_date;				// ��M��������
  u16 event_id;					// �C�x���g�h�c(�ő�2048���܂�)
  u8 card_message;      //  �J�[�h�̃��b�Z�[�W�^�C�v
  u8 gift_type;         // �����Ă���^�C�v
  u8 only_one_flag:1;	  // �P�x������M�t���O(0..���x�ł���M�\ 1..�P��̂�)
  u8 have:1;				    // �f�[�^���󂯎�����ꍇ 1
  u8 padding:6;
  u8 padding2;
  u32 dummy[4];            //�\���f�[�^16byte
  u16 crc;             //CRC�f�[�^
} GIFT_PACK_DATA;


// �T�[�o���瑗����f�[�^
typedef struct{
  GIFT_PACK_DATA data;
  u32 version;					// �Ώۃo�[�W����(�O�̏ꍇ�͐��������Ŕz�z)
  STRCODE event_text[GIFT_DATA_CARD_TEXT_MAX+EOM_SIZE];	// �����e�L�X�g
} DOWNLOAD_GIFT_DATA;




//------------------------------------------------------------------
/**
 * @brief	�ӂ����Z�[�u�f�[�^�ւ̕s���S�^��`
 */
//------------------------------------------------------------------
typedef struct MYSTERY_DATA MYSTERY_DATA;


//============================================================================================
//
//			�O���Q��
//
//============================================================================================
//------------------------------------------------------------------
//�f�[�^�T�C�Y�擾
//------------------------------------------------------------------
extern int MYSTERYDATA_GetWorkSize(void);

//------------------------------------------------------------------
//����������
//------------------------------------------------------------------
extern void MYSTERYDATA_Init(MYSTERY_DATA * fd);

//------------------------------------------------------------------
/**
 * @brief	�ӂ����f�[�^�ւ̃|�C���^�擾
 */
//------------------------------------------------------------------
extern MYSTERY_DATA * SaveData_GetMysteryData(SAVE_CONTROL_WORK * sv);


//------------------------------------------------------------------
/// �J�[�h�f�[�^�̎擾
//------------------------------------------------------------------
extern GIFT_PACK_DATA *MYSTERYDATA_GetCardData(MYSTERY_DATA *fd, u32 cardindex);
//------------------------------------------------------------------
/// �J�[�h�f�[�^���Z�[�u�f�[�^�o�^
//------------------------------------------------------------------
extern BOOL MYSTERYDATA_SetCardData(MYSTERY_DATA *fd, const void *p);
//------------------------------------------------------------------
/// �J�[�h�f�[�^�𖕏�����
//------------------------------------------------------------------
extern void MYSTERYDATA_RemoveCardData(MYSTERY_DATA *fd, u32 cardindex);
//------------------------------------------------------------------
/// �J�[�h�f�[�^���Z�[�u�ł��邩�`�F�b�N
//------------------------------------------------------------------
extern BOOL MYSTERYDATA_CheckCardDataSpace(MYSTERY_DATA *fd);
//------------------------------------------------------------------
/// �J�[�h�f�[�^�����݂��邩�Ԃ�
//------------------------------------------------------------------
extern BOOL MYSTERYDATA_IsExistsCard(const MYSTERY_DATA * fd, u32 cardindex);
//------------------------------------------------------------------
/// �Z�[�u�f�[�^���ɃJ�[�h�f�[�^�����݂��邩�Ԃ�
//------------------------------------------------------------------
extern BOOL MYSTERYDATA_IsExistsCardAll(const MYSTERY_DATA *fd);
//------------------------------------------------------------------
//	 �w��̃J�[�h����f�[�^���󂯎��ς݂��Ԃ�
//------------------------------------------------------------------
extern BOOL MYSTERYDATA_IsHavePresent(const MYSTERY_DATA * fd, u32 cardindex);
//------------------------------------------------------------------
//   �w��̃J�[�h����f�[�^���󂯎�������Ƃɂ���
//------------------------------------------------------------------
extern void MYSTERYDATA_SetHavePresent(MYSTERY_DATA * fd, u32 index);
//------------------------------------------------------------------
/// �w��̃C�x���g�͂��łɂ���������Ԃ�
//------------------------------------------------------------------
extern BOOL MYSTERYDATA_IsEventRecvFlag(MYSTERY_DATA * fd, u32 eventno);
//------------------------------------------------------------------
/// �w��̃C�x���g���������t���O�𗧂Ă�
//------------------------------------------------------------------
extern void MYSTERYDATA_SetEventRecvFlag(MYSTERY_DATA * fd, u32 eventno);

//------------------------------------------------------------------
/// ��M�����@�N�����󂯎��
//------------------------------------------------------------------
inline s32 MYSTERYDATA_GetYear( s32 recv_date )
{ 
  return (recv_date >> MYSTERYGIFT_DATE_SHIFT_YEAR) & MYSTERYGIFT_DATE_MASK_YEAR;
}
inline s32 MYSTERYDATA_GetMonth( s32 recv_date )
{ 
  return (recv_date >> MYSTERYGIFT_DATE_SHIFT_MONTH) & MYSTERYGIFT_DATE_MASK_MONTH;
}
inline s32 MYSTERYDATA_GetDay( s32 recv_date )
{ 
  return (recv_date >> MYSTERYGIFT_DATE_SHIFT_DAY) & MYSTERYGIFT_DATE_MASK_DAY;
}

//------------------------------------------------------------------
//	 �w��̃J�[�h�����ւ���
//------------------------------------------------------------------
extern void MYSTERYDATA_SwapCard( MYSTERY_DATA * fd, u32 cardindex1, u32 cardindex2 );

//------------------------------------------------------------------
/// �f�o�b�O�p�Ƀ|�P�����f�[�^���Z�b�g����֐�
//------------------------------------------------------------------
extern void MYSTERYDATA_DebugSetPokemon(void);

