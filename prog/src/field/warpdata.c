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

#include "arc/fieldmap/zone_id.h" //ZONE_ID_�`

#include "field/field_const.h"  //GRID_TO_FX32

#include "field/field_dir.h"    //DIR_�`

//#include "fieldobj_code.h"		//DIR_UP DIR_DOWN
//#include "ev_mapchange.h"		//DOOR_ID_JUMP_CODE

//SYS_FLAG_ARRIVE_�`
#include "../../../resource/fldmapdata/flagwork/flag_define.h"


//�������������͂��̃V���{����`
#include "../../../resource/fldmapdata/warpdata/warpdata.h"

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	u16	arrive_flag;	//�����t���O

	u16	IsTeleportPos:1;
	u16	AutoSetArriveFlag:1;
	u16	:14;

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

SDK_COMPILER_ASSERT(sizeof(WARPDATA) == 20);

//�������������͂��̃f�[�^��`
#include "../../../resource/fldmapdata/warpdata/warpdata.cdat"


static void set_location(LOCATION * loc, u16 zone_id, u16 dir, u16 gx, u16 gz);
static int regulate_warp_id(int warp_id);
//============================================================================================
//============================================================================================
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
 * @brief ���[�vID�͈̔̓`�F�b�N���s��
 */
//------------------------------------------------------------------
BOOL WARPDATA_IsValidID( int warp_id )
{
	if(warp_id <= 0 || warp_id > NELEMS(WarpData)) {
		GF_ASSERT_MSG(0, "�s���ȃ��[�vID�i%d�j�ł��B\n", warp_id);
    return FALSE;
	}
  return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief ���[�v��̃]�[��ID�擾
 * @param	warp_id		���[�vID
 * @retval  u16     �]�[��ID
 */
//------------------------------------------------------------------
u16 WARPDATA_GetWarpZoneID( int warp_id )
{
  warp_id = regulate_warp_id( warp_id );
  return WarpData[ warp_id ].fld_id;
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
	warp_id = regulate_warp_id(warp_id);

  set_location( loc,
      WarpData[warp_id].room_id,
      DIR_UP, 
      WarpData[warp_id].room_gx,
      WarpData[warp_id].room_gz );
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
 * @param gamedata  �Q�[���f�[�^�ւ̃|�C���^
 * @param	zone_id		�Z�b�g����ꏊ�̃]�[��ID
 *
 * @note
 * �ߋ���ł�ArriveFlag�̈����͓��ꂾ�������A
 * ���񂩂�͒P�Ȃ�V�X�e���t���O�Ƃ��Ĉ����悤�ɂ���
 */
//------------------------------------------------------------------
void ARRIVEDATA_SetArriveFlag( GAMEDATA * gamedata, int zone_id)
{
	int i;
	for (i = 0; i < NELEMS(WarpData); i++) {
		//OS_Printf("zone = %d, WarpZone = %d\n",zone_id,WarpData[i].fld_id);
		if (WarpData[i].fld_id == zone_id && WarpData[i].AutoSetArriveFlag) {
			EVENTWORK_SetEventFlag(GAMEDATA_GetEventWork( gamedata ), WarpData[i].arrive_flag);
      TAMADA_Printf("Arrive Flag Set: %x\n", WarpData[i].arrive_flag );
			return;
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	�����t���O���Q�Ƃ���
 * @param gamedata  �Q�[���f�[�^�ւ̃|�C���^
 * @param	warp_id		�ΏۂƂȂ�}�b�v�̃��[�vID
 * @retval	BOOL		TRUE�̂Ƃ��A�������Ƃ�����
 *
 * @note
 * �ߋ���ł�ArriveFlag�̈����͓��ꂾ�������A
 * ���񂩂�͒P�Ȃ�V�X�e���t���O�Ƃ��Ĉ����悤�ɂ���
 */
//------------------------------------------------------------------
BOOL ARRIVEDATA_GetArriveFlag(GAMEDATA * gamedata, int warp_id)
{
	int reg_id = regulate_warp_id(warp_id);
	return EVENTWORK_CheckEventFlag(GAMEDATA_GetEventWork( gamedata ), WarpData[reg_id].arrive_flag);
}


//============================================================================================
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief LOCATION�̃Z�b�g�A�b�v
 * @param loc       �Z�b�g����LOCATION�ւ̃|�C���^
 * @param zone_id   �o����̃]�[���w��ID
 * @param dir       ����
 * @param gx        �o������X�ʒu�i�O���b�h�P�ʁj
 * @param gz        �o������Z�ʒu�i�O���b�h�P�ʁj
 *
 * @todo  dir�̒l�̒�`�Ɏ�茈�߂��Ȃ�
 * @todo  Y�����f�[�^���ݒ肷��ׂ����ǂ����H
 */
//------------------------------------------------------------------
static void set_location(LOCATION * loc, u16 zone_id, u16 dir, u16 gx, u16 gz)
{
  fx32 x, y, z;
  x = GRID_TO_FX32( gx );
  y = 0;
  z = GRID_TO_FX32( gz );

  LOCATION_SetDirect(loc, zone_id, DIR_DOWN, x, y, z);
}

//------------------------------------------------------------------
/**
 * @brief	���[�vID�̐��K��
 * @param	warp_id		���[�vID
 * @return	int			���K���������[�vID
 *
 * �s���Ȓl�͕ϊ�����B1�I���W����0�I���W���ɕϊ�����B
 */
//------------------------------------------------------------------
static int regulate_warp_id(int warp_id)
{
  if ( WARPDATA_IsValidID( warp_id ) == FALSE ) {
    //���i�łł͕s���Ȓl�ł����삷��悤�ɕ␳����
		warp_id = WARPDATA_GetInitializeID();
	}
	warp_id --;		//1 origin --> 0 origin
	return warp_id;
}


