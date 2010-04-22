//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scrcmd_weather.c
 *	@brief  �V�C�X�N���v�g�R�}���h
 *	@author	tomoya takahashi
 *	@date		2010.02.10
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/pm_weather.h"

#include "script.h"
#include "script_def.h"
#include "script_local.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "fieldmap.h"

#include "scrcmd_weather.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  �V�C�ύX
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdWeatherChange( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  FIELDMAP_WORK* fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  FIELD_WEATHER* weather = FIELDMAP_GetFieldWeather( fieldWork );
  u16 weather_no = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��1����
  u16 is_notenvse = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��1����

  if(is_notenvse){
    // �ʏ�SE�Đ��œV�C���N��
    FIELD_WEATHER_ChangeNotEnvSe( weather, weather_no );
  }else{
    // ��SE�Đ��œV�C���N��
    FIELD_WEATHER_Change( weather, weather_no );
  }
  GAMEDATA_SetWeatherNo( gdata, weather_no );

  return VMCMD_RESULT_CONTINUE;
}



