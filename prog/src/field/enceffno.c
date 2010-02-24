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

//--------------------------------------------------------------
/**
 * �쐶��G�t�F�N�g���I�i�ނ�������j
 * @param inMonsNo    �����X�^�[�i���o�[
 * @param fieldmap    �t�B�[���h�}�b�v�|�C���^  
 * @retval int        �G���J�E���g�G�t�F�N�g�i���o�[
 */
//--------------------------------------------------------------
int ENCEFFNO_GetWildEncEffNo( const int inMonsNo, FIELDMAP_WORK *fieldmap )
{
  int no;
  //����̃����X�^�[���𒲂ׂ�
  switch (inMonsNo){
  //�p�b�P�[�W
  case MONSNO_SIN:
  case MONSNO_MU:
  case MONSNO_RAI:
    return ENCEFFID_PACKAGE_POKE;
  //�O�e�m  
  case MONSNO_PORUTOSU:
  case MONSNO_ARAMISU:
  case MONSNO_DARUTANISU:
    return ENCEFFID_THREE_POKE;
  //�ړ��|�P�Ƃ�
  case MONSNO_KAZAKAMI:
  case MONSNO_RAIKAMI:
  case MONSNO_TUTINOKAMI:
    return ENCEFFID_MOVE_POKE;
  }

  //���@�̂���ꏊ�̃A�g���r���[�g���擾
  {
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
      no = ENCEFFID_WILD_WATER;
    }
    else if ( MAPATTR_VALUE_CheckEncountGrassA( val ) ) //�㑐
    {
      no = ENCEFFID_WILD_NORMAL;
    }
    else if ( MAPATTR_VALUE_CheckEncountGrassB( val ) ) //����
    {
      no = ENCEFFID_WILD_HEIGH;
    }
    else if ( MAPATTR_VALUE_CheckMarsh( val ) )         //���
    {
      no = ENCEFFID_WILD_NORMAL;
    }
    else if ( MAPATTR_VALUE_CheckDesertDeep( val ) )    //��
    {
      no = ENCEFFID_WILD_DESERT;
    }
    else  //���̑��@�����Ƃ݂Ȃ�
    {
      no = ENCEFFID_WILD_INNER;
    }
  }

  return no;

}

//--------------------------------------------------------------
/**
 * �g���[�i�[��G�t�F�N�g���I(�T�u�E�F�C������)
 * @param inTrType      �g���[�i�[ID
 * @param fieldmap    �t�B�[���h�}�b�v�|�C���^  
 * @retval int        �G���J�E���g�G�t�F�N�g�i���o�[
 */
//--------------------------------------------------------------
int ENCEFFNO_GetTrEncEffNo( const int inTrID, FIELDMAP_WORK *fieldmap )
{
  int no;
  int tr_type;
  tr_type = TT_TrainerDataParaGet( inTrID, ID_TD_tr_type );

  switch(tr_type){
  //�W�����[�_�[
  case TRTYPE_LEADER1A:
    no = ENCEFFID_LEADER1A;
    return no;
  case TRTYPE_LEADER1B:
    no = ENCEFFID_LEADER1B;
    return no;
  case TRTYPE_LEADER1C:
    no = ENCEFFID_LEADER1C;
    return no;
  case TRTYPE_LEADER2:
    no = ENCEFFID_LEADER2;
    return no;
  case TRTYPE_LEADER3:
    no = ENCEFFID_LEADER3;
    return no;
  case TRTYPE_LEADER4:
    no = ENCEFFID_LEADER4;
    return no;
  case TRTYPE_LEADER5:
    no = ENCEFFID_LEADER5;
    return no;
  case TRTYPE_LEADER6:
    no = ENCEFFID_LEADER6;
    return no;
  case TRTYPE_LEADER7:
    no = ENCEFFID_LEADER7;
    return no;
  case TRTYPE_LEADER8A:
    no = ENCEFFID_LEADER8A;
    return no;
  case TRTYPE_LEADER8B:
    no = ENCEFFID_LEADER8B;
    return no;
  //�l�V��
  case TRTYPE_BIGFOUR1:
    no = ENCEFFID_BIGFOUR1;
    return no;
  case TRTYPE_BIGFOUR2:
    no = ENCEFFID_BIGFOUR2;
    return no;
  case TRTYPE_BIGFOUR3:
    no = ENCEFFID_BIGFOUR3;
    return no;
  case TRTYPE_BIGFOUR4:
    no = ENCEFFID_BIGFOUR4;
    return no;
  //���C�o��
  case TRTYPE_RIVAL:
    no = ENCEFFID_RIVAL;
    return no;
  //�T�|�[�^�[
  case TRTYPE_SUPPORT:
    no = ENCEFFID_SUPPORT;
    return no;
  //�v���Y�}�c
  case TRTYPE_HAKAIM1:
  case TRTYPE_HAKAIW1:
    no = ENCEFFID_PRAZUMA;
    return no;
  //�m
  case TRTYPE_BOSS:
    no = ENCEFFID_BOSS;
    return no;
  //�Q�[�c�B�X
  case TRTYPE_SAGE1:
    no = ENCEFFID_SAGE;
    return no;
  //�`�����v
  case TRTYPE_CHAMPION:
    no = ENCEFFID_CHAMP;
    return no;
  }

  //����ȊO
  {
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
      no = ENCEFFID_TR_WATER;
    }
    else if ( MAPATTR_VALUE_CheckDesert( val ) )        //�����A�g���r���[�g
    {
      no = ENCEFFID_TR_DESERT;
    }
    else{
      AREADATA* areadata = FIELDMAP_GetAreaData( fieldmap );
      BOOL outdoor = ( AREADATA_GetInnerOuterSwitch(areadata) != 0 );
      if ( outdoor )  //���O
      {
        no = ENCEFFID_TR_NORMAL;
      }else{          //����
        no = ENCEFFID_TR_INNER;
      }
    }
  }

  return no;
}



