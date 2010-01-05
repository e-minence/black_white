//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_bbd_color.c
 *	@brief  �r���{�[�h�@�J���[�@�f�[�^
 *	@author	tomoya takahashi
 *	@date		2010.01.05
 *
 *	���W���[�����FFLD_BBD_COLOR
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "field_bbd_color.h"

#include "arc/arc_def.h"
#include "arc/fieldmap/area_map_mmdl_color.naix"

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
///	
//=====================================
typedef struct 
{
  fx32 norm_x;
  fx32 norm_y;
  fx32 norm_z;

  u16 def_r;
  u16 def_g;
  u16 def_b;

  u16 amb_r;
  u16 amb_g;
  u16 amb_b;

  u16 spq_r;
  u16 spq_g;
  u16 spq_b;

  u16 emi_r;
  u16 emi_g;
  u16 emi_b;

} FLD_BBD_COLOER_DATA;


//-------------------------------------
///	�r���{�[�h�J���[�ݒ�V�X�e��
//=====================================
struct _FLD_BBD_COLOR 
{
  ARCHANDLE* p_handle;
  FLD_BBD_COLOER_DATA buff;
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------



//----------------------------------------------------------------------------
/**
 *	@brief  �r���{�[�h�J���[�@�Ǘ��V�X�e������
 *
 *	@param	heapID    �q�[�vID
 *
 *	@return �V�X�e��
 */
//-----------------------------------------------------------------------------
FLD_BBD_COLOR* FLD_BBD_COLOR_Create( HEAPID heapID )
{
  FLD_BBD_COLOR* p_wk;

  p_wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLD_BBD_COLOR) );

  p_wk->p_handle = GFL_ARC_OpenDataHandle( ARCID_FLD_BBD_COLOR, heapID );
  
  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �r���{�[�h�J���[�@�Ǘ��V�X�e���j��
 *
 *	@param	p_wk    ���[�N
 */
//-----------------------------------------------------------------------------
void FLD_BBD_COLOR_Delete( FLD_BBD_COLOR* p_wk )
{
  GFL_ARC_CloseDataHandle( p_wk->p_handle );
  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �r���{�[�h�J���[�@�Ǘ��V�X�e��  �ǂݍ���
 *
 *	@param	p_wk    ���[�N
 *	@param	idx     �C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void FLD_BBD_COLOR_Load( FLD_BBD_COLOR* p_wk, u32 idx )
{
  GFL_ARC_LoadDataByHandle( p_wk->p_handle, idx, &p_wk->buff );

  // �ǂݍ��ݏ��̏o��
  TOMOYA_Printf( "Normal x[0x%x] y[0x%x] z[0x%x]\n", p_wk->buff.norm_x, p_wk->buff.norm_y, p_wk->buff.norm_z );
  TOMOYA_Printf( "def r[%d] g[%d] b[%d]\n", p_wk->buff.def_r, p_wk->buff.def_g, p_wk->buff.def_b );
  TOMOYA_Printf( "amb r[%d] g[%d] b[%d]\n", p_wk->buff.amb_r, p_wk->buff.amb_g, p_wk->buff.amb_b );
  TOMOYA_Printf( "spc r[%d] g[%d] b[%d]\n", p_wk->buff.spq_r, p_wk->buff.spq_g, p_wk->buff.spq_b );
  TOMOYA_Printf( "emi r[%d] g[%d] b[%d]\n", p_wk->buff.emi_r, p_wk->buff.emi_g, p_wk->buff.emi_b );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �r���{�[�h�J���[�@�Ǘ��V�X�e��  �f�[�^�ݒ�
 *
 *	@param	cp_wk        ���[�N
 *	@param	p_bbdsys    �ݒ��r���{�[�h�V�X�e��
 */
//-----------------------------------------------------------------------------
void FLD_BBD_COLOR_SetData( const FLD_BBD_COLOR* cp_wk, GFL_BBD_SYS* p_bbdsys )
{
  VecFx16 n;
  GXRgb def;
  GXRgb amb;
  GXRgb spq;
  GXRgb emi;
  
  // �e��f�[�^�ݒ�
  {
    // �@��
    n.x = cp_wk->buff.norm_x; 
    n.y = cp_wk->buff.norm_y;
    n.z = cp_wk->buff.norm_z; 
    VEC_Fx16Normalize( &n, &n );
    GFL_BBD_SetUseCustomVecN( p_bbdsys, &n );

    // �f�B�t���[�Y
    def = GX_RGB( cp_wk->buff.def_r, cp_wk->buff.def_g, cp_wk->buff.def_b );
    GFL_BBD_SetDiffuse( p_bbdsys, &def );

    // �A���r�G���g 
    amb = GX_RGB( cp_wk->buff.amb_r, cp_wk->buff.amb_g, cp_wk->buff.amb_b );
    GFL_BBD_SetAmbient( p_bbdsys, &amb );

    // �X�y�L�����[
    spq = GX_RGB( cp_wk->buff.spq_r, cp_wk->buff.spq_g, cp_wk->buff.spq_b );
    GFL_BBD_SetSpecular( p_bbdsys, &spq );

    // �G�~�b�V����
    emi = GX_RGB( cp_wk->buff.emi_r, cp_wk->buff.emi_g, cp_wk->buff.emi_b );
    GFL_BBD_SetEmission( p_bbdsys, &emi );
  }
}



