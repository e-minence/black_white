//======================================================================
/**
 * @file	field_sound_proc.c
 * @brief	フィールドのサウンド関連 プロセス
 * @author	kagaya
 * @data	05.07.13
 */
//======================================================================
#pragma once
#include <gflib.h>

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct  
//======================================================================
///FIELD_SOUND
typedef struct _TAG_FIELD_SOUND FIELD_SOUND;

//======================================================================
//  extern
//======================================================================
extern FIELD_SOUND * FIELD_SOUND_Create( HEAPID heapID );
extern void FIELD_SOUND_Delete( FIELD_SOUND *fsnd );
