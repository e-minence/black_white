//////////////////////////////////////////////////////////////////////////////
/**
 * @brief  調査データ
 * @file   research_data.h
 * @author obata
 * @date   2010.02.22
 */
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>


//============================================================================
// ■定数
//============================================================================
#define MAX_ANSWER_NUM_PER_QUESTION (17) // 質問あたりの回答選択肢の最大数
#define QUESTION_NUM_PER_TOPIC      (3)  // 調査項目あたりの質問数


//============================================================================
// ■回答データ
//============================================================================
typedef struct
{
  u16 ID;         // 回答ID
  u8  colorR;     // 表示カラー(R) [0, 31]
  u8  colorG;     // 表示カラー(G) [0, 31]
  u8  colorB;     // 表示カラー(B) [0, 31]
  u16 todayCount; // 今日の回答人数
  u32 totalCount; // いままでの回答人数

} ANSWER_DATA;


//============================================================================
// ■質問データ
//============================================================================
typedef struct
{
  u8  ID;         // 質問ID 
  u8  answerNum;  // 回答の数
  u16 todayCount; // 今日の回答人数
  u32 totalCount; // いままでの回答人数
  ANSWER_DATA answerData[ MAX_ANSWER_NUM_PER_QUESTION ]; // 回答データ

} QUESTION_DATA;


//============================================================================
// ■調査データ
//============================================================================
typedef struct 
{
  u8 topicID; // 調査項目ID
  QUESTION_DATA questionData[ QUESTION_NUM_PER_TOPIC ]; // 質問データ

} RESEARCH_DATA; 


//============================================================================
// ■データ取得関数
//============================================================================
// 調査項目IDを取得する
extern u8 RESEARCH_DATA_GetTopicID( const RESEARCH_DATA* data );
// 質問IDを取得する
extern u8 RESEARCH_DATA_GetQuestionID_byIndex( const RESEARCH_DATA* data, u8 qIdx );
// 回答の数を取得する
extern u8 RESEARCH_DATA_GetAnswerNumOfQuestion_byIndex( const RESEARCH_DATA* data, u8 qIdx );
// 今日の回答人数を取得する
extern u16 RESEARCH_DATA_GetTodayCountOfQuestion_byIndex( const RESEARCH_DATA* data, u8 qIdx );
// いままでの回答人数を取得する
extern u16 RESEARCH_DATA_GetTotalCountOfQuestion_byIndex( const RESEARCH_DATA* data, u8 qIdx );
// 回答IDを取得する
extern u16 RESEARCH_DATA_GetAnswerID_byIndex( const RESEARCH_DATA* data, u8 qIdx, u8 aIdx );
// 今日の回答人数を取得する
extern u16 RESEARCH_DATA_GetTodayCountOfAnswer( const RESEARCH_DATA* data, u8 qIdx, u8 aIdx );
// いままでの回答人数を取得する
extern u16 RESEARCH_DATA_GetTotalCountOfAnswer( const RESEARCH_DATA* data, u8 qIdx, u8 aIdx );
// 回答の表示カラー ( R ) を取得する
extern u8 RESEARCH_DATA_GetColorR( const RESEARCH_DATA* data, u8 qIdx, u8 aIdx );
// 回答の表示カラー ( G ) を取得する
extern u8 RESEARCH_DATA_GetColorG( const RESEARCH_DATA* data, u8 qIdx, u8 aIdx );
// 回答の表示カラー ( B ) を取得する
extern u8 RESEARCH_DATA_GetColorB( const RESEARCH_DATA* data, u8 qIdx, u8 aIdx );
// 質問IDのインデックスを取得する ( IDを指定 )
extern u8 RESEARCH_DATA_GetQuestionIndex_byID( const RESEARCH_DATA* data, u8 qID );
// 回答IDのインデックスを取得する ( IDを指定 )
extern u8 RESEARCH_DATA_GetAnswerIndex_byID( const RESEARCH_DATA* data, u8 qID, u8 aID );
