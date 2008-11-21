//======================================================================
/**
 * @file	field_comm_func.c
 * @brief	フィールド通信　通信機能部
 * @author	ariizumi
 * @data	08/11/13
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "net/network_define.h"

#include "gamesystem/playerwork.h"
#include "field_comm_main.h"
#include "field_comm_func.h"
#include "field_comm_data.h"
#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
//送受信関数用
enum FIELD_COMM_COMMAND_TYPE
{
    FC_CMD_SELFDATA = GFL_NET_CMD_COMMAND_MAX,	//自機データ
	FC_CMD_REQUEST_DATA,	//データ要求
	FC_CMD_SELF_PROFILE,	//キャラの基本情報
	FC_CMD_COMMON_FLG,		//汎用フラグ
};

//======================================================================
//	typedef struct
//======================================================================
struct _FIELD_COMM_FUNC
{
	HEAPID	heapID_;
	FIELD_COMM_MODE commMode_;
	u8	seqNo_;
	
	BOOL	isInitCommSystem_;
	
	//会話関係
	u8		talkID_;		//会話対象
	u8		talkPosX_;
	u8		talkPosZ_;		//会話対象位置
	
};

typedef struct
{
	u8	mode_:1;	//0:待機 1:探索
	u8	memberNum_:3;	//人数

	u8	pad_:4;
}FIELD_COMM_BEACON;

//======================================================================
//	proto
//======================================================================

FIELD_COMM_FUNC* FIELD_COMM_FUNC_InitSystem( HEAPID heapID );
void	FIELD_COMM_FUNC_TermSystem( FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_InitCommSystem( FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_TermCommSystem( FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_UpdateSystem( FIELD_COMM_FUNC *commFunc );
static	void FIELD_COMM_FUNC_UpdateSearchParent( FIELD_COMM_FUNC *commFunc );
static	u8	FIELD_COMM_FUNC_CompareBeacon( const FIELD_COMM_BEACON *firstBcn , const FIELD_COMM_BEACON *secondBcn );

void	FIELD_COMM_FUNC_StartCommWait( FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_StartCommSearch( FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_StartCommChangeover( FIELD_COMM_FUNC *commFunc );

//各種チェック関数
const BOOL FIELD_COMM_FUNC_IsFinishInitCommSystem( FIELD_COMM_FUNC *commFunc );
const BOOL FIELD_COMM_FUNC_IsFinishTermCommSystem( FIELD_COMM_FUNC *commFunc );
const FIELD_COMM_MODE FIELD_COMM_FUNC_GetCommMode( FIELD_COMM_FUNC *commFunc );
const int	FIELD_COMM_FUNC_GetMemberNum( FIELD_COMM_FUNC *commFunc );
const int	FIELD_COMM_FUNC_GetSelfIndex( FIELD_COMM_FUNC *commFunc );
const BOOL FIELD_COMM_FUNC_IsReserveTalk( FIELD_COMM_FUNC *commFunc );
void FIELD_COMM_FUNC_ResetReserveTalk( FIELD_COMM_FUNC *commFunc );
void FIELD_COMM_FUNC_GetTalkParterData_ID( u8 *ID , FIELD_COMM_FUNC *commFunc );
void FIELD_COMM_FUNC_GetTalkParterData_Pos( u8 *posX , u8 *posZ , FIELD_COMM_FUNC *commFunc );

//送受信関数
void	FIELD_COMM_FUNC_Send_SelfData( FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_Post_SelfData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
void	FIELD_COMM_FUNC_Send_RequestData( const u8 charaIdx , const F_COMM_REQUEST_TYPE reqType , FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_Post_RequestData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
void	FIELD_COMM_FUNC_Send_SelfProfile( const int sendNetID ,FIELD_COMM_FUNC *commFunc );
void	FIELD_COMM_FUNC_Post_SelfProfile( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
void	FIELD_COMM_FUNC_Send_CommonFlg( const F_COMM_COMMON_FLG flg , const u16 val , const u8 sendID );
void	FIELD_COMM_FUNC_Post_CommonFlg( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );

//各種コールバック
void	FIELD_COMM_FUNC_FinishInitCallback( void* pWork );
void	FIELD_COMM_FUNC_FinishTermCallback( void* pWork );
void*	FIELD_COMM_FUNC_GetBeaconData(void* pWork);		// ビーコンデータ取得関数  
int		FIELD_COMM_FUNC_GetBeaconSize(void* pWork);		// ビーコンデータサイズ取得関数 
BOOL	FIELD_COMM_FUNC_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 ); // ビーコンのサービスを比較して繋いで良いかどうか判断する
void	FIELD_COMM_FUNC_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork);		// 通信不能なエラーが起こった場合呼ばれる 切断するしかない
void	FIELD_COMM_FUNC_DisconnectCallBack(void* pWork);	// 通信切断時に呼ばれる関数(終了時

static const NetRecvFuncTable FieldCommRecvTable[] = {
	{ FIELD_COMM_FUNC_Post_SelfData		,NULL },
	{ FIELD_COMM_FUNC_Post_RequestData	,NULL },
	{ FIELD_COMM_FUNC_Post_SelfProfile	,NULL },
	{ FIELD_COMM_FUNC_Post_CommonFlg	,NULL },
};

//--------------------------------------------------------------
//	システム初期化
//	この時点では通信は開始しません
//--------------------------------------------------------------
FIELD_COMM_FUNC* FIELD_COMM_FUNC_InitSystem( HEAPID heapID )
{
	FIELD_COMM_FUNC	*commFunc;
	
	commFunc = GFL_HEAP_AllocMemory( heapID , sizeof(FIELD_COMM_FUNC) );
	commFunc->heapID_ = heapID;
	commFunc->isInitCommSystem_ = GFL_NET_IsInit();
	//通信ステートのバックアップをチェック
	if( FIELD_COMM_DATA_IsExistSystem() == TRUE )
		commFunc->commMode_ = (FIELD_COMM_MODE)FIELD_COMM_DATA_GetFieldCommMode();
	else
		commFunc->commMode_ = FIELD_COMM_MODE_NONE;
	//通信が初期化されていたならアイコンをリロードする
	if( commFunc->isInitCommSystem_ == TRUE )
		GFL_NET_ReloadIcon();
	return commFunc;
}

//--------------------------------------------------------------
//	システム開放
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_TermSystem( FIELD_COMM_FUNC *commFunc )
{
	//通信のステートを保存
	if( FIELD_COMM_DATA_IsExistSystem() == TRUE )
		FIELD_COMM_DATA_SetFieldCommMode(commFunc->commMode_);
	GFL_HEAP_FreeMemory( commFunc );
}

//--------------------------------------------------------------
//	通信開始
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_InitCommSystem( FIELD_COMM_FUNC *commFunc )
{
	GFLNetInitializeStruct aGFLNetInit = {
		FieldCommRecvTable,	//NetSamplePacketTbl,  // 受信関数テーブル
		NELEMS(FieldCommRecvTable), // 受信テーブル要素数
        NULL,    ///< ハードで接続した時に呼ばれる
        NULL,    ///< ネゴシエーション完了時にコール
        NULL,	// ユーザー同士が交換するデータのポインタ取得関数
		NULL,	// ユーザー同士が交換するデータのサイズ取得関数
		FIELD_COMM_FUNC_GetBeaconData,		// ビーコンデータ取得関数  
		FIELD_COMM_FUNC_GetBeaconSize,		// ビーコンデータサイズ取得関数 
		FIELD_COMM_FUNC_CheckConnectService,	// ビーコンのサービスを比較して繋いで良いかどうか判断する
		FIELD_COMM_FUNC_ErrorCallBack,		// 通信不能なエラーが起こった場合呼ばれる 切断するしかない
		FIELD_COMM_FUNC_DisconnectCallBack,	// 通信切断時に呼ばれる関数(終了時
		NULL,	// オート接続で親になった場合
#if GFL_NET_WIFI
		NULL,NULL,NULL,NULL,
#endif //GFL_NET_WIFI
		0x444,	//ggid  DP=0x333,RANGER=0x178,WII=0x346
		GFL_HEAPID_APP,  //元になるheapid
		HEAPID_NETWORK,  //通信用にcreateされるHEAPID
		HEAPID_WIFI,  //wifi用にcreateされるHEAPID
		HEAPID_NETWORK,	//
		GFL_WICON_POSX,GFL_WICON_POSY,	// 通信アイコンXY位置
		4,//_MAXNUM,	//最大接続人数
		48,//_MAXSIZE,	//最大送信バイト数
		4,//_BCON_GET_NUM,  // 最大ビーコン収集数
		TRUE,		// CRC計算
		FALSE,		// MP通信＝親子型通信モードかどうか
		GFL_NET_TYPE_WIRELESS,		//通信タイプの指定
		TRUE,		// 親が再度初期化した場合、つながらないようにする場合TRUE
		WB_NET_FIELDMOVE_SERVICEID	//GameServiceID
	};

	GFL_NET_Init( &aGFLNetInit , FIELD_COMM_FUNC_FinishInitCallback , (void*)commFunc );
}

//--------------------------------------------------------------
//	通信システム終了(通信の切断はしません
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_TermCommSystem( FIELD_COMM_FUNC *commFunc )
{
	GFL_NET_Exit( FIELD_COMM_FUNC_FinishTermCallback );
}

//--------------------------------------------------------------
//	通信システム更新(ビーコンの待ちうけ
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_UpdateSystem( FIELD_COMM_FUNC *commFunc )
{
	//待ち受け側でもビーコンをチェックしてみる
	//if( commFunc->commMode_ == FIELD_COMM_MODE_SEARCH )
	//	待ちうけ・探索中で親機ではない時
	if( commFunc->commMode_ == FIELD_COMM_MODE_SEARCH ||
		commFunc->commMode_ == FIELD_COMM_MODE_WAIT	)
	{
		//子機と親機で処理を分ける←見分ける方法が無いので断念・・・
	//	if( GFL_NET_IsParentMachine() == FALSE )
		{
			//子機の場合はビーコンのチェック
			FIELD_COMM_FUNC_UpdateSearchParent( commFunc );
		}
	//	else
		{
			if( FIELD_COMM_FUNC_GetMemberNum(commFunc) > 1 )
			{
				//親機の場合は子機が来たら接続状態に
				ARI_TPrintf("Connect!(Parent)\n");
				commFunc->commMode_ = FIELD_COMM_MODE_CONNECT;
			}
		}
	}
	if( commFunc->commMode_ == FIELD_COMM_MODE_TRY_CONNECT )
	{
		switch( commFunc->seqNo_ )
		{
		case 0:
			if( GFL_NET_HANDLE_RequestNegotiation() == TRUE )
			{
				commFunc->seqNo_++;
			}
			break;
		case 1:
			{
				GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
				if( GFL_NET_HANDLE_IsNegotiation( selfHandle ) == TRUE )
				{
					commFunc->seqNo_++;
				}
			}
			break;
		case 2:
			{
				GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
				if( GFL_NET_IsSendEnable( selfHandle ) == TRUE )
				{
					commFunc->commMode_ = FIELD_COMM_MODE_CONNECT;
				}
			}
		}
	}
}

//--------------------------------------------------------------
//	子機で親機を探している状態
//--------------------------------------------------------------
static	void FIELD_COMM_FUNC_UpdateSearchParent( FIELD_COMM_FUNC *commFunc )
{
	u8 bcnIdx = 0;
	int targetIdx = -1;
	FIELD_COMM_BEACON *bcnData;
	const FIELD_COMM_BEACON *selfBcn = FIELD_COMM_FUNC_GetBeaconData((void*)commFunc);
	while( GFL_NET_GetBeaconData(bcnIdx) != NULL )
	{
		bcnData = GFL_NET_GetBeaconData( bcnIdx );
		if( selfBcn->mode_ == 1 || bcnData->mode_ == 1 )
		{
			//接続条件を満たした。
			if( targetIdx == -1 )
			{
				targetIdx = bcnIdx;
			}
			else
			{
				//すでに他のビーコンが接続候補にあるので比較
				const FIELD_COMM_BEACON *compBcn = GFL_NET_GetBeaconData(targetIdx);
				const u8 result = FIELD_COMM_FUNC_CompareBeacon( bcnData , compBcn );
				if( result == 1 )
				{
					targetIdx = bcnIdx;
				}
			}
		}
		bcnIdx++;
	}
		if( targetIdx != -1 )
	{
		//ビーコンがあった
		u8 *macAdr = GFL_NET_GetBeaconMacAddress(targetIdx);
		if( macAdr != NULL )
		{
			GFL_NET_ConnectToParent( macAdr ); 
			commFunc->commMode_ = FIELD_COMM_MODE_TRY_CONNECT;
			commFunc->seqNo_ = 0;
			ARI_TPrintf("Connect!(Child)\n");
		}
	}
}
	
//--------------------------------------------------------------
//	ビーコンの比較(人数が多い・共に待ち受け中を優先する
//	@return 0:エラー 1:第１引数のビーコン 2:第２引数のビーコン
//--------------------------------------------------------------
static	u8	FIELD_COMM_FUNC_CompareBeacon( const FIELD_COMM_BEACON *firstBcn , const FIELD_COMM_BEACON *secondBcn )
{
	//人数マックスチェック
	if( firstBcn->memberNum_ == FIELD_COMM_MEMBER_MAX &&
		secondBcn->memberNum_ == FIELD_COMM_MEMBER_MAX )
		return 0;
	
	if(	secondBcn->memberNum_ == FIELD_COMM_MEMBER_MAX ||
		firstBcn->memberNum_ > secondBcn->memberNum_ )
		return 1;
	if( firstBcn->memberNum_ == FIELD_COMM_MEMBER_MAX ||
		firstBcn->memberNum_ < secondBcn->memberNum_ )
		return 2;
	
	if( firstBcn->mode_ > secondBcn->mode_ )
		return 1;
	if( firstBcn->mode_ < secondBcn->mode_ )
		return 2;

	return (GFUser_GetPublicRand(2)+1);
}

//--------------------------------------------------------------
//	通信の開始(侵入受付状態
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_StartCommWait( FIELD_COMM_FUNC *commFunc )
{
	if( commFunc->commMode_ == FIELD_COMM_MODE_NONE )
	{
		GFL_NET_Changeover(NULL);
	}
	commFunc->commMode_ = FIELD_COMM_MODE_WAIT;
}

//--------------------------------------------------------------
//	通信の開始(侵入先探索状態
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_StartCommSearch( FIELD_COMM_FUNC *commFunc )
{
	if( commFunc->commMode_ == FIELD_COMM_MODE_NONE )
	{
		GFL_NET_Changeover(NULL);
	}
	commFunc->commMode_ = FIELD_COMM_MODE_SEARCH;
}

//--------------------------------------------------------------
//	探索通信の開始(親子交互通信・未使用
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_StartCommChangeover( FIELD_COMM_FUNC *commFunc )
{
	//GFL_NET_ChangeoverConnect(NULL);
}


//======================================================================
//	各種チェック関数
//======================================================================
//--------------------------------------------------------------
// 通信ライブラリ　初期化・開放用  
//--------------------------------------------------------------
const BOOL FIELD_COMM_FUNC_IsFinishInitCommSystem( FIELD_COMM_FUNC *commFunc )
{
	return commFunc->isInitCommSystem_;
}
const BOOL FIELD_COMM_FUNC_IsFinishTermCommSystem( FIELD_COMM_FUNC *commFunc )
{
	return !commFunc->isInitCommSystem_;
}

//--------------------------------------------------------------
//	通信状態？を取得  
//--------------------------------------------------------------
const FIELD_COMM_MODE FIELD_COMM_FUNC_GetCommMode( FIELD_COMM_FUNC *commFunc )
{
	return commFunc->commMode_;
}
//--------------------------------------------------------------
//	接続人数を取得  
//--------------------------------------------------------------
const int	FIELD_COMM_FUNC_GetMemberNum( FIELD_COMM_FUNC *commFunc )
{
	/*
	u8 i;
	u8 num = 1;//自分の分
	for( i=0;i<FIELD_COMM_MEMBER_MAX;i++ )
	{
		if( FIELD_COMM_DATA_GetCharaData_State(i) != FCCS_NONE )
			num++;
	}
	return num;
	*/
	return GFL_NET_GetConnectNum();
}

//--------------------------------------------------------------
// 自分のインデックス(netID)を取得  
//--------------------------------------------------------------
const int	FIELD_COMM_FUNC_GetSelfIndex( FIELD_COMM_FUNC *commFunc )
{
	GFL_NETHANDLE *handle =GFL_NET_HANDLE_GetCurrentHandle();
	return GFL_NET_GetNetID( handle );
}

//--------------------------------------------------------------
// 会話予約のデータ取得  
//--------------------------------------------------------------
void FIELD_COMM_FUNC_GetTalkParterData_ID( u8 *ID , FIELD_COMM_FUNC *commFunc )
{
	*ID = commFunc->talkID_;
}
void FIELD_COMM_FUNC_GetTalkParterData_Pos( u8 *posX , u8 *posZ , FIELD_COMM_FUNC *commFunc )
{
	*posX = commFunc->talkPosX_;
	*posZ = commFunc->talkPosZ_;
}

//======================================================================
//送受信関数
//======================================================================
typedef struct
{
	u16 posX_;
	u16 posZ_;
	s8	posY_;		//マックス不明なので。場合によってはなしでOK？
	u8	dir_;		//グリッドなので上下左右が0〜3で入る
	u16	zoneID_;	//ここは通信用のIDとして変換して抑えられる
	u8	talkState_;
}FIELD_COMM_PLAYER_PACKET;
//--------------------------------------------------------------
// 自分のデータ送信  
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_Send_SelfData( FIELD_COMM_FUNC *commFunc )
{
	FIELD_COMM_PLAYER_PACKET plPkt;
	PLAYER_WORK *plWork = NULL;
	const VecFx32 *pos;
	u16 dir;
	ZONEID zoneID;
	u8	talkState;

	//データ収集
	plWork = FIELD_COMM_DATA_GetSelfData_PlayerWork();
	zoneID = PLAYERWORK_getZoneID( plWork );
	pos = PLAYERWORK_getPosition( plWork );
	dir = PLAYERWORK_getDirection( plWork );
	talkState = FIELD_COMM_DATA_GetTalkState( FCD_SELF_INDEX );
	//パケットにセット
	plPkt.zoneID_ = zoneID;
	plPkt.posX_ = F32_CONST( pos->x );
	plPkt.posY_ = (int)F32_CONST( pos->y );
	plPkt.posZ_ = F32_CONST( pos->z );
	//plPkt.dir_ = dir>>8;
	plPkt.dir_ = dir;
	plPkt.talkState_ = talkState;

//	ARI_TPrintf("SEND[ ][%d][%d][%d][%d]\n",plPkt.posX_,plPkt.posY_,plPkt.posZ_,dir);
	{
		GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
		const BOOL ret = GFL_NET_SendDataEx( selfHandle , GFL_NET_SENDID_ALLUSER , 
				FC_CMD_SELFDATA , sizeof(FIELD_COMM_PLAYER_PACKET) , 
				(void*)&plPkt , FALSE , TRUE , FALSE );
		if( ret == FALSE ){
			ARI_TPrintf("FieldComm SendSelfData is failue!\n");
		}
	}
}

//--------------------------------------------------------------
// 自分のデータ受信(要は他のプレイヤーの受信  
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_Post_SelfData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
	//自分のデータも一応セットしておく(同期用に使う
	PLAYER_WORK *plWork;
	const FIELD_COMM_PLAYER_PACKET *pkt = (FIELD_COMM_PLAYER_PACKET*)pData;
	VecFx32 pos;
	u16 dir;
	
	//パケットデータを戻す
	pos.x = FX32_CONST( pkt->posX_ );
	pos.y = FX32_CONST( pkt->posY_ );
	pos.z = FX32_CONST( pkt->posZ_ );
	//dir	= pkt->dir_<<8;
	dir = pkt->dir_;

//	ARI_TPrintf("POST[%d][%d][%d][%d][%d]\n",netID,pkt->posX_,pkt->posY_,pkt->posZ_,dir);
	
	//set
	plWork = FIELD_COMM_DATA_GetCharaData_PlayerWork(netID);
	PLAYERWORK_setPosition( plWork , &pos );
	PLAYERWORK_setDirection( plWork , dir );
	PLAYERWORK_setZoneID( plWork , pkt->zoneID_ );
	FIELD_COMM_DATA_SetCharaData_IsValid( netID , TRUE );
	FIELD_COMM_DATA_SetTalkState( netID , pkt->talkState_ );

//	ARI_TPrintf("FieldComm PostSelfData[%d]\n",netID);
}

//--------------------------------------------------------------
// 他のプレイヤーにデータを要求する
// Postの関数でそのまま"要求されたキャラだけに"データを送る
// @param charaIdx キャラ番号(=netID)
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_Send_RequestData( const u8 charaIdx , const F_COMM_REQUEST_TYPE reqType , FIELD_COMM_FUNC *commFunc )
{
	GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
	u8	type = reqType;
	const BOOL ret = GFL_NET_SendDataEx( selfHandle , charaIdx , 
			FC_CMD_REQUEST_DATA , 1 , 
			(void*)&type , FALSE , FALSE , FALSE );
		ARI_TPrintf("FieldComm Send RequestData[%d:%d].\n",charaIdx,type);
	if( ret == FALSE ){
		ARI_TPrintf("FieldComm SendRequestData is failue!\n");
	}
}
void	FIELD_COMM_FUNC_Post_RequestData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	const u8 type = *(u8*)pData;
	ARI_TPrintf("FieldComm PostReqData[%d:%d]\n",netID,type);
	switch( type )
	{
	case FCRT_PROFILE:
		FIELD_COMM_FUNC_Send_SelfProfile( netID , commFunc );
		break;
	default:
		OS_TPrintf("Invalid Type[%d]!\n!",type);
		GF_ASSERT( NULL );
		break;
	}
}

//--------------------------------------------------------------
// 自分のプロフィールの送信(最初に送る物 IDとかキャラの基本情報
// 要求された相手だけに送る
//--------------------------------------------------------------
typedef struct
{
	u16 ID_;
	u8	sex_:1;
	u8	regionCode_:7;
}FIELD_COMM_CHARA_PROFILE;
void	FIELD_COMM_FUNC_Send_SelfProfile( const int sendNetID ,FIELD_COMM_FUNC *commFunc )
{
	GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
	FIELD_COMM_CHARA_PROFILE profile;
	//FIXME:IDとかの正しい持って来る方法がわからないので仮処理
	profile.ID_ = 1000+GFL_NET_GetNetID( selfHandle );
	profile.sex_ = 0;
	profile.regionCode_ = 0;
	{
		const BOOL ret = GFL_NET_SendDataEx( selfHandle , sendNetID , 
				FC_CMD_SELF_PROFILE , sizeof( FIELD_COMM_CHARA_PROFILE ) , 
				(void*)&profile , FALSE , FALSE , FALSE );
		ARI_TPrintf("FieldComm Send SelfProfile[%d:%d].\n",sendNetID,profile.ID_);
		if( ret == FALSE ){
			ARI_TPrintf("FieldComm Send SelfProfile is failue!\n");
		}
	}
}
void	FIELD_COMM_FUNC_Post_SelfProfile( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	const FIELD_COMM_CHARA_PROFILE *prof = (FIELD_COMM_CHARA_PROFILE*)pData;
	PLAYER_WORK *plWork = FIELD_COMM_DATA_GetCharaData_PlayerWork( netID );
	ARI_TPrintf("FieldComm Post SelfProfile[%d:%d]\n",netID,prof->ID_);
	
	plWork->mystatus.id = prof->ID_;
	plWork->mystatus.sex = prof->sex_;
	plWork->mystatus.region_code = prof->regionCode_;
	FIELD_COMM_DATA_SetCharaData_State( netID , FCCS_EXIST_DATA );

}

//--------------------------------------------------------------
// 汎用フラグ
//--------------------------------------------------------------
typedef struct
{
	u8	flg_;
	u16 value_;
}FIELD_COMM_COMMONFLG_PACKET;
void	FIELD_COMM_FUNC_Send_CommonFlg( const F_COMM_COMMON_FLG flg , const u16 val , const u8 sendID)
{
	GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
	FIELD_COMM_COMMONFLG_PACKET pkt;
	pkt.flg_ = flg;
	pkt.value_ = val;
	{
		const BOOL ret = GFL_NET_SendDataEx( selfHandle , sendID , 
				FC_CMD_COMMON_FLG , sizeof( FIELD_COMM_COMMONFLG_PACKET ) , 
				(void*)&pkt , FALSE , FALSE , FALSE );
		ARI_TPrintf("FieldComm Send commonFlg[%d:%d:%d].\n",sendID,flg,val);
		if( ret == FALSE ){
			ARI_TPrintf("FieldComm Send commonFlg is failue!\n");
		}
	}
}
void	FIELD_COMM_FUNC_Post_CommonFlg( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	const FIELD_COMM_COMMONFLG_PACKET *pkt = (FIELD_COMM_COMMONFLG_PACKET*)pData;
	const F_COMM_TALK_STATE talkState = FIELD_COMM_DATA_GetTalkState( FCD_SELF_INDEX );
	ARI_TPrintf("FieldComm PostCommonFlg[%d:%d:%d]\n",netID,pkt->flg_,pkt->value_);
	switch( pkt->flg_ )
	{
	case FCCF_TALK_REQUEST:		//会話要求
		if( talkState == FCTS_NONE )
		{	
			FIELD_COMM_DATA_SetTalkState( FCD_SELF_INDEX , FCTS_RESERVE_TALK );
			commFunc->talkPosX_ = pkt->value_ & 0x00FF;
			commFunc->talkPosZ_ = pkt->value_>>8;
			commFunc->talkID_ = netID;
		}
		else
		{
			FIELD_COMM_FUNC_Send_CommonFlg( FCCF_TALK_UNPOSSIBLE , 0xf , netID );
		}
		break;

	case FCCF_TALK_UNPOSSIBLE:	//要求に対して無理
		GF_ASSERT( talkState == FCTS_WAIT_TALK );
		FIELD_COMM_DATA_SetTalkState( FCD_SELF_INDEX , FCTS_UNPOSSIBLE );
		break;

	case FCCF_TALK_ACCEPT:		//要求に対して許可
		GF_ASSERT( talkState == FCTS_WAIT_TALK );
		FIELD_COMM_DATA_SetTalkState( FCD_SELF_INDEX , FCTS_ACCEPT );
		commFunc->talkID_ = netID;
		break;

	default:
		OS_TPrintf("Invalid flg[%d]!\n!",pkt->flg_);
		GF_ASSERT( NULL );
		break;
	}
}

//======================================================================
//各種コールバック
//======================================================================
//--------------------------------------------------------------
// 通信ライブラリ　初期化・開放用  
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_FinishInitCallback( void* pWork )
{
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	commFunc->isInitCommSystem_ = TRUE;
}
void	FIELD_COMM_FUNC_FinishTermCallback( void* pWork )
{
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	commFunc->isInitCommSystem_ = FALSE;
}

//--------------------------------------------------------------
// ビーコンデータ取得関数  
//--------------------------------------------------------------
void*	FIELD_COMM_FUNC_GetBeaconData(void* pWork)
{
	static FIELD_COMM_BEACON beacon;
	FIELD_COMM_FUNC *commFunc = (FIELD_COMM_FUNC*)pWork;
	
	if( commFunc->commMode_ == FIELD_COMM_MODE_WAIT )
		beacon.mode_ = 0;
	else
		beacon.mode_ = 1;
	beacon.memberNum_ = FIELD_COMM_FUNC_GetMemberNum(commFunc);

	return (void*)&beacon;
}

//--------------------------------------------------------------
// ビーコンデータサイズ取得関数 
//--------------------------------------------------------------
int		FIELD_COMM_FUNC_GetBeaconSize(void *pWork)
{
	return sizeof( FIELD_COMM_BEACON );
}

//--------------------------------------------------------------
// ビーコンのサービスを比較して繋いで良いかどうか判断する
//--------------------------------------------------------------
BOOL	FIELD_COMM_FUNC_CheckConnectService(GameServiceID GameServiceID1 , GameServiceID GameServiceID2 )
{
	return (GameServiceID1==GameServiceID2);
}

//--------------------------------------------------------------
// 通信不能なエラーが起こった場合呼ばれる 切断するしかない
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_ErrorCallBack(GFL_NETHANDLE* pNet,int errNo, void* pWork)
{
}

//--------------------------------------------------------------
// 通信切断時に呼ばれる関数(終了時
//--------------------------------------------------------------
void	FIELD_COMM_FUNC_DisconnectCallBack(void* pWork)
{
}

