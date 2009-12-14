
#pragma once

#define CRC_LOADCHECK (1)
//---------------------------------------------------------------------------
/// �Z�[�u�Ɏg�p���Ă���Z�N�^��
//---------------------------------------------------------------------------
#define PT_SAVE_PAGE_MAX   (32)

#define PT_SAVE_SECTOR_SIZE  (0x1000)

//---------------------------------------------------------------------------
/// �Z�[�u���ڂ̒�`
//---------------------------------------------------------------------------
typedef enum {
  //�m�[�}���f�[�^�O���[�v
  PT_GMDATA_ID_SYSTEM_DATA,
  PT_GMDATA_ID_PLAYER_DATA,
  PT_GMDATA_ID_TEMOTI_POKE,
  PT_GMDATA_ID_TEMOTI_ITEM,
  PT_GMDATA_ID_EVENT_WORK,
  PT_GMDATA_ID_POKETCH_DATA,
  PT_GMDATA_ID_SITUATION,
  PT_GMDATA_ID_ZUKANWORK,
  PT_GMDATA_ID_SODATEYA,
  PT_GMDATA_ID_FRIEND,
  PT_GMDATA_ID_MISC,
  PT_GMDATA_ID_FIELDOBJSV,
  PT_GMDATA_ID_UNDERGROUNDDATA,
  PT_GMDATA_ID_REGULATION,
  PT_GMDATA_ID_IMAGECLIPDATA,
  PT_GMDATA_ID_MAILDATA,
  PT_GMDATA_ID_PORUTODATA,
  PT_GMDATA_ID_RANDOMGROUP,
  PT_GMDATA_ID_FNOTE,
  PT_GMDATA_ID_TRCARD,
  PT_GMDATA_ID_RECORD,
  PT_GMDATA_ID_CUSTOM_BALL,
  PT_GMDATA_ID_PERAPVOICE,
  PT_GMDATA_ID_FRONTIER,
  PT_GMDATA_ID_SP_RIBBON,
  PT_GMDATA_ID_ENCOUNT,
  PT_GMDATA_ID_WORLDTRADEDATA,
  PT_GMDATA_ID_TVWORK,
  PT_GMDATA_ID_GUINNESS,
  PT_GMDATA_ID_WIFILIST,
  PT_GMDATA_ID_WIFIHISTORY,
  PT_GMDATA_ID_FUSHIGIDATA,
  PT_GMDATA_ID_POKEPARKDATA,
  PT_GMDATA_ID_CONTEST,
  PT_GMDATA_ID_PMS,
  PT_GMDATA_ID_EMAIL,   //PT�̂�
  PT_GMDATA_ID_WFHIROBA,    //PT�̂�

  PT_GMDATA_NORMAL_FOOTER,  //WB�Œǉ��B�Z�[�u��CRC�Ƃ��J�E���^�Ƃ������Ă�
  //�{�b�N�X�f�[�^�O���[�v
  PT_GMDATA_ID_BOXDATA,

  PT_GMDATA_BOX_FOOTER,   //WB�Œǉ��B�{�b�N�X��CRC�Ƃ��J�E���^�Ƃ������Ă�

  PT_GMDATA_ID_MAX,
}PT_GMDATA_ID;
//=============================================================================
//=============================================================================

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define MAGIC_NUMBER_PT (0x20060623)
#define PT_SECTOR_SIZE   (PT_SAVE_SECTOR_SIZE)
#define PT_SECTOR_MAX    (PT_SAVE_PAGE_MAX)

#define FIRST_MIRROR_START  (0)
#define SECOND_MIRROR_START (64)

#define MIRROR1ST (0)
#define MIRROR2ND (1)
#define MIRRORERROR (2)

#define HEAPID_SAVE_TEMP  (HEAPID_BASE_APP)

#define SEC_DATA_SIZE   PT_SECTOR_SIZE

#define PT_SAVE_SIZE (PT_SECTOR_SIZE * PT_SECTOR_MAX)


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef struct {
  u32 g_count;    ///<�O���[�o���J�E���^�iMY�f�[�^�ABOX�f�[�^���L�j
  u32 b_count;    ///<�u���b�N���J�E���^�iMY�f�[�^��BOX�f�[�^�ƂœƗ��j
  u32 size;     ///<�f�[�^�T�C�Y�i�t�b�^�T�C�Y�܂ށj
  u32 magic_number; ///<�}�W�b�N�i���o�[
  u8 blk_id;      ///<�Ώۂ̃u���b�N�w��ID
  u16 crc;      ///<�f�[�^�S�̂�CRC�l
}PT_SAVE_FOOTER;

//---------------------------------------------------------------------------
/**
 * @brief �Z�[�u���[�N�\����
 *
 * ���ۂ̃Z�[�u����镔���̍\��
 */
//---------------------------------------------------------------------------
/*
typedef struct {
  u8 data[PT_SECTOR_SIZE * PT_SECTOR_MAX];  ///<���ۂ̃f�[�^�ێ��̈�
}SAVEWORK;
*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define PT_BOX_MAX_TRAY      (18)
#define PT_BOX_MAX_POS       (5*6)
#define PT_BOX_TRAYNAME_BUFSIZE  (20)  // ���{��W�����{EOM�B�C�O�ŗp�̗]�T�����Ă��̒��x�B

typedef struct pt_box_data PT_BOX_DATA;
typedef struct
{
  u32 dummy[0x88/4];
}PT_POKEMON_PARAM;


struct pt_box_data
{
  u32         currentTrayNumber;
  PT_POKEMON_PARAM  ppp[PT_BOX_MAX_TRAY][PT_BOX_MAX_POS];
  STRCODE       trayName[PT_BOX_MAX_TRAY][PT_BOX_TRAYNAME_BUFSIZE];
  u8          wallPaper[PT_BOX_MAX_TRAY];
  u8          daisukiBitFlag;
};

