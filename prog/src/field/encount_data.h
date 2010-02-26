/*
 *  @file encount_data.h
 *  @brief  �t�B�[���h�@�G���J�E���g�f�[�^�e�[�u���\����
 *  @author Miyuki Iwasawa
 *  @date 09.09.25
 */

#ifndef __H_ENCOUNT_DATA_H__
#define __H_ENCOUNT_DATA_H__

#define ENC_DATA_INVALID_ID (0xFFFF)  ///<�����G���J�E�g�f�[�^ID

#define ENC_MONS_NUM_GROUND_L   (12)   ///<�ʏ�G���J�E���g(���[�N���X)
#define ENC_MONS_NUM_GROUND_H   (12)   ///<�ʏ�G���J�E���g�Q(�n�C�N���X)
#define ENC_MONS_NUM_GROUND_SP  (12)   ///<����G���J�E���g

#define ENC_MONS_NUM_WATER         (5) ///<����G���J�E���g
#define ENC_MONS_NUM_WATER_SP      (5) ///<���ꐅ��G���J�E���g
#define ENC_MONS_NUM_FISHING       (5) ///<�ނ�G���J�E���g
#define ENC_MONS_NUM_FISHING_SP    (5) ///<����ނ�G���J�E���g
#define ENC_MONS_NUM_BINGO        (15)  ///<�r���S�G���J�E���g�����X�^�[��

#define ENC_MONS_NUM_MAX          (ENC_MONS_NUM_BINGO)  ///<����r���S���ő�

#define ENC_MONS_FORM_RND_CODE    (0x1F)  //�t�H�����������_���Ō��߂����R�[�h

///�G���J�E���g���P�[�V����ID
typedef enum{
 ENC_LOCATION_GROUND_L,
 ENC_LOCATION_GROUND_H,
 ENC_LOCATION_GROUND_SP,
 ENC_LOCATION_WATER,
 ENC_LOCATION_WATER_SP,
 ENC_LOCATION_FISHING,
 ENC_LOCATION_FISHING_SP,
 ENC_LOCATION_MAX,
 ENC_LOCATION_NONE = 0xFF,  //����ID
}ENCOUNT_LOCATION;

///�G���J�E���g�^�C�v
typedef enum{
 ENC_TYPE_NORMAL,   ///<�ʏ�G���J�E���g
 ENC_TYPE_EFFECT,   ///<�G�t�F�N�g�G���J�E���g
 ENC_TYPE_FORCE,    ///<�����G���J�E���g(�Â�����/�Â���)
 ENC_TYPE_WFBC,    ///<WFBC�G���J�E���g(�����_���}�b�v)
 ENC_TYPE_FISHING,  //�ނ�
 ENC_TYPE_DEMO,  //�f���@���������Ƃ��ēn�����߂����Ɏg�p���Ă��܂��B
 ENC_TYPE_MAX,
}ENCOUNT_TYPE;

///�G���J�E���g�����X�^�[�e�[�u���f�[�^
typedef struct _ENC_COMMON_DAT{
  u16 monsNo:11;
  u16 form:5;
  u8  minLevel;
  u8  maxLevel;
}ENC_COMMON_DATA;

typedef struct _ENC_MONS_DATA{
  u16 monsNo;
  u8  level;
  u8  form;
}ENC_MONS_DATA;

typedef struct _ENCOUNT_DATA{
  union{
    struct{
      u8  probGroundLow;   ///<�n�ʃ��[�N���X�G���J�E���g��
      u8  probGroundHigh;  ///<�n�ʃn�C�N���X�G���J�E���g��
      u8  probGroundSp;    ///<�n�ʓ���G���J�E���g��(0or1)
      u8  probWater;     ///<����G���J�E�g��
      u8  probWaterSp;   ///<�������G���J�E���g��(0or1)
      u8  probFishing;   ///<�ނ�G���J�E���g��
      u8  probFishingSp; ///<�ނ����G���J�E���g��(0or1)
      u8  itemTblIdx:7;   ///<�t�B�[���h�G�t�F�N�g�A�C�e���e�[�u���Q��idx
      u8  enable_f:1;     ///<�L�������t���O
    };
    u8 prob[ENC_LOCATION_MAX+1];
  };

  ENC_COMMON_DATA  groundLmons[ENC_MONS_NUM_GROUND_L];
  ENC_COMMON_DATA  groundHmons[ENC_MONS_NUM_GROUND_H];
  ENC_COMMON_DATA  groundSmons[ENC_MONS_NUM_GROUND_SP];
  
  ENC_COMMON_DATA  waterMons[ENC_MONS_NUM_WATER];
  ENC_COMMON_DATA  waterSpMons[ENC_MONS_NUM_WATER_SP];
  ENC_COMMON_DATA  fishingMons[ENC_MONS_NUM_FISHING];
  ENC_COMMON_DATA  fishingSpMons[ENC_MONS_NUM_FISHING_SP];
}ENCOUNT_DATA;

typedef struct _ENCOUNT_TABLE{
  ENCOUNT_DATA  enc_tbl[4];
}ENCOUNT_TABLE;

#endif  //__H_ENCOUNT_DATA_H__
