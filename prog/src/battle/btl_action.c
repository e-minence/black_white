//=============================================================================================
/**
 * @file  btl_action.c
 * @brief �|�P����WB�o�g�� �v���C���[���I������������e�̎󂯓n���\���̒�`
 * @author  taya
 *
 * @date  2010.05.11  �쐬
 */
//=============================================================================================

#include "btl_action.h"

// ���������A�N�V�����ݒ�
void BTL_ACTION_SetFightParam( BTL_ACTION_PARAM* p, WazaID waza, u8 targetPos )
{
  p->raw = 0;
  p->fight.cmd = BTL_ACTION_FIGHT;
  p->fight.targetPos = targetPos;
  p->fight.waza = waza;
}
void BTL_ACTION_ChangeFightTargetPos( BTL_ACTION_PARAM* p, u8 nextTargetPos )
{
  if( p->gen.cmd == BTL_ACTION_FIGHT )
  {
    if( nextTargetPos != BTL_POS_NULL ){
      p->fight.targetPos = nextTargetPos;
    }
  }
}

// ���������A�N�V���� -> ���U�������[�h��
void BTL_ACTION_FightParamToWazaInfoMode( BTL_ACTION_PARAM* p )
{
  if( p->gen.cmd == BTL_ACTION_FIGHT ){
    p->fight.wazaInfoFlag = TRUE;
  }
}
// ���U�������[�h�ݒ肩�ǂ�������
BOOL BTL_ACTION_IsWazaInfoMode( const BTL_ACTION_PARAM* p )
{
  if( p->gen.cmd == BTL_ACTION_FIGHT )
  {
    return p->fight.wazaInfoFlag;
  }
  return FALSE;
}

/**
 *  ���UID�擾�i���������A�N�V�����ݒ莞�̂݁j
 */
WazaID BTL_ACTION_GetWazaID( const BTL_ACTION_PARAM* act )
{
  if( act->gen.cmd == BTL_ACTION_FIGHT ){
    return act->fight.waza;
  }
  return WAZANO_NULL;
}

// �A�C�e�������A�N�V�����ݒ�
void BTL_ACTION_SetItemParam( BTL_ACTION_PARAM* p, u16 itemNumber, u8 targetIdx, u8 wazaIdx )
{
  p->raw = 0;
  p->item.cmd = BTL_ACTION_ITEM;
  p->item.number = itemNumber;
  p->item.targetIdx = targetIdx;
  p->item.param = wazaIdx;
}

// ����ւ��|�P�����I���A�N�V�����i�I��Ώۂ͖���j
void BTL_ACTION_SetChangeBegin( BTL_ACTION_PARAM* p )
{
  p->raw = 0;
  p->change.cmd = BTL_ACTION_CHANGE;
  p->change.posIdx = 0;
  p->change.memberIdx = 0;
  p->change.depleteFlag = 0;
}

// ����ւ��|�P�����I���A�N�V�����i�ʏ�j
void BTL_ACTION_SetChangeParam( BTL_ACTION_PARAM* p, u8 posIdx, u8 memberIdx )
{
  p->raw = 0;
  p->change.cmd = BTL_ACTION_CHANGE;
  p->change.posIdx = posIdx;
  p->change.memberIdx = memberIdx;
  p->change.depleteFlag = 0;
}

// ����ւ��|�P�����I���A�N�V�����i�����킦��|�P���������Ȃ��j
void BTL_ACTION_SetChangeDepleteParam( BTL_ACTION_PARAM* p )
{
  p->raw = 0;
  p->change.cmd = BTL_ACTION_CHANGE;
  p->change.memberIdx = 0;
  p->change.depleteFlag = 1;
}
// ����ւ�����|�P�������������Ȃ�����
BOOL BTL_ACTION_IsDeplete( const BTL_ACTION_PARAM* p )
{
  return ((p->change.cmd == BTL_ACTION_CHANGE) && (p->change.depleteFlag == 1));
}

// ���[�e�[�V����
void BTL_ACTION_SetRotation( BTL_ACTION_PARAM* p, BtlRotateDir dir )
{
  p->raw = 0;
  p->rotation.cmd = BTL_ACTION_ROTATION;
  p->rotation.dir = dir;
}

void BTL_ACTION_SetEscapeParam( BTL_ACTION_PARAM* p )
{
  p->gen.cmd = BTL_ACTION_ESCAPE;
}

void BTL_ACTION_SetMoveParam( BTL_ACTION_PARAM* p )
{
  p->gen.cmd = BTL_ACTION_MOVE;
}

void BTL_ACTION_SetNULL( BTL_ACTION_PARAM* p )
{
  p->gen.cmd = BTL_ACTION_NULL;
  p->gen.param = 0;
}

void BTL_ACTION_SetSkip( BTL_ACTION_PARAM* p )
{
  p->gen.cmd = BTL_ACTION_SKIP;
}

/**
 *  �A�N�V�����R�}���h�擾
 */
BtlAction BTL_ACTION_GetAction( const BTL_ACTION_PARAM* p )
{
  return p->gen.cmd;
}

/**
 *  �^��o�b�t�@���ӂ�R�}���h
 */
void BTL_ACTION_SetRecPlayOver( BTL_ACTION_PARAM* act )
{
  act->gen.cmd = BTL_ACTION_RECPLAY_TIMEOVER;
  act->gen.param = 0;
}

/**
 *  �ǂݍ��ݎ��s�ʒm�R�}���h
 */
void BTL_ACTION_SetRecPlayError( BTL_ACTION_PARAM* act )
{
  act->gen.cmd = BTL_ACTION_RECPLAY_ERROR;
  act->gen.param = 0;
}

