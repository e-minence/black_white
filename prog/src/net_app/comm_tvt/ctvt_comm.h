//======================================================================
/**
 * @file	ctvt_comm.h
 * @brief	通信TVTシステム：通信管
 * @author	ariizumi
 * @data	09/12/17
 *
 * モジュール名：CTVT_COMM
 */

//======================================================================
#pragma once

#include "comm_tvt_local_def.h"
//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define CTVT_COMM_INVALID_MEMBER (0xFF)
//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
//フラグのタイプ
typedef enum
{
  //COMMが使う
  CCFT_REQ_PHOTO,

  //COMMが使う＆親機が子に通知する用
  CCFT_TALK_MEMBER,
    
  //外で使う
  CCFT_REQ_TALK,  //会話要求通知
  CCFT_FINISH_TALK,  //会話終了通知
  CCFT_CANCEL_TALK,  //会話取り消し通知 //何かしらうまくいかなかったとき
  
  CCFT_MAX,
}CTVT_COMM_FLAG_TYPE;


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
extern CTVT_COMM_WORK* CTVT_COMM_InitSystem( COMM_TVT_WORK *work , const HEAPID heapId );
extern void CTVT_COMM_TermSystem( COMM_TVT_WORK *work , CTVT_COMM_WORK *commWork );
extern void CTVT_COMM_Main( COMM_TVT_WORK *work , CTVT_COMM_WORK *commWork );

extern const BOOL CTVT_COMM_SendFlg( COMM_TVT_WORK *work , CTVT_COMM_WORK *commWork , const u8 flg , const u32 value );

extern const u8 CTVT_COMM_GetSelfNetId( COMM_TVT_WORK *work , CTVT_COMM_WORK *commWork );
extern const u8 CTVT_COMM_GetTalkMember( COMM_TVT_WORK *work , CTVT_COMM_WORK *commWork );

//--------------------------------------------------------------
//	
//--------------------------------------------------------------