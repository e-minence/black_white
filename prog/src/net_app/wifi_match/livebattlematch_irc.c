//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		livebattlematch_flow.c
 *	@brief  ���C�u���ԊO���ʐM���W���[��
 *	@author	Toru=Nagihashi
 *	@data		2010.03.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/main.h"

//�l�b�g
#include "net/network_define.h"
#include "net/dreamworld_netdata.h"
#include "net/delivery_irc.h"

//�O�����J
#include "livebattlematch_irc.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�ėp��M�o�b�t�@
//=====================================
#define LIVEBATTLEMATCH_IRC_RECV_BUFFER_LEN (0x1000)

//-------------------------------------
///	��M�t���O
//=====================================
enum
{ 
  LIVEBATTLEMATCH_IRC_RECV_FLAG_ENEMYDATA,
  LIVEBATTLEMATCH_IRC_RECV_FLAG_POKEPARTY,
  LIVEBATTLEMATCH_IRC_RECV_FLAG_REGULATION,

  LIVEBATTLEMATCH_IRC_RECV_FLAG_MAX
};

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�ԊO�����W���[��
//=====================================
struct _LIVEBATTLEMATCH_IRC_WORK 
{ 
  u32 seq;
  HEAPID  heapID;
  u8  recv_buffer[LIVEBATTLEMATCH_IRC_RECV_BUFFER_LEN];
  BOOL is_recv[LIVEBATTLEMATCH_IRC_RECV_FLAG_MAX];
  DELIVERY_IRC_WORK *p_delivery;
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	��M�R�[���o�b�N
//=====================================
static void LiveBattleMatch_RecvCallback_RecvEnemyData( const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE *p_handle );
static void LiveBattleMatch_RecvCallback_RecvPokeParty( const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE *p_handle );
static void LiveBattleMatch_RecvCallback_RecvRegulation( const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE *p_handle );
static u8* LiveBattleMatch_RecvCallback_GetRecvBuffer( int netID, void* p_wk_adrs, int size );

//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================
//-------------------------------------
///	��M�R�[���o�b�N
//=====================================
typedef enum
{
  LIVEBATTLEMATCH_IRC_SEND_CMD_ENEMYDATA = GFL_NET_CMD_IRC_BATTLE,
  LIVEBATTLEMATCH_IRC_SEND_CMD_POKEPARTY,
  LIVEBATTLEMATCH_IRC_SEND_CMD_REGULATION,
  
  LIVEBATTLEMATCH_IRC_SEND_CMD_MAX,
}LIVEBATTLEMATCH_IRC_SEND_CMD;
static const NetRecvFuncTable LIVEBATTLEMATCH_IRC_RecvFuncTable[] =
{
  { LiveBattleMatch_RecvCallback_RecvEnemyData, LiveBattleMatch_RecvCallback_GetRecvBuffer },
  { LiveBattleMatch_RecvCallback_RecvPokeParty, LiveBattleMatch_RecvCallback_GetRecvBuffer },
  { LiveBattleMatch_RecvCallback_RecvRegulation, LiveBattleMatch_RecvCallback_GetRecvBuffer },
};

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//-------------------------------------
///	���[�N�쐬�E�j���E����
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  IRC���W���[�� ������
 *
 *	@param	GAMEDATA *p_gamedata    �Q�[���f�[�^
 *	@param	heapID                  �q�[�vID 
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
LIVEBATTLEMATCH_IRC_WORK *LIVEBATTLEMATCH_IRC_Init( GAMEDATA *p_gamedata, HEAPID heapID )
{
  LIVEBATTLEMATCH_IRC_WORK *p_wk;

  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(LIVEBATTLEMATCH_IRC_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(LIVEBATTLEMATCH_IRC_WORK) );
  p_wk->heapID  = heapID;

  //���̃��W���[���̓l�b�g�ڑ����ɏ������Ɣj�����s���̂ŁA
  //����R�}���h�e�[�u����ǉ��E�j�����邱�ƂŁA
  //��M�R�[���o�b�N���̃��[�N�A�h���X���ێ�����
  if( GFL_NET_IsInit() )
  { 
    GFL_NET_AddCommandTable( GFL_NET_CMD_IRC_BATTLE, LIVEBATTLEMATCH_IRC_RecvFuncTable, NELEMS(LIVEBATTLEMATCH_IRC_RecvFuncTable), p_wk );
  }

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  IRC���W���[�� �j��
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void LIVEBATTLEMATCH_IRC_Exit( LIVEBATTLEMATCH_IRC_WORK *p_wk )
{ 
  if( GFL_NET_IsInit() )
  { 
    GFL_NET_DelCommandTable( GFL_NET_CMD_IRC_BATTLE );
  }

  GFL_HEAP_FreeMemory( p_wk );

}
//----------------------------------------------------------------------------
/**
 *	@brief  IRC���W���[�� ���C������
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void LIVEBATTLEMATCH_IRC_Main( LIVEBATTLEMATCH_IRC_WORK *p_wk )
{ 

}

//-------------------------------------
///	�G���[
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  IRC���W���[�� �G���[���G���[�C���^�C�v��Ԃ�
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk  ���[�N
 *
 *	@return �G���[�C���^�C�v
 */
//-----------------------------------------------------------------------------
LIVEBATTLEMATCH_IRC_ERROR_REPAIR_TYPE LIVEBATTLEMATCH_IRC_CheckErrorRepairType( LIVEBATTLEMATCH_IRC_WORK *p_wk )
{
  //@todo
  return  LIVEBATTLEMATCH_IRC_ERROR_REPAIR_NONE;
}

//-------------------------------------
///	�ڑ��E�ؒf
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �ڑ��J�n
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk  ���[�N
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL LIVEBATTLEMATCH_IRC_StartConnect( LIVEBATTLEMATCH_IRC_WORK *p_wk )
{ 
  p_wk->seq  = 0;
  return TRUE;
}
enum
{ 
  SEQ_START_INIT,
  SEQ_WAIT_INIT,
  SEQ_ADD_CMD,
  SEQ_START_CONNECT,
  SEQ_WAIT_CONNECT,
  SEQ_END,
};
//----------------------------------------------------------------------------
/**
 *	@brief  �ڑ�����
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk  ���[�N
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL LIVEBATTLEMATCH_IRC_WaitConnect( LIVEBATTLEMATCH_IRC_WORK *p_wk )
{ 
  static const GFLNetInitializeStruct aGFLNetInit = 
  {
    NULL,  // ��M�֐��e�[�u��
    0, //��M�e�[�u���v�f��
    NULL,   ///< �n�[�h�Őڑ��������ɌĂ΂��
    NULL,   ///< �l�S�V�G�[�V�����������ɃR�[��
    NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    NULL,//IrcBattleBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
    NULL,//IrcBattleBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    NULL,//IrcBattleBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
    NULL,//FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    NULL, //_endCallBack,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,   ///< DWC�`���̗F�B���X�g
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
    SYASHI_NETWORK_GGID,  //ggid  
    GFL_HEAPID_APP,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    HEAPID_IRC,   //�ԊO���ʐM�p��create�����HEAPID
    GFL_WICON_POSX, GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    LIVEBATTLEMATCH_IRC_CONNECT_NUM,     // �ő�ڑ��l��
    100,  //�ő呗�M�o�C�g��
    16,    // �ő�r�[�R�����W��
    TRUE,     // CRC�v�Z
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    GFL_NET_TYPE_IRC,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_IRC_BATTLE,  //GameServiceID
    0xfffe, // �ԊO���^�C���A�E�g����
    0,//MP�ʐM�e�@���M�o�C�g��
    0,//dummy
  };

  switch( p_wk->seq )
  { 
  case SEQ_START_INIT:
    GFL_NET_Init( &aGFLNetInit, NULL, NULL );
    p_wk->seq = SEQ_WAIT_INIT;
    break;

  case SEQ_WAIT_INIT:
    if( GFL_NET_IsInit() )
    { 
      p_wk->seq  = SEQ_ADD_CMD;
    }
    break;

  case SEQ_ADD_CMD:
    GFL_NET_AddCommandTable( GFL_NET_CMD_IRC_BATTLE, LIVEBATTLEMATCH_IRC_RecvFuncTable, NELEMS(LIVEBATTLEMATCH_IRC_RecvFuncTable), p_wk );
    p_wk->seq = SEQ_START_CONNECT;
    break;

  case SEQ_START_CONNECT:
    GFL_NET_ChangeoverConnect( NULL );
    p_wk->seq = SEQ_WAIT_CONNECT;
    break;

  case SEQ_WAIT_CONNECT:
    if( GFL_NET_GetConnectNum() > 1 )
    { 
      p_wk->seq = SEQ_END;
    }
    break;

  case SEQ_END:
    return TRUE;
  }


  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ڑ��L�����Z������  �J�n
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void LIVEBATTLEMATCH_IRC_StartCancelConnect( LIVEBATTLEMATCH_IRC_WORK *p_wk )
{
  if( p_wk->seq >= SEQ_ADD_CMD )
  { 
    GFL_NET_DelCommandTable( GFL_NET_CMD_IRC_BATTLE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ڑ��L�����Z������  ������
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk ���[�N
 *
 *	@return TRUE�Ŋ���  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL LIVEBATTLEMATCH_IRC_WaitCancelConnect( LIVEBATTLEMATCH_IRC_WORK *p_wk )
{ 
  enum
  { 
    SEQ_START_EXIT,
    SEQ_WAIT_EXIT,
    SEQ_END,
  };

  switch( p_wk->seq )
  { 
  case SEQ_START_EXIT:
    if( GFL_NET_IsInit() )
    { 
      GFL_NET_Exit( NULL );
      p_wk->seq = SEQ_WAIT_EXIT;
    }
    else
    { 
      p_wk->seq = SEQ_END;
    }
    break;

  case SEQ_WAIT_EXIT:
    if( GFL_NET_IsExit() )
    { 
      p_wk->seq = SEQ_END;
    }
    break;

  case SEQ_END:
    return TRUE;
  }

  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ؒf�J�n
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk  ���[�N
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL LIVEBATTLEMATCH_IRC_StartDisConnect( LIVEBATTLEMATCH_IRC_WORK *p_wk )
{ 
  p_wk->seq  = 0;
  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ؒf����
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk  ���[�N
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL LIVEBATTLEMATCH_IRC_WaitDisConnect( LIVEBATTLEMATCH_IRC_WORK *p_wk )
{ 
  enum
  { 
    SEQ_START_EXIT,
    SEQ_WAIT_EXIT,
    SEQ_END,
  };

  switch( p_wk->seq )
  { 
  case SEQ_START_EXIT:
    GFL_NET_DelCommandTable( GFL_NET_CMD_IRC_BATTLE );
    GFL_NET_Exit( NULL );
    p_wk->seq = SEQ_WAIT_EXIT;
    break;

  case SEQ_WAIT_EXIT:
    if( GFL_NET_IsExit() )
    { 
      p_wk->seq = SEQ_END;
    }
    break;

  case SEQ_END:
    return TRUE;
  }

  return FALSE;
}



//-------------------------------------
///	����̃f�[�^��M  ���݂��̃f�[�^�𑗂�  SENDDATA�n
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃f�[�^�𑊎�ɑ���
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk  ���[�N
 *	@param	void *cp_buff                   ���M�f�[�^
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL LIVEBATTLEMATCH_IRC_StartEnemyData( LIVEBATTLEMATCH_IRC_WORK *p_wk, const void *cp_buff )
{
  NetID netID;

  //����ɂ̂ݑ��M
  netID = GFL_NET_GetNetID( GFL_NET_HANDLE_GetCurrentHandle() );
  netID = netID == 0? 1: 0;
  return GFL_NET_SendDataEx( GFL_NET_HANDLE_GetCurrentHandle(), netID, LIVEBATTLEMATCH_IRC_SEND_CMD_ENEMYDATA, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE, cp_buff, TRUE, TRUE, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ����̃f�[�^����M�܂�
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk  ���[�N
 *	@param	**pp_data                       ��M�o�b�t�@�̃|�C���^�󂯎��|�C���^
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL LIVEBATTLEMATCH_IRC_WaitEnemyData( LIVEBATTLEMATCH_IRC_WORK *p_wk, WIFIBATTLEMATCH_ENEMYDATA **pp_data )
{
  BOOL ret;
  ret = p_wk->is_recv[LIVEBATTLEMATCH_IRC_RECV_FLAG_ENEMYDATA];

  if( ret )
  { 
    *pp_data  = (WIFIBATTLEMATCH_ENEMYDATA *)p_wk->recv_buffer;
  }
  return ret;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �����̃|�P�����p�[�e�B�𑊎�֑���
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk  ���[�N
 *	@param	POKEPARTY *cp_party             ���M�f�[�^
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL LIVEBATTLEMATCH_IRC_SendPokeParty( LIVEBATTLEMATCH_IRC_WORK *p_wk, const POKEPARTY *cp_party )
{
  NetID netID;

  //����ɂ̂ݑ��M
  netID = GFL_NET_GetNetID( GFL_NET_HANDLE_GetCurrentHandle() );
  netID = netID == 0? 1: 0;
  return GFL_NET_SendDataEx( GFL_NET_HANDLE_GetCurrentHandle(), netID, LIVEBATTLEMATCH_IRC_SEND_CMD_POKEPARTY, PokeParty_GetWorkSize(), cp_party, TRUE, TRUE, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ����̃|�P�����p�[�e�B����M����
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk  ���[�N
 *	@param	*p_party                        ��M�f�[�^�󂯎��
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL LIVEBATTLEMATCH_IRC_RecvPokeParty( LIVEBATTLEMATCH_IRC_WORK *p_wk, POKEPARTY *p_party )
{
  BOOL ret;
  ret = p_wk->is_recv[LIVEBATTLEMATCH_IRC_RECV_FLAG_POKEPARTY];

  if( ret )
  { 
    GFL_STD_MemCopy( p_wk->recv_buffer, p_party, PokeParty_GetWorkSize() );
  }
  return ret;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ���M�����[�V��������M�J�n  GFL_NET_Init����Ă��Ȃ�����
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk  ���[�N
 *	@param	*p_recv                         ��M�f�[�^�󂯎��
 *
 */
//-----------------------------------------------------------------------------
void LIVEBATTLEMATCH_IRC_StartRecvRegulation( LIVEBATTLEMATCH_IRC_WORK *p_wk, REGULATION_CARDDATA *p_recv )
{ 
  DELIVERY_IRC_INIT init;
  GFL_STD_MemClear( &init, sizeof(DELIVERY_IRC_INIT) );
  init.NetDevID = WB_NET_IRC_BATTLE;
  init.datasize = sizeof(REGULATION_CARDDATA);
  init.pData  = (u8*)p_recv;
  init.ConfusionID  = 0;
  init.heapID = p_wk->heapID;

  GF_ASSERT( p_wk->p_delivery == NULL );
  p_wk->p_delivery  = DELIVERY_IRC_Init(&init);
  p_wk->seq = 0; 
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���M�����[�V��������M�I��
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk  ���[�N
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL LIVEBATTLEMATCH_IRC_WaitRecvRegulation( LIVEBATTLEMATCH_IRC_WORK *p_wk )
{ 
  enum
  { 
    SEQ_START_RECV,
    SEQ_WAIT_RECV,
    SEQ_DELETE,
    SEQ_END,
  };

  if( p_wk->p_delivery )
  { 
    DELIVERY_IRC_Main( p_wk->p_delivery );
  }

  switch( p_wk->seq )
  { 
  case SEQ_START_RECV:
    DELIVERY_IRC_RecvStart(p_wk->p_delivery);
    p_wk->seq++;
    break;

  case SEQ_WAIT_RECV:
    if( DELIVERY_IRC_RecvCheck( p_wk->p_delivery ) )
    { 
      p_wk->seq++;
    }
    break;

  case SEQ_DELETE:
    DELIVERY_IRC_End( p_wk->p_delivery );
    p_wk->p_delivery  = NULL;
    p_wk->seq++;
    break;

  case SEQ_END:
    if( GFL_NET_IsResetEnable() )
    { 
      return TRUE;
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M�����[�V������M���L�����Z��  �J�n
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk  ���[�N
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
void LIVEBATTLEMATCH_IRC_StartCancelRecvRegulation( LIVEBATTLEMATCH_IRC_WORK *p_wk )
{ 
  p_wk->seq = 0; 
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���M�����[�V������M���L�����Z��  �I��
 *
 *	@param	LIVEBATTLEMATCH_IRC_WORK *p_wk  ���[�N
 *
 *	@return TRUE�ŏI��  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL LIVEBATTLEMATCH_IRC_WaitCancelRecvRegulation( LIVEBATTLEMATCH_IRC_WORK *p_wk )
{ 
  enum
  { 
    SEQ_START,
    SEQ_WAIT,
  };

  if( p_wk->p_delivery )
  { 
    DELIVERY_IRC_Main( p_wk->p_delivery );
  }

  switch( p_wk->seq )
  { 
  case SEQ_START:
    if( p_wk->p_delivery )
    { 
      DELIVERY_IRC_End( p_wk->p_delivery );
      p_wk->p_delivery  = NULL;
    }
    p_wk->seq++;
    break;

  case SEQ_WAIT:
    if( GFL_NET_IsResetEnable() )
    { 
      return TRUE;
    }
  }

  return FALSE;
}

//=============================================================================
/**
 *    ��M�R�[���o�b�N
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �G�̃f�[�^��M�R�[���o�b�N
 *
 *	@param	const int netID       �l�b�gID
 *	@param	int size              �f�[�^�T�C�Y
 *	@param	void* cp_data_adrs    �f�[�^�A�h���X
 *	@param	p_wk_adrs             INIT�ŗ^�������[�N
 *	@param	*p_handle             �l�b�g�n���h��
 */
//-----------------------------------------------------------------------------
static void LiveBattleMatch_RecvCallback_RecvEnemyData( const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE *p_handle )
{ 
  LIVEBATTLEMATCH_IRC_WORK *p_wk  = p_wk_adrs;

  if( p_handle != GFL_NET_HANDLE_GetCurrentHandle() )
  {
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if( netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()) )
  {
    return;//�����͍̂��͎󂯎��Ȃ�
  }

  p_wk->is_recv[LIVEBATTLEMATCH_IRC_RECV_FLAG_ENEMYDATA] = TRUE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �|�P�p�[�e�B��M�R�[���o�b�N
 *
 *	@param	const int netID       �l�b�gID
 *	@param	int size              �f�[�^�T�C�Y
 *	@param	void* cp_data_adrs    �f�[�^�A�h���X
 *	@param	p_wk_adrs             INIT�ŗ^�������[�N
 *	@param	*p_handle             �l�b�g�n���h��
 */
//-----------------------------------------------------------------------------
static void LiveBattleMatch_RecvCallback_RecvPokeParty( const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE *p_handle )
{ 
  LIVEBATTLEMATCH_IRC_WORK *p_wk  = p_wk_adrs;

  if( p_handle != GFL_NET_HANDLE_GetCurrentHandle() )
  {
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if( netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()) )
  {
    return;//�����͍̂��͎󂯎��Ȃ�
  }

  p_wk->is_recv[LIVEBATTLEMATCH_IRC_RECV_FLAG_POKEPARTY] = TRUE;
}
//----------------------------------------------------------------------------
/**
 *  @brief  ���M�����[�V������M�R�[���o�b�N
 *
 *	@param	const int netID       �l�b�gID
 *	@param	int size              �f�[�^�T�C�Y
 *	@param	void* cp_data_adrs    �f�[�^�A�h���X
 *	@param	p_wk_adrs             INIT�ŗ^�������[�N
 *	@param	*p_handle             �l�b�g�n���h��
 */
//-----------------------------------------------------------------------------
static void LiveBattleMatch_RecvCallback_RecvRegulation( const int netID, const int size, const void* cp_data_adrs, void* p_wk_adrs, GFL_NETHANDLE *p_handle )
{ 
  LIVEBATTLEMATCH_IRC_WORK *p_wk  = p_wk_adrs;

  if( p_handle != GFL_NET_HANDLE_GetCurrentHandle() )
  {
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if( netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()) )
  {
    return;//�����͍̂��͎󂯎��Ȃ�
  }

  p_wk->is_recv[LIVEBATTLEMATCH_IRC_RECV_FLAG_REGULATION] = TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ėp��M�o�b�t�@�[���擾
 *
 *	@param	int netID             �l�b�gID
 *	@param	p_wk_adrs             INIT�ŗ^�������[�N
 *	@param	size                  �f�[�^�T�C�Y
 *
 *	@return ��M�o�b�t�@
 */
//-----------------------------------------------------------------------------
static u8* LiveBattleMatch_RecvCallback_GetRecvBuffer( int netID, void* p_wk_adrs, int size )
{ 
  LIVEBATTLEMATCH_IRC_WORK *p_wk  = p_wk_adrs;

  if( netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle()) )
  {
    GF_ASSERT( 0 );
    return NULL;//�����͎̂󂯎��Ȃ�
  }
  else
  { 
    return p_wk->recv_buffer;
  }
}

