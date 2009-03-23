//======================================================================
/**
 * @file	musical_system.h
 * @brief	�~���[�W�J���p �ėp��`
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================

#include "musical/musical_system.h"

//�~���[�W�J���̎Q�����i�����邩���ׂ�(��)
BOOL	MUSICAL_SYSTEM_CheckEntryMusical( POKEMON_PARAM *pokePara )
{
	OS_TPrintf("MUSICAL_SYSTEM_CheckEntryMusical is not created!!!\n");
	return TRUE;
}

//�~���[�W�J���p�p�����[�^�̏�����
MUSICAL_POKE_PARAM* MUSICAL_SYSTEM_InitMusPoke( POKEMON_PARAM *pokePara , HEAPID heapId )
{
	int i;
	MUSICAL_POKE_PARAM *musPara;
	musPara = GFL_HEAP_AllocMemory( heapId , sizeof(MUSICAL_POKE_PARAM) );
	musPara->pokePara = pokePara;
	
	for( i=0;i<MUS_POKE_EQUIP_MAX;i++ )
	{
		musPara->equip[i].itemNo = MUSICAL_ITEM_INVALID;
		musPara->equip[i].angle = 0;
	}
	
	return musPara;
}