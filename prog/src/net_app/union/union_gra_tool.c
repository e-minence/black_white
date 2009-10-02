//==============================================================================
/**
 * @file	gra_tool.c
 * @brief	�O���t�B�b�N�c�[��
 * @author	goto
 * @date	2005.11.16
 *
 * �����ɐF�X�ȉ�����������Ă��悢
 *
 */
//==============================================================================

#include <gflib.h>
#include "net_app/union/union_gra_tool.h"
#include "poke_tool/monsno_def.h"
#include "poke_tool/poke_tool.h"
#include "arc_def.h"

//	GS��system\softsprite.c����ڐA
void			SoftSpriteChrMask(u8* src);
void			SoftSpriteChrMask_DP(u8 *src);
void			SoftSpriteChrMask_ArcID(u8 *src, int arc_id);
// =============================================================================
//
//
//	���摜�ϊ�
//
//
// =============================================================================
// 1Char��2D��1D�ϊ�
static void CharCopy1_1D_from_2D(const u8* src, u8* dst, int* ofs, int* bofs, int cs, int len);

//--------------------------------------------------------------
/**
 * @brief	2D��1D��1�L�����ϊ�
 *
 * @param	src			���f�[�^
 * @param	dst			�W�J��
 * @param	ofs			���f�[�^�̃R�s�[�J�n�ʒu
 * @param	bofs		�W�J��o�b�t�@�̏������݈ʒu
 * @param	cs			�R�s�[�T�C�Y
 * @param	len			2D�f�[�^�̕�
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
static void CharCopy1_1D_from_2D(const u8* src, u8* dst, int* ofs, int* bofs, int cs, int len)
{
	int i;
	u32 dat;
	
	for (i = 0; i < 8; i++){
		GFL_STD_MemCopy(&src[ *ofs ], &dst[ *bofs ],  cs);
		*ofs  += len;
		*bofs += cs;
	}
}

//--------------------------------------------------------------
/**
 * @brief	2D��1D�ɕϊ�����
 *
 * @param	arc_id				�A�[�J�C�u�C���f�b�N�X
 * @param	index_no			�t�@�C���C���f�b�N�X
 * @param	heap_id				�q�[�vID
 * @param	x					�ϊ��Ώۋ�`:x
 * @param	y					�ϊ��Ώۋ�`:y
 * @param	w					�ϊ��Ώۋ�`:w
 * @param	h					�ϊ��Ώۋ�`:h
 * @param	buff				�W�J��o�b�t�@
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
void ChangesInto_1D_from_2D(int arc_id, int index_no, int heap_id, int x, int y, int w, int h, void* buff)
{
	void* aw = NULL;
	u8* p_data = NULL;
	u8* p_buff = NULL;
	NNSG2dCharacterData* ch_data = NULL;

	int size;
	int cs;
	int ofs;
	int bofs;
	int len;
	BOOL res;
	
	///< �f�[�^�̓W�J
	aw = GFL_ARC_LoadDataAlloc(arc_id, index_no, heap_id);
	
	GF_ASSERT(aw != NULL);
	
	///< �L�����f�[�^�̎擾
	res = NNS_G2dGetUnpackedCharacterData(aw, &ch_data);
	
	GF_ASSERT(res != FALSE);

	GF_ASSERT(ch_data->W >= (x + w));		///< �T�C�Y�`�F�b�N
	GF_ASSERT(ch_data->H >= (y + h));
	
	///< ����
	cs     = (sizeof(u8) * 4);				///< �R�s�[�T�C�Y 8dot = 4Byte
	p_data = ch_data->pRawData;				///< �L�����f�[�^
	
	SoftSpriteChrMask_ArcID(p_data, arc_id);				///< ����
	
	p_buff = (u8*)buff;						///< �W�J�o�b�t�@
	
	len = (ch_data->W * cs);				///< ����(���摜�̕�)
	ofs = (x * cs) + (y * len);				///< �J�n�ʒu
	bofs = 0;								///< �o�b�t�@�̏������݈ʒu
	
	//�S�̺�߰
	{
		int i;
		int j;

		for (j = y; j < (y + h); j++){
			for (i = x; i < (x + w); i++){
				ofs = (i * cs) + (j * len * CHAR_DOT_Y);			///< �J�n�ʒu
				CharCopy1_1D_from_2D(p_data, p_buff, &ofs, &bofs, cs, len);	///< 1�L�����R�s�[
			}
		}
	}
	
	GFL_HEAP_FreeMemory(aw);
}

//	GS��gflib\calctool.c����ڐA
//============================================================================================
/**
 *	�����Í��L�[�������[�`��
 *
 * @param[in,out]	code	�Í��L�[�i�[���[�N�̃|�C���^
 *
 * @return	�Í��L�[�i�[���[�N�̏��2�o�C�g���Í��L�[�Ƃ��ĕԂ�
 */
//============================================================================================
static	u16 CodeRand_UniGraTool(u32 *code)
{
    code[0] = code[0] *1103515245L + 24691;
    return (u16)(code[0] / 65536L) ;
}

//	GS��system\softsprite.c����ڐA
//============================================================================================
/**
 *	�|�P�����O���t�B�b�N�ɂ�����ꂽ�}�X�N������
 *
 * @param[in/out]	src		�|�P�����O���t�B�b�N���W�J���ꂽ�̈�̃|�C���^
 */
//============================================================================================
void	SoftSpriteChrMask(u8 *src)
{

	int	i;
	u16	*buf;
	u32	code;

	buf=(u16 *)src;

	code=buf[0];

	//�v���`�i�͑O���}�X�N
	for(i=0;i<(20*10*0x20)/2;i++){
		buf[i]^=code;
		CodeRand_UniGraTool(&code);
	}

//DP�ł́A����}�X�N
#if 0
	code=buf[(20*10*0x20)/2-1];

	for(i=(20*10*0x20)/2-1;i>-1;i--){
		buf[i]^=code;
		CodeRand_UniGraTool(&code);
	}
#endif
}

//============================================================================================
/**
 *	�|�P�����O���t�B�b�N�ɂ�����ꂽ�}�X�N������(DP�O���t�B�b�N�p)
 *
 * @param[in/out]	src		�|�P�����O���t�B�b�N���W�J���ꂽ�̈�̃|�C���^
 */
//============================================================================================
void	SoftSpriteChrMask_DP(u8 *src)
{
	int	i;
	u16	*buf;
	u32	code;

	buf=(u16 *)src;

	code=buf[(20*10*0x20)/2-1];

	for(i=(20*10*0x20)/2-1;i>-1;i--){
		buf[i]^=code;
		CodeRand_UniGraTool(&code);
	}
}

//--------------------------------------------------------------
/**
 * @brief   �|�P�����O���t�B�b�N�ɂ�����ꂽ�}�X�N������
 * 			�A�[�J�C�uID�ɂ���ăv���`�i�p�ADP�p�̕��������ɕ���
 *
 * @param   src			�|�P�����O���t�B�b�N���W�J���ꂽ�̈�̃|�C���^
 * @param   arc_id		�A�[�J�C�uID
 */
//--------------------------------------------------------------
void	SoftSpriteChrMask_ArcID(u8 *src, int arc_id)
{
#if 1	//PLATINUM_MERGE_UNFIX
//	if(arc_id == ARC_DP_POKE_GRA || arc_id == ARC_DP_OTHER_POKE || arc_id == ARC_TRF_GRA){
	if( arc_id == ARCID_TRF_GRA){
		SoftSpriteChrMask_DP(src);
	}
	else{
		SoftSpriteChrMask(src);
	}
#else
	//����ł͂܂�Dp�̕����������Ă�ł���	08.07.07
	SoftSpriteChrMask_DP(src);
#endif
}

