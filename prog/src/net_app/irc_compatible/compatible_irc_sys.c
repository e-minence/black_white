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
#include "net\network_define.h"

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
///	���C���V�[�P���X
//=====================================
typedef enum {
	COMPATIBLE_IRC_SEQ_WAIT	= 0,			
	COMPATIBLE_IRC_SEQ_INIT_START	= 0,
	COMPATIBLE_IRC_SEQ_INIT_WAIT,
	COMPATIBLE_IRC_SEQ_INIT_END,
	COMPATIBLE_IRC_SEQ_CONNECT_START	= 0,
	COMPATIBLE_IRC_SEQ_CONNECT_WAIT,
	COMPATIBLE_IRC_SEQ_CONNECT_TIMING_START,
	COMPATIBLE_IRC_SEQ_CONNECT_TIMING_WAIT,
	COMPATIBLE_IRC_SEQ_CONNECT_END,
	COMPATIBLE_IRC_SEQ_DISCONNECT_START	= 0,
	COMPATIBLE_IRC_SEQ_DISCONNECT_WAIT,
	COMPATIBLE_IRC_SEQ_DISCONNECT_END,
	COMPATIBLE_IRC_SEQ_EXIT_START	= 0,
	COMPATIBLE_IRC_SEQ_EXIT_WAIT,
	COMPATIBLE_IRC_SEQ_EXIT_END,
	COMPATIBLE_IRC_SEQ_TIMING_START	= 0,
	COMPATIBLE_IRC_SEQ_TIMING_WAIT,
	COMPATIBLE_IRC_SEQ_TIMING_END,
	COMPATIBLE_IRC_SEQ_MAIN	= 0,
} COMPATIBLE_IRC_SEQ;

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
///	�V�X�e�����[�N�G���A
//=====================================
struct _COMPATIBLE_IRC_SYS
{	
	u32		irc_timeout;
	u32		seq;
	u32		err_seq;
	u32		connect_bit;
	BOOL	is_connect;
	BOOL	is_exit;
	BOOL	is_recv;
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
static void NET_RECV_DecideMenu( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle );
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
	SENDCMD_DECIDEMENU	= 0,
};
static const NetRecvFuncTable sc_net_recv_tbl[]	=
{	
	{	
		NET_RECV_DecideMenu, NULL
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
	switch( p_sys->seq )
	{
	case COMPATIBLE_IRC_SEQ_INIT_START:
		{
			GFLNetInitializeStruct net_init = sc_net_init;
			net_init.irc_timeout = p_sys->irc_timeout;
			GFL_NET_Init( &net_init, NET_INIT_InitCallBack, p_sys );
		}
		p_sys->seq	= COMPATIBLE_IRC_SEQ_INIT_WAIT;
		break;

	case COMPATIBLE_IRC_SEQ_INIT_WAIT:
		if(GFL_NET_IsInit())
		{
			p_sys->seq	= COMPATIBLE_IRC_SEQ_INIT_END;
		}
		break;

	case COMPATIBLE_IRC_SEQ_INIT_END:
		p_sys->seq	= COMPATIBLE_IRC_SEQ_WAIT;
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
	switch(p_sys->seq){
	case COMPATIBLE_IRC_SEQ_EXIT_START:
		if( GFL_NET_Exit(NET_EXIT_ExitCallBack ) )
		{
			p_sys->seq	= COMPATIBLE_IRC_SEQ_EXIT_WAIT;
		}
		else
		{
			return TRUE;
		}
		break;

	case COMPATIBLE_IRC_SEQ_EXIT_WAIT:
		if( p_sys->is_exit )
		{
			p_sys->seq	= COMPATIBLE_IRC_SEQ_EXIT_END;
		}
		break;

	case COMPATIBLE_IRC_SEQ_EXIT_END:
		p_sys->seq	= COMPATIBLE_IRC_SEQ_WAIT;
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
	switch( p_sys->seq ){
	case COMPATIBLE_IRC_SEQ_CONNECT_START:
		GFL_NET_ChangeoverConnect( NULL );
		p_sys->seq	= COMPATIBLE_IRC_SEQ_CONNECT_WAIT;
		break;

	case COMPATIBLE_IRC_SEQ_CONNECT_WAIT:
		//�����ڑ��҂�
		if( p_sys->is_connect )
		{
			IRC_Print("�ڑ�����\n");
			p_sys->seq	= COMPATIBLE_IRC_SEQ_CONNECT_TIMING_START;
		}
		break;

	case COMPATIBLE_IRC_SEQ_CONNECT_TIMING_START:
		GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,COMPATIBLE_IRC_CONNECT_TIMINGSYNC_NO);
		p_sys->seq	= COMPATIBLE_IRC_SEQ_CONNECT_TIMING_WAIT;
		break;

	case COMPATIBLE_IRC_SEQ_CONNECT_TIMING_WAIT:
		if( GFL_NET_HANDLE_IsTimingSync( GFL_NET_HANDLE_GetCurrentHandle(),
					COMPATIBLE_IRC_CONNECT_TIMINGSYNC_NO) )
		{
			p_sys->seq	= COMPATIBLE_IRC_SEQ_CONNECT_END;
		}
		break;

	case COMPATIBLE_IRC_SEQ_CONNECT_END:
		IRC_Print("�^�C�~���O��萬��\n");
		IRC_Print("�ڑ��l�� = %d\n", GFL_NET_GetConnectNum());
		p_sys->seq = COMPATIBLE_IRC_SEQ_WAIT;
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
	switch(p_sys->seq){
	case COMPATIBLE_IRC_SEQ_DISCONNECT_START:
		if(GFL_NET_IsParentMachine() == TRUE)
		{
			IRC_Print("�e�@�FGFL_NET_CMD_EXIT_REQ���M\n");
			if( GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_CMD_EXIT_REQ, 0, NULL) )
			{
				p_sys->seq	= COMPATIBLE_IRC_SEQ_DISCONNECT_WAIT;
			}
		}
		else
		{
			p_sys->seq	= COMPATIBLE_IRC_SEQ_DISCONNECT_WAIT;
		}
//		break;
//	fall through

	case COMPATIBLE_IRC_SEQ_DISCONNECT_WAIT:
		if( !p_sys->is_connect ){
			p_sys->seq = COMPATIBLE_IRC_SEQ_DISCONNECT_END;
		}
		break;

	case COMPATIBLE_IRC_SEQ_DISCONNECT_END:
		p_sys->seq = COMPATIBLE_IRC_SEQ_WAIT;
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
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@ratval	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
BOOL COMPATIBLE_IRC_TimingSyncWait( COMPATIBLE_IRC_SYS *p_sys )
{
	switch( p_sys->seq )
	{	
	case COMPATIBLE_IRC_SEQ_TIMING_START:
		GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,COMPATIBLE_IRC_TIMINGSYNC_NO);
		p_sys->seq	= COMPATIBLE_IRC_SEQ_TIMING_WAIT;
		break;

	case COMPATIBLE_IRC_SEQ_TIMING_WAIT:
		if( GFL_NET_HANDLE_IsTimingSync( GFL_NET_HANDLE_GetCurrentHandle(),
					COMPATIBLE_IRC_TIMINGSYNC_NO) )
		{
			p_sys->seq	= COMPATIBLE_IRC_SEQ_TIMING_END;
		}
		break;

	case COMPATIBLE_IRC_SEQ_TIMING_END:
		IRC_Print("�^�C�~���O��萬��\n");
		p_sys->seq = COMPATIBLE_IRC_SEQ_WAIT;
		return TRUE;

	default:
		GF_ASSERT_MSG( 0, "NET SEQ ERROR %d\n", p_sys->seq  );
	}

	return FALSE;
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
	switch(p_sys->err_seq){
	case 0:
		if(GFL_NET_IsInit() == TRUE && GFL_NET_SystemIsError() != 0)
		{
			IRC_Print("�ʐM�G���[������\n");
			if(GFL_NET_Exit(NET_EXIT_ExitCallBack) == TRUE)
			{
				p_sys->err_seq++;
			}
			else
			{
				p_sys->err_seq = 100;
			}
			return TRUE;
		}
		break;

	case 1:
		if(p_sys->is_exit == TRUE)
		{
			p_sys->err_seq = 100;
		}
		return TRUE;

	case 100:
		GFL_STD_MemClear(p_sys, sizeof(COMPATIBLE_IRC_SYS));
		return FALSE;
	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���j���[����҂�����
 *
 *	@param	COMPATIBLE_IRC_SYS *p_sys		���[�N
 *
 *	@retval	TRUE�Ȃ�ΏI��
 *	@ratval	FALSE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
BOOL COMPATIBLE_IRC_SendMenuData( COMPATIBLE_IRC_SYS *p_sys, u32 menu_id )
{	
	switch( p_sys->seq )
	{
	case 0:
		if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), 
				SENDCMD_DECIDEMENU, sizeof(u32), &menu_id ))
		{
			OS_TPrintf("�ŏ��̃f�[�^���M����\n");
			p_sys->seq++;
		}
		break;
	case 1:	//����̃f�[�^�������Ă���̂�҂�
		if( p_sys->is_recv == TRUE )
		{
			OS_TPrintf("�ŏ��̃f�[�^��M����\n");
			p_sys->is_recv = FALSE;
			p_sys->seq = 0;
			return TRUE;
		}
		break;
	}
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���j���[���擾
 *
 *	@param	const COMPATIBLE_IRC_SYS *cp_sys ���[�N
 *
 *	@return	���j���[ID
 */
//-----------------------------------------------------------------------------
u32 COMPATIBLE_IRC_GetMenuData( const COMPATIBLE_IRC_SYS *cp_sys )
{	
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
 *	@brief	���j���[����
 *
 *	@param	const int netID	�l�b�gID
 *	@param	int size				�T�C�Y
 *	@param	void* cp_data		�f�[�^
 *	@param	p_work					�p�����[�^
 *	@param	p_net_handle		�l�b�g�n���h��
 *
 */
//-----------------------------------------------------------------------------
static void NET_RECV_DecideMenu( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle )
{	
	COMPATIBLE_IRC_SYS *p_sys	= p_work;

	if( p_net_handle != GFL_NET_HANDLE_GetCurrentHandle() )
	{
		return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
	}
	if( netID == GFL_NET_SystemGetCurrentID() )
	{
		return;	//�����̃f�[�^�͖���
	}

	p_sys->is_recv	= TRUE;
	IRC_Print( "���j���[����" );
}
