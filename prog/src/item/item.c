//============================================================================================
/**
 * @file  item.c
 * @brief �A�C�e���f�[�^����
 * @author  Hiroyuki Nakamura
 * @date  05.09.06
 */
//============================================================================================
#include <gflib.h>
#include "arc_def.h"
#include "item_data.naix"
#include "item_icon.naix"
//#include "agb_itemsym.h"
#include "item/item.h"
#include "item/shooter_item.h"
#include "waza_tool/wazano_def.h"
#include "itemdata.h"
#include "message.naix"
#include "msg\msg_itemname.h"
#include "msg\msg_iteminfo.h"


//============================================================================================
//  �萔��`
//============================================================================================
#define NORMAL_WAZAMACHINE_MAX  ( 95 )   // �ʏ�̋Z�}�V����
#define HIDENWAZA_MAX           ( 6 )     // ��`�Z��
#define HIDENWAZA_START_POS     ( 92 )    // ��`�}�V���J�n�ʒu


//==============================================================================
//  �\���̒�`
//==============================================================================
typedef struct {
//  u16 arc_data;
  u16 arc_cgx;
  u16 arc_pal;
//  u16 agb_id;
}ITEMDATA_INDEX;


//============================================================================================
//  �f�[�^
//============================================================================================
#include "itemindex.dat"
#include "waza_mcn.dat"
#include "itemtype.dat"
#include "shooter_item_icon.cdat"


//============================================================================================
//  �v���g�^�C�v�錾
//============================================================================================
static s32 ItemParamRcvGet( ITEMPARAM_RCV * rcv, u16 param );



//--------------------------------------------------------------------------------------------
/**
 * �w��A�C�e�����w��ʒu�ɑ}��
 *
 * @param item  �A�C�e���f�[�^
 * @param pos1  �w��A�C�e���ʒu
 * @param pos2  �}���ʒu
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
#if 0
�܂��莝���A�C�e���̃Z�[�u�f�[�^�܂ł͂����Ă��Ă��Ȃ��̂ŃR�����g�A�E�g
2008.12.01(��) matsuda
void ItemPutIn( void * item, u16 pos1, u16 pos2 )
{
  MINEITEM * data;
  MINEITEM copy;
  s16 i;

  if( pos1 == pos2 ){ return; }

  data = (MINEITEM *)item;
  copy = data[pos1];

  if( pos2 > pos1 ){
    pos2 -= 1;
    for( i=(s16)pos1; i<(s16)pos2; i++ ){
      data[i] = data[i+1];
    }
  }else{
    for( i=(s16)pos1; i>(s16)pos2; i-- ){
      data[i] = data[i-1];
    }

  }
  data[pos2] = copy;
}
#endif


//============================================================================================
//============================================================================================
//  �f�[�^�擾
//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �f�[�^�C���f�b�N�X�擾
 *
 * @param item    �A�C�e���ԍ�
 * @param type    �擾�f�[�^
 *
 * @return  �w��f�[�^
 *
 * @li  type = ITEM_GET_DATA    : �A�C�e���f�[�^�̃A�[�J�C�u�C���f�b�N�X
 * @li  type = ITEM_GET_ICON_CGX  : �A�C�R���L�����̃A�[�J�C�u�C���f�b�N�X
 * @li  type = ITEM_GET_ICON_PAL  : �A�C�R���p���b�g�̃A�[�J�C�u�C���f�b�N�X
 * @li  type = ITEM_GET_AGB_NUM   : AGB�̃A�C�e���ԍ�
 */
//--------------------------------------------------------------------------------------------
u16 ITEM_GetIndex( u16 item, u16 type )
{
  switch( type ){
  case ITEM_GET_DATA:   // �A�C�e���f�[�^
    if( item == ITEM_DUMMY_ID || item == ITEM_RETURN_ID ){ break; }
//    return ItemDataIndex[item].arc_data;
		return item;

  case ITEM_GET_ICON_CGX: // �A�C�R���L�����f�[�^
    if( item == ITEM_DUMMY_ID ){ return NARC_item_icon_item_dumy_NCGR; }
    if( item == ITEM_RETURN_ID ){ return NARC_item_icon_item_yaji_NCGR; }
    return ItemDataIndex[item].arc_cgx;

  case ITEM_GET_ICON_PAL: // �A�C�R���p���b�g�f�[�^
    if( item == ITEM_DUMMY_ID ){ return NARC_item_icon_item_dumy_NCLR; }
    if( item == ITEM_RETURN_ID ){ return NARC_item_icon_item_yaji_NCLR; }
    return ItemDataIndex[item].arc_pal;
/*
  case ITEM_GET_AGB_NUM:  // AGB�̃A�C�e���ԍ�
    if( item == ITEM_DUMMY_ID || item == ITEM_RETURN_ID ){ break; }
    return ItemDataIndex[item].agb_id;
*/

	case ITEM_GET_SHOOTER_ICON_CGX:		// �V���[�^�[�p�A�C�R���L����
		return ShooterItemDataIndex[SHOOTER_ITEM_GetTableIndex(item)].arc_cgx;

	case ITEM_GET_SHOOTER_ICON_PAL:		// �V���[�^�[�p�A�C�R���p���b�g
		return ShooterItemDataIndex[SHOOTER_ITEM_GetTableIndex(item)].arc_pal;
  }

  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * AGB�̃A�C�e����DP�̃A�C�e���ɕϊ�
 *
 * @param agb   AGB�̃A�C�e���ԍ�
 *
 * @retval  "ITEM_DUMMY_ID = DP�ɂȂ��A�C�e��"
 * @retval  "ITEM_DUMMY_ID != DP�̃A�C�e���ԍ�"
 */
//--------------------------------------------------------------------------------------------
/*
u16 ITEM_CnvAgbItem( u16 agb )
{
  u16 i;

  for( i=1; i<=ITEM_DATA_MAX; i++ ){
    if( agb == ItemDataIndex[i].agb_id ){
      return i;
    }
  }
  return ITEM_DUMMY_ID;
}
*/

//--------------------------------------------------------------
/**
 * @brief   �A�C�e���A�C�R���̃A�[�J�C�uID�擾
 *
 * @retval  �A�C�e���A�C�R���̃A�[�J�C�uID
 */
//--------------------------------------------------------------
u16 ITEM_GetIconArcID(void)
{
  return ARCID_ITEMICON;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���̃Z���A�[�J�C�u�C���f�b�N�X�擾
 *
 * @param none
 *
 * @return  �Z���A�[�J�C�u�C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
u16 ITEM_GetIconCell(void)
{
  return NARC_item_icon_itemicon_NCER;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���A�C�R���̃Z���A�j���A�[�J�C�u�C���f�b�N�X�擾
 *
 * @param none
 *
 * @return  �Z���A�j���A�[�J�C�u�C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
u16 ITEM_GetIconCellAnm(void)
{
  return NARC_item_icon_itemicon_NANR;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�[�J�C�u�n���h���I�[�v��
 *
 * @param		heapID		�q�[�vID
 *
 * @return  �A�[�N�n���h��
 */
//--------------------------------------------------------------------------------------------
ARCHANDLE * ITEM_OpenItemDataArcHandle( HEAPID heapID )
{
	return GFL_ARC_OpenDataHandle( ARCID_ITEMDATA, heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���f�[�^�擾�i�A�[�N�n���h���Łj
 *
 * @param		ah				�A�[�N�n���h��
 * @param		item			�A�C�e���ԍ�
 * @param		heapID		�q�[�vID
 *
 * @return  �A�C�e���f�[�^
 */
//--------------------------------------------------------------------------------------------
void * ITEM_GetItemDataArcHandle( ARCHANDLE * ah, u16 item, HEAPID heapID )
{
  if( item > ITEM_DATA_MAX ){
		return NULL;
	}
	return GFL_ARC_LoadDataAllocByHandle( ah, item, heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�[�J�C�u�f�[�^���[�h
 *
 * @param item    �A�C�e���ԍ�
 * @param type    �擾�f�[�^
 * @param heap_id   �q�[�vID
 *
 * @return  �w��f�[�^
 *
 * @li  type = ITEM_GET_DATA    : �A�C�e���f�[�^
 * @li  type = ITEM_GET_ICON_CGX  : �A�C�R���̃L�����f�[�^
 * @li  type = ITEM_GET_ICON_PAL  : �A�C�R���̃p���b�g�f�[�^
 */
//--------------------------------------------------------------------------------------------
void * ITEM_GetItemArcData( u16 item, u16 type, HEAPID heap_id )
{
  if( item > ITEM_DATA_MAX ){ item = 0; }

  switch( type ){
  case ITEM_GET_DATA:   // �A�C�e���f�[�^
//    return GFL_ARC_LoadDataAlloc(ARCID_ITEMDATA, ItemDataIndex[item].arc_data, heap_id);
    return GFL_ARC_LoadDataAlloc(ARCID_ITEMDATA, item, heap_id);
  case ITEM_GET_ICON_CGX: // �A�C�R���L�����f�[�^
    return GFL_ARC_LoadDataAlloc(ARCID_ITEMICON, ItemDataIndex[item].arc_cgx, heap_id);
  case ITEM_GET_ICON_PAL: // �A�C�R���p���b�g�f�[�^
    return GFL_ARC_LoadDataAlloc(ARCID_ITEMICON, ItemDataIndex[item].arc_pal, heap_id);
  }
  return NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e�����擾
 *
 * @param   buf     �A�C�e�����i�[��o�b�t�@
 * @param   item    �A�C�e���ԍ�
 * @param   heap_id   �q�[�vID�i�e���|�����Ƃ��Ďg�p�j
 *
 */
//--------------------------------------------------------------------------------------------
void ITEM_GetItemName( STRBUF* buf, u16 item, HEAPID heap_id )
{
  GFL_MSGDATA *man;

  man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_itemname_dat, heap_id);
  GFL_MSG_GetString(man, item, buf);
  GFL_MSG_Delete( man );
}

//--------------------------------------------------------------------------------------------
/**
 * �����擾
 *
 * @param   buf     �A�C�e�����i�[��o�b�t�@
 * @param item    �A�C�e���ԍ�
 * @param heap_id   �q�[�vID
 *
 * @return  ����
 */
//--------------------------------------------------------------------------------------------
void ITEM_GetInfo( STRBUF * buf, u16 item, HEAPID heap_id )
{
  GFL_MSGDATA *man;

  man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_iteminfo_dat, heap_id);
  GFL_MSG_GetString(man, item, buf);
  GFL_MSG_Delete( man );
}

//--------------------------------------------------------------------------------------------
/**
 * �p�����[�^�擾�i�A�C�e���ԍ��w��j
 *
 * @param item  �A�C�e���ԍ�
 * @param param �擾�p�����[�^��`
 * @param heap_id   �q�[�vID
 *
 * @return  �w��p�����[�^
 */
//--------------------------------------------------------------------------------------------
s32 ITEM_GetParam( u16 item, u16 param, HEAPID heap_id )
{
  ITEMDATA * dat;
  s32 ret;

  dat = (ITEMDATA *)ITEM_GetItemArcData( item, ITEM_GET_DATA, GFL_HEAP_LOWID(heap_id) );
  ret = ITEM_GetBufParam( dat, param );
  GFL_HEAP_FreeMemory(dat);

  return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * �p�����[�^�擾�i�A�C�e���f�[�^�w��j
 *
 * @param item  �A�C�e���f�[�^
 * @param param �擾�p�����[�^��`
 *
 * @return  �w��p�����[�^
 */
//--------------------------------------------------------------------------------------------
s32 ITEM_GetBufParam( ITEMDATA * item, u16 param )
{
  switch( param ){
  case ITEM_PRM_PRICE:              // ���l
    return (s32)( item->price * 10 );
  case ITEM_PRM_EQUIP:              // ��������
    return (s32)item->eqp;
  case ITEM_PRM_ATTACK:             // �З�
    return (s32)item->atc;
  case ITEM_PRM_EVENT:              // �d�v
    return (s32)item->imp;
  case ITEM_PRM_CNV:                // �֗��{�^��
    return (s32)item->cnv_btn;
  case ITEM_PRM_POCKET:             // �ۑ���i�|�P�b�g�ԍ��j
    return (s32)item->fld_pocket;
  case ITEM_PRM_FIELD:              // field�@�\
    return (s32)item->field_func;
  case ITEM_PRM_BATTLE:             // battle�@�\
    return (s32)item->battle_func;
  case ITEM_PRM_TUIBAMU_EFF:        // ���΂ތ���
    return (s32)item->tuibamu_eff;
  case ITEM_PRM_NAGETUKERU_EFF:     // �Ȃ��������
    return (s32)item->nage_eff;
  case ITEM_PRM_NAGETUKERU_ATC:     // �Ȃ�����З�
    return (s32)item->nage_atc;
  case ITEM_PRM_SIZENNOMEGUMI_ATC:  // ������̂߂��݈З�
    return (s32)item->sizen_atc;
  case ITEM_PRM_SIZENNOMEGUMI_TYPE: // ������̂߂��݃^�C�v
    return (s32)item->sizen_type;
  case ITEM_PRM_BTL_POCKET:         // �퓬�ۑ���i�|�P�b�g�ԍ��j
    return (s32)item->btl_pocket;
  case ITEM_PRM_W_TYPE:             // ���[�N�^�C�v
    return (s32)item->work_type;
  case ITEM_PRM_ITEM_TYPE:          // �A�C�e�����
    return (s32)item->type;
  case ITEM_PRM_ITEM_SPEND:         // ����邩
    return (s32)item->spend;
	case ITEM_PRM_SORT_NUMBER:				// �\�[�g�ԍ�
		return (s32)item->sort;

  default:              // �ėp���[�N
    switch( item->work_type ){
    case ITEM_WKTYPE_DUMMY:     // ���ʃf�[�^
      return (s32)item->work[0];
    case ITEM_WKTYPE_POKEUSE:   // �|�P�����Ɏg������
      return ItemParamRcvGet( (ITEMPARAM_RCV *)item->work, param );
    }
  }

  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �񕜃p�����[�^�擾
 *
 * @param rcv   �񕜃f�[�^
 * @param param �擾�p�����[�^��`
 *
 * @return  �w��p�����[�^
 */
//--------------------------------------------------------------------------------------------
static s32 ItemParamRcvGet( ITEMPARAM_RCV * rcv, u16 param )
{
  switch( param ){
  case ITEM_PRM_SLEEP_RCV:      // �����
    return (s32)rcv->sleep_rcv;
  case ITEM_PRM_POISON_RCV:     // �ŉ�
    return (s32)rcv->poison_rcv;
  case ITEM_PRM_BURN_RCV:       // �Ώ���
    return (s32)rcv->burn_rcv;
  case ITEM_PRM_ICE_RCV:        // �X��
    return (s32)rcv->ice_rcv;
  case ITEM_PRM_PARALYZE_RCV:     // ��჉�
    return (s32)rcv->paralyze_rcv;
  case ITEM_PRM_PANIC_RCV:      // ������
    return (s32)rcv->panic_rcv;
  case ITEM_PRM_MEROMERO_RCV:     // ����������
    return (s32)rcv->meromero_rcv;
  case ITEM_PRM_ABILITY_GUARD:    // �\�̓K�[�h
    return (s32)rcv->ability_guard;
  case ITEM_PRM_DEATH_RCV:      // �m����
    return (s32)rcv->death_rcv;
  case ITEM_PRM_ALL_DEATH_RCV:    // �S���m����
    return (s32)rcv->alldeath_rcv;
  case ITEM_PRM_LV_UP:        // ���x���A�b�v
    return (s32)rcv->lv_up;
  case ITEM_PRM_EVOLUTION:      // �i��
    return (s32)rcv->evolution;
  case ITEM_PRM_ATTACK_UP:      // �U���̓A�b�v
    return (s32)rcv->atc_up;
  case ITEM_PRM_DEFENCE_UP:     // �h��̓A�b�v
    return (s32)rcv->def_up;
  case ITEM_PRM_SP_ATTACK_UP:     // ���U�A�b�v
    return (s32)rcv->spa_up;
  case ITEM_PRM_SP_DEFENCE_UP:    // ���h�A�b�v
    return (s32)rcv->spd_up;
  case ITEM_PRM_AGILITY_UP:     // �f�����A�b�v
    return (s32)rcv->agi_up;
  case ITEM_PRM_HIT_UP:       // �������A�b�v
    return (s32)rcv->hit_up;
  case ITEM_PRM_CRITICAL_UP:      // �N���e�B�J�����A�b�v
    return (s32)rcv->critical_up;
  case ITEM_PRM_PP_UP:        // PP�A�b�v
    return (s32)rcv->pp_up;
  case ITEM_PRM_PP_3UP:       // PP�A�b�v�i�R�i�K�j
    return (s32)rcv->pp_3up;
  case ITEM_PRM_PP_RCV:       // PP��
    return (s32)rcv->pp_rcv;
  case ITEM_PRM_ALL_PP_RCV:     // PP�񕜁i�S�Ă̋Z�j
    return (s32)rcv->allpp_rcv;
  case ITEM_PRM_HP_RCV:       // HP��
    return (s32)rcv->hp_rcv;
  case ITEM_PRM_HP_EXP:       // HP�w�͒l�A�b�v
    return (s32)rcv->hp_exp;
  case ITEM_PRM_POWER_EXP:      // �U���w�͒l�A�b�v
    return (s32)rcv->pow_exp;
  case ITEM_PRM_DEFENCE_EXP:      // �h��w�͒l�A�b�v
    return (s32)rcv->def_exp;
  case ITEM_PRM_AGILITY_EXP:      // �f�����w�͒l�A�b�v
    return (s32)rcv->agi_exp;
  case ITEM_PRM_SP_ATTACK_EXP:    // ���U�w�͒l�A�b�v
    return (s32)rcv->spa_exp;
  case ITEM_PRM_SP_DEFENCE_EXP:   // ���h�w�͒l�A�b�v
    return (s32)rcv->spd_exp;
  case ITEM_PRM_EXP_LIMIT_FLAG:   // �w�͒l���E����
		return (s32)rcv->exp_limit;
  case ITEM_PRM_FRIEND1:        // �Ȃ��x�P
    return (s32)rcv->friend_exp1;
  case ITEM_PRM_FRIEND2:        // �Ȃ��x�Q
    return (s32)rcv->friend_exp2;
  case ITEM_PRM_FRIEND3:        // �Ȃ��x�R
    return (s32)rcv->friend_exp3;

  case ITEM_PRM_HP_EXP_POINT:     // HP�w�͒l�̒l
    return (s32)rcv->prm_hp_exp;
  case ITEM_PRM_POWER_EXP_POINT:    // �U���w�͒l�̒l
    return (s32)rcv->prm_pow_exp;
  case ITEM_PRM_DEFENCE_EXP_POINT:  // �h��w�͒l�̒l
    return (s32)rcv->prm_def_exp;
  case ITEM_PRM_AGILITY_EXP_POINT:  // �f�����w�͒l�̒l
    return (s32)rcv->prm_agi_exp;
  case ITEM_PRM_SP_ATTACK_EXP_POINT:  // ���U�w�͒l�̒l
    return (s32)rcv->prm_spa_exp;
  case ITEM_PRM_SP_DEFENCE_EXP_POINT: // ���h�w�͒l�̒l
    return (s32)rcv->prm_spd_exp;
  case ITEM_PRM_HP_RCV_POINT:     // HP�񕜒l�̒l
    return (s32)rcv->prm_hp_rcv;
  case ITEM_PRM_PP_RCV_POINT:     // pp�񕜒l�̒l
    return (s32)rcv->prm_pp_rcv;
  case ITEM_PRM_FRIEND1_POINT:    // �Ȃ��x1�̒l
    return (s32)rcv->prm_friend1;
  case ITEM_PRM_FRIEND2_POINT:    // �Ȃ��x2�̒l
    return (s32)rcv->prm_friend2;
  case ITEM_PRM_FRIEND3_POINT:    // �Ȃ��x3�̒l
    return (s32)rcv->prm_friend3;
  }

  return 0;
}


//============================================================================================
//  �Z�}�V��
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief   �Z�}�V�����ǂ���
 *
 * @param   item  �A�C�e���ԍ�
 *
 * @retval  "TRUE = �Z�}�V��"
 * @retval  "FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ITEM_CheckWazaMachine( u16 item )
{
  if( ( item >= ITEM_WAZAMASIN01 && item <= ITEM_HIDENMASIN06 ) ||
      ( item >= ITEM_WAZAMASIN93 && item <= ITEM_WAZAMASIN95 ) ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �Z�}�V���Ŋo������Z���擾
 *
 * @param item  �A�C�e���ԍ�
 *
 * @return  �Z�ԍ�
 */
//--------------------------------------------------------------------------------------------
const u16 ITEM_GetWazaNo( u16 item )
{
  if( ITEM_CheckWazaMachine( item ) == FALSE ){
    return WAZANO_NULL;
  }

  if( item >= ITEM_WAZAMASIN93 ){
    item = item - ITEM_WAZAMASIN93 + HIDENWAZA_START_POS + HIDENWAZA_MAX;
  }else{
    item -= ITEM_WAZAMASIN01;
  }
  return MachineNo[ item ];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �I�����ꂽ�Z���|�P���������ɓK���Ȃ��Z���ǂ����i��`�Z���ǂ����j
 *
 * @param waza  �Z�ԍ�
 *
 * @retval  "TRUE = �����ɓK���Ȃ��Z(��`�Z"
 * @retval  "FALSE = �����ɓK����Z(��`�Z�ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ITEM_CheckHidenWaza( u16 waza )
{
  u8  i;

  for( i=0; i<HIDENWAZA_MAX; i++ ){
    if( MachineNo[HIDENWAZA_START_POS+i] == waza ){
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �Z�}�V���ԍ��擾
 *
 * @param item  �A�C�e���ԍ�
 *
 * @return  �Z�}�V���ԍ�
 *
 * @li  ��`�}�V���͒ʏ�̋Z�}�V���̌�ɃJ�E���g�����
 */
//--------------------------------------------------------------------------------------------
u8 ITEM_GetWazaMashineNo( u16 item )
{
  if( ITEM_CheckWazaMachine( item ) == FALSE ){
    return 0xff;
  }

  if( item >= ITEM_WAZAMASIN93 ){
    return ( item - ITEM_WAZAMASIN93 + HIDENWAZA_START_POS );
  }else if( item >= ITEM_HIDENMASIN01 ){
    return ( item - ITEM_HIDENMASIN01 + NORMAL_WAZAMACHINE_MAX );
  }
  return ( item - ITEM_WAZAMASIN01 );
}

//--------------------------------------------------------------------------------------------
/**
 * ��`�}�V���ԍ��擾
 *
 * @param item  �A�C�e���ԍ�
 *
 * @return  ��`�}�V���ԍ�
 */
//--------------------------------------------------------------------------------------------
u8 ITEM_GetHidenMashineNo( u16 item )
{
  if( item >= ITEM_HIDENMASIN01 && item <= ITEM_HIDENMASIN06 ){
    return ( item - ITEM_HIDENMASIN01 );
  }
  return 0xff;
}


//============================================================================================
//  ���[���`�F�b�N
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e�������[�����ǂ������`�F�b�N
 *
 * @param item  �A�C�e���ԍ�
 *
 * @retval  "TRUE = ���[��"
 * @retval  "FALSE = ���[���ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ITEM_CheckMail( u16 item )
{
  u32 i;

  for( i=0; i<ITEM_MAIL_MAX; i++ ){
    if( ItemMailTable[i] == item ){
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���ԍ����烁�[���̃f�U�C�����擾
 *
 * @param item  �A�C�e���ԍ�
 *
 * @return  �f�U�C��ID
 */
//--------------------------------------------------------------------------------------------
u8 ITEM_GetMailDesign( u16 item )
{
  u32 i;

  for( i=0; i<ITEM_MAIL_MAX; i++ ){
    if( ItemMailTable[i] == item ){
      return (u8)i;
    }
  }
  return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * ���[���̃f�U�C������A�C�e���ԍ����擾
 *
 * @param design    �f�U�C��ID
 *
 * @return  �A�C�e���ԍ�
 */
//--------------------------------------------------------------------------------------------
u16 ITEM_MailNumGet( u8 design )
{
  if( design >= ITEM_MAIL_MAX ){
    return 0;
  }
  return ItemMailTable[design];
}


//============================================================================================
//  �؂̎��`�F�b�N
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e�����؂̎����ǂ������`�F�b�N
 *
 * @param item  �A�C�e���ԍ�
 *
 * @retval  "TRUE = �؂̎�"
 * @retval  "FALSE = �؂̎��ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ITEM_CheckNuts( u16 item )
{
  u32 i;

  for( i=0; i<ITEM_NUTS_MAX; i++ ){
    if( ItemNutsTable[i] == item ){
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �؂̎��̔ԍ����擾
 *
 * @param item  �A�C�e���ԍ�
 *
 * @return  �؂̎��ԍ�
 */
//--------------------------------------------------------------------------------------------
u8 ITEM_GetNutsNo( u16 item )
{
  if( item < ITEM_KURABONOMI ){
    return 0xff;
  }
  return ( item - ITEM_KURABONOMI );
}

//--------------------------------------------------------------------------------------------
/**
 * �w��ID�̖؂̎��̃A�C�e���ԍ����擾
 *
 * @param id    �؂̎���ID
 *
 * @return  �A�C�e���ԍ�
 */
//--------------------------------------------------------------------------------------------
u16 ITEM_GetNutsID( u8 id )
{
  u32 i;

  if( id >= ITEM_NUTS_MAX ){ return 0xffff; }   // �G���[

  return ItemNutsTable[id];
}


//============================================================================================
//  ������`�F�b�N
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���������򂩂ǂ������`�F�b�N
 *
 * @param item  �A�C�e���ԍ�
 *
 * @retval  "TRUE = ������"
 * @retval  "FALSE = ������"
 */
//--------------------------------------------------------------------------------------------
BOOL ITEM_CheckKanpouyaku( u16 item )
{
  u32 i;

  if( item == ITEM_TIKARANOKONA || item == ITEM_TIKARANONEKKO ||
    item == ITEM_BANNOUGONA || item == ITEM_HUKKATUSOU ){

    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���e�[�u�������ׂă������ɓW�J
 *
 * @param heapID  �W�J���郁�����q�[�vID
 *
 * @retval  �m�ۂ����������̃A�h���X
 */
//--------------------------------------------------------------------------------------------
void  *ITEM_LoadDataTable( HEAPID heapID )
{
  int max;

  max=ITEM_GetIndex(ITEM_DATA_MAX,ITEM_GET_DATA);
  return GFL_ARC_LoadDataAllocOfs(ARCID_ITEMDATA, 0, heapID, 0, sizeof(ITEMDATA)*ITEM_DATA_MAX);
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e���e�[�u������C�ӂ̃A�h���X���擾
 *
 * @param item  �A�C�e���e�[�u��
 * @param index �Ƃ肾���C���f�b�N�X�i���o�[
 *
 * @retval  �擾�����A�h���X
 */
//--------------------------------------------------------------------------------------------
ITEMDATA  *ITEM_GetDataPointer(ITEMDATA *item,u16 index)
{
  u8 *ret_item;

  ret_item=(u8 *)item;
  ret_item+=index*sizeof(ITEMDATA);

  return (ITEMDATA *)ret_item;
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e�����|�P�����Ɏ��������邩
 *
 * @param item  �A�C�e���ԍ�
 *
 * @retval  "TRUE = ��"
 * @retval  "FALSE = �s��"
 *
 *  ��{�A����Œǉ����ꂽ����
 */
//--------------------------------------------------------------------------------------------
BOOL ITEM_CheckPokeAdd( u16 item )
{
  if( item == ITEM_SUPIIDOBOORU ||
    item == ITEM_REBERUBOORU ||
    item == ITEM_RUAABOORU ||
    item == ITEM_HEBIIBOORU ||
    item == ITEM_RABURABUBOORU ||
    item == ITEM_HURENDOBOORU ||
    item == ITEM_MUUNBOORU ||
    item == ITEM_KONPEBOORU ||
    item == ITEM_PAAKUBOORU ){
    return FALSE;
  }

  return TRUE;
}


#if PM_DEBUG
//--------------------------------------------------------------------------------------------
/**
 * �Â��A�C�e���ō��͎g���Ă��Ȃ����ǂ�����������
 * @param item  �A�C�e���ԍ�
 * @retval  TRUE  = ������
 * @retval  FALSE = �����Ȃ�
 */
//--------------------------------------------------------------------------------------------
BOOL ITEM_CheckEnable( u16 item )
{
  return (ItemDataIndex[item].arc_cgx != NARC_item_icon_item_dumy_NCGR);
}
#endif

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e��ID �� �{�[��ID �ϊ�
 *
 * @param   value    �A�C�e��No or BallID
 */
//--------------------------------------------------------------------------------------------
static u16 item_ConvBallItemID( u16 value, BOOL mode )
{
   static const struct {
    u16 id[2];
  }convertTbl[] = {
    { ITEM_MASUTAABOORU,  BALLID_MASUTAABOORU   }, //01 �}�X�^�[�{�[��
    { ITEM_HAIPAABOORU,   BALLID_HAIPAABOORU    }, //02 �n�C�p�[�{�[��
    { ITEM_SUUPAABOORU,   BALLID_SUUPAABOORU    }, //03 �X�[�p�[�{�[��
    { ITEM_MONSUTAABOORU, BALLID_MONSUTAABOORU  }, //04 �����X�^�[�{�[��
    { ITEM_SAFARIBOORU,   BALLID_SAFARIBOORU    }, //05 �T�t�@���{�[��
    { ITEM_NETTOBOORU,    BALLID_NETTOBOORU     }, //06 �l�b�g�{�[��
    { ITEM_DAIBUBOORU,    BALLID_DAIBUBOORU     }, //07 �_�C�u�{�[��
    { ITEM_NESUTOBOORU,   BALLID_NESUTOBOORU    }, //08 �l�X�g�{�[��
    { ITEM_RIPIITOBOORU,  BALLID_RIPIITOBOORU   }, //09 ���s�[�g�{�[��
    { ITEM_TAIMAABOORU,   BALLID_TAIMAABOORU    }, //10 �^�C�}�[�{�[��
    { ITEM_GOOZYASUBOORU, BALLID_GOOZYASUBOORU  }, //11 �S�[�W���X�{�[��
    { ITEM_PUREMIABOORU,  BALLID_PUREMIABOORU   }, //12 �v���~�A�{�[��
    { ITEM_DAAKUBOORU,    BALLID_DAAKUBOORU     }, //13 �_�[�N�{�[��
    { ITEM_HIIRUBOORU,    BALLID_HIIRUBOORU     }, //14 �q�[���{�[��
    { ITEM_KUIKKUBOORU,   BALLID_KUIKKUBOORU    }, //15 �N�C�b�N�{�[��
    { ITEM_PURESYASUBOORU,BALLID_PURESYASUBOORU }, //16 �v���V���X�{�[��
    { ITEM_SUPIIDOBOORU,  BALLID_SUPIIDOBOORU   }, //17 �X�s�[�h�{�[��
    { ITEM_REBERUBOORU,   BALLID_REBERUBOORU    }, //18 ���x���{�[��
    { ITEM_RUAABOORU,     BALLID_RUAABOORU      }, //19 ���A�[�{�[��
    { ITEM_HEBIIBOORU,    BALLID_HEBIIBOORU     }, //20 �w�r�[�{�[��
    { ITEM_RABURABUBOORU, BALLID_RABURABUBOORU  }, //21 ���u���u�{�[��
    { ITEM_HURENDOBOORU,  BALLID_HURENDOBOORU   }, //22 �t�����h�{�[��
    { ITEM_MUUNBOORU,     BALLID_MUUNBOORU      }, //23 ���[���{�[��
    { ITEM_KONPEBOORU,    BALLID_KONPEBOORU     }, //24 �R���y�{�[��
    { ITEM_DORIIMUBOORU,  BALLID_DORIIMUBOORU,  }, //25 �h���[���{�[��
  };

  u32 i;
  for(i=0; i<NELEMS(convertTbl); ++i)
  {
    if( convertTbl[i].id[mode] == value ){
      return convertTbl[i].id[mode^1];
    }
  }
  return 0; 
}

//--------------------------------------------------------------------------------------------
/**
 * �A�C�e��ID -> �{�[��ID �ϊ�
 *
 * @param   item    �A�C�e��No
 *
 * @retval  BALL_ID   �{�[��ID�i�w�肳�ꂽ�A�C�e��ID���{�[���ȊO�̏ꍇ�ABALLID_NULL�j
 */
//--------------------------------------------------------------------------------------------
BALL_ID ITEM_GetBallID( u16 item )
{
  return item_ConvBallItemID( item, 0);
}

//--------------------------------------------------------------------------------------------
/**
 * �{�[��ID -> �A�C�e��No �ϊ�
 *
 * @param   BALL_ID   �{�[��ID
 * @retval  itemID    �A�C�e��No�i�w�肳�ꂽ�A�C�e��ID���{�[���ȊO�̏ꍇ�AITEM_DUMMY_DATA
 */
//--------------------------------------------------------------------------------------------
u16 ITEM_BallID2ItemID( BALL_ID ballID )
{
  return item_ConvBallItemID( ballID, 1);
}


