//=============================================================================
/**
 * @file	comm_command_balloon.c
 * @brief	データを送るためのコマンドをテーブル化しています
 *          風船ミニゲーム用です
 *          comm_command_balloon.h の enum と同じ並びである必要があります
 * @author	matsuda
 * @date    2007.11.26(月)
 */
//=============================================================================
#include <gflib.h>
#include "system/palanm.h"
#include "print\printsys.h"
#include <arc_tool.h>
#include "print\gf_font.h"
#include "net\network_define.h"
#include "message.naix"
#include "system/wipe.h"
#include <procsys.h>
#include "system/main.h"
#include "print\gf_font.h"
#include "font/font.naix"
#include "font/font.naix"

#include "balloon_common.h"
#include "balloon_comm_types.h"
#include "balloon_game_types.h"
#include "net_app/balloon.h"
#include "balloon_game.h"
#include "balloon_tcb_pri.h"
#include "balloon_sonans.h"
#include "balloon_tool.h"
#include "comm_command_balloon.h"
#include "balloon_comm_types.h"
#include "balloon_send_recv.h"
#include "balloon_id.h"
#include "balloon_control.h"
#include "balloon_snd_def.h"

#include "balloon_gra_def.h"
#include "balloon_particle.naix"
#include "balloon_particle_lst.h"
#include "msg/msg_balloon.h"
#include "wlmngm_tool.naix"		//タッチペングラフィック
#include "system/actor_tool.h"
#include "arc_def.h"
#include "system/sdkdef.h"
#include "system/gfl_use.h"
#include <calctool.h>
#include "system/bmp_winframe.h"
#include "comm_command_balloon.h"
#include "balloon_comm_types.h"
#include "balloon_send_recv.h"



//==============================================================================
//  テーブルに書く関数の定義
//==============================================================================
static	void Recv_CommBalloonPlayData(const int id_no, const int size,const void *pData,void *work, GFL_NETHANDLE *pNetHandle);
static	void Recv_CommGameEnd(const int id_no, const int size,const void *pData,void *work, GFL_NETHANDLE *pNetHandle);
static	void Recv_CommServerVersion(const int id_no,const int size,const void *pData,void *work, GFL_NETHANDLE *pNetHandle);



//==============================================================================
//	テーブル宣言
//  comm_shar.h の enum と同じならびにしてください
//  CALLBACKを呼ばれたくない場合はNULLを書いてください
//  コマンドのサイズを返す関数を書いてもらえると通信が軽くなります
//  _getZeroはサイズなしを返します。_getVariableは可変データ使用時に使います
//==============================================================================
static const NetRecvFuncTable _CommPacketTbl[] = {
    {NULL,                      NULL},	// CB_EXIT_BALLOON
	{Recv_CommServerVersion,	NULL},	// CB_SERVER_VERSION
	{Recv_CommBalloonPlayData, 	NULL},	// CB_PLAY_PARAM
	{Recv_CommGameEnd,		 	NULL},	// CB_GAME_END
};

//--------------------------------------------------------------
/**
 * @brief   コンテスト用のコマンド体系に初期化します
 * @param   none
 * @retval  none
 */
//--------------------------------------------------------------
void CommCommandBalloonInitialize(void* pWork)
{
#if WB_TEMP_FIX
    int length = sizeof(_CommPacketTbl)/sizeof(NetRecvFuncTable);
    CommCommandInitialize(_CommPacketTbl, length, pWork);
#else
	//GGIDを後で入れる
	GFL_NET_COMMAND_Init(0, _CommPacketTbl, NELEMS(_CommPacketTbl), pWork);
#endif

    GF_ASSERT(sizeof(BALLOON_SIO_PLAY_WORK) < 256);	//このサイズを超えたらHugeBuffにする必要がある
}





//==============================================================================
//
//	
//
//==============================================================================

//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ゲームプレイ中データ通信受信処理
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		
 */
//--------------------------------------------------------------
static	void Recv_CommBalloonPlayData(const int id_no, const int size,const void *pData,void *work, GFL_NETHANDLE *pNetHandle)
{
	BALLOON_GAME_PTR game = work;
	
	BalloonSio_RecvBufferSet(game, id_no, pData);
}

//--------------------------------------------------------------
/**
 * @brief   ゲームプレイ中データ送信命令
 * @param   consys		コンテストシステムワークへのポインタ
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL Send_CommBalloonPlayData(BALLOON_GAME_PTR game, BALLOON_SIO_PLAY_WORK *send_data)
{
#if WB_FIX
	if(CommSendData(CB_PLAY_PARAM, send_data, sizeof(BALLOON_SIO_PLAY_WORK)) == TRUE){
#else
	if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CB_PLAY_PARAM, sizeof(BALLOON_SIO_PLAY_WORK), send_data) == TRUE){
#endif
		return TRUE;
	}
	return FALSE;
}


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   ゲーム終了を伝える
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		
 */
//--------------------------------------------------------------
static	void Recv_CommGameEnd(const int id_no, const int size,const void *pData,void *work, GFL_NETHANDLE *pNetHandle)
{
	BALLOON_GAME_PTR game = work;
	
	game->game_end = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ゲーム終了メッセージ送信命令
 * @param   consys		コンテストシステムワークへのポインタ
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL Send_CommGameEnd(BALLOON_GAME_PTR game)
{
#if WB_FIX
	if(CommSendData(CB_GAME_END, NULL, 0) == TRUE){
#else
	if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CB_GAME_END, 0, NULL) == TRUE){
#endif
		return TRUE;
	}
	return FALSE;
}

//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   サーバーバージョンを受信する
 *
 * @param   id_no		送信者のネットID
 * @param   size		受信データサイズ
 * @param   pData		受信データ
 * @param   work		
 */
//--------------------------------------------------------------
static	void Recv_CommServerVersion(const int id_no,const int size,const void *pData,void *work, GFL_NETHANDLE *pNetHandle)
{
	BALLOON_GAME_PTR game = work;
	const u32 *recv_data = pData;
	u32 server_version;
	int i;
	
	server_version = recv_data[0];
	for(i = 0; i < game->bsw->player_max; i++){
		if(game->bsw->player_netid[i] == id_no){
			game->recv_server_version[i] = server_version;
			break;
		}
	}
	GF_ASSERT(i < game->bsw->player_max);
	OS_TPrintf("受信：サーバーバージョン = %d(net_id=%d, 受信サイズ=%d)\n", server_version, id_no, size);
	game->recv_server_version_cnt++;
}

//--------------------------------------------------------------
/**
 * @brief   サーバーバージョン送信命令
 * @param   consys		コンテストシステムワークへのポインタ
 * @retval  TRUE:送信した。　FALSE:送信失敗
 */
//--------------------------------------------------------------
BOOL Send_CommServerVersion(BALLOON_GAME_PTR game)
{
	u32 server_version = BALLOON_SERVER_VERSION;
	
#if WB_FIX
	if(CommSendData(CB_SERVER_VERSION, &server_version, sizeof(u32)) == TRUE){
#else
	if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), CB_SERVER_VERSION, sizeof(u32), &server_version) == TRUE){
#endif
		return TRUE;
	}
	return FALSE;
}
