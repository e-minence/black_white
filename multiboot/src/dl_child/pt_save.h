
#define CRC_LOADCHECK (1)
//---------------------------------------------------------------------------
///	�Z�[�u�Ɏg�p���Ă���Z�N�^��
//---------------------------------------------------------------------------
#define	SAVE_PAGE_MAX		(32)

#define	SAVE_SECTOR_SIZE	(0x1000)
//---------------------------------------------------------------------------
///	�Z�[�u�u���b�N�̒�`
//---------------------------------------------------------------------------
typedef enum {
	SVBLK_ID_NORMAL = 0,	///<�ʏ�Q�[���i�s�f�[�^
	SVBLK_ID_BOX,			///<�|�P�����{�b�N�X

	SVBLK_ID_MAX,
}SVBLK_ID;

//---------------------------------------------------------------------------
///	�Z�[�u���ڂ̒�`
//---------------------------------------------------------------------------
typedef enum {
	//�m�[�}���f�[�^�O���[�v
	GMDATA_ID_SYSTEM_DATA,
	GMDATA_ID_PLAYER_DATA,
	GMDATA_ID_TEMOTI_POKE,
	GMDATA_ID_TEMOTI_ITEM,
	GMDATA_ID_EVENT_WORK,
	GMDATA_ID_POKETCH_DATA,
	GMDATA_ID_SITUATION,
	GMDATA_ID_ZUKANWORK,
	GMDATA_ID_SODATEYA,
	GMDATA_ID_FRIEND,
	GMDATA_ID_MISC,
	GMDATA_ID_FIELDOBJSV,
	GMDATA_ID_UNDERGROUNDDATA,
    GMDATA_ID_REGULATION,
	GMDATA_ID_IMAGECLIPDATA,
	GMDATA_ID_MAILDATA,
	GMDATA_ID_PORUTODATA,
	GMDATA_ID_RANDOMGROUP,
	GMDATA_ID_FNOTE,
	GMDATA_ID_TRCARD,
	GMDATA_ID_RECORD,
	GMDATA_ID_CUSTOM_BALL,
	GMDATA_ID_PERAPVOICE,
	GMDATA_ID_FRONTIER,
	GMDATA_ID_SP_RIBBON,
	GMDATA_ID_ENCOUNT,
	GMDATA_ID_WORLDTRADEDATA,
	GMDATA_ID_TVWORK,
	GMDATA_ID_GUINNESS,
	GMDATA_ID_WIFILIST,
	GMDATA_ID_WIFIHISTORY,
	GMDATA_ID_FUSHIGIDATA,
	GMDATA_ID_POKEPARKDATA,
	GMDATA_ID_CONTEST,
	GMDATA_ID_PMS,
	GMDATA_ID_EMAIL,
	GMDATA_ID_WFHIROBA,

	//�{�b�N�X�f�[�^�O���[�v
	GMDATA_ID_BOXDATA,

	GMDATA_ID_MAX,
}GMDATA_ID;
//=============================================================================
//=============================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define MAGIC_NUMBER	(0x20060623)
#define	SECTOR_SIZE		(SAVE_SECTOR_SIZE)
#define SECTOR_MAX		(SAVE_PAGE_MAX)

#define	FIRST_MIRROR_START	(0)
#define	SECOND_MIRROR_START	(64)

#define MIRROR1ST	(0)
#define	MIRROR2ND	(1)
#define	MIRRORERROR	(2)

#define HEAPID_SAVE_TEMP	(HEAPID_BASE_APP)


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define	SEC_DATA_SIZE		SECTOR_SIZE


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef struct {
	u32 g_count;		///<�O���[�o���J�E���^�iMY�f�[�^�ABOX�f�[�^���L�j
	u32 b_count;		///<�u���b�N���J�E���^�iMY�f�[�^��BOX�f�[�^�ƂœƗ��j
	u32 size;			///<�f�[�^�T�C�Y�i�t�b�^�T�C�Y�܂ށj
	u32 magic_number;	///<�}�W�b�N�i���o�[
	u8 blk_id;			///<�Ώۂ̃u���b�N�w��ID
	u16 crc;			///<�f�[�^�S�̂�CRC�l
}SAVE_FOOTER;

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�u���b�N�f�[�^���
 */
//---------------------------------------------------------------------------
typedef struct {
	u8 id;				///<�u���b�N��`ID
	u8 start_sec;		///<�J�n�Z�N�^�ʒu
	u8 use_sec;			///<��L�Z�N�^��
	u32 start_ofs;		///<�Z�[�u�f�[�^�ł̊J�n�I�t�Z�b�g
	u32 size;			///<�Z�[�u�f�[�^�̐�L�T�C�Y
}SVBLK_INFO;

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^���ڂ��Ƃ̏���`
 */
//---------------------------------------------------------------------------
typedef struct {
	GMDATA_ID gmdataID;	///<�Z�[�u�f�[�^����ID
	u32 size;			///<�f�[�^�T�C�Y�i�[
	u32 address;		///<�f�[�^�J�n�ʒu
	u16 crc;			///<�G���[���o�pCRC�R�[�h�i�[
	u16 blockID;		///<�����u���b�N��ID
}SVPAGE_INFO;

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u���[�N�\����
 *
 * ���ۂ̃Z�[�u����镔���̍\��
 */
//---------------------------------------------------------------------------
typedef struct {
	u8 data[SECTOR_SIZE * SECTOR_MAX];	///<���ۂ̃f�[�^�ێ��̈�
}SAVEWORK;


//---------------------------------------------------------------------------
/**
 * @brief	�����]������p���[�N
 */
//---------------------------------------------------------------------------
typedef struct {
	BOOL total_save_mode;
	int block_start;
	int block_current;
	int block_end;
	s32 lock_id;
	int div_seq;
	u32 g_backup;
	u32 b_backup[SVBLK_ID_MAX];
	volatile BOOL lock_flg;
}NEWDIVSV_WORK;

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^�\����
 *
 * �Z�[�u�f�[�^���̂ƁA����p���[�N���܂Ƃ߂�����
 */
//---------------------------------------------------------------------------
typedef struct {
	BOOL flash_exists;			///<�o�b�N�A�b�vFLASH�����݂��邩�ǂ���
	BOOL data_exists;			///<�f�[�^�����݂��邩�ǂ���
	BOOL new_data_flag;			///<�u�������傩��v�̃f�[�^���ǂ���
	BOOL total_save_flag;		///<�S�̃Z�[�u���K�v���ǂ����̃t���O
//	LOAD_RESULT first_status;	///<��ԍŏ��̃Z�[�u�f�[�^�`�F�b�N����
	u32 first_status;			///<��ԍŏ��̃Z�[�u�f�[�^�`�F�b�N����(bit�w��)
#if !(CRC_LOADCHECK)
	MATHCRC16Table crc_table;	///<CRC�Z�o�p�e�[�u��
#endif //CRC_LOADCHECK
	SAVEWORK svwk;				///<�Z�[�u�f�[�^�{��

	u32 global_counter;
	u32 current_counters[SVBLK_ID_MAX];
	u8 current_side[SVBLK_ID_MAX];

	///�Z�[�u���ڃf�[�^���
	SVPAGE_INFO pageinfo[GMDATA_ID_MAX];

	///�Z�[�u�u���b�N�f�[�^���
	SVBLK_INFO blkinfo[SVBLK_ID_MAX];

	///�����]������p���[�N
	NEWDIVSV_WORK ndsw;

	int dendou_sector_switch;
	u32 dendou_counter;
}_SAVEDATA_PT;



