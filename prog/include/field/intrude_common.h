//==============================================================================
/**
 * @file    intrude_common.h
 * @brief   �N���S�̂Ŏg�p���鋤�ʃw�b�_
 * @author  matsuda
 * @date    2009.10.10(�y)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �萔��`
//==============================================================================
///�N���F�ʐM�ő�ڑ��l��
#define FIELD_COMM_MEMBER_MAX (4)

///�q�����Ă��Ȃ���������NETID
#define INTRUDE_NETID_NULL    (FIELD_COMM_MEMBER_MAX + 1)
///�p���X�G���A�ԍ������l(�܂��ʐM���ڑ�����Ă��Ȃ��āA���������Ԗڂ�������Ȃ����)
#define PALACE_AREA_NO_NULL     (128)

///�N���ł���X�̐�
#define INTRUDE_TOWN_MAX      (8)

///�X�̐苒�l   (WHITE ---- OCCUPY_TOWN_NEUTRALITY ---- OCCUPY_TOWN_BLACK)
enum{
  OCCUPY_TOWN_WHITE = 0,                            ///<�z���C�g�苒
  OCCUPY_TOWN_NEUTRALITY = 100,                     ///<����
  OCCUPY_TOWN_BLACK = OCCUPY_TOWN_NEUTRALITY * 2,   ///<�u���b�N�苒
};

//==============================================================================
//  �^��`
//==============================================================================
///_INTRUDE_COMM_SYS�\���̂̕s��`�|�C���^�^
typedef struct _INTRUDE_COMM_SYS * INTRUDE_COMM_SYS_PTR;
