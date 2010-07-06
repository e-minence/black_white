//============================================================================================
/**
 * @file	worldtrade_data.h
 * @brief
 * @date	2006.04.30
 * @author	mori GameFreak inc.
 */
//============================================================================================

#ifndef	__WORLDTRADE_DATA_H__
#define	__WORLDTRADE_DATA_H__

#include "savedata/save_control.h"	//SAVE_CONTROL_WORK�Q�Ƃ̂���
#include "poke_tool/poke_tool.h"
#include "system/gf_date.h"

#define RETURN_POSITION_TEMOTI	( 0xFF )

//����220�Ȃ̂ő���Ă���
#define WORLDTRADE_POKEMON_DATA_SIZE  (236)

//----------------------------------------------------------
/**
 * @brief	WORLDTRADE_DATA �ւ̕s���S�^��`
 */
//----------------------------------------------------------
typedef struct WORLDTRADE_DATA WORLDTRADE_DATA;

//���E�����f�[�^�̎擾
extern WORLDTRADE_DATA * SaveData_GetWorldTradeData( SAVE_CONTROL_WORK *sv );

extern int   WorldTradeData_GetWorkSize(void);
extern void  WorldTradeData_Init(WORLDTRADE_DATA *WorldTradeData );
extern int   WorldTradeData_GetFlag( WORLDTRADE_DATA* WorldTradeData );
extern void  WorldTradeData_SetFlag( WORLDTRADE_DATA* WorldTradeData, int flag );
extern void  WorldTradeData_GetPokemonData( WORLDTRADE_DATA* WorldTradeData, POKEMON_PARAM *poke );
extern POKEMON_PARAM *WorldTradeData_GetPokemonDataPtr( WORLDTRADE_DATA* WorldTradeData );
extern void  WorldTradeData_SetPokemonData( WORLDTRADE_DATA* WorldTradeData, POKEMON_PARAM *poke, int box );
extern GFDATE  WorldTradeData_GetLastDate_Deposit( WORLDTRADE_DATA* WorldTradeData );
extern void  WorldTradeData_SetLastDate_Deposit( WORLDTRADE_DATA* WorldTradeData, GFDATE date );
extern GFDATE  WorldTradeData_GetLastDate_Search( WORLDTRADE_DATA* WorldTradeData );
extern void  WorldTradeData_SetLastDate_Search( WORLDTRADE_DATA* WorldTradeData, GFDATE date );
extern int   WorldTradeData_GetBoxNo( WORLDTRADE_DATA* WorldTradeData );


#endif	/* __WORLDTRADE_DATA_H__ */

