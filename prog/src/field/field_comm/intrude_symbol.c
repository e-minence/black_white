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
  
  if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_INVASION){
    intcomm = GameCommSys_GetAppWork(game_comm);
    if(intcomm == NULL 
        || intcomm->intrude_status_mine.palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
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
BOOL IntrudeSymbol_CheckSymbolDataChange(INTRUDE_COMM_SYS_PTR intcomm)
{
  BOOL ret = intcomm->recv_symbol_change_flag;
  intcomm->recv_symbol_change_flag = FALSE;
  return ret;
}
