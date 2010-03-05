//=============================================================================
/**
 * @file	union_beacon_tool.c
 * @brief	���j�I�����[���r�[�R������
 * @author	Akito Mori
 * @date		2006.03.16
 */
//=============================================================================

#include <gflib.h>

#include "arc_def.h"
#include "test_graphic/trbgra.naix"
#include "test_graphic/trfgra.naix"

#include "print/wordset.h"

#include "net_app/union/union_beacon_tool.h"
#include "field/fldmmdl.h"
#include "msg/msg_trtype.h"
#include "tr_tool/trtype_def.h"

#define UNION_VIEW_TYPE_NUM	( 16 )

// ���j�I�����[���ŕ\������g���[�i�[�̃^�C�v�����ځ����O���g���[�i�[�O���t�B�b�N�e�[�u��
static const struct{
  u16 objcode;
  u8 msg_type;
  u8 tr_type;
}UnionViewTable[] = {
	{ BOY2,		MSG_TRTYPE_TANPAN,		TRTYPE_TANPAN	},	///< ����p��������
	{ TRAINERM,		MSG_TRTYPE_ELITEM,		TRTYPE_ELITEM	},	///< �G���[�g�g���[�i�[
	{ RANGERM,		MSG_TRTYPE_RANGERM,		TRTYPE_RANGERM	},	///< �|�P���������W���[
	{ BREEDERM,		MSG_TRTYPE_BREEDERM,		TRTYPE_BREEDERM	},	///< �|�P�����u���[�_�[
	{ ASSISTANTM,	MSG_TRTYPE_SCIENTISTM,		TRTYPE_SCIENTISTM	},	///< ���񂫂イ����
	{ MOUNTMAN,	MSG_TRTYPE_MOUNT,		TRTYPE_MOUNT	},	///< ��܂��Ƃ�
	{ BADMAN,	MSG_TRTYPE_HEADS,		TRTYPE_HEADS	},	///< �X�L���w�b�Y
	{ BABYBOY1,	MSG_TRTYPE_KINDERGARTENM,		TRTYPE_KINDERGARTENM	},	///< �悤������

	{ GIRL2,		MSG_TRTYPE_MINI,		TRTYPE_MINI	},	///< �~�j�X�J�[�g
	{ TRAINERW,		MSG_TRTYPE_ELITEW,		TRTYPE_ELITEW	},	///< �G���[�g�g���[�i�[
	{ RANGERW,		MSG_TRTYPE_RANGERW,		TRTYPE_RANGERW	},	///< �|�P���������W���[
	{ BREEDERW,		MSG_TRTYPE_BREEDERW,		TRTYPE_BREEDERW	},	///< �|�P�����u���[�_�[
	{ ASSISTANTW,		MSG_TRTYPE_SCIENTISTW,		TRTYPE_SCIENTISTW	},	///< ���񂫂イ����
	{ AMBRELLA,		MSG_TRTYPE_PARASOL,		TRTYPE_PARASOL	},	///< �p���\�����˂�����
	{ NURSE,	MSG_TRTYPE_NURSE,		TRTYPE_NURSE	},	///< �i�[�X
	{ BABYGIRL1,	MSG_TRTYPE_KINDERGARTENW,		TRTYPE_KINDERGARTENW	},	///< �悤������
};


//==================================================================
/**
 * �g���[�i�[�^�C�v�̃e�[�u��INDEX����OBJCODE���擾����
 * @param   view_index		�e�[�u��INDEX
 * @retval  int		OBJCODE
 */
//==================================================================
int UnionView_GetObjCode(int view_index)
{
  if(view_index >= NELEMS(UnionViewTable)){
    GF_ASSERT(0);
    return BOY2;
  }
  return UnionViewTable[view_index].objcode;
}

//==================================================================
/**
 * �g���[�i�[�^�C�v�̃e�[�u��INDEX����TRTYPE���擾����
 * @param   view_index		�e�[�u��INDEX
 * @retval  int		TRTYPE
 */
//==================================================================
int UnionView_GetTrType(int view_index)
{
  if(view_index >= NELEMS(UnionViewTable)){
    GF_ASSERT(0);
    return TRTYPE_TANPAN;
  }
  return UnionViewTable[view_index].tr_type;
}

//==================================================================
/**
 * �g���[�i�[�^�C�v�̃e�[�u��INDEX����MSGTYPE���擾����
 * @param   view_index		�e�[�u��INDEX
 * @retval  int		TRTYPE
 */
//==================================================================
int UnionView_GetMsgType(int view_index)
{
  if(view_index >= NELEMS(UnionViewTable)){
    GF_ASSERT(0);
    return MSG_TRTYPE_TANPAN;
  }
  return UnionViewTable[view_index].msg_type;
}

//==================================================================
/**
 * OBJCODE����Index���t��������
 * @param   objcode		OBJCODE
 * @retval  int		Index
 */
//==================================================================
int UnionView_Objcode_to_Index(u16 objcode)
{
  int i;
  
  for(i = 0; i < NELEMS(UnionViewTable); i++){
    if(UnionViewTable[i].objcode == objcode){
      return i;
    }
  }
  GF_ASSERT_MSG(0, "objcode = %d\n", objcode);
  return 0;
}

