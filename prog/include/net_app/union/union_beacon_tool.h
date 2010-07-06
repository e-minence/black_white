//=============================================================================
/**
 * @file	union_beacon_tool.H
 * @brief	���j�I�����[�������ڐݒ菈��
 * @author	Akito Mori
 * @date    	2006.03.16
 */
//=============================================================================
#pragma once

#include "print/wordset.h"


enum{
  UNION_VIEW_INDEX_MAN_START = 0,     ///<������INDEX�F�j�J�n
  UNION_VIEW_INDEX_MAN_END = 7,       ///<������INDEX�F�j�I��
  UNION_VIEW_INDEX_WOMAN_START = 8,   ///<������INDEX�F���J�n
  UNION_VIEW_INDEX_WOMAN_END = 15,    ///<������INDEX�F���I��
  
  UNION_VIEW_INDEX_MAX = UNION_VIEW_INDEX_WOMAN_END,  ///<������INDEX�ő吔
};


extern int UnionView_GetObjCode(int view_index);
extern int UnionView_GetTrType(int view_index);
extern int UnionView_GetMsgType(int view_index);
extern u16 *UnionView_GetPalNo( u16 * table, int sex, int view_type );
extern u16 *UnionView_PalleteTableAlloc( int heapID );
extern int UnionView_Objcode_to_Index(u16 objcode);
extern int UnionView_TrType_to_Index( u8 trtype );
