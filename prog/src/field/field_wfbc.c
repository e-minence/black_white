//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_wfbc.c
 *	@brief  White Forest  Black City 
 *	@author	tomoya takahashi
 *	@date		2009.11.09
 *
 *	モジュール名：FIELD_WFBC
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "field_wfbc.h"

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	人物情報
//=====================================
struct _FIELD_WFBC_PEOPLE 
{
  u16   status;   // FIELD_WFBC_PEOPLE_STATUS
  u16   npcid;    // 人物ID
  u16   objcode;  // 見た目
  u16   pad;      // 
};

//-------------------------------------
///	WFBCワーク
//=====================================
struct _FIELD_WFBC 
{
  FIELD_WFBC_CORE_TYPE type;
  FIELD_WFBC_PEOPLE people[ FIELD_WFBC_PEOPLE_MAX ];
};

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	FIELD_WFBC
//=====================================
static void WFBC_Clear( FIELD_WFBC* p_wk );
static int WFBC_GetClearPeople( const FIELD_WFBC* cp_wk );
static int WFBC_GetPeopleNum( const FIELD_WFBC* cp_wk );

static void WFBC_SetCore( FIELD_WFBC* p_wk, const FIELD_WFBC_CORE* cp_buff );
static void WFBC_GetCore( const FIELD_WFBC* cp_wk, FIELD_WFBC_CORE* p_buff );

//-------------------------------------
///	FIELD_WFBC_PEOPLE
//=====================================
static void WFBC_PEOPLE_Clear( FIELD_WFBC_PEOPLE* p_people );
static void WFBC_PEOPLE_SetUp( FIELD_WFBC_PEOPLE* p_people, const FIELD_WFBC_CORE_PEOPLE* cp_core );
static void WFBC_PEOPLE_GetCore( const FIELD_WFBC_PEOPLE* cp_people, FIELD_WFBC_CORE_PEOPLE* p_core );


//----------------------------------------------------------------------------
/**
 *	@brief  WFBC用ワークの生成
 *
 *	@param	heapID  ヒープID
 *
 *	@return WFBCワーク
 */
//-----------------------------------------------------------------------------
FIELD_WFBC* FIELD_WFBC_Create( HEAPID heapID )
{
  FIELD_WFBC* p_wk;

  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_WFBC) );

  WFBC_Clear( p_wk );

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBC用ワークの破棄
 *
 *	@param	p_wk  ワーク
 */ 
//-----------------------------------------------------------------------------
void FIELD_WFBC_Delete( FIELD_WFBC* p_wk )
{
  GF_ASSERT( p_wk );

  GFL_HEAP_FreeMemory( p_wk );
}


//----------------------------------------------------------------------------
/**
 *	@brief  WFBC用高さテーブルインデックスを返す
 *
 *	@param	cp_wk   ワーク
 *
 *	@return 高さテーブルのインデックス
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_GetPeopleNum( const FIELD_WFBC* cp_wk )
{
  GF_ASSERT( cp_wk );

  // 人数を数える
  return WFBC_GetPeopleNum( cp_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBCCOREデータを取得する
 *
 *	@param	cp_wk   ワーク
 *	@param	p_buff  格納バッファ
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_GetCore( const FIELD_WFBC* cp_wk, FIELD_WFBC_CORE* p_buff )
{
  WFBC_GetCore( cp_wk, p_buff );
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物情報の取得
 *
 *	@param	cp_wk   ワーク
 *	@param	index   インデックス
 *
 *	@return 人物情報
 */
//-----------------------------------------------------------------------------
const FIELD_WFBC_PEOPLE* FIELD_WFBC_GetPeople( const FIELD_WFBC* cp_wk, int index )
{
  GF_ASSERT( index < FIELD_WFBC_PEOPLE_MAX );
  GF_ASSERT( cp_wk );
  
  return &cp_wk->people[index];
}

//----------------------------------------------------------------------------
/**
 *	@brief  WFBCの街タイプを取得
 *
 *	@param	cp_wk   ワーク
 *
 *	@return 街タイプ
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_CORE_TYPE FIELD_WFBC_GetType( const FIELD_WFBC* cp_wk )
{
  GF_ASSERT( cp_wk );
  return cp_wk->type;
}

//----------------------------------------------------------------------------
/**
 *	@brief  街情報を設定
 *
 *	@param	p_wk        ワーク
 *	@param	cp_core     コア情報
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_SetUp( FIELD_WFBC* p_wk, const FIELD_WFBC_CORE* cp_core )
{
  WFBC_SetCore( p_wk, cp_core );
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物の追加
 *
 *	@param	p_wk    ワーク
 *	@param	objcode オブジェコード
 *
 *	@retval 追加したインデックス
 */
//-----------------------------------------------------------------------------
int FIELD_WFBC_AddPeople( FIELD_WFBC* p_wk, const FIELD_WFBC_CORE_PEOPLE* cp_core )
{
  int index;

  GF_ASSERT( p_wk );
  // 空いているワークを取得
  index = WFBC_GetClearPeople( p_wk );

  // 人情報をセットアップ
  WFBC_PEOPLE_SetUp( &p_wk->people[index], cp_core );

  return index;
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物の破棄
 *
 *	@param	p_wk    ワーク
 *	@param	index   破棄する人物のインデックス
 */
//-----------------------------------------------------------------------------
void FIELD_WFBC_DeletePeople( FIELD_WFBC* p_wk, int index )
{
  GF_ASSERT( p_wk );
  GF_ASSERT( index < FIELD_WFBC_PEOPLE_MAX );
  
  // インデックスのワークをクリーン
  WFBC_PEOPLE_Clear( &p_wk->people[index] );
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物　見た目の取得
 *
 *	@param	cp_people   ワーク
 *
 *	@return 見た目
 */
//-----------------------------------------------------------------------------
u32 FIELD_WFBC_PEOPLE_GetOBJCode( const FIELD_WFBC_PEOPLE* cp_people )
{
  GF_ASSERT( cp_people );
  return cp_people->objcode;
}





//-----------------------------------------------------------------------------
/**
 *      private関数
 */
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  WFBCワークをクリア
 */
//-----------------------------------------------------------------------------
static void WFBC_Clear( FIELD_WFBC* p_wk )
{
  int i;

  GFL_STD_MemClear( p_wk, sizeof(FIELD_WFBC) );

  //　人物ワークをクリア
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    WFBC_PEOPLE_Clear( &p_wk->people[i] );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  空いている人物ワークを取得する
 *
 *	@param	cp_wk     ワーク
 *
 *	@return 空いているワークのインデックス
 */
//-----------------------------------------------------------------------------
static int WFBC_GetClearPeople( const FIELD_WFBC* cp_wk )
{
  int i;

  GF_ASSERT( cp_wk );

  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( cp_wk->people[i].status != FIELD_WFBC_PEOPLE_STATUS_NONE )
    {
      return i;
    }
  }

  GF_ASSERT_MSG( 0, "WFBC People Over\n" );
  return 0; // メモリ破壊などがないように０を返す
}

//----------------------------------------------------------------------------
/**
 *	@brief  人物の数を取得する
 *
 *	@param	cp_wk   ワーク
 *
 *	@return 人物の数
 */
//-----------------------------------------------------------------------------
static int WFBC_GetPeopleNum( const FIELD_WFBC* cp_wk )
{
  int num;
  int i;
  
  GF_ASSERT( cp_wk );

  num = 0;
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    if( cp_wk->people[i].status != FIELD_WFBC_PEOPLE_STATUS_NONE )
    {
      num ++;
    }
  }

  return num;
}


//----------------------------------------------------------------------------
/**
 *	@brief  CORE情報を設定する
 *
 *	@param	p_wk        ワーク
 *	@param	cp_buff     バッファ
 */
//-----------------------------------------------------------------------------
static void WFBC_SetCore( FIELD_WFBC* p_wk, const FIELD_WFBC_CORE* cp_buff )
{
  int i;
  BOOL result;

  GF_ASSERT( p_wk );
  GF_ASSERT( cp_buff );

  result = FIELD_WFBC_CORE_IsInData( cp_buff );
  GF_ASSERT( result );
  
  // 現状方WFBC情報をクリーンアップ
  WFBC_Clear( p_wk );

  // 情報を設定
  p_wk->type = cp_buff->type;
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    // データがあったら設定
    if( cp_buff->people[i].data_in )
    {
      WFBC_PEOPLE_SetUp( &p_wk->people[i], &cp_buff->people[i] );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  CORE情報を取得する
 *
 *	@param	cp_wk     ワーク
 *	@param	p_buff    データ格納先
 */
//-----------------------------------------------------------------------------
static void WFBC_GetCore( const FIELD_WFBC* cp_wk, FIELD_WFBC_CORE* p_buff )
{
  int i;

  GF_ASSERT( cp_wk );
  GF_ASSERT( p_buff );

  // 情報のクリア
  FIELD_WFBC_CORE_Crear( p_buff );
  
  // 情報を格納
  p_buff->type = cp_wk->type;
  for( i=0; i<FIELD_WFBC_PEOPLE_MAX; i++ )
  {
    // データがあったら設定
    WFBC_PEOPLE_GetCore( &cp_wk->people[i], &p_buff->people[i] );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  人物ワークのクリア
 *
 *	@param	p_people  人物
 */
//-----------------------------------------------------------------------------
static void WFBC_PEOPLE_Clear( FIELD_WFBC_PEOPLE* p_people )
{
  p_people->status  = FIELD_WFBC_PEOPLE_STATUS_NONE;
  p_people->objcode = 0;
}


//----------------------------------------------------------------------------
/**
 *	@brief  人情報のセットアップ
 *
 *	@param	p_people    人物ワーク
 *	@param	cp_core     人物CORE情報
 */
//-----------------------------------------------------------------------------
static void WFBC_PEOPLE_SetUp( FIELD_WFBC_PEOPLE* p_people, const FIELD_WFBC_CORE_PEOPLE* cp_core )
{
  BOOL result;
  
  GF_ASSERT( p_people );
  // 正常な情報かチェック
  result = FIELD_WFBC_CORE_PEOPLE_IsInData( cp_core );
  GF_ASSERT( result == TRUE );

  p_people->status  = FIELD_WFBC_PEOPLE_STATUS_NORMAL;
  p_people->npcid   = cp_core->npc_id;

  // NPCIDの基本情報を読み込む
  p_people->objcode = 0;

  // 拡張データを設定する
}


//----------------------------------------------------------------------------
/**
 *	@brief  人物CORE情報の取得
 *
 *	@param	cp_people   人物ワーク
 *	@param	p_core      情報格納先
 */
//-----------------------------------------------------------------------------
static void WFBC_PEOPLE_GetCore( const FIELD_WFBC_PEOPLE* cp_people, FIELD_WFBC_CORE_PEOPLE* p_core )
{
  GF_ASSERT( cp_people );
  GF_ASSERT( p_core );

  FIELD_WFBC_CORE_PEOPLE_Crear( p_core );

  if( cp_people->status == FIELD_WFBC_PEOPLE_STATUS_NONE )
  {
    // 情報なし
    return ;
  }

  p_core->data_in = TRUE;
  p_core->npc_id  = cp_people->npcid;
}





//デバッグ操作
#ifdef PM_DEBUG
static void DEBWIN_Update_CityLevel( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_CityLevel( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Update_CityType( void* userWork , DEBUGWIN_ITEM* item );
static void DEBWIN_Draw_CityType( void* userWork , DEBUGWIN_ITEM* item );


void FIELD_FUNC_RANDOM_GENERATE_InitDebug( HEAPID heapId )
{
  DEBUGWIN_AddGroupToTop( 10 , "RandomMap" , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_CityLevel ,DEBWIN_Draw_CityLevel , 
                             NULL , 10 , heapId );
  DEBUGWIN_AddItemToGroupEx( DEBWIN_Update_CityType ,DEBWIN_Draw_CityType , 
                             NULL , 10 , heapId );
}

void FIELD_FUNC_RANDOM_GENERATE_TermDebug( void )
{
  DEBUGWIN_RemoveGroup( 10 );
}

static void DEBWIN_Update_CityLevel( void* userWork , DEBUGWIN_ITEM* item )
{
#if 0
  SAVE_CONTROL_WORK *saveWork = SaveControl_GetPointer();
  RANDOMMAP_SAVE* mapSave = RANDOMMAP_SAVE_GetRandomMapSave( saveWork );
  u16 level = RANDOMMAP_SAVE_GetCityLevel( mapSave );
  
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    if( level < 20 )
    {
      level++;
      RANDOMMAP_SAVE_SetCityLevel( mapSave , level );
      DEBUGWIN_RefreshScreen();
    }
  }
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  {
    if( level > 0 )
    {
      level--;
      RANDOMMAP_SAVE_SetCityLevel( mapSave , level );
      DEBUGWIN_RefreshScreen();
    }
  }
#endif
}

static void DEBWIN_Draw_CityLevel( void* userWork , DEBUGWIN_ITEM* item )
{
#if 0
  SAVE_CONTROL_WORK *saveWork = SaveControl_GetPointer();
  RANDOMMAP_SAVE* mapSave = RANDOMMAP_SAVE_GetRandomMapSave( saveWork );
  u16 level = RANDOMMAP_SAVE_GetCityLevel( mapSave );
  DEBUGWIN_ITEM_SetNameV( item , "Level[%d]",level );
#endif
}

static void DEBWIN_Update_CityType( void* userWork , DEBUGWIN_ITEM* item )
{
#if 0
  SAVE_CONTROL_WORK *saveWork = SaveControl_GetPointer();
  RANDOMMAP_SAVE* mapSave = RANDOMMAP_SAVE_GetRandomMapSave( saveWork );
  u8 type = RANDOMMAP_SAVE_GetCityType( mapSave );
  
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT || 
      GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT  ||
      GFL_UI_KEY_GetRepeat() & PAD_BUTTON_A )
  {
    if( type == RMT_BLACK_CITY )
    {
      type = RMT_WHITE_FOREST;
    }
    else
    {
      type = RMT_BLACK_CITY;
    }
    RANDOMMAP_SAVE_SetCityType( mapSave , type );
    DEBUGWIN_RefreshScreen();
  }
#endif 
}

static void DEBWIN_Draw_CityType( void* userWork , DEBUGWIN_ITEM* item )
{
#if 0
  SAVE_CONTROL_WORK *saveWork = SaveControl_GetPointer();
  RANDOMMAP_SAVE* mapSave = RANDOMMAP_SAVE_GetRandomMapSave( saveWork );
  u8 type = RANDOMMAP_SAVE_GetCityType( mapSave );
  if( type == RMT_BLACK_CITY )
  {
    DEBUGWIN_ITEM_SetName( item , "Type[BLACK CITY]" );
  }
  else
  {
    DEBUGWIN_ITEM_SetName( item , "Type[WHITE FOREST]" );
  }
#endif
}
#endif
