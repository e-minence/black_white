//======================================================================
/**
 * @file	mb_sel_local_def.h
 * @brief	�}���`�u�[�g�F�{�b�N�X ��`�Q
 * @author	ariizumi
 * @data	09/12/03
 *
 * ���W���[�����FMB_SEL_
 */
//======================================================================
#pragma once

#include "multiboot/mb_local_def.h"
#include "multiboot/mb_data_def.h"
#include "poke_tool/poke_tool.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define MB_SEL_PLT_OBJ_COMMON (0)   //3�{
#define MB_SEL_PLT_OBJ_POKEICON (3) //3�{
#define MB_SEL_PLT_OBJ_POKEICON_NONE (6) //3�{(�����Ă���Ȃ��Ó]�|�P
#define MB_SEL_PLT_OBJ_BAR_BUTTON (9) //3�{
#define MB_SEL_PLT_OBJ_BOX_ICON (12) //1�{
#define MB_SEL_PLT_OBJ_POKE_MARK (13) //1�{
//12���烁�b�Z�[�W�n


#define MB_SEL_PLT_BG_COMMON (0)   //1�{
#define MB_SEL_PLT_BG_BAR (3)   //1�{

#define MB_SEL_POKE_BOX_TOP  (44)
#define MB_SEL_POKE_BOX_LEFT (24)
#define MB_SEL_POKE_BOX_WIDTH (24)
#define MB_SEL_POKE_BOX_HEIGHT (24)
#define MB_SEL_POKE_BOX_X_NUM  (6)
#define MB_SEL_POKE_BOX_Y_NUM  (5)

#define MB_SEL_POKE_TRAY_TOP  (64)
#define MB_SEL_POKE_TRAY_LEFT (192)
#define MB_SEL_POKE_TRAY_WIDTH (40)
#define MB_SEL_POKE_TRAY_HEIGHT (32)
#define MB_SEL_POKE_TRAY_X_NUM  (2)
#define MB_SEL_POKE_TRAY_Y_NUM  (3)
#define MB_SEL_POKE_TRAY_OFS    (16) //�E�̃I�t�Z�b�g

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  MSCR_PLT_POKEICON,
  MSCR_PLT_POKEICON_NONE,
  MSCR_ANM_POKEICON,

  MSCR_PLT_COMMON,
  MSCR_NCG_COMMON,
  MSCR_ANM_COMMON,

  MSCR_PLT_BAR_ICON,
  MSCR_NCG_BAR_ICON,
  MSCR_ANM_BAR_ICON,
  
  MSCR_PLT_POKE_MARK,
  MSCR_NCG_POKE_MARK,
  MSCR_ANM_POKE_MARK,
  
  MSCR_MAX,
}MB_SEL_CELL_RES;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct _MB_SELECT_WORK MB_SELECT_WORK;
typedef struct _MB_SEL_POKE MB_SEL_POKE;


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

extern ARCHANDLE* MB_SELECT_GetIconArcHandle( MB_SELECT_WORK *work );
extern const DLPLAY_CARD_TYPE MB_SELECT_GetCardType( const MB_SELECT_WORK *work );
extern const HEAPID MB_SELECT_GetHeapId( const MB_SELECT_WORK *work );

//--------------------------------------------------------------
//	
//--------------------------------------------------------------