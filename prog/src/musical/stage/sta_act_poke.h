//======================================================================
/**
 * @file	sta_act_poke.h
 * @brief	�X�e�[�W�p�@�|�P����
 * @author	ariizumi
 * @data	09/03/06
 */
//======================================================================
#ifndef STA_ACT_POKE_H__
#define STA_ACT_POKE_H__

#include "musical/mus_poke_draw.h"
#include "musical/mus_item_draw.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================
//�|�P�����̌���(�o�g���x�[�X�Ȃ̂Ńf�t�H�͍�����
typedef enum
{
	SPD_LEFT,	//������
	SPD_RIGHT,	//�E����
}STA_POKE_DIR;


//======================================================================
//	typedef struct
//======================================================================

typedef struct _STA_POKE_SYS STA_POKE_SYS;
typedef struct _STA_POKE_WORK STA_POKE_WORK;

//======================================================================
//	proto
//======================================================================

STA_POKE_SYS* STA_POKE_InitSystem( HEAPID heapId , ACTING_WORK *actWork , MUS_POKE_DRAW_SYSTEM* drawSys , MUS_ITEM_DRAW_SYSTEM *itemDrawSys , GFL_BBD_SYS* bbdSys );
//�|�P�����̕`��̌�ɃA�C�e���̍X�V������K�v������̂ŕʊ֐���
void	STA_POKE_UpdateSystem( STA_POKE_SYS *work );
void	STA_POKE_UpdateSystem_Item( STA_POKE_SYS *work );
void	STA_POKE_DrawSystem( STA_POKE_SYS *work );
void	STA_POKE_ExitSystem( STA_POKE_SYS *work );

//�V�X�e���n
void	STA_POKE_System_SetScrollOffset( STA_POKE_SYS *work , const u16 scrOfs );


//�|�P�P�̌n
STA_POKE_WORK *STA_POKE_CreatePoke( STA_POKE_SYS *work , MUSICAL_POKE_PARAM *musPoke );
void STA_POKE_DeletePoke( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
void STA_POKE_GetPosition( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , VecFx32 *pos );
void STA_POKE_SetPosition( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , const VecFx32 *pos );
void STA_POKE_GetRotate( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , u16 *rotate );
void STA_POKE_SetRotate( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , const u16 rotate );
void STA_POKE_GetScale( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , VecFx32 *scale );
void STA_POKE_SetScale( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , VecFx32 *scale );
void STA_POKE_GetPositionOffset( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , VecFx32 *pos );
void STA_POKE_SetPositionOffset( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , const VecFx32 *pos );
void STA_POKE_StartAnime( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
void STA_POKE_StopAnime( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
void STA_POKE_ChangeAnime( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , const u8 anmIdx );
void STA_POKE_SetShowFlg( STA_POKE_SYS *work , STA_POKE_WORK *drawWork , const BOOL flg );
BOOL STA_POKE_GetShowFlg( STA_POKE_SYS *work , STA_POKE_WORK *drawWork );
STA_POKE_DIR STA_POKE_GetPokeDir( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
void STA_POKE_SetPokeDir( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , const STA_POKE_DIR dir );
void STA_POKE_SetFrontBack( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , const BOOL isFront );
void STA_POKE_SetDrawItem( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , const BOOL isDrawItem );
const VecFx32* STA_POKE_GetRotOffset( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );

//�A�C�e���g�p
void STA_POKE_UseItemFunc( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , MUS_POKE_EQUIP_POS ePos );


#endif STA_ACT_POKE_H__

