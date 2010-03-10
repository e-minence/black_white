//==============================================================================
/**
 * @file	pokeicon.c
 * @brief	�|�P�����A�C�R��
 * @author	matsuda changed by soga�i�V���`�ł̃t�H�����Ⴂ�ƃI�X���X���������ɑΉ��j
 * @date	2008.11.25(��)
 */
//==============================================================================
#include <gflib.h>
#include "arc_def.h"
#include "poke_tool/poke_personal.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

#include "pokeicon/pokeicon.h"
#include "poke_icon.naix"

//#define GENDER_VER

//============================================================================================
//	�萔��`
//============================================================================================

enum{ 
  POKEICON_M_NCGR = 0,
  POKEICON_F_NCGR,

  POKEICON_FILE_MAX,
};

//==============================================================================
//	�f�[�^
//==============================================================================
#include "pokeicon.dat"

//--------------------------------------------------------------------------------------------
/**
 * �L�����̃A�[�J�C�u�C���f�b�N�X�擾 ( POKEMON_PASO_PARAM �� )
 *
 * @param	ppp			POKEMON_PASO_PARAM
 *
 * @return	�A�[�J�C�u�C���f�b�N�X
 *
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_GetCgxArcIndex( const POKEMON_PASO_PARAM* ppp )
{
	u32  monsno;
	u32  arcIndex;
	BOOL fastMode;
	u32  form_no;
	u32  sex;
	u32  egg;

	fastMode  = PPP_FastModeOn((POKEMON_PASO_PARAM*)ppp);
	monsno    = PPP_Get(ppp, ID_PARA_monsno, NULL );
	form_no   = PPP_Get( (POKEMON_PASO_PARAM*)ppp, ID_PARA_form_no, NULL );
	sex       = PPP_Get( (POKEMON_PASO_PARAM*)ppp, ID_PARA_sex, NULL );
	egg       = PPP_Get(ppp, ID_PARA_tamago_flag, NULL );

#ifdef GENDER_VER
	arcIndex = POKEICON_GetCgxArcIndexByMonsNumber( monsno, form_no, sex, egg );
#else
	arcIndex = POKEICON_GetCgxArcIndexByMonsNumber( monsno, form_no, egg );
#endif
	PPP_FastModeOff((POKEMON_PASO_PARAM*)ppp, fastMode);
	return arcIndex;
}

//--------------------------------------------------------------------------------------------
/**
 * �L�����̃A�[�J�C�u�C���f�b�N�X�擾
 *
 * @param	mons_no	�|�P�����ԍ�
 * @param	form_no	�t�H�����ԍ�
 * @param	sex		  ����
 * @param	egg			�^�}�S�t���O(TRUE=�^�}�S)
 *
 * @return	�A�[�J�C�u�C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
#ifdef GENDER_VER
u32 POKEICON_GetCgxArcIndexByMonsNumber( u32 mons_no, u32 form_no, u32 sex, BOOL egg )
#else
u32 POKEICON_GetCgxArcIndexByMonsNumber( u32 mons_no, u32 form_no, BOOL egg )
#endif
{
#ifdef PM_DEBUG
#if 0
	{
		if(GX_GetOBJVRamModeChar() != GX_OBJVRAMMODE_CHAR_1D_128K){
			GF_ASSERT(0 && "��Ή��̃}�b�s���O���[�h�ł�");
			/*---------------
			�|�P�����A�C�R����CGX��1D128K�ō���Ă���ׁA���̂܂܃L�����N�^��]�������
			�}�b�s���O���[�h���ύX����Ă��܂��܂��B
			128k�}�b�s���O���[�h�ȊO�ł��g�p�������ꍇ��
			NNS_G2dGetUnpackedCharacterData�ŃA���p�b�N�������
			pCharData->mapingType�̃}�b�s���O�^�C�v��ύX���Ă���
			NNS_G2dLoadImage1DMapping�œ]������悤�ɂ��Ă�������
			��������32k,64k�p�̃|�P�����A�C�R��CGX�f�[�^���A�[�J�C�u���Ɏ������I�����̈�ł�
			-----------------*/
		}
	}
#endif
#endif	//PM_DEBUG
	u32 file_start;

  file_start	= NARC_poke_icon_poke_icon_000_m_NCGR + POKEICON_FILE_MAX * mons_no;

  //�^�}�S�`�F�b�N
  if( egg )
  { 
    if( mons_no == MONSNO_MANAFI )
    { 
      form_no = 1;
    }
    else
    { 
      form_no = 0;
    }
    file_start = POKEICON_FILE_MAX * ( MONSNO_TAMAGO + form_no );
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
#if 0
      //@todo �A�C�R���̕��̃A���Z�E�X���ǂ����邩���܂��ĂȂ��̂Ō���͕ω��Ȃ��ɂ���
      if( pltt_only_offset )
      { 
        *pltt_only_offset = POKEICON_FILE_MAX * ( MONSNO_MAX + OTHER_FORM_MAX + 1 ) + 13 + POKEGRA_PLTT_ONLY_MAX * ( gra_index + form_no - 1 ) + rare;
      }
#else
      file_start = NARC_poke_icon_poke_icon_000_m_NCGR + POKEICON_FILE_MAX * ( MONSNO_MAX + 1 ) + POKEICON_FILE_MAX * ( gra_index + form_no - 1 );
#endif
    }
    else
    { 
      file_start = NARC_poke_icon_poke_icon_000_m_NCGR + POKEICON_FILE_MAX * ( MONSNO_MAX + 1 ) + POKEICON_FILE_MAX * ( gra_index + form_no - 1 );
    }
  }

#ifdef GENDER_VER
  //���ʂ̃`�F�b�N
  switch( sex ){
  case PTL_SEX_MALE:
    break;
  case PTL_SEX_FEMALE:
    //�I�X���X�����������Ă��邩�`�F�b�N����i�T�C�Y���O�Ȃ珑�������Ȃ��j
    sex = ( GFL_ARC_GetDataSize( ARCID_POKEICON, file_start + POKEICON_F_NCGR ) == 0 ) ? PTL_SEX_MALE : PTL_SEX_FEMALE;
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

	return ( file_start + sex );
#else
	return ( file_start );
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�ԍ��擾
 *
 * @param	mons_no		�|�P�����ԍ�
 * @param	form_no		�t�H�����ԍ�
 * @param	sex		    ����
 * @param	egg			  �^�}�S�t���O(TRUE:�^�}�S)
 *
 * @return	�p���b�g�ԍ�
 */
//--------------------------------------------------------------------------------------------
#ifdef GENDER_VER
const u8 POKEICON_GetPalNum( u32 mons_no, u32 form_no, u32 sex, BOOL egg )
#else
const u8 POKEICON_GetPalNum( u32 mons_no, u32 form_no, BOOL egg )
#endif
{
  //�^�}�S�`�F�b�N
  if( egg )
  { 
    if( mons_no == MONSNO_MANAFI )
    { 
      form_no = 1;
    }
    else
    { 
      form_no = 0;
    }
    mons_no = MONSNO_TAMAGO + form_no;
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
#if 0
      //@todo �A�C�R���̕��̃A���Z�E�X���ǂ����邩���܂��ĂȂ��̂Ō���͕ω��Ȃ��ɂ���
      if( pltt_only_offset )
      { 
        *pltt_only_offset = POKEICON_FILE_MAX * ( MONSNO_MAX + OTHER_FORM_MAX + 1 ) + 13 + POKEGRA_PLTT_ONLY_MAX * ( gra_index + form_no - 1 ) + rare;
      }
#else
      mons_no = POKEICON_FILE_MAX * ( MONSNO_MAX + 1 ) + POKEICON_FILE_MAX * ( gra_index + form_no - 1 );
#endif
    }
    else
    { 
      mons_no = POKEICON_FILE_MAX * ( MONSNO_MAX + 1 ) + POKEICON_FILE_MAX * ( gra_index + form_no - 1 );
    }
  }
#ifdef GENDER_VER
  if( sex )
  { 
	  return ( IconPalAtr[ mons_no ] & 0xf0 ) >> 4;
  }
#endif
	return IconPalAtr[ mons_no ] & 0x0f;
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�ԍ��擾�iPOKEMON_PASO_PARAM�Łj
 *
 * @param	ppp		POKEMON_PASO_PARAM
 *
 * @return	�p���b�g�ԍ�
 */
//--------------------------------------------------------------------------------------------
u8 POKEICON_GetPalNumGetByPPP( const POKEMON_PASO_PARAM * ppp )
{
	BOOL fast;
	u32  mons;
	u32  form;
	u32  sex;
	u32  egg;

	fast = PPP_FastModeOn( (POKEMON_PASO_PARAM *)ppp );

	mons = PPP_Get( (POKEMON_PASO_PARAM*)ppp, ID_PARA_monsno, NULL );
	form = PPP_Get( (POKEMON_PASO_PARAM*)ppp, ID_PARA_form_no, NULL );
	sex  = PPP_Get( (POKEMON_PASO_PARAM*)ppp, ID_PARA_sex, NULL );
	egg  = PPP_Get( (POKEMON_PASO_PARAM*)ppp, ID_PARA_tamago_flag, NULL );

	PPP_FastModeOff( (POKEMON_PASO_PARAM *)ppp, fast );

#ifdef GENDER_VER
	return POKEICON_GetPalNum( mons, form, sex, egg );
#else
	return POKEICON_GetPalNum( mons, form, egg );
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�̃A�[�J�C�u�C���f�b�N�X�擾
 *
 * @param	none
 *
 * @return	�A�[�J�C�u�C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_GetPalArcIndex(void)
{
	return NARC_poke_icon_poke_icon_NCLR;
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���̃A�[�J�C�u�C���f�b�N�X�擾
 *
 * @param	none
 *
 * @return	�A�[�J�C�u�C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_GetCellArcIndex(void)
{
	GXOBJVRamModeChar vrammode;
	
	vrammode = GX_GetOBJVRamModeChar();
	switch(vrammode){
	case GX_OBJVRAMMODE_CHAR_1D_32K:
		return NARC_poke_icon_poke_icon_32k_NCER;
	case GX_OBJVRAMMODE_CHAR_1D_64K:
		return NARC_poke_icon_poke_icon_64k_NCER;
	case GX_OBJVRAMMODE_CHAR_1D_128K:
		return NARC_poke_icon_poke_icon_128k_NCER;
	default:
		GF_ASSERT(0);	//��Ή��̃}�b�s���O���[�h
		return NARC_poke_icon_poke_icon_128k_NCER;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���̃A�[�J�C�u�C���f�b�N�X�擾
 *
 * @param	none
 *
 * @return	�A�[�J�C�u�C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_GetCellSubArcIndex(void)
{
	GXOBJVRamModeChar vrammode;
	
	vrammode = GXS_GetOBJVRamModeChar();
	switch(vrammode){
	case GX_OBJVRAMMODE_CHAR_1D_32K:
		return NARC_poke_icon_poke_icon_32k_NCER;
	case GX_OBJVRAMMODE_CHAR_1D_64K:
		return NARC_poke_icon_poke_icon_64k_NCER;
	case GX_OBJVRAMMODE_CHAR_1D_128K:
		return NARC_poke_icon_poke_icon_128k_NCER;
	default:
		GF_ASSERT(0);	//��Ή��̃}�b�s���O���[�h
		return NARC_poke_icon_poke_icon_128k_NCER;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���A�j���̃A�[�J�C�u�C���f�b�N�X�擾
 *
 * @param	none
 *
 * @return	�A�[�J�C�u�C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_GetAnmArcIndex(void)
{
	GXOBJVRamModeChar vrammode;
	
	vrammode = GX_GetOBJVRamModeChar();
	switch(vrammode){
	case GX_OBJVRAMMODE_CHAR_1D_32K:
		return NARC_poke_icon_poke_icon_32k_NANR;
	case GX_OBJVRAMMODE_CHAR_1D_64K:
		return NARC_poke_icon_poke_icon_64k_NANR;
	case GX_OBJVRAMMODE_CHAR_1D_128K:
		return NARC_poke_icon_poke_icon_128k_NANR;
	default:
		GF_ASSERT(0);	//��Ή��̃}�b�s���O���[�h
		return NARC_poke_icon_poke_icon_128k_NANR;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �Z���A�j���̃A�[�J�C�u�C���f�b�N�X�擾
 *
 * @param	none
 *
 * @return	�A�[�J�C�u�C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_GetAnmSubArcIndex(void)
{
	GXOBJVRamModeChar vrammode;
	
	vrammode = GXS_GetOBJVRamModeChar();
	switch(vrammode){
	case GX_OBJVRAMMODE_CHAR_1D_32K:
		return NARC_poke_icon_poke_icon_32k_NANR;
	case GX_OBJVRAMMODE_CHAR_1D_64K:
		return NARC_poke_icon_poke_icon_64k_NANR;
	case GX_OBJVRAMMODE_CHAR_1D_128K:
		return NARC_poke_icon_poke_icon_128k_NANR;
	default:
		GF_ASSERT(0);	//��Ή��̃}�b�s���O���[�h
		return NARC_poke_icon_poke_icon_128k_NANR;
	}
}
