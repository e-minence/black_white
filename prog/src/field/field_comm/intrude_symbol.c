//==============================================================================
/**
 * @file    intrude_symbol.c
 * @brief   �N���F�V���{���|�P����
 * @author  matsuda
 * @date    2010.03.04(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "field/intrude_comm.h"
#include "intrude_comm_command.h"
#include "intrude_types.h"
#include "field/intrude_symbol.h"
#include "intrude_work.h"
#include "field/fieldmap.h"
#include "intrude_field.h"
#include "field/zonedata.h"


//==============================================================================
//  �f�[�^
//==============================================================================
///�p���X�ƐX�̏o��������W(�p���X��)
static const VecFx32 PalaceForestDoorwayPos = {FX32_CONST(504), FX32_CONST(32), FX32_CONST(360)};



//==================================================================
/**
 * �����̐N�����NetID���擾����
 *
 * @param   game_comm		
 * @param   gamedata		
 *
 * @retval  NetID		    �N�����NetID
 *                      ��ʐM OR ������ROM�ɂ���ꍇ�� INTRUDE_NETID_NULL
 *
 * �ʐM�G���[���N���Ă��Ă��N����ŃG���[�ɂȂ��Ă���ꍇ�͈��������N�����NetID��Ԃ��܂�
 */
//==================================================================
NetID IntrudeSymbol_CheckIntrudeNetID(GAME_COMM_SYS_PTR game_comm, GAMEDATA *gamedata)
{
  INTRUDE_COMM_SYS_PTR intcomm;
  NetID my_netid = GAMEDATA_GetIntrudeMyID(gamedata);
  
  intcomm = Intrude_Check_CommConnect(game_comm);
  if(NetErr_App_CheckError()){
    NetID invasion_netid = GameCommStatus_GetPlayerStatus_InvasionNetID(game_comm, my_netid);
    if(invasion_netid == my_netid){
      return INTRUDE_NETID_NULL;
    }
    return invasion_netid;
  }
  else if(intcomm != NULL){
    if(intcomm->intrude_status_mine.palace_area == my_netid){
      return INTRUDE_NETID_NULL;
    }
    return intcomm->intrude_status_mine.palace_area;
  }
  else{
    return INTRUDE_NETID_NULL;
  }
}

//==================================================================
/**
 * �p���X�ƐX�̏o��������W���擾(�p���X��)
 *
 * @param   game_comm		
 * @param   gamedata		
 * @param   dest_pos		���W�����
 */
//==================================================================
void IntrudeSymbol_GetPosPalaceForestDoorway(GAME_COMM_SYS_PTR game_comm, GAMEDATA *gamedata, VecFx32 *dest_pos)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  int map_offset;
  
  if(intcomm == NULL 
      || intcomm->intrude_status_mine.palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
    map_offset = 0;
  }
  else{
    map_offset = intcomm->intrude_status_mine.palace_area - GAMEDATA_GetIntrudeMyID(gamedata);
    if(map_offset < 0){
      map_offset = intcomm->member_num + map_offset;
    }
  }
  
  *dest_pos = PalaceForestDoorwayPos;
  dest_pos->x += PALACE_MAP_LEN * map_offset;
}

//==================================================================
/**
 * �N����̑���ɑ΂��āA�V���{���G���J�E���g�p�f�[�^��v������
 *
 * @param   intcomm		  
 * @param   zone_type		����֗v������SYMBOL_ZONE_TYPE
 * @param   map_no		  ����֗v������}�b�v�ԍ�(�L�[�v�]�[���̏ꍇ��0�Œ�)
 *
 * @retval  BOOL		    TRUE:���M�����@FALSE:���M���s
 */
//==================================================================
BOOL IntrudeSymbol_ReqSymbolData(INTRUDE_COMM_SYS_PTR intcomm, SYMBOL_MAP_ID symbol_map_id)
{
  SYMBOL_DATA_REQ sdr;
  
  sdr.symbol_map_id = symbol_map_id;
  sdr.occ = TRUE;
  return IntrudeSend_SymbolDataReq(intcomm, intcomm->intrude_status_mine.palace_area, &sdr);
}

//==================================================================
/**
 * IntrudeSymbol_ReqSymbolData�ŗv�������f�[�^����M�������`�F�b�N
 *
 * @param   intcomm		
 *
 * @retval  BOOL		TRUE:��M�����@FALSE:��M���Ă��Ȃ�
 *
 * ���̊֐���TRUE�m�F��AIntrudeSymbol_GetSymbolBuffer�Ŏ�M�����f�[�^�������Ă���o�b�t�@��
 * �|�C���^���擾�ł��܂�
 */
//==================================================================
BOOL IntrudeSymbol_CheckRecvSymbolData(INTRUDE_COMM_SYS_PTR intcomm)
{
  return intcomm->recv_symbol_flag;
}

//==================================================================
/**
 * �N����̃V���{���f�[�^��M�o�b�t�@�̃|�C���^���擾
 *
 * @param   intcomm		
 *
 * @retval  INTRUDE_SYMBOL_WORK *		
 */
//==================================================================
INTRUDE_SYMBOL_WORK * IntrudeSymbol_GetSymbolBuffer(INTRUDE_COMM_SYS_PTR intcomm)
{
  GF_ASSERT(intcomm->recv_symbol_flag == TRUE);
  return &intcomm->intrude_symbol;
}

//==================================================================
/**
 * �����̃V���{���f�[�^�ɕύX�����������ׁA�݂�Ȃɒʒm����
 *
 * @param   intcomm		
 * @param   symbol_map_id		�������ύX�����V���{���}�b�vID
 *
 * �|�P�����̈ړ��FSymbolSave_Field_Move_FreeToKeep or SymbolSave_Field_Move_KeepToFree
 * �|�P�����̕ߊl�FSymbolSave_DataShift
 * ��L�̂����ꂩ���s�����ꍇ�A�ʐM����ɂ������ʒm����ׂɁA���̖��߂��g���đ��M���Ă�������
 * ����ŒʐM����ɒm�点�鎖�ő��葤��IntrudeSymbol_CheckSymbolDataChange�Ō��m���鎖���o���܂�
 */
//==================================================================
void IntrudeSymbol_SendSymbolDataChange(INTRUDE_COMM_SYS_PTR intcomm, SYMBOL_MAP_ID symbol_map_id)
{
  intcomm->send_symbol_change.symbol_map_id = symbol_map_id;
  intcomm->send_symbol_change.occ = TRUE;
}

//==================================================================
/**
 * �N����̃V���{���f�[�^�ɕύX�����������ׂ�
 *    �����������݂���V���{���}�b�v�ɕύX���������Ȃ������TRUE�ɂ͂Ȃ�܂���B
 *      ����������ꏊ�Ƃ͈Ⴄ�}�b�v�ŕύX�����������ꍇ��FALSE�̂܂܂ł�
 *
 * @param   intcomm		
 *
 * @retval  TRUE:�ύX��������(IntrudeSymbol_ReqSymbolData���g���A�ŐV�̃f�[�^���擾���Ȃ����Ă�������)
 * @retval  FALSE:�ύX�͖���
 *
 * ��x���̊֐���TRUE���擾��A�����t���O�����Z�b�g����ׁA�V���Ȓʒm����������FALSE��Ԃ��܂�
 */
//==================================================================
BOOL IntrudeSymbol_CheckSymbolDataChange(INTRUDE_COMM_SYS_PTR intcomm, u16 zone_id)
{
  BOOL ret = intcomm->recv_symbol_change_flag;
  intcomm->recv_symbol_change_flag = FALSE;
  if(ret == TRUE && ZONEDATA_IsBingo( zone_id ) == TRUE){
    return TRUE;
  }
  return FALSE;
}
