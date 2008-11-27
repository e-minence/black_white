
//============================================================================================
/**
 * @file	poke_mcss_def.h
 * @brief	�|�P����MCSS�Ǘ�
 * @author	soga
 * @date	2008.11.14
 */
//============================================================================================

#ifndef	__POKE_MCSS_DEF_H_
#define	__POKE_MCSS_DEF_H_

#define	POKE_MCSS_MAX	( 6 )	//POKE_MCSS�ŊǗ�����MCSS��MAX�l

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

#endif	//__POKE_MCSS_DEF_H_
