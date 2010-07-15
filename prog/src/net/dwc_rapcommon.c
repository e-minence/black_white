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

#define DWCRAPCOMMON_SUBHEAP_GROUPID  0xea

typedef struct{
	NNSFndHeapHandle headHandle;
  void *heapPtr;

  NNSFndHeapHandle headHandleSub; //�T�u�q�[�v
  void          *heapPtrSub;
  DWCAllocType  SubAllocType;  //�T�u�q�[�v����m�ۂ���^�C�v
  u32           subheap_alloc_cnt;  //�������擾��
  u32           subheap_border_size; //
  BOOL          is_leak_err;
  HEAPID        auto_release_subheapID;
} DWCRAPCOMMON_WORK;

static DWCRAPCOMMON_WORK* pDwcRapWork = NULL;

static void DWC_RAPCOMMON_ResetSubHeapIDCore(void);
//----------------------------------------------------------------------------
/**
 *	@brief  DWC�̃T�u�q�[�v���쐬
 *	        �i�T�u�q�[�v�͓���̃^�C�v�̂݃A���P�[�g��������̂ł�
 *	        �@����AWIFI���ƃ����_���}�b�`���[�e�B���O�Ŏg�p���Ă���܂��j
 *
 *	@param	DWCAllocType SubAllocType ���̃q�[�v�ł���A���P�[�g����^�C�v
 *	@param	size                      �T�u�q�[�v�̃T�C�Y
 *	@param	heapID                    �q�[�vID
 */
//-----------------------------------------------------------------------------
void DWC_RAPCOMMON_SetSubHeapID( DWCAllocType SubAllocType, u32 size, HEAPID heapID )
{ 
  DWC_RAPCOMMON_SetSubHeapIDEx( SubAllocType, size, 0, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief  DWC�̃T�u�q�[�v���쐬 �擾�T�C�Y�ɏ���ݒ�������
 *	        name��DWC_ALLOCTYPE_GS�ł����Ă��}�b�`���C�N��SC���C�u������
 *	        �Q������ꍇ������B
 *	        �����������؂蕪���邽�߂�SC���C�u�����ő傫���T�C�Y�������Ƃ��̂�
 *	        �q�[�v��؂蕪����
 *	        ���̂��߂ɏ���ݒ���ł���֐���p�ӂ��܂���
 *
 *	@param	DWCAllocType SubAllocType ���̃q�[�v�ł���A���P�[�g����^�C�v
 *	@param	size                      �T�u�q�[�v�̃T�C�Y
 *	@param  border                    ���̒l�ȏ�̃A���P�[�g�Ȃ�ΐ؂蕪����
 *	@param	heapID                    �q�[�vID
 */
//-----------------------------------------------------------------------------
void DWC_RAPCOMMON_SetSubHeapIDEx( DWCAllocType SubAllocType, u32 size, u32 border_size, HEAPID heapID )
{
  GF_ASSERT_HEAVY( pDwcRapWork != NULL );
  GF_ASSERT_HEAVY( pDwcRapWork->heapPtrSub == NULL );
  GF_ASSERT_HEAVY( pDwcRapWork->headHandleSub == NULL );
  pDwcRapWork->heapPtrSub    = GFL_HEAP_AllocMemory(heapID, size-0x80);
  pDwcRapWork->headHandleSub = NNS_FndCreateExpHeap( (void *)( ((u32)pDwcRapWork->heapPtrSub + 31) / 32 * 32 ), size-0x80-64); 
  pDwcRapWork->SubAllocType = SubAllocType;

  NNS_FndSetAllocModeForExpHeap( pDwcRapWork->headHandleSub, NNS_FND_EXPHEAP_ALLOC_MODE_NEAR );
  NNS_FndSetGroupIDForExpHeap(
	      pDwcRapWork->headHandleSub,DWCRAPCOMMON_SUBHEAP_GROUPID );

  pDwcRapWork->subheap_border_size  = border_size;
}

//----------------------------------------------------------------------------
/**
 *	@brief  DWC�̃T�u�q�[�v��j��
 */
//-----------------------------------------------------------------------------
void DWC_RAPCOMMON_ResetSubHeapID(void)
{
  GF_ASSERT_HEAVY(pDwcRapWork);

  //�����j�������i�K�Ń��������c���Ă���΃G���[�ɂ���
  if( pDwcRapWork->subheap_alloc_cnt != 0 )
  {
    NAGI_Printf( "�I�I�q�[�v�����G���[�I�I\n" );
    GFL_NET_StateSetWifiError( 0, 0, 0, ERRORCODE_HEAP );


    //�����_���}�b�`�ŁA���|�[�g�̏������T�u�q�[�v�Ő؂蕪���Ă��܂�
    //�؂蕪���Ă���Ƃ��Ƀ��[�N���N����ꍇ
    //�}�b�`���C�N�̂��̂��T�u�q�[�v�Ɏc���Ă���\���������ł��B
    //�ł��̂ŁA���[�N���N�������ꍇ�͂����ɉ�������֍s�����A
    //Wi-Fi�̏I�����Ƀ��C���q�[�v�Ƃ܂Ƃ߂ăN���A���܂�
    pDwcRapWork->is_leak_err  = TRUE;

    //GF_ASSERT_MSG_HEAVY( 0, "SUB HEAP ERROR! LEAK!! count=%d\n", pDwcRapWork->subheap_alloc_cnt );
  }
  else
  {
    DWC_RAPCOMMON_ResetSubHeapIDCore();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  DWC�̃T�u�q�[�v����R�A����
 */
//-----------------------------------------------------------------------------
static void DWC_RAPCOMMON_ResetSubHeapIDCore(void)
{
  if( pDwcRapWork->heapPtrSub )
  {
    NNS_FndDestroyExpHeap( pDwcRapWork->headHandleSub );
    GFL_HEAP_FreeMemory( pDwcRapWork->heapPtrSub );
    pDwcRapWork->headHandleSub  = NULL;
    pDwcRapWork->heapPtrSub = NULL;

    if( pDwcRapWork->auto_release_subheapID != DWC_RAPCOMMON_AUTORELEASE_HEAPID_NONE )
    {
      GFL_HEAP_DeleteHeap( pDwcRapWork->auto_release_subheapID );
      pDwcRapWork->auto_release_subheapID = DWC_RAPCOMMON_AUTORELEASE_HEAPID_NONE;
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �T�u�q�[�v��Reset���Ƀq�[�v�̃f���[�g������ꍇ�̐ݒ�
 *
 *	@param	HEAPID heapID �q�[�vID
 */
//-----------------------------------------------------------------------------
void DWC_RAPCOMMON_SetAutoDeleteSubHeap( HEAPID heapID )
{
  pDwcRapWork->auto_release_subheapID = heapID;
}

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

  //�T�u�q�[�v���o�^����Ă��Ă��A�w�肵���A���P�[�g�^�C�v�A���w�肵���T�C�Y�ȏ�Ȃ��
  if( (pDwcRapWork->headHandleSub != NULL ) 
      && (name == pDwcRapWork->SubAllocType ) 
      && (size > pDwcRapWork->subheap_border_size ) )
  { 
    {
    OSIntrMode  enable = OS_DisableInterrupts();
    ptr = NNS_FndAllocFromExpHeapEx( pDwcRapWork->headHandleSub, size, align );
    (void)OS_RestoreInterrupts(enable);
    NET_MEMORY_PRINT( "sub alloc memory size=%d rest=%d %d\n", size, NNS_FndGetTotalFreeSizeForExpHeap(pDwcRapWork->headHandleSub),name );
    }
    pDwcRapWork->subheap_alloc_cnt++;
    NAGI_Printf( "sub heap alloc cnt=%d size=%d\n", pDwcRapWork->subheap_alloc_cnt, size );
  }
  else
  { 
    OSIntrMode  enable = OS_DisableInterrupts();
    ptr = NNS_FndAllocFromExpHeapEx( pDwcRapWork->headHandle, size, align );
    (void)OS_RestoreInterrupts(enable);

    NET_MEMORY_PRINT( "main alloc memory size=%d rest=%d %d\n", size, NNS_FndGetTotalFreeSizeForExpHeap(pDwcRapWork->headHandle),name );
  }

  //�c��q�[�v����
#ifdef DEBUG_ONLY_FOR_toru_nagihashi
  /*
  { 
    static u32 s_wifi_buff  = 0xFFFFFFFF;
    u32 free_area;
    free_area  = NNS_FndGetTotalFreeSizeForExpHeap( pDwcRapWork->headHandle );
    if( s_wifi_buff > free_area )
    { 
      s_wifi_buff = free_area;
    }
    NET_MEMORY_PRINT( "HEAP[WIFI] most low free area rest=0x%x\n", s_wifi_buff );
  }
  */
#endif //DEBUG_ONLY_FOR_toru_nagihashi


  if(ptr == NULL){  // �q�[�v�������ꍇ
//    GF_ASSERT_MSG_HEAVY(ptr,"dwcalloc not allocate! size %d,align %d rest %d name %d\n", size, align, NNS_FndGetTotalFreeSizeForExpHeap(pDwcRapWork->headHandle), name );
//    GFL_NET_StateSetError(GFL_NET_ERROR_RESET_SAVEPOINT);
//    returnNo = ERRORCODE_HEAP;
#ifdef BUGFIX_BTS7819_20100715
    GF_PANIC("%d\n",size);
    //DWCLIB���荞�ݓr����HEAP���Ȃ��Ȃ�Ǐ󂪂ł����߁B�G���[���o�������ł��Ȃ������̂�PANIC���ɂ��܂���
    //���O�ɃG���[��ʂ�p�ӂ��Ă����Ƃ��A���C�u������ς��Ă��炤�Ƃ��A�������𑝂₷�Ƃ� DUMMY�����܂��ă��C���ŃG���[�Ƃ�
    //���{�I�A�܂��͊�ȑΏ����K�v�ł�
#endif //BUGFIX_BTS7819_20100715
    GFL_NET_StateSetWifiError( 0, 0, 0, ERRORCODE_HEAP );  //�G���[�ɂȂ�
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
void DWC_RAPCOMMON_Free( DWCAllocType name, void *ptr, u32 size )
{	

  //NAGI_Printf( "[>name%d ptr%x size%d\n", name, ptr, size );

  if ( !ptr ){
    return;  //NULL�J����F�߂�
  }
  NET_MEMORY_PRINT( "free memory size=%d\n", size );

  //�T�u�q�[�v���o�^����Ă��Ă��A�w�肵���A���P�[�g�^�C�v��������
//  if( (pDwcRapWork->headHandleSub != NULL ) &&  (name == pDwcRapWork->SubAllocType ) )
  if( DWCRAPCOMMON_SUBHEAP_GROUPID == NNS_FndGetGroupIDForMBlockExpHeap(ptr) )
  { 
    if( pDwcRapWork->headHandleSub != NULL )
    {
      OSIntrMode  enable = OS_DisableInterrupts();
      NNS_FndFreeToExpHeap( pDwcRapWork->headHandleSub, ptr );
      (void)OS_RestoreInterrupts(enable);
      pDwcRapWork->subheap_alloc_cnt--;
      NAGI_Printf( "sub_heap alloc cnt=%d\n", pDwcRapWork->subheap_alloc_cnt );
    }
    else
    {
      //�����T�u�q�[�v�����݂��Ă��Ȃ��̂ɉ�����ɂ����ꍇ��
      //�q�[�v�G���[�Ƃ��Ă��܂�
      NAGI_Printf( "�I�I�q�[�v�����G���[�I�I\n" );
      GFL_NET_StateSetWifiError( 0, 0, 0, ERRORCODE_HEAP );

      //GF_ASSERT_MSG_HEAVY( 0, "SUB HEAP ERROR! NOT_SUBHEAP!! name=%d ptr=%x size=%d \n", name, ptr, size );
    }
  }
  else
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
  GF_ASSERT_HEAVY(pDwcRapWork==NULL);

  pDwcRapWork=GFL_HEAP_AllocClearMemory(GFL_HEAPID_SYSTEM,sizeof(DWCRAPCOMMON_WORK));
  pDwcRapWork->heapPtr = GFL_HEAP_AllocMemory(id, size-0x80);
  pDwcRapWork->headHandle = NNS_FndCreateExpHeap( (void *)( ((u32)pDwcRapWork->heapPtr + 31) / 32 * 32 ), size-0x80-64);

  pDwcRapWork->auto_release_subheapID = DWC_RAPCOMMON_AUTORELEASE_HEAPID_NONE;
  
}

//==============================================================================
/**
 * @brief   WIFI�Ŏg��HEAPID�����Z�b�g����
 * @param   id     �ύX����HEAPID
 * @retval  none
 */
//==============================================================================

void DWC_RAPCOMMON_ResetHeapID(void)
{
  GF_ASSERT_HEAVY(pDwcRapWork);

  if( pDwcRapWork->is_leak_err == TRUE
      || pDwcRapWork->heapPtrSub != NULL )
  {
    DWC_RAPCOMMON_ResetSubHeapIDCore();
  }

  NNS_FndDestroyExpHeap( pDwcRapWork->headHandle );
  GFL_HEAP_FreeMemory( pDwcRapWork->heapPtr );
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

  GF_ASSERT_HEAVY(userdata);
  if( DWC_CheckHasProfile( userdata )
      && DWC_CheckValidConsole( userdata ) ){
    return TRUE;
  }
  return FALSE;
}

#endif //GFL_NET_WIFI
