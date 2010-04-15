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

#include "patch.cdat"

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
static  BOOL  MCSS_TOOL_MakeBuchi( const MCSS_ADD_WORK* maw, TCB_LOADRESOURCE_WORK* tlw, u32 work );

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
	int	mons_no = PPP_Get( ppp, ID_PARA_monsno,	NULL );
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
	maw->arcID = TRGRA_GetArcID( dir );
	maw->ncbr = TRGRA_GetCbrArcIndex( tr_type, dir );
	maw->nclr = TRGRA_GetPalArcIndex( tr_type, dir );
	maw->ncer = TRGRA_GetCelArcIndex( tr_type, dir );
	maw->nanr = TRGRA_GetAnmArcIndex( tr_type, dir );
	maw->nmcr = TRGRA_GetMCelArcIndex( tr_type, dir );
	maw->nmar = TRGRA_GetMAnmArcIndex( tr_type, dir );
	maw->ncec = TRGRA_GetNcecArcIndex( tr_type, dir );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P������MCSS_Add���s���i�p�b�`�[���̃u�`������j
 *
 * @param[in] mcss_sys  MCSS�V�X�e���Ǘ��\����
 * @param[in] pp        Add����POKEMON_PARAM�\����
 * @param[in] dir       �|�P�����̌���
 * @param[in] pos_x     Add����X���W
 * @param[in] pos_y     Add����Y���W
 * @param[in] pos_z     Add����Z���W
 */
//-----------------------------------------------------------------------------
MCSS_WORK*  MCSS_TOOL_AddPokeMcss( MCSS_SYS_WORK* mcss_sys, const POKEMON_PARAM* pp, MCSS_DIR dir,
                                   fx32	pos_x, fx32	pos_y, fx32	pos_z )
{ 
  MCSS_WORK* mcss;
  MCSS_ADD_WORK maw;
  u32 personal_rnd = PP_Get( pp, ID_PARA_personal_rnd, NULL );

  MCSS_TOOL_SetMakeBuchiCallback( mcss_sys, personal_rnd );
  MCSS_TOOL_MakeMAWPP( pp, &maw, dir );
  mcss = MCSS_Add( mcss_sys, pos_x, pos_y, pos_z, &maw );

  return mcss;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �L�����N�^�Ƀu�`�����邽�߂̃R�[���o�b�N�Z�b�g
 *
 * @param[in] mcss_sys      MCSS�V�X�e���Ǘ��\����
 * @param[in] personal_rnd  �u�`�̐����ɕK�v�Ȍ�����
 */
//-----------------------------------------------------------------------------
void  MCSS_TOOL_SetMakeBuchiCallback( MCSS_SYS_WORK* mcss_sys, u32 personal_rnd )
{ 
  MCSS_SetCallBackFunc( mcss_sys, MCSS_TOOL_MakeBuchi, personal_rnd );
}

//============================================================================================
/**
 * @brief �L�����f�[�^�ɂԂ�������
 *
 * @param[in] maw  MCSS_ADD_WORK�\���̂ւ̃|�C���^
 * @param[in] tlw  ���\�[�X�f�[�^���[�N�\����
 * @param[in] work �������������Ă���
 *
 * @retval  TRUE:�R�[���o�b�N����������
 */
//============================================================================================
static  BOOL  MCSS_TOOL_MakeBuchi( const MCSS_ADD_WORK* maw, TCB_LOADRESOURCE_WORK* tlw, u32 work )
{ 
  //�p�b�`�[���ɂԂ�������
  if( maw->ncec == NARC_pokegra_wb_pfwb_327_NCEC )
  { 
	  const	PATTIIRU_BUCHI_DATA	*pbd;
	  int i, j;
	  u8	setx, sety, cnt;
	  int	pos[ 2 ];
    u32 rnd = work;
    u8  *buf = tlw->pCharData->pRawData;

	  //1����
	  for( i = 0 ; i < 4 ; i++ )
    {
		  pbd = pbd_table[ i ];
		  cnt=0;
		  while( pbd[ cnt ].posx != 0xff )
      {
			  setx = pbd[ cnt ].posx +   ( ( rnd & 0x0f ) - 8 );
			  sety = pbd[ cnt ].posy + ( ( ( rnd & 0xf0 ) >> 4 ) - 8 );
			  pos[ 0 ] = setx / 2 + sety * 128;
			  pos[ 1 ] = setx / 2 + ( sety + 40 ) * 128;
        for( j = 0 ; j < 2 ; j++ )
        { 
			    if( setx & 1)
          {
				    if( ( ( buf[ pos[ j ] ] & 0xf0 ) >= 0x10 ) && ( ( buf[ pos[ j ] ] & 0xf0 ) <= 0x30) )
            {
					    buf[ pos[ j ] ] += 0x50;
				    }
			    }
			    else
          {
				    if( ( ( buf[ pos[ j ] ] & 0x0f ) >= 0x01 ) && ( ( buf[ pos[ j ] ] & 0x0f ) <= 0x03 ) )
            {
					    buf[ pos[ j ] ] += 0x05;
				    }
			    }
        }
			  cnt++;
		  }
		  rnd = rnd >> 8;
	  }
  }

  return TRUE;
}

