//=============================================================================================
/**
 * @file  btl_action.h
 * @brief �|�P����WB�o�g�� �v���C���[���I������������e�̎󂯓n���\���̒�`
 * @author  taya
 *
 * @date  2008.10.06  �쐬
 */
//=============================================================================================
#ifndef __BTL_ACTION_H__
#define __BTL_ACTION_H__

#include "waza_tool\wazadata.h"
#include "waza_tool\wazano_def.h"

#include "btl_common.h"

//--------------------------------------------------------------
/**
 *  �R�}���h�I��
 */
//--------------------------------------------------------------
typedef enum {
  BTL_ACTION_NULL=0,  ///< �������Ȃ��i����̃|�P�����I��҂����Ȃǁj

  BTL_ACTION_FIGHT,
  BTL_ACTION_ITEM,
  BTL_ACTION_CHANGE,
  BTL_ACTION_ESCAPE,
  BTL_ACTION_MOVE,
  BTL_ACTION_ROTATION,
  BTL_ACTION_SKIP,      ///< �����Ȃǂœ����Ȃ�

  BTL_ACTION_RECPLAY_TIMEOVER,   ///< �^��f�[�^�̎��Ԑ����ɂ��I��
  BTL_ACTION_RECPLAY_ERROR,

}BtlAction;


typedef union {

  u32 raw;

  struct {
    u32 cmd   : 4;
    u32 param : 28;
  }gen;

  struct {
    u32 cmd          : 4;
    u32 targetPos    : 3;
    u32 waza         : 16;
    u32 rot_dir      : 3;
    u32 wazaInfoFlag : 1; ///< ���U�������
    u32 _0           : 5;
  }fight;

  struct {
    u32 cmd       : 4;
    u32 targetIdx : 3;  ///< �Ώۃ|�P�����̃p�[�e�B���C���f�b�N�X
    u32 number    : 16; ///< �A�C�e��ID
    u32 param     : 8;  ///< �T�u�p�����[�^�iPP�񕜎��A�ǂ̃��U�ɓK�p���邩�A�Ȃǁj
    u32 _1        : 1;
  }item;

  struct {
    u32 cmd         : 4;
    u32 posIdx      : 3;  // ����ւ��ΏۈʒuID
    u32 memberIdx   : 3;  // �I�΂ꂽ�|�P�����̃p�[�e�B���C���f�b�N�X
    u32 depleteFlag : 1;  // ����ւ���|�P�������������Ȃ����Ƃ�ʒm����t���O
    u32 _2          : 21;
  }change;

  struct {
    u32 cmd     : 4;
    u32 _3      : 28;
  }escape;

  struct {
    u32 cmd     : 4;
    u32 _4      : 28;
  }move;

  struct {
    u32 cmd     : 4;
    u32 dir     : 3;
    u32 _5      : 25;
  }rotation;

}BTL_ACTION_PARAM;

// ���������A�N�V����
extern void BTL_ACTION_SetFightParam( BTL_ACTION_PARAM* p, WazaID waza, u8 targetPos );

extern void BTL_ACTION_FightParamToWazaInfoMode( BTL_ACTION_PARAM* p );

extern BOOL BTL_ACTION_IsWazaInfoMode( const BTL_ACTION_PARAM* p );

// �A�C�e�������A�N�V����
extern void BTL_ACTION_SetItemParam( BTL_ACTION_PARAM* p, u16 itemNumber, u8 targetIdx, u8 wazaIdx );

// ����ւ��|�P�����I���A�N�V�����i�I��Ώۂ͖���j
extern void BTL_ACTION_SetChangeBegin( BTL_ACTION_PARAM* p );

// ����ւ��|�P�����I���A�N�V�����i�ʏ�j
extern void BTL_ACTION_SetChangeParam( BTL_ACTION_PARAM* p, u8 posIdx, u8 memberIdx );

// ����ւ��|�P�����I���A�N�V�����i�����킦��|�P���������Ȃ��j
extern void BTL_ACTION_SetChangeDepleteParam( BTL_ACTION_PARAM* p );

// ���[�e�[�V����
extern void BTL_ACTION_SetRotation( BTL_ACTION_PARAM* p, BtlRotateDir dir );

extern BOOL BTL_ACTION_IsDeplete( const BTL_ACTION_PARAM* p );

// ������p�����[�^�ݒ�
extern void BTL_ACTION_SetEscapeParam( BTL_ACTION_PARAM* p );

// ���[�u�p�����[�^�ݒ�
extern void BTL_ACTION_SetMoveParam( BTL_ACTION_PARAM* p );

// NULL�i����ł���̂Ŕ�΂��j�p�����[�^�ݒ�
extern void BTL_ACTION_SetNULL( BTL_ACTION_PARAM* p );

extern void BTL_ACTION_SetSkip( BTL_ACTION_PARAM* p );

// �R�}���h�擾
extern BtlAction BTL_ACTION_GetAction( const BTL_ACTION_PARAM* p );

// ���UID�擾�i���������A�N�V�����ݒ莞�̂݁j
extern WazaID BTL_ACTION_GetWazaID( const BTL_ACTION_PARAM* act );

// �^��o�b�t�@���ӂ�ʒm�R�}���h�ʒm
extern void BTL_ACTION_SetRecPlayOver( BTL_ACTION_PARAM* act );

// �^��ǂݍ��ݎ��s�i�s���f�[�^�j�ʒm
extern void BTL_ACTION_SetRecPlayError( BTL_ACTION_PARAM* act );

#endif

