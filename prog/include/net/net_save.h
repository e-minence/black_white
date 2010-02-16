//=============================================================================
/**
 * @file	  net_save.h
 * @brief	  �ʐM�Z�[�u �G���[�ɂȂ�Ɠr���Ŕ����Ă��܂�
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date	  10/02/15
 */
//=============================================================================

#pragma once

#include "gamesystem/game_data.h"

typedef struct _NET_SAVE_WORK NET_SAVE_WORK;


//------------------------------------------------------------------
/**
 * @brief   �ʐM�Z�[�u�J�n
 * @param   HEAPID        �q�[�v
 * @param   GAMEDATA      �Q�[���f�[�^
 * @retval  NET_SAVE_WORK ���[�N
 */
//------------------------------------------------------------------

extern NET_SAVE_WORK* NET_SAVE_Init(HEAPID heapID, GAMEDATA* pGameData);


//------------------------------------------------------------------
/**
 * @brief   �ʐM�Z�[�u
 * @param   pNetSaveWork  ���[�N
 * @retval  �I��������TRUE
 */
//------------------------------------------------------------------

extern BOOL NET_SAVE_Main(NET_SAVE_WORK* pNetSaveWork);


//------------------------------------------------------------------
/**
 * @brief   �ʐM�Z�[�u�I������
 * @param   pNetSaveWork  ���[�N
 */
//------------------------------------------------------------------

extern void NET_SAVE_Exit(NET_SAVE_WORK* pNetSaveWork);


