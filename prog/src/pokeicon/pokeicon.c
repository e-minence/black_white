//==============================================================================
/**
 * @file	pokeicon.c
 * @brief	�|�P�����A�C�R��
 * @author	matsuda
 * @date	2008.11.25(��)
 */
//==============================================================================
#include <gflib.h>
#include "arc_def.h"
#include "poke_tool/poke_personal.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

#include "pokeicon.h"
#include "poke_icon.naix"


//============================================================================================
//	�萔��`
//============================================================================================
#define	POKEICON_GetPAL_TAMAGO			( 1 )	// �^�}�S�̃p���b�g�ԍ�
#define	POKEICON_GetPAL_TAMAGO_MNF		( 2 )	// �}�i�t�B�̃^�}�S�̃p���b�g�ԍ�

//�t�H�����ɂ���ăp���b�g���ς��|�P������IconPalAtr�e�[�u���Q�Ɣԍ�
#define	POKEICON_GetTAMAGO		( 494 )		// �^�}�S
#define	POKEICON_GetTAMAGO_MNF	( 495 )		// �}�i�t�B�̃^�}�S
#define	POKEICON_GetDEOKISISU	( 496 )		// �f�I�L�V�X
#define	POKEICON_GetANNOON		( 499 )		// �A���m�[��
#define	POKEICON_GetMINOMUTTI	( 527 )		// �~�m���b�`
#define	POKEICON_GetMINOMESU	( 529 )		// �~�m���X
#define	POKEICON_GetSIIUSI		( 531 )		// �V�[�E�V
#define	POKEICON_GetSIIDORUGO	( 532 )		// �V�[�h���S
#define	POKEICON_GetGIRATHINA	( 533 )		// �M���e�B�i
#define	POKEICON_GetSHEIMI		( 534 )		// �V�F�C�~
#define	POKEICON_GetROTOMU		( 535 )		// ���g��


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
	u32  egg;

	fastMode = PPP_FastModeOn((POKEMON_PASO_PARAM*)ppp);
	monsno = PPP_Get(ppp, ID_PARA_monsno, NULL );
	egg = PPP_Get(ppp, ID_PARA_tamago_flag, NULL );
	form_no = POKEICON_GetCgxForm(ppp);

	arcIndex = POKEICON_GetCgxArcIndexByMonsNumber( monsno, form_no, egg );
	PPP_FastModeOff((POKEMON_PASO_PARAM*)ppp, fastMode);
	return arcIndex;
}

//--------------------------------------------------------------------------------------------
/**
 * �L�����̃A�[�J�C�u�C���f�b�N�X�擾
 *
 * @param	mons		�|�P�����ԍ�
 * @param	form_no		�t�H�����ԍ�
 * @param	egg			�^�}�S�t���O(TRUE=�^�}�S)
 *
 * @return	�A�[�J�C�u�C���f�b�N�X
 *
 *	form_no�̓f�I�L�V�X��A���m�[���Ɏg�p
 */
//--------------------------------------------------------------------------------------------
u32 POKEICON_GetCgxArcIndexByMonsNumber( u32 mons, u32 form_no, BOOL egg )
{
#ifdef PM_DEBUG
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

	if( egg == TRUE ){
		if( mons == MONSNO_MANAFI ){
			return NARC_poke_icon_poke_icon_mnf_NCGR;
		}else{
			return NARC_poke_icon_poke_icon_tam_NCGR;
		}
	}

//	form_no = PokeFuseiFormNoCheck(mons, form_no);

	if( form_no != 0 ){
		if( mons == MONSNO_DEOKISISU ){
			return ( NARC_poke_icon_poke_icon_d01_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_ANNOON ){
			return ( NARC_poke_icon_poke_icon_u02_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_MINOMUTTI ){
			return ( NARC_poke_icon_poke_icon_455_01_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_MINOMADAMU ){
			return ( NARC_poke_icon_poke_icon_457_01_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_KARANAKUSI ){
			return ( NARC_poke_icon_poke_icon_458_01_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_TORITODON ){
			return ( NARC_poke_icon_poke_icon_459_01_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_GIRATHINA ){
			return ( NARC_poke_icon_poke_icon_509_01_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_SHEIMI ){
			return ( NARC_poke_icon_poke_icon_516_01_NCGR + form_no - 1 );
		}
		if( mons == MONSNO_ROTOMU ){
			return ( NARC_poke_icon_poke_icon_519_01_NCGR + form_no - 1 );
		}
	}

	if( mons > MONSNO_END ){ mons = 0; }

	return ( NARC_poke_icon_poke_icon_000_NCGR + mons );
}

//------------------------------------------------------------------
/**
 * �L�����̊g���p�^�[���i���o�[���擾
 *
 * @param   ppp		
 *
 * @retval  u16		�i���p�^�[���i���o�[
 */
//------------------------------------------------------------------
u16 POKEICON_GetCgxForm( const POKEMON_PASO_PARAM* ppp )
{
	u32 monsno;

	monsno = PPP_Get( ppp, ID_PARA_monsno_egg, NULL );

	switch( monsno ){
	case MONSNO_ANNOON:
	case MONSNO_DEOKISISU:
	case MONSNO_MINOMUTTI:
	case MONSNO_MINOMADAMU:
	case MONSNO_KARANAKUSI:
	case MONSNO_TORITODON:
	case MONSNO_GIRATHINA:
	case MONSNO_SHEIMI:
	case MONSNO_ROTOMU:
		return PPP_Get( (POKEMON_PASO_PARAM*)ppp, ID_PARA_form_no, NULL );

	default:
		return 0;
	}
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �p���b�g�ԍ��擾
 *
 * @param	mons		�|�P�����ԍ�
 * @param	form		�t�H�����ԍ�
 * @param	egg			�^�}�S�t���O(TRUE:�^�}�S)
 *
 * @return	�p���b�g�ԍ�
 */
//--------------------------------------------------------------------------------------------
const u8 POKEICON_GetPalNum( u32 mons, u32 form, BOOL egg )
{
	if( egg == 1 ){
		if( mons == MONSNO_MANAFI ){
			mons = POKEICON_GetTAMAGO_MNF;
		}else{
			mons = POKEICON_GetTAMAGO;
		}
	}else if( mons > MONSNO_END ){
		mons = 0;
	}else if( form != 0 ){
		if( mons == MONSNO_DEOKISISU ){
			mons = POKEICON_GetDEOKISISU + form - 1;
		}else if( mons == MONSNO_ANNOON ){
			mons = POKEICON_GetANNOON + form - 1;
		}else if( mons == MONSNO_MINOMUTTI ){
			mons = POKEICON_GetMINOMUTTI + form - 1;
		}else if( mons == MONSNO_MINOMADAMU ){
			mons = POKEICON_GetMINOMESU + form - 1;
		}else if( mons == MONSNO_KARANAKUSI ){
			mons = POKEICON_GetSIIUSI + form - 1;
		}else if( mons == MONSNO_TORITODON ){
			mons = POKEICON_GetSIIDORUGO + form - 1;
		}else if( mons == MONSNO_GIRATHINA ){
			mons = POKEICON_GetGIRATHINA + form - 1;
		}else if( mons == MONSNO_SHEIMI ){
			mons = POKEICON_GetSHEIMI + form - 1;
		}else if( mons == MONSNO_ROTOMU ){
			mons = POKEICON_GetROTOMU + form - 1;
		}
	}
	return IconPalAtr[mons];
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
	u32  egg;

	fast = PPP_FastModeOn( (POKEMON_PASO_PARAM *)ppp );

	form = POKEICON_GetCgxForm( ppp );
	mons = PPP_Get( (POKEMON_PASO_PARAM*)ppp, ID_PARA_monsno, NULL );
	egg  = PPP_Get( (POKEMON_PASO_PARAM*)ppp, ID_PARA_tamago_flag, NULL );

	PPP_FastModeOff( (POKEMON_PASO_PARAM *)ppp, fast );

	return POKEICON_GetPalNum( mons, form, egg );
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

