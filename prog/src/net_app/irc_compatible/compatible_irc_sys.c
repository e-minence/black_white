//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		compatible_irc_sys.h
 *	@brief	�����f�f�Q�[���p�ԊO���ڑ��V�X�e��
 *	@author	Toru=Nagihashi
 *	@data		2009.05.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//lib
#include <gflib.h>

//const
#include "system/main.h"
#include "system/gfl_use.h"

//mine
#include "net_app/compatible_irc_sys.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�f�o�b�O
//=====================================
#ifdef PM_DEBUG
#define DEBUG_COMPATIBLE_IRC_PRINT
#endif //PM_DEBUG

#ifdef DEBUG_COMPATIBLE_IRC_PRINT
#define IRC_Print(...)	OS_TPrintf( __VA_ARGS__ )
#else
#define IRC_Print(...)	((void)0)
#endif //DEBUG_COMPATIBLE_IRC_PRINT

//-------------------------------------
///	
//=====================================
#define COMPATIBLE_IRC_CONNECT_TIMINGSYNC_NO	(14)
#define COMPATIBLE_IRC_TIMINGSYNC_NO					(15)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	���j���[�p�l�b�g
//=====================================
typedef struct
{	
	u32	select;	//�I�񂾂���
	u32	ms;	//���j���[��I�񂾎���
} MENUNET_DECIDEMENU_DATA;
typedef struct 
{
	COMPATIBLE_IRC_SYS	*p_irc;
	u32 seq;
	MENUNET_DECIDEMENU_DATA	menudata;
	MENUNET_DECIDEMENU_DATA	menusend;
	BOOL	is_recv;
	BOOL	is_return;
	BOOL	is_start_proc;
	BOOL	is_status_recv;
	MYSTATUS	*p_send_status;
	MYSTATUS	*p_my_status;
	MYSTATUS	*p_you_status;
} MENUNET_WORK;

//-------------------------------------
///	�V�X�e�����[�N�G���A
//=====================================
struct _COMPATIBLE_IRC_SYS
{	
	MENUNET_WORK	menu;

	u32		irc_timeout;
	u32		seq;
	u32		err_seq;
	u32		connect_bit;
	BOOL	is_connect;
	BOOL	is_exit;
	BOOL	is_recv;
	BOOL	is_return;
	BOOL	is_start;
	u32		random;
	BOOL	is_timing[COMPATIBLE_TIMING_NO_MAX];

	u8		mac_address[6];

	HEAPID	heapID;
};




//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//network_setup_callback
static void NET_INIT_NegotiationCallBack( void *p_work, int netID );
static void* NET_INIT_GetBeaconCallBack( void *p_work );
static int NET_INIT_GetBeaconSizeCallBack( void *p_work );
static BOOL NET_INIT_CompBeaconCallBack( GameServiceID myNO, GameServiceID beaconNO );
static void NET_INIT_DisConnectCallBack( void *p_work );
static void NET_INIT_InitCallBack( void *p_work );
static void NET_EXIT_ExitCallBack( void *p_work );
//��M�R�}���h�e�[�u��
static void NET_RECV_Timing( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle );

//net
static void MENUNET_Init( MENUNET_WORK *p_wk, COMPATIBLE_IRC_SYS *p_irc, HEAPID heapID );
static void MENUNET_Exit( MENUNET_WORK *p_wk );
static BOOL MENUNET_SendMenuData( MENUNET_WORK *p_wk, u32 ms, u32 select );
static BOOL MENUNET_RecvMenuData( MENUNET_WORK *p_wk );
static void MENUNET_GetMenuData( const MENUNET_WORK *cp_wk, u32 *p_ms, u32 *p_select );
static BOOL MENUNET_SendReturnMenu( MENUNET_WORK *p_sys );
static BOOL MENUNET_RecvReturnMenu( MENUNET_WORK *p_sys );
static BOOL MENUNET_SendStatusData( MENUNET_WORK *p_wk, GAMESYS_WORK *p_gamesys );
static void MENUNET_GetStatusData( const MENUNET_WORK *cp_wk, MYSTATUS *p_status );
//netrecv
static void NETRECV_DecideMenu( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle );
static void NETRECV_RecvMenu( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle );
static void NETRECV_StartProcTiming( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle );
static void NETRECV_SendStatus( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle );
static u8*NETRECV_GetStatusBuffer( int netID, void* p_work, int size );
//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	��M�R�[���o�b�N
//=====================================
enum
{
	SENDCMD_TIMING = GFL_NET_CMD_IRCCOMPATIBLE,
};
static const NetRecvFuncTable sc_net_recv_tbl[]	=
{	
	{	
		NET_RECV_Timing, NULL
	},
};

//-------------------------------------
///	�r�[�R��
//=====================================
typedef struct{
    int gameNo;   ///< �Q�[�����
} COMPATIBLE_IRC_BEACON;
static COMPATIBLE_IRC_BEACON sc_beacon = { WB_NET_IRCCOMPATIBLE };

//-------------------------------------
///	�������\����
//=====================================
static const GFLNetInitializeStruct sc_net_init =
{
    sc_net_recv_tbl,  // ��M�֐��e�[�u��
    NELEMS(sc_net_recv_tbl), // ��M�e�[�u���v�f��
    NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
    NET_INIT_NegotiationCallBack,    ///< �l�S�V�G�[�V�����������ɃR�[��
    NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    NET_INIT_GetBeaconCallBack,  // �r�[�R���f�[�^�擾�֐�
    NET_INIT_GetBeaconSizeCallBack,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
    NET_INIT_CompBeaconCallBack,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
    FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
    NET_INIT_DisConnectCallBack,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,   ///< DWC�`���̗F�B���X�g	
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
    0x532,//0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,  //���ɂȂ�heapID
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    HEAPID_IRC,   //��check�@�ԊO���ʐM�p��create�����HEAPID
    GFL_WICON_POSX, GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
    COMPATIBLE_IRC_CONNEXT_NUM,     // �ő�ڑ��l��
    COMPATIBLE_IRC_SENDDATA_BYTE,  //�ő呗�M�o�C�g��
    COMPATIBLE_IRC_GETBEACON_NUM,    // �ő�r�[�R�����W��
    TRUE,     // CRC�v�Z
    FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    GFL_NET_TYPE_IRC,  //wifi�ʐM���s�����ǂ���
    TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_IRCCOMPATIBLE,  //GameServiceID
#if GFL_NET_IRC
		IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
};


//-------------------------------------
///	NET
//=====================================
enum
{	
	NETRECV_DECIDE_MENU	= GFL_NET_CMD_IRCMENU,
	NETRECV_RECV_MENU,
	NETRECV_START_PROC,
	NETRECV_SEND_STATUS,
};
static const NetRecvFuncTable	sc_recv_tbl[]	=
{
	{	
		NETRECV_DecideMenu, NULL
	},
	{	
		NETRECV_RecvMenu, NULL
	},
	{	
		NETRECV_StartProcTiming, NULL
	},
	{	
		NETRECV_SendStatus, NETRECV_GetStatusBuffer,
	}
};

//=============================================================================
/**
 *					GLOBAL
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�V�X�e���쐬
 *
 *	@param	u32 irc_timeout	�ԊO���^�C���A�E�g����
 *	@param	heapID					�q�[�vID
 *
 *	@return	�V�X�e���n���h��
 */
//-----------------------------------------------------------------------------
COMPATIBLE_IRC_SYS * COMPATIBLE_IRC_CreateSystem( u32 irc_timeout, HEAPID heapID )
{	
	COMPATIBLE_IRC_SYS *p_sys;
	//���[�N�쐬
	p_sys	= GFL_HEAP_AllocMemory( heapID, sizeof(COMPATIBLE_IRC_SYS) );
	GFL_STD_MemClear( p_sys, sizeof(COMPATIBLE_IRC_SYS) );
	//������
	p_sys->irc_timeout		= irc_timeout;
	p_sys->heapID	= heapID;

	return p_sys;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�V�X�e��������
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys		���[�N
 *
 */
//-----------------------------------------------------------------------------
void COMPATIBLE_IRC_DeleteSystem( COMPATIBLE_IRC_SYS *p_sys )
{	
	GFL_HEAP_FreeMemory( p_sys );
}


//----------------------------------------------------------------------------
/**
 *	@brief	�������҂�����
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys		���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@ratval	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
BOOL COMPATIBLE_IRC_InitWait( COMPATIBLE_IRC_SYS *p_sys )
{	
	enum
	{	
		SEQ_INIT_START,
		SEQ_INIT_WAIT,
		SEQ_INIT_MENU,
		SEQ_INIT_END,
	};

	switch( p_sys->seq )
	{
	case SEQ_INIT_START:
		{
			GFLNetInitializeStruct net_init = sc_net_init;
			net_init.irc_timeout = p_sys->irc_timeout;
			GFL_NET_Init( &net_init, NET_INIT_InitCallBack, p_sys );
		}
		p_sys->seq	= SEQ_INIT_WAIT;
		break;

	case SEQ_INIT_WAIT:
		if(GFL_NET_IsInit())
		{
			p_sys->seq	= SEQ_INIT_MENU;
		}
		break;

	case SEQ_INIT_MENU:
		MENUNET_Init( &p_sys->menu, p_sys, p_sys->heapID );
		p_sys->seq	= SEQ_INIT_END;
		break;

	case SEQ_INIT_END:
		p_sys->seq	= 0;
		return TRUE;

	default:
		GF_ASSERT_MSG( 0, "NET SEQ ERROR %d\n", p_sys->seq  );
	}
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I���҂�����
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys		���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@ratval	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
BOOL COMPATIBLE_IRC_ExitWait( COMPATIBLE_IRC_SYS *p_sys )
{	
	enum
	{	
		SEQ_EXIT_START,
		SEQ_EXIT_WAIT,
		SEQ_EXIT_END,
	};

	switch(p_sys->seq)
	{
	case SEQ_EXIT_START:
		if( GFL_NET_Exit(NET_EXIT_ExitCallBack ) )
		{
			p_sys->seq	= SEQ_EXIT_WAIT;
		}
		else
		{
			return TRUE;
		}
		break;

	case SEQ_EXIT_WAIT:
		if( p_sys->is_exit )
		{
			p_sys->seq	= SEQ_EXIT_END;
		}
		break;

	case SEQ_EXIT_END:
		p_sys->seq	= 0;
		return TRUE;

	default:
		GF_ASSERT_MSG( 0, "NET SEQ ERROR %d\n", p_sys->seq  );
	}
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ڑ��҂�����
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys		���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@ratval	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
BOOL COMPATIBLE_IRC_ConnextWait( COMPATIBLE_IRC_SYS *p_sys )
{	
	enum
	{	
		SEQ_CONNECT_START,
		SEQ_CONNECT_WAIT,
		SEQ_CONNECT_TIMING_START,
		SEQ_CONNECT_TIMING_WAIT,
		SEQ_CONNECT_END,
	};

	switch( p_sys->seq ){
	case SEQ_CONNECT_START:
		GFL_NET_ChangeoverConnect( NULL );
		p_sys->seq	= SEQ_CONNECT_WAIT;
		break;

	case SEQ_CONNECT_WAIT:
		//�����ڑ��҂�
		if( p_sys->is_connect )
		{
			IRC_Print("�ڑ�����\n");
			p_sys->seq	= SEQ_CONNECT_TIMING_START;
		}
		break;

	case SEQ_CONNECT_TIMING_START:
		GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),
				COMPATIBLE_IRC_CONNECT_TIMINGSYNC_NO);
		p_sys->seq	= SEQ_CONNECT_TIMING_WAIT;
		break;

	case SEQ_CONNECT_TIMING_WAIT:
		if( GFL_NET_HANDLE_IsTimingSync( GFL_NET_HANDLE_GetCurrentHandle(),
					COMPATIBLE_IRC_CONNECT_TIMINGSYNC_NO) )
		{
			p_sys->seq	= SEQ_CONNECT_END;
		}
		break;

	case SEQ_CONNECT_END:
		IRC_Print("�^�C�~���O��萬��\n");
		IRC_Print("�ڑ��l�� = %d\n", GFL_NET_GetConnectNum());
		p_sys->seq = 0;
		return TRUE;

	default:
		GF_ASSERT_MSG( 0, "NET SEQ ERROR %d\n", p_sys->seq  );
	}
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ؒf�҂�����
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys		���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@ratval	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
BOOL COMPATIBLE_IRC_DisConnextWait( COMPATIBLE_IRC_SYS *p_sys )
{	
	enum
	{	
		SEQ_DISCONNECT_MENU,
		SEQ_DISCONNECT_START,
		SEQ_DISCONNECT_WAIT,
		SEQ_DISCONNECT_END,
	};

	switch(p_sys->seq){
	case SEQ_DISCONNECT_MENU:
		MENUNET_Exit( &p_sys->menu );
		p_sys->seq	= SEQ_DISCONNECT_START;
		break;

	case SEQ_DISCONNECT_START:
#if 0
		if(GFL_NET_IsParentMachine() == TRUE)
		{
			IRC_Print("�e�@�FGFL_NET_CMD_EXIT_REQ���M\n");
			if( GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_CMD_EXIT_REQ, 0, NULL) )
			{
				p_sys->seq	= SEQ_DISCONNECT_WAIT;
			}
		}
		else
		{
			p_sys->seq	= SEQ_DISCONNECT_WAIT;
		}
#else
		//������I��点�Ă��܂��̂ŁAMENU��NET���������Ȃ��Ȃ��Ă��܂�
		//���������I��邽��Exit�����
		if( GFL_NET_Exit(NULL) )
		{	
			p_sys->seq	= SEQ_DISCONNECT_WAIT;
		}
#endif
//		break;
//	fall through

	case SEQ_DISCONNECT_WAIT:
		if( !p_sys->is_connect || GFL_NET_IsExit() ){
			p_sys->seq = SEQ_DISCONNECT_END;
		}
		break;

	case SEQ_DISCONNECT_END:
		p_sys->seq = 0;
		return TRUE;

	default:
		GF_ASSERT_MSG( 0, "NET SEQ ERROR %d\n", p_sys->seq  );
	}
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�^�C�~���O�҂�����
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys		���[�N
 *	@param	timing_no	�^�C�~���ONO
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@ratval	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
BOOL COMPATIBLE_IRC_TimingSyncWait( COMPATIBLE_IRC_SYS *p_sys, COMPATIBLE_TIMING_NO timing_no )
{
	enum
	{	
		SEQ_TIMING_START,
		SEQ_TIMING_WAIT,
		SEQ_TIMING_END,
	};

	switch( p_sys->seq )
	{	
	case SEQ_TIMING_START:
		if( COMPATIBLE_IRC_SendData( p_sys, SENDCMD_TIMING, sizeof(COMPATIBLE_IRC_TIMINGSYNC_NO), &timing_no ) )
		{	
			p_sys->seq	= SEQ_TIMING_WAIT;
		}
		break;

	case SEQ_TIMING_WAIT:
		if( p_sys->is_timing[timing_no] )
		{
			p_sys->is_timing[timing_no]	= FALSE;
			p_sys->seq	= SEQ_TIMING_END;
		}
		break;

	case SEQ_TIMING_END:
		IRC_Print("�^�C�~���O��萬��\n");
		p_sys->seq = 0;
		return TRUE;

	default:
		GF_ASSERT_MSG( 0, "NET SEQ ERROR %d\n", p_sys->seq  );
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Wait�������L�����Z������
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys		���[�N
 *
 */
//-----------------------------------------------------------------------------
void COMPATIBLE_IRC_Cancel( COMPATIBLE_IRC_SYS *p_sys )
{	
	p_sys->seq	= 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�G���[�`�F�b�N�҂�����
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys		���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@ratval	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
BOOL COMPATIBLE_IRC_IsError( COMPATIBLE_IRC_SYS *p_sys )
{	
	enum
	{	
		SEQ_ERROR_START,
		SEQ_ERROR_WAIT,
		SEQ_ERROR_END,
	};

	switch(p_sys->err_seq){
	case SEQ_ERROR_START:
		if(GFL_NET_IsInit() == TRUE && GFL_NET_SystemIsError() != 0)
		{
			IRC_Print("�ʐM�G���[������\n");
			if(GFL_NET_Exit(NET_EXIT_ExitCallBack) == TRUE)
			{
				p_sys->err_seq = SEQ_ERROR_WAIT;
			}
			else
			{
				p_sys->err_seq = SEQ_ERROR_END;
			}
			return TRUE;
		}
		break;

	case SEQ_ERROR_WAIT:
		if(p_sys->is_exit == TRUE)
		{
			p_sys->err_seq = SEQ_ERROR_END;
		}
		return TRUE;

	case SEQ_ERROR_END:
		GFL_STD_MemClear(p_sys, sizeof(COMPATIBLE_IRC_SYS));
		return FALSE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]������
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys	���[�N
 *	@param	send_command							���M�R�}���h
 *	@param	size											�T�C�Y
 *	@param	void *cp_data							�f�[�^
 *
 *	@retval	TRUE�Ȃ�Α��M
 *	@ratval	FALSE�Ȃ�Α��M�ł��Ȃ�����
 */
//-----------------------------------------------------------------------------
BOOL COMPATIBLE_IRC_SendData( COMPATIBLE_IRC_SYS *p_sys, u16 send_command, u16 size, const void *cp_data )
{	
	return GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), send_command, size, cp_data );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�]������	EX��
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys	���[�N
 *	@param	send_command							���M�R�}���h
 *	@param	size											�T�C�Y
 *	@param	void *cp_data							�f�[�^
 *	@param	sendID										���鑊��	GFL_NET_SENDID_ALLUSER
 *	@param	BOOL b_fast								�i�ʏ��FALSE�j
 *	@param	BOOL b_repeat							�i�ʏ��FALSE�j
 *	@param	BOOL b_send_buff_lock			�傫���f�[�^�𑗂�A�o�b�t�@�������ŕێ�����ꍇTRUE
 *
 *	@retval	TRUE�Ȃ�Α��M
 *	@ratval	FALSE�Ȃ�Α��M�ł��Ȃ�����
 */
//-----------------------------------------------------------------------------
BOOL COMPATIBLE_IRC_SendDataEx( COMPATIBLE_IRC_SYS *p_sys, u16 send_command, u16 size, const void *cp_data, const NetID sendID, const BOOL b_fast, const BOOL b_repeat, const BOOL b_send_buff_lock )
{	
	return GFL_NET_SendDataEx( GFL_NET_HANDLE_GetCurrentHandle(), sendID,
			send_command, size, cp_data, b_fast, b_repeat, b_send_buff_lock );
}
//----------------------------------------------------------------------------
/**
 *	@brief	��M�e�[�u����ǉ�
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys	���[�N
 *	@param	cmdkindID									�R�}���h�Ăяo���̌n
 *	@param	NetRecvFuncTable *cp_net_recv_tbl	��M�e�[�u��
 *	@param	tbl_max														�e�[�u���v�f��
 *	@param	*p_param													��M�R�[���o�b�N�ł����Ƃ�p�����[�^
 *
 */
//-----------------------------------------------------------------------------
void COMPATIBLE_IRC_AddCommandTable( COMPATIBLE_IRC_SYS *p_sys, int cmdkindID, const NetRecvFuncTable *cp_net_recv_tbl, int tbl_max, void *p_param )
{	
	GFL_NET_AddCommandTable(cmdkindID, cp_net_recv_tbl, tbl_max, p_param );
}

//----------------------------------------------------------------------------
/**
 *	@brief	��M�e�[�u�����폜
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys	���[�N
 *	@param	cmdkindID									�R�}���h�Ăё����̌n�ԍ�
 *
 */
//-----------------------------------------------------------------------------
void COMPATIBLE_IRC_DelCommandTable( COMPATIBLE_IRC_SYS *p_sys, int cmdkindID )
{	
	GFL_NET_DelCommandTable( cmdkindID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���j���[�f�[�^���M
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys	���[�N
 *	@param	ms												�b��
 *	@param	select										�I�񂾂���
 *
 *	@retval	TRUE�Ȃ�Α��M
 *	@ratval	FALSE�Ȃ�Α��M�ł��Ȃ�����
 */
//-----------------------------------------------------------------------------
BOOL COMPATIBLE_MENU_SendMenuData( COMPATIBLE_IRC_SYS *p_sys, u32 ms, u32 select )
{	
	return MENUNET_SendMenuData( &p_sys->menu, ms, select );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[�f�[�^��M
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys	���[�N
 *
 *	@retval	TRUE�Ȃ�Ύ�M
 *	@ratval	FALSE�Ȃ�Ύ�M�ł��Ȃ�����
 */
//-----------------------------------------------------------------------------
BOOL COMPATIBLE_MENU_RecvMenuData( COMPATIBLE_IRC_SYS *p_sys )
{	
	return MENUNET_RecvMenuData( &p_sys->menu );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[�f�[�^�擾
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys	���[�N
 *	@param	ms												�b���󂯎��
 *	@param	select										�I�񂾂��̎󂯎��
 *
 */
//-----------------------------------------------------------------------------
void COMPATIBLE_MENU_GetMenuData( const COMPATIBLE_IRC_SYS *cp_sys, u32 *p_ms, u32 *p_select )
{	
	MENUNET_GetMenuData( &cp_sys->menu, p_ms, p_select );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[���^�[��
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys ���[�N
 *
 *	@retval	TRUE�Ȃ�Α��M
 *	@ratval	FALSE�Ȃ�Α��M�ł��Ȃ�����
 */
//-----------------------------------------------------------------------------
BOOL COMPATIBLE_MENU_SendReturnMenu( COMPATIBLE_IRC_SYS *p_sys )
{	
	return MENUNET_SendReturnMenu( &p_sys->menu );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[���^�[����M
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys ���[�N
 *
 *	@retval	TRUE�Ȃ�Ύ�M
 *	@ratval	FALSE�Ȃ�Ύ�M�ł��Ȃ�����
 */
//-----------------------------------------------------------------------------
BOOL COMPATIBLE_MENU_RecvReturnMenu( COMPATIBLE_IRC_SYS *p_sys )
{	
	return MENUNET_RecvReturnMenu( &p_sys->menu );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�X�e�[�^�X���M
 *
 *	@param	COMPATIBLE_IRC_SYS *p_wk	���[�N
 *	@param	*p_gamesys								�Q�[���V�X�e��
 *
 *	@retval	TRUE�Ȃ�Α��M
 *	@ratval	FALSE�Ȃ�Α��M�ł��Ȃ�����
 */
//-----------------------------------------------------------------------------
BOOL COMPATIBLE_MENU_SendStatusData( COMPATIBLE_IRC_SYS *p_sys, GAMESYS_WORK *p_gamesys )
{	
	return MENUNET_SendStatusData( &p_sys->menu, p_gamesys );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�X�e�[�^�X�󂯎��
 *
 *	@param	const COMPATIBLE_IRC_SYS *cp_wk	���[�N
 *	@param	*p_status												�X�e�[�^�X
 *
 */
//-----------------------------------------------------------------------------
void COMPATIBLE_MENU_GetStatusData( const COMPATIBLE_IRC_SYS *cp_sys, MYSTATUS *p_status )
{	
	MENUNET_GetStatusData( &cp_sys->menu, p_status );
}
//=============================================================================
/**
 *		network_setup_callback
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�l�S�V�G�[�V�����������ɌĂ΂��
 *
 *	@param	void *p_work	���[�N
 *	@param	netID					�l�b�gID
 *
 */
//-----------------------------------------------------------------------------
static void NET_INIT_NegotiationCallBack( void *p_work, int netID )
{	
	COMPATIBLE_IRC_SYS *p_sys	= p_work;
	u32 temp;

	//�l�����J�E���g
	p_sys->connect_bit |= 1 << netID;
	IRC_Print( "�����`�F�b�N�ʐM�V�X�e���l�S�V�G�[�V�������� %d \n", netID );

	//�l������������ʐM����
	temp	= p_sys->connect_bit;
	if( MATH_CountPopulation(temp) >= COMPATIBLE_IRC_CONNEXT_NUM )
	{	
		IRC_Print( "�����`�F�b�N�ʐM�V�X�e������������ �l���r�b�g%x \n", p_sys->connect_bit );
		p_sys->is_connect	= TRUE;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�r�[�R���擾�R�[���o�b�N
 *
 *	@param	void *p_work	���[�N
 *
 *	@return	�r�[�R���̃A�h���X
 */
//-----------------------------------------------------------------------------
static void* NET_INIT_GetBeaconCallBack( void *p_work )
{	
	return &sc_beacon;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�r�[�R���T�C�Y�擾�R�[���o�b�N
 *
 *	@param	void *p_work	���[�N
 *
 *	@return	�r�[�R���̃T�C�Y
 */
//-----------------------------------------------------------------------------
static int NET_INIT_GetBeaconSizeCallBack( void *p_work )
{	
	return sizeof(COMPATIBLE_IRC_BEACON);
}
//----------------------------------------------------------------------------
/**
 *	@brief	�r�[�R����r�֐�
 *
 *	@param	GameServiceID myNO	�����̔ԍ�
 *	@param	beaconNO						����̔ԍ�
 *
 *	@return	�ڑ����Ă悢�Ȃ��TRUE�A�����Ȃ���FALSE
 */
//-----------------------------------------------------------------------------
static BOOL NET_INIT_CompBeaconCallBack( GameServiceID myNO, GameServiceID beaconNO )
{	
	return myNO	== beaconNO;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ؒf�������ɌĂ΂��
 *
 *	@param	void *p_work	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void NET_INIT_DisConnectCallBack( void *p_work )
{	
	COMPATIBLE_IRC_SYS *p_sys	= p_work;
	p_sys->is_connect	= FALSE;
	IRC_Print( "�����`�F�b�N�ʐM�V�X�e���ؒf��������\n" );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�V�X�e�������������R�[���o�b�N
 *
 *	@param	void *p_work	���[�N
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void NET_INIT_InitCallBack( void *p_work )
{	
	IRC_Print( "�����`�F�b�N�ʐM�V�X�e������������\n" );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM����������R�[���o�b�N
 *
 *	@param	void *p_work	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void NET_EXIT_ExitCallBack( void *p_work )
{	
	COMPATIBLE_IRC_SYS *p_sys	= p_work;
	p_sys->is_exit	= TRUE;
	IRC_Print( "�����`�F�b�N�ʐM�V�X�e���������\n" );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�^�C�~���O���킹
 *
 *	@param	const int netID	�l�b�gID
 *	@param	int size				�T�C�Y
 *	@param	void* cp_data		�f�[�^
 *	@param	p_work					�p�����[�^
 *	@param	p_net_handle		�l�b�g�n���h��
 *
 */
//-----------------------------------------------------------------------------
static void NET_RECV_Timing( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle )
{	
	COMPATIBLE_IRC_SYS *p_sys	= p_work;
	COMPATIBLE_TIMING_NO	no;

	if( p_net_handle != GFL_NET_HANDLE_GetCurrentHandle() )
	{
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	if( netID == GFL_NET_SystemGetCurrentID() )
	{
		return;	//�����̃f�[�^�͖���
	}

	GFL_STD_MemCopy( cp_data, &no, sizeof(COMPATIBLE_IRC_TIMINGSYNC_NO) );

	GF_ASSERT( no < COMPATIBLE_TIMING_NO_MAX );
	p_sys->is_timing[no]		= TRUE;
	IRC_Print( "�ԓ� %d \n", no );
}

//=============================================================================
/**
 *					NET
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	NET������
 *
 *	@param	MENUNET_WORK *p_wk	���[�N
 *	@param	COMPATIBLE_IRC_SYS	�ԊO��
 *	@param	HEAPID							�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void MENUNET_Init( MENUNET_WORK *p_wk, COMPATIBLE_IRC_SYS *p_irc, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(MENUNET_WORK) );
	p_wk->p_irc	= p_irc;
	p_wk->p_send_status	= GFL_HEAP_AllocMemory( heapID, MyStatus_GetWorkSize() );
	GFL_STD_MemClear( p_wk->p_send_status, MyStatus_GetWorkSize() );
	p_wk->p_my_status	= GFL_HEAP_AllocMemory( heapID, MyStatus_GetWorkSize() );
	GFL_STD_MemClear( p_wk->p_my_status, MyStatus_GetWorkSize() );
	p_wk->p_you_status	= GFL_HEAP_AllocMemory( heapID, MyStatus_GetWorkSize() );
	GFL_STD_MemClear( p_wk->p_you_status, MyStatus_GetWorkSize() );

	COMPATIBLE_IRC_AddCommandTable( p_irc, GFL_NET_CMD_IRCMENU, sc_recv_tbl, NELEMS(sc_recv_tbl), p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	NET�j��
 *
 *	@param	MENUNET_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void MENUNET_Exit( MENUNET_WORK *p_wk )
{	
	COMPATIBLE_IRC_DelCommandTable( p_wk->p_irc, GFL_NET_CMD_IRCMENU );
	GFL_HEAP_FreeMemory( p_wk->p_send_status );
	GFL_HEAP_FreeMemory( p_wk->p_my_status );
	GFL_HEAP_FreeMemory( p_wk->p_you_status );
	GFL_STD_MemClear( p_wk, sizeof(MENUNET_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[����f�[�^���M
 *
 *	@param	MENUNET_WORK *p_wk	���[�N
 *	@param	MENUNET_DECIDEMENU_DATA *cp_data	���M�f�[�^
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@ratval	FALSE�Ȃ�Ώ�����
 **/
//-----------------------------------------------------------------------------
static BOOL MENUNET_SendMenuData( MENUNET_WORK *p_wk, u32 ms, u32 select )
{	

	p_wk->menusend.ms	= ms;
	p_wk->menusend.select	= select;

	if(COMPATIBLE_IRC_SendData( p_wk->p_irc, 
				NETRECV_DECIDE_MENU, sizeof(MENUNET_DECIDEMENU_DATA), &p_wk->menusend ))
	{
		OS_TPrintf("���j���[���M�J�n\n");
		return TRUE;
	}
	
	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[����f�[�^��M�҂�
 *
 *	@param	MENUNET_WORK *p_wk	���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@ratval	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL MENUNET_RecvMenuData( MENUNET_WORK *p_wk )
{	
	if( p_wk->is_recv == TRUE )
	{
		OS_TPrintf("���j���[��M����\n");
		p_wk->is_recv = FALSE;
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���j���[����f�[�^�󂯎��
 *
 *	@param	const MENUNET_WORK *cp_wk	���[�N
 *	@param	*p_data										���j���[����f�[�^�󂯎��
 *
 */
//-----------------------------------------------------------------------------
static void MENUNET_GetMenuData( const MENUNET_WORK *cp_wk, u32 *p_ms, u32 *p_select )
{	
	if( p_ms )
	{	
		*p_ms			= cp_wk->menudata.ms;
	}

	if( p_select )
	{	
		*p_select	= cp_wk->menudata.select;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[�ԐM�f�[�^���M
 *
 *	@param	MENUNET_WORK *p_wk	���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@ratval	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL MENUNET_SendReturnMenu( MENUNET_WORK *p_wk )
{	
	u32 dummy;
	if(COMPATIBLE_IRC_SendData( p_wk->p_irc, 
				NETRECV_RECV_MENU, sizeof(u32), &dummy ))
	{
		OS_TPrintf("�ԐM�J�n\n");
		return TRUE;
	}
	
	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[�ԐM�f�[�^��M
 *
 *	@param	MENUNET_WORK *p_wk	���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@ratval	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL MENUNET_RecvReturnMenu( MENUNET_WORK *p_wk )
{	
	if( p_wk->is_return == TRUE )
	{
		p_wk->is_return = FALSE;
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PROC�J�n�^�C�~���O�擾
 *
 *	@param	MENUNET_WORK *p_wk	���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@ratval	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL MENUNET_SendStartProcTiming( MENUNET_WORK *p_wk )
{	
	enum
	{	
		SEQ_PROC_SEND,
		SEQ_PROC_WAIT,
	};

	u32 dummy;

	switch( p_wk->seq )
	{	

	case SEQ_PROC_SEND:

		
		if(COMPATIBLE_IRC_SendData( p_wk->p_irc, 
					NETRECV_START_PROC, sizeof(u32), &dummy ))
		{
			OS_TPrintf("���j���[���M�J�n\n");
			p_wk->seq	= SEQ_PROC_WAIT;
		}
		break;

	case SEQ_PROC_WAIT:
		if( p_wk->is_start_proc )
		{	
			p_wk->is_start_proc	= FALSE;
			p_wk->seq			= 0;
			return TRUE;
		}
		break;
	
	}
	
	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����̃X�e�[�^�X�𑗐M
 *
 *	@param	MENUNET_WORK *p_wk	���[�N
 *	@param	cp_data							�����̃X�e�[�^�X
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@ratval	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
static BOOL MENUNET_SendStatusData( MENUNET_WORK *p_wk, GAMESYS_WORK *p_gamesys )
{	
	enum
	{	
		SEQ_PROC_SEND,
		SEQ_PROC_WAIT,
	};

	u32 dummy;
	MYSTATUS *p_status;
	PLAYER_WORK *p_player;

	switch( p_wk->seq )
	{	

	case SEQ_PROC_SEND:
		//�f�[�^�擾
		if( p_gamesys )
		{	
			p_player	= GAMESYSTEM_GetMyPlayerWork( p_gamesys );
			p_status	= &p_player->mystatus;
		}
		else
		{	
			return TRUE;
		}

		//���M
		if(COMPATIBLE_IRC_SendDataEx( p_wk->p_irc, 
					NETRECV_SEND_STATUS, MyStatus_GetWorkSize(), p_status, 
					GFL_NET_SENDID_ALLUSER, FALSE, FALSE, TRUE ))
		{
			OS_TPrintf("���j���[���M�J�n\n");
			p_wk->seq	= SEQ_PROC_WAIT;
		}
		break;

	case SEQ_PROC_WAIT:
		if( p_wk->is_status_recv )
		{	
			p_wk->is_status_recv	= FALSE;
			p_wk->seq			= 0;
			return TRUE;
		}
		break;
	
	}
	
	return FALSE;

}

//----------------------------------------------------------------------------
/**
 *	@brief	����̃X�e�[�^�X���擾
 *
 *	@param	const MENUNET_WORK *cp_wk	���[�N	
 *	@param	*p_status									�X�e�[�^�X�i�[�o�b�t�@
 *
 */
//-----------------------------------------------------------------------------
static void MENUNET_GetStatusData( const MENUNET_WORK *cp_wk, MYSTATUS *p_status )
{	
	MyStatus_Copy(cp_wk->p_you_status, p_status);
}

//=============================================================================
/**
 *				NETRECV
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[����f�[�^���M
 *
 *	@param	const int netID	�l�b�gID
 *	@param	int size				�T�C�Y
 *	@param	void* p_data		�f�[�^
 *	@param	p_work					���[�N
 *	@param	p_net_handle		�n���h��
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void NETRECV_DecideMenu( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle )
{	
	MENUNET_WORK *p_wk	= p_work;

	if( p_net_handle != GFL_NET_HANDLE_GetCurrentHandle() )
	{
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	if( netID == GFL_NET_SystemGetCurrentID() )
	{
		return;	//�����̃f�[�^�͖���
	}

	GFL_STD_MemCopy( cp_data, &p_wk->menudata, sizeof(MENUNET_DECIDEMENU_DATA) );
	NAGI_Printf("���ʃf�[�^�󂯎�芮��\n" );
	p_wk->is_recv		= TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[�f�[�^�󂯎��ԐM
 *
 *	@param	const int netID	�l�b�gID
 *	@param	int size				�T�C�Y
 *	@param	void* p_data		�f�[�^
 *	@param	p_work					���[�N
 *	@param	p_net_handle		�n���h��
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void NETRECV_RecvMenu( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle )
{	
	MENUNET_WORK *p_wk	= p_work;

	if( p_net_handle != GFL_NET_HANDLE_GetCurrentHandle() )
	{
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	if( netID == GFL_NET_SystemGetCurrentID() )
	{
		return;	//�����̃f�[�^�͖���
	}

	NAGI_Printf("�ԐM����\n" );
	p_wk->is_return		= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PROC�J�n�^�C�~���O��M
 *
 *	@param	const int netID	�l�b�gID
 *	@param	int size				�T�C�Y
 *	@param	void* p_data		�f�[�^
 *	@param	p_work					���[�N
 *	@param	p_net_handle		�n���h��
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void NETRECV_StartProcTiming( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle )
{	
	MENUNET_WORK *p_wk	= p_work;

	if( p_net_handle != GFL_NET_HANDLE_GetCurrentHandle() )
	{
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	if( netID == GFL_NET_SystemGetCurrentID() )
	{
		return;	//�����̃f�[�^�͖���
	}

	NAGI_Printf("PROC�J�n�^�C�~���O����\n" );
	p_wk->is_start_proc		= TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�e�[�^�X���M
 *
 *	@param	const int netID	�l�b�gID
 *	@param	int size				�T�C�Y
 *	@param	void* cp_data		�f�[�^
 *	@param	p_work					���[�N
 *	@param	p_net_handle		�n���h��
 *
 */
//-----------------------------------------------------------------------------
static void NETRECV_SendStatus( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle )
{	
	MENUNET_WORK *p_wk	= p_work;

	if( p_net_handle != GFL_NET_HANDLE_GetCurrentHandle() )
	{
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	if( netID == GFL_NET_SystemGetCurrentID() )
	{
		return;	//�����̃f�[�^�͖���
	}

	NAGI_Printf("PROC�J�n�^�C�~���O����\n" );
	p_wk->is_status_recv		= TRUE;

}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�e�[�^�X�p�o�b�t�@�擾
 *
 *	@param	int netID	�l�b�gID
 *	@param	p_work		���[�N
 *	@param	size			�T�C�Y
 *
 *	@return	�o�b�t�@
 */
//-----------------------------------------------------------------------------
static u8*NETRECV_GetStatusBuffer( int netID, void* p_work, int size )
{	
	MENUNET_WORK *p_wk	= p_work;

	if( netID == GFL_NET_SystemGetCurrentID() )
	{
		//�����̃f�[�^
		return (u8*)p_wk->p_my_status;
	}
	else
	{	
		//�����Ẵf�[�^
		return (u8*)p_wk->p_you_status;
	}
}
