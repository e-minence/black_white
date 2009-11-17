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
	 530,	//�킴��	583 ... 1112
	  18,	//�|�P�����^�C�v��	1113 ... 1130
	 124,	//�Ƃ�������	1131 ... 1254
	  38,	//�g���[�i�[	1255 ... 1292
	  38,	//�Ђ�	1293 ... 1330
	 107,	//��������	1331 ... 1437
	 104,	//��������	1438 ... 1541
	  10,	//������	1542 ... 1551
	  32,	//�Ȃ񂩂����Ƃ�	1552 ... 1583
	  23,	//���j�I��	1584 ... 1606
	  10,	//�s�N�`��	1607 ... 1616
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
	1341,1548,1613,
};

static const PMS_WORD DupWord_01[] = {
	1547,1612,
};

static const PMS_WORD DupWord_02[] = {
	1031,1449,
};

static const PMS_WORD DupWord_03[] = {
	1545,1610,
};

static const PMS_WORD DupWord_04[] = {
	959,1261,
};

static const PMS_WORD DupWord_05[] = {
	 93,1120,
};

static const PMS_WORD DupWord_06[] = {
	1381,1550,1615,
};

static const PMS_WORD DupWord_07[] = {
	1193,1265,
};

static const PMS_WORD DupWord_08[] = {
	1331,1543,1608,
};

static const PMS_WORD DupWord_09[] = {
	1549,1614,
};

static const PMS_WORD DupWord_10[] = {
	1544,1609,
};

static const PMS_WORD DupWord_11[] = {
	1546,1611,
};

static const PMS_WORD DupWord_12[] = {
	1404,1551,1616,
};

static const PMS_WORD DupWord_13[] = {
	1405,1542,1607,
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

#define  PMS_SRCFILE_MAX  (12)

#undef __PMS_WORD_RES__
#endif

