//============================================================================================
/**
 * @file		zukan_savedata_local.h
 * @brief		�}�ӃZ�[�u�f�[�^�����Q�ƒ�`
 * @author	mori / tamada GAME FREAK inc.
 * @date		2009.10.26
*/
//============================================================================================
#pragma	once

enum {
  POKEZUKAN_ARRAY_LEN = 21, ///<�|�P�����}�Ӄt���O�z��̂������� 21 * 32 = 672�܂ő��v

  MAGIC_NUMBER = 0xbeefcafe,

  POKEZUKAN_TEXT_POSSIBLE  = MONSNO_ARUSEUSU, // �C�O�}�Ӄe�L�X�g���ǂ߂�ő吔�i���V���I�E�|�P�����܂Łj

	// �����p��`
	SEE_FLAG_MALE = 0,		// ��
	SEE_FLAG_FEMALE,			// ��
	SEE_FLAG_M_RARE,			// �����A
	SEE_FLAG_F_RARE,			// �����A
	SEE_FLAG_MAX,

	// �t�H������	
	FORM_MAX_ANNOON = 28,					// �A���m�[��
	FORM_MAX_DEOKISISU = 4,				// �f�I�L�V�X
	FORM_MAX_SHEIMI = 2,					// �V�F�C�~
	FORM_MAX_GIRATHINA = 2,				// �M���e�B�i
	FORM_MAX_ROTOMU = 6,					// ���g��
	FORM_MAX_KARANAKUSI = 2,			// �J���i�N�V
	FORM_MAX_TORITODON = 2,				// �g���g�h��
	FORM_MAX_MINOMUTTI = 3,				// �~�m���b�`
	FORM_MAX_MINOMADAMU = 3,			// �~�m�}�_��
	FORM_MAX_POWARUN = 4,					// �|������ ������
	FORM_MAX_THERIMU = 2,					// �`�F���� ������
	FORM_MAX_BANBIINA = 4,				// �o���r�[�i ���V�K
	FORM_MAX_SIKIZIKA = 4,				// �V�L�W�J ���V�K
	FORM_MAX_MERODHIA = 2,				// �����f�B�A ���V�K
	FORM_MAX_HIHIDARUMA = 2,			// �q�q�_���} ���V�K
	FORM_MAX_BASURAO = 2,					// �o�X���I ���V�K
	/* ���v: 72 */

	FORM_ARRAY_LEN = 9,		///< �t�H�����t���O�z��̑傫�� 9 * 8 = 72�܂ő��v

	// �F��`
	COLOR_NORMAL = 0,		// �ʏ�J���[	
	COLOR_RARE,					// ���A�J���[
	COLOR_MAX,

	TEXTVER_VER_MAX = 7,				///< �����ސ�		��/�p/��/��/��/��/��
	TEXTVER_ARRAY_LEN = 432,		///< ����t���O		�|�P������ * ���ꐔ / 8 = 493 * 7 / 8 = 432
};


//----------------------------------------------------------
/**
 * @brief ������ԃf�[�^�^��`
 */
//----------------------------------------------------------
struct ZUKAN_SAVEDATA {
  u32 zukan_magic;					///< �}�W�b�N�i���o�[

	u32	zenkoku_flag:1;				///< �S���}�ӕێ��t���O
	u32	ver_up_flg:10;				///< �o�[�V�����A�b�v�t���O
	u32	zukan_mode:1;					///< �{�����̐}�Ӄ��[�h
	u32	defaultMonsNo:10;			///< �{�����̃|�P�����ԍ�
	u32	shortcutMonsNo:10;		///< �x�o�^���ꂽ�|�P�����ԍ��i������ɂ�薢�g�p�j

  u32 get_flag[POKEZUKAN_ARRAY_LEN];    ///< �߂܂����t���O�p���[�N

  u32 sex_flag[SEE_FLAG_MAX][POKEZUKAN_ARRAY_LEN];		///< �I�X���X�t���O�p���[�N
	u32	draw_sex[SEE_FLAG_MAX][POKEZUKAN_ARRAY_LEN];		///< �{�����I�X���X�t���O

  u8	form_flag[COLOR_MAX][FORM_ARRAY_LEN];						///< �t�H�����t���O�p���[�N
	u8	draw_form[COLOR_MAX][FORM_ARRAY_LEN];						///< �{�����t�H�����t���O

  u8 TextVersionUp[TEXTVER_ARRAY_LEN];	///< ����o�[�W�����A�b�v�}�X�N

  u32 PachiRandom;            ///< �p�b�`�[���p�������ێ����[�N
};

