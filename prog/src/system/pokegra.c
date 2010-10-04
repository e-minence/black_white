//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		pokegra.c
 *	@brief	�|�P�����O���t�B�b�N�f�ނ̎擾
 *	@author	Toru=Nagihashi
 *	@data		2009.10.05
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�A�[�J�C�u
#include "arc_def.h"
#include "pokegra/pokegra_wb.naix"

//�|�P�c�[��
#include "poke_tool/monsno_def.h"
#include "poke_tool/poke_tool.h"

//�O���Q��
#include "system/pokegra.h"

//�p�b�`�[���u�`�쐬
#include "patch.cdat"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define POKEGRA_CELL01_X  8
#define POKEGRA_CELL01_Y  8
#define POKEGRA_CELL02_X  4
#define POKEGRA_CELL02_Y  8
#define POKEGRA_CELL03_X  8
#define POKEGRA_CELL03_Y  4
#define POKEGRA_CELL04_X  4
#define POKEGRA_CELL04_Y  4

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
static void PokeGra_GetFileOffset( ARCID arc_id , int mons_no, int form_no, int sex, int rare, int dir, BOOL egg, u32 *p_mons_offset, u32 *p_dir_offset, u32 *p_sex_offset, u32 *p_rare_offset, u32 *pltt_only, BOOL ncgr_flag );

//=============================================================================
/**
 *		���\�[�X�擾�֐�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	POKEGRA�̃A�[�J�C�uID
 *		
 *	@return	�A�[�J�C�uID
 */
//-----------------------------------------------------------------------------
ARCID POKEGRA_GetArcID( void )
{	
  return 0;
  //	return ARCID_POKEGRA;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	�L�����A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetCgrArcIndex( ARCID arc_id , int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
	u32 mons_offset;
	u32 dir_offset;
	u32 sex_offset;

  PokeGra_GetFileOffset( arc_id , mons_no, form_no, sex, rare, dir, egg, &mons_offset, &dir_offset, &sex_offset, NULL, NULL, TRUE );
  return mons_offset + dir_offset + POKEGRA_M_NCGR + sex_offset;
}
//----------------------------------------------------------------------------
/**
 *	@brief	
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	�L�����r�b�g�}�b�v�A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetCbrArcIndex( ARCID arc_id , int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
	u32 mons_offset;
	u32 dir_offset;
	u32 sex_offset;

  PokeGra_GetFileOffset( arc_id , mons_no, form_no, sex, rare, dir, egg, &mons_offset, &dir_offset, &sex_offset, NULL, NULL, FALSE );
  return mons_offset + dir_offset + POKEGRA_M_NCBR + sex_offset;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	��گăA�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetPalArcIndex( ARCID arc_id , int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
  u32 mons_offset;
  u32 rare_offset;
  u32 pltt_only = 0;
  PokeGra_GetFileOffset( arc_id , mons_no, form_no, sex, rare, dir, egg, &mons_offset, NULL, NULL, &rare_offset, &pltt_only, FALSE );
  if( pltt_only )
  { 
    return pltt_only;
  }
  return mons_offset + POKEGRA_NORMAL_NCLR + rare_offset;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	�Z���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetCelArcIndex( ARCID arc_id , int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
  u32 mons_offset;
  u32 dir_offset;

  PokeGra_GetFileOffset( arc_id , mons_no, form_no, sex, rare, dir, egg, &mons_offset, &dir_offset, NULL, NULL, NULL, FALSE );
  return mons_offset + dir_offset + POKEGRA_NCER;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	�Z���A�j���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetAnmArcIndex( ARCID arc_id , int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
	u32 mons_offset;
	u32 dir_offset;

	PokeGra_GetFileOffset( arc_id , mons_no, form_no, sex, rare, dir, egg, &mons_offset, &dir_offset, NULL, NULL, NULL, FALSE );

	return mons_offset + dir_offset + POKEGRA_NANR;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	�}���`�Z���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetMCelArcIndex( ARCID arc_id , int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
	u32 mons_offset;
	u32 dir_offset;

	PokeGra_GetFileOffset( arc_id , mons_no, form_no, sex, rare, dir, egg, &mons_offset, &dir_offset, NULL, NULL, NULL, FALSE );

	return mons_offset + dir_offset + POKEGRA_NMCR;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	�}���`�Z���A�j���A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetMAnmArcIndex( ARCID arc_id , int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
	u32 mons_offset;
	u32 dir_offset;

	PokeGra_GetFileOffset( arc_id , mons_no, form_no, sex, rare, dir, egg, &mons_offset, &dir_offset, NULL, NULL, NULL, FALSE );

	return mons_offset + dir_offset + POKEGRA_NMAR;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 * @param	mons_no �����X�^�[�i���o�[
 * @param	form_no �t�H�����i���o�[
 * @param	sex		����
 * @param	rare	���A���ǂ���
 * @param	dir		�|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *
 *	@return	�A�[�J�C�u�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
ARCDATID POKEGRA_GetNcecArcIndex( ARCID arc_id , int mons_no, int form_no, int sex, int rare, int dir, BOOL egg )
{	
	u32 mons_offset;
	u32 dir_offset;

	PokeGra_GetFileOffset( arc_id , mons_no, form_no, sex, rare, dir, egg, &mons_offset, &dir_offset, NULL, NULL, NULL, FALSE );

	return mons_offset + dir_offset + POKEGRA_NCEC;
}

//----------------------------------------------------------------------------
/**
 *	@brief  2D�p�L�����̓Z�������̂��߁A���ёւ�����Ă���̂ŁA
 *          �����BG�Ŏg����悤�ɕ��ђ����֐�
 *
 *	@param	NNSG2dCharacterData *p_chara �L����
 *	@param  heapID  �e���|�����쐬�p�q�[�v
 */
//-----------------------------------------------------------------------------
void POKEGRA_SortBGCharacter( NNSG2dCharacterData *p_chara, HEAPID heapID )
{ 
  //�e���|�����ɃL�������R�s�[
  u8 *p_buff  = p_chara->pRawData;
  u8 *p_temp  = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(heapID), POKEGRA_POKEMON_CHARA_SIZE );
  MI_CpuCopyFast( p_buff, p_temp, POKEGRA_POKEMON_CHARA_SIZE );

  //8x8,4*8,8*4,4*4��4��OAM�ɂ��킹��悤�ɓ]��
  {
    int i;
    u32 dst_pos, src_pos;

    dst_pos = 0;
    src_pos = 0;

    //8*8��]��
    for( i = 0; i < POKEGRA_CELL01_Y; i++ )
    { 
      dst_pos = (i * POKEGRA_POKEMON_CHARA_WIDTH) * GFL_BG_1CHRDATASIZ;
      MI_CpuCopyFast( &p_temp[src_pos], &p_buff[dst_pos], POKEGRA_CELL01_X*GFL_BG_1CHRDATASIZ );
      src_pos += POKEGRA_CELL01_X*GFL_BG_1CHRDATASIZ;
    }
    //4*8��]��
    for( i = 0; i < POKEGRA_CELL02_Y; i++ )
    { 
      dst_pos = (POKEGRA_CELL01_X + i * POKEGRA_POKEMON_CHARA_WIDTH) * GFL_BG_1CHRDATASIZ;
      MI_CpuCopyFast( &p_temp[src_pos], &p_buff[dst_pos], POKEGRA_CELL02_X*GFL_BG_1CHRDATASIZ );
      src_pos += POKEGRA_CELL02_X*GFL_BG_1CHRDATASIZ;
    }
    //8*4��]��
    for( i = 0; i < POKEGRA_CELL03_Y; i++ )
    { 
      dst_pos = ((POKEGRA_CELL01_Y + i) * POKEGRA_POKEMON_CHARA_WIDTH) * GFL_BG_1CHRDATASIZ;
      MI_CpuCopyFast( &p_temp[src_pos], &p_buff[dst_pos], POKEGRA_CELL03_X*GFL_BG_1CHRDATASIZ );
      src_pos += POKEGRA_CELL03_X*GFL_BG_1CHRDATASIZ;
    }
    //4*4��]��
    for( i = 0; i < POKEGRA_CELL04_Y; i++ )
    { 
      dst_pos = (POKEGRA_CELL03_X + (POKEGRA_CELL02_Y + i) 
                * POKEGRA_POKEMON_CHARA_WIDTH) * GFL_BG_1CHRDATASIZ;
      MI_CpuCopyFast( &p_temp[src_pos], &p_buff[dst_pos], POKEGRA_CELL04_X*GFL_BG_1CHRDATASIZ );
      src_pos += POKEGRA_CELL04_X*GFL_BG_1CHRDATASIZ;
    }
  }

  //�ꎞ�o�b�t�@�폜
  GFL_HEAP_FreeMemory( p_temp );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ��L�ŕ��ђ��������̂����Ƃ̕��тɖ߂��֐�
 *
 *	@param	NNSG2dCharacterData *p_chara �L����
 *	@param  heapID  �e���|�����쐬�p�q�[�v
 */
//-----------------------------------------------------------------------------
void POKEGRA_SortOBJCharacter( NNSG2dCharacterData *p_chara, HEAPID heapID )
{ 
  //�e���|�����ɃL�������R�s�[
  u8 *p_buff  = p_chara->pRawData;
  u8 *p_temp  = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(heapID), POKEGRA_POKEMON_CHARA_SIZE );
  MI_CpuCopyFast( p_buff, p_temp, POKEGRA_POKEMON_CHARA_SIZE );

  //8x8,4*8,8*4,4*4��4��OAM�ɂ��킹��悤�ɓ]��
  {
    int i;
    u32 dst_pos, src_pos;

    dst_pos = 0;
    src_pos = 0;

    //8*8��]��
    for( i = 0; i < POKEGRA_CELL01_Y; i++ )
    { 
      src_pos = (i * POKEGRA_POKEMON_CHARA_WIDTH) * GFL_BG_1CHRDATASIZ;
      MI_CpuCopyFast( &p_temp[src_pos], &p_buff[dst_pos], POKEGRA_CELL01_X*GFL_BG_1CHRDATASIZ );
      dst_pos += POKEGRA_CELL01_X*GFL_BG_1CHRDATASIZ;
    }
    //4*8��]��
    for( i = 0; i < POKEGRA_CELL02_Y; i++ )
    { 
      src_pos = (POKEGRA_CELL01_X + i * POKEGRA_POKEMON_CHARA_WIDTH) * GFL_BG_1CHRDATASIZ;
      MI_CpuCopyFast( &p_temp[src_pos], &p_buff[dst_pos], POKEGRA_CELL02_X*GFL_BG_1CHRDATASIZ );
      dst_pos += POKEGRA_CELL02_X*GFL_BG_1CHRDATASIZ;
    }
    //8*4��]��
    for( i = 0; i < POKEGRA_CELL03_Y; i++ )
    { 
      src_pos = ((POKEGRA_CELL01_Y + i) * POKEGRA_POKEMON_CHARA_WIDTH) * GFL_BG_1CHRDATASIZ;
      MI_CpuCopyFast( &p_temp[src_pos], &p_buff[dst_pos], POKEGRA_CELL03_X*GFL_BG_1CHRDATASIZ );
      dst_pos += POKEGRA_CELL03_X*GFL_BG_1CHRDATASIZ;
    }
    //4*4��]��
    for( i = 0; i < POKEGRA_CELL04_Y; i++ )
    { 
      src_pos = (POKEGRA_CELL03_X + (POKEGRA_CELL02_Y + i) 
                * POKEGRA_POKEMON_CHARA_WIDTH) * GFL_BG_1CHRDATASIZ;
      MI_CpuCopyFast( &p_temp[src_pos], &p_buff[dst_pos], POKEGRA_CELL04_X*GFL_BG_1CHRDATASIZ );
      dst_pos += POKEGRA_CELL04_X*GFL_BG_1CHRDATASIZ;
    }
  }

  //�ꎞ�o�b�t�@�폜
  GFL_HEAP_FreeMemory( p_temp );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �p�b�`�[���̂��߂̃u�`���L�����ɏ�������(BG�L�����̕��т̂Ƃ���z��)
 *
 *	@param	NNSG2dCharacterData *p_chara  �L�����f�[�^
 */
//-----------------------------------------------------------------------------
void POKEGRA_MakePattiiruBuchi( NNSG2dCharacterData *p_chara, u32 personal_rnd )
{ 
  const	PATTIIRU_BUCHI_DATA	*pbd;
  int i, j;
  int	setx, sety, cnt;
  int	pos;
  u32 rnd = personal_rnd;
  u8  *buf = p_chara->pRawData;

  for( i = 0 ; i < 4 ; i++ )
  {
    pbd = pbd_table[ i ];
    cnt=0;
    while( pbd[ cnt ].posx != 0xff )
    {
      setx = pbd[ cnt ].posx - OFS_X + 8 - 2 + ( ( (rnd) & 0x0f ) - 8 );
      sety = pbd[ cnt ].posy - OFS_Y + 8     + ( ( ( (rnd) & 0xf0 ) >> 4 ) - 8 );

      pos = (setx / 8 + (sety / 8) * 12 ) * 64  + ((setx) % 8 + (sety % 8) * 8) ;
      pos /= 2;

      //NAGI_Printf( "[%d]=x[%d]y[%d]\n", i, setx, sety );

      if( setx & 1 )
      {
        if( ( ( buf[ pos ] & 0xf0 ) >= 0x10 ) && ( ( buf[ pos ] & 0xf0 ) <= 0x30) )
        {
          buf[ pos ] += 0x50;
        }
      }
      else
      {
        if( ( ( buf[ pos ] & 0x0f ) >= 0x01 ) && ( ( buf[ pos ] & 0x0f ) <= 0x03 ) )
        {
          buf[ pos ] += 0x05;
        }
      }
      cnt++;
    }
    (rnd) = (rnd) >> 8;
  }
}

//=============================================================================
/**
 *	private
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	 mons_no	�����X�^�[�i���o�[
 *	@param	form_no		�t�H�����i���o�[
 *	@param	sex				����
 *	@param	rare			���A���ǂ���
 *	@param	dir		    �|�P�����̌����iPOKEGRA_DIR_FRONT:���ʁAPOKEGRA_DIR_BACK:�w�ʁj
 *	@param  egg       �^�}�S���ǂ���
 *	@param	*p_mons_offset				�|�P�����ԍ��t�@�C���I�t�Z�b�g
 *	@param	*p_dir_offset			�w�ʃI�t�Z�b�g
 *	@param	*p_sex_offset				���ʃI�t�Z�b�g
 *	@param	u32 *p_rare_offset	���A�I�t�Z�b�g
 *	@param	*pltt_only_offset    �ʃt�H�������p���b�g�݂̂̂Ƃ��̃I�t�Z�b�g
 *	@param	ncgr_flag           ���Y�����������f��ncgr�t�@�C���ōs�����ǂ����H
 */
//-----------------------------------------------------------------------------
static void PokeGra_GetFileOffset( ARCID arc_id , int mons_no, int form_no, int sex, int rare, int dir, BOOL egg, u32 *p_mons_offset, u32 *p_dir_offset, u32 *p_sex_offset, u32 *p_rare_offset, u32* pltt_only_offset, BOOL ncgr_flag )
{	
	u32 file_start;
	u32 file_offset;
	u32 mf_ratio;

  file_start	= POKEGRA_FILE_MAX * mons_no;
  file_offset	= (dir == POKEGRA_DIR_FRONT) ? POKEGRA_FRONT_M_NCGR: POKEGRA_BACK_M_NCGR;
  if( ncgr_flag )
  { 
    mf_ratio	= (dir == POKEGRA_DIR_FRONT) ? POKEGRA_FRONT_M_NCGR: POKEGRA_BACK_M_NCGR;
  }
  else
  { 
    mf_ratio	= (dir == POKEGRA_DIR_FRONT) ? POKEGRA_FRONT_M_NCBR: POKEGRA_BACK_M_NCBR;
  }

  //�^�}�S�`�F�b�N
  if( egg )
  { 
    if( mons_no == MONSNO_MANAFI )
    { 
      form_no = FORMNO_TAMAGO_MANAFI;
    }
    else
    { 
      form_no = FORMNO_TAMAGO_NORMAL;
    }
    file_start = POKEGRA_FILE_MAX * ( MONSNO_TAMAGO + form_no );
  }
  //�ʃt�H��������
  else if( form_no )
  { 
    int gra_index = POKETOOL_GetPersonalParam( mons_no, 0, POKEPER_ID_form_gra_index );
    int pltt_only = POKETOOL_GetPersonalParam( mons_no, 0, POKEPER_ID_pltt_only );
    int form_max = POKETOOL_GetPersonalParam( mons_no, 0, POKEPER_ID_form_max );
    if( form_no >= form_max )
    { 
      form_no = 0;
    }
    if( pltt_only )
    { 
      if( ( pltt_only_offset ) && ( form_no ) )
      { 
        *pltt_only_offset = POKEGRA_FILE_MAX * ( MONSNO_MAX + OTHER_FORM_MAX + 1 ) + 13 + POKEGRA_PLTT_ONLY_MAX * ( gra_index + form_no - 1 ) + rare;
      }
    }
    else
    { 
      if( form_no )
      { 
        file_start = POKEGRA_FILE_MAX * ( MONSNO_MAX + 1 ) + POKEGRA_FILE_MAX * ( gra_index + form_no - 1 );
      }
    }
  }

  //���ʂ̃`�F�b�N
  switch( sex ){
  case PTL_SEX_MALE:
    break;
  case PTL_SEX_FEMALE:
    //�I�X���X�����������Ă��邩�`�F�b�N����i�T�C�Y���O�Ȃ珑�������Ȃ��j
    sex = ( GFL_ARC_GetDataSize( arc_id, file_start + mf_ratio + 1 ) == 0 ) ? PTL_SEX_MALE : PTL_SEX_FEMALE;
    break;
  case PTL_SEX_UNKNOWN:
    //���ʂȂ��́A�I�X�����ɂ���
    sex = PTL_SEX_MALE;
    break;
  default:
    //���肦�Ȃ�����
    GF_ASSERT(0);
    break;
  }

  //�󂯎��
  if( p_mons_offset )
  {	
    *p_mons_offset	= file_start;
  }
  if( p_dir_offset )
	{	
		*p_dir_offset		= file_offset;
	}
	if( p_sex_offset )
	{	
		*p_sex_offset		= sex;
	}
	if( p_rare_offset )
	{	
		*p_rare_offset	= rare;
	}
}
