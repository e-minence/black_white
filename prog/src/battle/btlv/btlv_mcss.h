
//============================================================================================
/**
 * @file	btlv_mcss.h
 * @brief	ポケモンMCSS管理
 * @author	soga
 * @date	2008.11.14
 */
//============================================================================================

#include "system/mcss.h"
#include "poke_tool/poke_tool.h"

#ifndef	__BTLV_MCSS_H_
#define	__BTLV_MCSS_H_

//ポケモンの立ち位置定義
// 1vs1
//		BB
//
// AA
//
// 2vs2
//		D B
//
// A C
// 3vs3
//		 F
//		D B
//
// A C
//  E
typedef enum{
	BTLV_MCSS_POS_AA = 0,
	BTLV_MCSS_POS_BB,
	BTLV_MCSS_POS_A,
	BTLV_MCSS_POS_B,
	BTLV_MCSS_POS_C,
	BTLV_MCSS_POS_D,
	BTLV_MCSS_POS_E,
	BTLV_MCSS_POS_F,
	
	BTLV_MCSS_POS_MAX,

	BTLV_MCSS_POS_PAIR_BIT = 0x02,
	BTLV_MCSS_POS_ERROR = 0xff
}BtlvMcssPos;

#define	BTLV_MCSS_MEPACHI_ON	( MCSS_MEPACHI_ON )
#define	BTLV_MCSS_MEPACHI_OFF	( MCSS_MEPACHI_OFF )
#define	BTLV_MCSS_ANM_STOP_ON	( MCSS_ANM_STOP_ON )
#define	BTLV_MCSS_ANM_STOP_OFF	( MCSS_ANM_STOP_OFF )
#define	BTLV_MCSS_VANISH_ON		( MCSS_VANISH_ON )
#define	BTLV_MCSS_VANISH_OFF	( MCSS_VANISH_OFF )

typedef struct _BTLV_MCSS_WORK BTLV_MCSS_WORK;

extern	BTLV_MCSS_WORK	*BTLV_MCSS_Init( GFL_TCBSYS *tcb_sys, HEAPID heapID );
extern	void			BTLV_MCSS_Exit( BTLV_MCSS_WORK *bmw );
extern	void			BTLV_MCSS_Main( BTLV_MCSS_WORK *bmw );
extern	void			BTLV_MCSS_Draw( BTLV_MCSS_WORK *bmw );
extern	void			BTLV_MCSS_Add( BTLV_MCSS_WORK *bmw, const POKEMON_PARAM *pp, int position );
extern	void			BTLV_MCSS_Del( BTLV_MCSS_WORK *bmw, int position );
extern	void			BTLV_MCSS_SetOrthoMode( BTLV_MCSS_WORK *bmw );
extern	void			BTLV_MCSS_ResetOrthoMode( BTLV_MCSS_WORK *bmw );
extern	void			BTLV_MCSS_SetMepachiFlag( BTLV_MCSS_WORK *bmw, int position, int flag );
extern	void			BTLV_MCSS_SetAnmStopFlag( BTLV_MCSS_WORK *bmw, int position, int flag );
extern	int				BTLV_MCSS_GetVanishFlag( BTLV_MCSS_WORK *bmw, int position );
extern	void			BTLV_MCSS_SetVanishFlag( BTLV_MCSS_WORK *bmw, int position, int flag );
extern	void			BTLV_MCSS_GetPokeDefaultPos( VecFx32 *pos, int position );
extern	fx32			BTLV_MCSS_GetPokeDefaultScale( BTLV_MCSS_WORK *bmw, int position );
extern	void			BTLV_MCSS_GetScale( BTLV_MCSS_WORK *bmw, int position, VecFx32 *scale );
extern	void			BTLV_MCSS_SetScale( BTLV_MCSS_WORK *bmw, int position, VecFx32 *scale );
extern	void			BTLV_MCSS_MovePosition( BTLV_MCSS_WORK *bmw, int position, int move_type, VecFx32 *pos, int frame, int wait, int count );
extern	void			BTLV_MCSS_MoveScale( BTLV_MCSS_WORK *bmw, int position, int move_type, VecFx32 *scale, int frame, int wait, int count );
extern	BOOL			BTLV_MCSS_CheckTCBExecute( BTLV_MCSS_WORK *bmw, int position );
extern	BOOL			BTLV_MCSS_CheckExistPokemon( BTLV_MCSS_WORK *bmw, int position );

#endif	//__BTLV_MCSS_H_
