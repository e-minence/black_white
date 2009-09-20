//============================================================================================
/**
 * @file	warpdata.c
 * @brief	���[�v�p�f�[�^�֘A�\�[�X
 * @since	2005.12.19
 * 
 * 2009.09.20 HGSS����ڐA�J�n
 */
//============================================================================================

#include <gflib.h>

#include "gamesystem/game_data.h"
#include "warpdata.h"

#include "field/location.h"

#include "arc/fieldmap/zone_id.h"

#include "field/field_const.h"  //GRID_TO_FX32

//#include "fieldobj_code.h"		//DIR_UP DIR_DOWN
//#include "ev_mapchange.h"		//DOOR_ID_JUMP_CODE

#include "../../../resource/fldmapdata/flagwork/flag_define.h"
//#include "sysflag.h"			//SysFlag_ArriveSet

//�������������͂��̃V���{����`
#include "../../../resource/fldmapdata/warpdata/warpdata.h"

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
#if 0
typedef struct {
	u16 room_id, room_gx, room_gz;
	u16 fld_id, fld_gx, fld_gz;
	u8 IsTeleportPos;
	u8 AutoSetArriveFlag;
	u16 arrive_id;
}WARPDATA;
#else
typedef struct {
	u16	arrive_id:8;	//�����t���O
	u16	IsTeleportPos:1;
	u16	AutoSetArriveFlag:1;
	u16	:6;

	u16	room_id;	//�S�Ŏ��̖߂��ZoneID
	u16	room_gx:8;	//�S�Ŏ��̖߂��X
	u16	room_gz:8;	//�S�Ŏ��̖߂��Z
	
	u16	fld_id;		//���[�v���WZoneID
	u16 fld_gx;		//���[�v���WX
	u16	fld_gz;		//���[�v���WZ

	u16	escape_id;	//�o�����WZoneID
	u16 escape_gx;	//�o�����WX
	u16	escape_gz;	//�o�����WZ
}WARPDATA;

SDK_COMPILER_ASSERT(sizeof(WARPDATA) == 18);

#endif
//�������������͂��̃f�[�^��`
#include "../../../resource/fldmapdata/warpdata/warpdata.cdat"


static void setLocation(LOCATION * loc, u16 zone_id, u16 dir, u16 gx, u16 gz);
//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	���[�vID�̐��K��
 * @param	warp_id		���[�vID
 * @return	int			���K���������[�vID
 *
 * �s���Ȓl�͕ϊ�����B1�I���W����0�I���W���ɕϊ�����B
 */
//------------------------------------------------------------------
static int RegulateWarpID(int warp_id)
{
	if(warp_id <= 0 || warp_id > NELEMS(WarpData)) {
		GF_ASSERT_MSG(0, "�s���ȃ��[�vID�i%d�j�ł��B\n", warp_id);
		warp_id = 1;		//���i�łł͕s���Ȓl�ł����삷��悤�ɕ␳����
	}
	warp_id --;		//1 origin --> 0 origin
	return warp_id;
}

//------------------------------------------------------------------
/**
 * @brief	���[�vID�̏����l���擾����
 */
//------------------------------------------------------------------
int WARPDATA_GetInitializeID(void)
{
	return 1;
}

//------------------------------------------------------------------
/**
 * @brief	���[�v�ꏊ�̎擾
 * @param	warp_id		���[�vID
 * @param	loc			�ꏊ���󂯎��LOCATION�ւ̃|�C���^
 */
//------------------------------------------------------------------
void WARPDATA_GetWarpLocation(int warp_id, LOCATION * loc)
{
	warp_id = RegulateWarpID(warp_id);

  setLocation( loc,
      WarpData[warp_id].fld_id,
      DIR_DOWN, 
      WarpData[warp_id].fld_gx,
      WarpData[warp_id].fld_gz );
}

//------------------------------------------------------------------
/**
 * @brief	�����ꏊ�̎擾
 * @param	warp_id		���[�vID
 * @param	loc			�ꏊ���󂯎��LOCATION�ւ̃|�C���^
 */
//------------------------------------------------------------------
void WARPDATA_GetRevivalLocation(int warp_id, LOCATION * loc)
{
	warp_id = RegulateWarpID(warp_id);

  setLocation( loc,
      WarpData[warp_id].room_id,
      DIR_UP, 
      WarpData[warp_id].room_gx,
      WarpData[warp_id].room_gz );
}

//------------------------------------------------------------------
/**
 * @brief	�o�����W�̎擾(Escape�t���O�̂Ȃ��|�C���g�Ȃ���ԍ��W��Ԃ�)
 * @param	warp_id		���[�vID
 * @param	loc			�ꏊ���󂯎��LOCATION�ւ̃|�C���^
 */
//------------------------------------------------------------------
void WARPDATA_GetEscapeLocation(int warp_id, LOCATION * loc)
{
	int id = RegulateWarpID(warp_id);

  setLocation( loc,
      WarpData[warp_id].escape_id,
      DIR_DOWN, 
      WarpData[warp_id].escape_gx,
      WarpData[warp_id].escape_gz );
}

//------------------------------------------------------------------
/**
 * @brief	���[�vID�̎擾
 * @param	zone_id		�������鉮���̃]�[��ID
 * @retval	int			���[�vID�i�P�I���W���j
 * @retval	0			������Ȃ�����
 */
//------------------------------------------------------------------
int WARPDATA_SearchByRoomID(int zone_id)
{
	int i;
	for (i = 0; i < NELEMS(WarpData); i++) {
		if (WarpData[i].room_id == zone_id
				&& WarpData[i].IsTeleportPos) {
			return i + 1;
		}
	}
	return 0;
}

//------------------------------------------------------------------
/**
 * @brief	���[�vID�̎擾
 * @param	zone_id		��������t�B�[���h�̃]�[��ID
 * @retval	int			���[�vID�i�P�I���W���j
 * @retval	0			������Ȃ�����
 */
//------------------------------------------------------------------
int WARPDATA_SearchByFieldID(int zone_id)
{
	int i;
	for (i = 0; i < NELEMS(WarpData); i++) {
		if (WarpData[i].fld_id == zone_id
				&& WarpData[i].IsTeleportPos) {
			return i + 1;
		}
	}
	return 0;
}

//------------------------------------------------------------------
/**
 * @brief	���[�vID�̎擾�i�u������Ƃԁv�p�j
 * @param	zone_id		��������t�B�[���h�̃]�[��ID
 * @retval	int			���[�vID�i�P�I���W���j
 * @retval	0			������Ȃ�����
 */
//------------------------------------------------------------------
int WARPDATA_SearchForFly(int zone_id)
{
	int i;
	int id = 0;
	for (i = 0; i < NELEMS(WarpData); i++) {
		if (WarpData[i].fld_id == zone_id) {
			return (i+1);
		}
	}
	return id;
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�����t���O�̃Z�b�g
 * @param	fsys		�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @param	zone_id		�Z�b�g����ꏊ�̃]�[��ID
 */
//------------------------------------------------------------------
void ARRIVEDATA_SetArriveFlag( GAMEDATA * gamedata, int zone_id)
{
	int i;
	for (i = 0; i < NELEMS(WarpData); i++) {
		OS_Printf("zone = %d, WarpZone = %d\n",zone_id,WarpData[i].fld_id);
		if (WarpData[i].fld_id == zone_id && WarpData[i].AutoSetArriveFlag) {
			EVENTWORK_SetEventFlag(GAMEDATA_GetEventWork( gamedata ), WarpData[i].arrive_id);
			return;
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	�����t���O���Q�Ƃ���
 * @param	fsys		�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @param	warp_id		�ΏۂƂȂ�}�b�v�̃��[�vID
 * @retval	BOOL		TRUE�̂Ƃ��A�������Ƃ�����
 */
//------------------------------------------------------------------
BOOL ARRIVEDATA_GetArriveFlag(GAMEDATA * gamedata, int warp_id)
{
	int reg_id = RegulateWarpID(warp_id);
	return EVENTWORK_CheckEventFlag(GAMEDATA_GetEventWork( gamedata ), WarpData[reg_id].arrive_id);
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void setLocation(LOCATION * loc, u16 zone_id, u16 dir, u16 gx, u16 gz)
{
  fx32 x, y, z;
  x = GRID_TO_FX32( gx );
  y = 0;
  z = GRID_TO_FX32( gz );

  LOCATION_SetDirect(loc, zone_id, DIR_DOWN, x, y, z);
}


