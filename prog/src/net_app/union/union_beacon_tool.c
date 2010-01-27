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
#include "record_gra.naix"
#include "test_graphic/trbgra.naix"
#include "test_graphic/trfgra.naix"

#include "print/wordset.h"
#include "field/fldmmdl_pl_code.h"

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

// �����ڕύX���₶�p�̃e�[�u���iID���W�Ŋ��������܂�ɑΉ�����j
static const int UnionViewNameTable[][4]={
	{0,1,2,3,},
	{1,6,7,0,},

	{2,3,4,5,},
	{3,0,5,6,},

	{4,1,2,7,},
	{5,2,7,0,},

	{6,3,4,1,},
	{7,4,5,6,},
};

#define UNION_VIEW_SELECT_NUM	( 4 )
#define UNION_VIEW_SELECT_TYPE	( 8 )

//==============================================================================
/**
 * $brief	ID�Ɛ��ʂ���I���ł���S�̃g���[�i�[�^�C�v�̕�������i�[����
 *
 * @param	id			���[�U�[ID
 * @param	sex			����
 * @param	wordset		������i�[�o�b�t�@
 *
 * @retval	none		
 */
//==============================================================================
void UnionView_SetUpTrainerTypeSelect( u32 id, int sex, WORDSET *wordset )
{
	int key    = id%8;
	int i;

	// ID���W�Ŋ������]��ŁA�Ή�����4�̑I�����𐫕ʂ��Ƃɗp�ӂ���

	for(i=0; i<UNION_VIEW_SELECT_NUM; i++){
		int patern = UnionViewNameTable[key][i] + UNION_VIEW_SELECT_TYPE * sex;
		WORDSET_RegisterTrTypeName( wordset, i, UnionViewTable[patern].msg_type );
	}


}


//==============================================================================
/**
 * $brief	�I�����ʂ��猈�肵���g���[�i�[�^�C�v�̃e�[�u��Index���擾����
 *
 * @param	id			���[�U�[ID
 * @param	sex			����
 * @param	select		�I�����ʁi0-3)
 *
 * @retval	int		
 */
//==============================================================================
int UnionView_GetTrainerTypeIndex( u32 id, int sex, u32 select )
{
	int key    = id%8;
	int patern = UnionViewNameTable[key][select] + UNION_VIEW_SELECT_TYPE * sex;

  return patern;
}


//==============================================================================
/**
 * $brief	�I�����ʂ��猈�肵���g���[�i�[�^�C�v�̔ԍ����̂��̂��Z�o����
 *
 * @param	id			���[�U�[ID
 * @param	sex			����
 * @param	select		�I�����ʁi0-3)
 *
 * @retval	int		
 */
//==============================================================================
int UnionView_GetTrainerType( u32 id, int sex, u32 select )
{
  int patern = UnionView_GetTrainerTypeIndex(id, sex, select);
	return UnionViewTable[patern].objcode;
}

//------------------------------------------------------------------
/**
 * $brief	���j�I�����[���p�̌����ڔԍ��e�[�u�����������ēY������Ԃ�
 *
 * @param	sex			����
 * @param	view_type	�����ڔԍ�
 *
 * @retval	int			�Y�����ԍ�
 */
//------------------------------------------------------------------
static int Search_ViewNo( int sex, int view_type )
{
	int i;
	for(i=0;i<UNION_VIEW_TYPE_NUM/2;i++){
		if(UnionViewTable[i+(sex*(UNION_VIEW_TYPE_NUM/2))].objcode==view_type){
			return i + (sex*(UNION_VIEW_TYPE_NUM/2));
		}	
	}
	return 0;
}
//==============================================================================
/**
 * $brief	�����ڔԍ��Ɛ��ʂ���2D�L�����̈ʒu���擾����
 *
 * @param	sex		
 * @param	view_type		
 *
 * @retval	int		
 */
//==============================================================================
int UnionView_GetCharaNo( int sex, int view_type )
{

	return Search_ViewNo( sex, view_type );
}

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


//==============================================================================
/**
 * $brief	�g���[�i�[VIEWTYPE�Ɛ��ʂ�����������o��
 *
 * @param	sex			����
 * @param	view_type	MyStatus_GetTrainerView�Ŏ擾�ł���R�[�h
 * @param	info		�~�����f�[�^�̃^�C�v
 *						UNIONVIEW_ICONINDEX:	0�l�߂ŉ��Ԗڂ�
 *						UNIONVIEW_MSGTYPE:		���b�Z�[�W�f�[�^�ԍ��iex �u���キ������v��MSGNO�j
 *						UNIONVIEW_TRTYPE:		�g���[�i�[�O���t�B�b�N�ԍ�
 *
 * @retval	int			0����l�߂����ɉ��Ԗڂ̃L������
 */
//==============================================================================
int UnionView_GetTrainerInfo( int sex, int view_type, int info )
{
	int i = Search_ViewNo( sex, view_type );

	switch(info){
  case UNIONVIEW_ICONINDEX:
		return i;
  case UNIONVIEW_MSGTYPE:
		return UnionViewTable[i].msg_type;
  case UNIONVIEW_TRTYPE:
		return UnionViewTable[i].tr_type;
  default:
		GF_ASSERT(0);
	}

	// �����܂Ŕ�����̂͂�������
	return 0;
}


//==============================================================================
/**
 * $brief	�w���OBJ�ɕK�v�ȃp���b�g�̃|�C���^��Ԃ�
 *
 * @param	sex		
 * @param	view_type		
 *
 * @retval	u16 *		
 */
//==============================================================================
u16 *UnionView_GetPalNo( u16 * table, int sex, int view_type )
{
	int ret;
	ret = Search_ViewNo( sex, view_type );
	
	return &table[16*2*ret];
}

//==============================================================================
/**
 * $brief	18�̕��̃p���b�g�̃e�[�u����Alloc��۰�ނ��ĕԂ�
 *
 * @param	heapID		�q�[�vID
 *
 * @retval	u16 *		OBJ�p�̃p���b�g�e�[�u���i����͊e���Łj
 */
//==============================================================================
u16 *UnionView_PalleteTableAlloc( int heapID )
{
	u8 *data1, *data2;
	NNSG2dPaletteData *pal1,*pal2;
	u16 *palette_table,*src;
	int i;
	
	data1 = GFL_ARC_UTIL_LoadPalette( ARCID_RECORD_GRA, NARC_record_gra_union_chara_NCLR, &pal1, heapID );

	palette_table = GFL_HEAP_AllocMemory( heapID, 16*18*2 );
	OS_Printf("allocwork = %08x srcpallette = %08x\n",palette_table, pal1->pRawData);

	src = (u16*)pal1->pRawData;
	for(i=0;i<16*16;i++){
		palette_table[i] = src[i];
	}
//	MI_CpuCopyFast(pal1->pRawData, &palette_table[	  0], 16*16*2);
//	MI_CpuCopyFast(pal2->pRawData, &palette_table[16*16], 2*16*2);
	
	GFL_HEAP_FreeMemory(data1);
	
	return palette_table;
}

//�ȉ�poke_tool����ڐA

//============================================================================================
/**
 *	�g���[�i�[�L������CLACT���\�[�X��ID���擾
 *
 * @param[in]	trtype	�g���[�i�[�^�C�v
 * @param[in]	dir		PARA_FRONT:���� PARA_BACK:�w��
 * @param[out]	tcb		TR_CLACT_GRA�^���[�N�ւ̃|�C���^	
 * @param[in]	non_throw	CLACT���\�[�XID�ւ̃I�t�Z�b�g(EX�̂�)
 *
 * @retval	CLACT�ւ̃|�C���^
 */
//============================================================================================
void	TrCLACTGraDataGetEx(int trtype,int dir,BOOL non_throw,TR_CLACT_GRA *tcg)
{
	if(dir==TR_PARA_FRONT){
		tcg->arcID=ARCID_TRF_GRA;
		tcg->ncgrID=NARC_trfgra_trgs_hero_NCGR+trtype*5;
		tcg->nclrID=NARC_trfgra_trgs_hero_NCLR+trtype*5;
		tcg->ncerID=NARC_trfgra_trgs_hero_NCER+trtype*5;
		tcg->nanrID=NARC_trfgra_trgs_hero_NANR+trtype*5;
		tcg->ncbrID=NARC_trfgra_trgs_hero_NCBR+trtype*5;
	}
	else{
		GF_ASSERT_MSG(0,"Plz fix me!!\n");
/*	//�����Ƀg���[�i�[��ނ��K�v�Ȃ̂ŕۗ�
		tcg->arcID=ARCID_TRB_GRA;
		trtype=TrainerBTrTypeGet(trtype,non_throw);
		tcg->ncgrID=NARC_trbgra_gtr1b_002_NCGR+trtype*5;
		tcg->nclrID=NARC_trbgra_gtr1b_002_NCLR+trtype*5;
		tcg->ncerID=NARC_trbgra_gtr1b_002_NCER+trtype*5;
		tcg->nanrID=NARC_trbgra_gtr1b_002_NANR+trtype*5;
		tcg->ncbrID=NARC_trbgra_gtr1b_002_NCBR+trtype*5;
*/
	}
}
void	TrCLACTGraDataGet(int trtype,int dir,TR_CLACT_GRA *tcg)
{
	TrCLACTGraDataGetEx(trtype,dir,FALSE,tcg);
}