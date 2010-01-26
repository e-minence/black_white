//============================================================================================
/**
 * @file	mcss_tool.c
 * @brief	MCSS�c�[��
 * @author	soga
 * @date	2009.02.10
 */
//============================================================================================

#include <gflib.h>

#include "system/mcss_tool.h"

#include "arc_def.h"

//============================================================================================
/**
 *	�萔�錾
 */
//============================================================================================
//�g���[�i�[��̂��\������MCSS�p�t�@�C���̍\��
enum{
	TRGRA_NCGR,
	TRGRA_NCBR,
	TRGRA_NCER,
	TRGRA_NANR,
	TRGRA_NMCR,
	TRGRA_NMAR,
	TRGRA_NCEC,
	TRGRA_NCLR,

	TRGRA_FILE_MAX,			//�g���[�i�[��̂��\������MCSS�p�t�@�C���̑���
};

//============================================================================================
/**
 *	�v���g�^�C�v�錾
 */
//============================================================================================

//============================================================================================
/**
 *	POKEMON_PARAM����MCSS_ADD_WORK�𐶐�����
 *
 * @param[in]	pp		POKEMON_PARAM�\���̂ւ̃|�C���^
 * @param[out]	maw		MCSS_ADD_WORK���[�N�ւ̃|�C���^
 * @param[in]	dir		�|�P�����̌����iMCSS_DIR_FRONT:���ʁAMCSS_DIR_BACK:�w�ʁj
 */
//============================================================================================
void	MCSS_TOOL_MakeMAWPP( const POKEMON_PARAM *pp, MCSS_ADD_WORK *maw, int dir )
{
	MCSS_TOOL_MakeMAWPPP( PP_GetPPPPointerConst( pp ), maw, dir );
}

//============================================================================================
/**
 *	POKEMON_PASO_PARAM����MCSS_ADD_WORK�𐶐�����
 *
 * @param[in]	ppp		POKEMON_PASO_PARAM�\���̂ւ̃|�C���^
 * @param[out]	maw		MCSS_ADD_WORK���[�N�ւ̃|�C���^
 * @param[in]	dir		�|�P�����̌����iMCSS_DIR_FRONT:���ʁAMCSS_DIR_BACK:�w�ʁj
 */
//============================================================================================
void	MCSS_TOOL_MakeMAWPPP( const POKEMON_PASO_PARAM *ppp, MCSS_ADD_WORK *maw, int dir )
{
	int	mons_no = PPP_Get( ppp, ID_PARA_monsno_egg,	NULL );
	int	form_no = PPP_Get( ppp, ID_PARA_form_no,NULL );
	int	sex		= PPP_Get( ppp, ID_PARA_sex,	NULL );
	int	rare	= PPP_CheckRare( ppp );
	int	egg	  = PPP_Get( ppp, ID_PARA_tamago_flag,	NULL );

	MCSS_TOOL_MakeMAWParam( mons_no, form_no, sex, rare, egg, maw, dir );
}

//============================================================================================
/**
 *	�p�����[�^����MCSS_ADD_WORK�𐶐�����
 *
 * @param[in]	mons_no �����X�^�[�i���o�[
 * @param[in]	form_no �t�H�����i���o�[
 * @param[in]	sex		����
 * @param[in]	rare	���A���ǂ���
 * @param[out]	maw		MCSS_ADD_WORK���[�N�ւ̃|�C���^
 * @param[in]	dir		�|�P�����̌����iMCSS_DIR_FRONT:���ʁAMCSS_DIR_BACK:�w�ʁj
 */
//============================================================================================
void	MCSS_TOOL_MakeMAWParam( int	mons_no, int form_no, int sex, int rare, BOOL egg, MCSS_ADD_WORK *maw, int dir )
{
	maw->arcID = POKEGRA_GetArcID();
	maw->ncbr = POKEGRA_GetCbrArcIndex( mons_no, form_no, sex, rare,  dir, egg );
	maw->nclr = POKEGRA_GetPalArcIndex( mons_no, form_no, sex, rare,  dir, egg );
	maw->ncer = POKEGRA_GetCelArcIndex( mons_no, form_no, sex, rare,  dir, egg );
	maw->nanr = POKEGRA_GetAnmArcIndex( mons_no, form_no, sex, rare,  dir, egg );
	maw->nmcr = POKEGRA_GetMCelArcIndex( mons_no, form_no, sex, rare,  dir, egg );
	maw->nmar = POKEGRA_GetMAnmArcIndex( mons_no, form_no, sex, rare,  dir, egg );
	maw->ncec = POKEGRA_GetNcecArcIndex( mons_no, form_no, sex, rare,  dir, egg );
}

//============================================================================================
/**
 *	�p�����[�^����MCSS_ADD_WORK�𐶐�����
 *
 * @param[in]   tr_type �g���[�i�[�^�C�v
 * @param[out]  maw     MCSS_ADD_WORK���[�N�ւ̃|�C���^
 * @param[in]   dir     �g���[�i�[�̌����iMCSS_DIR_FRONT:���ʁAMCSS_DIR_BACK:�w�ʁj
 */
//============================================================================================
void	MCSS_TOOL_MakeMAWTrainer( int	tr_type, MCSS_ADD_WORK *maw, int dir )
{
	int	file_start = TRGRA_FILE_MAX * tr_type;	//�g���[�i�[�^�C�v����t�@�C���̃I�t�Z�b�g���v�Z
	int	file_offset = 0;	//�����̌v�Z�i����͂Ȃ��j

	maw->arcID = ARCID_TRFGRA;
	maw->ncbr = file_start + file_offset + TRGRA_NCBR;
	maw->nclr = file_start + file_offset + TRGRA_NCLR;
	maw->ncer = file_start + file_offset + TRGRA_NCER;
	maw->nanr = file_start + file_offset + TRGRA_NANR;
	maw->nmcr = file_start + file_offset + TRGRA_NMCR;
	maw->nmar = file_start + file_offset + TRGRA_NMAR;
	maw->ncec = file_start + file_offset + TRGRA_NCEC;
}

