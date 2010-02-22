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
	 557,	//�킴��	652 ... 1208
	  17,	//�|�P�����^�C�v��	1209 ... 1225
	 162,	//�Ƃ�������	1226 ... 1387
	  38,	//�g���[�i�[	1388 ... 1425
	  38,	//�Ђ�	1426 ... 1463
	 102,	//��������	1464 ... 1565
	 104,	//��������	1566 ... 1669
	  10,	//������	1670 ... 1679
	  32,	//�Ȃ񂩂����Ƃ�	1680 ... 1711
	  23,	//���j�I��	1712 ... 1734
	  10,	//�s�N�`��	1735 ... 1744
	   6,	//����	1745 ... 1750
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
	1470,1676,1741,
};

static const PMS_WORD DupWord_01[] = {
	1675,1740,
};

static const PMS_WORD DupWord_02[] = {
	1100,1577,
};

static const PMS_WORD DupWord_03[] = {
	1673,1738,
};

static const PMS_WORD DupWord_04[] = {
	1028,1394,
};

static const PMS_WORD DupWord_05[] = {
	 93,1216,
};

static const PMS_WORD DupWord_06[] = {
	1509,1678,1743,
};

static const PMS_WORD DupWord_07[] = {
	1288,1398,
};

static const PMS_WORD DupWord_08[] = {
	1464,1671,1736,
};

static const PMS_WORD DupWord_09[] = {
	1677,1742,
};

static const PMS_WORD DupWord_10[] = {
	1672,1737,
};

static const PMS_WORD DupWord_11[] = {
	1674,1739,
};

static const PMS_WORD DupWord_12[] = {
	1532,1679,1744,
};

static const PMS_WORD DupWord_13[] = {
	1533,1670,1735,
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
};

//----------------------------------------------------------------
/**
 * �֘A�萔
*/
//----------------------------------------------------------------

#define  PMS_SRCFILE_MAX  (13)

#undef __PMS_WORD_RES__
#endif

