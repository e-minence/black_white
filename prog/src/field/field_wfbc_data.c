//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_wfbc_data.c
 *	@brief  White Forest  Black City 基本データ
 *	@author	tomoya takahashi
 *	@date		2009.11.10
 *
 *	モジュール名：FIELD_WFBC_CORE
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "system/gfl_use.h"

#include "field/field_wfbc_data.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 day*					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

static u32 WFBC_CORE_GetNotUseNpcID( const FIELD_WFBC_CORE* cp_wk, int idx );
static u32 WFBC_CORE_GetNotUseNpcIndex( const FIELD_WFBC_CORE* cp_wk, int idx );
static BOOL WFBC_CORE_IsUseNpc( const FIELD_WFBC_CORE* cp_wk, int npc_id );

// 人データをソート
static void WFBC_CORE_SortPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array, HEAPID heapID );
// 人データをつめる
static void WFBC_CORE_PackPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array );
// 人データを押し出し設定
static void WFBC_CORE_PushPeople( FIELD_WFBC_CORE* p_wk, const FIELD_WFBC_CORE_PEOPLE* cp_people );
// 配列がいっぱいかチェック
static BOOL WFBC_CORE_IsPeopleArrayFull( const FIELD_WFBC_CORE_PEOPLE* cp_array );
// 人を入れる
static void WFBC_CORE_PushPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array, const FIELD_WFBC_CORE_PEOPLE* cp_people );

// 空いている人物ワークを取得
static FIELD_WFBC_CORE_PEOPLE* WFBC_CORE_GetClearPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array );


//-------------------------------------
///	人物情報
//=====================================
static void WFBC_CORE_SetUpPeople( FIELD_WFBC_CORE_PEOPLE* p_people, u8 npc_id, const MYSTATUS* cp_mystatus );
static BOOL WFBC_CORE_People_IsGoBack( const FIELD_WFBC_CORE_PEOPLE* cp_people );
static BOOL WFBC_CORE_People_AddMood( FIELD_WFBC_CORE_PEOPLE* p_people, int add );


//-------------------------------------
///	FIELD_WFBC_CORE用関数
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  WFBCワークのクリア
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_Clear( FIELD_WFBC_CORE* p_wk )
{
  int i;

  GF_ASSERT( p_wk );
  
  p_wk->data_in = FALSE;
  p_wk->type    = FIELD_WFBC_CORE_TYPE_MAX;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[i] );
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->back_people[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief ノーマルマップの情報をクリア
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_ClearNormal( FIELD_WFBC_CORE* p_wk )
{
  int i;

  GF_ASSERT( p_wk );
  
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  裏世界の人情報をクリア
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_ClearBack( FIELD_WFBC_CORE* p_wk )
{
  int i;

  GF_ASSERT( p_wk );
  
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->back_people[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  街の情報を設定する
 *
 *	@param	p_wk          ワーク
 *	@param  cp_mystatus   まいステータス
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_SetUp( FIELD_WFBC_CORE* p_wk, const MYSTATUS* cp_mystatus )
{
  int i;
  int pos_idx;
  int npc_idx;
  int set_npc_idx;
  FIELD_WFBC_CORE_PEOPLE* p_people;
  GF_ASSERT( p_wk );
  
  p_wk->data_in = TRUE;
#if PM_VERSION == VERSION_BLACK
  p_wk->type    = FIELD_WFBC_CORE_TYPE_BLACK_CITY;
#else   // PM_VERSION == VERSION_BLACK
  p_wk->type    = FIELD_WFBC_CORE_TYPE_WHITE_FOREST; 
#endif  // PM_VERSION == VERSION_BLACK

  // 人を設定する
  for( i=0; i<FIELD_WFBC_INIT_PEOPLE_NUM; i++ )
  {
    // 設定位置と人を選ぶ
    npc_idx = GFUser_GetPublicRand( FIELD_WFBC_NPCID_MAX - i );

    p_people  = WFBC_CORE_GetClearPeopleArray( p_wk->people );
    set_npc_idx = WFBC_CORE_GetNotUseNpcID( p_wk, npc_idx );
    WFBC_CORE_SetUpPeople( p_people, set_npc_idx, cp_mystatus );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  WFBCデータの整合性チェック
 *
 *	@param	cp_wk         ワーク
 *
 *	@retval TRUE    正常
 *	@retval FALSE   不正
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_IsConfomity( const FIELD_WFBC_CORE* cp_wk )
{
  int i;

  GF_ASSERT( cp_wk );
  
  if( (cp_wk->data_in == TRUE) && (cp_wk->type >= FIELD_WFBC_CORE_TYPE_MAX) )
  {
    return FALSE;
  }
  
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsConfomity( &cp_wk->people[i] ) == FALSE )
    {
      return FALSE;
    }
  }

  // OK
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  データ調整  不正データの場合は、正常な情報に書き換えます。
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_Management( FIELD_WFBC_CORE* p_wk )
{
  int i;

  GF_ASSERT( p_wk );
  
  // 一応データがない状態も管理しておく
  if( (p_wk->data_in == FALSE) && (p_wk->type != FIELD_WFBC_CORE_TYPE_MAX) )
  {
    p_wk->type = FIELD_WFBC_CORE_TYPE_MAX;
  }

  // 街タイプの調整
  if( (p_wk->data_in == TRUE) && (p_wk->type >= FIELD_WFBC_CORE_TYPE_MAX) )
  {
    // @TODO 自分か通信相手のROMコードに合わせるべき
    p_wk->type = FIELD_WFBC_CORE_TYPE_BLACK_CITY;
  }

  // 人物情報の調整
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    FIELD_WFBC_CORE_PEOPLE_Management( &p_wk->people[i] );
  }
}

// データの有無   不正データの場合、FALSEを返します。
//----------------------------------------------------------------------------
/**
 *	@brief  データの有無チェック  不正データの場合、FALSEを返します。
 *
 *	@param	cp_wk   ワーク
 *
 *	@retval TRUE  データあり
 *	@retval FALSE データなし
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_IsInData( const FIELD_WFBC_CORE* cp_wk )
{
  GF_ASSERT( cp_wk );
  
  if( !cp_wk->data_in )
  {
    return FALSE;
  }

  if( FIELD_WFBC_CORE_IsConfomity( cp_wk ) == FALSE )
  {
    return FALSE;
  }

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  人の数を返す
 *
 *	@param	cp_wk   ワーク
 *
 *	@return 人の数
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_CORE_GetPeopleNum( const FIELD_WFBC_CORE* cp_wk )
{
  int i;
  int num;
  GF_ASSERT( cp_wk );


  num = 0;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_wk->people[i] ) )
    {
      num ++;
    }
  }
  return num;
}


//----------------------------------------------------------------------------
/**
 *	@brief  機嫌値でソートする
 *
 *	@param	p_wk    ワーク
 *	@param  heapID  ヒープID
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_SortData( FIELD_WFBC_CORE* p_wk, HEAPID heapID )
{
  WFBC_CORE_SortPeopleArray( p_wk->people, heapID );
  WFBC_CORE_SortPeopleArray( p_wk->back_people, heapID );
}


//----------------------------------------------------------------------------
/**
 *	@brief  1日の切り替え管理
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_CalcOneDataStart( FIELD_WFBC_CORE* p_wk )
{
  int i;
  BOOL result;
  
  // 全員のMoodをFIELD_WFBC_MOOD_SUB減らす
  // マスクのクリア
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_wk->people[i] ) )
    {
      // バトル、各種加算を可能とする。
      p_wk->people[i].one_day_msk = FIELD_WFBC_ONEDAY_MSK_INIT;
      
      result = WFBC_CORE_People_AddMood( &p_wk->people[i], FIELD_WFBC_MOOD_SUB );
      if( result )
      {
        // 背面世界側のワークに格納後クリア
        // けつの情報を裏世界に渡す。
        WFBC_CORE_PushPeopleArray( p_wk->back_people, &p_wk->people[i] );
        FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[i] );
      }
    }
  }
  
  
  // 空いているワークをつめる
  WFBC_CORE_PackPeopleArray( p_wk->people );
}

//----------------------------------------------------------------------------
/**
 *	@brief  街に入った時の機嫌値計算
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_CalcMoodInTown( FIELD_WFBC_CORE* p_wk )
{
  int i;
  
  // 全員のMoodをFIELD_WFBC_MOOD_SUB減らす
  // マスクのクリア
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_wk->people[i] ) )
    {
      // 加算チェック
      if( FIELD_WFBC_ONEDAY_MSK_CHECK( p_wk->people[i].one_day_msk, FIELD_WFBC_ONEDAY_MSK_INTOWN ) )
      {
        WFBC_CORE_People_AddMood( &p_wk->people[i], FIELD_WFBC_MOOD_ADD_INTOWN );

        // OFF
        p_wk->people[i].one_day_msk = FIELD_WFBC_ONEDAY_MSK_OFF( p_wk->people[i].one_day_msk, FIELD_WFBC_ONEDAY_MSK_INTOWN );
      }
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  人を足す　押し出し式
 *
 *	@param	p_wk      ワーク  
 *	@param	cp_people 格納する人
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_AddPeople( FIELD_WFBC_CORE* p_wk, const FIELD_WFBC_CORE_PEOPLE* cp_people )
{
  WFBC_CORE_PushPeople( p_wk, cp_people );
}

//----------------------------------------------------------------------------
/**
 *	@brief  人を探す  NPCIDの人
 *
 *	@param	p_wk      ワーク
 *	@param	npc_id    NPC　ID
 *
 *	@retval 人ワーク
 *	@retval NULL    いない
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_CORE_PEOPLE* FIELD_WFBC_CORE_GetNpcIDPeople( FIELD_WFBC_CORE* p_wk, u32 npc_id )
{
  int i;

  GF_ASSERT( p_wk );
  GF_ASSERT( npc_id < FIELD_WFBC_NPCID_MAX );

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_wk->people[i] ) )
    {
      if( p_wk->people[i].npc_id == npc_id )
      {
        return &p_wk->people[i];
      }
    }

    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_wk->back_people[i] ) )
    {
      if( p_wk->back_people[i].npc_id == npc_id )
      {
        return &p_wk->back_people[i];
      }
    }
  }

  return NULL;
}




//-------------------------------------
///	FIELD_WFBC_CORE_PEOPLE用関数
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  人物ワークのクリア
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_Clear( FIELD_WFBC_CORE_PEOPLE* p_wk )
{
  GF_ASSERT( p_wk );

  p_wk->data_in = FALSE;
  p_wk->npc_id  = 0;
  p_wk->mood    = 0;
  p_wk->one_day_msk  = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  情報の整合性チェック
 *
 *	@param	cp_wk   ワーク
 *
 *	@retval TRUE    正常
 *	@retval FALSE   不正
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_PEOPLE_IsConfomity( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  データ調整  不正データの場合は、正常な情報に書き換えます。
 *
 *	@param	p_wk  人物ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_Management( FIELD_WFBC_CORE_PEOPLE* p_wk )
{
  GF_ASSERT( p_wk );

  //
}

//----------------------------------------------------------------------------
/**
 *	@brief  データの有無   不正データの場合、FALSEを返します。
 *
 *	@param	cp_wk   人物ワーク
 *
 *	@retval TRUE    データあり
 *	@retval FALSE   データなし
 */
//-----------------------------------------------------------------------------
BOOL FIELD_WFBC_CORE_PEOPLE_IsInData( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );
  
  if( !cp_wk->data_in )
  {
    return FALSE;
  }
  if( FIELD_WFBC_CORE_PEOPLE_IsConfomity( cp_wk ) == FALSE )
  {
    return FALSE;
  }

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  話しかけたときの計算
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_CalcTalk( FIELD_WFBC_CORE_PEOPLE* p_wk )
{
  if( FIELD_WFBC_ONEDAY_MSK_CHECK( p_wk->one_day_msk, FIELD_WFBC_ONEDAY_MSK_TALK ) )
  {
    // 計算
    WFBC_CORE_People_AddMood( p_wk, FIELD_WFBC_MOOD_ADD_TALK );

    // OFF
    p_wk->one_day_msk = FIELD_WFBC_ONEDAY_MSK_OFF( p_wk->one_day_msk, FIELD_WFBC_ONEDAY_MSK_TALK );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  親データの設定
 *
 *	@param	p_wk        ワーク
 *	@param  cp_mystatus 親の情報
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_SetParentData( FIELD_WFBC_CORE_PEOPLE* p_wk, const MYSTATUS* cp_mystatus )
{
  GF_ASSERT( p_wk );
  GF_ASSERT( cp_mystatus );

  p_wk->parent_id = MyStatus_GetID( cp_mystatus );
  GFL_STD_MemCopy( MyStatus_GetMyName( cp_mystatus ), p_wk->parent, sizeof(STRCODE) * (PERSON_NAME_SIZE + EOM_SIZE) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  親の名前を取得
 *
 *	@param	cp_wk   ワーク
 *	@param	p_buff  バッファ
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_CORE_PEOPLE_GetParentName( const FIELD_WFBC_CORE_PEOPLE* cp_wk, STRCODE* p_buff )
{
  GF_ASSERT( cp_wk );
  GFL_STD_MemCopy( cp_wk->parent, p_buff, sizeof(STRCODE) * (PERSON_NAME_SIZE + EOM_SIZE) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  親のプレイヤーIDを取得
 *
 *	@param	cp_wk   ワーク
 *
 *	@return プレイヤーID
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_CORE_PEOPLE_GetParentID( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->parent_id;
}

//----------------------------------------------------------------------------
/**
 *	@brief  NPCIDを取得
 *
 *	@param	cp_wk   ワーク
 *
 *	@return NPCID
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_CORE_PEOPLE_GetNpcID( const FIELD_WFBC_CORE_PEOPLE* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->npc_id;
}






//-----------------------------------------------------------------------------
/**
 *        private関数
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  まだ、人物データとして保持していない人物のIDX番目を取得する
 *
 *	@param	cp_wk     ワーク
 *	@param	idx       インデックス
 */ 
//-----------------------------------------------------------------------------
static u32 WFBC_CORE_GetNotUseNpcID( const FIELD_WFBC_CORE* cp_wk, int idx )
{
  int i;
  int count;
  BOOL result;

  count = 0;
  for( i=0; i<FIELD_WFBC_NPCID_MAX; i++ )
  {
    result = WFBC_CORE_IsUseNpc( cp_wk, i );
    if( result == FALSE )
    {
      if( count == idx )
      {
        return i;
      }
      count ++;
    }
  }

  // 全部持ってる（ありえない）
  GF_ASSERT( 0 );
  return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  まだ、人物データとして保持していないワークインデックスを求める
 *
 *	@param	cp_wk     ワーク
 *	@param	idx       インデックス
 */
//-----------------------------------------------------------------------------
static u32 WFBC_CORE_GetNotUseNpcIndex( const FIELD_WFBC_CORE* cp_wk, int idx )
{
  int i;
  int count;
  BOOL result;

  count = 0;
  for( i=0; i<FIELD_WFBC_NPCID_MAX; i++ )
  {
    result = FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_wk->people[i] );
    if( result == FALSE )
    {
      if( count == idx )
      {
        return i;
      }
      count ++;
    }
  }

  for( i=0; i<FIELD_WFBC_NPCID_MAX; i++ )
  {
    result = FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_wk->back_people[i] );
    if( result == FALSE )
    {
      if( count == idx )
      {
        return i;
      }
      count ++;
    }
  }

  // 全部入ってます
  GF_ASSERT( 0 );
  return 0;
}

//----------------------------------------------------------------------------
/**
 *	@brieif NPC_IDがワーク内の人物にいるのかチェック
 *
 *	@param	cp_wk     ワーク
 *	@param	npc_id    NPCID
 *
 *	@retval TRUE  いる
 *	@retval FALSE いない
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_CORE_IsUseNpc( const FIELD_WFBC_CORE* cp_wk, int npc_id )
{
  int i;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_wk->people[i] ) )
    {
      if( cp_wk->people[i].npc_id == npc_id )
      {
        return TRUE;
      }
    }
  }

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_wk->back_people[i] ) )
    {
      if( cp_wk->back_people[i].npc_id == npc_id )
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  人物配列の内容を機嫌値でソートする
 *
 *	@param	p_array   配列
 *	@param  heapID    ヒープID
 */
//-----------------------------------------------------------------------------
static void WFBC_CORE_SortPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array, HEAPID heapID )
{
  int i, j;
  BOOL flag;
  FIELD_WFBC_CORE_PEOPLE* p_tmp;

  p_tmp = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( heapID ), sizeof(FIELD_WFBC_CORE_PEOPLE) );


  // 機嫌値の高い順番にバブルソート
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX-1; i++ )
  {
    flag = 0;
    for(j=0; j<FIELD_WFBC_PEOPLE_MAX-1; j++)
    {
      if( p_array[j].mood < p_array[j+1].mood )
      {
        // いれかえ
        GFL_STD_MemCopy( &p_array[j+1], p_tmp, sizeof(FIELD_WFBC_CORE_PEOPLE) );
        GFL_STD_MemCopy( &p_array[j], &p_array[j+1], sizeof(FIELD_WFBC_CORE_PEOPLE) );
        GFL_STD_MemCopy( p_tmp, &p_array[j], sizeof(FIELD_WFBC_CORE_PEOPLE) );
        flag =1;
      }
    }

    if( flag == 0 )
    {
      // 1度も変更がなければソート完了
      break;
    }
  }

  GFL_HEAP_FreeMemory( p_tmp );
}


//----------------------------------------------------------------------------
/**
 *	@brief  人物ワークの内容をつめる
 *
 *	@param	p_array   配列
 */
//-----------------------------------------------------------------------------
static void WFBC_CORE_PackPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array )
{
  int  i, j;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( FIELD_WFBC_CORE_PEOPLE_IsInData( &p_array[i] ) == FALSE )
    {
      for( j=i+1; j<FIELD_WFBC_PEOPLE_MAX; j++ )
      {
        GFL_STD_MemCopy( &p_array[j], &p_array[j-1], sizeof(FIELD_WFBC_CORE_PEOPLE) );
      }
      // 最後のワークをクリア
      FIELD_WFBC_CORE_PEOPLE_Clear( &p_array[FIELD_WFBC_PEOPLE_MAX-1] );
    }
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  人を入れる　一番けつの情報は破棄
 *	        通常世界の配列に追加  通常配列を追い出す形で、裏世界の配列にも格納される
 *
 *	@param	p_wk          ワーク
 *	@param	cp_people     追加する人データ
 */
//-----------------------------------------------------------------------------
static void WFBC_CORE_PushPeople( FIELD_WFBC_CORE* p_wk, const FIELD_WFBC_CORE_PEOPLE* cp_people )
{
  GF_ASSERT( p_wk );
  GF_ASSERT( cp_people );
  
  if( WFBC_CORE_IsPeopleArrayFull( p_wk->people ) == TRUE )
  {
    // けつの情報を裏世界に渡す。
    WFBC_CORE_PushPeopleArray( p_wk->back_people, &p_wk->people[FIELD_WFBC_PEOPLE_MAX - 1] );
    FIELD_WFBC_CORE_PEOPLE_Clear( &p_wk->people[FIELD_WFBC_PEOPLE_MAX - 1] );
  }

  // 通常世界の情報に追加
  WFBC_CORE_PushPeopleArray( p_wk->people, cp_people );
}

//----------------------------------------------------------------------------
/**
 *	@brief  人の配列がいっぱいかチェック
 *
 *	@param	cp_array  配列
 *
 *	@retval   TRUE    いっぱい
 *	@retval   FALSE   まだ余裕がある
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_CORE_IsPeopleArrayFull( const FIELD_WFBC_CORE_PEOPLE* cp_array )
{
  int i;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( !FIELD_WFBC_CORE_PEOPLE_IsInData( &cp_array[i] ) )
    {
      return FALSE;
    }
  }
  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  人配列に人を入れる。  一番けつの情報は破棄
 *
 *	@param	p_array     配列
 *	@param	cp_people   人
 */
//-----------------------------------------------------------------------------
static void WFBC_CORE_PushPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array, const FIELD_WFBC_CORE_PEOPLE* cp_people )
{ 
  int i;
  
  // すべて１つづつずらして、先頭に追加
  for( i=FIELD_WFBC_PEOPLE_MAX-1; i>0; i++ )
  {
    GFL_STD_MemCopy( &p_array[i-1], &p_array[i], sizeof(FIELD_WFBC_CORE_PEOPLE) );
  }
  GFL_STD_MemCopy( cp_people, &p_array[0], sizeof(FIELD_WFBC_CORE_PEOPLE) );
}




//----------------------------------------------------------------------------
/**
 *	@brief  空いてる人物ワークを返す
 *
 *	@param	p_array   配列
 *
 *	@return 空いている人物ワーク
 */
//-----------------------------------------------------------------------------
static FIELD_WFBC_CORE_PEOPLE* WFBC_CORE_GetClearPeopleArray( FIELD_WFBC_CORE_PEOPLE* p_array )
{
  int i;

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( !FIELD_WFBC_CORE_PEOPLE_IsInData( &p_array[i] ) )
    {
      return &p_array[i];
    }
  }
  // 空きがなし
  GF_ASSERT( 0 );
  return &p_array[0]; // フリーズは回避
}



//----------------------------------------------------------------------------
/**
 *	@brief  人物情報をセットアップ
 *
 *	@param	p_people  人物ワーク
 *	@param	npc_id    NPCID
 */
//-----------------------------------------------------------------------------
static void WFBC_CORE_SetUpPeople( FIELD_WFBC_CORE_PEOPLE* p_people, u8 npc_id, const MYSTATUS* cp_mystatus )
{
  p_people->data_in = TRUE;
  p_people->npc_id  = npc_id;
  p_people->mood    = FIELD_WFBC_MOOD_DEFAULT;
  p_people->one_day_msk  = FIELD_WFBC_ONEDAY_MSK_INIT;

  // 親の設定
  FIELD_WFBC_CORE_PEOPLE_SetParentData( p_people, cp_mystatus );
}


//----------------------------------------------------------------------------
/**
 *	@brief  人物ワーク  背面世界に行くべきかチェック
 *
 *	@param	cp_people   人物ワーク
 *
 *	@retval TRUE  いくべき
 *	@retval FALSE いかないべき
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_CORE_People_IsGoBack( const FIELD_WFBC_CORE_PEOPLE* cp_people )
{
  if( cp_people->mood == 0 )
  {
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  機嫌値の計算
 *
 *	@param	p_people  人物ワーク
 *	@param	add       加算  マイナスも可
 *
 *	@retval TRUE    背面世界にいくべき
 *	@retval FALSE   背面世界にいかないべき
 */
//-----------------------------------------------------------------------------
static BOOL WFBC_CORE_People_AddMood( FIELD_WFBC_CORE_PEOPLE* p_people, int add )
{
  int mood = p_people->mood;
  BOOL ret = FALSE;
  
  if( (mood + add) > FIELD_WFBC_MOOD_MAX )
  {
    p_people->mood = FIELD_WFBC_MOOD_MAX;
  }
  else if( (mood + add) < 0 )
  {
    p_people->mood = 0;
    ret = TRUE;
  }
  else
  {
    p_people->mood += add;
  }
  return ret;
}
