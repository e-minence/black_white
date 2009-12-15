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
	 626,	//�|�P������	0 ... 625
	 529,	//�킴��	626 ... 1154
	  17,	//�|�P�����^�C�v��	1155 ... 1171
	 160,	//�Ƃ�������	1172 ... 1331
	  38,	//�g���[�i�[	1332 ... 1369
	  38,	//�Ђ�	1370 ... 1407
	 107,	//��������	1408 ... 1514
	 104,	//��������	1515 ... 1618
	  10,	//������	1619 ... 1628
	  32,	//�Ȃ񂩂����Ƃ�	1629 ... 1660
	  23,	//���j�I��	1661 ... 1683
	  10,	//�s�N�`��	1684 ... 1693
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
	1418,1625,1690,
};

static const PMS_WORD DupWord_01[] = {
	1624,1689,
};

static const PMS_WORD DupWord_02[] = {
	1074,1526,
};

static const PMS_WORD DupWord_03[] = {
	1622,1687,
};

static const PMS_WORD DupWord_04[] = {
	1002,1338,
};

static const PMS_WORD DupWord_05[] = {
	 93,1162,
};

static const PMS_WORD DupWord_06[] = {
	1458,1627,1692,
};

static const PMS_WORD DupWord_07[] = {
	1234,1342,
};

static const PMS_WORD DupWord_08[] = {
	1408,1620,1685,
};

static const PMS_WORD DupWord_09[] = {
	1626,1691,
};

static const PMS_WORD DupWord_10[] = {
	1621,1686,
};

static const PMS_WORD DupWord_11[] = {
	1623,1688,
};

static const PMS_WORD DupWord_12[] = {
	1481,1628,1693,
};

static const PMS_WORD DupWord_13[] = {
	1482,1619,1684,
};

static const PMS_WORD DupWord_14[] = {
	770,1322,
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

