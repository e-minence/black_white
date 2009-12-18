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
	 545,	//�킴��	626 ... 1170
	  17,	//�|�P�����^�C�v��	1171 ... 1187
	 160,	//�Ƃ�������	1188 ... 1347
	  38,	//�g���[�i�[	1348 ... 1385
	  38,	//�Ђ�	1386 ... 1423
	 107,	//��������	1424 ... 1530
	 104,	//��������	1531 ... 1634
	  10,	//������	1635 ... 1644
	  32,	//�Ȃ񂩂����Ƃ�	1645 ... 1676
	  23,	//���j�I��	1677 ... 1699
	  10,	//�s�N�`��	1700 ... 1709
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
	1434,1641,1706,
};

static const PMS_WORD DupWord_01[] = {
	1640,1705,
};

static const PMS_WORD DupWord_02[] = {
	1074,1542,
};

static const PMS_WORD DupWord_03[] = {
	1638,1703,
};

static const PMS_WORD DupWord_04[] = {
	1002,1354,
};

static const PMS_WORD DupWord_05[] = {
	 93,1178,
};

static const PMS_WORD DupWord_06[] = {
	1474,1643,1708,
};

static const PMS_WORD DupWord_07[] = {
	1250,1358,
};

static const PMS_WORD DupWord_08[] = {
	1424,1636,1701,
};

static const PMS_WORD DupWord_09[] = {
	1642,1707,
};

static const PMS_WORD DupWord_10[] = {
	1637,1702,
};

static const PMS_WORD DupWord_11[] = {
	1639,1704,
};

static const PMS_WORD DupWord_12[] = {
	1497,1644,1709,
};

static const PMS_WORD DupWord_13[] = {
	1498,1635,1700,
};

static const PMS_WORD DupWord_14[] = {
	770,1338,
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

