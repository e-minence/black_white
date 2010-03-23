//=========================================================================
/*
 *  @file   encount_data.c
 *  @brief  �G���J�E���g�f�[�^�A�[�J�C�u�A�N�Z�X
 *  @author Miyuki Iwasawa
 *  @date   10.03.23
 */
//=========================================================================

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "system/main.h"

#include "fieldmap/zone_id.h"
#include "field/zonedata.h"
#include "encount_data.h"

//------------------------------------------------------------------
/*
 *  @brief  �]�[���G���J�E���g�f�[�^���[�h
 *
 *  @param  data    �G���J�E���g�f�[�^�����[�h���郏�[�N�A�h���X
 *  @param  handel  �G���J�E���g�f�[�^�A�[�J�C�u�n���h��
 *  @param  zone_id �]�[��ID
 *  @param  season_id �G��ID
 *  @retval true  �G���J�E���g�f�[�^�擾�ɐ���
 *  @retval false �w��]�[���ɂ̓G���J�E���g�f�[�^���Ȃ�
 */
//------------------------------------------------------------------
BOOL ENCOUNT_DATA_Load( ENCOUNT_DATA *data, ARCHANDLE* handle, u16 zone_id, u8 season_id )
{
  u32 size;
  u16 id,season;
  MI_CpuClear8( data,sizeof(ENCOUNT_DATA));

  id = ZONEDATA_GetEncountDataID(zone_id);
  if(id == ENC_DATA_INVALID_ID){
    return FALSE;
  }
  //�f�[�^�̌����擾
  size = GFL_ARC_GetDataSizeByHandle( handle, id ) / sizeof(ENCOUNT_DATA);
  if(size != 1 && size != 4){
    GF_ASSERT(0);
    return FALSE;
  }
  //�G�߂��擾
  if(size == 1){
    season = 0;
  }else{
    season = season_id;
  }
  //���[�h
  GFL_ARC_LoadDataOfsByHandle( handle, id,
      season*sizeof(ENCOUNT_DATA), sizeof(ENCOUNT_DATA), data );
  data->enable_f = TRUE;

  return TRUE;
}



