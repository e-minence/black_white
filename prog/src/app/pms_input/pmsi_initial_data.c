//============================================================================================
/**
	* @file	pmsi_initial_data.c
	* @bfief	�ȈՉ�b���͉�ʁi�J�e�S���C�j�V�������[�h�p�̃f�[�^�e�[�u�������j
	* @author	taya
	* @date	06.02.14
	*/
//============================================================================================
#include <gflib.h>

#include "system\pms_word.h"
#include "msg\msg_pms_category.h"
#include "message.naix"


#include "pms_input_prv.h"
#include "pmsi_initial_data.h"

//======================================================================
// �C�j�V�������[�h���̑I�𕶎��e�[�u���i���ꂲ�Ƃɐݒ�j
//======================================================================

//-------------------------------------------------------------
/**
	*  �e�����̕`�敝
	*/
//-------------------------------------------------------------
enum {
	INITIAL_WRITE_X_MARGIN = 8*3,//13,		// �P�������Ƃ̕`�敝
	INITIAL_WRITE_Y_MARGIN = 16+8//16,		// �P�������Ƃ̕`�捂��
};

#define INITIAL_XPOS(n)	( (n)*INITIAL_WRITE_X_MARGIN + ( ((n)>=6) ? -16 : 0 ) ) ///< 6�ȏ�͍��Ɋ񂹂邱�ƂŃ}�[�W����\��
#define INITIAL_YPOS(n)	((n)*INITIAL_WRITE_Y_MARGIN)

#if 0
//-------------------------------------------------------------
/**
	*  �e�����̃C���f�b�N�X
	*/
//-------------------------------------------------------------
enum {
	INI_A = 0,
	INI_I,
	INI_U,
	INI_E,
	INI_O,
	INI_KA,
	INI_KI,
	INI_KU,
	INI_KE,
	INI_KO,
	INI_SA,
	INI_SI,
	INI_SU,
	INI_SE,
	INI_SO,
	INI_TA,
	INI_TI,
	INI_TU,
	INI_TE,
	INI_TO,
	INI_NA,
	INI_NI,
	INI_NU,
	INI_NE,
	INI_NO,
	INI_HA,
	INI_HI,
	INI_HU,
	INI_HE,
	INI_HO,
	INI_MA,
	INI_MI,
	INI_MU,
	INI_ME,
	INI_MO,
#if 1
	INI_YA,
	INI_YU,
	INI_YO,
	INI_RA,
	INI_RI,
	INI_RU,
	INI_RE,
	INI_RO,
#else
	INI_RA,
	INI_RI,
	INI_RU,
	INI_RE,
	INI_RO,
	INI_YA,
	INI_YU,
	INI_YO,
#endif
	INI_WA,
	INI_OTHER,

	INI_BACK = CATEGORY_POS_BACK,
	INI_DIS  = CATEGORY_POS_DISABLE,
};
#endif

static const struct {
	STRCODE		code;
	u8			xpos;
	u8			ypos;
	u8			up;
	u8			down;
	u8			left;
	u8			right;
	u8			col_top;
	u8			col_bottom;
}InitialParamTbl[] = {

	{ L'��',  INITIAL_XPOS(0),  INITIAL_YPOS(0), INI_SELECT,  INI_KA,    INI_HO,    INI_I,     INI_A,   INI_NA, },
	{ L'��',  INITIAL_XPOS(1),  INITIAL_YPOS(0), INI_SELECT,  INI_KI,    INI_A,     INI_U,     INI_I,   INI_NI, },
	{ L'��',  INITIAL_XPOS(2),  INITIAL_YPOS(0), INI_SELECT,  INI_KU,    INI_I,     INI_E,     INI_U,   INI_NU, },
	{ L'��',  INITIAL_XPOS(3),  INITIAL_YPOS(0), INI_SELECT,  INI_KE,    INI_U,     INI_O,     INI_E,   INI_NE, },
	{ L'��',  INITIAL_XPOS(4),  INITIAL_YPOS(0), INI_SELECT,  INI_KO,    INI_E,     INI_HA,    INI_O,   INI_NO, },

	{ L'��',  INITIAL_XPOS(0),  INITIAL_YPOS(1), INI_A,     INI_SA,    INI_MO,    INI_KI,    INI_A,   INI_NA, },
	{ L'��',  INITIAL_XPOS(1),  INITIAL_YPOS(1), INI_I,     INI_SI,    INI_KA,    INI_KU,    INI_I,   INI_NI, },
	{ L'��',  INITIAL_XPOS(2),  INITIAL_YPOS(1), INI_U,     INI_SU,    INI_KI,    INI_KE,    INI_U,   INI_NU, },
	{ L'��',  INITIAL_XPOS(3),  INITIAL_YPOS(1), INI_E,     INI_SE,    INI_KU,    INI_KO,    INI_E,   INI_NE, },
	{ L'��',  INITIAL_XPOS(4),  INITIAL_YPOS(1), INI_O,     INI_SO,    INI_KE,    INI_MA,    INI_O,   INI_NO, },

	{ L'��',  INITIAL_XPOS(0),  INITIAL_YPOS(2), INI_KA,    INI_TA,    INI_YO,    INI_SI,    INI_A,   INI_NA, },
	{ L'��',  INITIAL_XPOS(1),  INITIAL_YPOS(2), INI_KI,    INI_TI,    INI_SA,    INI_SU,    INI_I,   INI_NI, },
	{ L'��',  INITIAL_XPOS(2),  INITIAL_YPOS(2), INI_KU,    INI_TU,    INI_SI,    INI_SE,    INI_U,   INI_NU, },
	{ L'��',  INITIAL_XPOS(3),  INITIAL_YPOS(2), INI_KE,    INI_TE,    INI_SU,    INI_SO,    INI_E,   INI_NE, },
	{ L'��',  INITIAL_XPOS(4),  INITIAL_YPOS(2), INI_KO,    INI_TO,    INI_SE,    INI_YA,    INI_O,   INI_NO, },

	{ L'��',  INITIAL_XPOS(0),  INITIAL_YPOS(3), INI_SA,    INI_NA,    INI_RO,    INI_TI,    INI_A,   INI_NA, },
	{ L'��',  INITIAL_XPOS(1),  INITIAL_YPOS(3), INI_SI,    INI_NI,    INI_TA,    INI_TU,    INI_I,   INI_NI, },
	{ L'��',  INITIAL_XPOS(2),  INITIAL_YPOS(3), INI_SU,    INI_NU,    INI_TI,    INI_TE,    INI_U,   INI_NU, },
	{ L'��',  INITIAL_XPOS(3),  INITIAL_YPOS(3), INI_SE,    INI_NE,    INI_TU,    INI_TO,    INI_E,   INI_NE, },
	{ L'��',  INITIAL_XPOS(4),  INITIAL_YPOS(3), INI_SO,    INI_NO,    INI_TE,    INI_RA,    INI_O,   INI_NO, },

	{ L'��',  INITIAL_XPOS(0),  INITIAL_YPOS(4), INI_TA,    INI_SELECT,  INI_OTHER, INI_NI,    INI_A,  INI_NA, },
	{ L'��',  INITIAL_XPOS(1),  INITIAL_YPOS(4), INI_TI,    INI_SELECT,  INI_NA,    INI_NU,    INI_I,  INI_NI, },
	{ L'��',  INITIAL_XPOS(2),  INITIAL_YPOS(4), INI_TU,    INI_SELECT,  INI_NI,    INI_NE,    INI_U,  INI_NU, },
	{ L'��',  INITIAL_XPOS(3),  INITIAL_YPOS(4), INI_TE,    INI_SELECT,  INI_NU,    INI_NO,    INI_E,  INI_NE, },
	{ L'��',  INITIAL_XPOS(4),  INITIAL_YPOS(4), INI_TO,    INI_SELECT,  INI_NE,    INI_WA,    INI_O,  INI_NO, },

  // X���s

	{ L'��',  INITIAL_XPOS(6),  INITIAL_YPOS(0), INI_SELECT,  INI_MA,    INI_O,     INI_HI,   INI_HA,  INI_WA, },
	{ L'��',  INITIAL_XPOS(7),  INITIAL_YPOS(0), INI_SELECT,  INI_MI,    INI_HA,    INI_HU,   INI_HI,  INI_WO, },
	{ L'��',  INITIAL_XPOS(8),  INITIAL_YPOS(0), INI_SELECT,  INI_MU,    INI_HI,    INI_HE,   INI_HU,  INI_NN, },
	{ L'��',  INITIAL_XPOS(9),  INITIAL_YPOS(0), INI_SELECT,  INI_ME,    INI_HU,    INI_HO,   INI_HE,  INI_BOU, },
	{ L'��',  INITIAL_XPOS(10), INITIAL_YPOS(0), INI_SELECT,  INI_MO,    INI_HE,    INI_A,    INI_HO,  INI_OTHER, },

/*
	{ L'��',  INITIAL_XPOS(6),  INITIAL_YPOS(1), INI_HA,    INI_YA,    INI_KO,    INI_MI,   INI_HA,  INI_WA, },
	{ L'��',  INITIAL_XPOS(7),  INITIAL_YPOS(1), INI_HI,    INI_YU,    INI_MA,    INI_MU,   INI_HI,  INI_WO, },
	{ L'��',  INITIAL_XPOS(8),  INITIAL_YPOS(1), INI_HU,    INI_YO,    INI_MI,    INI_ME,   INI_HU,  INI_NN, },
	{ L'��',  INITIAL_XPOS(9),  INITIAL_YPOS(1), INI_HE,    INI_RE,    INI_MU,    INI_MO,   INI_HE,  INI_BOU, },
	{ L'��',  INITIAL_XPOS(10), INITIAL_YPOS(1), INI_HO,    INI_RO,    INI_ME,    INI_KA,   INI_HO,  INI_OTHER, },
*/

	{ L'��',  INITIAL_XPOS(6),  INITIAL_YPOS(1), INI_HA,    INI_YA,    INI_KO,    INI_MI,   INI_HA,  INI_WA, },
	{ L'��',  INITIAL_XPOS(7),  INITIAL_YPOS(1), INI_HI,    INI_RI,    INI_MA,    INI_MU,   INI_HI,  INI_WO, },
	{ L'��',  INITIAL_XPOS(8),  INITIAL_YPOS(1), INI_HU,    INI_YU,    INI_MI,    INI_ME,   INI_HU,  INI_NN, },
	{ L'��',  INITIAL_XPOS(9),  INITIAL_YPOS(1), INI_HE,    INI_RE,    INI_MU,    INI_MO,   INI_HE,  INI_BOU, },
	{ L'��',  INITIAL_XPOS(10), INITIAL_YPOS(1), INI_HO,    INI_YO,    INI_ME,    INI_KA,   INI_HO,  INI_OTHER, },

/*
	{ L'��',  INITIAL_XPOS(6), INITIAL_YPOS(2), INI_MA,     INI_RA,    INI_SO,    INI_YU,   INI_HA,  INI_WA, },
	{ L'��',  INITIAL_XPOS(7), INITIAL_YPOS(2), INI_MI,     INI_RI,    INI_YA,    INI_YO,   INI_HI,  INI_WO, },
	{ L'��',  INITIAL_XPOS(8), INITIAL_YPOS(2), INI_MU,     INI_RU,    INI_YU,    INI_SA,   INI_HU,  INI_NN, },
*/

	{ L'��',  INITIAL_XPOS(6),  INITIAL_YPOS(2), INI_MA,    INI_RA,    INI_SO,    INI_YU,   INI_HA,  INI_WA, },
	{ L'��',  INITIAL_XPOS(8),  INITIAL_YPOS(2), INI_MU,    INI_RU,    INI_YA,    INI_YO,   INI_HU,  INI_NN, },
	{ L'��',  INITIAL_XPOS(10), INITIAL_YPOS(2), INI_MO,    INI_RO,    INI_YU,    INI_SA,   INI_HO,  INI_OTHER, },

/*
	{ L'��',  INITIAL_XPOS(6),  INITIAL_YPOS(3), INI_YA,    INI_WA,    INI_TO,    INI_RI,    INI_HA,  INI_WA, },
	{ L'��',  INITIAL_XPOS(7),  INITIAL_YPOS(3), INI_YU,    INI_WO,     INI_RA,    INI_RU,    INI_HI,  INI_WO, },
	{ L'��',  INITIAL_XPOS(8),  INITIAL_YPOS(3), INI_YO,    INI_NN,     INI_RI,    INI_RE,    INI_HU,  INI_NN, },
	{ L'��',  INITIAL_XPOS(9),  INITIAL_YPOS(3), INI_ME,    INI_BOU,    INI_RU,    INI_RO,    INI_HE,  INI_BOU, },
	{ L'��',  INITIAL_XPOS(10), INITIAL_YPOS(3), INI_MO,    INI_OTHER,  INI_RE,    INI_TA,    INI_HO,  INI_OTHER, },
*/

	{ L'��',  INITIAL_XPOS(6),  INITIAL_YPOS(3), INI_YA,    INI_WA,    INI_TO,    INI_RI,    INI_HA,  INI_WA, },
	{ L'��',  INITIAL_XPOS(7),  INITIAL_YPOS(3), INI_MI,    INI_WO,     INI_RA,    INI_RU,    INI_HI,  INI_WO, },
	{ L'��',  INITIAL_XPOS(8),  INITIAL_YPOS(3), INI_YU,    INI_NN,     INI_RI,    INI_RE,    INI_HU,  INI_NN, },
	{ L'��',  INITIAL_XPOS(9),  INITIAL_YPOS(3), INI_ME,    INI_BOU,    INI_RU,    INI_RO,    INI_HE,  INI_BOU, },
	{ L'��',  INITIAL_XPOS(10), INITIAL_YPOS(3), INI_YO,    INI_OTHER,  INI_RE,    INI_TA,    INI_HO,  INI_OTHER, },

  { L'��',  INITIAL_XPOS(6), INITIAL_YPOS(4), INI_RA,     INI_SELECT,  INI_NO,    INI_WO,    INI_HA,  INI_WA, },
	{ L'�I',  INITIAL_XPOS(10), INITIAL_YPOS(4), INI_RO,    INI_SELECT,  INI_BOU,   INI_NA,    INI_HO,  INI_OTHER },
  
  // ��50��������gmm�e�[�u���ƑΉ����Ă���̂ŏ��ԌŒ聪
  
  // WB�ǉ� 
	{ L'��',  INITIAL_XPOS(7), INITIAL_YPOS(4), INI_RI,     INI_SELECT,  INI_WA,    INI_NN,     INI_HI,  INI_WO },
	{ L'��',  INITIAL_XPOS(8), INITIAL_YPOS(4), INI_RU,     INI_SELECT,  INI_WO,    INI_BOU,    INI_HU,  INI_NN },
	{ L'�[',  INITIAL_XPOS(9), INITIAL_YPOS(4), INI_RE,     INI_SELECT,  INI_NN,  INI_OTHER,  INI_HE,  INI_BOU },
};



//------------------------------------------------------------------
/**
	* �C�j�V�����i���o�[�ő��Ԃ�
	*
	* @retval  u32		
	*/
//------------------------------------------------------------------
u32 PMSI_INITIAL_DAT_GetInitialMax( void )
{
	return NELEMS(InitialParamTbl);
}

//------------------------------------------------------------------
/**
	* �P�����̕����R�[�h���o�b�t�@�ɃR�s�[
	*
	* @param   initial		�C�j�V�����i���o�[
	* @param   buf			�R�s�[��o�b�t�@
	*
	*/
//------------------------------------------------------------------
void PMSI_INITIAL_DAT_GetStr( u32 initial, STRBUF* buf )
{
	GF_ASSERT(initial < NELEMS(InitialParamTbl));

	GFL_STR_SetStringCodeOrderLength( buf, &(InitialParamTbl[initial].code), 2);
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �w��C�j�V�����i���o�[��STRCODE��Ԃ�
 *
 *	@param	u32 initial �C�j�V�����i���o�[
 *
 *	@retval STRCODE 
 */
//-----------------------------------------------------------------------------
STRCODE PMSI_INITIAL_DAT_GetStrCode( u32 initial )
{
	GF_ASSERT(initial < NELEMS(InitialParamTbl));

  return InitialParamTbl[initial].code;
}

//------------------------------------------------------------------
/**
	* �`��p��ʒu�i�h�b�g�P�ʁj���擾
	*
	* @param   initial		�C�j�V�����i���o�[
	* @param   x			�`��ʒu�w
	* @param   y			�`��ʒu�x
	*
	*/
//------------------------------------------------------------------
void PMSI_INITIAL_DAT_GetPrintPos( u32 initial, u32* x, u32* y )
{
	GF_ASSERT_MSG((initial < NELEMS(InitialParamTbl)), "pos=%d", initial);

	*x = InitialParamTbl[initial].xpos;
	*y = InitialParamTbl[initial].ypos;
}

//------------------------------------------------------------------
/**
	* ��L�[�������̈ړ���C�j�V�����i���o�[��Ԃ�
	*
	* @param   initial		�C�j�V�����i���o�[
	*
	* @retval  int		�ړ���C�j�V�����i���o�[
	*/
//------------------------------------------------------------------
int PMSI_INITIAL_DAT_GetUpCode( u32 initial )
{
	GF_ASSERT(initial < NELEMS(InitialParamTbl));
	return InitialParamTbl[initial].up;
}

//------------------------------------------------------------------
/**
	* ���L�[�������̈ړ���C�j�V�����i���o�[��Ԃ�
	*
	* @param   initial		�C�j�V�����i���o�[
	*
	* @retval  int		�ړ���C�j�V�����i���o�[
	*/
//------------------------------------------------------------------
int PMSI_INITIAL_DAT_GetDownCode( u32 initial )
{
	GF_ASSERT(initial < NELEMS(InitialParamTbl));
	return InitialParamTbl[initial].down;
}

//------------------------------------------------------------------
/**
	* ���L�[�������̈ړ���C�j�V�����i���o�[��Ԃ�
	*
	* @param   initial		�C�j�V�����i���o�[
	*
	* @retval  int		�ړ���C�j�V�����i���o�[
	*/
//------------------------------------------------------------------
int PMSI_INITIAL_DAT_GetLeftCode( u32 initial )
{
	GF_ASSERT(initial < NELEMS(InitialParamTbl));
	return InitialParamTbl[initial].left;
}

//------------------------------------------------------------------
/**
	* �E�L�[�������̈ړ���C�j�V�����i���o�[��Ԃ�
	*
	* @param   initial		�C�j�V�����i���o�[
	*
	* @retval  int		�ړ���C�j�V�����i���o�[
	*/
//------------------------------------------------------------------
int PMSI_INITIAL_DAT_GetRightCode( u32 initial )
{
	GF_ASSERT(initial < NELEMS(InitialParamTbl));
	return InitialParamTbl[initial].right;
}

//------------------------------------------------------------------
/**
	* �w��L�[�̂����̍ŏ�i�i���o�[��Ԃ�
	*
	* @param   initial		�C�j�V�����i���o�[
	*
	* @retval  int		�ړ���C�j�V�����i���o�[
	*/
//------------------------------------------------------------------
int PMSI_INITIAL_DAT_GetColTopCode( u32 initial )
{
	GF_ASSERT(initial < NELEMS(InitialParamTbl));
	return InitialParamTbl[initial].col_top;
}

//------------------------------------------------------------------
/**
	* �w��L�[�̂����̍ŉ��i�i���o�[��Ԃ�
	*
	* @param   initial		�C�j�V�����i���o�[
	*
	* @retval  int		�ړ���C�j�V�����i���o�[
	*/
//------------------------------------------------------------------
int PMSI_INITIAL_DAT_GetColBottomCode( u32 initial )
{
	GF_ASSERT(initial < NELEMS(InitialParamTbl));
	return InitialParamTbl[initial].col_bottom;
}


