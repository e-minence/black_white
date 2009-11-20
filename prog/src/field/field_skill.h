//======================================================================
/**
 * @file  field_skill.h
 * @brief  �t�B�[���h�Z�����i��`�Z�Ȃǁj
 * @author  Hiroyuki Nakamura
 * @date  2005.12.01
 */
//======================================================================
#pragma once

#include <gflib.h>
#include "system/gfl_use.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "gamesystem/playerwork.h"

#include "field/fieldmap_proc.h"
#include "fldmmdl.h"
#include "map_attr.h"

//======================================================================
//  define
//======================================================================
//--------------------------------------------------------------
/// FLDSKILL_IDX �Z�C���f�b�N�X
//--------------------------------------------------------------
typedef enum
{
  FLDSKILL_IDX_IAIGIRI = 0,    // ����������
  FLDSKILL_IDX_NAMINORI,    // �Ȃ݂̂�
  FLDSKILL_IDX_TAKINOBORI,    // �����̂ڂ�
  FLDSKILL_IDX_KAIRIKI,    // �����肫
  FLDSKILL_IDX_SORAWOTOBU,    // ������Ƃ�
  FLDSKILL_IDX_KIRIBARAI,    // ����΂炢
  FLDSKILL_IDX_IWAKUDAKI,    // ���킭����
  FLDSKILL_IDX_ROCKCLIMB,    // ���b�N�N���C��
  FLDSKILL_IDX_FLASH,    // �t���b�V��
  FLDSKILL_IDX_TELEPORT,    // �e���|�[�g
  FLDSKILL_IDX_ANAWOHORU, // ���Ȃ��ق�
  FLDSKILL_IDX_AMAIKAORI,    // ���܂�������
  FLDSKILL_IDX_OSYABERI,    // ������ׂ�
  FLDSKILL_IDX_MAX, //�ő�
}FLDSKILL_IDX;

//--------------------------------------------------------------
/// FLDSKILL_PRM �Z�g�p�֐��擾�p�����[�^
//--------------------------------------------------------------
typedef enum
{
  FLDSKILL_PRM_USEFUNC = 0,  //�g�p�֐�
  FLDSKILL_PRM_CHECKFUNC  //�`�F�b�N�֐�
}FIEDLSKILL_PRM;

//--------------------------------------------------------------
/// FLDSKILL_RET �g�p�`�F�b�N�̖߂�l
//--------------------------------------------------------------
typedef enum
{
  FLDSKILL_RET_USE_OK = 0,  // �g�p�\
  FLDSKILL_RET_USE_NG,    // �g�p�s�i�����ł͎g���܂���j
  FLDSKILL_RET_NO_BADGE,    // �g�p�s�E�o�b�W�Ȃ�
  FLDSKILL_RET_COMPANION,    // �g�p�s�E�A�����
  FLDSKILL_RET_PLAYER_SWIM,    // �g�p�s�E�Ȃ݂̂蒆
}FLDSKILL_RET;

//======================================================================
//  struct
//======================================================================
///�Z�g�p���[�N�ւ̕s���S�^�|�C���^
typedef struct _TAG_FLDSKILL_USE_HEADER FLDSKILL_USE_HEADER;

///�Z�g�p�`�F�b�N���[�N�ւ̕s���S�^�|�C���^
typedef struct _TAG_FLDSKILL_CHECK_WORK FLDSKILL_CHECK_WORK;

///�Z�g�p�`�F�b�N�֐�
typedef FLDSKILL_RET (*FLDSKILL_CHECK_FUNC)(const FLDSKILL_CHECK_WORK*);

///�Z�g�p�֐�
typedef GMEVENT* (*FLDSKILL_USE_FUNC)(const FLDSKILL_USE_HEADER*,const FLDSKILL_CHECK_WORK*);

//--------------------------------------------------------------
/// FLDSKILL_USE_WORK �Z�g�p�w�b�_�[
//--------------------------------------------------------------
struct _TAG_FLDSKILL_USE_HEADER
{
  u16 poke_pos; //�Z���g�p����莝���|�P�����ʒu�ԍ�
  u16 use_wazano; //�g�p����Z�ԍ�
  u32 zoneID;
  u32 GridX;
  u32 GridY;
  u32 GridZ;
};

//--------------------------------------------------------------
/// FLDSKILL_CHECK_WORK �Z�g�p�`�F�b�N���[�N
//--------------------------------------------------------------
struct _TAG_FLDSKILL_CHECK_WORK
{
  u16 zone_id;
  u16 enable_skill;
  PLAYER_MOVE_FORM moveform;
  GAMESYS_WORK *gsys;
  MMDL *front_mmdl;
  FIELDMAP_WORK *fieldmap;
};

//======================================================================
//  proto
//======================================================================
//extern FLDSKILL_CHECK_FUNC FLDSKILL_GetCheckFunc( FLDSKILL_IDX );
//extern FLDSKILL_USE_FUNC FLDSKILL_GetUseFunc( FLDSKILL_IDX idx );
extern void FLDSKILL_InitCheckWork(
    FIELDMAP_WORK *fieldmap, FLDSKILL_CHECK_WORK *scwk );
extern FLDSKILL_RET FLDSKILL_CheckUseSkill(
    FLDSKILL_IDX idx, FLDSKILL_CHECK_WORK *scwk );
extern void FLDSKILL_InitUseHeader( FLDSKILL_USE_HEADER *head,
    u16 poke_pos, u16 use_wazano,
    u32 zoneID, u32 inGridX, u32 inGridY, u32 inGridZ);
extern GMEVENT * FLDSKILL_UseSkill( FLDSKILL_IDX idx,
    const FLDSKILL_USE_HEADER *head, const FLDSKILL_CHECK_WORK *scwk );
