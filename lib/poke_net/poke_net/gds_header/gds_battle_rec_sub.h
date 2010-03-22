//==============================================================================
/**
 *
 * �퓬�^��̃T�u�\���̂�萔�̒�`�Ȃ�
 *
 */
//==============================================================================
#ifndef __GT_GDS_BATTLE_REC_SUB_H__
#define __GT_GDS_BATTLE_REC_SUB_H__


//==============================================================================
//  �萔��`
//==============================================================================
///�^��f�[�^�̑��ݗL��
#define REC_OCC_MAGIC_KEY   (0xe281)

///�^��w�b�_�Ɋi�[����|�P�����ԍ��̍ő�v�f��
#define HEADER_MONSNO_MAX   (12)

#define WAZA_TEMOTI_MAX   (4)   ///<1�̂̃|�P���������Ă�Z�̍ő�l

#define PERSON_NAME_SIZE  7 // �l���̖��O�̒����i�������܂ށj
#define MONS_NAME_SIZE    10  // �|�P�������̒���(�o�b�t�@�T�C�Y EOM_�܂܂�)
#define EOM_SIZE      1 // �I���R�[�h�̒���

#define TEMOTI_POKEMAX    6 ///< �莝���|�P�����ő吔

typedef enum {
  BTL_CLIENT_MAX = 4       ///< �ғ�����N���C�A���g��
}BTL_CLIENT;

typedef enum {
  BTLREC_OPERATION_BUFFER_SIZE = 0xc00   ///< �S�N���C�A���g�̑�����e�ێ��o�b�t�@�T�C�Y
}BTLREC;

typedef enum {
  TIMEZONE_MORNING =  0,
  TIMEZONE_NOON =     1,
  TIMEZONE_EVENING =  2,
  TIMEZONE_NIGHT =    3,
  TIMEZONE_MIDNIGHT = 4,

  TIMEZONE_MAX
}TIMEZONE;

//--------------------------------------------------------------
/**
 *  �V��
 */
//--------------------------------------------------------------
typedef enum {

  BTL_WEATHER_NONE = 0,   ///< �V��Ȃ�

  BTL_WEATHER_SHINE,    ///< �͂�
  BTL_WEATHER_RAIN,     ///< ����
  BTL_WEATHER_SNOW,     ///< �����
  BTL_WEATHER_SAND,     ///< ���Ȃ��炵
  BTL_WEATHER_MIST,     ///< ����

  BTL_WEATHER_MAX

}BtlWeather;

//--------------------------------------------------------------
/**
 *  �퓬�w�i�w��^�C�vID
 */
//--------------------------------------------------------------
typedef enum {
  BATTLE_BG_TYPE_GRASS,         ///< ���ނ�
  BATTLE_BG_TYPE_GRASS_SEASON,  ///< ���ނ�(�l�G�L��)
  BATTLE_BG_TYPE_CITY,          ///< �X
  BATTLE_BG_TYPE_CITY_SEASON,   ///< �X(�l�G�L��)
  BATTLE_BG_TYPE_CAVE,          ///< ���A
  BATTLE_BG_TYPE_CAVE_DARK,     ///< ���A(�Â�)
  BATTLE_BG_TYPE_FOREST,        ///< �X
  BATTLE_BG_TYPE_MOUNTAIN,      ///< �R
  BATTLE_BG_TYPE_SEA,           ///< �C
  BATTLE_BG_TYPE_ROOM,          ///< ����
  BATTLE_BG_TYPE_SAND,          ///< ����
   
  BATTLE_BG_TYPE_MAX
}BtlBgType;

//--------------------------------------------------------------
/*
 *  �퓬�w�i�w��A�g���r���[�gID
 */
//--------------------------------------------------------------
typedef enum {
  //�Ő�
  BATTLE_BG_ATTR_LAWN,
  //�ʏ�n��
  BATTLE_BG_ATTR_NORMAL_GROUND,
  //�n�ʂP
  BATTLE_BG_ATTR_GROUND1,
  //�n�ʂQ
  BATTLE_BG_ATTR_GROUND2,
  //��
  BATTLE_BG_ATTR_GRASS,
  //����
  BATTLE_BG_ATTR_WATER,
  //�ጴ
  BATTLE_BG_ATTR_SNOW,
  //���n
  BATTLE_BG_ATTR_SAND,
  //�󂢎���
  BATTLE_BG_ATTR_MARSH,
  //���A
  BATTLE_BG_ATTR_CAVE,
  //�����܂�
  BATTLE_BG_ATTR_POOL,
  //��
  BATTLE_BG_ATTR_SHOAL,
  //�X��
  BATTLE_BG_ATTR_ICE,

  BATTLE_BG_ATTR_MAX
}BtlBgAttr;


//--------------------------------------------------------------
//  �o�g�����[�h
//--------------------------------------------------------------
// �u�V���O���v�u�_�u���v�u�g���v���v�u���[�e�[�V�����v�u�}���`�v
#define BATTLEMODE_TYPE_BIT 0

//�u�ʐM�v���u�T�u�E�F�C�v���B
//�ʐM�E�T�u�E�F�C�A�ǂ���̃����L���O�Ɋ܂߂邩�́A������𗘗p���܂��B
#define BATTLEMODE_COMMUNICATION_BIT 3

//�u�ʐM�v�̏ꍇ�͒ʐM�̃^�C�v�A�u�T�u�E�F�C�v�̏ꍇ�͂O
#define BATTLEMODE_COMMMODE_BIT 4

// �u�����_���}�b�`�v�̏ꍇ�A�u�t���[�v���u���[�e�B���O��v��
#define BATTLEMODE_RATING_BIT 6

// �V���[�^�[���L��������
#define BATTLEMODE_SHOOTER_BIT 7

// ���x�����M�����[�V�����i�t���[�A�X�^���_�[�h�A���x���T�O�j
#define BATTLEMODE_REGULATION_BIT 8

typedef enum{
	BATTLEMODE_TYPE_SINGLE = 0,
	BATTLEMODE_TYPE_DOUBLE,
	BATTLEMODE_TYPE_TRIPLE,
	BATTLEMODE_TYPE_ROTATION,
	BATTLEMODE_TYPE_MULTI
} BATTLEMODE_TYPE;

typedef enum{
	BATTLEMODE_COMMUNICATION_SUBWAY = 0,
	BATTLEMODE_COMMUNICATION_COMMUNICATION = 1
} BATTLEMODE_COMMUNICATION;

typedef enum{
	BATTLEMODE_COMMMODE_SUBWAY = 0,
	BATTLEMODE_COMMMODE_COLOSSEUM,
	BATTLEMODE_COMMMODE_RANDOM,
	BATTLEMODE_COMMMODE_CHAMPIONSHIP
} BATTLEMODE_COMMMODE;

typedef enum {
	BATTLEMODE_RATING_FREE = 0,
	BATTLEMODE_RATING_RATING
} BATTLEMODE_RATING;

typedef enum {
	BATTLEMODE_SHOOTER_OFF = 0,
	BATTLEMODE_SHOOTER_ON
} BATTLEMODE_SHOOTER;

typedef enum {
	BATTLEMODE_REGULATION_FREE = 0,
	BATTLEMODE_REGULATION_STANDARD,
	BATTLEMODE_REGULATION_LEVEL50
} BATTLEMODE_REGULATION;

#define BATTLEMODE_TYPE_MASK 0x7
#define BATTLEMODE_COMMUNICATION_MASK 0x8

//�u�ʐM�v�̏ꍇ�͒ʐM�̃^�C�v�A�u�T�u�E�F�C�v�̏ꍇ�͂O
#define BATTLEMODE_COMMMODE_MASK 0x30

// �u�����_���}�b�`�v�̏ꍇ�A�u�t���[�v���u���[�e�B���O��v��
#define BATTLEMODE_RATING_MASK 0x40

// �V���[�^�[���L��������
#define BATTLEMODE_SHOOTER_MASK 0x80

// ���x�����M�����[�V�����i�t���[�A�X�^���_�[�h�A���x���T�O�j
#define BATTLEMODE_REGULATION_MASK 0x300


//--------------------------------------------------------------
//  ���������h�w��Ȃ��h�̒�`
//--------------------------------------------------------------
///�|�P�����w�薳��
#define BATTLE_REC_SEARCH_MONSNO_NONE     (0xffff)
///���R�[�h�w�薳��
#define BATTLE_REC_SEARCH_COUNTRY_CODE_NONE   (0xff)
///�n���R�[�h�w�薳��
#define BATTLE_REC_SEARCH_LOCAL_CODE_NONE   (0xff)

//==============================================================================
//  �^��`
//==============================================================================
///�f�[�^�i���o�[�̌^
typedef u64 DATA_NUMBER;


//----------------------------------------------------------
/**
 * @brief ������ԃf�[�^�^��`
 */
//----------------------------------------------------------
typedef struct {
  STRCODE name[PERSON_NAME_SIZE + EOM_SIZE];    // 16
  u32 id;               // 20 PlayerID
  u32 profileID;        // 24
  u8 nation;            ///< ��  25
  u8 area;              ///< �n�� 26
  u8 region_code;       //PM_LANG   27
  u8 rom_code;          // PM_VERSION   28
  u8 trainer_view;  // ���j�I�����[�����ł̌����ڃt���O29
  u8 sex;            //���� 30
  u8 dummy2;                    // 31
  u8 dummy3;                    // 32
}MYSTATUS;


//--------------------------------------------------------------
/**
 * �ΐ�^��p�ɃJ�X�^�}�C�Y���ꂽ�|�P�����p�����[�^
 * 112byte
 */
//--------------------------------------------------------------
typedef struct{
  u32 personal_rnd;             //04h ������
  u16 pp_fast_mode    :1;         //06h �Í��^�����^�`�F�b�N�T����������񂵂ɂ��āA���������������[�h
  u16 ppp_fast_mode   :1;         //06h �Í��^�����^�`�F�b�N�T����������񂵂ɂ��āA���������������[�h
  u16 fusei_tamago_flag :1;         //06h �_���^�}�S�t���O
  u16 seikaku:8;                  // ���i
  u16           :5;        //06h
  
  u16 monsno;               //02h �����X�^�[�i���o�[    
  u16 item;               //04h �����A�C�e���i���o�[  
  u32 id_no;                //08h IDNo
  u32 exp;                //0ch �o���l
  u8  friendshipness;           //0dh �Ȃ��x
  u8  speabino;             //0eh ����\��
  u8  hp_exp;               //11h HP�w�͒l
  u8  pow_exp;              //12h �U���͓w�͒l
  u8  def_exp;              //13h �h��͓w�͒l
  u8  agi_exp;              //14h �f�����w�͒l
  u8  spepow_exp;             //15h ���U�w�͒l
  u8  spedef_exp;             //16h ���h�w�͒l

  u16 waza[WAZA_TEMOTI_MAX];        //08h �����Z
  u8  pp[WAZA_TEMOTI_MAX];        //0ch �����ZPP
  u8  pp_count[WAZA_TEMOTI_MAX];      //10h �����ZPP_COUNT
  u32 hp_rnd      :5;         //    HP����
  u32 pow_rnd     :5;         //    �U���͗���
  u32 def_rnd     :5;         //    �h��͗���
  u32 agi_rnd     :5;         //    �f��������
  u32 spepow_rnd    :5;         //    ���U����
  u32 spedef_rnd    :5;         //    ���h����
  u32 tamago_flag   :1;         //    �^�}�S�t���O�i0:�^�}�S����Ȃ��@1:�^�}�S����j
  u32 nickname_flag :1;         //14h �j�b�N�l�[�����������ǂ����t���O
  u8  event_get_flag  :1;         //    �C�x���g�Ŕz�z�������Ƃ������t���O
  u8  sex       :2;         //    �|�P�����̐���
  u8  form_no     :5;         //19h �`��i���o�[
  
  STRCODE nickname[MONS_NAME_SIZE+EOM_SIZE];  //16h �j�b�N�l�[��
  
  STRCODE oyaname[PERSON_NAME_SIZE+EOM_SIZE]; //10h �e�̖��O
  u8  get_ball;               //1ch �߂܂����{�[��
  u8  language;               // ����R�[�h
  
  u32     condition;              //04h �R���f�B�V����
  u8      level;                //05h ���x��
  u8      cb_id;                //06h �J�X�^���{�[��ID
  u16     hp;                 //08h HP
  u16     hpmax;                //0ah HPMAX
  u16     pow;                //0ch �U����
  u16     def;                //0eh �h���
  u16     agi;                //10h �f����
  u16     spepow;               //12h ���U
  u16     spedef;               //14h ���h
  
} REC_POKEPARA;

//--------------------------------------------------------------
/**
 *  �퓬�^��p�ɃJ�X�^�}�C�Y���ꂽPOKEPARTY
 *  676 byte
 */
//--------------------------------------------------------------
typedef struct{
  /// �ێ��ł���|�P�������̍ő�
  u16 PokeCountMax;
  /// ���ݕێ����Ă���|�P������
  u16 PokeCount;
  /// �|�P�����f�[�^
  REC_POKEPARA member[TEMOTI_POKEMAX];
}REC_POKEPARTY;

//---------------------------------------------------------------------------
/**
 * @brief �ݒ�f�[�^�\���̒�`
 *      4byte
 */
//---------------------------------------------------------------------------
typedef struct {
  u16 msg_speed:4;      ///<MSGSPEED    �l�r�f����̑��x
  u16 sound_mode:2;     ///<SOUNDMODE   �T�E���h�o��
  u16 battle_rule:1;      ///<BATTLERULE    �퓬���[��
  u16 wazaeff_mode:1;     ///<WAZAEFF_MODE  �킴�G�t�F�N�g
  u16 moji_mode:1;      ///<MOJIMODE
  u16 wirelesssave_mode:1;      ///<WIRELESSSAVE_MODE   ���C�����X�ŃZ�[�u���͂��ނ��ǂ���
  u16 network_search:1;         ///<NETWORK_SEARCH_MODE �Q�[�����Ƀr�[�R���T�[�`���邩�ǂ���

  u16 padding1:5;
  u16 padding2;
}CONFIG;


//-----------------------------------------------------------------------------------
/**
 * �t�B�[���h�̏�Ԃ��猈�肳���o�g���V�`���G�[�V�����f�[�^
 */
//-----------------------------------------------------------------------------------
typedef struct {

  BtlBgType   bgType;
  BtlBgAttr   bgAttr;
  BtlWeather  weather;

  TIMEZONE    timeZone;
  u8          season;
}BTL_FIELD_SITUATION;

/**
  *  �o�g����ʃZ�b�g�A�b�v�p�����[�^�̘^��f�[�^�p�T�u�Z�b�g
 */
typedef struct _BTLREC_SETUP_SUBSET {
  GFL_STD_RandContext   randomContext;    ///< �����̃^�l
  BTL_FIELD_SITUATION   fieldSituation;   ///< �w�i�E���~�Ȃǂ̍\�z�p���
  CONFIG                config;           ///< ���[�U�[�R���t�B�O
  u16  musicDefault;            ///< �f�t�H���gBGM
  u16  musicPinch;              ///< �s���`��BGM
  u16  debugFlagBit;            ///< �f�o�b�O�@�\Bit�t���O -> enum BtlDebugFlag @ battle/battle.h
  u8   competitor : 5;          ///< �ΐ�҃^�C�v�i�Q�[�����g���[�i�[�A�ʐM�ΐ�j-> enum BtlCompetitor @ battle/battle.h
  u8   myCommPos  : 3;          ///< �ʐM�ΐ펞�̎����̗����ʒu�i�}���`�̎��A0,2 vs 1,3 �ɂȂ�A0,1�������^2,3���E���ɂȂ�j
  u8   rule       : 7;          ///< ���[���i�V���O���E�_�u���E�g���v���E���[�e�j-> enum BtlRule @ battle/battle.h
  u8   fMultiMode :1;           ///< �}���`�o�g���t���O�i���[���͕K���_�u���j

}BTLREC_SETUP_SUBSET;

/**
  *  �N���C�A���g������e�ۑ��o�b�t�@
 */
typedef struct {
  u32  size;
  u8   buffer[ BTLREC_OPERATION_BUFFER_SIZE ];
}BTLREC_OPERATION_BUFFER;

typedef enum {
  BTLREC_CLIENTSTATUS_NONE,
  BTLREC_CLIENTSTATUS_PLAYER,
  BTLREC_CLIENTSTATUS_TRAINER
}BtlRecClientStatusType;

typedef enum {
  BSP_TRAINERDATA_ITEM_MAX = 4
}BSP_TRAINERDATA;

#define BUFLEN_PERSON_NAME        (16)  // �l�����i�v���C���[�܂ށj�i�C�O14�����܂�+EOM�j

/**
  *  �^��Z�[�u�p�g���[�i�[�f�[�^
 */
typedef struct {
  u32     ai_bit;
  u16     tr_id;
  u16     tr_type;
  u16     use_item[ BSP_TRAINERDATA_ITEM_MAX ]; // �g�p����A�C�e��u16x4 = 8byte
  /// 16 byte

  STRCODE name[BUFLEN_PERSON_NAME];  // �g���[�i�[��<-32byte

  PMS_DATA  win_word;   //�퓬�I�����������b�Z�[�W <-8byte
  PMS_DATA  lose_word;  //�퓬�I�����������b�Z�[�W <-8byte

}BTLREC_TRAINER_STATUS;

typedef struct {
  BtlRecClientStatusType    type;
  union {
    MYSTATUS             player;
    BTLREC_TRAINER_STATUS   trainer;
  };
}BTLREC_CLIENT_STATUS;


//----------------------------------------------------------
/**
 *  �^��Z�[�u�f�[�^�{�́i6116byte�j
 */
//----------------------------------------------------------
typedef struct {
  BTLREC_SETUP_SUBSET       setupSubset;   ///< �o�g����ʃZ�b�g�A�b�v�p�����[�^�̃T�u�Z�b�g
  BTLREC_OPERATION_BUFFER   opBuffer;      ///< �N���C�A���g������e�̕ۑ��o�b�t�@

  REC_POKEPARTY         rec_party[ BTL_CLIENT_MAX ];
  BTLREC_CLIENT_STATUS  clientStatus[ BTL_CLIENT_MAX ];

  u16 magic_key;
  u16 padding;
  
  //u32         checksum; //�`�F�b�N�T��  4byte
  GDS_CRC crc;              ///< CRC            4
}BATTLE_REC_WORK;

//--------------------------------------------------------------
/**
 *  �퓬�^��̃w�b�_
 *    68byte
 */
//--------------------------------------------------------------
typedef struct {
  u16 monsno[HEADER_MONSNO_MAX];  ///<�|�P�����ԍ�(�\������K�v���Ȃ��̂Ń^�}�S�̏ꍇ��0) 24
  u8 form_no_and_sex[HEADER_MONSNO_MAX];  ///<6..0bit�ځF�|�P�����̃t�H�����ԍ��@7bit�ځF�|�P�����̐���		12

  u16 battle_counter;   ///<�A����                        2
  u16 mode;        ///<�퓬���[�h(�t�@�N�g���[50�A�t�@�N�g���[100�A�ʐM�ΐ�...)
  
  u16 magic_key;      ///<�}�W�b�N�L�[
  u8 secure;        ///<TRUE:���S���ۏႳ��Ă���B�@FALSE�F�Đ����������Ȃ�
  
  u8 server_vesion;   ///<�o�g���̃T�[�o�[�o�[�W����
  u8 work[12];      ///< �\��
  
  DATA_NUMBER data_number;    ///<�f�[�^�i���o�[(�T�[�o�[���ŃZ�b�g�����)  8
  GDS_CRC crc;              ///< CRC            4
}BATTLE_REC_HEADER;


#endif  //__GT_GDS_BATTLE_REC_SUB_H__

