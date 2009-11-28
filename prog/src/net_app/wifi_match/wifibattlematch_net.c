//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_net.c
 *	@brief  WIFI�o�g���p�l�b�g���W���[��
 *	@author	Toru=Nagihashi
 *	@data		2009.11.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	���C�u����
#include <gflib.h>

//	�V�X�e��
#include "system/main.h"

//  �l�b�g���[�N
#include "net/network_define.h"
#include "net/dwc_rap.h"

//  �O�����J
#include "wifibattlematch_net.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�}�b�`�V�[�P���X
//=====================================
typedef enum
{
  WIFIBATTLEMATCH_NET_SEQ_MATCH_IDLE,
  WIFIBATTLEMATCH_NET_SEQ_MATCH_START,
  WIFIBATTLEMATCH_NET_SEQ_MATCH_WAIT,

  WIFIBATTLEMATCH_NET_SEQ_CONNECT_START,
  WIFIBATTLEMATCH_NET_SEQ_CONNECT_CHILD,
  WIFIBATTLEMATCH_NET_SEQ_CONNECT_PARENT,
  WIFIBATTLEMATCH_NET_SEQ_TIMING_END,

  WIFIBATTLEMATCH_NET_SEQ_MATCH_END,
} WIFIBATTLEMATCH_NET_SEQ;


//-------------------------------------
///	�}�b�`���C�N�L�[
//=====================================
typedef enum
{
  MATCHMAKE_KEY_BTL,  //�_�~�[
  MATCHMAKE_KEY_DEBUG,  //�_�~�[

  MATCHMAKE_KEY_MAX,
} MATCHMAKE_KEY;

#define WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT  15

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�F�B���w��s�A�}�b�`���C�N�p�ǉ��L�[�f�[�^�\����
//=====================================
typedef struct
{
  int ivalue;       // �L�[�ɑΉ�����l�iint�^�j
  u8  keyID;        // �}�b�`���C�N�p�L�[ID
  u8 pad;           // �p�f�B���O
  char keyStr[3];   // �}�b�`���C�N�p�L�[������
} MATCHMAKE_KEY_WORK;


//-------------------------------------
///	�l�b�g���W���[��
//=====================================
struct _WIFIBATTLEMATCH_NET_WORK
{ 
  u32 seq_matchmake;
  char filter[128];
  MATCHMAKE_KEY_WORK  key_wk[ MATCHMAKE_KEY_MAX ];
  HEAPID heapID;
};

//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================
//-------------------------------------
///	�}�b�`���C�N�L�[������
//=====================================
static const char *sc_matchmake_key_str[ MATCHMAKE_KEY_MAX ] =
{ 
  "bt",
  "db",
};


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
static void MATCHMAKE_KEY_Set( WIFIBATTLEMATCH_NET_WORK *p_wk, MATCHMAKE_KEY key, int value );
static int WIFIBATTLEMATCH_Eval_Callback( int index, void* p_param_adrs );

//=============================================================================
/**
 *					�������E�j��
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ������
 *
 *	@param	HEAPID heapID   �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
WIFIBATTLEMATCH_NET_WORK * WIFIBATTLEMATCH_NET_Init( HEAPID heapID )
{ 
  WIFIBATTLEMATCH_NET_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_NET_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_NET_WORK) );


  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j��
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_Exit( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 

  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C������
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_Main( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 

}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���C�N�J�n
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void WIFIBATTLEMATCH_NET_StartMatchMake( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  if( p_wk->seq_matchmake == WIFIBATTLEMATCH_NET_SEQ_MATCH_IDLE )
  { 
    GFL_NET_SetWifiBothNet(FALSE);

    MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_DEBUG, MATCHINGKEY );
    MATCHMAKE_KEY_Set( p_wk, MATCHMAKE_KEY_BTL, 0 );
    STD_TSPrintf( p_wk->filter, "bt=%d And db=%d", 0, MATCHINGKEY );
    p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_MATCH_START;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �}�b�`���C�N����
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_WaitMatchMake( WIFIBATTLEMATCH_NET_WORK *p_wk )
{ 
  switch( p_wk->seq_matchmake )
  { 
  case WIFIBATTLEMATCH_NET_SEQ_MATCH_IDLE:
    /* �������Ȃ�  */
    break;

  case WIFIBATTLEMATCH_NET_SEQ_MATCH_START:
    if( GFL_NET_DWC_StartMatchFilter( p_wk->filter, 2 ,&WIFIBATTLEMATCH_Eval_Callback, p_wk ) != 0 )
    {
      GFL_NET_DWC_SetVChat( FALSE );
      p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_MATCH_WAIT;
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_MATCH_WAIT:
    {
      int ret;

      ret = GFL_NET_DWC_GetStepMatchResult();
      switch(ret)
      {
      case STEPMATCH_CONTINUE:
        /* �ڑ��� */
        break;

      case STEPMATCH_SUCCESS:
        p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CONNECT_START;
        break;

      case STEPMATCH_CANCEL:
        GF_ASSERT( 0 );
        break;

      case STEPMATCH_FAIL:
        GF_ASSERT( 0 );
        break;
      }
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_CONNECT_START:
    if(GFL_NET_SystemGetCurrentID() != GFL_NET_NO_PARENTMACHINE){  // �q�@�Ƃ��Đڑ�����������
      if( GFL_NET_HANDLE_RequestNegotiation() )
      {
        p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CONNECT_CHILD;
      }
    }
    else
    {
      p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_CONNECT_PARENT;
    }
    break;


  case WIFIBATTLEMATCH_NET_SEQ_CONNECT_CHILD:
    GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT);
    p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_TIMING_END;
    break;

  case WIFIBATTLEMATCH_NET_SEQ_CONNECT_PARENT:
    if( GFL_NET_HANDLE_GetNumNegotiation() != 0 )
    {
      if( GFL_NET_HANDLE_RequestNegotiation() )
      {
        GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT);
        p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_TIMING_END;
      }
    }
    break;

  case WIFIBATTLEMATCH_NET_SEQ_TIMING_END:
    if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),WIFIBATTLEMATCH_NET_TIMINGSYNC_CONNECT))
    {
      p_wk->seq_matchmake = WIFIBATTLEMATCH_NET_SEQ_MATCH_END;
    }
    break;


  case WIFIBATTLEMATCH_NET_SEQ_MATCH_END:
    /* �������Ȃ� */
    return TRUE;
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G���[����
 *
 *	@param	const WIFIBATTLEMATCH_NET_WORK *cp_wk   ���[�N
 *
 *	@return TRUE�ŃG���[  FALSE�Œʏ�
 */
//-----------------------------------------------------------------------------
BOOL WIFIBATTLEMATCH_NET_IsError( const WIFIBATTLEMATCH_NET_WORK *cp_wk )
{ 
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �L�[��ݒ�
 *
 *	@param	WIFIBATTLEMATCH_NET_WORK *p_wk  ���[�N
 *	@param	key                             �L�[
 *	@param	value                           �l
 */
//-----------------------------------------------------------------------------
static void MATCHMAKE_KEY_Set( WIFIBATTLEMATCH_NET_WORK *p_wk, MATCHMAKE_KEY key, int value )
{ 
  MATCHMAKE_KEY_WORK *p_key_wk = &p_wk->key_wk[ key ];

  p_key_wk->ivalue  = value;
  GFL_STD_MemCopy( sc_matchmake_key_str[ key ], p_key_wk->keyStr, 2 );
  p_wk->key_wk[ key ].keyID  = DWC_AddMatchKeyInt( p_key_wk->keyID,
      p_key_wk->keyStr, &p_key_wk->ivalue );
  if( p_key_wk->keyID == 0 )
  {
      NAGI_Printf("AddMatchKey failed key=%d.value=%d\n",key, value);
  }
  
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ڑ��R�[���o�b�N
 *
 *	@param	int index
 *	@param	p_param_adrs 
 *
 *	@return �]���l  0�ȉ��͂Ȃ���Ȃ��B�@1�ȏ�͐����������قǐڑ�����₷���Ȃ�l
 *	                                        �i���������v���C���[�̕]���l���j
 *	                                        ���C�u����������8�r�b�g���V�t�g������8�r�b�g�ɗ����������
 */
//-----------------------------------------------------------------------------
static int WIFIBATTLEMATCH_Eval_Callback( int index, void* p_param_adrs )
{ 
  WIFIBATTLEMATCH_NET_WORK *p_wk  = p_param_adrs;

  int value=0;
/*
  targetlv = DWC_GetMatchIntValue(index,pWork->aMatchKey[_MATCHKEY_LEVEL].keyStr,-1);
  targetfriend = DWC_GetMatchIntValue(index,pWork->aMatchKey[_MATCHKEY_IMAGE_FRIEND].keyStr,-1);
  targetmy = DWC_GetMatchIntValue(index,pWork->aMatchKey[_MATCHKEY_IMAGE_MY].keyStr,-1);
*/
 
  //@todo   �����ɕ]������������
  value = 100;


  OS_TPrintf("�]���R�[���o�b�N %d \n",value );
  return value;

}
