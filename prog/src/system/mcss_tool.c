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
#include "pokegra/pokegra_wb.naix"

//============================================================================================
/**
 *	�萔�錾
 */
//============================================================================================
//�|�P������̂��\������MCSS�p�t�@�C���̍\��
enum{
	POKEGRA_FRONT_M_NCGR = 0,
	POKEGRA_FRONT_F_NCGR,
	POKEGRA_FRONT_M_NCBR,
	POKEGRA_FRONT_F_NCBR,
	POKEGRA_FRONT_NCER,
	POKEGRA_FRONT_NANR,
	POKEGRA_FRONT_NMCR,
	POKEGRA_FRONT_NMAR,
	POKEGRA_FRONT_NCEC,
	POKEGRA_BACK_M_NCGR,
	POKEGRA_BACK_F_NCGR,
	POKEGRA_BACK_M_NCBR,
	POKEGRA_BACK_F_NCBR,
	POKEGRA_BACK_NCER,
	POKEGRA_BACK_NANR,
	POKEGRA_BACK_NMCR,
	POKEGRA_BACK_NMAR,
	POKEGRA_BACK_NCEC,
	POKEGRA_NORMAL_NCLR,
	POKEGRA_RARE_NCLR,

	POKEGRA_FILE_MAX,			//�|�P������̂��\������MCSS�p�t�@�C���̑���

	POKEGRA_M_NCGR = 0,
	POKEGRA_F_NCGR,
	POKEGRA_M_NCBR,
	POKEGRA_F_NCBR,
	POKEGRA_NCER,
	POKEGRA_NANR,
	POKEGRA_NMCR,
	POKEGRA_NMAR,
	POKEGRA_NCEC
};

//============================================================================================
/**
 *	�v���g�^�C�v�錾
 */
//============================================================================================

void	MCSS_TOOL_MakeMAWPP( const POKEMON_PARAM *pp, MCSS_ADD_WORK *maw, int dir );
void	MCSS_TOOL_MakeMAWPPP( const POKEMON_PASO_PARAM *ppp, MCSS_ADD_WORK *maw, int dir );
void	MCSS_TOOL_MakeMAWParam( int mons_no, int form_no, int sex, int rare, MCSS_ADD_WORK *maw, int dir );

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
	int	mons_no = PPP_Get( ppp, ID_PARA_monsno,	NULL ) - 1;
	int	form_no = PPP_Get( ppp, ID_PARA_form_no,NULL );
	int	sex		= PPP_Get( ppp, ID_PARA_sex,	NULL );
	int	rare	= PPP_CheckRare( ppp );

	MCSS_TOOL_MakeMAWParam( mons_no, form_no, sex, rare, maw, dir );
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
void	MCSS_TOOL_MakeMAWParam( int	mons_no, int form_no, int sex, int rare, MCSS_ADD_WORK *maw, int dir )
{
	int	file_start = POKEGRA_FILE_MAX * mons_no;							//�|�P�����i���o�[����t�@�C���̃I�t�Z�b�g���v�Z
	int	file_offset = ( dir == MCSS_DIR_FRONT ) ? 0 : POKEGRA_BACK_M_NCGR;	//�����̌v�Z

	//�{���͕ʃt�H��������������
#ifdef DEBUG_ONLY_FOR_sogabe
#warning Another Form Nothing
#endif

	//���ʂ̃`�F�b�N
	switch( sex ){
	case PTL_SEX_MALE:
		break;
	case PTL_SEX_FEMALE:
		//�I�X���X�����������Ă��邩�`�F�b�N����i�T�C�Y���O�Ȃ珑�������Ȃ��j
		sex = ( GFL_ARC_GetDataSize( ARCID_POKEGRA, file_start + file_offset + 1 ) == 0 ) ? PTL_SEX_MALE : PTL_SEX_FEMALE;
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

	maw->arcID = ARCID_POKEGRA;
	maw->ncbr = file_start + file_offset + POKEGRA_M_NCBR + sex;
	maw->nclr = file_start + POKEGRA_NORMAL_NCLR + rare;
	maw->ncer = file_start + file_offset + POKEGRA_NCER;
	maw->nanr = file_start + file_offset + POKEGRA_NANR;
	maw->nmcr = file_start + file_offset + POKEGRA_NMCR;
	maw->nmar = file_start + file_offset + POKEGRA_NMAR;
	maw->ncec = file_start + file_offset + POKEGRA_NCEC;
}

