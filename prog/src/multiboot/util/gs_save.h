
#define CRC_LOADCHECK (1)
//---------------------------------------------------------------------------
/// �Z�[�u�Ɏg�p���Ă���Z�N�^��
//---------------------------------------------------------------------------
#define GS_SAVE_PAGE_MAX   (35)

#define GS_SAVE_SECTOR_SIZE  (0x1000)
//---------------------------------------------------------------------------
/// �Z�[�u���ڂ̒�`
//---------------------------------------------------------------------------
typedef enum {
  //�m�[�}���f�[�^�O���[�v
  GS_GMDATA_ID_SYSTEM_DATA,
  GS_GMDATA_ID_PLAYER_DATA,
  GS_GMDATA_ID_TEMOTI_POKE,
  GS_GMDATA_ID_TEMOTI_ITEM,
  GS_GMDATA_ID_EVENT_WORK,
  GS_GMDATA_ID_SITUATION,
  GS_GMDATA_ID_ZUKANWORK,
  GS_GMDATA_ID_SODATEYA,
  GS_GMDATA_ID_FRIEND,
  GS_GMDATA_ID_MISC,
  GS_GMDATA_ID_FIELDOBJSV,
  GS_GMDATA_ID_REGULATION,
  GS_GMDATA_ID_IMAGECLIPDATA,
  GS_GMDATA_ID_MAILDATA,
  GS_GMDATA_ID_RANDOMGROUP,
  GS_GMDATA_ID_TRCARD,
  GS_GMDATA_ID_RECORD,
  GS_GMDATA_ID_CUSTOM_BALL,
  GS_GMDATA_ID_PERAPVOICE,
  GS_GMDATA_ID_FRONTIER,
  GS_GMDATA_ID_SP_RIBBON,
  GS_GMDATA_ID_ENCOUNT,
  GS_GMDATA_ID_WORLDTRADEDATA,
  GS_GMDATA_ID_TVWORK,
  GS_GMDATA_ID_GUINNESS,
  GS_GMDATA_ID_WIFILIST,
  GS_GMDATA_ID_WIFIHISTORY,
  GS_GMDATA_ID_FUSHIGIDATA,
  GS_GMDATA_ID_POKEPARKDATA,
  GS_GMDATA_ID_PKTH_WPOKE_SAVE,
  GS_GMDATA_ID_PMS,
  GS_GMDATA_ID_EMAIL,
  GS_GMDATA_ID_WFHIROBA,
  GS_GMDATA_ID_PAIR_POKE,
  GS_GMDATA_ID_PGEAR_SAVE,
  GS_GMDATA_ID_SAFARI_SAVE,
  GS_GMDATA_ID_PICTURE_SAVE,
  GS_GMDATA_ID_POKETHLON_SAVE,
  GS_GMDATA_ID_BONGURI_SAVE,
  GS_GMDATA_ID_PHC_SAVE,
  GS_GMDATA_ID_TR_HOUSE_SAVE,
  GS_GMDATA_NORMAL_FOOTER,  //WB�Œǉ��B�Z�[�u��CRC�Ƃ��J�E���^�Ƃ������Ă�
  GS_GMDATA_PADDING,        //WB�Œǉ��B�{�b�N�X�̑O��256�A���C�����g����Ă���̂�
  
  //�{�b�N�X�f�[�^�O���[�v
  GS_GMDATA_ID_BOXDATA,
  GS_GMDATA_BOX_FOOTER,     //WB�Œǉ��B�{�b�N�X��CRC�Ƃ��J�E���^�Ƃ������Ă�

  GS_GMDATA_ID_MAX,
}GS_GMDATA_ID;
//=============================================================================
//=============================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define MAGIC_NUMBER_GS (0x20060623)
#define GS_SECTOR_SIZE   (GS_SAVE_SECTOR_SIZE)
#define GS_SECTOR_MAX    (GS_SAVE_PAGE_MAX)

#define FIRST_MIRROR_START  (0)
#define SECOND_MIRROR_START (64)

#define MIRROR1ST (0)
#define MIRROR2ND (1)
#define MIRRORERROR (2)

#define HEAPID_SAVE_TEMP  (HEAPID_BASE_APP)

#define GS_SEC_DATA_SIZE   GS_SECTOR_SIZE

#define GS_SAVE_SIZE (GS_SECTOR_SIZE * GS_SECTOR_MAX)
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef struct {
  u32 g_count;    ///<�O���[�o���J�E���^�iMY�f�[�^�ABOX�f�[�^���L�j
  u32 size;     ///<�f�[�^�T�C�Y�i�t�b�^�T�C�Y�܂ށj
  u32 magic_number; ///<�}�W�b�N�i���o�[
  u8 blk_id;      ///<�Ώۂ̃u���b�N�w��ID
  u16 crc;      ///<�f�[�^�S�̂�CRC�l
}GS_SAVE_FOOTER;

//---------------------------------------------------------------------------
/**
 * @brief �Z�[�u���[�N�\����
 *
 * ���ۂ̃Z�[�u����镔���̍\��
 */
//---------------------------------------------------------------------------
/*
typedef struct {
  u8 data[GS_SECTOR_SIZE * GS_SECTOR_MAX];  ///<���ۂ̃f�[�^�ێ��̈�
}SAVEWORK;
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define GS_BOX_MAX_TRAY      (18)
#define GS_BOX_MAX_POS       (5*6)
#define GS_BOX_TRAYNAME_BUFSIZE  (20)  // ���{��W�����{EOM�B�C�O�ŗp�̗]�T�����Ă��̒��x�B
#define GS_BOX_TRAY_ALL_USE_BIT	(0b111111111111111111)

typedef struct
{
  u32 dummy[0x88/4];
}GS_POKEMON_PARAM;


//�t���b�V���̃y�[�W���܂����Ȃ��悤��1�g���[���̃f�[�^��256�o�C�g�A���C�����g����
typedef struct
{
  GS_POKEMON_PARAM  ppp[GS_BOX_MAX_POS];
  u8 dummy[16];
}GS_BOX_TRAY_DATA;

typedef struct {
/// POKEMON_PASO_PARAM  ppp[BOX_MAX_TRAY][BOX_MAX_POS];
  GS_BOX_TRAY_DATA   btd[GS_BOX_MAX_TRAY];
  //������艺�̃f�[�^���Q�T�U�o�C�g�A���C�����g���ꂽ�Ƃ���Ƀ}�b�s���O�����
  u32         currentTrayNumber;                //4
  u32         UseBoxBits;                   //4
  STRCODE     trayName[GS_BOX_MAX_TRAY][GS_BOX_TRAYNAME_BUFSIZE]; //2*20*18 = 720
  u8          wallPaper[GS_BOX_MAX_TRAY];            //18
  u8          daisukiBitFlag;                 //1
  u8          dummy[17];    //�A���C�����g�_�~�[      //17
  
  //�|�P�����f�[�^��0x1000*18 �o�C�g = 0x12000
  //�|�P�����f�[�^�ȊO��764�o�C�g = 0x2fc
  //�v0x122fc�o�C�g�@
  //����ɂb�q�b�S�o�C�g��ǉ�����ƁA0x12300�ƂȂ�A256�o�C�g�A���C�����g����A
  //�t�b�^�[���݂̂̏������݂̍ۂɁA�t���b�V���y�[�W���܂����Ȃ��Ȃ�
}GS_BOX_DATA;


//---------------------------------------------------------------------------
//  �A�C�e���֌W
//---------------------------------------------------------------------------
typedef struct
{
  u16 id;   // �A�C�e���ԍ�
  u16 no;   // ��
}GS_MINEITEM;

#define GS_BAG_NORMAL_ITEM_MAX    ( 165 )   // ����|�P�b�g�ő吔
#define GS_BAG_EVENT_ITEM_MAX     ( 50 )    // ��؂ȕ��|�P�b�g�ő吔
#define GS_BAG_WAZA_ITEM_MAX      ( 101 )   // �Z�}�V���|�P�b�g�ő吔
#define GS_BAG_SEAL_ITEM_MAX      ( 12 )    // �V�[���|�P�b�g�ő吔
#define GS_BAG_DRUG_ITEM_MAX      ( 40 )    // ��|�P�b�g�ő吔
#define GS_BAG_NUTS_ITEM_MAX      ( 64 )    // �؂̎��|�P�b�g�ő吔
#define GS_BAG_BALL_ITEM_MAX      ( 24 )    // �����X�^�[�{�[���|�P�b�g�ő吔
#define GS_BAG_BATTLE_ITEM_MAX    ( 30 )    // �퓬�p�A�C�e���|�P�b�g�ő吔

typedef struct {
  GS_MINEITEM MyNormalItem[ GS_BAG_NORMAL_ITEM_MAX ]; // �莝���̕��ʂ̓���
  GS_MINEITEM MyEventItem[ GS_BAG_EVENT_ITEM_MAX ];   // �莝���̑�؂ȓ���
  GS_MINEITEM MySkillItem[ GS_BAG_WAZA_ITEM_MAX ];    // �莝���̋Z�}�V��
  GS_MINEITEM MySealItem[ GS_BAG_SEAL_ITEM_MAX ];   // �莝���̃V�[��(���ۂ̓��[��
  GS_MINEITEM MyDrugItem[ GS_BAG_DRUG_ITEM_MAX ];   // �莝���̖�
  GS_MINEITEM MyNutsItem[ GS_BAG_NUTS_ITEM_MAX ];   // �莝���̖؂̎�
  GS_MINEITEM MyBallItem[ GS_BAG_BALL_ITEM_MAX ];   // �莝���̃����X�^�[�{�[��
  GS_MINEITEM MyBattleItem[ GS_BAG_BATTLE_ITEM_MAX ]; // �莝���̐퓬�p�A�C�e��
  u16 cnv_button1;                // �֗��{�^��1(Y�{�^���j
  u16 cnv_button2;                // �֗��{�^��2
}GS_MYITEM;

//------------------------------------------------------------------
//  �ӂ����f�[�^�̒�`
//------------------------------------------------------------------
#define GS_MYSTERYGIFT_TYPE_NONE		0	// ��������
#define GS_MYSTERYGIFT_TYPE_POKEMON	1	// �|�P����
#define GS_MYSTERYGIFT_TYPE_POKEEGG	2	// �^�}�S
#define GS_MYSTERYGIFT_TYPE_ITEM		3	// �ǂ���
#define GS_MYSTERYGIFT_TYPE_RULE		4	// ���[��
#define GS_MYSTERYGIFT_TYPE_GOODS		5	// �O�b�Y
#define GS_MYSTERYGIFT_TYPE_ACCESSORY	6	// �A�N�Z�T��
#define GS_MYSTERYGIFT_TYPE_RANGEREGG	7	// �}�i�t�B�[�̃^�}�S
#define GS_MYSTERYGIFT_TYPE_MEMBERSCARD	8	// �����o�[�Y�J�[�h
#define GS_MYSTERYGIFT_TYPE_LETTER		9	// �I�[�L�h�̂Ă���
#define GS_MYSTERYGIFT_TYPE_WHISTLE	10	// �Ă񂩂��̂ӂ�
#define GS_MYSTERYGIFT_TYPE_POKETCH	11	// �|�P�b�`
#define GS_MYSTERYGIFT_TYPE_SECRET_KEY	12	// �閧�̌�
#define GS_MYSTERYGIFT_TYPE_MOVIE		13	// �f��z�z
#define GS_MYSTERYGIFT_TYPE_PHC		14	// PHC�R�[�X
#define GS_MYSTERYGIFT_TYPE_PHOTO		15	// �ʐ^
#define GS_MYSTERYGIFT_TYPE_MAX	16	// 
#define GS_MYSTERYGIFT_TYPE_CLEAR		255	// �ӂ����̈�̋����N���A

#define GS_GIFT_DELIVERY_MAX	8	// �z�B���W��
#define GS_GIFT_CARD_MAX		3	// �J�[�h�f�[�^�R��
#define GS_GIFT_DATA_CARD_TITLE_MAX	36
#define GS_GIFT_DATA_CARD_TEXT_MAX		250
#define GS_MYSTERYGIFT_POKEICON	3
// �T�C�Y�Œ�p�\����
typedef struct
{
  u8 data[256];
}GS_GIFT_PRESENT_ALL;

// �ǂ���
typedef struct
{
  int itemNo;
  int movieflag;
}GS_GIFT_PRESENT_ITEM;

typedef union 
{
  GS_GIFT_PRESENT_ALL    all;
//  GIFT_PRESENT_POKEMON    pokemon;
//  GIFT_PRESENT_POKEEGG    egg;
  GS_GIFT_PRESENT_ITEM   item;
//  GIFT_PRESENT_GOODS    goods;
//  GIFT_PRESENT_RULE   rule;
//  GIFT_PRESENT_ACCESSORY  accessory;
//  GIFT_PRESENT_RANGEREGG  rangeregg;
//  GIFT_PRESENT_MEMBERSCARD  memberscard;
//  GIFT_PRESENT_LETTER   letter;
//  GIFT_PRESENT_WHISTLE    whistle;
//  GIFT_PRESENT_POKETCH    poketch;
//  GIFT_PRESENT_PHCMAP   phcmap;
//  GIFT_PRESENT_PICTURE    picture;
//  GIFT_PRESENT_REMOVE   remove;
}GS_GIFT_PRESENT;

// �ӂ����Ȃ�������́@�r�[�R���f�[�^
typedef struct
{
  STRCODE event_name[GS_GIFT_DATA_CARD_TITLE_MAX]; // �C�x���g�^�C�g��
  u32 version;            // �Ώۃo�[�W����(�O�̏ꍇ�͐��������Ŕz�z)
  u16 event_id;           // �C�x���g�h�c(�ő�2048���܂�)
  u8 only_one_flag: 1;    // �P�x������M�t���O(0..���x�ł���M�\ 1..�P��̂�)
  u8 access_point: 1;     // �A�N�Z�X�|�C���g(���������ĕK�v�Ȃ��Ȃ����H)
  u8 have_card: 1;        // �J�[�h�����܂�ł��邩(0..�܂�ł��Ȃ�  1..�܂�ł�)
  u8 delivery_flag: 1;    // �z�B������󂯎����̂��܂�ł��邩
  u8 re_deal_flag: 1;     // ���z�z���鎖���\���H(0..�o���Ȃ� 1..�o����)
  u8 groundchild_flag: 1; // ���z�z�t���O(0..�Ⴄ 1..���z�z)
  u8 dummy: 2;
}GS_GIFT_BEACON;

// �z�B��(�ő�W��)
typedef struct
{
  u16 gift_type;
  u16 link : 2;         // �J�[�h�ւ̃����N(0: 2:�����N 3�����N�Ȃ�)
  u16 dummy : 14;
  GS_GIFT_PRESENT data;
}GS_GIFT_DELIVERY;


// �J�[�h���(�ő�R��)
typedef struct 
{
  u16 gift_type;
  u16 dummy;            // �z�B���ւ̃����N
  GS_GIFT_PRESENT data;

  GS_GIFT_BEACON beacon;   // �r�[�R�����Ɠ����̏�������

  STRCODE event_text[GS_GIFT_DATA_CARD_TEXT_MAX];  // �����e�L�X�g
  u8 re_deal_count;     // �Ĕz�z�̉�(0�`254�A255�͖�����)
  u16 pokemon_icon[GS_MYSTERYGIFT_POKEICON]; // �|�P�����A�C�R���R��

  // ���z�z����̂͂����܂�
  // �����̉��̓t���b�V���ɃZ�[�u���鎞�̂ݕK�v�ȃf�[�^
  
  u8 re_dealed_count;       // �z�z������
  s32 recv_date;        // ��M��������
  
} GS_GIFT_CARD;

#define GS_FUSHIGI_DATA_MAX_EVENT		2048
typedef struct 
{
  u8 recv_flag[GS_FUSHIGI_DATA_MAX_EVENT / 8];   //256 * 8 = 2048 bit
  GS_GIFT_DELIVERY delivery[GS_GIFT_DELIVERY_MAX];    // �z�B���W��
  GS_GIFT_CARD card[GS_GIFT_CARD_MAX];      // �J�[�h���R��
  GS_GIFT_CARD rockcapcard;      // ���b�N�J�v�Z���p�P��
}GS_FUSHIGI_DATA;
