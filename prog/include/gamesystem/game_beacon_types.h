//==============================================================================
/**
 * @file    game_beacon_types.h
 * @brief   ����Ⴂ�̃r�[�R�����
 * @author  matsuda
 * @date    2010.01.19(��)
 */
//==============================================================================
#pragma once

#include "gamesystem/game_beacon.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�V�X�e�����O����
#define GAMEBEACON_SYSTEM_LOG_MAX       (30)

///���烁�b�Z�[�W�̕�����(EOM����)
#define GAMEBEACON_THANKYOU_MESSAGE_LEN   (11)
///���ȏЉ�b�Z�[�W�̕�����(!! EOM���� !!)
#define GAMEBEACON_SELFINTRODUCTION_MESSAGE_LEN   (10)


//==============================================================================
//  �\���̒�`
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �ڍ׃p�����[�^
 *
 * �t�B�[���h�r�[�R���ɍ�����ׁA�O�����J���Ă��܂��B
 * ��L�ȊO�ł̒��ڎQ�Ƃ͍T���Ă�������
 */
//--------------------------------------------------------------
typedef struct{
  u8 details_no;                 ///<�ڍ�No(GAMEBEACON_DETAILS_NO_xxx)
  u8 padding;
  union{
    u16 battle_trainer;          ///<�퓬����̃g���[�i�[�R�[�h
    u16 battle_monsno;           ///<�퓬����̃|�P����No
  };
}GAMEBEACON_INFO_DETAILS;

//--------------------------------------------------------------
/**
 * @brief   �s���p�����[�^
 *
 * �t�B�[���h�r�[�R���ɍ�����ׁA�O�����J���Ă��܂��B
 * ��L�ȊO�ł̒��ڎQ�Ƃ͍T���Ă�������
 */
//--------------------------------------------------------------
typedef struct{
  u16 action_no;                            ///<�s��No
  u16 padding;
  
  //action_no�ɂ���ăZ�b�g�������̂��ς��
  union{
    u8 work[24];
    u16 tr_no;                              ///<�g���[�i�[�ԍ�
    u16 itemno;                             ///<�A�C�e���ԍ�
    u32 target_trainer_id;                  ///<�Ώۑ���̃g���[�i�[ID
    STRCODE nickname[BUFLEN_POKEMON_NAME];  ///<�|�P������(�j�b�N�l�[��)
    STRCODE thankyou_message[GAMEBEACON_THANKYOU_MESSAGE_LEN]; ///���烁�b�Z�[�W
  };
}GAMEBEACON_INFO_ACTION;

//==================================================================
/**
 * @brief   ����M�����r�[�R���p�����[�^
 *
 * �t�B�[���h�r�[�R���ɍ�����ׁA�O�����J���Ă��܂��B
 * ��L�ȊO�ł̒��ڎQ�Ƃ͍T���Ă�������
 */
//==================================================================
struct _GAMEBEACON_INFO{
  u8 version_bit;                          ///<��M�\��PM_VERSION��bit�w��
  u8 send_counter;                         ///<���MNo(�����f�[�^�̖�������Ɏg�p)
  u8 nation;                               ///<���R�[�h
  u8 area;                                 ///<�n��R�[�h
  
  u32 zone_id:16;                          ///<���ݒn
  u32 g_power_id:8;                        ///<�������Ă���G�p���[ID(GPOWER_ID_xxx)
  u32 pm_version:7;
  u32 sex:1;                               ///<����

  u32 suretigai_count:17;                  ///<����Ⴂ�l��
  u32 language:5;                          ///<PM_LANG
  u32 trainer_view:3;                      ///<���j�I�����[���ł̌�����
  u32           :7;

  u32 favorite_color:15;                   ///<�{�̏��̐F(GXRgb)
  u32 thanks_recv_count:17;                ///<������󂯂���
  
  STRCODE name[PERSON_NAME_SIZE + EOM_SIZE];         ///<�g���[�i�[��
  u32 trainer_id;                          ///<�g���[�i�[ID
  
  STRCODE self_introduction[GAMEBEACON_SELFINTRODUCTION_MESSAGE_LEN]; ///<���ȏЉ�b�Z�[�W
  
  GAMEBEACON_INFO_DETAILS details;         ///<�ڍ׏��
  GAMEBEACON_INFO_ACTION action;           ///<�s���p�����[�^
};


//==================================================================
/**
 * @brief   ����M�����r�[�R���p�����[�^�̃��O�e�[�u��
 *
 * �t�B�[���h�r�[�R���ɍ�����ׁA�O�����J���Ă��܂��B
 * ��L�ȊO�ł̒��ڎQ�Ƃ͍T���Ă�������
 */
//==================================================================
struct _GAMEBEACON_INFO_TBL{
  struct _GAMEBEACON_INFO info[GAMEBEACON_INFO_TBL_MAX];
  u16 time[GAMEBEACON_INFO_TBL_MAX];    ///<��M����(���8bit�F��(0�`23)�A����8bit�F��(0�`59))
  
  u8  ring_top;   //�����O�o�b�t�@����p ���X�g�̃g�b�vindex��ێ�
  u8  entry_num;  //�����O�o�^��
};
