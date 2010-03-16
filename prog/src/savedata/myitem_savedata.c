//==============================================================================
/**
 * @file	myitem_savedata.c
 * @brief	�莝���A�C�e��
 * @author	matsuda
 * @date	2008.12.04(��)
 */
//==============================================================================
#include <gflib.h>
#include "savedata/myitem_savedata.h"
#include "item/item.h"
#include "myitem_savedata_local.h"



//==============================================================================
//	�\���̒�`
//==============================================================================

// �t�B�[���h�̃o�b�O�̃J�[�\���ʒu
typedef struct {
	s16	scr[BAG_POKE_MAX];  //10
	s16	pos[BAG_POKE_MAX];  //20
	u16	pocket;        //22
	u16	dummy;         //24byte
}FLDBAG_CURSOR;

// �퓬�̃o�b�O�̃J�[�\���ʒu
typedef struct {
	s16	scr[BATTLE_BAG_POKE_MAX];
	s16	pos[BATTLE_BAG_POKE_MAX];
	u16	item;			// �Ō�Ɏg�����A�C�e��
	u16	page;			// �Ō�Ɏg�����A�C�e���̃|�P�b�g
//	u16	pocket;
}BTLBAG_CURSOR;

// �o�b�O�̃J�[�\���ʒu�f�[�^
struct _BAG_CURSOR {
	FLDBAG_CURSOR	fld;
	BTLBAG_CURSOR	btl;
};

//==============================================================================
//	�萔��`
//==============================================================================
#define	ITEM_CHECK_ERR	( 0xffffffff )		// �`�F�b�N�G���[

#define	ITEM_MAX_NORMAL			( 999 )		// �ʏ�̃A�C�e���������ő�
#define	ITEM_MAX_WAZAMACHINE	( 99 )		// �Z�}�V���̏������ő�

static u32 MyItemDataGet( MYITEM_PTR myitem, u16 id, ITEM_ST ** item, u32 * max, HEAPID heap );
static u32 MyPocketDataGet( MYITEM_PTR myitem, s32 pocket, ITEM_ST ** item, u32 * max);
static void bottomInsert( ITEM_ST * item, const u32 max );



//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �莝���A�C�e�����[�N�̃T�C�Y�擾
 * @retval  �T�C�Y
 */
//--------------------------------------------------------------
int MYITEM_GetWorkSize(void)
{
	return sizeof(MYITEM);
}

//------------------------------------------------------------------
/**
 * @brief	�莝���A�C�e�����[�N�𐶐�����
 * @param	myitem	�莝���A�C�e���\���̂ւ̃|�C���^
 * @return	MYITEM�ւ̃|�C���^
 */
//------------------------------------------------------------------
MYITEM_PTR MYITEM_AllocWork(HEAPID heapID)
{
	MYITEM_PTR item;
	item = GFL_HEAP_AllocMemory(heapID, sizeof(MYITEM));
	MYITEM_Init(item);
	return item;
}

//------------------------------------------------------------------
/**
 * @brief	����������
 * @param	item	MYITEM�ւ̃|�C���^
 */
//------------------------------------------------------------------
void MYITEM_Init(MYITEM_PTR item)
{
	GFL_STD_MemClear(item, sizeof(MYITEM));
}

//------------------------------------------------------------------
/**
 * @brief	MYITEM�̃R�s�[
 * @param	from	�R�s�[���ւ̃|�C���^
 * @param	to		�R�s�[��ւ̃|�C���^
 */
//------------------------------------------------------------------
void MYITEM_Copy(const MYITEM_PTR from, MYITEM_PTR to)
{
	GFL_STD_MemCopy(from, to, sizeof(MYITEM));
}


//------------------------------------------------------------------
/**
 * @brief	MYITEM��ITEM_ST�̃R�s�[
 * @param	myitem	  MYITEM�̃|�C���^
 * @param	itemst		�A�C�e�����X�g�̐擪�|�C���^
 * @param	pocket		�|�P�b�g�ԍ�
 * @param	bMyGet		myitem����f�[�^���ق����ꍇTRUE  myitem�Ɋi�[����ꍇFALSE
 */
//------------------------------------------------------------------
void MYITEM_ITEM_STCopy(MYITEM_PTR myitem, ITEM_ST* itemst, int pocket, int bMyGet)
{
  ITEM_ST* MyItemSt;
  u32 max;

  MyPocketDataGet(myitem, pocket, &MyItemSt, &max);

  if(bMyGet)
  {
    GFL_STD_MemCopy(MyItemSt, itemst, sizeof(ITEM_ST) * max );
  }
  else
  {
    GFL_STD_MemCopy(itemst, MyItemSt,  sizeof(ITEM_ST) * max );
  }
}


//------------------------------------------------------------------
/**
 * @brief	�֗��{�^���Ɋ��蓖�Ă��Ă���A�C�e���擾
 * @param	myitem	MYITEM�ւ̃|�C���^
 * @return	�A�C�e���ԍ�
 */
//------------------------------------------------------------------
u32 MYITEM_CnvButtonItemGet( const MYITEM_PTR myitem, int index )
{
	return myitem->cnv_button[index];
}

//------------------------------------------------------------------
/**
 * @brief	�֗��{�^���ɃA�C�e���Z�b�g
 * @param	myitem	MYITEM�ւ̃|�C���^
 * @param	setitem	�Z�b�g����A�C�e��
 * @return	�A�C�e���ԍ�
 */
//------------------------------------------------------------------
void MYITEM_CnvButtonItemSet( MYITEM_PTR myitem, int index , u32 setitem )
{
  GF_ASSERT(index < DUMMY_SHORTCUT_MAX);
	myitem->cnv_button[index] = setitem;
}

//------------------------------------------------------------------
/**
 * @brief	�e�|�P�b�g��ITEM_ST�ƍő吔�擾
 * @param	pocket		�|�P�b�g�ԍ�
 * @param	item	MYITEM�擾�ꏊ
 * @param	max		�ő吔�擾�ꏊ
 * @param	heap		�q�[�vID
 * @return none
 */
//------------------------------------------------------------------
static u32 MyPocketDataGet( MYITEM_PTR myitem, s32 pocket, ITEM_ST ** item, u32 * max)
{
	switch( pocket ){
	case BAG_POKE_EVENT:	// ��؂ȕ�
		*item = myitem->MyEventItem;
		*max = BAG_EVENT_ITEM_MAX;
		break;
	case BAG_POKE_NORMAL:	// ����
		*item = myitem->MyNormalItem;
		*max = BAG_NORMAL_ITEM_MAX;
		break;
	case BAG_POKE_NUTS:		// �؂̎�
		*item = myitem->MyNutsItem;
		*max = BAG_NUTS_ITEM_MAX;
		break;
	case BAG_POKE_DRUG:		// ��
		*item = myitem->MyDrugItem;
		*max = BAG_DRUG_ITEM_MAX;
		break;
	case BAG_POKE_WAZA:		// �Z�}�V��
		*item = myitem->MySkillItem;
		*max = BAG_WAZA_ITEM_MAX;
		break;
	default:
		GF_ASSERT(0);
		*item = NULL;
		break;
	}
	return pocket;
}

//------------------------------------------------------------------
/**
 * @brief	MYITEM�ƍő吔�擾
 * @param	id		�A�C�e���ԍ�
 * @param	item	MYITEM�擾�ꏊ
 * @param	max		�ő吔�擾�ꏊ
 * @param	heap		�q�[�vID
 * @return	�|�P�b�g�ԍ�
 */
//------------------------------------------------------------------
static u32 MyItemDataGet( MYITEM_PTR myitem, u16 id, ITEM_ST ** item, u32 * max, HEAPID heap )
{
	s32 pocket = ITEM_GetParam( id, ITEM_PRM_POCKET, heap );

	MyPocketDataGet(myitem, pocket, item, max);
	return pocket;
}


//------------------------------------------------------------------
/**
 * @brief	������A�C�e�����`�F�b�N
 * @param	item		�莝���A�C�e���\���̂ւ̃|�C���^
 * @param	siz			�莝���A�C�e���\���̂̃T�C�Y�i���j
 * @param	id			�w�肷��A�C�e���̃i���o�[
 * @param	num			������A�C�e���̌�
 * @param	max			�A�C�e���ő吔
 * @retval	���� = �A�C�e���ʒu
 * @retval	���s = NULL
 */
//------------------------------------------------------------------
static ITEM_ST * AddItemSearch( ITEM_ST * item, u32 siz, u16 id, u16 num, u16 max )
{
	u32	i;
	u32	pos = ITEM_CHECK_ERR;

	for( i=0; i<siz; i++ ){
		if( item[i].id == id ){
			if( ( item[i].no + num ) > max ){
				return NULL;
			}
			return &item[i];
		}
		if( pos == ITEM_CHECK_ERR ){
			if( item[i].id == 0 && item[i].no == 0 ){
				pos = i;
			}
		}
	}

	if( pos == ITEM_CHECK_ERR ){
		return NULL;
	}
	return &item[pos];
}

//------------------------------------------------------------------
/**
 * @brief	�A�C�e����������ꏊ���擾
 * @param	myitem		�莝���A�C�e���\���̂ւ̃|�C���^
 * @param	item_no		�w�肷��A�C�e���̃i���o�[
 * @param	num			�A�C�e���̌�
 * @param	heap		�q�[�vID
 * @retval	���� = �A�C�e���ʒu
 * @retval	���s = NULL
 */
//------------------------------------------------------------------
static ITEM_ST * AddItemPosGet( MYITEM_PTR myitem, u16 item_no, u16 num, HEAPID heap )
{
	ITEM_ST * add;
	u32	max;
	u32	pocket;

	pocket = MyItemDataGet( myitem, item_no, &add, &max, heap );
	if( pocket == BAG_POKE_WAZA ){
		return AddItemSearch( add, max, item_no, num, ITEM_MAX_WAZAMACHINE );
	}
	return AddItemSearch( add, max, item_no, num, ITEM_MAX_NORMAL );
}

//------------------------------------------------------------------
/**
 * @brief	�莝���ɃA�C�e�����������邩�`�F�b�N
 * @param	myitem		�莝���A�C�e���\���̂ւ̃|�C���^
 * @param	item_no		�w�肷��A�C�e���̃i���o�[
 * @param	num			�A�C�e���̌�
 * @param	heap		�q�[�vID
 * @retval	TRUE	����
 * @retval	FALSE	���s
 */
//------------------------------------------------------------------
BOOL MYITEM_AddCheck( MYITEM_PTR myitem, u16 item_no, u16 num, HEAPID heap )
{
	if( AddItemPosGet( myitem, item_no, num, heap ) == NULL ){
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�莝���ɃA�C�e����������
 * @param	myitem		�莝���A�C�e���\���̂ւ̃|�C���^
 * @param	item_no		�w�肷��A�C�e���̃i���o�[
 * @param	num			�A�C�e���̌�
 * @param	heap		�q�[�vID
 * @retval	TRUE	����
 * @retval	FALSE	���s
 */
//------------------------------------------------------------------
BOOL MYITEM_AddItem( MYITEM_PTR myitem, u16 item_no, u16 num, HEAPID heap )
{
  int before = 0;
	ITEM_ST * add = AddItemPosGet( myitem, item_no, num, heap );

	if( add == NULL ){ return FALSE; }
  before = add->no;
	add->id = item_no;
	add->no += num;

	{
		u32	pocket;
		u32	max;
		pocket = MyItemDataGet( myitem, item_no, &add, &max, heap );
		if( pocket == BAG_POKE_NUTS || pocket == BAG_POKE_WAZA ){
			MYITEM_SortNumber( add, max );
		}
    else if(before == 0){  //�������A�C�e������ڂ������ꍇ�A���X�g�̍ŏ��ɂ����Ă���
      ITEM_ST * item;
      u32	num;
      MyPocketDataGet(myitem, pocket, &item, &num);
      num = MYITEM_GetItemThisPocketNumber(item, num);
      bottomInsert( item,  num );
    }
	}
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	��菜���A�C�e�����`�F�b�N
 * @param	item		�莝���A�C�e���\���̂ւ̃|�C���^
 * @param	siz			�莝���A�C�e���\���̂̃T�C�Y�i���j
 * @param	id			�w�肷��A�C�e���̃i���o�[
 * @param	num			�A�C�e���̌�
 * @retval	���� = �A�C�e���ʒu
 * @retval	���s = NULL
 */
//------------------------------------------------------------------
static ITEM_ST * SubItemSearch( ITEM_ST * item, u32 siz, u16 id, u16 num )
{
	u32	i;

	for( i=0; i<siz; i++ ){
		if( item[i].id == id ){
			if( item[i].no >= num ){
				return &item[i];
			}else{
				return NULL;
			}
		}
	}
	return NULL;
}

//------------------------------------------------------------------
/**
 * @brief	�A�C�e������菜���ꏊ���擾
 * @param	myitem		�莝���A�C�e���\���̂ւ̃|�C���^
 * @param	item_no		�w�肷��A�C�e���̃i���o�[
 * @param	num			�A�C�e���̌�
 * @param	heap		�q�[�vID
 * @retval	���� = �A�C�e���ʒu
 * @retval	���s = NULL
 */
//------------------------------------------------------------------
static ITEM_ST * SubItemPosGet( MYITEM_PTR myitem, u16 item_no, u16 num, HEAPID heap )
{
	ITEM_ST * sub;
	u32	max;

	MyItemDataGet( myitem, item_no, &sub, &max, heap );
	return SubItemSearch( sub, max, item_no, num );
}

//------------------------------------------------------------------
/**
 * @brief	�莝������A�C�e������菜��
 * @param	myitem		�莝���A�C�e���\���̂ւ̃|�C���^
 * @param	item_no		�w�肷��A�C�e���̃i���o�[
 * @param	num			�A�C�e���̌�
 * @param	heap		�q�[�vID
 * @retval	TRUE	����
 * @retval	FALSE	���s
 */
//------------------------------------------------------------------
BOOL MYITEM_SubItem( MYITEM_PTR myitem, u16 item_no, u16 num, HEAPID heap )
{
	ITEM_ST * sub = SubItemPosGet( myitem, item_no, num, heap );

	if( sub == NULL ){ return FALSE; }

	sub->no -= num;
	if( sub->no == 0 ){
		sub->id = 0;
	}

	{
		u32	max;

		MyItemDataGet( myitem, item_no, &sub, &max, heap );
		MYITEM_SortSpace( sub, max );
	}

	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�莝������A�C�e������菜���i�|�P�b�g�w��j
 * @param	myitem		�莝���A�C�e���\���̂ւ̃|�C���^
 * @param	max			������
 * @param	item_no		�w�肷��A�C�e���̃i���o�[
 * @param	num			�A�C�e���̌�
 * @param	heap		�q�[�vID
 * @retval	TRUE	����
 * @retval	FALSE	���s
 */
//------------------------------------------------------------------
BOOL MYITEM_SubItemDirect( ITEM_ST * myitem, u32 max, u16 item_no, u16 num, HEAPID heap )
{
	ITEM_ST * sub = SubItemSearch( myitem, max, item_no, num );

	if( sub == NULL ){ return FALSE; }

	sub->no -= num;
	if( sub->no == 0 ){
		sub->id = 0;
	}

	MYITEM_SortSpace( myitem, max );

	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�莝���A�C�e�����ł̑��݃`�F�b�N
 * @param	myitem		�莝���A�C�e���\���̂ւ̃|�C���^
 * @param	item_no		�w�肷��A�C�e���̃i���o�[
 * @param	num			�A�C�e���̌�
 * @param	heap		�q�[�vID
 * @retval	TRUE	���݂���
 * @retval	FALSE	���݂��Ȃ�
 */
//------------------------------------------------------------------
BOOL MYITEM_CheckItem( MYITEM_PTR myitem, u16 item_no, u16 num, HEAPID heap )
{
	if( SubItemPosGet( myitem, item_no, num, heap ) == NULL ){
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�|�P�b�g�ɃA�C�e�������݂��邩�`�F�b�N
 * @param	myitem		�莝���A�C�e���\���̂ւ̃|�C���^
 * @param	pocket		�|�P�b�g�ԍ�
 * @retval	TRUE	���݂���
 * @retval	FALSE	���݂��Ȃ�
 */
//------------------------------------------------------------------
BOOL MYITEM_CheckItemPocket( MYITEM_PTR myitem, u32 pocket )
{
	ITEM_ST * item;
	u32	max;
	u32	i;

	MyPocketDataGet(myitem, pocket, &item, &max);	
	if(item)
	{
		for( i=0; i<max; i++ )
		{
			if( item[i].id != 0 )
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�|�P�b�g�\���̂̃A�C�e����
 * @param	myitem		�|�P�b�g�\���̂ւ̃|�C���^
 * @return     ��
 */
//------------------------------------------------------------------

u32 MYITEM_GetItemThisPocketNumber( ITEM_ST * item,int max )
{
	u32	num=0;
	u32	i;

  if(item)
	{
		for( i=0; i < max; i++ )
		{
			if( item[i].id != ITEM_DUMMY_DATA )
			{
				num++;
			}
		}
	}
	return num;
}


//------------------------------------------------------------------
/**
 * @brief	�|�P�b�g�̃A�C�e����
 * @param	myitem		�莝���A�C�e���\���̂ւ̃|�C���^
 * @param	pocket		�|�P�b�g�ԍ�
 * @return     ��
 */
//------------------------------------------------------------------
u32 MYITEM_GetItemPocketNumber( MYITEM_PTR myitem, u32 pocket )
{
	ITEM_ST * item;
	u32	max,num=0;
	u32	i;

	MyPocketDataGet(myitem, pocket, &item, &max);	
	return MYITEM_GetItemThisPocketNumber(item, max);
}

//------------------------------------------------------------------
/**
 * @brief	�莝���̌��`�F�b�N
 * @param	myitem		�莝���A�C�e���\���̂ւ̃|�C���^
 * @param	item_no		�w�肷��A�C�e���̃i���o�[
 * @param	heap		�q�[�vID
 * @return	�莝����
 */
//------------------------------------------------------------------
u16 MYITEM_GetItemNum( MYITEM_PTR myitem, u16 item_no, HEAPID heap )
{
	ITEM_ST * sub = SubItemPosGet( myitem, item_no, 1, heap );

	if( sub == NULL ){
		return 0;
	}
	return sub->no;
}

//------------------------------------------------------------------
/**
 * @brief	�莝���̌��`�F�b�N�i�|�P�b�g�w��j
 * @param	myitem		�莝���A�C�e���\���̂ւ̃|�C���^
 * @param	max			������
 * @param	item_no		�w�肷��A�C�e���̃i���o�[
 * @param	num			�A�C�e���̌�
 * @param	heap		�q�[�vID
 * @retval	TRUE	����
 * @retval	FALSE	���s
 */
//------------------------------------------------------------------
u16 MYITEM_GetItemNumDirect( ITEM_ST * myitem, u32 max, u16 item_no, HEAPID heap )
{
	ITEM_ST * sub = SubItemSearch( myitem, max, item_no, 1 );

	if( sub == NULL ){ return 0; }

	return sub->no;
}

//------------------------------------------------------------------
/**
 * @brief	�w����̃A�C�e�������ւ�
 * @param	p1		����ւ���f�[�^1
 * @param	p2		����ւ���f�[�^2
 * @return	none
 */
//------------------------------------------------------------------
static void ChengeItem( ITEM_ST * p1, ITEM_ST * p2 )
{
	ITEM_ST	copy;

	copy = *p1;
	*p1  = *p2;
	*p2  = copy;
}

//------------------------------------------------------------------
/**
 * @brief	��ԉ��̃A�C�e������ɒu��
 * @param	item	�A�C�e���f�[�^
 * @param	max		�ő�l
 * @return	none
 */
//------------------------------------------------------------------
static void bottomInsert( ITEM_ST * item, const u32 num )
{
	int	i, j;
  ITEM_ST bottom;

  bottom.id = ITEM_DUMMY_DATA;

  if(num <= 1){
    return;   //�O��P�͂��K�v���Ȃ�
  }
  
	for(i = (num-1) ; i >= 0; i-- ){
    if(bottom.id == ITEM_DUMMY_DATA){
      GFL_STD_MemCopy(&item[i], &bottom, sizeof(ITEM_ST));
    }
    else{
      GFL_STD_MemCopy(&item[i], &item[i+1], sizeof(ITEM_ST));
    }
  }
  GFL_STD_MemCopy(&bottom, &item[0], sizeof(ITEM_ST));
}

//------------------------------------------------------------------
/**
 * @brief		�A�C�e���ԍ��O�Ɍ��������Ă������
 * @param		item	�A�C�e���f�[�^
 * @param		max		�ő�l
 * @return	none
 */
//------------------------------------------------------------------
static void DeleteNullItem( ITEM_ST * item, const u32 max )
{
	u32	i;

	for( i=0; i<max; i++ ){
		if( item[i].id == 0 ){
			item[i].no = 0;
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	�A�C�e���\�[�g�i�X�y�[�X���߂�j
 * @param	item	�A�C�e���f�[�^
 * @param	max		�ő�l
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_SortSpace( ITEM_ST * item, const u32 max )
{
	u32	i, j;

	DeleteNullItem( item, max );

	for( i=0; i<max-1; i++ ){
		for( j=i+1; j<max; j++ ){
			if( item[i].no == 0 ){
				ChengeItem( &item[i], &item[j] );
			}
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	�A�C�e���\�[�g�i�ԍ����j
 * @param	p	�|�P�b�g�f�[�^
 * @param	max		�ő�l
 * @return	none
 */	
//------------------------------------------------------------------
void MYITEM_SortNumber( ITEM_ST * item, const u32 max )
{
	u32	i, j;

	DeleteNullItem( item, max );

	for( i=0; i<max-1; i++ ){
		for( j=i+1; j<max; j++ ){
			if( item[i].no == 0 || ( item[j].no != 0 && item[i].id > item[j].id ) ){
				ChengeItem( &item[i], &item[j] );
			}
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief		�莝���A�C�e���S�̂�NULL�A�C�e���������ċl�߂�
 * @param		myitem	�莝���A�C�e���f�[�^
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_CheckSafety( MYITEM_PTR myitem )
{
	ITEM_ST * item;
	u32	max;

	// ��؂ȕ�
	MyPocketDataGet( myitem, BAG_POKE_EVENT, &item, &max );
	MYITEM_SortSpace( item, max );
	// ����
	MyPocketDataGet( myitem, BAG_POKE_NORMAL, &item, &max );
	MYITEM_SortSpace( item, max );
	// �؂̎�
	MyPocketDataGet( myitem, BAG_POKE_NUTS, &item, &max );
	MYITEM_SortSpace( item, max );
	// ��
	MyPocketDataGet( myitem, BAG_POKE_DRUG, &item, &max );
	MYITEM_SortSpace( item, max );
	// �Z�}�V��
	MyPocketDataGet( myitem, BAG_POKE_WAZA, &item, &max );
	MYITEM_SortSpace( item, max );
}


//------------------------------------------------------------------
/**
 * @brief	�o�b�O�̃f�[�^�쐬
 * @param	myitem		�莝���A�C�e���\���̂ւ̃|�C���^
 * @param	list		�|�P�b�g���X�g
 * @param	heap		�q�[�vID
 */
//------------------------------------------------------------------
#if 0	//�܂��o�b�O��ʂ͂Ȃ��̂łƂ肠�������� 2008.12.05(��) matsuda
void * MYITEM_MakeBagData( MYITEM_PTR myitem, const u8 * list, HEAPID heap )
{
	BAG_DATA * bag;
	int i;

	bag = BagSystemDataAlloc( heap );
	for( i=0; list[i] != 0xff; i++ ){
		switch( list[i] ){
		case BAG_POKE_EVENT:
			BagPocketDataMake( bag, myitem->MyEventItem, BAG_POKE_EVENT, i );
			break;
		case BAG_POKE_NORMAL:
			BagPocketDataMake( bag, myitem->MyNormalItem, BAG_POKE_NORMAL, i );
			break;
		case BAG_POKE_NUTS:
			BagPocketDataMake( bag, myitem->MyNutsItem, BAG_POKE_NUTS, i );
			break;
		case BAG_POKE_DRUG:
			BagPocketDataMake( bag, myitem->MyDrugItem, BAG_POKE_DRUG, i );
			break;
		case BAG_POKE_WAZA:
			BagPocketDataMake( bag, myitem->MySkillItem, BAG_POKE_WAZA, i );
			break;
		}
	}
	return bag;
}
#endif


//------------------------------------------------------------------
/**
 * @brief	�w��|�P�b�g�̎w��ʒu�̃A�C�e�����擾
 * @param	myitem		�莝���A�C�e���\���̂ւ̃|�C���^
 * @param	pocket		�|�P�b�gID
 * @param	pos			�ʒu
 * @return	�A�C�e���f�[�^
 */
//------------------------------------------------------------------
ITEM_ST * MYITEM_PosItemGet( MYITEM_PTR myitem, u16 pocket, u16 pos )
{
	ITEM_ST * item;
	u16	max;

	switch( pocket ){
	case BAG_POKE_EVENT:	// ��؂ȕ�
		item = myitem->MyEventItem;
		max  = BAG_EVENT_ITEM_MAX;
		break;
	case BAG_POKE_NORMAL:	// ����
		item = myitem->MyNormalItem;
		max  = BAG_NORMAL_ITEM_MAX;
		break;
	case BAG_POKE_NUTS:		// �؂̎�
		item = myitem->MyNutsItem;
		max  = BAG_NUTS_ITEM_MAX;
		break;
	case BAG_POKE_DRUG:		// ��
		item = myitem->MyDrugItem;
		max  = BAG_DRUG_ITEM_MAX;
		break;
	case BAG_POKE_WAZA:		// �Z�}�V��
		item = myitem->MySkillItem;
		max  = BAG_WAZA_ITEM_MAX;
		break;
	}

	if( pos >= max ){ return NULL; }

	return &item[pos];
}

//------------------------------------------------------------------
/**
 * @brief		�P�̃A�C�e���̍ő及�������擾
 *
 * @param		pocket	�|�P�b�g
 *
 * @return	�P�̃A�C�e���̍ő及����
 */
//------------------------------------------------------------------
u32 MYITEM_GetItemMax( u16 item )
{
	if( ITEM_CheckWazaMachine( item ) == TRUE ){
		return ITEM_MAX_WAZAMACHINE;
	}
	return ITEM_MAX_NORMAL;
}

//------------------------------------------------------------------
/**
 * @brief	�o�b�O�̃J�[�\���ʒu�f�[�^�m��
 * @param	heapID	�q�[�vID
 * @return	�J�[�\���ʒu�f�[�^
 */
//------------------------------------------------------------------
BAG_CURSOR * MYITEM_BagCursorAlloc( HEAPID heapID )
{
	return GFL_HEAP_AllocClearMemory( heapID, sizeof(BAG_CURSOR) );
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�o�b�O�̃J�[�\���ʒu�擾
 * @param	wk		�J�[�\���f�[�^
 * @param	pocket	�|�P�b�gID ITEMPOCKET_NORMAL�Ȃ� (item\itempocket_def.h)
 * @param	pos		�J�[�\���ʒu�擾�ꏊ
 * @param	scr		�X�N���[���J�E���^�擾�ꏊ
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_FieldBagCursorGet( BAG_CURSOR * wk, u16 pocket, s16 * pos, s16 * scr )
{
	*pos = wk->fld.pos[pocket];
	*scr = wk->fld.scr[pocket];
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�o�b�O�̃|�P�b�g�ʒu�擾
 * @param	wk		�J�[�\���f�[�^
 * @return	none
 */
//------------------------------------------------------------------
u16 MYITEM_FieldBagPocketGet( BAG_CURSOR * wk )
{
	return wk->fld.pocket;
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�o�b�O�̃J�[�\���ʒu�Z�b�g
 * @param	wk		�J�[�\���f�[�^
 * @param	pocket	�|�P�b�gID
 * @param	pos		�J�[�\���ʒu
 * @param	scr		�X�N���[���J�E���^
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_FieldBagCursorSet( BAG_CURSOR * wk, u16 pocket, s16 pos, s16 scr )
{
  GF_ASSERT(pocket >= 0);
  GF_ASSERT(pocket < BAG_POKE_MAX);
  GF_ASSERT(scr >= 0);
  GF_ASSERT(pos >= 0);
  wk->fld.pos[pocket] = pos;
	wk->fld.scr[pocket] = scr;
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�o�b�O�̃|�P�b�g�ʒu�Z�b�g
 * @param	wk		�J�[�\���f�[�^
 * @param	pocket	�|�P�b�gID
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_FieldBagPocketSet( BAG_CURSOR * wk, u16 pocket )
{
  GF_ASSERT(pocket >= 0);
  GF_ASSERT(pocket < BAG_POKE_MAX);
  wk->fld.pocket = pocket;
}

//------------------------------------------------------------------
/**
 * @brief	�퓬�o�b�O�̃J�[�\���ʒu�擾
 * @param	wk		�J�[�\���f�[�^
 * @param	pocket	�|�P�b�gID
 * @param	pos		�J�[�\���ʒu�擾�ꏊ
 * @param	scr		�X�N���[���J�E���^�擾�ꏊ
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_BattleBagCursorGet( BAG_CURSOR * wk, u16 pocket, s16 * pos, s16 * scr )
{
  *pos = wk->btl.pos[pocket];
	*scr = wk->btl.scr[pocket];
}

//------------------------------------------------------------------
/**
 * @brief	�퓬�o�b�O�̍Ō�Ɏg��������擾
 * @param	wk		�J�[�\���f�[�^
 * @return	�Ō�Ɏg��������
 */
//------------------------------------------------------------------
u16 MYITEM_BattleBagLastItemGet( BAG_CURSOR * wk )
{
	return wk->btl.item;
}

//------------------------------------------------------------------
/**
 * @brief	�퓬�o�b�O�̍Ō�Ɏg��������̃|�P�b�g�擾
 * @param	wk		�J�[�\���f�[�^
 * @return	�Ō�Ɏg��������̃|�P�b�g
 */
//------------------------------------------------------------------
u16 MYITEM_BattleBagLastPageGet( BAG_CURSOR * wk )
{
	return wk->btl.page;
}

//------------------------------------------------------------------
/**
 * @brief	�퓬�o�b�O�̃|�P�b�g�y�[�W�̃J�[�\���ʒu�擾
 * @param	wk		�J�[�\���f�[�^
 * @return	�Ō�Ɏg��������
 */
//------------------------------------------------------------------
/*
u16 MYITEM_BattleBagPocketPagePosGet( BAG_CURSOR * wk )
{
	return wk->btl.pocket;
}
*/

//------------------------------------------------------------------
/**
 * @brief	�퓬�o�b�O�̃J�[�\���ʒu�Z�b�g
 * @param	wk		�J�[�\���f�[�^
 * @param	pos		�J�[�\���ʒu
 * @param	scr		�X�N���[���J�E���^
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_BattleBagCursorSet( BAG_CURSOR * wk, s16 * pos, s16 * scr )
{
	u32	i;

	for( i=0; i<BATTLE_BAG_POKE_MAX; i++ ){
		wk->btl.pos[i] = pos[i];
		wk->btl.scr[i] = scr[i];
	}
}

//------------------------------------------------------------------
/**
 * @brief	�퓬�o�b�O�̃J�[�\���ʒu������
 * @param	wk		�J�[�\���f�[�^
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_BattleBagCursorPosInit( BAG_CURSOR * wk )
{
	u32	i;
	s16 p[BATTLE_BAG_POKE_MAX];


	for( i=0; i<BATTLE_BAG_POKE_MAX; i++ ){
		p[i] = 0;
	}
	MYITEM_BattleBagCursorSet( wk, p, p );
//	MYITEM_BattleBagPocketPagePosSet( wk, 0 );
}

//------------------------------------------------------------------
/**
 * @brief	�퓬�o�b�O�̍Ō�Ɏg����������Z�b�g
 * @param	wk		�J�[�\���f�[�^
 * @param	item	�Ō�Ɏg�����A�C�e��
 * @param	page	�Ō�Ɏg�����A�C�e���̃y�[�W
 * @return	none
 */
//------------------------------------------------------------------
void MYITEM_BattleBagLastItemSet( BAG_CURSOR * wk, u16 item, u16 page )
{
	wk->btl.item = item;
	wk->btl.page = page;
}

//------------------------------------------------------------------
/**
 * @brief	�퓬�o�b�O�̃|�P�b�g�y�[�W�̃J�[�\���ʒu�Z�b�g
 * @param	wk		�J�[�\���f�[�^
 * @return	�Ō�Ɏg��������
 */
//------------------------------------------------------------------
/*
void MYITEM_BattleBagPocketPagePosSet( BAG_CURSOR * wk, u16 pocket )
{
	wk->btl.pocket = pocket;
}
*/

// �O���Q�ƃC���f�b�N�X����鎞�̂ݗL��(�Q�[�����͖���)
#ifdef CREATE_INDEX
void *Index_Get_Myitem_Offset(MYITEM_PTR item, int type)
{
  switch(type){
  case BAG_POKE_NORMAL: return item->MyNormalItem; break;
  case BAG_POKE_EVENT: return item->MyEventItem; break;
  case BAG_POKE_WAZA: return item->MySkillItem; break;
  case BAG_POKE_DRUG: return item->MyDrugItem; break;
  case BAG_POKE_NUTS: return item->MyNutsItem; break;
  }
}
#endif


#if PM_DEBUG
//���̊֐���PDW�ŃZ�[�u�̃A�h���X�𓱂��������ߗp�ł�
//���Ȃ炸gamedata�̕����g���Ă�������
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
extern MYITEM_PTR* SaveData_GetMyItem(SAVE_CONTROL_WORK * sv);
//----------------------------------------------------------
/**
 * @brief	   �A�C�e���f�[�^�\���̂̎擾
 * @param	   sv		�Z�[�u�f�[�^�ւ̃|�C���^
 * @return	 WIFI_HISTORY�f�[�^�ւ̃|�C���^
 */
//----------------------------------------------------------
MYITEM_PTR* SaveData_GetMyItem(SAVE_CONTROL_WORK * sv)
{
  return (MYITEM_PTR*)SaveControl_DataPtrGet(sv, GMDATA_ID_MYITEM);
}

#endif

