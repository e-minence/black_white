//=============================================================================================
/**
 * @file	btl_action.h
 * @brief	�|�P����WB�o�g�� �v���C���[���I������������e�̎󂯓n���\���̒�`
 * @author	taya
 *
 * @date	2008.10.06	�쐬
 */
//=============================================================================================
#ifndef __BTL_ACTION_H__
#define __BTL_ACTION_H__

//--------------------------------------------------------------
/**
 *	�R�}���h�I��
 */
//--------------------------------------------------------------
typedef enum {
	BTL_ACTION_FIGHT,
	BTL_ACTION_ITEM,
	BTL_ACTION_CHANGE,
	BTL_ACTION_ESCAPE,

	BTL_ACTION_NULL,	///< �������Ȃ��i����̃|�P�����I��҂����Ȃǁj
}BtlAction;


typedef union {

	struct {
		u32 cmd		: 3;
		u32 param	: 29;
	}gen;

	struct {
		u32 cmd			: 3;
		u32 wazaIdx		: 3;
		u32 targetIdx	: 3;
	}fight;

	struct {
		u32 cmd				: 3;
		u32 number		: 16;
		u32 targetIdx	: 3;
	}item;

	struct {
		u32 cmd				: 3;
		u32 posIdx		: 3;	// ����ւ��ΏۈʒuID
		u32 memberIdx	: 3;	// �I�΂ꂽ�|�P�����̃p�[�e�B���C���f�b�N�X
		u32 depleteFlag	: 1;
	}change;

	struct {
		u32 cmd			: 3;
	}escape;


}BTL_ACTION_PARAM;

// ���������A�N�V����
static void BTL_ACTION_SetFightParam( BTL_ACTION_PARAM* p, u8 wazaIdx, u8 targetIdx )
{
	p->fight.cmd = BTL_ACTION_FIGHT;
	p->fight.wazaIdx = wazaIdx;
	p->fight.targetIdx = targetIdx;
}
// �A�C�e�������A�N�V����
static void BTL_ACTION_SetItemParam( BTL_ACTION_PARAM* p, u16 itemNumber, u8 targetIdx )
{
	p->item.cmd = BTL_ACTION_ITEM;
	p->item.number = itemNumber;
	p->item.targetIdx = targetIdx;
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
static void BTL_ACTION_SetChangeParam( BTL_ACTION_PARAM* p, u8 posIdx, u8 memberIdx )
{
	p->change.cmd = BTL_ACTION_CHANGE;
	p->change.posIdx = posIdx;
	p->change.memberIdx = memberIdx;
	p->change.depleteFlag = 0;
}
// ����ւ��|�P�����I���A�N�V�����i�����킦��|�P���������Ȃ��j
static void BTL_ACTION_SetChangeDepleteParam( BTL_ACTION_PARAM* p )
{
	p->change.cmd = BTL_ACTION_CHANGE;
	p->change.memberIdx = 0;
	p->change.depleteFlag = 1;
}

static void BTL_ACTION_SetEscapeParam( BTL_ACTION_PARAM* p )
{
	p->escape.cmd = BTL_ACTION_ESCAPE;
}

static void BTL_ACTION_SetNULL( BTL_ACTION_PARAM* p )
{
	p->gen.cmd = BTL_ACTION_NULL;
}

static inline BtlAction BTL_ACTION_GetAction( const BTL_ACTION_PARAM* p )
{
	return p->gen.cmd;
}

#endif

