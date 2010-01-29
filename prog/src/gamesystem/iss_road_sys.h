//////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_road_sys.h
 * @brief  ���HISS�V�X�e��
 * @author obata_toshihiro
 * @date   2009.07.29
 */
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>
#include "gamesystem/playerwork.h"


// ���HISS�V�X�e���̕s���S�^�̐錾
typedef struct _ISS_ROAD_SYS ISS_ROAD_SYS;


//============================================================================
// ���V�X�e��
//============================================================================
// �쐬/�j��
extern ISS_ROAD_SYS* ISS_ROAD_SYS_Create( PLAYER_WORK* player, HEAPID heapID );
extern void          ISS_ROAD_SYS_Delete( ISS_ROAD_SYS* system );

// ����
extern void ISS_ROAD_SYS_Update( ISS_ROAD_SYS* system );

// ����
extern void ISS_ROAD_SYS_On ( ISS_ROAD_SYS* system );
extern void ISS_ROAD_SYS_Off( ISS_ROAD_SYS* system );
