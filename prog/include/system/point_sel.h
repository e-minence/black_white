//============================================================================================
/**
 * @file	point_sel.h
 * @brief	�ʒu�C���f�b�N�X�I������
 * @author	Hiroyuki Nakamura
 * @date	2004.11.12
 *
 *	���W���[�����FPOINTSEL
 */
//============================================================================================
#pragma	once


//============================================================================================
//	�萔��`
//============================================================================================
// �p�����[�^�擾�p��`
enum {
/*
	POINT_WK_PX = 0,	// X���W
	POINT_WK_PY,		// Y���W
	POINT_WK_SX,		// X�T�C�Y
	POINT_WK_SY,		// Y�T�C�Y
	POINT_WK_UP,		// ������ւ̃C���f�b�N�X�ԍ�
	POINT_WK_DOWN,		// �������ւ̃C���f�b�N�X�ԍ�
	POINT_WK_LEFT,		// �������ւ̃C���f�b�N�X�ԍ�
	POINT_WK_RIGHT		// �E�����ւ̃C���f�b�N�X�ԍ�
*/
	POINTSEL_PRM_PX = 0,	// X���W
	POINTSEL_PRM_PY,		// Y���W
	POINTSEL_PRM_SX,		// X�T�C�Y
	POINTSEL_PRM_SY,		// Y�T�C�Y
	POINTSEL_PRM_UP,		// ������ւ̃C���f�b�N�X�ԍ�
	POINTSEL_PRM_DOWN,		// �������ւ̃C���f�b�N�X�ԍ�
	POINTSEL_PRM_LEFT,		// �������ւ̃C���f�b�N�X�ԍ�
	POINTSEL_PRM_RIGHT		// �E�����ւ̃C���f�b�N�X�ԍ�
};

// �f�[�^�ړ���`
enum {
/*
	POINT_MV_UP = 0,	// ��
	POINT_MV_DOWN,		// ��
	POINT_MV_LEFT,		// �E
	POINT_MV_RIGHT,		// ��
	POINT_MV_NULL		// �_�~�[
*/
	POINTSEL_MV_UP = 0,	// ��
	POINTSEL_MV_DOWN,		// ��
	POINTSEL_MV_LEFT,		// �E
	POINTSEL_MV_RIGHT,		// ��
	POINTSEL_MV_NULL		// �_�~�[
};

//#define	POINT_SEL_NOMOVE	( 0xff )	// �\���L�[�ړ��ŏ\���L�[��������ĂȂ��ꍇ�̕Ԃ�l
#define	POINTSEL_MOVE_NONE	( 0xff )	// �\���L�[�ړ��ŏ\���L�[��������ĂȂ��ꍇ�̕Ԃ�l

typedef struct {
	u8	px;			// X���W�i�L�����ł��h�b�g�ł��j
	u8	py;			// Y���W
	u8	sx;			// X�T�C�Y
	u8	sy;			// Y�T�C�Y

	u8	up;			// ������ւ̃C���f�b�N�X�ԍ�
	u8	down;		// �������ւ̃C���f�b�N�X�ԍ�
	u8	left;		// �������ւ̃C���f�b�N�X�ԍ�
	u8	right;		// �E�����ւ̃C���f�b�N�X�ԍ�
//}POINTER_WORK;
}POINTSEL_WORK;


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �p�����[�^�擾
 *
 * @param	pw		���[�N
 * @param	prm		�擾�t���O
 *
 * @return	�w��p�����[�^
 */
//--------------------------------------------------------------------------------------------
//extern u8 PointerWorkGet( const POINTSEL_WORK * pw, u8 prm );
extern u8 POINTSEL_GetParam( const POINTSEL_WORK * pw, u8 prm );

//--------------------------------------------------------------------------------------------
/**
 * ���W�擾
 *
 * @param	pw		���[�N
 * @param	x		X���W�i�[�ꏊ
 * @param	y		Y���W�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void PointerWkPosGet( const POINTSEL_WORK * pw, u8 * x, u8 * y );
extern void POINTSEL_GetPos( const POINTSEL_WORK * pw, u8 * x, u8 * y );

//--------------------------------------------------------------------------------------------
/**
 * �T�C�Y�擾
 *
 * @param	pw		���[�N
 * @param	x		X�T�C�Y�i�[�ꏊ
 * @param	y		Y�T�C�Y�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void PointerWkSizeGet( const POINTSEL_WORK * pw, u8 * x, u8 * y );
extern void POINTSEL_GetSize( const POINTSEL_WORK * pw, u8 * x, u8 * y );

//--------------------------------------------------------------------------------------------
/**
 * �ړ��������w�肵�A���W�A�T�C�Y�A�C���f�b�N�X���擾����
 *
 * @param	pw		���[�N
 * @param	px		X���W�i�[�ꏊ
 * @param	py		Y���W�i�[�ꏊ
 * @param	sx		X�T�C�Y�i�[�ꏊ
 * @param	sy		Y�T�C�Y�i�[�ꏊ
 * @param	now		���݂̃C���f�b�N�X
 * @param	mv		�ړ�����
 *
 * @return	�ړ���̃C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
//extern u8 PointerWkMoveSel(
extern u8 POINTSEL_MoveVec(
				const POINTSEL_WORK * pw, u8 * px, u8 * py, u8 * sx, u8 * sy, u8 now, u8 mv );

//--------------------------------------------------------------------------------------------
/**
 * �\���L�[�̏�񂩂�A���W�A�T�C�Y�A�C���f�b�N�X���擾����
 *
 * @param	pw		���[�N
 * @param	px		X���W�i�[�ꏊ
 * @param	py		Y���W�i�[�ꏊ
 * @param	sx		X�T�C�Y�i�[�ꏊ
 * @param	sy		Y�T�C�Y�i�[�ꏊ
 * @param	now		���݂̃C���f�b�N�X
 *
 * @return	�ړ���̃C���f�b�N�X�i�\���L�[��������Ă��Ȃ��ꍇ��POINTSEL_MOVE_NONE�j
 *
 * @li	sys.trg���g�p
 */
//--------------------------------------------------------------------------------------------
//extern u8 PointerWkMoveSelTrg(
extern u8 POINTSEL_MoveTrg(
				const POINTSEL_WORK * pw, u8 * px, u8 * py, u8 * sx, u8 * sy, u8 now );

//--------------------------------------------------------------------------------------------
/**
 * ���W���w�肵�A�C���f�b�N�X���擾����
 *
 * @param	pw		���[�N
 * @param	px		X���W
 * @param	py		Y���W
 * @param	siz		�f�[�^�T�C�Y
 *
 * @return	�w����W�̃C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
//extern u8 PointerWkMovePos( const POINTSEL_WORK * pw, u8 px, u8 py, u8 siz );
extern u8 POINTSEL_GetPosIndex( const POINTSEL_WORK * pw, u8 px, u8 py, u8 siz );
