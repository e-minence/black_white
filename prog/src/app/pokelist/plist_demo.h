//======================================================================
/**
 * @file	plist_demo.c
 * @brief	�|�P�������X�g�F�t�H�����`�F���W���o
 * @author	ariizumi
 * @data	10/02/22
 *
 * ���W���[�����FPLIST_DEMO
 */
//======================================================================
#pragma once

#include "plist_local_def.h"
//======================================================================
//	define
//======================================================================
#pragma mark [> define

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
extern void PLIST_DEMO_DemoInit( PLIST_WORK *work );
extern void PLIST_DEMO_DemoTerm( PLIST_WORK *work );
extern void PLIST_DEMO_DemoMain( PLIST_WORK *work );

//�{����DEMO����Ȃ����ǁA�����ɂ����B
extern const BOOL PLIST_DEMO_CheckGirathnaToOrigin( PLIST_WORK *work , POKEMON_PARAM *pp );
extern void PLIST_DEMO_ChangeGirathinaToOrigin( PLIST_WORK *work , POKEMON_PARAM *pp );
extern const BOOL PLIST_DEMO_CheckGirathnaToAnother( PLIST_WORK *work , POKEMON_PARAM *pp );
extern void PLIST_DEMO_ChangeGirathinaToAnother( PLIST_WORK *work , POKEMON_PARAM *pp );
extern const BOOL PLIST_DEMO_CheckSheimiToSky( PLIST_WORK *work , POKEMON_PARAM *pp );
extern void PLIST_DEMO_ChangeSheimiToSky( PLIST_WORK *work , POKEMON_PARAM *pp );
