//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mystery_net.c
 *	@brief  �ӂ����Ȃ�������̒ʐM����
 *	@author	Toru=Nagihashi
 *	@data		2009.12.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/main.h"

//�l�b�g
#include "net/network_define.h"
#include "net/dwc_rap.h"
#include "net/wih_dwc.h"
#include "system/net_err.h"

//�Z�[�u�f�[�^
#include "savedata/wifilist.h"

//�O�����J
#include "mystery_net.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�V�[�P���X�Ǘ�
//=====================================
typedef struct _SEQ_WORK SEQ_WORK;	//�֐��^��邽�ߑO���錾
typedef void (*SEQ_FUNCTION)( SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );
struct _SEQ_WORK
{
	SEQ_FUNCTION	seq_function;		//���s���̃V�[�P���X�֐�
	BOOL is_end;									//�V�[�P���X�V�X�e���I���t���O
	int seq;											//���s���̃V�[�P���X�֐��̒��̃V�[�P���X
	void *p_wk_adrs;							//���s���̃V�[�P���X�֐��ɓn�����[�N
};

//-------------------------------------
///	�r�[�R�����
//=====================================
typedef struct 
{
	GameServiceID	gsID;
	STRCODE		playername[ PERSON_NAME_SIZE + EOM_SIZE ];
	u32				sex;
	u64				friendcode;
} BEACON_DATA;

//-------------------------------------
///	�ӂ����Ȃ�������̒ʐM���[�N
//=====================================
struct _MYSTERY_NET_WORK
{
  SEQ_WORK          seq;
  WIH_DWC_WORK      *p_wih;
  BEACON_DATA       my_beacon;
  BEACON_DATA       recv_beacon;
  GAME_COMM_STATUS_BIT  comm_status;
  HEAPID            heapID;
  BOOL              is_exit;
} ;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	SEQ
//=====================================
static void SEQ_Init( SEQ_WORK *p_wk, void *p_wk_adrs, SEQ_FUNCTION seq_function );
static void SEQ_Exit( SEQ_WORK *p_wk );
static void SEQ_Main( SEQ_WORK *p_wk );
static BOOL SEQ_IsEnd( const SEQ_WORK *cp_wk );
static void SEQ_SetNext( SEQ_WORK *p_wk, SEQ_FUNCTION seq_function );
static void SEQ_End( SEQ_WORK *p_wk );
static BOOL SEQ_IsComp( const SEQ_WORK *cp_wk, SEQ_FUNCTION seq_function );

//-------------------------------------
///	SEQFUNC
//=====================================
static void SEQFUNC_Wait( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_InitBeaconScan( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_ExitBeaconScan( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_MainBeaconScan( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_InitWirelessScan( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_ExitWirelessScan( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_MainWirelessScan( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_InitIrcScan( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_ExitIrcScan( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_MainIrcScan( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

static void SEQFUNC_LogoutWifi( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	�ʐM�R�[���o�b�N
//=====================================
static void * NETCALLBACK_GetBeaconData( void *p_wk_adrs );
static int NETCALLBACK_GetBeaconSize( void *p_wk_adrs );
static BOOL NETCALLBACK_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 );
static void	NETCALLBACK_ExitCallback(void* p_wk_adrs);

//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================
//-------------------------------------
///	�ʐM�������\����
//=====================================
static const GFLNetInitializeStruct sc_net_init =
{
  NULL, //NetSamplePacketTbl,  // ��M�֐��e�[�u��
  0, // ��M�e�[�u���v�f��
  NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
  NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
  NULL, // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
  NULL, // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
  NETCALLBACK_GetBeaconData,    // �r�[�R���f�[�^�擾�֐�
  NETCALLBACK_GetBeaconSize,    // �r�[�R���f�[�^�T�C�Y�擾�֐�
  NETCALLBACK_CheckConnectService,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
  NULL,    // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂��
  NULL,  //FatalError
  NULL, // �ʐM�ؒf���ɌĂ΂��֐�(�I����
  NULL, // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
  NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
  NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
  NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
  NULL,   ///< DWC�`���̗F�B���X�g
  NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
  0,   ///< DWC�ւ�HEAP�T�C�Y
  TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
  0x222,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
  GFL_HEAPID_APP,  //���ɂȂ�heapid
  HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
  HEAPID_WIFI,  //wifi�p��create�����HEAPID
  HEAPID_NETWORK, //
  GFL_WICON_POSX,GFL_WICON_POSY,  // �ʐM�A�C�R��XY�ʒu
  2,//_MAXNUM,  //�ő�ڑ��l��
  48,//_MAXSIZE,  //�ő呗�M�o�C�g��
  2,//_BCON_GET_NUM,  // �ő�r�[�R�����W��
  TRUE,   // CRC�v�Z
  FALSE,    // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
  GFL_NET_TYPE_WIRELESS,    //�ʐM�^�C�v�̎w��
  TRUE,   // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  WB_NET_MYSTERY, //GameServiceID
#if GFL_NET_IRC
  IRC_TIMEOUT_STANDARD, // �ԊO���^�C���A�E�g����
#endif
  0,//MP�e�ő�T�C�Y 512�܂�
  0,//dummy
};

//=============================================================================
/**
 *          �O�����J
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
MYSTERY_NET_WORK * MYSTERY_NET_Init( HEAPID heapID )
{ 
  MYSTERY_NET_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_NET_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_NET_WORK) );
  p_wk->heapID  = heapID;

  //���W���[���쐬
	SEQ_Init( &p_wk->seq, p_wk, SEQFUNC_Wait );

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �j��
 *
 *	@param	MYSTERY_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void MYSTERY_NET_Exit( MYSTERY_NET_WORK *p_wk )
{ 
  SEQ_Exit( &p_wk->seq );
  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C������
 *
 *	@param	MYSTERY_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void MYSTERY_NET_Main( MYSTERY_NET_WORK *p_wk )
{ 
  //�V�[�P���X
  SEQ_Main( &p_wk->seq );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��ԕψڃ��N�G�X�g
 *
 *	@param	MYSTERY_NET_WORK *p_wk  ���[�N
 *	@param	state                   ���N�G�X�g
 */
//-----------------------------------------------------------------------------
void MYSTERY_NET_ChangeStateReq( MYSTERY_NET_WORK *p_wk, MYSTERY_NET_STATE state )
{ 
  switch( state )
  { 
  case MYSTERY_NET_STATE_START_BEACON:
    if( SEQ_IsComp(&p_wk->seq, SEQFUNC_Wait ) )
    { 
      SEQ_SetNext( &p_wk->seq, SEQFUNC_InitBeaconScan );
    }
    break;

  case MYSTERY_NET_STATE_END_BEACON:
    if( SEQ_IsComp(&p_wk->seq, SEQFUNC_MainBeaconScan ) )
    { 
      SEQ_SetNext( &p_wk->seq, SEQFUNC_ExitBeaconScan );
    }
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��Ԃ��擾
 *
 *	@param	const MYSTERY_NET_WORK *cp_wk   ���[�N
 *
 *	@return�@���
 */
//-----------------------------------------------------------------------------
MYSTERY_NET_STATE MYSTERY_NET_GetState( const MYSTERY_NET_WORK *cp_wk )
{ 
  if( SEQ_IsComp( &cp_wk->seq, SEQFUNC_Wait ) )
  { 
    return MYSTERY_NET_STATE_WAIT;
  }
  else if( SEQ_IsComp( &cp_wk->seq, SEQFUNC_InitBeaconScan ) )
  { 
    return MYSTERY_NET_STATE_START_BEACON;
  }
  else if( SEQ_IsComp( &cp_wk->seq, SEQFUNC_MainBeaconScan ) )
  { 
    return MYSTERY_NET_STATE_MAIN_BEACON;
  }
  else if( SEQ_IsComp( &cp_wk->seq, SEQFUNC_ExitBeaconScan ) )
  { 
    return MYSTERY_NET_STATE_END_BEACON;
  }


  GF_ASSERT(0);
  return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����_�Ŏ��s���Ă���ʐM���[�h���擾
 *
 *	@param	const MYSTERY_NET_WORK *cp_wk   ���[�N
 *
 *	@return �ʐM�^�C�v
 */
//-----------------------------------------------------------------------------
GAME_COMM_STATUS_BIT MYSTERY_NET_GetCommStatus( const MYSTERY_NET_WORK *cp_wk )
{ 
  return cp_wk->comm_status;
}
//=============================================================================
/**
 *						SEQFUNC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �Ȃɂ����Ȃ�
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Wait( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 

}
//----------------------------------------------------------------------------
/**
 *	@brief  �r�[�R���T�[�`�J�n
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_InitBeaconScan( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_INIT_WAIT,
    SEQ_END,
  };
  MYSTERY_NET_WORK *p_wk  = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
		GFL_NET_Init( &sc_net_init, NULL, p_wk );
    *p_seq = SEQ_INIT_WAIT;
    break;

  case SEQ_INIT_WAIT:
    if( GFL_NET_IsInit() )
		{
      p_wk->p_wih = WIH_DWC_AllBeaconStart(sc_net_init.maxBeaconNum, p_wk->heapID );
      GFL_NET_Changeover(NULL);
      *p_seq = SEQ_END;
    }
    break;

  case SEQ_END:
    SEQ_SetNext( p_seqwk, SEQFUNC_MainBeaconScan );
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �r�[�R���T�[�`�I��
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_ExitBeaconScan( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_WIH_EXIT,
    SEQ_NET_EXIT,
    SEQ_EXIT_WAIT,
    SEQ_END,
  };
  MYSTERY_NET_WORK *p_wk  = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_WIH_EXIT:
    WIH_DWC_AllBeaconEnd(p_wk->p_wih);
    *p_seq  = SEQ_NET_EXIT;
    break;

  case SEQ_NET_EXIT:
		if( GFL_NET_Exit( NETCALLBACK_ExitCallback ) )
    { 
      *p_seq = SEQ_EXIT_WAIT;
    }
    else
    { 
      *p_seq = SEQ_END;
    }
    break;

  case SEQ_EXIT_WAIT:
    if( p_wk->is_exit )
		{
      p_wk->is_exit = FALSE;
      *p_seq = SEQ_END;
    }
    break;

  case SEQ_END:
    SEQ_SetNext( p_seqwk, SEQFUNC_Wait );
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �r�[�R���T�[�`�I��
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					  �V�[�P���X
 *	@param	*p_wk_adrs		  	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_MainBeaconScan( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  MYSTERY_NET_WORK *p_wk  = p_wk_adrs;

  WIH_DWC_MainLoopScanBeaconData();
  p_wk->comm_status = WIH_DWC_GetAllBeaconTypeBit();
}

//----------------------------------------------------------------------------
/**
 *	@brief  Wifi���烍�O�A�E�g
 *
 *	@param	SEQ_WORK *p_seqwk   �V�[�P���X���[�N
 *	@param	*p_seq              �V�[�P���X
 *	@param	*p_wk_adrs          ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_LogoutWifi( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_EXIT,
    SEQ_EXIT_WAIT,
    SEQ_END,
  };

  MYSTERY_NET_WORK *p_wk  = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_EXIT:
    if( GFL_NET_Exit( NETCALLBACK_ExitCallback ) )
    { 
      *p_seq  = SEQ_EXIT_WAIT;
    }
    else
    { 
      *p_seq  = SEQ_END;
    }
    break;

  case SEQ_EXIT_WAIT:
    if( p_wk->is_exit )
		{
      p_wk->is_exit = FALSE;
      *p_seq = SEQ_END;
    } 
    break;

  case SEQ_END:
    SEQ_SetNext( p_seqwk, SEQFUNC_Wait );
    break;
  }
}
//=============================================================================
/**
 *						SEQ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	������
 *
 *	@param	SEQ_WORK *p_wk	���[�N
 *	@param	*p_param				�p�����[�^
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Init( SEQ_WORK *p_wk, void *p_wk_adrs, SEQ_FUNCTION seq_function )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );

	//������
	p_wk->p_wk_adrs	= p_wk_adrs;

	//�Z�b�g
	SEQ_SetNext( p_wk, seq_function );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�j��
 *
 *	@param	SEQ_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void SEQ_Exit( SEQ_WORK *p_wk )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���C������
 *
 *	@param	SEQ_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Main( SEQ_WORK *p_wk )
{	
	if( !p_wk->is_end )
	{	
		p_wk->seq_function( p_wk, &p_wk->seq, p_wk->p_wk_adrs );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I���擾
 *
 *	@param	const SEQ_WORK *cp_wk		���[�N
 *
 *	@return	TRUE�Ȃ�ΏI��	FALSE�Ȃ�Ώ�����
 */	
//-----------------------------------------------------------------------------
static BOOL SEQ_IsEnd( const SEQ_WORK *cp_wk )
{	
	return cp_wk->is_end;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���̃V�[�P���X��ݒ�
 *
 *	@param	SEQ_WORK *p_wk	���[�N
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_SetNext( SEQ_WORK *p_wk, SEQ_FUNCTION seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�I��
 *
 *	@param	SEQ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_End( SEQ_WORK *p_wk )
{	
	p_wk->is_end	= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �V�[�P���X���`�F�b�N
 *
 *	@param	const SEQ_WORK *cp_wk ���[�N
 *	@param	seq_function          ���ׂ�V�[�P���X
 *
 *	@return TRUE�ň�v  FALSE�ŕs��v
 */
//-----------------------------------------------------------------------------
static BOOL SEQ_IsComp( const SEQ_WORK *cp_wk, SEQ_FUNCTION seq_function )
{ 
  return cp_wk->seq_function == seq_function; 
}
//=============================================================================
/**
 *					�ʐM�R�[���o�b�N
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�r�[�R���f�[�^�擾�֐�
 *
 *	@param	void *p_wk_adrs		���[�N
 *
 *	@return	�r�[�R���f�[�^
 */
//-----------------------------------------------------------------------------
static void * NETCALLBACK_GetBeaconData( void *p_wk_adrs )
{	
	MYSTERY_NET_WORK	*p_wk	= p_wk_adrs;

	return &p_wk->my_beacon;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�r�[�R���T�C�Y�擾�֐�
 *
 *	@param	void *p_wk_adrs		���[�N
 *
 *	@return	�r�[�R���T�C�Y
 */
//-----------------------------------------------------------------------------
static int NETCALLBACK_GetBeaconSize( void *p_wk_adrs )
{	
	return sizeof(BEACON_DATA);
}
//----------------------------------------------------------------------------
/**
 *	@brief	�r�[�R���̃T�[�r�X���r���Čq���ł悢���ǂ������f����
 *
 *	@param	GameServiceID1	�T�[�r�XID�P
 *	@param	GameServiceID2	�T�[�r�XID�Q
 *
 *	@return	TRUE�Ȃ�ΐڑ�OK	FALSE�Ȃ�ΐڑ�NG
 */
//-----------------------------------------------------------------------------
static BOOL NETCALLBACK_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 )
{	
	//�󂯎��r�[�R���𔻒f
	return GameServiceID1 == GameServiceID2;
}
//--------------------------------------------------------------
/**
 * �I�������R�[���o�b�N
 *
 * @param   pWork		GAME_BEACON_SYS_PTR
 */
//--------------------------------------------------------------
static void	NETCALLBACK_ExitCallback(void* p_wk_adrs)
{
	MYSTERY_NET_WORK	*p_wk	= p_wk_adrs;
  
  OS_TPrintf("GameBeacon:ExitCallback\n");
  p_wk->is_exit = TRUE;
}
