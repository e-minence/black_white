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
  BTL_ACTION_ROTATE,

  BTL_ACTION_SKIP,    ///< �����Ȃǂœ����Ȃ�
}BtlAction;


typedef union {

  struct {
    u32 cmd   : 3;
    u32 param : 29;
  }gen;

  struct {
    u32 cmd       : 3;
    u32 targetPos : 3;
    u32 waza      : 16;
    u32 _0        : 9;
  }fight;

  struct {
    u32 cmd       : 3;
    u32 targetIdx : 3;  ///< �Ώۃ|�P�����̃p�[�e�B���C���f�b�N�X
    u32 number    : 16; ///< �A�C�e��ID
    u32 param     : 8;  ///< �T�u�p�����[�^�iPP�񕜎��A�ǂ̃��U�ɓK�p���邩�A�Ȃǁj
    u32 _1        : 1;
  }item;

  struct {
    u32 cmd         : 3;
    u32 posIdx      : 3;  // ����ւ��ΏۈʒuID
    u32 memberIdx   : 3;  // �I�΂ꂽ�|�P�����̃p�[�e�B���C���f�b�N�X
    u32 depleteFlag : 1;  // ����ւ���|�P�������������Ȃ����Ƃ�ʒm����t���O
    u32 _2          : 22;
  }change;

  struct {
    u32 cmd     : 3;
    u32 _3      : 29;
  }escape;

  struct {
    u32 cmd     : 3;
    u32 _4      : 29;
  }move;

  struct {
    u32 cmd     : 3;
    u32 dir     : 29;
  }rotate;

}BTL_ACTION_PARAM;

// ���������A�N�V����
static inline void BTL_ACTION_SetFightParam( BTL_ACTION_PARAM* p, WazaID waza, u8 targetPos )
{
  p->fight.cmd = BTL_ACTION_FIGHT;
  p->fight.targetPos = targetPos;
  p->fight.waza = waza;
}
// �A�C�e�������A�N�V����
static inline void BTL_ACTION_SetItemParam( BTL_ACTION_PARAM* p, u16 itemNumber, u8 targetIdx )
{
  p->item.cmd = BTL_ACTION_ITEM;
  p->item.number = itemNumber;
  p->item.targetIdx = targetIdx;
  p->item.param = 0;
}
// ����ւ��|�P�����I���A�N�V�����i�I��Ώۂ͖���j
static inline void BTL_ACTION_SetChangeBegin( BTL_ACTION_PARAM* p )
{
  p->change.cmd = BTL_ACTION_CHANGE;
  p->change.posIdx = 0;
  p->change.memberIdx = 0;
  p->change.depleteFlag = 0;
}

// ����ւ��|�P�����I���A�N�V�����i�ʏ�j
static inline void BTL_ACTION_SetChangeParam( BTL_ACTION_PARAM* p, u8 posIdx, u8 memberIdx )
{
  p->change.cmd = BTL_ACTION_CHANGE;
  p->change.posIdx = posIdx;
  p->change.memberIdx = memberIdx;
  p->change.depleteFlag = 0;
}
// ����ւ��|�P�����I���A�N�V�����i�����킦��|�P���������Ȃ��j
static inline void BTL_ACTION_SetChangeDepleteParam( BTL_ACTION_PARAM* p )
{
  p->change.cmd = BTL_ACTION_CHANGE;
  p->change.memberIdx = 0;
  p->change.depleteFlag = 1;
}

static inline void BTL_ACTION_SetEscapeParam( BTL_ACTION_PARAM* p )
{
  p->gen.cmd = BTL_ACTION_ESCAPE;
}

static inline void BTL_ACTION_SetMoveParam( BTL_ACTION_PARAM* p )
{
  p->gen.cmd = BTL_ACTION_MOVE;
}
static inline void BTL_ACTION_SetNULL( BTL_ACTION_PARAM* p )
{
  p->gen.cmd = BTL_ACTION_NULL;
}

static inline void BTL_ACTION_SetSkip( BTL_ACTION_PARAM* p )
{
  p->gen.cmd = BTL_ACTION_SKIP;
}

static inline BtlAction BTL_ACTION_GetAction( const BTL_ACTION_PARAM* p )
{
  return p->gen.cmd;
}

static inline WazaID BTL_ACTION_GetWazaID( const BTL_ACTION_PARAM* act )
{
  if( act->gen.cmd == BTL_ACTION_FIGHT ){
    return act->fight.waza;
  }
  return WAZANO_NULL;
}

#endif

