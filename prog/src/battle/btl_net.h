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



extern void BTL_NET_InitSystem( GFL_NETHANDLE* netHandle, HEAPID heapID );
extern void BTL_NET_QuitSystem( void );



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





extern void BTL_NET_StartCommand( BtlNetCommand cmd );
extern BOOL BTL_NET_WaitCommand( void );


#endif
