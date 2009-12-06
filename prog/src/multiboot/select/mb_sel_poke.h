//======================================================================
/**
 * @file	mb_sel_poke.h
 * @brief	�}���`�u�[�g�F�{�b�N�X�E�|�P����
 * @author	ariizumi
 * @data	09/12/03
 *
 * ���W���[�����FMB_SEL_POKE
 */
//======================================================================
#pragma once

#include "./mb_sel_local_def.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define MB_SEL_POKE_BG_PRI_BOX (1)
#define MB_SEL_POKE_BG_PRI_TRAY (0)
#define MB_SEL_POKE_BG_PRI_HOLD (0)
#define MB_SEL_POKE_SOFT_PRI_BOX (16)
#define MB_SEL_POKE_SOFT_PRI_TRAY (0)
#define MB_SEL_POKE_SOFT_PRI_HOLD (0)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//�A�C�R���̎��(�{�b�N�X���g���C��
typedef enum
{
  MSPT_BOX,
  MSPT_TRAY,
  
  MSPT_HOLD,  //�ێ���(�v���C�I���e�B�ݒ��
  
}MB_SEL_POKE_TYPE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  HEAPID heapId;
  ARCHANDLE *arcHandle;
  
  MB_SEL_POKE_TYPE iconType;
  u8               idx;
  
  GFL_CLUNIT  *cellUnit;
  u32   palResIdx;
  u32   anmResIdx;
  
}MB_SEL_POKE_INIT_WORK;


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
extern MB_SEL_POKE* MB_SEL_POKE_CreateWork( MB_SELECT_WORK *selWork , MB_SEL_POKE_INIT_WORK *initWork );
extern void MB_SEL_POKE_DeleteWork( MB_SELECT_WORK *selWork , MB_SEL_POKE *pokeWork );
extern const BOOL  MB_SEL_POKE_UpdateWork( MB_SELECT_WORK *selWork , MB_SEL_POKE *pokeWork );

extern void MB_SEL_POKE_SetPPP( MB_SELECT_WORK *selWork , MB_SEL_POKE *pokeWork , POKEMON_PASO_PARAM *ppp );
extern void MB_SEL_POKE_SetPos( MB_SELECT_WORK *selWork , MB_SEL_POKE *pokeWork , GFL_CLACTPOS *pos );
extern void MB_SEL_POKE_SetMove( MB_SELECT_WORK *selWork , 
                                 MB_SEL_POKE *pokeWork , 
                                 int startX , int startY , 
                                 int endX , int endY , 
                                 const u8 cnt , const BOOL autoDel );
extern void MB_SEL_POKE_SetPri( MB_SELECT_WORK *selWork , MB_SEL_POKE *pokeWork , const MB_SEL_POKE_TYPE type );

extern const int MB_SEL_POKE_GetPosX( MB_SEL_POKE *pokeWork );
extern const int MB_SEL_POKE_GetPosY( MB_SEL_POKE *pokeWork );
extern const BOOL MB_SEL_POKE_isValid( const MB_SEL_POKE *pokeWork );
extern const MB_SEL_POKE_TYPE MB_SEL_POKE_GetType( const MB_SEL_POKE *pokeWork );
extern void MB_SEL_POKE_SetIdx( MB_SEL_POKE *pokeWork , u8 idx );
extern const u8 MB_SEL_POKE_GetIdx( const MB_SEL_POKE *pokeWork );
