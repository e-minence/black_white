//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   event_field_proclink.h
 *  @brief  ���j���[������Ă΂��o�b�O�Ȃǂ̃T�u�v���b�N�̌q����C�x���g
 *  @author Toru=Nagihashi
 *  @date   2009.10.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "field/fieldmap_proc.h"
#include "item/itempocket_def.h"  //�o�b�O�̃y�[�W�w��̃f�t�@�C��
#include "/app/p_status.h"    //�X�e�[�^�X�̃y�[�W�w��̃f�t�@�C��

//=============================================================================
/**
 *          �萔�錾
*/
//=============================================================================
//-------------------------------------
/// EVENT_PROCLINK_PARAM->data�̎w�肵�Ȃ��l
//=====================================
#define EVENT_PROCLINK_DATA_NONE  (0xFFFFFFFF)

//-------------------------------------
/// �ŏ��ɌĂ΂��PROC
//=====================================
typedef enum
{
  //���j���[�ɂ����{�v���Z�X
  EVENT_PROCLINK_CALL_POKELIST,     //�|�P�������X�g  data��PSTATUS_PAGE_TYPE������ƃy�[�W�w��
  EVENT_PROCLINK_CALL_ZUKAN,        //�}��
  EVENT_PROCLINK_CALL_BAG,          //�o�b�O          data��itempoket_def.h�������ƃy�[�W�w��
  EVENT_PROCLINK_CALL_TRAINERCARD,  //�g���[�i�[�J�[�h
  EVENT_PROCLINK_CALL_REPORT,       //���|�[�g
  EVENT_PROCLINK_CALL_CONFIG,       //�R���t�B�O

  //����ȊO�̃v���Z�X
  EVENT_PROCLINK_CALL_STATUS,       //�X�e�[�^�X
  EVENT_PROCLINK_CALL_TOWNMAP,      //�^�E���}�b�v
  EVENT_PROCLINK_CALL_WIFINOTE,     //�Ƃ������蒠
  EVENT_PROCLINK_CALL_MAIL,        //���[�����
  EVENT_PROCLINK_CALL_EVOLUTION,    //�i�����
  EVENT_PROCLINK_CALL_BTLRECORDER,  //�o�g�����R�[�_�[

  EVENT_PROCLINK_CALL_MAX           //.c���Ŏg�p
} EVENT_PROCLINK_CALL_TYPE;

//-------------------------------------
/// ����
//=====================================
typedef enum
{ 
  EVENT_PROCLINK_RESULT_RETURN,     //���j���[�ɖ߂�
  EVENT_PROCLINK_RESULT_EXIT,       //���j���[�������ĕ������Ԃ܂Ŗ߂�
  EVENT_PROCLINK_RESULT_SKILL,      //���j���[�𔲂��ăt�B�[���h�Z���g��
  EVENT_PROCLINK_RESULT_ITEM,       //���j���[�𔲂��ăA�C�e�����g��
} EVENT_PROCLINK_RESULT;

//=============================================================================
/**
 *          �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���j���[��Y�{�^�����X�g�Ȃǂ�
//    �J����������肷��R�[���o�b�N
//=====================================
typedef struct _EVENT_PROCLINK_PARAM EVENT_PROCLINK_PARAM;
typedef void (*EVENT_PROCLINK_MENUOPEN_FUNC)( const EVENT_PROCLINK_PARAM *param, void *wk_adrs );
typedef void (*EVENT_PROCLINK_MENUCLOSE_FUNC)( const EVENT_PROCLINK_PARAM *param, void *wk_adrs );

//-------------------------------------
/// �C�x���g�̈���
//    PROC�Ɠ����悤��Alloc���ĕێ����Ă��������B
//    �߂�l�������Ă��܂�
//=====================================
struct _EVENT_PROCLINK_PARAM
{
  //�V�X�e��
  GAMESYS_WORK              *gsys;      //[in ]�Q�[���V�X�e��
  FIELDMAP_WORK             *field_wk;  //[in ]�t�B�[���h���[�N
  GMEVENT                   *event;     //[in ]�e�C�x���g

  //�Ăԃv���b�N
  EVENT_PROCLINK_CALL_TYPE  call;       //[in ]�ŏ��ɌĂ΂��PROC
  u32                       data;       //[in ]call�ɑΉ��������

  //�R�[���o�b�N
  EVENT_PROCLINK_MENUOPEN_FUNC  open_func;    //[in ]���j���[�����J���֐�   (NULL�ōs��Ȃ�)
  EVENT_PROCLINK_MENUCLOSE_FUNC close_func;   //[in ]���j���[�������֐� (NULL�ōs��Ȃ�)
  void *wk_adrs;                              //[in ]�R�[���o�b�N�ɓn������

  EVENT_PROCLINK_RESULT     result;     //[out]�I������
  u32 select_param;   //[out]�I�����ꂽ�Z�A�A�C�e�� FLDSKILL_IDX or EVENT_ITEMUSE_CALL_TYPE���Ԃ�
  u32 select_poke;    //[out]�I�����ꂽ�|�P����
  u32 zoneID;         //[out]����ԗp�]�[��ID
  int grid_x;         //[out]����ԗp��ѐ�w��X
  int grid_y;         //[out]����ԗp��ѐ�w��Y
  int grid_z;         //[out]����ԗp��ѐ�w��Z
};

//=============================================================================
/**
 *          �v���g�^�C�v�錾
*/
//=============================================================================
extern GMEVENT * EVENT_ProcLink( EVENT_PROCLINK_PARAM *param, HEAPID heapID );
