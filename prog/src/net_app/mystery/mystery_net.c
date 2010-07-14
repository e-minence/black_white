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
#include <dwc.h>

//�V�X�e��
#include "pm_version.h"
#include "system/main.h"

//�l�b�g
#include "net/network_define.h"
#include "net/dwc_rap.h"
#include "net/wih_dwc.h"
#include "system/net_err.h"
#include "net/delivery_beacon.h"
#include "net/delivery_irc.h"
#include "net/dwc_error.h"

//�Z�[�u�f�[�^
#include "savedata/wifilist.h"
#include "savedata/mystery_data.h"

//�O�����J
#include "mystery_net.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//�t�@�C������
#define WIFI_FILE_ATTR1			"MYSTERY_J" //�ӂ����Ȃ�������̎��ʎq�{����R�[�h
#define WIFI_FILE_ATTR2			""          //ROM�o�[�W�����̃r�b�g�i���ƂŔ��ʂ��邽�ߍ���NULL�j
#define WIFI_FILE_ATTR3			""          //���g�p

#define MYSTERY_DOWNLOAD_FILE_MAX  10

#define MYSTERY_DOWNLOAD_GIFT_DATA_SIZE (4096)

enum {
  ND_RESULT_EXECUTE,		// ���s��
  ND_RESULT_COMPLETE,		// ����_�E�����[�h�I��
  ND_RESULT_NOT_FOUND_FILES,	// �t�@�C����������Ȃ�����
  ND_RESULT_DOWNLOAD_CANCEL,	// ���[�U�[����L�����Z�����ꂽ
  ND_RESULT_DOWNLOAD_ERROR,	// �Ȃ�炩�̃G���[���N����
  ND_RESULT_MAX
};

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
///	�G���[���[�N
//=====================================
typedef struct 
{
  int dummy;
} MYSTERY_ERROR_WORK;


//-------------------------------------
///	WIFI�_�E�����[�h
//=====================================
typedef struct 
{
  DWCNdFileInfo     fileInfo[ MYSTERY_DOWNLOAD_FILE_MAX ];
  BOOL              recvflag[ MYSTERY_DOWNLOAD_FILE_MAX ];
  s32               next_seq;
  BOOL              cancel_req;
  BOOL              wifi_cancel;
  int               recv_filenum;
  u32               percent;
  u32               recived;
  u32               contentlen;
  u32               target;
  char              *p_buffer;
  u32               async_timeout;
#ifdef BUGFIX_BTS7821_20100714
  BOOL              is_nd_disconnect_enable;
#endif //BUGFIX_BTS7821_20100714
} WIFI_DOWNLOAD_DATA;

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
  WIFI_DOWNLOAD_DATA  wifi_download_data;
  const SAVE_CONTROL_WORK *cp_sv;
  DELIVERY_BEACON_WORK  *p_beacon;
  DELIVERY_IRC_WORK     *p_irc;

  MYSTERY_ERROR_WORK    error;

  char              buffer[MYSTERY_DOWNLOAD_GIFT_DATA_SIZE];
  MYSTERY_NET_RECV_STATUS recv_status;
} ;

//-------------------------------------
///	�R�[���o�b�N�t���O
//    �R�[���o�b�N�Ɉ������Ȃ��̂ł������Ȃ�static
//=====================================
static BOOL s_callback_flag;
static int  s_callback_result;
static BOOL s_cleanup_callback_flag;
static int  s_cleanup_callback_result;

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

static void SEQFUNC_InitBeaconDownload( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_ExitBeaconDownload( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_MainBeaconDownload( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

static void SEQFUNC_InitIrcDownload( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_ExitIrcDownload( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_MainIrcDownload( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

static void SEQFUNC_WifiDownload( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

static void SEQFUNC_LogoutWifi( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	WIFI_DOWNLOAD�Ŏg���֐�
//=====================================
static void WIFI_DOWNLOAD_WaitNdCallback( WIFI_DOWNLOAD_DATA *p_wk, int *p_seq, int next_seq );
static void WIFI_DOWNLOAD_WaitNdCleanCallback( WIFI_DOWNLOAD_DATA *p_wk, int result, int *p_seq, int next_seq, int false_seq);

//-------------------------------------
///	�ʐM�R�[���o�b�N
//=====================================
static void * NETCALLBACK_GetBeaconData( void *p_wk_adrs );
static int NETCALLBACK_GetBeaconSize( void *p_wk_adrs );
static BOOL NETCALLBACK_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 , void* pWork);
static void	NETCALLBACK_ExitCallback(void* p_wk_adrs);
// wifi download�Ŏg�p����R�[���o�b�N
static void NdCallback(DWCNdCallbackReason reason, DWCNdError error, int servererror);
static void NdCleanupCallback( void );

//-------------------------------------
///	�G���[
//=====================================
static MYSTERY_NET_ERROR_REPAIR_TYPE Mystery_Net_Wifi_GetErrorRepairType( const MYSTERY_NET_WORK *cp_wk );
static MYSTERY_NET_ERROR_REPAIR_TYPE Mystery_Net_Wireless_GetErrorRepairType( const MYSTERY_NET_WORK *cp_wk );
static MYSTERY_NET_ERROR_REPAIR_TYPE Mystery_Net_Irc_GetErrorRepairType( const MYSTERY_NET_WORK *cp_wk );

//-------------------------------------
///	���̑�
//=====================================
static u32  UTIL_StringToHex( const char *buf );
static BOOL UTIL_IsClear( void *p_adrs, u32 size );

#ifdef BUGFIX_BTS7821_20100714
static BOOL Mystery_Net_ErrDisconnectCallbackEx(void* p_wk_adrs, int code, int type, int ret );
#endif //BUGFIX_BTS7821_20100714

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
  SYACHI_NETWORK_GGID,  //ggid  
  GFL_HEAPID_APP,  //���ɂȂ�heapid
  HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
  HEAPID_WIFI,  //wifi�p��create�����HEAPID
  HEAPID_NETWORK, //
  GFL_WICON_POSX,GFL_WICON_POSY,  // �ʐM�A�C�R��XY�ʒu
  1,//_MAXNUM,  //�ő�ڑ��l��
  88,//_MAXSIZE,  //�ő呗�M�o�C�g��
  4,//_BCON_GET_NUM,  // �ő�r�[�R�����W��
  TRUE,   // CRC�v�Z
  FALSE,    // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
  GFL_NET_TYPE_WIRELESS,    //�ʐM�^�C�v�̎w��
  FALSE,   // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  WB_NET_MYSTERY, //GameServiceID
#if GFL_NET_IRC
  IRC_TIMEOUT_STANDARD, // �ԊO���^�C���A�E�g����
#endif
  500,//MP�e�ő�T�C�Y 512�܂�
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
 *  @param  cp_sv           ���[��
 *	@param	HEAPID heapID   �q�[�vID
 *
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
MYSTERY_NET_WORK * MYSTERY_NET_Init( const SAVE_CONTROL_WORK *cp_sv, HEAPID heapID )
{ 
  MYSTERY_NET_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(MYSTERY_NET_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_NET_WORK) );
  p_wk->heapID  = heapID;
  p_wk->cp_sv   = cp_sv;

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

  case MYSTERY_NET_STATE_WIFI_DOWNLOAD:
    if( SEQ_IsComp(&p_wk->seq, SEQFUNC_Wait ) )
    { 
      SEQ_SetNext( &p_wk->seq, SEQFUNC_WifiDownload );
    }
    break;

  case MYSTERY_NET_STATE_CANCEL_WIFI_DOWNLOAD:
    if( SEQ_IsComp(&p_wk->seq, SEQFUNC_WifiDownload ) )
    {
      p_wk->wifi_download_data.cancel_req = TRUE;
    }
    break;

  case MYSTERY_NET_STATE_LOGOUT_WIFI :
    SEQ_SetNext( &p_wk->seq, SEQFUNC_LogoutWifi );
    break;

  case MYSTERY_NET_STATE_START_BEACON_DOWNLOAD:
    SEQ_SetNext( &p_wk->seq, SEQFUNC_InitBeaconDownload );
    break;

  case MYSTERY_NET_STATE_END_BEACON_DOWNLOAD:
    SEQ_SetNext( &p_wk->seq, SEQFUNC_ExitBeaconDownload );
    break;
  case MYSTERY_NET_STATE_START_IRC_DOWNLOAD:
    SEQ_SetNext( &p_wk->seq, SEQFUNC_InitIrcDownload );
    break;

  case MYSTERY_NET_STATE_END_IRC_DOWNLOAD:
    SEQ_SetNext( &p_wk->seq, SEQFUNC_ExitIrcDownload );
    break;
    
  case MYSTERY_NET_STATE_WAIT:
    SEQ_SetNext( &p_wk->seq, SEQFUNC_Wait );
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
  else if( SEQ_IsComp( &cp_wk->seq, SEQFUNC_WifiDownload ) )
  { 
    return MYSTERY_NET_STATE_WIFI_DOWNLOAD;
  }
  else if( SEQ_IsComp( &cp_wk->seq, SEQFUNC_LogoutWifi ) )
  { 
    return MYSTERY_NET_STATE_LOGOUT_WIFI;
  }
  else if( SEQ_IsComp( &cp_wk->seq, SEQFUNC_InitBeaconDownload ) )
  { 
    return MYSTERY_NET_STATE_START_BEACON_DOWNLOAD;
  }
  else if( SEQ_IsComp( &cp_wk->seq, SEQFUNC_MainBeaconDownload ) )
  { 
    return MYSTERY_NET_STATE_MAIN_BEACON_DOWNLOAD;
  }
  else if( SEQ_IsComp( &cp_wk->seq, SEQFUNC_ExitBeaconDownload ) )
  { 
    return MYSTERY_NET_STATE_END_BEACON_DOWNLOAD;
  }
  else if( SEQ_IsComp( &cp_wk->seq, SEQFUNC_InitIrcDownload ) )
  { 
    return MYSTERY_NET_STATE_START_IRC_DOWNLOAD;
  }
  else if( SEQ_IsComp( &cp_wk->seq, SEQFUNC_MainIrcDownload ) )
  { 
    return MYSTERY_NET_STATE_MAIN_IRC_DOWNLOAD;
  }
  else if( SEQ_IsComp( &cp_wk->seq, SEQFUNC_ExitIrcDownload ) )
  { 
    return MYSTERY_NET_STATE_END_IRC_DOWNLOAD;
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
//----------------------------------------------------------------------------
/**
 *	@brief  �_�E�����[�h�����f�[�^���擾
 *
 *	@param	const MYSTERY_NET_WORK *cp_wk ���[�N
 *	@param	*p_data     �f�[�^
 *	@param	size        �T�C�Y
 *
 *	@return MYSTERY_NET_RECV_STATUS��
 */
//-----------------------------------------------------------------------------
MYSTERY_NET_RECV_STATUS MYSTERY_NET_GetDownloadData( const MYSTERY_NET_WORK *cp_wk, void *p_data, u32 size )
{ 
  if( cp_wk->recv_status == MYSTERY_NET_RECV_STATUS_SUCCESS )
  { 
    GFL_STD_MemCopy( cp_wk->buffer, p_data, size );
    return MYSTERY_NET_RECV_STATUS_SUCCESS;
  }
  return cp_wk->recv_status;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �G���[�C���߂����擾
 *
 *	@param	MYSTERY_NET_WORK *p_wk  ���[�N
 *
 *	@return MYSTERY_NET_ERROR_REPAIR_TYPE�ɏ�����
 */
//-----------------------------------------------------------------------------
MYSTERY_NET_ERROR_REPAIR_TYPE MYSTERY_NET_GetErrorRepairType( const MYSTERY_NET_WORK *cp_wk )
{ 
  MYSTERY_NET_ERROR_REPAIR_TYPE repair  = MYSTERY_NET_ERROR_REPAIR_NONE;

  if( GFL_NET_IsInit() )
  { 
    switch( GFL_NET_GetNETInitStruct()->bNetType )
    { 
    case GFL_NET_TYPE_WIRELESS:
    case GFL_NET_TYPE_WIRELESS_SCANONLY:
      repair  = Mystery_Net_Wireless_GetErrorRepairType( cp_wk );
      break;

    case GFL_NET_TYPE_WIFI:
      repair  = Mystery_Net_Wifi_GetErrorRepairType( cp_wk );
      break;

    case GFL_NET_TYPE_IRC:
      repair  = Mystery_Net_Irc_GetErrorRepairType( cp_wk );
      break;

    default:
      GF_ASSERT(0);
    }
  }

  return repair;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G���[���C��
 *
 *	@param	MYSTERY_NET_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void MYSTERY_NET_ClearError( MYSTERY_NET_WORK *p_wk )
{ 
  if( p_wk->p_wih )
  { 
    WIH_DWC_AllBeaconEnd(p_wk->p_wih);
    p_wk->p_wih = NULL;
  }

  MYSTERY_NET_ChangeStateReq( p_wk, MYSTERY_NET_STATE_WAIT );
  NetErr_ErrWorkInit();
}

//----------------------------------------------------------------------------
/**
 *	@brief  WIFI�̃G���[�C���߂����擾
 *
 *	@param	MYSTERY_NET_WORK *p_wk  ���[�N
 *
 *	@return MYSTERY_NET_ERROR_REPAIR_TYPE�ɏ�����
 */
//-----------------------------------------------------------------------------
static MYSTERY_NET_ERROR_REPAIR_TYPE Mystery_Net_Wifi_GetErrorRepairType( const MYSTERY_NET_WORK *cp_wk )
{ 
  MYSTERY_NET_ERROR_REPAIR_TYPE repair  = MYSTERY_NET_ERROR_REPAIR_NONE;

  if( NetErr_App_CheckError() )
  { 
    const GFL_NETSTATE_DWCERROR* cp_error  =  GFL_NET_StateGetWifiError();

    switch( cp_error->errorType )
    { 
    case DWC_ETYPE_LIGHT:
    case DWC_ETYPE_SHOW_ERROR:
    case DWC_ETYPE_SHUTDOWN_FM:
    case DWC_ETYPE_SHUTDOWN_GHTTP:
    case DWC_ETYPE_SHUTDOWN_ND:
    case DWC_ETYPE_DISCONNECT:
      GFL_NET_StateClearWifiError();
      DWC_ClearError();
      NetErr_ExitNetSystem();
      NetErr_DispCallPushPop();
      NetErr_ErrWorkInit();
      repair  = MYSTERY_NET_ERROR_REPAIR_RETURN;
      break;

    case DWC_ETYPE_FATAL:
      GFL_NET_StateClearWifiError();
      NetErr_DispCallFatal();
      repair  = MYSTERY_NET_ERROR_REPAIR_FATAL;
      break;
    }
  }

  return repair;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WIRELESS�̃G���[�C���߂����擾
 *
 *	@param	MYSTERY_NET_WORK *p_wk  ���[�N
 *
 *	@return MYSTERY_NET_ERROR_REPAIR_TYPE�ɏ�����
 */
//-----------------------------------------------------------------------------
static MYSTERY_NET_ERROR_REPAIR_TYPE Mystery_Net_Wireless_GetErrorRepairType( const MYSTERY_NET_WORK *cp_wk )
{ 
  MYSTERY_NET_ERROR_REPAIR_TYPE repair  = MYSTERY_NET_ERROR_REPAIR_NONE;

  if( NetErr_App_CheckError() )
  { 
    NetErr_ExitNetSystem();
    NetErr_DispCallPushPop();
    NetErr_ErrWorkInit();

    repair  = MYSTERY_NET_ERROR_REPAIR_RETURN;
  }

  return repair;
}
//----------------------------------------------------------------------------
/**
 *	@brief  IRC�̃G���[�C���߂����擾
 *
 *	@param	MYSTERY_NET_WORK *p_wk  ���[�N
 *
 *	@return MYSTERY_NET_ERROR_REPAIR_TYPE�ɏ�����
 */
//-----------------------------------------------------------------------------
static MYSTERY_NET_ERROR_REPAIR_TYPE Mystery_Net_Irc_GetErrorRepairType( const MYSTERY_NET_WORK *cp_wk )
{ 
  MYSTERY_NET_ERROR_REPAIR_TYPE repair  = MYSTERY_NET_ERROR_REPAIR_NONE;

  if( NetErr_App_CheckError() )
  { 
    NetErr_ExitNetSystem();
    NetErr_DispCallPushPop();
    NetErr_ErrWorkInit();

    repair  = MYSTERY_NET_ERROR_REPAIR_RETURN;
  }
  return repair;
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
    
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );

    *p_seq = SEQ_INIT_WAIT;
    break;

  case SEQ_INIT_WAIT:
    if( GFL_NET_IsInit() )
		{
      GFL_NET_ReloadIconTopOrBottom( TRUE, p_wk->heapID );
      p_wk->p_wih = WIH_DWC_AllBeaconStart(sc_net_init.maxBeaconNum, p_wk->heapID );
      GFL_NET_StartBeaconScan();
      *p_seq = SEQ_END;
    }
    break;

  case SEQ_END:
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
    OS_TFPrintf( 3, "�r�[�R��������\n" );
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
    p_wk->p_wih = NULL;
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
    OS_TFPrintf( 3, "�r�[�R���J��\n" );
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
  p_wk->comm_status = WIH_DWC_GetAllBeaconTypeBit(NULL);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �r�[�R���_�E�����[�h�J�n
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_InitBeaconDownload( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
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
    { 
      DELIVERY_BEACON_INIT  belivery_beacon_init;
      GFL_STD_MemClear( &belivery_beacon_init, sizeof(DELIVERY_BEACON_INIT) );
      belivery_beacon_init.NetDevID     = WB_NET_MYSTERY;
      belivery_beacon_init.data[0].datasize = sizeof( DOWNLOAD_GIFT_DATA );  //�f�[�^�S�̃T�C�Y
      belivery_beacon_init.data[0].pData = (void*)p_wk->buffer;     // ��M�o�b�t�@�f�[�^
      belivery_beacon_init.data[0].LangCode  = CasetteLanguage;     // �󂯎�錾��R�[�h
      belivery_beacon_init.data[0].version   = 1<<GET_VERSION();     // �󂯎��o�[�W�����̃r�b�g
      belivery_beacon_init.dataNum = 1;  //�󂯎�葤�͂P
      belivery_beacon_init.ConfusionID  = 0;
      belivery_beacon_init.heapID       = p_wk->heapID;
      p_wk->p_beacon  = DELIVERY_BEACON_Init( &belivery_beacon_init );
      DELIVERY_BEACON_RecvStart( p_wk->p_beacon );    
      
      GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
    }
    p_wk->recv_status = MYSTERY_NET_RECV_STATUS_NONE;
    *p_seq = SEQ_INIT_WAIT;
    break;

  case SEQ_INIT_WAIT:
    if( GFL_NET_IsInit() )
		{
      //�ʐM�A�C�R��
      GFL_NET_ReloadIconTopOrBottom( TRUE, p_wk->heapID );

      GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
      *p_seq = SEQ_END;
    }
    break;

  case SEQ_END:
    OS_TFPrintf( 3, "�r�[�R��������\n" );
    SEQ_SetNext( p_seqwk, SEQFUNC_MainBeaconDownload );
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
static void SEQFUNC_ExitBeaconDownload( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_WIH_EXIT,
    SEQ_EXIT_WAIT,
    SEQ_END,
  };
  MYSTERY_NET_WORK *p_wk  = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_WIH_EXIT:
    DELIVERY_BEACON_End( p_wk->p_beacon );
    *p_seq  = SEQ_EXIT_WAIT;
    break;

  case SEQ_EXIT_WAIT:
    if( GFL_NET_IsResetEnable() )
		{
      *p_seq = SEQ_END;
    }
    break;

  case SEQ_END:
    OS_TFPrintf( 3, "�r�[�R���J��\n" );
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
static void SEQFUNC_MainBeaconDownload( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  MYSTERY_NET_WORK *p_wk  = p_wk_adrs;

  DELIVERY_BEACON_Main( p_wk->p_beacon );

  if( DELIVERY_BEACON_RecvCheck(p_wk->p_beacon) )
  { 
    OS_TFPrintf( 3, "�r�[�R���󂯎��\n" );
    p_wk->recv_status = MYSTERY_NET_RECV_STATUS_SUCCESS;
    SEQ_SetNext( p_seqwk, SEQFUNC_ExitBeaconDownload );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ԊO���Ń_�E�����[�h�J�n
 *
 *	@param	SEQ_WORK *p_seqwk   �V�[�P���X���[�N
 *	@param	*p_seq              �V�[�P���X
 *	@param	*p_wk_adrs          ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_InitIrcDownload( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
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
    { 
      DELIVERY_IRC_INIT  belivery_irc_init;
      GFL_STD_MemClear( &belivery_irc_init, sizeof(DELIVERY_IRC_INIT) );
      belivery_irc_init.NetDevID     = WB_NET_MYSTERY;
      belivery_irc_init.data[0].datasize     = sizeof( DOWNLOAD_GIFT_DATA );
      belivery_irc_init.data[0].pData        = (void*)p_wk->buffer;
      belivery_irc_init.ConfusionID  = 0;
      belivery_irc_init.heapID       = p_wk->heapID;
      belivery_irc_init.data[0].LangCode     = CasetteLanguage;
      belivery_irc_init.dataNum = 1;
      belivery_irc_init.data[0].version = 1<<GET_VERSION();     // �󂯎��o�[�W�����̃r�b�g
      p_wk->p_irc  = DELIVERY_IRC_Init( &belivery_irc_init );
      DELIVERY_IRC_RecvStart( p_wk->p_irc );
    }
    p_wk->recv_status = MYSTERY_NET_RECV_STATUS_NONE;
    *p_seq = SEQ_INIT_WAIT;


    break;

  case SEQ_INIT_WAIT:
    if( GFL_NET_IsInit() )
		{
      //�ʐM�A�C�R��
      GFL_NET_ReloadIconTopOrBottom( TRUE, p_wk->heapID );
      *p_seq = SEQ_END;
    }
    break;

  case SEQ_END:
    OS_TFPrintf( 3, "�ԊO��������\n" );
    SEQ_SetNext( p_seqwk, SEQFUNC_MainIrcDownload );
    break;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  �ԊO���Ń_�E�����[�h�I��
 *
 *	@param	SEQ_WORK *p_seqwk   �V�[�P���X���[�N
 *	@param	*p_seq              �V�[�P���X
 *	@param	*p_wk_adrs          ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_ExitIrcDownload( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_WIH_EXIT,
    SEQ_EXIT_WAIT,
    SEQ_END,
  };
  MYSTERY_NET_WORK *p_wk  = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_WIH_EXIT:
    DELIVERY_IRC_End( p_wk->p_irc );
    *p_seq  = SEQ_EXIT_WAIT;
    break;

  case SEQ_EXIT_WAIT:
    if( GFL_NET_IsResetEnable() )
		{
      *p_seq = SEQ_END;
    }
    break;

  case SEQ_END:
    OS_TFPrintf( 3, "�ԊO���J��\n" );
    SEQ_SetNext( p_seqwk, SEQFUNC_Wait );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ԊO���Ń_�E�����[�h��
 *
 *	@param	SEQ_WORK *p_seqwk   �V�[�P���X���[�N
 *	@param	*p_seq              �V�[�P���X
 *	@param	*p_wk_adrs          ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_MainIrcDownload( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  MYSTERY_NET_WORK *p_wk  = p_wk_adrs;
  int ret;

  DELIVERY_IRC_Main( p_wk->p_irc );

  ret = DELIVERY_IRC_RecvCheck( p_wk->p_irc );
  if( DELIVERY_IRC_FUNC != ret  )
  { 
    OS_TFPrintf( 3, "�ԊO���󂯎�� %d\n",ret );

    switch( ret )
    { 
    case DELIVERY_IRC_SUCCESS:
      p_wk->recv_status = MYSTERY_NET_RECV_STATUS_SUCCESS;
      break;
    case DELIVERY_IRC_FAILED:
    case DELIVERY_IRC_NOTDATA:
      p_wk->recv_status = MYSTERY_NET_RECV_STATUS_FAILED;
      break;
    }

    SEQ_SetNext( p_seqwk, SEQFUNC_ExitIrcDownload );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  Wifi����_�E�����[�h
 *	@note{  
 *	WIFI���玝���Ă���ꍇ�̎d�l
 *  ������
 *	�E�ŐV�̃f�[�^�̂݃T�[�o�[��ɂ����A�ߋ��̃f�[�^�͏�������
 *	�E�����̃f�[�^���������ꍇ�A�v���C���[���Ƃ̌Œ�l����_�E�����[�h����f�[�^��������
 *	�EROM�o�[�W�����⌾��R�[�h���قȂ���̂ł�����T�[�o�[��ɂ���
 *	�E�ӂ����Ȃ�������̈ȊO�ɂ����M�����[�V�����f�[�^�Ȃǂ�����T�[�o�[��ɂ���
 *  �EMYSTERY_J�̑����P���������̂͂P�O�܂ŃT�[�o�[�ɂ������Ƃ��ł���B
 *
 *  �������̎g����
 *	�����P�j	�ӂ����Ȃ�������̎��ʎq�{����R�[�h
 *			��	�c	MYSTERY_J
 *
 *	�����Q�j	ROM�o�[�W�������r�b�g�ɂ������l��16�i���ɂ�������
 *			��	�c	FFFFFFFF
 *
 *	�����R�j	���g�p
 *
 *	���T�[�o�[�A�N�Z�X�p�x��ቺ�����邽�߂̏���
 *	�EDWC_NdGetFileListNumAsync�͎g�p�֎~
 *
 *	}
 *
 *	@param	SEQ_WORK *p_seqwk   �V�[�P���X���[�N
 *	@param	*p_seq              �V�[�P���X
 *	@param	*p_wk_adrs          ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_WifiDownload( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_ATTR,
    SEQ_CHECK_ROM,
    SEQ_FILELIST,
    SEQ_CHECKLIST,
    SEQ_GET_FILE,
    SEQ_GETTING_FILE,

    SEQ_CANCEL,
    SEQ_DOWNLOAD_COMPLETE,
    SEQ_END,

    SEQ_WAIT_CALLBACK         = 100,
    SEQ_WAIT_CLEANUP_CALLBACK = 200,
  };

  MYSTERY_NET_WORK    *p_wk       = p_wk_adrs;
  WIFI_DOWNLOAD_DATA  *p_nd_data  = &p_wk->wifi_download_data;

  switch( *p_seq )
  { 
  case SEQ_INIT:
#ifdef BUGFIX_BTS7821_20100714
      GFL_NET_DWC_SetErrDisconnectCallbackEx(Mystery_Net_ErrDisconnectCallbackEx, &p_wk->wifi_download_data );
#endif //BUGFIX_BTS7821_20100714

    GFL_STD_MemClear( p_nd_data, sizeof(WIFI_DOWNLOAD_DATA) );
    p_nd_data->cancel_req   = FALSE;
    p_nd_data->wifi_cancel  = FALSE;
    p_nd_data->target = 0;
    p_nd_data->p_buffer     = p_wk->buffer;
    p_wk->recv_status = MYSTERY_NET_RECV_STATUS_NONE;
    if( DWC_NdInitAsync( NdCallback, GF_DWC_ND_LOGIN, WIFI_ND_LOGIN_PASSWD ) == FALSE )
    {
      OS_TPrintf( "DWC_NdInitAsync: Failed\n" );
      p_wk->recv_status  = MYSTERY_NET_RECV_STATUS_FAILED;
      *p_seq  = SEQ_END;
    }
    else
    { 
#ifdef BUGFIX_BTS7821_20100714
      p_nd_data->is_nd_disconnect_enable = TRUE;
#endif //BUGFIX_BTS7821_20100714
      WIFI_DOWNLOAD_WaitNdCallback( p_nd_data, p_seq, SEQ_ATTR );
    }
    break;

  case SEQ_ATTR:
    //�L�����Z��
    if( p_nd_data->wifi_cancel )
    { 
      p_wk->recv_status  = MYSTERY_NET_RECV_STATUS_FAILED;
      WIFI_DOWNLOAD_WaitNdCleanCallback( p_nd_data,ND_RESULT_DOWNLOAD_CANCEL, p_seq, SEQ_CANCEL, SEQ_CANCEL );
    }
    else
    { 
      // �t�@�C�������̐ݒ�
      if( DWC_NdSetAttr(WIFI_FILE_ATTR1, WIFI_FILE_ATTR2, WIFI_FILE_ATTR3) == FALSE )
      {
        OS_TPrintf( "DWC_NdSetAttr: Failed\n." );
        p_wk->recv_status  = MYSTERY_NET_RECV_STATUS_FAILED;
        WIFI_DOWNLOAD_WaitNdCleanCallback( p_nd_data,ND_RESULT_DOWNLOAD_ERROR, p_seq, SEQ_END, SEQ_END );
      }
      else
      { 
        *p_seq = SEQ_FILELIST;
      }
    }
    break;

//-------------------------------------
///	�T�[�o�[�̃t�@�C���`�F�b�N
//=====================================
  case SEQ_FILELIST:
    { 
      //��C�ɂP�O���ɍs��
      if( DWC_NdGetFileListAsync( p_nd_data->fileInfo, 0, MYSTERY_DOWNLOAD_FILE_MAX ) == FALSE)
      {
        OS_TPrintf( "DWC_NdGetFileListNumAsync: Failed.\n" );
        p_wk->recv_status  = MYSTERY_NET_RECV_STATUS_FAILED;
        WIFI_DOWNLOAD_WaitNdCleanCallback( p_nd_data,ND_RESULT_DOWNLOAD_ERROR, p_seq, SEQ_END, SEQ_END );
      }
      else
      { 
          WIFI_DOWNLOAD_WaitNdCallback( p_nd_data, p_seq, SEQ_CHECK_ROM );
      }
    }
    break;

  case SEQ_CHECK_ROM:
    { 
      int i;
      u32 rom_version;
      //�����o�[�W�����̃`�F�b�N���A��M�\�t���O������
      for( i = 0; i < MYSTERY_DOWNLOAD_FILE_MAX; i++ )
      { 
        //��M�ł��Ȃ��������̂͂O�N���A���ꂽ�܂�
        if( !UTIL_IsClear( &p_nd_data->fileInfo[i], sizeof(DWCNdFileInfo) ) )
        {
          rom_version  = UTIL_StringToHex( p_nd_data->fileInfo[ i ].param2 );
          OS_TPrintf( "[%d}=rom_version%x\n", i, rom_version );

          if( rom_version & 1<<GET_VERSION() )
          { 
            p_nd_data->recvflag[i]  = TRUE;
            OS_TPrintf( "��M�ł���o�[�W����%x %x\n",rom_version, 1<<GET_VERSION() );
          }
          else
          { 
            p_nd_data->recvflag[i]  = FALSE;
          OS_TPrintf( "��M�ł��Ȃ��o�[�W����%x %x\n",rom_version, 1<<GET_VERSION() );
          }
        }
        else
        {
          p_nd_data->recvflag[i]  = FALSE;
          OS_TPrintf( "��Ȃ̂Ŏ�M�ł��Ȃ�\n" );
        }
      }

      //ROM�o�[�W�����������Ă���{���Ɏ�M�\�Ȃ��̂̐����擾
      p_nd_data->recv_filenum = 0;
      for( i = 0; i < MYSTERY_DOWNLOAD_FILE_MAX; i++ )
      { 
        if( p_nd_data->recvflag[i] )
        {
          p_nd_data->recv_filenum++;
        }
      }

      *p_seq  = SEQ_CHECKLIST;
    }
    break;

  case SEQ_CHECKLIST:
    if( p_nd_data->recv_filenum == 1 )
    { 
      //�������擾�ł���t�@�C�����P�Ȃ�΁A����𗎂Ƃ�
      int i;
      for( i = 0; i < MYSTERY_DOWNLOAD_FILE_MAX; i++ )
      { 
        if( p_nd_data->recvflag[i] )
        {
          p_nd_data->target = i;
          break;
        }
      }
    }
    else if( p_nd_data->recv_filenum > 1 )
    {
      //�������擾�ł���t�@�C������������Ƃ��́A
      //������ID����擾���ׂ��t�@�C�������߂āA����𗎂Ƃ�
      //->���S�����ɂȂ�܂���
      int i;
      s32 index;
      int cnt   = 0;
      GF_ASSERT( p_nd_data->recv_filenum );

      index     = GFL_STD_MtRand( p_nd_data->recv_filenum );

      //�Q�ȏ�d�����Ă���̂ŁA
      //���̏d�����Ă��钆����A�v���C���[ID%N�̂��̂����o��
      for( i = 0; i < MYSTERY_DOWNLOAD_FILE_MAX; i++ )
      { 
        if( p_nd_data->recvflag[i] )
        { 
          if( cnt == index )
          { 
            p_nd_data->target = i;
            break;
          }

          cnt++;
        }
      }

      p_nd_data->target = i;
      OS_TPrintf( "�d�����Ă���̂ŁA�v���C���[���Ƃ̌Œ�l�łƂ�� ID%d index%d cnt%d\n", index, p_nd_data->target, cnt );
    }
    else
    { 
      //�t�@�C�����Ȃ��Ƃ��͏I��
      p_wk->recv_status  = MYSTERY_NET_RECV_STATUS_FAILED;
      WIFI_DOWNLOAD_WaitNdCleanCallback( p_nd_data, ND_RESULT_COMPLETE, p_seq, SEQ_DOWNLOAD_COMPLETE, SEQ_DOWNLOAD_COMPLETE ); 
      break;
    }

    *p_seq  = SEQ_GET_FILE;
    break;
//-------------------------------------
///	�t�@�C���ǂݍ��݊J�n
//=====================================
  case SEQ_GET_FILE:
    // �t�@�C���ǂݍ��݊J�n
    OS_TPrintf( "�擾������� target%d max%d\n", p_nd_data->target, p_nd_data->recv_filenum );
    if(DWC_NdGetFileAsync( &p_nd_data->fileInfo[ p_nd_data->target ], p_nd_data->p_buffer, MYSTERY_DOWNLOAD_GIFT_DATA_SIZE) == FALSE){
      OS_TPrintf( "DWC_NdGetFileAsync: Failed.\n" );
      p_wk->recv_status  = MYSTERY_NET_RECV_STATUS_FAILED;
      WIFI_DOWNLOAD_WaitNdCleanCallback( p_nd_data,ND_RESULT_DOWNLOAD_ERROR, p_seq, SEQ_END, SEQ_END ); 
    }
    else
    { 
      p_nd_data->percent = 0;
      s_callback_flag   = FALSE;
      s_callback_result = DWC_ND_ERROR_NONE;
      p_nd_data->async_timeout  = 0;
      *p_seq = SEQ_GETTING_FILE;
    }
    break;
    
  case SEQ_GETTING_FILE:
    // �t�@�C���ǂݍ��ݒ�
    DWC_NdProcess();
    if( p_nd_data->async_timeout++ > WIFI_ND_TIMEOUT_SYNC )
    {
      GFL_NET_StateSetWifiError( 0, 0, 0, ERRORCODE_USER_TIMEOUT );
      p_wk->recv_status  = MYSTERY_NET_RECV_STATUS_FAILED;
      WIFI_DOWNLOAD_WaitNdCleanCallback( p_nd_data,ND_RESULT_DOWNLOAD_CANCEL, p_seq, SEQ_CANCEL, SEQ_CANCEL );
    }

    if( s_callback_flag == FALSE )
    {
      // �t�@�C���ǂݍ��ݒ�
      if( p_nd_data->cancel_req )
      {
        // �_�E�����[�h�L�����Z��
        p_wk->recv_status  = MYSTERY_NET_RECV_STATUS_FAILED;
        WIFI_DOWNLOAD_WaitNdCleanCallback( p_nd_data,ND_RESULT_DOWNLOAD_CANCEL, p_seq, SEQ_CANCEL, SEQ_CANCEL ); 
      }
      else
      {
        // �i�s�x��\��
        if(DWC_NdGetProgress( &p_nd_data->recived, &p_nd_data->contentlen ) == TRUE)
        {
          if(p_nd_data->percent != (p_nd_data->recived*100)/p_nd_data->contentlen)
          {
            p_nd_data->percent = (p_nd_data->recived*100)/p_nd_data->contentlen;
            OS_Printf( "Download %d/100\n", p_nd_data->percent );
          }
        }
      }
    }
    else if( s_callback_result != DWC_ND_ERROR_NONE)
    {
      p_wk->recv_status  = MYSTERY_NET_RECV_STATUS_FAILED;
      WIFI_DOWNLOAD_WaitNdCleanCallback( p_nd_data, ND_RESULT_DOWNLOAD_ERROR, p_seq, SEQ_END, SEQ_END ); 
    }
    else
    { //callback1_result
      if(p_nd_data->wifi_cancel == FALSE)
      {
        // �t�@�C���ǂݍ��ݏI��
        p_wk->recv_status  = MYSTERY_NET_RECV_STATUS_SUCCESS;
        WIFI_DOWNLOAD_WaitNdCleanCallback( p_nd_data, ND_RESULT_COMPLETE, p_seq, SEQ_DOWNLOAD_COMPLETE, SEQ_DOWNLOAD_COMPLETE ); 
      } else {
        // �_�E�����[�h�L�����Z��
        p_wk->recv_status  = MYSTERY_NET_RECV_STATUS_FAILED;
        WIFI_DOWNLOAD_WaitNdCleanCallback( p_nd_data, ND_RESULT_DOWNLOAD_CANCEL, p_seq, SEQ_DOWNLOAD_COMPLETE, SEQ_DOWNLOAD_COMPLETE ); 
      }
    }
    break;

//-------------------------------------
///	�I���E�L�����Z������
//=====================================
  case SEQ_CANCEL:
    if( DWC_NdCancelAsync() == FALSE )
    {
      *p_seq = SEQ_DOWNLOAD_COMPLETE;
    }
    else 
    {
      *p_seq = SEQ_DOWNLOAD_COMPLETE;
    }
    break;

  case SEQ_DOWNLOAD_COMPLETE:
    *p_seq  = SEQ_END;
    break;
    
  case SEQ_END:
#ifdef BUGFIX_BTS7821_20100714
      p_nd_data->is_nd_disconnect_enable = FALSE;
#endif //BUGFIX_BTS7821_20100714

#ifdef BUGFIX_BTS7821_20100714
    GFL_NET_DWC_SetErrDisconnectCallbackEx( NULL, NULL );
#endif //BUGFIX_BTS7821_20100714

    SEQ_SetNext( p_seqwk, SEQFUNC_LogoutWifi );
    break;

//-------------------------------------
///	�G���[����
//=====================================

//-------------------------------------
///	�R�[���o�b�N�҂�����  
//�@�@WIFI_DOWNLOAD_WaitNdCallback��WIFI_DOWNLOAD_WaitNdCleanCallback�����g���Ă�������
//=====================================
  case SEQ_WAIT_CALLBACK:
    //�R�[���o�b�N������҂�
    DWC_NdProcess();

    if( p_nd_data->async_timeout++ > WIFI_ND_TIMEOUT_SYNC )
    {
      GFL_NET_StateSetWifiError( 0, 0, 0, ERRORCODE_USER_TIMEOUT );
      p_wk->recv_status  = MYSTERY_NET_RECV_STATUS_FAILED;
      WIFI_DOWNLOAD_WaitNdCleanCallback( p_nd_data, ND_RESULT_DOWNLOAD_CANCEL, p_seq, SEQ_END, SEQ_END ); 
    }

    if( s_callback_flag )
    { 
      s_callback_flag = FALSE;
      if( s_callback_result != DWC_ND_ERROR_NONE)
      {
        //�G���[���N�������̂ŏI��
        p_wk->recv_status  = MYSTERY_NET_RECV_STATUS_FAILED;
        WIFI_DOWNLOAD_WaitNdCleanCallback( p_nd_data, ND_RESULT_DOWNLOAD_CANCEL, p_seq, SEQ_END, SEQ_END );  
      }
      else
      { 
        *p_seq  = p_nd_data->next_seq;
      }
    }
    else if( p_nd_data->cancel_req )
    { 
      OS_Printf( "�L�����Z�����܂���!\n" );
      p_nd_data->wifi_cancel  = TRUE;
    }
    break;

  case SEQ_WAIT_CLEANUP_CALLBACK:
    DWC_NdProcess();

    if( p_nd_data->async_timeout++ > WIFI_ND_TIMEOUT_SYNC )
    {
      GFL_NET_StateSetWifiError( 0, 0, 0, ERRORCODE_USER_TIMEOUT );
      *p_seq  = p_nd_data->next_seq;
    }

    if( s_cleanup_callback_flag )
    { 
      s_cleanup_callback_flag = FALSE;
      *p_seq  = p_nd_data->next_seq;
    }
    else if( p_nd_data->cancel_req )
    { 
      OS_Printf( "�N���[���A�b�v���܂���! 2\n" );
      p_nd_data->wifi_cancel  = TRUE;
    }
    break;
  }
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
#if 0

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
#endif
  case SEQ_EXIT:
    if( GFL_NET_Exit( NULL ) )
    { 
      *p_seq  = SEQ_EXIT_WAIT;
    }
    else
    { 
      *p_seq  = SEQ_END;
    }
    break;

  case SEQ_EXIT_WAIT:
    if( !GFL_NET_IsInit() )
		{
      p_wk->is_exit = FALSE;
      *p_seq = SEQ_END;
    } 
    break;

  case SEQ_END:
    OS_TFPrintf( 3, "WIFI�I��\n" );
    SEQ_SetNext( p_seqwk, SEQFUNC_Wait );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �R�[���o�b�N�҂�
 *
 *	@param	WIFI_DOWNLOAD_DATA *p_wk  ���[�N
 *	@param	*p_seq                    �V�[�P���X
 *	@param	next_seq                  �R�[���o�b�N������ւ����V�[�P���X
 */
//-----------------------------------------------------------------------------
static void WIFI_DOWNLOAD_WaitNdCallback( WIFI_DOWNLOAD_DATA *p_wk, int *p_seq, int next_seq )
{ 
  s_callback_flag   = FALSE;
  s_callback_result = DWC_ND_ERROR_NONE;
  p_wk->next_seq  = next_seq;
  p_wk->async_timeout  = 0;
  *p_seq          = 100;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �R�[���o�b�N�҂��Q
 *
 *	@param	WIFI_DOWNLOAD_DATA *p_wk  ���[�N
 *	@param	result                    ����
 *	@param	*seq                      �V�[�P���X
 *	@param	next_seq                  �R�[���o�b�N������ɂ����V�[�P���X
 *	@param	false_seq                 �R�[���o�b�N���s��ɂ����V�[�P���X
 */
//-----------------------------------------------------------------------------
static void WIFI_DOWNLOAD_WaitNdCleanCallback( WIFI_DOWNLOAD_DATA *p_wk, int result, int *p_seq, int next_seq, int false_seq)
{ 
  s_cleanup_callback_flag = FALSE;
  s_callback_result       = result;
  p_wk->next_seq          = next_seq;
  p_wk->async_timeout  = 0;
  *p_seq                  = 200;

  if( !DWC_NdCleanupAsync(  ) ){  //FALSE�̏ꍇ�R�[���o�b�N���Ă΂�Ȃ�
    OS_Printf("DWC_NdCleanupAsync�Ɏ��s\n");
    *p_seq = false_seq;
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
static BOOL NETCALLBACK_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2, void* pWork )
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


/*-------------------------------------------------------------------------*
 * Name        : NdCallback
 * Description : ND�p�R�[���o�b�N
 * Arguments   : None.
 * Returns     : None.
 *-------------------------------------------------------------------------*/
static void NdCallback(DWCNdCallbackReason reason, DWCNdError error, int servererror)
{
  OS_Printf("NdCallback: Called\n");
  switch(reason) {
  case DWC_ND_CBREASON_GETFILELISTNUM:
    OS_Printf("DWC_ND_CBREASON_GETFILELISTNUM\n");
    break;
  case DWC_ND_CBREASON_GETFILELIST:
    OS_Printf("DWC_ND_CBREASON_GETFILELIST\n");
    break;
  case DWC_ND_CBREASON_GETFILE:
    OS_Printf("DWC_ND_CBREASON_GETFILE\n");
    break;
  case DWC_ND_CBREASON_INITIALIZE:
    OS_Printf("DWC_ND_CBREASON_INITIALIZE\n");
    break;
  }
	
  switch(error) {
  case DWC_ND_ERROR_NONE:
    OS_Printf("DWC_ND_NOERR\n");
    break;
  case DWC_ND_ERROR_ALLOC:
    OS_Printf("DWC_ND_MEMERR\n");
    break;
  case DWC_ND_ERROR_BUSY:
    OS_Printf("DWC_ND_BUSYERR\n");
    break;
  case DWC_ND_ERROR_HTTP:
    OS_Printf("DWC_ND_HTTPERR\n");
    // �t�@�C�����̎擾�łg�s�s�o�G���[�����������ꍇ�̓_�E�����[�h�T�[�o�Ɍq�����Ă��Ȃ��\��������
    if( reason == DWC_ND_CBREASON_GETFILELISTNUM )
      {
          OS_Printf( "It is not possible to connect download server.\n." );
      }
    break;
  case DWC_ND_ERROR_BUFFULL:
    OS_Printf("DWC_ND_BUFFULLERR\n");
    break;
  case DWC_ND_ERROR_DLSERVER:
    OS_Printf("DWC_ND_SERVERERR\n");
    break;
  case DWC_ND_ERROR_CANCELED:
    OS_Printf("DWC_ND_CANCELERR\n");
    break;
  }
  OS_Printf("errorcode = %d\n", servererror);
  s_callback_flag   = TRUE;
  s_callback_result = error;

  NdCleanupCallback();
}

/*-------------------------------------------------------------------------*
 * Name        : NdCleanupCallback
 * Description : DWC_NdCleanupAsync�p�R�[���o�b�N
 * Arguments   : None.
 * Returns     : None.
 *-------------------------------------------------------------------------*/
static void NdCleanupCallback( void )
{
  OS_Printf("--------------------------------\n");
  switch( s_cleanup_callback_result )
    {
    case ND_RESULT_COMPLETE:
      OS_Printf("DWC_ND: finished - complete -\n");
      break;
    case ND_RESULT_NOT_FOUND_FILES:
      OS_Printf("DWC_ND: finished - no files -\n");
      break;
    case ND_RESULT_DOWNLOAD_CANCEL:
      OS_Printf("DWC_ND: finished - cancel -\n");
      break;
    }
  OS_Printf("--------------------------------\n");
  s_cleanup_callback_flag = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  16�i�������񂩂琔�l�֕ϊ�����
 *
 *	@param	char *str   ������A�X�L�[�R�[�h  0�`FFFFFFFF
 *
 *	@return �ϊ���̐��l
 */
//-----------------------------------------------------------------------------
static u32 UTIL_StringToHex( const char *buf )
{ 
  static const struct
  { 
    char  c;
    s8    n;
  } sc_hash_tbl[] = 
  { 
    //����
    { 
      '0',0
    },
    { 
      '1',1
    },
    { 
      '2',2
    },
    { 
      '3',3
    },
    { 
      '4',4
    },
    { 
      '5',5
    },
    { 
      '6',6
    },
    { 
      '7',7
    },
    { 
      '8',8
    },
    { 
      '9',9
    },
    //�啶��
    { 
      'A',0xA
    },
    { 
      'B',0xB
    },
    { 
      'C',0xC
    },
    { 
      'D',0xD
    },
    { 
      'E',0xE
    },
    { 
      'F',0xF
    },
    //�������ł�OK
    {
      'a',0xA
    },
    { 
      'b',0xB
    },
    { 
      'c',0xC
    },
    { 
      'd',0xD
    },
    { 
      'e',0xE
    },
    { 
      'f',0xF
    },
    { 
      '\0',-1
    }
  };

  int i; 
  int num;
  int ret;
  const char *origin = buf;

  ret = 0;
  while(1)
  {
    num = -1;
    for( i = 0; i < NELEMS(sc_hash_tbl); i++ )
    { 
      if( sc_hash_tbl[ i ].c == *buf )
      { 
        num = sc_hash_tbl[ i ].n;
        break;
      }
    }
    
    if( num == -1 )
    { 
      //�I���R�[�h���e�[�u���ɂȂ��l�̏ꍇ�I��
      break;
    }
    else
    { 
      ret *= 0x10;  //16�i���ɂ���̂�
      ret += num;
      buf++;
    }
  }

  NAGI_Printf( "%s==[%d]\n", origin, ret );
  return ret;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �O�N���A����Ă��邩�ǂ���
 *
 *	@param	void *p_adrs  �A�h���X
 *	@param	size          �o�C�g�T�C�Y
 *
 *	@return TRUE�łO�N���A����Ă���  FALSE�ł���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL UTIL_IsClear( void *p_adrs, u32 size )
{
  int i;
  u32 *p_ptr  = (u32*)p_adrs;

  for( i = 0; i < size/4; i++ )
  {
    if( *(p_ptr + i) != 0 )
    {
      return FALSE;
    }
  }

  return TRUE;
}

#ifdef BUGFIX_BTS7821_20100714
//----------------------------------------------------------------------------
/**
 *	@brief  �G���[���ɐؒf�̑O�ɌĂ΂��R�[���o�b�N  �g����
 *
 *	@param	p_wk_adrs ���[�N�A�h���X
 *	@param	code      �G���[�R�[�h
 *	@param	type      �G���[�^�C�v
 *	@param	ret       �G���[���^�[��
 *  @retval  TRUE�Ȃ�ΐؒf���������Ȃ��ł��̊֐����ĂтÂ���@
 *          FALSE�Ȃ�ΐؒf���������ďI������
 */
//-----------------------------------------------------------------------------
static BOOL Mystery_Net_ErrDisconnectCallbackEx(void* p_wk_adrs, int code, int type, int ret )
{
  WIFI_DOWNLOAD_DATA *p_wk  = p_wk_adrs;

  NAGI_Printf( "Call ErrDisconnectCallback[EX] code=%d type=%d ret=%d\n", code, type, ret );
  switch( type )
  {
    //�ؒf�G���[
  case DWC_ETYPE_SHUTDOWN_FM:
  case DWC_ETYPE_DISCONNECT:
    return p_wk->is_nd_disconnect_enable;
  }

  return FALSE;
}
#endif //BUGFIX_BTS7821_20100714
