
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

