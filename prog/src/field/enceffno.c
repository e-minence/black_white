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
    return 0;
  //�O�e�m  
  case MONSNO_PORUTOSU:
  case MONSNO_ARAMISU:
  case MONSNO_DARUTANISU:
    return 0;
  //�ړ��|�P�Ƃ�
  case MONSNO_KAZAKAMI:
  case MONSNO_RAIKAMI:
  case MONSNO_TUTINOKAMI:
    return 0;
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
      no = 0;
    }
    else if ( MAPATTR_VALUE_CheckEncountGrassA( val ) ) //�㑐
    {
      no = 0;
    }
    else if ( MAPATTR_VALUE_CheckEncountGrassB( val ) ) //����
    {
      no = 0;
    }
    else if ( MAPATTR_VALUE_CheckMarsh( val ) )         //���
    {
      no = 0;
    }
    else if ( MAPATTR_VALUE_CheckDesertDeep( val ) )    //��
    {
      no = 0;
    }
    else  //���̑�
    {
      no = 0;
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
    no = 0;
    return no;
  case TRTYPE_LEADER1B:
    no = 0;
    return no;
  case TRTYPE_LEADER1C:
    no = 0;
    return no;
  case TRTYPE_LEADER2:
    no = 0;
    return no;
  case TRTYPE_LEADER3:
    no = 0;
    return no;
  case TRTYPE_LEADER4:
    no = 0;
    return no;
  case TRTYPE_LEADER5:
    no = 0;
    return no;
  case TRTYPE_LEADER6:
    no = 0;
    return no;
  case TRTYPE_LEADER7:
    no = 0;
    return no;
  case TRTYPE_LEADER8A:
    no = 0;
    return no;
  case TRTYPE_LEADER8B:
    no = 0;
    return no;
  //�l�V��
  case TRTYPE_BIGFOUR1:
    no = 0;
    return no;
  case TRTYPE_BIGFOUR2:
    no = 0;
    return no;
  case TRTYPE_BIGFOUR3:
    no = 0;
    return no;
  case TRTYPE_BIGFOUR4:
    no = 0;
    return no;
  //���C�o���E�T�|�[�^�[
  case TRTYPE_RIVAL:
  case TRTYPE_SUPPORT:
    no = 0;
    return no;
  //�v���Y�}�c
  case TRTYPE_HAKAIM1:
  case TRTYPE_HAKAIW1:
    no = 0;
    return no;
  //�m
  case TRTYPE_BOSS:
    no = 0;
    return no;
  //�Q�[�c�B�X
  case TRTYPE_SAGE1:
    no = 0;
    return no;
  //�`�����v
  ;
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
      no = 0;
    }
    else if ( MAPATTR_VALUE_CheckDesert( val ) )        //�����A�g���r���[�g
    {
      no = 0;
    }
    else{
      AREADATA* areadata = FIELDMAP_GetAreaData( fieldmap );
      BOOL outdoor = ( AREADATA_GetInnerOuterSwitch(areadata) != 0 );
      if ( outdoor )  //���O
      {
        no = 0;
      }else{          //����
        no = 0;
      }
    }
  }

  return no;
}



