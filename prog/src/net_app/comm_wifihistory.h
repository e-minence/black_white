//============================================================================================
/**
 * @file    comm_wifihistory.h
 * @brief   �n���V���o�^����
 * @author  Akito Mori
 * @date    10.06.03
 */
//============================================================================================
#ifndef __COMM_WIFIHISTORY_H__
#define __COMM_WIFIHISTORY_H__


//==============================================================================
/**
 * @brief   ����̃J�[�g���b�W���[�W�����E���E�n��R�[�h����n���V����o�^����
 *
 * @param   wifiHistory   �W�I�l�b�g�Z�[�u�f�[�^�\����
 * @param   Nation        ���ԍ�
 * @param   Area          �n��ԍ�
 * @param   langCode      �Ώۂ̃J�Z�b�g�R�[�h
 *
 * @retval  none    
 */
//==============================================================================
static inline void Comm_WifiHistoryDataSet( WIFI_HISTORY *wifiHistory, int Nation, int Area, int langCode )
{
#if (PM_LANG == LANG_JAPAN)
  // ����̃J�[�g���b�W�����{�Ȃ̂�
  if(langCode==LANG_JAPAN){
    // �Ⴄ���̒n���V�R�[�h�����Ă�����o�^���Ȃ�
    if(Nation!=WIFI_NATION_JAPAN){
      return;
    }
  }
#endif
  
  // ���E�n���o�^
    OS_Printf("no=%d, Nation=%d area=%d langCode=%d\n", WIFIHISTORY_GetStat(wifiHistory, Nation, Area ), Nation, Area, langCode );
  if(WIFIHISTORY_GetStat( wifiHistory, Nation, Area )==WIFIHIST_STAT_NODATA){
    WIFIHISTORY_SetStat( wifiHistory, Nation, Area, WIFIHIST_STAT_NEW );
  }
}

//=============================================================================================
/**
 * @brief   �n���V����MYSTATUS�̔z�񂩂�o�^
 *
 * @param   wifiHistory �W�I�l�b�g���\����
 * @param   status      MYSTATUS�|�C���^�z��
 * @param   num         MYSTATUS�̐�
 */
//=============================================================================================
static inline void Comm_WifiHistoryCheck( WIFI_HISTORY *wifiHistory, const MYSTATUS *status[], int num )
{
  int i;
  int Nation,Area,region;

  // �ڑ����Ă���l�����܂킷
  for(i=0;i<num;i++){

    // �����ĂȂ��������΍�
    if(status[i]==NULL){
      continue;
    }

    // ���E�n��R�[�h��o�^�i���������{�J�[�g���b�W��p�`�F�b�N����j
    Nation = MyStatus_GetMyNation(status[i]);
    Area   = MyStatus_GetMyArea(status[i]);
    region = MyStatus_GetRegionCode(status[i]);
    Comm_WifiHistoryDataSet( wifiHistory, Nation, Area, region );
  }
}



#endif
