//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		event_debug_beacon.c
 *	@brief	�f�o�b�O	�r�[�R������Ă̗F�B�R�[�h�o�^
 *	@author	Toru=Nagihashi
 *	@data		2009.11.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>
#include "system/main.h"      //GFL_HEAPID_APP�Q��

//�t�B�[���h
#include "field/field_msgbg.h"
#include "field/fieldmap.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

//�v�����g
#include "print/wordset.h"

//�ʐM
#include "net/network_define.h"
#include "../net/dwc_rapfriend.h"	//GFL_NET_DWC_CheckFriendCodeByToken�̂���

//�A�[�J�C�u
#include "arc_def.h"
#include "message.naix"
#include "msg/msg_debug_beacon_friendcode.h"

//�O�����J
#include "event_debug_beacon.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define BEACON_SEARCH_MAX				(3)
#define BEACON_SEARCH_INTERVAL	(30)

//-------------------------------------
///	�r�[�R���o�^����
//=====================================
typedef enum 
{
	BEACON_REGISTER_RES_SUCCESS,				//����
	BEACON_REGISTER_RES_WIFINOTE_FULL,	//���C�t�@�C�m�[�g�������ς�
	BEACON_REGISTER_RES_SETING_ALREADY,	//���łɐݒ�ς�
	BEACON_REGISTER_RES_FRIENDKEY_NG,		//�F�B�R�[�h���s��
	
} BEACON_REGISTER_RESULT;


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
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
///	���ɐڑ������r�[�R�����
//=====================================
typedef struct 
{
	u8					macaddr[6];
	u8					dummy[2];
	BEACON_DATA	beacon;
} BEACON_RECV_DATA;

//-------------------------------------
///	�ڑ������r�[�R�����Z�b�g
//=====================================
typedef struct 
{
	BEACON_RECV_DATA	recv_beacon[BEACON_SEARCH_MAX];	//�󂯎�����r�[�R��
	u32								recv_num;			//�󂯎������
} BEACON_RECV_SET;

//-------------------------------------
///	�C�x���g���[�N
//=====================================
typedef struct 
{
	BEACON_DATA				my_beacon;			//�����̃r�[�R��
	BEACON_RECV_SET		recv_set;					//�󂯎�����r�[�R��

	FLDMENUFUNC						*p_menu;
	FLDMENUFUNC_LISTDATA	*p_menu_data;

  GAMEDATA          *p_gamedata;
	GAMESYS_WORK			*p_gsys;
	FIELDMAP_WORK			*p_field;
	FLDMSGBG					*p_msg_bg;
	GFL_MSGDATA				*p_msg_data;
	FLDMSGWIN					*p_msg_win;
	HEAPID						heapID;

	u32								cnt;
	u32								check_idx;
	BOOL							is_send;
	STRBUF						*p_strbuf;
	WORDSET						*p_word;
} DEBUG_BEACON_WORK;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�C�x���g
//=====================================
static GMEVENT_RESULT EVENT_DebugBeaconMain( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	�r�[�R���f�[�^
//=====================================
static void BEACON_DATA_Init( BEACON_DATA *p_wk, GAMEDATA *p_gamedata );
static BEACON_REGISTER_RESULT BEACON_DATA_Register( const BEACON_DATA *cp_data, GAMEDATA *p_gamedata, HEAPID heapID );
static void BEACON_DATA_SetWord( const BEACON_DATA *cp_wk, WORDSET *p_word, STRBUF *p_str, HEAPID heapID );

//-------------------------------------
///	�ʐM�R�[���o�b�N
//=====================================
static void * NETCALLBACK_GetBeaconData( void *p_wk_adrs );
static int NETCALLBACK_GetBeaconSize( void *p_wk_adrs );
static BOOL NETCALLBACK_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 );

//-------------------------------------
///	���̑�
//=====================================
static void NET_RecvBeacon( BEACON_RECV_SET *p_wk );

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	�l�b�g���[�N
//=====================================
//�������\����
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
	4,//_MAXNUM,  //�ő�ڑ��l��
	48,//_MAXSIZE,  //�ő呗�M�o�C�g��
	BEACON_SEARCH_MAX,//_BCON_GET_NUM,  // �ő�r�[�R�����W��
	TRUE,   // CRC�v�Z
	FALSE,    // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
	GFL_NET_TYPE_WIRELESS,    //�ʐM�^�C�v�̎w��
	TRUE,   // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
	WB_NET_DEBUG_BEACON_FRIENDCODE, //GameServiceID
#if GFL_NET_IRC
	IRC_TIMEOUT_STANDARD, // �ԊO���^�C���A�E�g����
#endif
	0,//MP�e�ő�T�C�Y 512�܂�
	0,//dummy
};

//--------------------------------------------------------------
//  �I�����j���[�w�b�_�[
//--------------------------------------------------------------
static const FLDMENUFUNC_HEADER DATA_MenuHeader =
{
	1,		//���X�g���ڐ�
	10,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
//	0,		//�\���x���W
	3,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	1,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_NON,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	12,		//�����T�C�YY(�h�b�g
	0,		//�\�����WX �L�����P��
	0,		//�\�����WY �L�����P��
	0,		//�\���T�C�YX �L�����P��
	0,		//�\���T�C�YY �L�����P��
};

//----------------------------------------------------------------------------
/**
 *	@brief	�r�[�R���X�L�����v���Z�X
 *
 *	@param	GAMESYS_WORK *p_gsys	�Q�[���V�X�e��
 *	@param	*p_field							�t�B�[���h�}�b�v
 *	@param	p_msg_bg							���b�Z�[�WBG
 *	@param	*p_event							�e�C�x���g
 *	@param	heapID								�q�[�vID
 */
//-----------------------------------------------------------------------------
void EVENT_DebugBeacon( GAMESYS_WORK *p_gsys, FIELDMAP_WORK *p_field, FLDMSGBG *p_msg_bg, GMEVENT *p_event, HEAPID heapID )
{	
	DEBUG_BEACON_WORK	*p_wk;

	GMEVENT_Change( p_event, EVENT_DebugBeaconMain, sizeof(DEBUG_BEACON_WORK) );
  p_wk = GMEVENT_GetEventWork(p_event);

	GFL_STD_MemClear( p_wk, sizeof(DEBUG_BEACON_WORK) );
	p_wk->p_gamedata	= GAMESYSTEM_GetGameData( p_gsys );
	p_wk->p_gsys		= p_gsys;
	p_wk->p_field		= p_field;
	p_wk->p_msg_bg	= p_msg_bg;
	p_wk->heapID		= heapID;

	BEACON_DATA_Init( &p_wk->my_beacon, p_wk->p_gamedata );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�r�[�R���X�L�����C�x���g���C��
 *
 *	@param	GMEVENT *p_event	�C�x���g
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_wk_adrs				���[�N�A�h���X
 *
 *	@return	�C�x���g�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_DebugBeaconMain( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{	
	enum
	{	
		SEQ_START_CHECK,

		SEQ_SELECT,
		SEQ_SELECT_WAIT,
		SEQ_INIT,
		SEQ_INIT_WAIT,

		SEQ_BEACON_SEND,
		SEQ_BEACON_SEARCH,
		SEQ_BEACON_CHECK,
		SEQ_END,
		SEQ_END_WAIT,

		//���b�Z�[�W�\��
		SEQ_MSG_PRINT,
		SEQ_MSG_PRINT_WAIT,

		//������G���[
		SEQ_CONNECT_ERROR_PRINT,
		SEQ_CONNECT_ERROR_PRINT_WAIT,

		//�����Ȃ��ł���
		SEQ_CONNECT_ALREADY_PRINT,
		SEQ_CONNECT_ALREADY_PRINT_WAIT,

		SEQ_EXIT,

	};

	DEBUG_BEACON_WORK	*p_wk	= p_wk_adrs;

	switch( *p_seq )
	{	
	case SEQ_START_CHECK:
		//���b�Z�[�W�ʍ쐬
		p_wk->p_msg_data	= FLDMSGBG_CreateMSGDATA( p_wk->p_msg_bg, NARC_message_debug_beacon_friendcode_dat );
		p_wk->p_msg_win		= FLDMSGWIN_AddTalkWin( p_wk->p_msg_bg, p_wk->p_msg_data );
		p_wk->p_strbuf		= GFL_STR_CreateBuffer( 255, p_wk->heapID );
		p_wk->p_word			= WORDSET_Create( p_wk->heapID );

		//�����Ȃ��ł����烁�b�Z�[�W�����ďI��
		if(  GFL_NET_IsInit() )
		{	
			*p_seq	= SEQ_CONNECT_ALREADY_PRINT;
		}
		else
		{	
			FLDMSGWIN_ClearWindow( p_wk->p_msg_win );
			FLDMSGWIN_Print( p_wk->p_msg_win, 0, 0, DEBUG_BEACON_STR_00 );
			*p_seq	= SEQ_SELECT;
		}
		break;

	
	case SEQ_SELECT:
		if( FLDMSGWIN_CheckPrintTrans(p_wk->p_msg_win) )
		{
			int i;
			FLDMENUFUNC_HEADER menuH	= DATA_MenuHeader;
			const FLDMENUFUNC_LIST menuList[2] = {
				{ DEBUG_BEACON_STR_06, (void*)0 }, { DEBUG_BEACON_STR_07, (void*)1 }, 
			};

			p_wk->p_menu_data	= FLDMENUFUNC_CreateMakeListData(
            menuList, NELEMS(menuList), p_wk->p_msg_data, p_wk->heapID );
			FLDMENUFUNC_InputHeaderListSize( &menuH, NELEMS(menuList), 24, 10, 7, 4 );
			p_wk->p_menu	= FLDMENUFUNC_AddMenu( p_wk->p_msg_bg, &menuH, p_wk->p_menu_data );
			*p_seq	= SEQ_SELECT_WAIT;
		}
		break;

	case SEQ_SELECT_WAIT:
		{
			u32 ret;
			ret	= FLDMENUFUNC_ProcMenu( p_wk->p_menu );
			if( ret == FLDMENUFUNC_CANCEL )
			{	
				FLDMENUFUNC_DeleteMenu( p_wk->p_menu );
				*p_seq	= SEQ_EXIT;
			}
			else if( ret != FLDMENUFUNC_NULL )
			{	
				FLDMENUFUNC_DeleteMenu( p_wk->p_menu );
				FLDMSGWIN_ClearWindow( p_wk->p_msg_win );
				if( ret == 0 )
				{	
					FLDMSGWIN_Print( p_wk->p_msg_win, 0, 0, DEBUG_BEACON_STR_09 );
					p_wk->is_send	= FALSE;
				}
				else
				{	
					p_wk->is_send	= TRUE;
					FLDMSGWIN_Print( p_wk->p_msg_win, 0, 0, DEBUG_BEACON_STR_08 );
				}
				*p_seq	= SEQ_INIT;
			}
		}
		break;

	case SEQ_INIT:
		GFL_NET_Init( &sc_net_init, NULL, p_wk );
		*p_seq	= SEQ_INIT_WAIT;
		break;

	case SEQ_INIT_WAIT:
		if( GFL_NET_IsInit() )
		{	
			if( p_wk->is_send )
			{	
				GFL_NET_InitServer();
				*p_seq	= SEQ_BEACON_SEND;
			}
			else
			{	
				GFL_NET_StartBeaconScan();
				*p_seq	= SEQ_BEACON_SEARCH;
			}
		}
		break;


	case SEQ_BEACON_SEND:
		//B�{�^���ŏI��
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
		{	
			*p_seq	= SEQ_END;
		}
		break;
		
	case SEQ_BEACON_SEARCH:
		//BEACON_SEARCH_INTERVAL��1��T�[�`����
		if( p_wk->cnt++ > BEACON_SEARCH_INTERVAL )
		{	
			BEACON_RECV_SET	temp;
			//�r�[�R���T�[�`
			NET_RecvBeacon( &temp );
			if( GFL_STD_MemComp( &temp, &p_wk->recv_set, sizeof(BEACON_RECV_SET) ) != 0
					&& temp.recv_num > 0 )
			{	
				p_wk->recv_set		= temp;
				p_wk->check_idx		= 0;
				*p_seq	= SEQ_BEACON_CHECK; 
			}

			p_wk->cnt	= 0;
		}
			
		//B�{�^���ŏI��
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
		{	
			*p_seq	= SEQ_END;
		}
		break;
		
	case SEQ_BEACON_CHECK:
		{
			BEACON_REGISTER_RESULT	res;
			BEACON_RECV_DATA	*p_data;
			BOOL	is_msg;

			is_msg	= FALSE;
			for( ; p_wk->check_idx < p_wk->recv_set.recv_num; p_wk->check_idx++ )
			{	
				p_data	= &p_wk->recv_set.recv_beacon[ p_wk->check_idx ];
				res	= BEACON_DATA_Register( &p_data->beacon, p_wk->p_gamedata, p_wk->heapID );

				switch( res )
				{	
				case BEACON_REGISTER_RES_SUCCESS:				//����
					GFL_MSG_GetString( p_wk->p_msg_data, DEBUG_BEACON_STR_01, p_wk->p_strbuf );
					is_msg	= TRUE;
					break;
				case BEACON_REGISTER_RES_WIFINOTE_FULL:	//���C�t�@�C�m�[�g�������ς�
					GFL_MSG_GetString( p_wk->p_msg_data, DEBUG_BEACON_STR_03, p_wk->p_strbuf );
					is_msg	= TRUE;
					break;
				case BEACON_REGISTER_RES_FRIENDKEY_NG:		//�F�B�R�[�h���s��
					GFL_MSG_GetString( p_wk->p_msg_data, DEBUG_BEACON_STR_04, p_wk->p_strbuf );
					is_msg	= TRUE;
					break;
				case BEACON_REGISTER_RES_SETING_ALREADY:	//���łɐݒ�ς�
					GFL_MSG_GetString( p_wk->p_msg_data, DEBUG_BEACON_STR_10, p_wk->p_strbuf );
					is_msg	= TRUE;	
					break;
				}
				
				//���b�Z�[�W�Ȃ�΁A���[�v�r���ŏI��
				if( is_msg )
				{
					BEACON_DATA_SetWord( &p_data->beacon, p_wk->p_word, p_wk->p_strbuf, p_wk->heapID );
					break;
				}

			}
			if( !is_msg )
			{	
				FLDMSGWIN_ClearWindow( p_wk->p_msg_win );
				FLDMSGWIN_Print( p_wk->p_msg_win, 0, 0, DEBUG_BEACON_STR_09 );

				*p_seq	= SEQ_BEACON_SEARCH;
			}
			else
			{	
				*p_seq	= SEQ_MSG_PRINT;
			}
		}
		break;

	case SEQ_MSG_PRINT :
		FLDMSGWIN_ClearWindow( p_wk->p_msg_win );
		FLDMSGWIN_PrintStrBuf( p_wk->p_msg_win, 0, 0, p_wk->p_strbuf );
		*p_seq	= SEQ_MSG_PRINT_WAIT;
		break;

	case SEQ_MSG_PRINT_WAIT:
		if( FLDMSGWIN_CheckPrintTrans(p_wk->p_msg_win) )
		{
			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B 
				|| GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
			{	
				p_wk->check_idx++;	//���̂̓v�����g���������̂Ŏ��̂�����ׂ�
				*p_seq	= SEQ_BEACON_CHECK;
			}
		}
		break;

	case SEQ_END:
		GFL_NET_Exit(NULL);
		*p_seq	= SEQ_END_WAIT;
		break;

	case SEQ_END_WAIT:
		if( GFL_NET_IsExit() )
		{
			*p_seq	= SEQ_EXIT;
		}
		break;

	case SEQ_CONNECT_ALREADY_PRINT:
		FLDMSGWIN_ClearWindow( p_wk->p_msg_win );
		FLDMSGWIN_Print( p_wk->p_msg_win, 0, 0, DEBUG_BEACON_STR_02 );
		*p_seq	= SEQ_CONNECT_ALREADY_PRINT_WAIT;
		break;

	case SEQ_CONNECT_ALREADY_PRINT_WAIT:
		if( FLDMSGWIN_CheckPrintTrans(p_wk->p_msg_win) )
		{
			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B 
				|| GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
			{	
				*p_seq	= SEQ_EXIT;
			}
		}
		break;

	case SEQ_CONNECT_ERROR_PRINT:
		FLDMSGWIN_ClearWindow( p_wk->p_msg_win );
		FLDMSGWIN_Print( p_wk->p_msg_win, 0, 0, DEBUG_BEACON_STR_05 );
		*p_seq	= SEQ_CONNECT_ERROR_PRINT_WAIT;
		break;

	case SEQ_CONNECT_ERROR_PRINT_WAIT:
		if( FLDMSGWIN_CheckPrintTrans(p_wk->p_msg_win) )
		{
			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B 
				|| GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
			{	
				*p_seq	= SEQ_EXIT;
			}
		}
		break;

	case SEQ_EXIT:
		if( p_wk->p_word )
		{	
			WORDSET_Delete( p_wk->p_word );
		}
		if( p_wk->p_strbuf )
		{	
			GFL_STR_DeleteBuffer( p_wk->p_strbuf );
		}
		if( p_wk->p_msg_data )
		{	
			GFL_MSG_Delete( p_wk->p_msg_data );
		}
		if( p_wk->p_msg_win )
		{	
			FLDMSGWIN_Delete( p_wk->p_msg_win );
		}
		return GMEVENT_RES_FINISH;
	}

#if 0
	//�G���[�`�F�b�N
	if( GFL_NET_SystemIsError() &&
			SEQ_BEACON_SEARCH <= *p_seq && *p_seq < SEQ_CONNECT_ERROR_PRINT )
	{	
		*p_seq	= SEQ_CONNECT_ERROR_PRINT;
	}
#endif

	return GMEVENT_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�r�[�R���̏���ݒ�
 *
 *	@param	BEACON_DATA *p_wk	���[�N
 *	@param	*p_sv							�Z�[�u�f�[�^
 */
//-----------------------------------------------------------------------------
static void BEACON_DATA_Init( BEACON_DATA *p_wk, GAMEDATA *p_gamedata )
{	
	GFL_STD_MemClear( p_wk, sizeof(BEACON_DATA) );

	p_wk->gsID	= WB_NET_DEBUG_BEACON_FRIENDCODE;

	//�����̖��O�擾
	{
		MYSTATUS * p_status;
		p_status	=  GAMEDATA_GetMyStatus( p_gamedata );
		MyStatus_CopyNameStrCode( p_status, p_wk->playername, PERSON_NAME_SIZE + EOM_SIZE );
		p_wk->sex	= MyStatus_GetMySex( p_status );
	}

	//�����̗F�B�R�[�h�擾
	{	
		WIFI_LIST* p_list;
		DWCUserData*	p_userdata;

		p_list						= GAMEDATA_GetWiFiList( p_gamedata );
		p_userdata				= WifiList_GetMyUserInfo( p_list );
		p_wk->friendcode	= DWC_CreateFriendKey( p_userdata );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	�r�[�R���̏���o�^
 *
 *	@param	const BEACON_DATA *cp_data	���[�N
 *	@param	*p_sv												�Z�[�u�f�[�^
 *	@param	heapID											�e���|�����o�b�t�@�p�q�[�vID
 *
 *	@retval	�o�^����
 *
 */
//-----------------------------------------------------------------------------
static BEACON_REGISTER_RESULT BEACON_DATA_Register( const BEACON_DATA *cp_data, GAMEDATA *p_gamedata, HEAPID heapID )
{	
	WIFI_LIST* p_list;
	DWCUserData*	p_userdata;

	//�����̏��擾
	p_list						= GAMEDATA_GetWiFiList( p_gamedata );
	p_userdata				= WifiList_GetMyUserInfo( p_list );

	{	
		int i;
		for( i = 0; i < WIFILIST_FRIEND_MAX; i++ )
		{	
			if( WifiList_IsFriendData( p_list, i ) == FALSE )
			{
				//�R�[�h�s���`�F�b�N
				if( !DWC_CheckFriendKey( p_userdata, cp_data->friendcode ))
        {
          // �F�B�R�[�h���Ⴄ
          return BEACON_REGISTER_RES_FRIENDKEY_NG;
				}

				//�ݒ�ς݃`�F�b�N
				{	
					int ret;
					int pos;
					
					ret	= GFL_NET_DWC_CheckFriendCodeByToken( p_list, cp_data->friendcode, &pos );
					if(ret == DWCFRIEND_INLIST)
					{
						// �����ݒ肸��
						return BEACON_REGISTER_RES_SETING_ALREADY;
					}
				}

				// �����̂��߃f�[�^��o�^
				{	
					DWCFriendData *p_dwc_list;
					STRBUF	*p_name;

					p_name	= GFL_STR_CreateBuffer( PERSON_NAME_SIZE + EOM_SIZE, heapID );
					GFL_STR_SetStringCodeOrderLength( p_name , cp_data->playername, PERSON_NAME_SIZE + EOM_SIZE );

					//���O�A���ʁA�F�B�R�[�h��ݒ�
					p_dwc_list = WifiList_GetDwcDataPtr(p_list, i );
					DWC_CreateFriendKeyToken( p_dwc_list, cp_data->friendcode );
					WifiList_SetFriendName( p_list, i, p_name );
					WifiList_SetFriendInfo( p_list, i, WIFILIST_FRIEND_SEX, cp_data->sex );

					GFL_STR_DeleteBuffer( p_name );

					//����
					return BEACON_REGISTER_RES_SUCCESS;
				}
			}
		}
	}

	//�F�B�R�[�h����t
	return BEACON_REGISTER_RES_WIFINOTE_FULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�r�[�R���̃f�[�^��P��Z�b�g
 *
 *	@param	const BEACON_DATA *cp_wk	���[�N
 *	@param	*p_word										�P��o�^�Ǘ�
 *	@param	*p_str										�o�^����o�b�t�@
 *	@param	heapID										�e���|�����p�q�[�vID
 */
//-----------------------------------------------------------------------------
static void BEACON_DATA_SetWord( const BEACON_DATA *cp_wk, WORDSET *p_word, STRBUF *p_str, HEAPID heapID )
{	
	STRBUF	*p_name;
	STRBUF	*p_src_str;
	p_name	= GFL_STR_CreateBuffer( PERSON_NAME_SIZE + EOM_SIZE, heapID );
	GFL_STR_SetStringCodeOrderLength( p_name , cp_wk->playername, PERSON_NAME_SIZE + EOM_SIZE );

	p_src_str	= GFL_STR_CreateCopyBuffer( p_str, heapID );

	WORDSET_RegisterWord( p_word, 0, p_name, 0, 0, 0 );
	WORDSET_ExpandStr( p_word, p_str, p_src_str );

	GFL_STR_DeleteBuffer( p_src_str );
	GFL_STR_DeleteBuffer( p_name );

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
	DEBUG_BEACON_WORK	*p_wk	= p_wk_adrs;

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
	//�r�[�R���̎󂯎�肾���ŁA�q����Ȃ�
	return GameServiceID1 == GameServiceID2;
}
//=============================================================================
/**
 *			���̑�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�r�[�R����M
 *
 *	@param	BEACON_RECV_SET *p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void NET_RecvBeacon( BEACON_RECV_SET *p_wk )
{	
	int i;
	BEACON_RECV_DATA	data;
	u8					*p_macaddr;
	BEACON_DATA	*p_beacon;
	
	GFL_STD_MemClear( p_wk, sizeof(BEACON_RECV_SET) );

	for( i = 0; i < BEACON_SEARCH_MAX; i++ )
	{	
		p_beacon	= GFL_NET_GetBeaconData( i );
		p_macaddr	= GFL_NET_GetBeaconMacAddress( i );

		NAGI_Printf( "beacon 0x%x 0x%x %d\n", p_beacon, p_macaddr, p_wk->recv_num );
		if( p_beacon && p_macaddr && p_wk->recv_num < BEACON_SEARCH_MAX )
		{
			GFL_STD_MemCopy( p_beacon, &data.beacon, sizeof(BEACON_DATA) );
			GFL_STD_MemCopy( p_macaddr, data.macaddr, sizeof(u8)*6 );

			p_wk->recv_beacon[ p_wk->recv_num ]	= data;
			p_wk->recv_num++;
		}
	}
}
