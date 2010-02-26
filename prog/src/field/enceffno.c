//======================================================================
/*
 * @file	enceffno.c
 * @brief	�G���J�E���g�G�t�F�N�g���I
 * @author saito
 */
//======================================================================
#include "field/fieldmap.h"
#include "enceffno.h"

#include "tr_tool/trtype_def.h"
#include "tr_tool/tr_tool.h"

#include "enceffno_def.h"
#include "sound/wb_sound_data.sadl" //�T�E���h���x���t�@�C��

//--------------------------------------------------------------
/**
 * �쐶��G�t�F�N�g���I�i�ނ�������j
 * @param inMonsNo    �����X�^�[�i���o�[
 * @param inEncType   �G���J�E���g�^�C�v�@encount_data.h�Q��
 * @param fieldmap    �t�B�[���h�}�b�v�|�C���^
 * @param   outEffNo    �G���J�E���g�G�t�F�N�g�i���o�[
 * @param   outBgmNo    BGM�i���o�[
 * @retval none        
 */
//--------------------------------------------------------------
void ENCEFFNO_GetWildEncEffNoBgmNo( const int inMonsNo, ENCOUNT_TYPE inEncType, FIELDMAP_WORK *fieldmap,
                                    int *outEffNo, u16 *outBgmNo )
{
  //����̃����X�^�[���𒲂ׂ�
  switch (inMonsNo){
  case MONSNO_ZOROAAKU:
    *outEffNo = ENCEFFID_PACKAGE_POKE;    //@todo
    *outBgmNo = SEQ_BGM_VS_TSUYOPOKE;
    return;
  //�p�b�P�[�W
  case MONSNO_SIN:
    *outBgmNo = SEQ_BGM_VS_SHIN;
    *outEffNo = ENCEFFID_PACKAGE_POKE;
    return;
  case MONSNO_MU:
    *outBgmNo = SEQ_BGM_VS_MU;
    *outEffNo = ENCEFFID_PACKAGE_POKE;
    return;
  case MONSNO_RAI:
    *outBgmNo = SEQ_BGM_VS_RAI;
    *outEffNo = ENCEFFID_PACKAGE_POKE;
    return;
  //�O�e�m  
  case MONSNO_PORUTOSU:
  case MONSNO_ARAMISU:
  case MONSNO_DARUTANISU:
    *outBgmNo = SEQ_BGM_VS_SETPOKE;
    *outEffNo = ENCEFFID_THREE_POKE;
    return;
  //�ړ��|�P�Ƃ�
  case MONSNO_KAZAKAMI:
  case MONSNO_RAIKAMI:
  case MONSNO_TUTINOKAMI:
    *outBgmNo = SEQ_BGM_VS_MOVEPOKE;
    *outEffNo = ENCEFFID_MOVE_POKE;
    return;
  }

  //�ނ�`�F�b�N
  if ( inEncType == ENC_TYPE_FISHING )
  {
    *outBgmNo = SEQ_BGM_VS_NORAPOKE;
    *outEffNo = ENCEFFID_WILD_WATER;
    return;

  }

  //���@�̂���ꏊ�̃A�g���r���[�g���擾
  {
    int eff;
    u16 bgm;
    MAPATTR_VALUE val;
    MAPATTR attr;
    MAPATTR_FLAG attr_flag;
    FIELD_PLAYER *player = FIELDMAP_GetFieldPlayer( fieldmap );
    attr = FIELD_PLAYER_GetMapAttr( player );
    val = MAPATTR_GetAttrValue(attr);
    attr_flag = MAPATTR_GetAttrFlag( attr );
    //�擾�����A�g���r���[�g�ŕ���
    if( attr_flag & MAPATTR_FLAGBIT_WATER )             //�g���A�g���r���[�g
    {
      bgm = SEQ_BGM_VS_NORAPOKE;
      eff = ENCEFFID_WILD_WATER;
    }
    else if ( MAPATTR_VALUE_CheckEncountGrassA( val ) ) //�㑐
    {
      bgm = SEQ_BGM_VS_NORAPOKE;
      eff = ENCEFFID_WILD_NORMAL;
    }
    else if ( MAPATTR_VALUE_CheckEncountGrassB( val ) ) //����
    {
      bgm = SEQ_BGM_VS_TSUYOPOKE;
      eff = ENCEFFID_WILD_HEIGH;
    }
    else if ( MAPATTR_VALUE_CheckMarsh( val ) )         //���
    {
      bgm = SEQ_BGM_VS_NORAPOKE;
      eff = ENCEFFID_WILD_NORMAL;
    }
    else if ( MAPATTR_VALUE_CheckDesertDeep( val ) )    //��
    {
      bgm = SEQ_BGM_VS_NORAPOKE;
      eff = ENCEFFID_WILD_DESERT;
    }
    else  //���̑��@�����Ƃ݂Ȃ�
    {
      bgm = SEQ_BGM_VS_NORAPOKE;
      eff = ENCEFFID_WILD_INNER;
    }
  
    *outEffNo = eff;
    *outBgmNo = bgm;
  }

  return;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[��G�t�F�N�g���I(�T�u�E�F�C������)
 * @param inTrType      �g���[�i�[ID
 * @param fieldmap    �t�B�[���h�}�b�v�|�C���^  
 * @param   outEffNo    �G���J�E���g�G�t�F�N�g�i���o�[
 * @param   outBgmNo    BGM�i���o�[
 * @retval none
 */
//--------------------------------------------------------------
void ENCEFFNO_GetTrEncEffNoBgmNo( const int inTrID, FIELDMAP_WORK *fieldmap,
                                  int *outEffNo, u16 *outBgmNo )
{
  int tr_type;
  tr_type = TT_TrainerDataParaGet( inTrID, ID_TD_tr_type );

  switch(tr_type){
  //�W�����[�_�[
  case TRTYPE_LEADER1A:
    *outBgmNo = SEQ_BGM_VS_GYMLEADER;
    *outEffNo = ENCEFFID_LEADER1A;
    return;
  case TRTYPE_LEADER1B:
    *outBgmNo = SEQ_BGM_VS_GYMLEADER;
    *outEffNo = ENCEFFID_LEADER1B;
    return;
  case TRTYPE_LEADER1C:
    *outBgmNo = SEQ_BGM_VS_GYMLEADER;
    *outEffNo = ENCEFFID_LEADER1C;
    return;
  case TRTYPE_LEADER2:
    *outBgmNo = SEQ_BGM_VS_GYMLEADER;
    *outEffNo = ENCEFFID_LEADER2;
    return;
  case TRTYPE_LEADER3:
    *outBgmNo = SEQ_BGM_VS_GYMLEADER;
    *outEffNo = ENCEFFID_LEADER3;
    return;
  case TRTYPE_LEADER4:
    *outBgmNo = SEQ_BGM_VS_GYMLEADER;
    *outEffNo = ENCEFFID_LEADER4;
    return;
  case TRTYPE_LEADER5:
    *outBgmNo = SEQ_BGM_VS_GYMLEADER;
    *outEffNo = ENCEFFID_LEADER5;
    return;
  case TRTYPE_LEADER6:
    *outBgmNo = SEQ_BGM_VS_GYMLEADER;
    *outEffNo = ENCEFFID_LEADER6;
    return;
  case TRTYPE_LEADER7:
     *outBgmNo = SEQ_BGM_VS_GYMLEADER;
     *outEffNo = ENCEFFID_LEADER7;
    return;
  case TRTYPE_LEADER8A:
    *outBgmNo = SEQ_BGM_VS_GYMLEADER;
    *outEffNo = ENCEFFID_LEADER8A;
    return;
  case TRTYPE_LEADER8B:
    *outBgmNo = SEQ_BGM_VS_GYMLEADER;    
    *outEffNo = ENCEFFID_LEADER8B;
    return;
  //�l�V��
  case TRTYPE_BIGFOUR1:
    *outBgmNo = SEQ_BGM_VS_SHITENNO;
    *outEffNo = ENCEFFID_BIGFOUR1;
    return;
  case TRTYPE_BIGFOUR2:
    *outBgmNo = SEQ_BGM_VS_SHITENNO;
    *outEffNo = ENCEFFID_BIGFOUR2;
    return;
  case TRTYPE_BIGFOUR3:
    *outBgmNo = SEQ_BGM_VS_SHITENNO;
    *outEffNo = ENCEFFID_BIGFOUR3;
    return;
  case TRTYPE_BIGFOUR4:
    *outBgmNo = SEQ_BGM_VS_SHITENNO;
    *outEffNo = ENCEFFID_BIGFOUR4;
    return;
  //���C�o��
  case TRTYPE_RIVAL:
    *outBgmNo = SEQ_BGM_VS_RIVAL;
    *outEffNo = ENCEFFID_RIVAL;
    return;
  //�T�|�[�^�[
  case TRTYPE_SUPPORT:
    *outBgmNo = SEQ_BGM_VS_RIVAL;
    *outEffNo = ENCEFFID_SUPPORT;
    return;
  //�v���Y�}�c
  case TRTYPE_HAKAIM1:
  case TRTYPE_HAKAIW1:
    *outBgmNo = SEQ_BGM_VS_PLASMA;
    *outEffNo = ENCEFFID_PRAZUMA;
    return;
  //�m
  case TRTYPE_BOSS:
    *outBgmNo = SEQ_BGM_VS_N;
    *outEffNo = ENCEFFID_BOSS;
    return;
  //�Q�[�c�B�X
  case TRTYPE_SAGE1:
    *outBgmNo = SEQ_BGM_VS_G_CIS;
    *outEffNo = ENCEFFID_SAGE;
    return;
  //�`�����v
  case TRTYPE_CHAMPION:
    *outBgmNo = SEQ_BGM_VS_CHAMP;
    *outEffNo = ENCEFFID_CHAMP;
    return;
  }

  //����ȊO
  {
    int eff;
    MAPATTR_VALUE val;
    MAPATTR attr;
    MAPATTR_FLAG attr_flag;
    FIELD_PLAYER *player = FIELDMAP_GetFieldPlayer( fieldmap );
    attr = FIELD_PLAYER_GetMapAttr( player );
    val = MAPATTR_GetAttrValue(attr);
    attr_flag = MAPATTR_GetAttrFlag( attr );
    //�擾�����A�g���r���[�g�ŕ���
    if( attr_flag & MAPATTR_FLAGBIT_WATER )             //�g���A�g���r���[�g
    {
      eff = ENCEFFID_TR_WATER;
    }
    else if ( MAPATTR_VALUE_CheckDesert( val ) )        //�����A�g���r���[�g
    {
      eff = ENCEFFID_TR_DESERT;
    }
    else{
      AREADATA* areadata = FIELDMAP_GetAreaData( fieldmap );
      BOOL outdoor = ( AREADATA_GetInnerOuterSwitch(areadata) != 0 );
      if ( outdoor )  //���O
      {
        eff = ENCEFFID_TR_NORMAL;
      }else{          //����
        eff = ENCEFFID_TR_INNER;
      }
    }

    *outBgmNo = SEQ_BGM_VS_TRAINER;
    *outEffNo = eff;
  }
  return;
}



