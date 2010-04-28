//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		tpoke_data.c
 *	@brief  �A������|�P�������  OBJCODE �傫��
 *	@author	tomoya takahashi
 *	@date		2010.03.23
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "arc_def.h"
#include "fieldmap/tpoke_data.naix"
#include "fieldmap/fldmmdl_mdlparam.naix"

#include "field/tpoke_data.h"

#include "fldmmdl.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�A������|�P�����P�f�[�^
//=====================================
typedef struct {
  u16 monsno;     // �����X�^�[�i���o�[
  u16 sex;        // ����
  u16 formno;     // �t�H�����i���o�[
  u16 objcode;    // �I�u�W�F�R�[�h
} TPOKE_DAT;


//-------------------------------------
///	�A������|�P�������
//=====================================
struct _TPOKE_DATA {
  TPOKE_DAT* p_data;
  u8* p_objcodeParamBuf;
  OBJCODE_PARAM* p_objcodeParam;
  u32 num;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

static int TPokeData_GetIndex( const TPOKE_DATA* cp_wk, u16 monsno, u16 sex, u16 formno );

static const OBJCODE_PARAM* TPokeData_GetObjCodeParam( const TPOKE_DATA* cp_wk, u16 objcode );


//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�ǂݍ���
 *
 *	@param	heapID  �q�[�vID
 *
 *	@return �f�[�^
 */
//-----------------------------------------------------------------------------
TPOKE_DATA* TPOKE_DATA_Create( HEAPID heapID )
{
  TPOKE_DATA* p_wk;
  u32 size;

  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(TPOKE_DATA) );

  p_wk->p_data = GFL_ARC_UTIL_LoadEx( ARCID_TPOKE_DATA, NARC_tpoke_data_tpoke_dat, FALSE, heapID, &size );
  GF_ASSERT( (size % sizeof(TPOKE_DAT)) == 0 );
  p_wk->num = size / sizeof(TPOKE_DAT);


  // �I�u�W�F�R�[�h�p�����[�^
  p_wk->p_objcodeParamBuf = GFL_ARC_LoadDataAlloc( ARCID_MMDL_PARAM, 
      NARC_fldmmdl_mdlparam_fldmmdl_mdlparam_bin, heapID );
  p_wk->p_objcodeParam = (OBJCODE_PARAM*)(&p_wk->p_objcodeParamBuf[OBJCODE_PARAM_TOTAL_NUMBER_SIZE]);

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�j��
 *
 *	@param	p_wk  ���[�N
 */
//-----------------------------------------------------------------------------
void TPOKE_DATA_Delete( TPOKE_DATA* p_wk )
{
  GFL_HEAP_FreeMemory( p_wk->p_objcodeParamBuf );
  GFL_HEAP_FreeMemory( p_wk->p_data );
  GFL_HEAP_FreeMemory( p_wk );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �I�u�W�F�N�g�R�[�h���擾����
 *
 *	@param	cp_wk   ���[�N
 *	@param	monsno  �����X�^�[�i���o�[
 *	@param	sex     ����
 *	@param	formno  �t�H�[��NO
 *
 *	@return �I�u�W�F�R�[�h
 */
//-----------------------------------------------------------------------------
u16 TPOKE_DATA_GetObjCode( const TPOKE_DATA* cp_wk, u16 monsno, u16 sex, u16 formno )
{
  int index;
  index = TPokeData_GetIndex( cp_wk, monsno, sex, formno );
  return cp_wk->p_data[ index ].objcode;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���f���傫���`�F�b�N  �傫���T�C�Y���H
 *
 *  @param  cp_gdata�Q�[���f�[�^
 *	@param	cp_wk   ���[�N
 *	@param	monsno  �����X�^�[�i���o�[
 *	@param	sex     ����
 *	@param	formno  �t�H�[��NO
 *
 *	@retval TRUE    �傫��
 *	@retval FALSE   �ʏ�̑傫�� 
 */
//-----------------------------------------------------------------------------
BOOL TPOKE_DATA_IsSizeBig( const GAMEDATA* cp_gdata, const TPOKE_DATA* cp_wk, u16 monsno, u16 sex, u16 formno )
{
  int index;
  const OBJCODE_PARAM * cp_objcode_para;
  
  
  index = TPokeData_GetIndex( cp_wk, monsno, sex, formno );

  // �ړI�̃I�u�W�F�R�[�h�p�����[�^�擾
  cp_objcode_para = TPokeData_GetObjCodeParam( cp_wk, cp_wk->p_data[ index ].objcode );
  
  if( cp_objcode_para->mdl_size == MMDL_BLACT_MDLSIZE_64x64 ){
    return TRUE;
  }

#ifdef PM_DEBUG
  // CHECK 16x16
  if( cp_objcode_para->mdl_size == MMDL_BLACT_MDLSIZE_16x16 ){
    GF_ASSERT( cp_objcode_para->mdl_size != MMDL_BLACT_MDLSIZE_16x16 );
    return FALSE;
  }
#endif
  
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���f���傫���`�F�b�N  �ʏ�T�C�Y���H
 *
 *	@param	cp_wk   ���[�N
 *	@param	monsno  �����X�^�[�i���o�[
 *	@param	sex     ����
 *	@param	formno  �t�H�[��NO
 *
 *	@retval TRUE    �ʏ�̑傫��
 *	@retval FALSE   �傫��
 */
//-----------------------------------------------------------------------------
BOOL TPOKE_DATA_IsSizeNormal( const GAMEDATA* cp_gdata, const TPOKE_DATA* cp_wk, u16 monsno, u16 sex, u16 formno )
{
  int index;
  const OBJCODE_PARAM * cp_objcode_para;
  
  
  index = TPokeData_GetIndex( cp_wk, monsno, sex, formno );

  // �ړI�̃I�u�W�F�R�[�h�p�����[�^�擾
  cp_objcode_para = TPokeData_GetObjCodeParam( cp_wk, cp_wk->p_data[ index ].objcode );
  
  if( cp_objcode_para->mdl_size == MMDL_BLACT_MDLSIZE_32x32 ){
    return TRUE;
  }

#ifdef PM_DEBUG
  // CHECK 16x16
  if( cp_objcode_para->mdl_size == MMDL_BLACT_MDLSIZE_16x16 ){
    GF_ASSERT( cp_objcode_para->mdl_size != MMDL_BLACT_MDLSIZE_16x16 );
    return FALSE;
  }
#endif
  
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�[�^�C���f�b�N�X���擾
 *
 *	@param	cp_wk   ���[�N
 *	@param	monsno  �����X�^�[�i���o�[
 *	@param	sex     ����
 *	@param	formno  �t�H�[��
 *
 *	@return �f�[�^�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
int TPOKE_DATA_GetIndex( const TPOKE_DATA* cp_wk, u16 monsno, u16 sex, u16 formno )
{
  int i;

  for( i=0; i<cp_wk->num; i++ ){

    // �����X�^�[�i���o�[
    if( cp_wk->p_data[i].monsno != monsno ){
      continue;
    }

    // ����
    if( (cp_wk->p_data[i].sex != PM_NEUTRAL) ){ // NEUTRAL�ȊO�Ȃ�`�F�b�N����
      if( cp_wk->p_data[i].sex != sex ){
        continue;
      }
    }

    // �t�H����
    if( cp_wk->p_data[i].formno != formno ){ 
      continue;
    }
    
    return i;
  }

  // �݂���Ȃ��E�E�E
  return TPOKE_DATA_INDEX_NG;
}





//-----------------------------------------------------------------------------
/**
 *      private�֐�
 */
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 * TPOKE_DATA_GetIndex�̃��b�p�[�֐�
 *
 * �����i�łɂ����āA�C���f�b�N�X��������Ȃ��ꍇ��0��Ԃ��ăG���[���������
 */
//-----------------------------------------------------------------------------
static int TPokeData_GetIndex( const TPOKE_DATA* cp_wk, u16 monsno, u16 sex, u16 formno )
{
  int index = TPOKE_DATA_GetIndex( cp_wk, monsno, sex, formno );
  if (index < 0 )
  {
    GF_ASSERT( 0 );
    return 0; // �t���[�Y���
  }
  return index;
}


//----------------------------------------------------------------------------
/**
 *	@brief  objcode�̃I�u�W�F�R�[�h�p�����[�^�擾
 *
 *	@param	cp_wk     ���[�N
 *	@param	objcode   �I�u�W�F�R�[�h
 */
//-----------------------------------------------------------------------------
static const OBJCODE_PARAM* TPokeData_GetObjCodeParam( const TPOKE_DATA* cp_wk, u16 objcode )
{
  int code;
  
  code = MMDL_TOOL_OBJCodeToDataNumber( objcode );
  return( &(cp_wk->p_objcodeParam[code]) );
}



