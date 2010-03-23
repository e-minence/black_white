//============================================================================================
/**
 * @file  zukan_savedata_local.h
 * @brief �}�ӃZ�[�u�f�[�^�����Q�ƒ�`
 * @author  mori / tamada GAME FREAK inc.
 * @date  2009.10.26
*/
//============================================================================================


enum {
  POKEZUKAN_ARRAY_LEN = 21, ///<�|�P�����}�Ӄt���O�z��̂������� 21 * 32 = 672�܂ő��v

  POKEZUKAN_UNKNOWN_NUM = UNK_END,///<�A���m�[����

  MAGIC_NUMBER = 0xbeefcafe,

  POKEZUKAN_DEOKISISU_MSK = 0xf,  // bit�}�X�N  POKEZUKAN_ARRAY_LEN�̌��15bit�����f�I�L�V�X�Ɏg�p���Ă܂��B

  POKEZUKAN_TEXT_POSSIBLE  = MONSNO_ARUSEUSU, // �C�O�}�Ӄe�L�X�g���ǂ߂�ő吔�i���V���I�E�|�P�����܂Łj

  POKEZUKAN_TEXTVER_UP_NUM = POKEZUKAN_TEXT_POSSIBLE + 1, //  �����ڃA�b�v�f�[�g�o�b�t�@��  +1�̓p�f�B���O

	// �����p��`
	SEE_FLAG_MALE = 0,
	SEE_FLAG_FEMALE,
	SEE_FLAG_M_RARE,
	SEE_FLAG_F_RARE,
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
	FORM_MAX_HIHIDARUMA = 2,			// �V�L�W�J ���V�K
	FORM_MAX_BASURAO = 2,					// �o�X���I ���V�K
	/* ���v: 72 */

	FORM_ARRAY_LEN = 9,		///< �t�H�����t���O�z��̑傫�� 9 * 8 = 72�܂ő��v

	// �F��`
	COLOR_NORMAL = 0,
	COLOR_RARE,
	COLOR_MAX,

	TEXTVER_ARRAY_LEN = 370,		///< ����t���O
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
	u32	shortcutMonsNo:10;		///< �x�o�^���ꂽ�|�P�����ԍ�

  u32 get_flag[POKEZUKAN_ARRAY_LEN];    ///< �߂܂����t���O�p���[�N

  u32 sex_flag[SEE_FLAG_MAX][POKEZUKAN_ARRAY_LEN];		///< �I�X���X�t���O�p���[�N
	u32	draw_sex[SEE_FLAG_MAX][POKEZUKAN_ARRAY_LEN];		///< �{�����I�X���X�t���O
//	u32	draw_rare[COLOR_MAX][POKEZUKAN_ARRAY_LEN];			///< �{�������A�t���O

  u8	form_flag[COLOR_MAX][FORM_ARRAY_LEN];						///< �t�H�����t���O�p���[�N
	u8	draw_form[COLOR_MAX][FORM_ARRAY_LEN];						///< �{�����t�H�����t���O
//	u8	draw_form_rare[COLOR_MAX][FORM_ARRAY_LEN];			///< �{�����t�H�������A�t���O

  u8 TextVersionUp[TEXTVER_ARRAY_LEN];	///< ����o�[�W�����A�b�v�}�X�N

  u32 PachiRandom;            ///< �p�b�`�[���p�������ێ����[�N

/*
  u32 see_flag[POKEZUKAN_ARRAY_LEN];    ///<�������t���O�p���[�N

  u8 SiiusiTurn;              ///<�V�[�E�V�p���������ێ����[�N    2bit�K�v  1bit*2���
  u8 SiidorugoTurn;           ///<�V�[�h���S�p���������ێ����[�N  2bit�K�v  1bit*2���
  u8 MinomuttiTurn;           ///<�~�m���b�`�p���������ێ����[�N  6bit�K�v  2bit*3���
  u8 MinomesuTurn;            ///<�~�m���X�p���������ێ����[�N    6bit�K�v  2bit*3���
  u8 UnknownTurn[ POKEZUKAN_UNKNOWN_NUM ];///<�A���m�[�����������ԍ�
  u8 UnknownGetTurn[ POKEZUKAN_UNKNOWN_NUM ];///<�A���m�[����ߊl�����ԍ�
  u8 TextVersionUp[ POKEZUKAN_TEXTVER_UP_NUM ];///<����o�[�W�����A�b�v�}�X�N

  u8 GraphicVersionUp;          ///<�O���t�B�b�N�o�[�W�����p�@�\�g���t���O
  u8 TextVersionUpMasterFlag;       ///<����o�[�W�����A�b�vMaster�t���O

  ///<���g���p���������ԕێ����[�N    18bit�K�v 3bit*6���
  ///<�V�F�C�~�p���������ԕێ����[�N  2bit�K�v  1bit*2���
  ///<�M���e�B�i�p���������ԕێ����[�N  2bit�K�v  1bit*2���
  u32 rotomu_turn;
  u8  syeimi_turn;
  u8  giratyina_turn;
  u8  pityuu_turn;  ///< �s�`���[�p���������ێ����[�N 2bit*3���( ��/��/�M�U�~�~ )
  u8  turn_pad;   ///< �]��
*/

};

