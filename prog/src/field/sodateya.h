//////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  育て屋さん
 * @file   sodateya.h
 * @author obata
 * @date   2009.10.01
 *
 */
//////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//========================================================================================
// ■関数一覧
//========================================================================================
/*
SODATEYA* SODATEYA_Create( HEAPID heap_id, FIELDMAP_WORK* fieldmap, SODATEYA_WORK* work ); 
void SODATEYA_Delete( SODATEYA* sodateya );
void SODATEYA_TakePokemon( SODATEYA* sodateya, POKEPARTY* party, int pos );
void SODATEYA_TakeBackPokemon( SODATEYA* sodateya, int index, POKEPARTY* party );
void SODATEYA_TakeBackEgg( SODATEYA* sodateya, POKEPARTY* party );
void SODATEYA_DeleteEgg( SODATEYA* sodateya );
void SODATEYA_BreedPokemon( SODATEYA* sodateya );
BOOL SODATEYA_HaveEgg( const SODATEYA* sodateya );
u8 SODATEYA_GetPokemonNum( const SODATEYA* sodateya );
u32 SODATEYA_GetPokeLv_Current( const SODATEYA* sodateya, int index );
u32 SODATEYA_GetPokeLv_GrowUp( const SODATEYA* sodateya, int index );
u32 SODATEYA_GetCharge( const SODATEYA* sodateya, int index );
u8 SODATEYA_GetLoveLevel( const SODATEYA* sodateya );
const POKEMON_PARAM* SODATEYA_GetPokemonParam( const SODATEYA* sodateya, int index );
void POKEMON_EGG_Birth( POKEMON_PARAM* egg, HEAPID heap_id );
*/


//========================================================================================
// ■不完全型宣言
//========================================================================================
typedef struct _SODATEYA SODATEYA;


//========================================================================================
// ■作成・破棄
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief 育て屋さんを作成する
 *
 * @param heap_id  使用するヒープID
 * @param fieldmap 関連付けるフィールドマップ
 * @param work     管理する育て屋ワーク
 * @return 作成した育て屋さん
 */
//---------------------------------------------------------------------------------------- 
extern SODATEYA* SODATEYA_Create( HEAPID heap_id, 
                                  FIELDMAP_WORK* fieldmap, SODATEYA_WORK* work );

//----------------------------------------------------------------------------------------
/**
 * @brief 育て屋さんを破棄する
 *
 * @param sodateya 破棄する育て屋さん
 */
//----------------------------------------------------------------------------------------
extern void SODATEYA_Delete( SODATEYA* sodateya );


//========================================================================================
// ■ポケモン, タマゴの受け渡し
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief ポケモンを預かる
 *
 * @param sodateya 育て屋さん
 * @param party    預かるポケモンが属するパーティー
 * @param pos      預かるポケモンを指定
 */
//---------------------------------------------------------------------------------------- 
extern void SODATEYA_TakePokemon( SODATEYA* sodateya, POKEPARTY* party, int pos );

//----------------------------------------------------------------------------------------
/**
 * @brief ポケモンを返す
 *
 * @param sodateya 育て屋さん
 * @param index    返すポケモンを指定
 * @param party    返し先のパーティー
 */
//---------------------------------------------------------------------------------------- 
extern void SODATEYA_TakeBackPokemon( SODATEYA* sodateya, int index, POKEPARTY* party );

//----------------------------------------------------------------------------------------
/**
 * @brief タマゴを渡す
 *
 * @param sodateya 育て屋さん
 * @param party    渡す先のパーティー
 */
//---------------------------------------------------------------------------------------- 
extern void SODATEYA_TakeBackEgg( SODATEYA* sodateya, POKEPARTY* party );

//----------------------------------------------------------------------------------------
/**
 * @brief タマゴを破棄する
 *
 * @param sodateya 育て屋さん
 */
//---------------------------------------------------------------------------------------- 
extern void SODATEYA_DeleteEgg( SODATEYA* sodateya );


//========================================================================================
// ■育て屋さん仕事関数
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @brief ポケモンを育てる (1歩分)
 *
 * @param sodateya ポケモンを育てる育て屋さん
 * @retval  BOOL  TRUEのとき、タマゴが生まれた
 */
//---------------------------------------------------------------------------------------- 
extern BOOL SODATEYA_BreedPokemon( SODATEYA* sodateya );


//========================================================================================
// ■取得関数
//========================================================================================

//----------------------------------------------------------------------------------------
/**
 * @breif タマゴの有無を取得する
 *
 * @param sodateya 育て屋さん
 * @return タマゴがある場合 TRUE
 */
//----------------------------------------------------------------------------------------
extern BOOL SODATEYA_HaveEgg( const SODATEYA* sodateya );

//----------------------------------------------------------------------------------------
/**
 * @breif 飼育中のポケモンの数を取得する
 *
 * @param sodateya 育て屋さん
 * @return 飼育中のポケモン数
 */
//----------------------------------------------------------------------------------------
extern u8 SODATEYA_GetPokemonNum( const SODATEYA* sodateya );

//----------------------------------------------------------------------------------------
/**
 * @brief 飼育ポケモンについて, 現在のレベルを取得する
 *
 * @param sodateya 育て屋さん
 * @param index    対象となるポケモンを指定
 * @return 指定ポケモンの現時点のレベル
 */
//---------------------------------------------------------------------------------------- 
extern u32 SODATEYA_GetPokeLv_Current( const SODATEYA* sodateya, int index );

//----------------------------------------------------------------------------------------
/**
 * @brief 飼育ポケモンについて, 育ったレベルを取得する
 *
 * @param sodateya 育て屋さん
 * @param index    対象となるポケモンを指定
 * @return 指定ポケモンの成長レベル数
 */
//---------------------------------------------------------------------------------------- 
extern u32 SODATEYA_GetPokeLv_GrowUp( const SODATEYA* sodateya, int index );

//----------------------------------------------------------------------------------------
/**
 * @brief 飼育ポケモンの引き取り料金を取得する
 *
 * @param sodateya 育て屋さん
 * @param index    対象となるポケモンを指定
 * @return 指定ポケモンの引き取り金額
 */
//---------------------------------------------------------------------------------------- 
extern u32 SODATEYA_GetCharge( const SODATEYA* sodateya, int index );

//----------------------------------------------------------------------------------------
/**
 * @brief 飼育ポケモンの相性を取得する
 *
 * @param sodateya 育て屋さん
 * @return 飼育ポケモン同士の相性[%]
 */
//---------------------------------------------------------------------------------------- 
extern u8 SODATEYA_GetLoveLevel( const SODATEYA* sodateya );

//----------------------------------------------------------------------------------------
/**
 * @brief 飼育ポケモンの預かった当時のパラメータを取得する
 *
 * @param sodateya 育て屋さん
 * @param index    対象となるポケモンを指定
 * @return 指定ポケモンの飼育開始当初のパラメータ
 */
//---------------------------------------------------------------------------------------- 
extern const POKEMON_PARAM* SODATEYA_GetPokemonParam( const SODATEYA* sodateya, int index );



//========================================================================================
// ■タマゴの孵化
//========================================================================================

//---------------------------------------------------------------------------------------- 
/**
 * @brief タマゴを孵化させる
 *
 * @param egg     孵化させるタマゴ
 * @param owner   タマゴの所持者
 * @param heap_id 使用するヒープID
 */
//---------------------------------------------------------------------------------------- 
extern void POKEMON_EGG_Birth( POKEMON_PARAM* egg, const MYSTATUS* owner, HEAPID heap_id );
