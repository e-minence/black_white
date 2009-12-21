//============================================================================================
/**
 * @file		zknlist_obj.h
 * @brief		�}�Ӄ��X�g��� �n�a�i�֘A
 * @author	Hiroyuki Nakamura
 * @date		09.12.14
 *
 *	���W���[�����FZKNLISTOBJ
 */
//============================================================================================
#pragma	once


//============================================================================================
//	�萔��`
//============================================================================================

// �}�ӗp�n�a�i�A�j����`
enum {
	ZKNLISTOBJ_ANM_START = 0,
	ZKNLISTOBJ_ANM_SELECT,
	ZKNLISTOBJ_ANM_BAR,
	ZKNLISTOBJ_ANM_RAIL,
	ZKNLISTOBJ_ANM_START_ANM,
	ZKNLISTOBJ_ANM_SELECT_ANM,
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[������
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTOBJ_Init( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�폜
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTOBJ_Exit( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�A�j�����C��
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTOBJ_AnmMain( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�A�j���ύX
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 * @param		id		OBJ ID
 * @param		anm		�A�j���ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTOBJ_SetAnm( ZKNLISTMAIN_WORK * wk, u32 id, u32 anm );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�I�[�g�A�j���ݒ�
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 * @param		id		OBJ ID
 * @param		anm		�A�j���ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTOBJ_SetAutoAnm( ZKNLISTMAIN_WORK * wk, u32 id, u32 anm );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�A�j����Ԏ擾
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 * @param		id		OBJ ID
 *
 * @retval	"TRUE = �A�j����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ZKNLISTOBJ_CheckAnm( ZKNLISTMAIN_WORK * wk, u32 id );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�\���؂�ւ�
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 * @param		id		OBJ ID
 * @param		flg		�\���t���O
 *
 * @return	none
 *
 * @li	flg = TRUE : �\��
 * @li	flg = FALSE : ��\��
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTOBJ_SetVanish( ZKNLISTMAIN_WORK * wk, u32 id, BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�\���`�F�b�N
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 * @param		id		OBJ ID
 *
 * @retval	"TRUE = �\��"
 * @retval	"FALSE = ��\��"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ZKNLISTOBJ_CheckVanish( ZKNLISTMAIN_WORK * wk, u32 id );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�������ݒ�
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 * @param		id		OBJ ID
 * @param		flg		ON/OFF�t���O
 *
 * @return	none
 *
 * @li	flg : TRUE = ON, FALSE = OFF
 */
//--------------------------------------------------------------------------------------------
extern void BOX2OBJ_SetBlendMode( ZKNLISTMAIN_WORK * wk, u32 id, BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[���W�ݒ�
 *
 * @param		wk			�}�Ӄ��X�g���[�N
 * @param		id			OBJ ID
 * @param		x				�w���W
 * @param		y				�x���W
 * @param		setsf		�\�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTOBJ_SetPos( ZKNLISTMAIN_WORK * wk, u32 id, s16 x, s16 y, u16 setsf );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[���W�擾
 *
 * @param		wk			�}�Ӄ��X�g���[�N
 * @param		id			OBJ ID
 * @param		x				�w���W
 * @param		y				�x���W
 * @param		setsf		�\�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTOBJ_GetPos( ZKNLISTMAIN_WORK * wk, u32 id, s16 * x, s16 * y, u16 setsf );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�p���b�g�ύX
 *
 * @param		wk			�}�Ӄ��X�g���[�N
 * @param		id			OBJ ID
 * @param		pal			�p���b�g�ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTOBJ_ChgPltt( ZKNLISTMAIN_WORK * wk, u32 id, u32 pal );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�������ʊG�Z���A�N�^�[�Z�b�g
 *
 * @param		wk			�}�Ӄ��X�g���[�N
 * @param		mons		�|�P�����ԍ�
 * @param		form		�t�H����
 * @param		sex			����
 * @param		rare		���A���ǂ���
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTOBJ_SetPokeGra( ZKNLISTMAIN_WORK * wk, u16 mons, u8 form, u8 sex, u8 rare );

extern void ZKNLISTOBJ_PutListPosPokeGra( ZKNLISTMAIN_WORK * wk, s16 pos );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����A�C�R���L������������
 *
 * @param		wk			�}�Ӄ��X�g���[�N
 * @param		idx			OBJ Index
 * @param		form		�t�H����
 * @param		disp		�\�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTOBJ_ChgPokeIcon( ZKNLISTMAIN_WORK * wk, u32 idx, u16 mons, u16 form, BOOL disp );

extern void ZKNLISTOBJ_ScrollPokeIcon( ZKNLISTMAIN_WORK * wk, s16 mv );

extern void ZKNLISTOBJ_PutPokeList( ZKNLISTMAIN_WORK * wk, u32 objIdx, s32 listPos, BOOL disp );

extern void ZKNLISTOBJ_PutScrollList( ZKNLISTMAIN_WORK * wk, u32 idx, u32 mv );

extern void ZKNLISTOBJ_InitScrollList( ZKNLISTMAIN_WORK * wk );

extern void ZKNLISTOBJ_ChgListPosAnm( ZKNLISTMAIN_WORK * wk, u32 pos, BOOL flg );

extern void ZKNLISTOBJ_SetScrollBar( ZKNLISTMAIN_WORK * wk, u32 py );
