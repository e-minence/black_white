//======================================================================
/**
 * @file	field_comm_func.c
 * @brief	フィールド通信　通信機能部
 * @author	ariizumi
 * @data	08/11/13
 */
//======================================================================

#ifndef FIELD_COMM_FUNC_H__
#define FIELD_COMM_FUNC_H__

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
//通信状態
typedef enum
{
	FIELD_COMM_MODE_NONE,	//通信なし
	FIELD_COMM_MODE_WAIT,		//侵入待ち
	FIELD_COMM_MODE_SEARCH,		//侵入先探し中
	FIELD_COMM_MODE_CONNECT,	//侵入後
	FIELD_COMM_MODE_TRY_CONNECT,	//子機となり親へ接続を試みる
}FIELD_COMM_MODE;

typedef enum
{
	FCRT_PROFILE,	//キャラの基本情報
}F_COMM_REQUEST_TYPE;

//汎用フラグの送信
typedef enum
{
	FCCF_TALK_REQUEST,		//会話要求
	FCCF_TALK_UNPOSSIBLE,	//要求に対して無理
	FCCF_TALK_ACCEPT,		//要求に対して許可

	FCCF_FLG_MAX,			//不正値として使う
}F_COMM_COMMON_FLG;
//======================================================================
//	typedef struct
//======================================================================
typedef struct _FIELD_COMM_FUNC FIELD_COMM_FUNC;

//======================================================================
//	proto
//======================================================================

extern FIELD_COMM_FUNC* FIELD_COMM_FUNC_InitSystem( HEAPID heapID );
extern void	FIELD_COMM_FUNC_TermSystem( FIELD_COMM_FUNC *commFunc );
extern void	FIELD_COMM_FUNC_InitCommSystem( FIELD_COMM_FUNC *commFunc );
extern void	FIELD_COMM_FUNC_TermCommSystem( FIELD_COMM_FUNC *commFunc );
extern void	FIELD_COMM_FUNC_UpdateSystem( FIELD_COMM_FUNC *commFunc );

extern void	FIELD_COMM_FUNC_StartCommWait( FIELD_COMM_FUNC *commFunc );
extern void	FIELD_COMM_FUNC_StartCommSearch( FIELD_COMM_FUNC *commFunc );
extern void	FIELD_COMM_FUNC_StartCommChangeover( FIELD_COMM_FUNC *commFunc );

//送信用関数
extern void	FIELD_COMM_FUNC_Send_SelfData( FIELD_COMM_FUNC *commFunc );
extern void	FIELD_COMM_FUNC_Send_RequestData( const u8 charaIdx , const F_COMM_REQUEST_TYPE reqType , FIELD_COMM_FUNC *commFunc );
extern void	FIELD_COMM_FUNC_Send_SelfProfile( const int sendNetID ,FIELD_COMM_FUNC *commFunc );
extern void FIELD_COMM_FUNC_Send_CommonFlg( const F_COMM_COMMON_FLG flg , const u16 val , const u8 sendID );
//各種チェック関数
extern const BOOL FIELD_COMM_FUNC_IsFinishInitCommSystem( FIELD_COMM_FUNC *commFunc );
extern const BOOL FIELD_COMM_FUNC_IsFinishTermCommSystem( FIELD_COMM_FUNC *commFunc );
extern const FIELD_COMM_MODE FIELD_COMM_FUNC_GetCommMode( FIELD_COMM_FUNC *commFunc );
extern const int	FIELD_COMM_FUNC_GetMemberNum( FIELD_COMM_FUNC *commFunc );
extern const int	FIELD_COMM_FUNC_GetSelfIndex( FIELD_COMM_FUNC *commFunc );
extern const BOOL FIELD_COMM_FUNC_IsReserveTalk( FIELD_COMM_FUNC *commFunc );
extern void FIELD_COMM_FUNC_ResetReserveTalk( FIELD_COMM_FUNC *commFunc );
extern void FIELD_COMM_FUNC_GetTalkParterData_ID( u8 *ID , FIELD_COMM_FUNC *commFunc );
extern void FIELD_COMM_FUNC_GetTalkParterData_Pos( u8 *posX , u8 *posZ , FIELD_COMM_FUNC *commFunc );


#endif //FIELD_COMM_FUNC_H__

