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
};

//----------------------------------------------------------------
/**
 * �egmm�t�@�C�����̗v�f��
*/
//----------------------------------------------------------------

static const u16 PMS_SrcElems[] = {
	 583,	//�|�P������	0 ... 582
	 529,	//�킴��	583 ... 1111
	  17,	//�|�P�����^�C�v��	1112 ... 1128
	 160,	//�Ƃ�������	1129 ... 1288
	  38,	//�g���[�i�[	1289 ... 1326
	  38,	//�Ђ�	1327 ... 1364
	 107,	//��������	1365 ... 1471
	 104,	//��������	1472 ... 1575
	  10,	//������	1576 ... 1585
	  32,	//�Ȃ񂩂����Ƃ�	1586 ... 1617
	  23,	//���j�I��	1618 ... 1640
	  10,	//�s�N�`��	1641 ... 1650
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
};

//----------------------------------------------------------------
/**
 * �d���P��e�[�u��
*/
//----------------------------------------------------------------

static const PMS_WORD DupWord_00[] = {
	1375,1582,1647,
};

static const PMS_WORD DupWord_01[] = {
	1581,1646,
};

static const PMS_WORD DupWord_02[] = {
	1031,1483,
};

static const PMS_WORD DupWord_03[] = {
	1579,1644,
};

static const PMS_WORD DupWord_04[] = {
	959,1295,
};

static const PMS_WORD DupWord_05[] = {
	 93,1119,
};

static const PMS_WORD DupWord_06[] = {
	1415,1584,1649,
};

static const PMS_WORD DupWord_07[] = {
	1191,1299,
};

static const PMS_WORD DupWord_08[] = {
	1365,1577,1642,
};

static const PMS_WORD DupWord_09[] = {
	1583,1648,
};

static const PMS_WORD DupWord_10[] = {
	1578,1643,
};

static const PMS_WORD DupWord_11[] = {
	1580,1645,
};

static const PMS_WORD DupWord_12[] = {
	1438,1585,1650,
};

static const PMS_WORD DupWord_13[] = {
	1439,1576,1641,
};

static const PMS_WORD DupWord_14[] = {
	727,1279,
};

static const struct {
    const PMS_WORD* data;
    int   count;
}DupWordTable[] = {
	{ DupWord_00, 3 },
	{ DupWord_01, 2 },
	{ DupWord_02, 2 },
	{ DupWord_03, 2 },
	{ DupWord_04, 2 },
	{ DupWord_05, 2 },
	{ DupWord_06, 3 },
	{ DupWord_07, 2 },
	{ DupWord_08, 3 },
	{ DupWord_09, 2 },
	{ DupWord_10, 2 },
	{ DupWord_11, 2 },
	{ DupWord_12, 3 },
	{ DupWord_13, 3 },
	{ DupWord_14, 2 },
};

//----------------------------------------------------------------
/**
 * �֘A�萔
*/
//----------------------------------------------------------------

#define  PMS_SRCFILE_MAX  (12)

#undef __PMS_WORD_RES__
#endif

