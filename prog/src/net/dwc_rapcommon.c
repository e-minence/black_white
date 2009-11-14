//=============================================================================
/**
 * @file	dwc_rapcommon.c
 * @bfief	DWC���b�p�[�B�풓���W���[���ɒu������
 * @author	kazuki yoshihara  -> k.ohno����
 * @date	06/02/23
 */
//=============================================================================

#include <nitro.h>
#include <nnsys.h>
#include "gflib.h"
#include "net/network_define.h"
#include "system/main.h"

#if GFL_NET_WIFI
#include <dwc.h>

#include "net/dwc_rapcommon.h"

static HEAPID sc_heapID	= 0;

//----------------------------------------------------------------------------
/**
 *	@brief	DWC���C�u�����������ɓn���A���P�[�g�֐�
 *
 *	@param	DWCAllocType name	�������
 *	@param	size							�K�v�ȃT�C�Y
 *	@param	align							�K�v�ȃo�C�g�A���C�����g
 *
 *	@return	�m�ې��������ꍇ�A�A���C�����g���ꂽ�o�b�t�@�B�@���s�����ꍇNULL
 */
//-----------------------------------------------------------------------------
static void* mydwc_alloc( DWCAllocType name, u32 size, int align )
{	
#pragma unused( name )

	void * ptr;
  OSIntrMode old;

#ifdef DEBUGPRINT_ON
  NET_PRINT("HEAP�擾(%d, %d) \n", size, align);
#endif

	GF_ASSERT_MSG( sc_heapID != 0, "DWC�QAlloc�֐����q�[�vID�ݒ肳�ꂸ�ɌĂ΂�܂���" );
	GF_ASSERT(align <= 32);  // �������������đΉ�
	old = OS_DisableInterrupts();
  ptr = GFL_NET_Align32Alloc(sc_heapID, size);
	OS_RestoreInterrupts( old );

  if(ptr == NULL){
    GF_ASSERT(ptr);
    // �q�[�v�������ꍇ�̏C��
    GFL_NET_StateSetError(GFL_NET_ERROR_RESET_SAVEPOINT);
    return NULL;
  }
  return ptr;


}

//----------------------------------------------------------------------------
/**
 *	@brief	DWC���C�u�����������ɓn���t���[�֐�
 *
 *	@param	DWCAllocType name	����������
 *	@param	ptr								�������m�ۊ֐��Ŋm�ۂ��ꂽ�o�b�t�@�̃|�C���^
 *	@param	size							����������
 */
//-----------------------------------------------------------------------------
static void mydwc_free( DWCAllocType name, void *ptr, u32 size )
{	
#pragma unused( name, size )
  OSIntrMode old;

  if ( !ptr ){
    return;  //NULL�J����F�߂�
  }
  old = OS_DisableInterrupts();
  GFL_NET_Align32Free(ptr);
  OS_RestoreInterrupts( old );
}

//==============================================================================
/**
 * @brief   WIFI�Ŏg��HEAPID���Z�b�g����
 * @param   id     �ύX����HEAPID
 * @retval  none
 */
//==============================================================================

void DWC_RAPCOMMON_SetHeapID(HEAPID id)
{
  sc_heapID = id;
}

//==============================================================================
/**
 * DWC���C�u����������
 * @param   GFL_WIFI_FRIENDLIST  ���[�U�[�f�[�^���Ȃ��ꍇ�쐬
 * @retval  DS�{�̂ɕۑ����郆�[�UID�̃`�F�b�N�E�쐬���ʁB
 */
//==============================================================================
int mydwc_init(HEAPID heapID)
{
  int ret;
  u8* pWork;
  u8* pTemp;

	sc_heapID	= heapID;

  // DWC���C�u����������
#ifdef DEBUG_SERVER
	ret	= DWC_InitForDevelopment( GF_DWC_GAMENAME, GF_DWC_GAMECODE, mydwc_alloc, mydwc_free );
#else
	ret	= DWC_InitForProduction( GF_DWC_GAMENAME, GF_DWC_GAMECODE, mydwc_alloc, mydwc_free );
#endif

  sc_heapID = 0;

  return ret;
}

//==============================================================================
/**
 * @brief   DWC  UserData�����
 * @param   DWCUserData  ���[�U�[�f�[�^���Ȃ��ꍇ�쐬
 * @retval  none
 */
//==============================================================================
void GFL_NET_WIFI_InitUserData( void *userdata )
{
  DWCUserData *pUserdata = userdata;

  // ���[�U�f�[�^�쐬������B
  MI_CpuClearFast(userdata, sizeof(DWCUserData));
  if( !DWC_CheckUserData( userdata ) ){
    DWC_CreateUserData( userdata );
    DWC_ClearDirtyFlag( userdata );
    NET_PRINT("WIFI���[�U�f�[�^���쐬\n");
  }
}

//==============================================================================
/**
 * ������GSID���������̂���������
 * @param   GFL_WIFI_FRIENDLIST
 * @retval  TRUE������
 */
//==============================================================================
BOOL mydwc_checkMyGSID(void)
{
  DWCUserData *userdata = GFI_NET_GetMyDWCUserData();

  GF_ASSERT(userdata);
  if( DWC_CheckHasProfile( userdata )
      && DWC_CheckValidConsole( userdata ) ){
    return TRUE;
  }
  return FALSE;
}

#endif GFL_NET_WIFI
