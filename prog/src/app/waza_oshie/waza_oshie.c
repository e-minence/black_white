//============================================================================================
/**
 * @file	waza_oshie.c
 * @brief	�Z����/�v���o������
 * @author	Hiroyuki Nakamura / Akito Mori(WB�Ɉ��z��)
 * @date	09.10.13
 */
//============================================================================================
#include <gflib.h>
#include "savedata/mystatus.h"
#include "savedata/config.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_personal.h"

#define WAZA_OSHIE_H_GLOBAL
#include "app/waza_oshie.h"

// --------poke_tool.h�ɂȂ������̂ň�U�������Ɏ����Ă���----------------
// �Z�o���e�[�u���Ɋւ����`
#define LEVELUPWAZA_OBOE_NUM	( 20 )							///<�Z20��
#define LEVELUPWAZA_OBOE_MAX	( LEVELUPWAZA_OBOE_NUM+2 )		///<�Z20�{�I�[�R�[�h:1
#define LEVELUPWAZA_OBOE_BUFLEN	( (LEVELUPWAZA_OBOE_MAX+1)*2)	///<�i�Z20�{�I�[�R�[�h:1+4�o�C�g���E�␳:1�j* 2�o�C�g
#define LEVELUPWAZA_OBOE_END	( 0xffff )
#define LEVELUPWAZA_LEVEL_MASK  ( 0xfe00 )
#define LEVELUPWAZA_WAZA_MASK   ( 0x01ff )
#define LEVELUPWAZA_LEVEL_SHIFT (      9 )
// ------------------------------------------------------------------------


#define LV_WAZA_OBOE_MAX	( LEVELUPWAZA_OBOE_MAX )	// ���x���A�b�v�Ŋo����Z�� + �I�[�R�[�h
#define LV_WAZA_OBOE_END	( LEVELUPWAZA_OBOE_END )	// �I�[�R�[�h
#define LV_WAZA_LV_MASK		( LEVELUPWAZA_LEVEL_MASK )	// �o���郌�x������
#define LV_WAZA_WAZA_MASK   ( LEVELUPWAZA_WAZA_MASK )	// �o����Z����
#define LV_WAZA_LV_SHIFT	( LEVELUPWAZA_LEVEL_SHIFT )	// ���x���擾�V�t�g


//--------------------------------------------------------------------------------------------
/**
 * �Z�����f�[�^�̈�擾
 *
 * @param	heap	�q�[�vID
 *
 * @return	�Z�����f�[�^�̈�
 */
//--------------------------------------------------------------------------------------------
WAZAOSHIE_DATA * WazaOshie_DataAlloc( u32 heap )
{
	WAZAOSHIE_DATA * dat = GFL_HEAP_AllocMemory( heap, sizeof(WAZAOSHIE_DATA) );
/*��[GS_CONVERT_TAG]*/
	GFL_STD_MemFill( dat, 0, sizeof(WAZAOSHIE_DATA) );
/*��[GS_CONVERT_TAG]*/
	return dat;
}

//--------------------------------------------------------------------------------------------
/**
 * �Z�����f�[�^�̈���
 *
 * @param	heap	�q�[�vID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void WazaOshie_DataFree( WAZAOSHIE_DATA * dat )
{
	GFL_HEAP_FreeMemory( dat );
/*��[GS_CONVERT_TAG]*/
}

extern void POKE_PERSONAL_LoadWazaOboeTable( u16 mons_no, u16 form_no, u16* dst );


//--------------------------------------------------------------------------------------------
/**
 * �v���o���\�ȋZ���擾
 *
 * @param	pp		�|�P�����f�[�^
 * @param	heap	�q�[�vID
 *
 * @return	�擾�����Z�f�[�^
 *
 *	����͊e����
 */
//--------------------------------------------------------------------------------------------
u16 * RemaindWazaGet( POKEMON_PARAM * pp, u32 heap )
{
	u16 * get;
	u16 * ret;
	u16	waza[4];
	u16	mons;
	u8	lv;
	u8	form;
	u8	h, i, j;

	mons = (u16)PP_Get( pp, ID_PARA_monsno, NULL );
	form = (u8)PP_Get( pp, ID_PARA_form_no, NULL );
	lv   = (u8)PP_Get( pp, ID_PARA_level, NULL );
	for( i=0; i<4; i++ ){
		waza[i] = (u16)PP_Get( pp, ID_PARA_waza1+i, NULL );
	}

	get = GFL_HEAP_AllocMemory( heap, LV_WAZA_OBOE_MAX * 2 );
	ret = GFL_HEAP_AllocMemory( heap, LV_WAZA_OBOE_MAX * 2 );

//	PokeFormNoWazaOboeDataGet( mons, form, get );
	POKE_PERSONAL_LoadWazaOboeTable( mons, form, get );  // alloc�ł͂Ȃ�Load�ɕύX

	j = 0;
	for( i=0; i<LV_WAZA_OBOE_MAX; i++ ){
		// �I�[�R�[�h�`�F�b�N
		if( get[i] == LV_WAZA_OBOE_END ){
			ret[j] = WAZAOSHIE_TBL_MAX;
			break;
		// ���݂̃��x���Ŋo������Z��
		}else if( ((get[i]&LV_WAZA_LV_MASK)>>LV_WAZA_LV_SHIFT) > lv ){
			continue;
		}else{
			get[i] &= LV_WAZA_WAZA_MASK;
			// ���łɊo���Ă��邩
			for( h=0; h<4; h++ ){
				if( get[i] == waza[h] ){ break; }
			}
			if( h == 4 ){
				// �e�[�u���ɓ����Z���Ȃ���
				for( h=0; h<j; h++ ){
					if( ret[h] == get[i] ){ break; }
				}
				if( h == j ){
					ret[j] = get[i];
					j++;
				}
			}
		}
	}

	GFL_HEAP_FreeMemory( get );
/*��[GS_CONVERT_TAG]*/

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * ��������/�v���o����Z�����邩�`�F�b�N
 *
 * @param	tbl		�Z�e�[�u��
 *
 * @retval	"TRUE = ����"
 * @retval	"FALSE = �Ȃ�"
 */
//--------------------------------------------------------------------------------------------
BOOL WazaOshie_WazaTableChack( u16 * tbl )
{
	if( tbl[0] == WAZAOSHIE_TBL_MAX ){
		return FALSE;
	}
	return TRUE;
}
