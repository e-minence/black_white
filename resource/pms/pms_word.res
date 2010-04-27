// ���̃t�@�C���̓R���o�[�^�Ŏ�����������Ă��܂�
// ���Ƃł̕ҏW�͍s��Ȃ��悤�ɂ��肢���܂�
// Game Freak   taya


#ifdef __PMS_WORD_RES__	// �P��̃t�@�C������̂�include��������

//----------------------------------------------------------------
/**
 * �egmm�t�@�C���w��̂��߂̒萔
*/
//----------------------------------------------------------------

enum {
	PMSW_SRC_POKEMON,
	PMSW_SRC_SKILL,
	PMSW_SRC_TYPE,
	PMSW_SRC_TOKUSEI,
	PMSW_SRC_TRAINER,
	PMSW_SRC_HITO,
	PMSW_SRC_AISATSU,
	PMSW_SRC_SEIKATSU,
	PMSW_SRC_KIMOTI,
	PMSW_SRC_NANKAI,
	PMSW_SRC_UNION,
	PMSW_SRC_PICTURE,
	PMSW_SRC_KOE,
};

//----------------------------------------------------------------
/**
 * �egmm�t�@�C�����̗v�f��
*/
//----------------------------------------------------------------

static const u16 PMS_SrcElems[] = {
	 652,	//�|�P������	0 ... 651
	 558,	//�킴��	652 ... 1209
	  17,	//�|�P�����^�C�v��	1210 ... 1226
	 163,	//�Ƃ�������	1227 ... 1389
	  44,	//�g���[�i�[	1390 ... 1433
	  38,	//�Ђ�	1434 ... 1471
	  48,	//��������	1472 ... 1519
	 103,	//��������	1520 ... 1622
	  47,	//������	1623 ... 1669
	  32,	//�Ȃ񂩂����Ƃ�	1670 ... 1701
	  26,	//���j�I��	1702 ... 1727
	  10,	//�s�N�`��	1728 ... 1737
	  61,	//����	1738 ... 1798
};

//----------------------------------------------------------------
/**
 * �egmm�t�@�C���̃A�[�J�C�u���t�@�C��ID
*/
//----------------------------------------------------------------

static const u16 PMS_SrcFileID[] = {
	NARC_message_monsname_dat,
	NARC_message_wazaname_dat,
	NARC_message_typename_dat,
	NARC_message_tokusei_dat,
	NARC_message_pms_word06_dat,
	NARC_message_pms_word07_dat,
	NARC_message_pms_word08_dat,
	NARC_message_pms_word09_dat,
	NARC_message_pms_word10_dat,
	NARC_message_pms_word11_dat,
	NARC_message_pms_word12_dat,
	NARC_message_pms_picture_dat,
	NARC_message_pms_word13_dat,
};

//----------------------------------------------------------------
/**
 * �d���P��e�[�u��
*/
//----------------------------------------------------------------

static const PMS_WORD DupWord_00[] = {
	1481,1734,
};

static const PMS_WORD DupWord_01[] = {
	1100,1532,
};

static const PMS_WORD DupWord_02[] = {
	1028,1398,
};

static const PMS_WORD DupWord_03[] = {
	 93,1217,
};

static const PMS_WORD DupWord_04[] = {
	1494,1736,
};

static const PMS_WORD DupWord_05[] = {
	1289,1403,
};

static const PMS_WORD DupWord_06[] = {
	1472,1729,
};

static const PMS_WORD DupWord_07[] = {
	1644,1730,
};

static const PMS_WORD DupWord_08[] = {
	1510,1737,
};

static const PMS_WORD DupWord_09[] = {
	1511,1728,
};

static const struct {
    const PMS_WORD* data;
    int   count;
}DupWordTable[] = {
	{ DupWord_00, 2 },
	{ DupWord_01, 2 },
	{ DupWord_02, 2 },
	{ DupWord_03, 2 },
	{ DupWord_04, 2 },
	{ DupWord_05, 2 },
	{ DupWord_06, 2 },
	{ DupWord_07, 2 },
	{ DupWord_08, 2 },
	{ DupWord_09, 2 },
};

//----------------------------------------------------------------
/**
 * �֘A�萔
*/
//----------------------------------------------------------------

#define  PMS_SRCFILE_MAX  (13)

#undef __PMS_WORD_RES__
#endif

