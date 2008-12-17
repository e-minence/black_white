//=============================================================================================
/**
 * @file	btl_net.h
 * @brief	�|�P����WB �o�g��  �ʐM����
 * @author	taya
 *
 * @date	2008.10.10	�쐬
 */
//=============================================================================================
#ifndef __BTL_NET_H__
#define __BTL_NET_H__

#include "poke_tool/pokeparty.h"

enum {
	BTL_NET_SERVER_VERSION = 100,
	BTL_NET_CONNECT_MACHINE_MAX = 4,	///< �����ڑ�����}�V���䐔�ő�
};

typedef enum {

	BTL_NET_TIMING_CLIENTID_DETERMINE,

}BtlNetTiming;


typedef struct {

	u8  serverVersion;		///< �T�[�o�o�[�W����
	u8  commPos;			///< �ΐ핔�������ʒu�i�}���`���ŗ��p�j
	u8  netID      : 4;		///< �l�b�gID
	u8  pokeCount  : 4;		///< �Q���|�P������
	u8  pokeData[1];		///< �|�P�����f�[�^��

}BTL_NET_INITIALIZE_DATA;

typedef struct {

	struct {
		u8 netID;
		u8 clientID;
	}PairID[ BTL_CLIENT_MAX ];

}BTL_NET_INITIALIZE_RET_DATA;


typedef enum {

	BTL_NETCMD_SUPPLY_POKEDATA = 0,

}BtlNetCommand;



extern void BTL_NET_InitSystem( GFL_NETHANDLE* netHandle, HEAPID heapID );
extern void BTL_NET_QuitSystem( void );
extern BOOL BTL_NET_IsServerDetermained( void );
extern BOOL BTL_NET_IsServer( void );
extern void BTL_NET_NotifyClientID( NetID netID, const u8* clientID, u8 numClients );
extern BOOL BTL_NET_IsClientIdDetermined( void );
extern u8 BTL_NET_GetMyClientID( u8 idx );
extern void BTL_NET_StartNotifyPartyData( const POKEPARTY* party );
extern BOOL BTL_NET_IsCompleteNotifyPartyData( void );
extern const POKEPARTY* BTL_NET_GetPartyData( int netID );
extern void BTL_NET_EndNotifyPartyData( void );

extern void BTL_NET_TimingSyncStart( u8 timingID );
extern BOOL BTL_NET_IsTimingSync( u8 timingID );

extern void BTL_NET_SendToClient( const void* adrs, u32 size );
extern BOOL BTL_NET_CheckSendToClient( void );

extern BOOL BTL_NET_IsServerCmdReceived( void );
extern u32 BTL_NET_GetReceivedCmdData( const void** ppDest );

extern void BTL_NET_ReturnToServer( const void* data, u32 size );
extern BOOL BTL_NET_CheckReturnFromClient( void );
extern void BTL_NET_ClearRecvData( void );



extern void BTL_NET_StartCommand( BtlNetCommand cmd );
extern BOOL BTL_NET_WaitCommand( void );


#endif
