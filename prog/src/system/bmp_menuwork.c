//======================================================================
/**
 * @file	bmp_menu_list.h
 * @brief	bmp_menu �� bmp_list �Ŏg�p���镶����w��\���̂̏���
 * @author	taya
 * @date	2005.12.02
 *
 * bmp_menu �� bmp_list �őS�������^�E�����p�r�̍\���̂�ʌɒ�`���Ă���̂ł܂Ƃ߂Ă����B
 *
 *	�E2008.09 DP����WB�ֈڐA ���t�@�C�� bmp_menu_list.c,h
 *
 */
//======================================================================
#include "system/bmp_menuwork.h"

//==============================================================
// Prototype
//==============================================================
static BMP_MENULIST_DATA * SeekEmptyListPos(
		BMP_MENULIST_DATA* list, u32* heapID );

//======================================================================
//	�֐�
//======================================================================
//------------------------------------------------------------------
/**
 * ���X�g�o�b�t�@���쐬����B
 * �쐬��ABMP_MENULIST_AddArchiveString ��
 * BMP_MENULIST_AddString ���g���ĕ�������Z�b�g����
 *
 * @param   maxElems		���X�g�ɓo�^���镶����̍ő匏��
 * @param   heapID			���X�g�쐬��q�[�vID
 *
 * @retval  BMP_MENULIST_DATA*		�쐬���ꂽ���X�g�o�b�t�@
 */
//------------------------------------------------------------------
BMP_MENULIST_DATA * BmpMenuWork_ListCreate( u32 maxElems, u32 heapID )
{
	// �w�萔���P���߂ɍ���Ă����A�Ō�ɏI�[�R�[�h��u���ĊǗ�����
	BMP_MENULIST_DATA * list = GFL_HEAP_AllocClearMemory(
		heapID, sizeof(BMP_MENULIST_DATA)*(maxElems+1) );

	if( list )
	{
		u32 i;

		for(i=0; i<maxElems; i++)
		{
			list[i].str = NULL;
			list[i].param = 0;
		}

		list[i].str = LIST_ENDCODE;
		list[i].param = heapID;
	}
	return list;
}


//------------------------------------------------------------------
/**
 * ���X�g�o�b�t�@��j������
 * ��BMP_MENULIST_Create�ō쐬�������̈ȊO��n���Ă̓_���B
 *
 * @param   list		���X�g�o�b�t�@
 *
 */
//------------------------------------------------------------------
void BmpMenuWork_ListDelete( BMP_MENULIST_DATA* list_top )
{
	BmpMenuWork_ListSTRBUFDelete(list_top);
	GFL_HEAP_FreeMemory( list_top );
}

//------------------------------------------------------------------
/**
 * ���X�g�o�b�t�@�ɕ����񁕃p�����[�^���Z�b�g����i�}�l�[�W������ăA�[�J�C�u�f�[�^����ǂݍ��݁j
 *
 * @param   list		[in] ���X�g�o�b�t�@
 * @param   man			[in] ���b�Z�[�W�f�[�^�}�l�[�W��
 * @param   strID		������ID
 * @param   param		������p�����[�^
 *
 */
//------------------------------------------------------------------
void BmpMenuWork_ListAddArchiveString( BMP_MENULIST_DATA* list,
		GFL_MSGDATA *msgdata, u32 strID, u32 param, HEAPID heap_id )
{
	u32 dmy;

	list = SeekEmptyListPos(list, &dmy);

	if( list )
	{
#if 0
		u32 len = 32;	//msgdata�����������̂��K�v
		list->str = GFL_STR_CreateBuffer( len, heap_id );
		GFL_MSG_GetString( msgdata, strID, (STRBUF*)list->str );
#else
		list->str	= GFL_MSG_CreateString( msgdata, strID );
#endif
		list->param = param;
	}
}

#if 0	//dp
void BmpMenuList_AddArchiveString( BMP_MENULIST_DATA* list,
		const MSGDATA_MANAGER *man, u32 strID, u32 param )
{
	u32 dmy;

	list = SeekEmptyListPos(list, &dmy);

	if( list )
	{
		list->str = 
//		list->str = MSGMAN_AllocString( man, strID );
		list->param = param;
	}
}
#endif

//------------------------------------------------------------------
/**
 * ���X�g�o�b�t�@�ɕ����񁕃p�����[�^���Z�b�g����i������𒼐ڎw��j
 *
 * @param   list		[in] ���X�g�o�b�t�@
 * @param   str			[in] ������
 * @param   param		������p�����[�^
 *
 */
//------------------------------------------------------------------
void BmpMenuWork_ListAddString( BMP_MENULIST_DATA* list,
		const STRBUF *str, u32 param, HEAPID heapID  )
{
	u32 dmy;
	
	list = SeekEmptyListPos(list, &dmy );
	
	if( list )
	{
		list->str = GFL_STR_CreateCopyBuffer( str, heapID );
		list->param = param;
	}
}

#if 0
void BmpMenuList_AddString( BMP_MENULIST_DATA* list,
		const STRBUF* str, u32 param )
{
	u32 heapID;

	list = SeekEmptyListPos(list, &heapID);

	if( list )
	{
		list->str = STRBUF_CreateBufferCopy( str, heapID );
		list->param = param;
	}
}
#endif

//------------------------------------------------------------------
/**
 * ���X�g�o�b�t�@�ɑ��̃��X�g�̃f�[�^��Link����
 * @param   list		[inout] ���X�g�o�b�t�@
 * @param   insList		[in] ���X�g�|�C���^
 *
 */
//------------------------------------------------------------------
void BmpMenuWork_ListAddLinkList(
	BMP_MENULIST_DATA* list, const BMP_MENULIST_DATA* insList )
{
	u32 heapID;

	list = SeekEmptyListPos(list, &heapID);

	if( list )
	{
		list->str = insList->str;
		list->param = insList->param;
	}
}

//------------------------------------------------------------------
/**
 * ���X�g�̐擪����󂫂�����ʒu���V�[�N����
 *
 * @param   list		[in]  �󂫈ʒu�|�C���^�i�������NULL�j
 * @param   heapID		[out] ���X�g�쐬���ɕۑ������q�[�vID���󂯎��
 *
 * @retval  BMP_MENULIST_DATA*		
 */
//------------------------------------------------------------------
static BMP_MENULIST_DATA * SeekEmptyListPos(
		BMP_MENULIST_DATA* list, u32* heapID )
{
	BMP_MENULIST_DATA* ret;

	while( list->str != NULL )
	{
		if( list->str == LIST_ENDCODE )
		{
			GF_ASSERT(0);
			return NULL;
		}
		list++;
	}

	ret = list;

	while( list->str != LIST_ENDCODE )
	{
		list++;
	}

	*heapID = list->param;

	return ret;
}

//------------------------------------------------------------------
/**
 * ���X�g�o�b�t�@��STRBUF��j������
 * ��BMP_MENULIST_Create�ō쐬�������̈ȊO��n���Ă̓_���B
 *
 * @param   list		���X�g�o�b�t�@
 *
 */
//------------------------------------------------------------------
void BmpMenuWork_ListSTRBUFDelete( BMP_MENULIST_DATA *list_top )
{
	BMP_MENULIST_DATA* list = list_top;
	
	while( list->str != LIST_ENDCODE )
	{
		if( list->str == NULL )
		{
			break;
		}
//		STRBUF_Delete((STRBUF*)(list->str));
		GFL_STR_DeleteBuffer( (STRBUF*)list->str );
        list->str = NULL;
		list++;
	}
}

//--------------------------------------------------------------
/**
 * ���X�g�o�b�t�@�A���X�g�ő啶�������擾
 * @param list_top
 * @retval u32 
 */
//--------------------------------------------------------------
u32 BmpMenuWork_GetListMaxLength(
    const BMP_MENULIST_DATA *list_top, int *no_buf )
{
  u32 len,max = 0,no = 0,max_no = 0;
	const BMP_MENULIST_DATA* list = list_top;
  
  while( list->str != LIST_ENDCODE )
  {
    if( list->str == NULL )
    {
      break;
    }
    
    len = GFL_STR_GetBufferLength( (const STRBUF*)list->str );
    
    if( len > max )
    {
      max = len;
      max_no = no;
    }
  
    list++;
    no++;
  }
  
  *no_buf = max_no;
  return( max );
}

//--------------------------------------------------------------
/**
 * ���X�g�o�b�t�@�@���X�g�ő吔���擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
u32 BmpMenuWork_GetListMax( const BMP_MENULIST_DATA *list_top )
{
  u32 max = 0;
	const BMP_MENULIST_DATA* list = list_top;
  
  while( list->str != LIST_ENDCODE )
  {
    if( list->str != NULL )
    {
      max++;
    }
    list++;
  }
  return( max );
}

