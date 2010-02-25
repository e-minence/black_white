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

 

// �������̎c�ʕ\������ 1:ON  0:OFF
#if GFL_NET_DEBUG

#define NET_MEMORY_DEBUG  (0)

#else // GFL_NET_DEBUG

#define NET_MEMORY_DEBUG  (0)

#endif// GFL_NET_DEBUG 


#if NET_MEMORY_DEBUG
#define NET_MEMORY_PRINT(...) \
  if(GFL_NET_DebugGetPrintOn()) (void) ((OS_TPrintf(__VA_ARGS__)))
#else   //NET_MEMORY_DEBUG
#define NET_MEMORY_PRINT(...)           ((void) 0)
#endif  // NET_MEMORY_DEBUG


typedef struct{
	NNSFndHeapHandle headHandle;
  void *heapPtr;
} DWCRAPCOMMON_WORK;

static DWCRAPCOMMON_WORK* pDwcRapWork;

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
void* DWC_RAPCOMMON_Alloc( DWCAllocType name, u32 size, int align )
{	

  void *ptr;
  OSIntrMode  enable = OS_DisableInterrupts();
  ptr = NNS_FndAllocFromExpHeapEx( pDwcRapWork->headHandle, size, align );
  (void)OS_RestoreInterrupts(enable);

  if(ptr == NULL){
    GF_ASSERT_MSG(ptr,"dwcalloc not allocate! size %d,align %d rest %d name %d\n", size, align, NNS_FndGetTotalFreeSizeForExpHeap(pDwcRapWork->headHandle), name );
    // �q�[�v�������ꍇ�̏C��
    GFL_NET_StateSetError(GFL_NET_ERROR_RESET_SAVEPOINT);
    return NULL;
  }
  NET_MEMORY_PRINT( "alloc memory size=%d rest=%d\n", size, NNS_FndGetTotalFreeSizeForExpHeap(pDwcRapWork->headHandle) );
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
void DWC_RAPCOMMON_Free( DWCAllocType name, void *ptr, u32 size )
{	
#pragma unused( name, size )

  if ( !ptr ){
    return;  //NULL�J����F�߂�
  }
  NET_MEMORY_PRINT( "free memory size=%d\n", size );
  {
    OSIntrMode  enable = OS_DisableInterrupts();
    NNS_FndFreeToExpHeap( pDwcRapWork->headHandle, ptr );
    (void)OS_RestoreInterrupts(enable);
  }
}

//==============================================================================
/**
 * @brief   WIFI�Ŏg��HEAPID���Z�b�g����
 * @param   id     �ύX����HEAPID
 * @retval  none
 */
//==============================================================================

void DWC_RAPCOMMON_SetHeapID(HEAPID id,int size)
{
  GF_ASSERT(pDwcRapWork==NULL);

  pDwcRapWork=GFL_HEAP_AllocClearMemory(GFL_HEAPID_SYSTEM,sizeof(DWCRAPCOMMON_WORK));
  pDwcRapWork->heapPtr = GFL_HEAP_AllocMemory(id, size-0x80);
  pDwcRapWork->headHandle = NNS_FndCreateExpHeap( (void *)( ((u32)pDwcRapWork->heapPtr + 31) / 32 * 32 ), size-0x80-64);

  
}

//==============================================================================
/**
 * @brief   WIFI�Ŏg��HEAPID���Z�b�g����
 * @param   id     �ύX����HEAPID
 * @retval  none
 */
//==============================================================================

void DWC_RAPCOMMON_ResetHeapID(void)
{
  GF_ASSERT(pDwcRapWork);
  NNS_FndDestroyExpHeap( pDwcRapWork->headHandle );
  GFL_HEAP_FreeMemory( pDwcRapWork->heapPtr  );
  GFL_HEAP_FreeMemory(pDwcRapWork);
  pDwcRapWork=NULL;
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

  DWC_RAPCOMMON_SetHeapID(heapID, 0x40000);

  // DWC���C�u����������
#ifdef DEBUG_SERVER
	ret	= DWC_InitForDevelopment( GF_DWC_GAMENAME, GF_DWC_GAMECODE, DWC_RAPCOMMON_Alloc, DWC_RAPCOMMON_Free );
#else
	ret	= DWC_InitForProduction( GF_DWC_GAMENAME, GF_DWC_GAMECODE, DWC_RAPCOMMON_Alloc, DWC_RAPCOMMON_Free );
#endif

  DWC_RAPCOMMON_ResetHeapID();

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
