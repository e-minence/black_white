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
#include "savedata/questionnaire_save.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�V�X�e�����O����
#define GAMEBEACON_SYSTEM_LOG_MAX       (30)

///���烁�b�Z�[�W�̕�����(EOM����)
#define GAMEBEACON_THANKYOU_MESSAGE_LEN   (9)
///�ꌾ���b�Z�[�W�̕�����(EOM����)
#define GAMEBEACON_FREEWORD_MESSAGE_LEN   (GAMEBEACON_THANKYOU_MESSAGE_LEN)
///���ȏЉ�b�Z�[�W�̕�����(!! EOM���� !!)
#define GAMEBEACON_SELFINTRODUCTION_MESSAGE_LEN   (8)


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
  u16 details_no:8;                 ///<�ڍ�No(GAMEBEACON_DETAILS_NO_xxx)
  
  //�ȈՉ�b�FGAMEBEACON_INFO�ɒ��ڒu���������A�p�f�B���O�̊֌W�ł����ɔz�u
  //�����o�S�Ă�u16�ɂ��鎖��2byte�A���C�����g�̍��W�ɔz�u�ł���悤�ɂ��Ă���
  u16 sentence_type:3;
  u16 sentence_id:5;
  PMS_WORD word[PMS_WORD_MAX];
  
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
  u16 monsno;   ///<�|�P�����ԍ� ���{���Ȃ�normal�̒��ɓ����Ă�����̂����Aaction_no��union�̊Ԃ�
                                 //�A���C�����g���������Ă���̂ŁAmonsno���A���C�����g�ɓ��Ă͂߁A
                                 //�ő�T�C�Y�ł���normal���k�����A���p�̎��̂̃A���C�����g���j�~
                                 //���A���ʓI�ɍ\���̑S�̂̃T�C�Y���k�߂Ă���

  //action_no�ɂ���ăZ�b�g�������̂��ς��
  union{
    u16 tr_no;                              ///<�g���[�i�[�ԍ�
    u16 itemno;                             ///<�A�C�e���ԍ�
    u16 wazano;                             ///<�Z�ԍ�
    u16 hour;                               ///<��
    u32 thankyou_count;                     ///<������󂯂���
    u32 victory_count;                      ///<�A����
    u8 trial_house_rank;                    ///<�g���C�A���n�E�X�̃����N
    u16 gpower_id;                          ///<G�p���[ID
    struct{ //�ʏ�
      STRCODE nickname[MONS_NAME_SIZE];       ///<�|�P������(�j�b�N�l�[��) !! EOM���� !!
    }normal;
    struct{ //�z�z�p
      u16 monsno;                           ///<�|�P�����ԍ�
      u16 itemno;                           ///<�A�C�e���ԍ�
      u32 magic_key;                        ///<�}�W�b�N�L�[
    }distribution;
    struct{
      STRCODE thankyou_message[GAMEBEACON_THANKYOU_MESSAGE_LEN]; ///���烁�b�Z�[�W
      u16 target_trainer_id;                ///<�Ώۑ���̃g���[�i�[ID(����16�r�b�g)
    }thanks;
    STRCODE freeword_message[GAMEBEACON_FREEWORD_MESSAGE_LEN]; ///�ꌾ���b�Z�[�W
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
  u8 nation;                               ///<���R�[�h
  u8 area;                                 ///<�n��R�[�h
  u8 g_power_id:7;                         ///<�������Ă���G�p���[ID(GPOWER_ID_xxx)
  u8 sex:1;                                ///<����
  
  u32 suretigai_count:17;                  ///<����Ⴂ�l��
  u32 zone_id:10;                          ///<���ݒn
  u32 language:5;                          ///<PM_LANG
  
  u32 thanks_recv_count:17;                ///<������󂯂���
  u32 send_counter:5;                      ///<���MNo(�����f�[�^�̖�������Ɏg�p)
  u32 townmap_root_zone_id:10;             ///<�^�E���}�b�v�ł̍��W�e�[�u��Index

  u32 pm_version:6;
  u32 trainer_view:3;                      ///<���j�I�����[���ł̌�����
  u32 research_team_rank:3;                ///<�����������N(RESEARCH_TEAM_RANK)
  u32 favorite_color_index:4;              ///<�{�̏��̐F(Index)
  u32 play_hour:10;                        ///<�v���C���ԁF��(MAX999)
  u32 play_min:6;                          ///<�v���C���ԁF��(MAX59)

  u16 trainer_id;                          ///<�g���[�i�[ID(����16�r�b�g)
  STRCODE name[PERSON_NAME_SIZE];          ///<�g���[�i�[��(!! EOM���� !!)

  STRCODE self_introduction[GAMEBEACON_SELFINTRODUCTION_MESSAGE_LEN]; ///<���ȏЉ�b�Z�[�W

  GAMEBEACON_INFO_ACTION action;           ///<�s���p�����[�^   28(0)
  GAMEBEACON_INFO_DETAILS details;         ///<�ڍ׏��         10(+2)
  QUESTIONNAIRE_ANSWER_WORK question_answer;  ///<�A���P�[�g��    10
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
