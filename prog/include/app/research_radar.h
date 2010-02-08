/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  調査レーダー
 * @file   research_radar.h
 * @author obata
 * @date   2010.02.02
 */
///////////////////////////////////////////////////////////////////////////////// 
#pragma once
#include <gflib.h>
#include "gamesystem/gamesystem.h"


//===============================================================================
// ■プロセス呼び出しパラメータ
//===============================================================================
typedef struct
{
  GAMESYS_WORK* gameSystem;

} RESEARCH_PARAM;


//===============================================================================
// ■プロセス 定義データ
//===============================================================================
extern GFL_PROC_DATA ResearchRadarProcData; 
