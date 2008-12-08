//==============================================================================
/**
 * @file	net_irc.c
 * @brief	IRC制御
 * @author	matsuda
 * @date	2008.10.10(金)
 */
//==============================================================================
#include "gflib.h"

#include "ui/ui_def.h"
#include "net_def.h"
#include "net_local.h"
#include "device/net_whpipe.h"
#include "device/dwc_rapcommon.h"
#include "net_system.h"
#include "net_command.h"
#include "net_state.h"
#include "wm_icon.h"
#include "device/dwc_rap.h"


#include "tool/net_ring_buff.h"
#include "tool/net_queue.h"
#include "net_handle.h"
#include "net_irc.h"


//==============================================================================
//	定数定義
//==============================================================================
///切断として判定するまでの時間(この時間内なら切断しても再接続を試みる)
#define NET_IRC_SHOUT_DOWN_TIME		(60 * 5)	//0を指定すれば時間切れ無し

///再接続する際に、再接続リクエスト開始するまでのウェイト
///(相手方の終了、初期化処理などもあるので少し間を空ける)
#define NET_IRC_RETRY_START_WAIT	(5)

///再接続する際に行うIRC_Connectを何フレーム空けて再度リクエストをかけるか
///中でリクエスト中はちゃんとはじいてるっぽいけど一応
#define NET_IRC_CONNECT_REQ_WAIT	(5)

/*----------------------
!!value値の扱い!!

下位4ビットを「何番のデータまで送信しました」という送信制御の値、
上位4ビットを「何番のデータまで受け取りました」という受信制御の値とする。
送信した側は受信側からvalue値でちゃんと「何番のデータを受け取りました」という返事を
もらわない限り次のデータは送信出来ないようにする
※IRC_Sendは手で一瞬隠したりすると切断はしていないのに送信だけ失敗してしまうようなので。
------------------------*/
///送信データ管理value値のレンジ
#define SEND_CHECK_VALUE_RANGE		(0xf)	//4bit内の数値にする事!!


//==============================================================================
//	構造体定義
//==============================================================================
typedef struct{
	IrcRecvFunc recieve_func;
	u8 initialize;		///<TRUE:初期化完了
	u8 send_use;		///<今フレームで既にSendを使用したかどうか(TRUE:使用した)
	u8 send_turn;		///<TRUE:送信可能。　FALSE:送信は通信相手のターン
	u8 send_lock;		///<TRUE:送信ロック中(相手側が巨大データ送信中)
	
	u8 parent_MacAddress[6];	///<親のMacAddress
	u8 isSender;		///<TRUE:最初に接続要求した(再接続時は値は変わりません)
	u8 connect;			///<TRUE:繋がっている(再接続中もTRUE)
	
	s32 retry_time;		///<再接続中のタイムカウンタ
	
	void *send_buf;		///<送信データへのポインタ
	u8 send_size;		///<送信サイズ
	u8 send_command;	///<送信コマンド
	u8 send_value;		///<送信バリュー
	u8 send_action;		///<TRUE:送信実行中(send_buf==NULLではコマンドのみの送信の可能性があるので)
	
	u8 my_value;		///<自分が送信してきたvalue値。何か送信する度にインクリメント
	u8 last_value;		///<最後に受信したvalue値
	u8 retry_send_reserve;	///<再接続した時に送信中のデータがあった場合、送信予約する
	u8 pading;
}NET_IRC_SYS;


//==============================================================================
//	ローカル変数
//==============================================================================
static NET_IRC_SYS NetIrcSys = {0,0};


//==============================================================================
//	プロトタイプ宣言
//==============================================================================
void GFL_NET_IRC_Init(void);
BOOL GFL_NET_IRC_SendTurnGet(void);
BOOL GFL_NET_IRC_SendLockFlagGet(void);
BOOL GFL_NET_IRC_SendCheck(void);
void GFL_NET_IRC_UpdateSendData(void);
static void IRC_ReceiveCallback(u8 *data, u8 size, u8 command, u8 value);
static int IRC_TargetIDGet(void);


//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   none		
 *
 * @retval  
 *
 *
 */
//--------------------------------------------------------------
void GFL_NET_IRC_Init(void)
{
	GFL_STD_MemClear(&NetIrcSys, sizeof(NET_IRC_SYS));
	IRC_Init();
	IRC_SetRecvCallback(IRC_ReceiveCallback);
//	GFL_NET_SystemIrcModeInit();
}

void GFL_NET_IRC_RecieveFuncSet(IrcRecvFunc recieve_func)
{
	NetIrcSys.recieve_func = recieve_func;
}

//--------------------------------------------------------------
/**
 * @brief   初期化完了フラグをセットする
 */
//--------------------------------------------------------------
void GFL_NET_IRC_InitializeFlagSet(void)
{
	GF_ASSERT(NetIrcSys.initialize == FALSE);
	NetIrcSys.initialize = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   初期化完了フラグを取得する
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_InitializeFlagGet(void)
{
	return NetIrcSys.initialize;
}

//--------------------------------------------------------------
/**
 * @brief   送信可能ターンフラグを取得
 *
 * @retval  TRUE:送信可能。　FALSE:不可能
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_SendTurnGet(void)
{
	return NetIrcSys.send_turn;
}

//--------------------------------------------------------------
/**
 * @brief   送信ロックフラグを取得
 *
 * @retval  TRUE:送信ロックがかかっている
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_SendLockFlagGet(void)
{
	return NetIrcSys.send_lock;
}

//--------------------------------------------------------------
/**
 * @brief   送信可能か調べる
 *
 * @retval  TRUE:送信可能。　FALSE:不可能
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_SendCheck(void)
{
	if(NetIrcSys.send_turn == TRUE && NetIrcSys.send_lock == FALSE 
			&& NetIrcSys.retry_time == 0 && NetIrcSys.send_action == 0){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   バッファ送信
 *
 * @param   buf			
 * @param   size		
 * @param   command		
 * @param   value		
 */
//--------------------------------------------------------------
void GFL_NET_IRC_Send(u8 *buf, u8 size, u8 command)
{
	u8 value;
	
	GF_ASSERT(NetIrcSys.send_turn == TRUE);
	
#if GFL_IRC_DEBUG
	if(command != GF_NET_COMMAND_CONTINUE){
		IRC_PRINT("IRC_SEND size=%d, command=%d, value=%d\n", size, command, value);
		{
			int i;
			for(i = 0; i < size; i++){
//				IRC_PRINT("%d, ", buf[i]);
			}
//			IRC_PRINT("\n");
		}
	}
#endif
	
	value = NetIrcSys.my_value | (NetIrcSys.last_value << 4);
	IRC_Send(buf, size, command, value);
	NetIrcSys.send_buf = buf;
	NetIrcSys.send_size = size;
	NetIrcSys.send_command = command;
	NetIrcSys.send_value = NetIrcSys.my_value;
	NetIrcSys.send_action = TRUE;
	
	NetIrcSys.send_turn = 0;
}

//--------------------------------------------------------------
/**
 * @brief   send_turnが回ってきているのに何もデータを送っていない場合、この関数を使用して
 * 			通信継続の為のコマンドを送信する
 */
//--------------------------------------------------------------
void GFL_NET_IRC_CommandContinue(void)
{
	NET_IRC_SYS *nis = &NetIrcSys;

	if(nis->send_turn == TRUE){
		if(nis->retry_send_reserve == TRUE){	//再接続後の予約がある場合はそちらを送信
			IRC_Send(nis->send_buf, nis->send_size, nis->send_command, 
				nis->send_value | (NetIrcSys.last_value << 4));
			OS_TPrintf("予約送信 value = %d\n", nis->send_value);
			nis->retry_send_reserve = 0;
		}
		else{
			//通信継続の為のsend_turnだけ回す
			IRC_Send(NULL, 0, GF_NET_COMMAND_CONTINUE, 
				nis->send_value | (NetIrcSys.last_value << 4));
		}
		nis->send_turn = 0;
	}
}

//--------------------------------------------------------------
/**
 * @brief   IRCネットID取得
 *
 * @retval  ID
 */
//--------------------------------------------------------------
int GFL_NET_IRC_System_GetCurrentAid(void)
{
	if(NetIrcSys.isSender == TRUE){
		return GFL_NET_NO_PARENTMACHINE;
	}
	return 1;
}

//--------------------------------------------------------------
/**
 * @brief   IRC受信時に呼ばれるコールバック
 *
 * @param   data		
 * @param   size		
 * @param   command		
 * @param   value		
 */
//--------------------------------------------------------------
static void IRC_ReceiveCallback(u8 *data, u8 size, u8 command, u8 value)
{
	int send_id;
	int send_value, receive_value;
	
	size -= IRC_HEADER_SIZE;
	
	send_value = value & 0xf;
	receive_value = value >> 4;
	
	if(command != GF_NET_COMMAND_CONTINUE){
		IRC_PRINT("IRC受信コールバック呼び出し, size=%d, command=%d, send_value=%d, receive_value=%d\n", size,command,send_value,receive_value);
	}
	
	NetIrcSys.send_turn = TRUE;

	//-- 赤外線ライブラリ内部で使用しているシステムコマンド --//
	if(command >= 0xf0){
		if(command == 0xf4){
			OS_TPrintf("IRC切断コマンドを受信\n");
		}
		return;	//赤外線ライブラリ内部で使用しているシステムコマンドの為、ここでは無視
	}
	
	if(NetIrcSys.send_action == TRUE && NetIrcSys.retry_send_reserve == 0){
		//何らかの返事が向こうから返ってきているなら先ほど送信したデータは向こうに渡ったという事
		//但しライブラリ内部のシステムコマンドは切断コマンドだったりするので、
		//システムコマンドの返事は無視する
		IRC_PRINT("action返事受信 my_value = %d, receive_value=%d\n", NetIrcSys.my_value, receive_value);
		if(receive_value == NetIrcSys.my_value){
			if(NetIrcSys.my_value < SEND_CHECK_VALUE_RANGE){
				NetIrcSys.my_value++;
			}
			else{
				NetIrcSys.my_value = 0;
			}
			NetIrcSys.send_action = 0;
		}
		else{
			NetIrcSys.retry_send_reserve = TRUE;	//再度送信予約
		}
	}
	
	//赤外線専用のシステムコマンド解釈
	switch(command){
	case GF_NET_COMMAND_CONTINUE:
		return;		//通信継続の為の延命コマンドの為、ここで終了
	default:
		break;
	}

	if(NetIrcSys.last_value == send_value){
		//最後に受け取ったvalue値と同じvalue値の場合、同じデータが2度送られてきているので無視する
		//※再接続した時にこのケースが発生する。
		OS_TPrintf("赤外線：同一valueを受信\n");
		return;
	}
	NetIrcSys.last_value = value;

	send_id = IRC_TargetIDGet();
	if(NetIrcSys.recieve_func != NULL){
		NetIrcSys.recieve_func(send_id, (u16*)data, size);
	}
}

//--------------------------------------------------------------
/**
 * @brief   通信相手のIDを取得
 *
 * @retval  相手のID
 */
//--------------------------------------------------------------
static int IRC_TargetIDGet(void)
{
	if(GFL_NET_IRC_IsSender() == TRUE){
		return 1;	//自分が親なら子からしかデータが来ないので2固定
	}
	
	//自分が子なら、親からしか来ないのでGFL_NET_NO_PARENTMACHINE固定
	return GFL_NET_NO_PARENTMACHINE;
}

//--------------------------------------------------------------
/**
 * @brief   親のMacAddressをワークに保存
 *
 * @param   mac_address		親のMacAddress
 */
//--------------------------------------------------------------
void GFL_NET_IRC_ParentMacAddressSet(const u8 *mac_address)
{
	GFL_STD_MemCopy(mac_address, NetIrcSys.parent_MacAddress, 6);
}

//--------------------------------------------------------------
/**
 * @brief   親のMacAddressを取り出す
 *
 * @retval  親のMacAddressを保持しているワークへのポインタ
 */
//--------------------------------------------------------------
u8 * GFL_NET_IRC_ParentMacAddressGet(void)
{
	return NetIrcSys.parent_MacAddress;
}

//--------------------------------------------------------------
/**
 * @brief   初めての接続後のワーク設定
 */
//--------------------------------------------------------------
void GFL_NET_IRC_FirstConnect(void)
{
	NetIrcSys.connect = TRUE;
	NetIrcSys.isSender = IRC_IsSender();
	NetIrcSys.my_value = 0;
	NetIrcSys.last_value = SEND_CHECK_VALUE_RANGE-1;	//最初のデータをちゃんと受け取れるようにmy_valueと違えば何でもよい
	if(NetIrcSys.isSender == TRUE){
		OS_TPrintf("赤外線：親機になった\n");
	}
	else{
		OS_TPrintf("赤外線：子機になった\n");
		//レシーバーから最初の通信を始める
		NetIrcSys.send_turn = TRUE;
	}
}

//--------------------------------------------------------------
/**
 * @brief   最初に接続要求した端末か調べる
 *
 * @retval  TRUE:最初に接続要求した端末である
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_IsSender(void)
{
	return NetIrcSys.isSender;
}

//--------------------------------------------------------------
/**
 * @brief   接続中かどうか(再接続中は繋がっていると返します)
 *
 * @retval  TRUE:接続している
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_IsConnect(void)
{
	if(IRC_IsConnect() == TRUE){
		return TRUE;
	}
	if(NetIrcSys.retry_time < NET_IRC_SHOUT_DOWN_TIME){
		return TRUE;	//再接続中
	}
	return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   再接続中など関係なく、純粋に今、繋がっているか
 *
 * @retval  TRUE:接続している
 */
//--------------------------------------------------------------
BOOL GFL_NET_IRC_IsConnectSystem(void)
{
	return IRC_IsConnect();
}

//--------------------------------------------------------------
/**
 * @brief   赤外線実行処理
 */
//--------------------------------------------------------------
void GFL_NET_IRC_Move(void)
{
	NET_IRC_SYS *nis = &NetIrcSys;

	if(nis->connect == FALSE && nis->retry_time >= NET_IRC_SHOUT_DOWN_TIME){
		//再接続も失敗して完全に終了状態。IRC_Moveを動かすとまだ相手がリトライをかけていた場合
		//こちらは完全終了に移行したのに向こうが繋がる、となってしまうのでここでreturnする
		return;
	}
	
	if(nis->retry_time == 0){
		IRC_Move();
	}
	
	//繋がっている場合、切断していないか調べ、切断していれば再接続処理へ回す
	if(nis->connect == TRUE){
		if(nis->retry_time > 0){
			//再接続中
			if(IRC_IsConnect() == TRUE){
				//再接続した
				nis->retry_time = 0;
				OS_TPrintf("赤外線：再接続OK!\n");
				if(IRC_IsSender() == TRUE){
					nis->send_turn = 0;
				}
				else{
					nis->send_turn = TRUE;
				}
				if(nis->send_action == TRUE){
					nis->retry_send_reserve = TRUE;	//切断前に送ったデータを再度送信予約
				}
			}
			else{
				//再接続待ち
				if(nis->retry_time < NET_IRC_RETRY_START_WAIT){
					//実際にリトライをかけるまで、相手方の初期化処理などもあるので
					//少し間を空ける
					nis->retry_time++;
					return;
				}
				else if(nis->retry_time == NET_IRC_RETRY_START_WAIT){
					IRC_Init();
					IRC_SetRecvCallback(IRC_ReceiveCallback);
					nis->retry_time++;
					return;
				}

				if(nis->retry_time < NET_IRC_SHOUT_DOWN_TIME || NET_IRC_SHOUT_DOWN_TIME == 0){
					if(nis->isSender == TRUE && nis->retry_time % NET_IRC_CONNECT_REQ_WAIT == 0){
						IRC_Connect();
					}
				#if (NET_IRC_SHOUT_DOWN_TIME != 0)
					nis->retry_time++;
				#endif
				}
				else{	//再接続の時間切れ
					OS_TPrintf("赤外線：再接続の時間切れ\n");
					nis->connect = FALSE;
					IRC_Shutdown();
				}
				IRC_Move();
			}
		}
		else{
			//繋がっている
			if(IRC_IsConnect() == FALSE){
				OS_TPrintf("赤外線：切断した。再接続します\n");
				IRC_Shutdown();
				nis->retry_time = 1;	//再接続へ
			}
		}
	}
}
