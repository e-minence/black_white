//======================================================================
/**
 * @file  map_attr.c
 * @brief	�}�b�v�A�g���r���[�g
 * @author kagaya
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "field_g3d_mapper.h"
#include "map_attr.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================

//======================================================================
//  �}�b�v�A�g���r���[�g
//======================================================================
//--------------------------------------------------------------
/**
 * �w��ʒu�̃A�g���r���[�g���擾
 * @param mapper FLDMAPPER
 * @param pos �A�g���r���[�g���擾������x,z���W,y�͌��݈ʒu(�������͊K�w����ɗp��������l)
 * @retval MAPATTR MAPATTR_ERROR=�擾�G���[
 */
//--------------------------------------------------------------
MAPATTR MAPATTR_GetAttribute( FLDMAPPER *mapper, const VecFx32 *pos )
{
  MAPATTR attr = MAPATTR_ERROR;
#if 0 //�P�w
  FLDMAPPER_GRIDINFO gridInfo;
  
  if( FLDMAPPER_GetGridInfo(mapper,pos,&gridInfo) == TRUE ){
    attr = gridInfo.gridData[0].attr;  
  }
#else //���w�Ή�
  FLDMAPPER_GRIDINFODATA gridData;
  if( FLDMAPPER_GetGridData(mapper,pos,&gridData) == TRUE){
    return gridData.attr;
  }
#endif
  
  return( attr );
}

//--------------------------------------------------------------
/**
 * MAPATTR����MAPATTR_VALUE���擾
 * @param attr MAPATTR
 * @retval MAPATTR_VALUE
 */
//--------------------------------------------------------------
MAPATTR_VALUE MAPATTR_GetAttrValue( const MAPATTR attr )
{
  MAPATTR_VALUE val = attr & 0xffff;
  return( val );
}

//--------------------------------------------------------------
/**
 * MAPATTR����MAPATTR_FLAG���擾
 * @param attr MAPATTR
 * @retval MAPATTR_FLAG
 */
//--------------------------------------------------------------
MAPATTR_FLAG MAPATTR_GetAttrFlag( const MAPATTR attr )
{
  MAPATTR_FLAG flag = (attr >> 16) & 0xffff;
  return( flag );
}

//--------------------------------------------------------------
/**
 * @brief MAPATTR����A�A�g���r���[�g�o�����[�̗L��/�������擾
 *
 * @param attr MAPATTR
 * @retval TRUE   �L�� 
 * @retval FALSE  ����
 */
//--------------------------------------------------------------
BOOL MAPATTR_IsEnable( const MAPATTR attr )
{
  u16 val = MAPATTR_GetAttrValue( attr );

  //�A�g���r���[�g���̂��́A��������Value���������`�F�b�N
  if( (attr == MAPATTR_ERROR) ||
      (val == MAPATTR_VAL_ERR)){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief MAPATTR����i���s��Ԃ��擾
 *
 * @param attr MAPATTR
 * @retval TRUE �i���s��
 * @retval FALSE �i����
 *
 * @com �A�g���r���[�g�����l���i���s�ƕԂ����߁A������Hitch�t���O�݂̂�Ԃ��Ă���̂ł͂���܂���
 */
//--------------------------------------------------------------
BOOL MAPATTR_GetHitchFlag( const MAPATTR attr )
{
  u16 flag = MAPATTR_GetAttrFlag( attr );

  //�A�g���r���[�gValue���������A�i���s�t���O��On�Ȃ�ʂ�Ȃ�
  if( (MAPATTR_IsEnable( attr ) == FALSE) ||
      (flag & MAPATTR_FLAGBIT_HITCH)){
    return TRUE;
  }
  return FALSE;
}

//======================================================================
//  �A�g���r���[�g�o�����[�@����
//======================================================================
//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@������
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckLongGrass( const MAPATTR_VALUE val )
{
  return (val == 0x06 || val == 0x07);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@��W�����v�A�g���r���[�g
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckJumpUp( const MAPATTR_VALUE val )
{
  return (val == 0x74);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@���W�����v�A�g���r���[�g
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckJumpDown( const MAPATTR_VALUE val )
{
  return (val == 0x75);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@���W�����v�A�g���r���[�g
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckumpLeft( const MAPATTR_VALUE val )
{
  return (val == 0x73);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�E�W�����v�A�g���r���[�g
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckJumpRight( const MAPATTR_VALUE val )
{
  return (val == 0x72);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@����
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckSandStream( const MAPATTR_VALUE val )
{
  return (val == 0x7c);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �J�E���^�[
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckCounter( const MAPATTR_VALUE val )
{
  return (val == 0xd4);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �}�b�g
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckMat( const MAPATTR_VALUE val )
{
  return (val == 0xd5);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �p�\�R��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckPC( const MAPATTR_VALUE val )
{
  return (val == 0xd6);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���E�n�}
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckMap( const MAPATTR_VALUE val )
{
  return (val == 0xd7);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �e���r
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckTV( const MAPATTR_VALUE val )
{
  return (val == 0xd8);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �{�I���̂P
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckBookShelf1( const MAPATTR_VALUE val )
{
  return (val == 0xd9);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �{�I���̂Q
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckBookShelf2( const MAPATTR_VALUE val )
{
  return (val == 0xda);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �{�I���̂R
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckBookShelf3( const MAPATTR_VALUE val )
{
  return (val == 0xdb);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �{�I���̂S
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckBookShelf4( const MAPATTR_VALUE val )
{
  return (val == 0xdc);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckVase( const MAPATTR_VALUE val )
{
  return (val == 0xdd);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N �S�~��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckDustBox( const MAPATTR_VALUE val )
{
  return (val == 0xde);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N  ���i�I���̂P
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckShopShelf1( const MAPATTR_VALUE val )
{
  return (val == 0xdf);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N  ���i�I���̂Q
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckShopShelf2( const MAPATTR_VALUE val )
{
  return (val == 0xe0);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N  ���i�I���̂R
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckShopShelf3( const MAPATTR_VALUE val )
{
  return (val == 0xe1);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���ނ�G���J�E���gA1
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckEncountGrassA1( const MAPATTR_VALUE val )
{
  return ( (val == 0x04) || (val == 0xa1) );  // 0xa1���[������~�葐�ނ�
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���ނ�G���J�E���gA2
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckEncountGrassA2( const MAPATTR_VALUE val )
{
  return (val == 0x05);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���ނ�G���J�E���gB1
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckEncountGrassB1( const MAPATTR_VALUE val )
{
  return ( (val == 0x06) || (val == 0xa2) );  // 0xa2���[������~�葐�ނ�@��
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���ނ�G���J�E���gB2
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckEncountGrassB2( const MAPATTR_VALUE val )
{
  return (val == 0x07);
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���ނ�G���J�E���gA�Q(A1,A2)
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckEncountGrassA( const MAPATTR_VALUE val )
{
  if( MAPATTR_VALUE_CheckEncountGrassA1(val) == TRUE ||
      MAPATTR_VALUE_CheckEncountGrassA2(val) == TRUE ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���ނ�G���J�E���gB�Q(B1,B2)
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckEncountGrassB( const MAPATTR_VALUE val )
{
  if( MAPATTR_VALUE_CheckEncountGrassB1(val) == TRUE ||
      MAPATTR_VALUE_CheckEncountGrassB2(val) == TRUE ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ���͌�
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckKairikiAna( const MAPATTR_VALUE val )
{
  return( val == 0x1d );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckShore( const MAPATTR_VALUE val )
{
  return( val == 0x41 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ������
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckPool( const MAPATTR_VALUE val )
{
  return( val == 0x14 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[ �`�F�b�N ��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckShoal( const MAPATTR_VALUE val )
{
  return( val == 0x17 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@��
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckWaterFall( const MAPATTR_VALUE val )
{
  return( val == 0x40 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�[����
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckSnow( const MAPATTR_VALUE val )
{
  return( val == 0x0e );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�ƂĂ��[����
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckDeepSnow( const MAPATTR_VALUE val )
{
  return( val == 0x0f );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@��n�A�g���r���[�g
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckSnowType( const MAPATTR_VALUE val )
{
  if( MAPATTR_VALUE_CheckSnow(val) || MAPATTR_VALUE_CheckDeepSnow(val) ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@�X
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckIce( const MAPATTR_VALUE val )
{
  return( val == 0x18 );
}

//--------------------------------------------------------------
/**
 * �A�g���r���[�g�o�����[�@�`�F�b�N�@����
 * @param val MAPATTR_VALUE
 * @retval BOOL FALSE=�Ⴄ
 */
//--------------------------------------------------------------
BOOL MAPATTR_VALUE_CheckMarsh( const MAPATTR_VALUE val )
{
  return( val == 0x1c );
}
