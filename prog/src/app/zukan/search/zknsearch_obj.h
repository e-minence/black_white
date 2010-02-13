//============================================================================================
/**
 * @file		zknsearch_obj.h
 * @brief		�}�ӌ������ �n�a�i�֘A
 * @author	Hiroyuki Nakamura
 * @date		10.02.09
 *
 *	���W���[�����FZKNSEARCHOBJ
 */
//============================================================================================
#pragma	once


//============================================================================================
//	�萔��`
//============================================================================================

// �}�ӗp�n�a�i�A�j����`
enum {
	ZKNSEARCHOBJ_ANM_BAR = 0,
	ZKNSEARCHOBJ_ANM_RAIL,
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[������
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHOBJ_Init( ZKNSEARCHMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�폜
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHOBJ_Exit( ZKNSEARCHMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�A�j�����C��
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHOBJ_AnmMain( ZKNSEARCHMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�A�j���ύX
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		id		OBJ ID
 * @param		anm		�A�j���ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHOBJ_SetAnm( ZKNSEARCHMAIN_WORK * wk, u32 id, u32 anm );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�I�[�g�A�j���ݒ�
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		id		OBJ ID
 * @param		anm		�A�j���ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHOBJ_SetAutoAnm( ZKNSEARCHMAIN_WORK * wk, u32 id, u32 anm );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�A�j���擾
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		id		OBJ ID
 *
 * @return	�A�j���ԍ�
 */
//--------------------------------------------------------------------------------------------
extern u32 ZKNSEARCHOBJ_GetAnm( ZKNSEARCHMAIN_WORK * wk, u32 id );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�A�j����Ԏ擾
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		id		OBJ ID
 *
 * @retval	"TRUE = �A�j����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ZKNSEARCHOBJ_CheckAnm( ZKNSEARCHMAIN_WORK * wk, u32 id );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�\���؂�ւ�
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		id		OBJ ID
 * @param		flg		�\���t���O
 *
 * @return	none
 *
 * @li	flg = TRUE : �\��
 * @li	flg = FALSE : ��\��
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHOBJ_SetVanish( ZKNSEARCHMAIN_WORK * wk, u32 id, BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�\���`�F�b�N
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		id		OBJ ID
 *
 * @retval	"TRUE = �\��"
 * @retval	"FALSE = ��\��"
 */
//--------------------------------------------------------------------------------------------
extern BOOL ZKNSEARCHOBJ_CheckVanish( ZKNSEARCHMAIN_WORK * wk, u32 id );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�������ݒ�
 *
 * @param		wk		�}�ӌ�����ʃ��[�N
 * @param		id		OBJ ID
 * @param		flg		ON/OFF�t���O
 *
 * @return	none
 *
 * @li	flg : TRUE = ON, FALSE = OFF
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHOBJ_SetBlendMode( ZKNSEARCHMAIN_WORK * wk, u32 id, BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[���W�ݒ�
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		id			OBJ ID
 * @param		x				�w���W
 * @param		y				�x���W
 * @param		setsf		�\�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHOBJ_SetPos( ZKNSEARCHMAIN_WORK * wk, u32 id, s16 x, s16 y, u16 setsf );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[���W�擾
 *
 * @param		wk			�}�ӌ�����ʃ��[�N
 * @param		id			OBJ ID
 * @param		x				�w���W
 * @param		y				�x���W
 * @param		setsf		�\�����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNSEARCHOBJ_GetPos( ZKNSEARCHMAIN_WORK * wk, u32 id, s16 * x, s16 * y, u16 setsf );

//extern void ZKNSEARCHOBJ_VanishAll( ZKNSEARCHMAIN_WORK * wk );

extern void ZKNSEARCHOBJ_VanishList( ZKNSEARCHMAIN_WORK * wk );



extern void ZKNSEARCHOBJ_PutMainPage( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHOBJ_PutRowPage( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHOBJ_PutNamePage( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHOBJ_PutTypePage( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHOBJ_PutColorPage( ZKNSEARCHMAIN_WORK * wk );
extern void ZKNSEARCHOBJ_PutFormPage( ZKNSEARCHMAIN_WORK * wk );

extern void ZKNSEARCHOBJ_PutFormListNow( ZKNSEARCHMAIN_WORK * wk );


extern void ZKNSEARCHOBJ_SetScrollBar( ZKNSEARCHMAIN_WORK * wk, u32 py );

extern void ZKNSEARCHOBJ_SetListPageArrowAnime( ZKNSEARCHMAIN_WORK * wk, BOOL anm );

extern void ZKNSEARCHOBJ_PutMark( ZKNSEARCHMAIN_WORK * wk, u16 num, s16 py, BOOL disp );
extern void ZKNSEARCHOBJ_ChangeMark( ZKNSEARCHMAIN_WORK * wk, u16 pos, BOOL flg );
extern void ZKNSEARCHOBJ_ChangeTypeMark( ZKNSEARCHMAIN_WORK * wk, u8	pos1, u8 pos2 );
extern void ZKNSEARCHOBJ_PutFormMark( ZKNSEARCHMAIN_WORK * wk, s16 py, BOOL disp );
extern void ZKNSEARCHOBJ_ChangeFormMark( ZKNSEARCHMAIN_WORK * wk, u16 pos, BOOL flg );
extern void ZKNSEARCHOBJ_PutFormList( ZKNSEARCHMAIN_WORK * wk, u16 num, s16 py, BOOL disp );

extern void ZKNSEARCHOBJ_ScrollList( ZKNSEARCHMAIN_WORK * wk, s8 mv );
extern void ZKNSEARCHOBJ_ScrollFormList( ZKNSEARCHMAIN_WORK * wk, s8 mv );

extern void ZKNSEARCHOBJ_MoveLoadingBar( ZKNSEARCHMAIN_WORK * wk, u32 cnt );
