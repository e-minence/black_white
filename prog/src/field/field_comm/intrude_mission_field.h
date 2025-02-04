//==============================================================================
/**
 * @file    intrude_mission_field.h
 * @brief   ミッションでフィールドでのみ使用するツール類　※FIELDMAPオーバーレイに配置
 * @author  matsuda
 * @date    2010.01.29(金)
 */
//==============================================================================
#pragma once


//==============================================================================
//  定数定義
//==============================================================================
///ミッションステータス
typedef enum{
  MISSION_STATUS_NULL,      ///<ミッション実施なし
  MISSION_STATUS_NOT_ENTRY, ///<ミッションは実施されているが参加していない
  MISSION_STATUS_READY,     ///<ミッション開始待ち
  MISSION_STATUS_EXE,       ///<ミッション中
  MISSION_STATUS_RESULT,    ///<結果が届いている or 結果表示中
}MISSION_STATUS;


//==============================================================================
//  外部関数宣言
//==============================================================================
extern MISSION_STATUS MISSION_FIELD_CheckStatus(MISSION_SYSTEM *mission);
extern TALK_TYPE MISSION_FIELD_GetTalkType(INTRUDE_COMM_SYS_PTR intcomm, NetID net_id);
