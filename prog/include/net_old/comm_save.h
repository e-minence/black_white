//=============================================================================
/**
 * @file	comm_save.h
 * @brief	�ʐM�����Z�[�u����
 * @author	Akito Mori
 * @date    2006.05.12
 */
//=============================================================================

#pragma once

#include "savedata/savedata.h"

extern void CommSyncronizeSaveInit( int *seq );
extern int CommSyncronizeSave( SAVEDATA *savedata, int BlockID, int *seq );



