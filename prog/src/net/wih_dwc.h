#pragma once

#include "net.h"
#include "gamesystem/game_comm.h"

/*---------------------------------------------------------------------------*
  Name:         WH_AllBeaconStart
  Description:  �r�[�R���Ȃ牽�ł����W GEAR�p
  Arguments:    num �r�[�R�����W��
 *---------------------------------------------------------------------------*/

extern void WH_AllBeaconStart(int num);
extern void WH_AllBeaconEnd(void);

/*---------------------------------------------------------------------------*
  Name:         WH_MainLoopScanBeaconData
  Description:  CGEAR�p�Ȃ�ł��r�[�R�����WMAIN
 *---------------------------------------------------------------------------*/

extern void WH_MainLoopScanBeaconData(void);
extern GAME_COMM_STATUS WH_GetAllBeaconType(void);


