//============================================================================================
/**
 * @file  record.h
 * @brief
 * @date  2006.03.28
 * @author  tamada/iwasawa
 */
//============================================================================================

#ifndef __RECORD_H__
#define __RECORD_H__

#ifndef __ASM_NO_DEF_

#include "savedata/save_control.h"
#include "score_def.h"

//============================================================================================
//
//  RECORD�𑀍삷��̂ɕK�v�Ȓ�`
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief RECORD�ւ̕s���S�^��`
 */
//----------------------------------------------------------
typedef struct RECORD RECORD;

//----------------------------------------------------------
//----------------------------------------------------------
//���R�[�h�f�[�^�̎擾
extern RECORD * SaveData_GetRecord(SAVE_CONTROL_WORK * sv);

extern void RECORD_Init(RECORD * rec);
extern int RECORD_GetWorkSize(void);

//----------------------------------------------------------
//----------------------------------------------------------
//�l���Z�b�g����
extern u32 RECORD_Set(RECORD * rec, int id, u32 value);
//���݂̒l�Ɣ�r�A�傫����Βl���Z�b�g����
extern u32 RECORD_SetIfLarge(RECORD * rec, int id, u32 value);
//�l���C���N�������g����
extern u32 RECORD_Inc(RECORD * rec, int id);
//�l���擾����
extern u32 RECORD_Get(RECORD * rec, int id);
//�l�𑫂�
extern u32 RECORD_Add(RECORD * rec, int id, u32 add_value);

extern void RECORD_SetThScore(RECORD * rec, u32 inScore);
extern u32  RECORD_GetThScore(RECORD * rec);
extern void RECORD_SetThRank(RECORD * rec, u8 inRank);
extern u8   RECORD_GetThRank(RECORD * rec);
extern void RECORD_1day_Update( RECORD *rec );


#ifdef PM_DEBUG
//�X�R�A�̃Z�b�g
extern void RECORD_Score_DebugSet(RECORD * rec, const u32 inScore);
extern void RECORD_DebugSet( RECORD *rec );
#endif


#endif  //__ASM_NO_DEF_


//============================================================================================
//
//  RECORD�e���ڂ̒�`
//
//============================================================================================
//----------------------------------------------------------
//----------------------------------------------------------
#define REC_FIRST   0

//�Í�������O���ׁA�����J�E���^��擪�Ɏ����Ă���
#define RECID_WALK_COUNT        (REC_FIRST) ///< @����

/**
 *  @brief  �X�R�A�擾�w��ID(4byte)
 *
 *  �l�̉��Z�ɂ͕K��RECORD_Score_Add()�֐�
 *  �l�̎Q�Ƃɂ͕K��RECORD_Score_Get()�֐����g�p���邱��
 *
 *  RECORD_Add()/RECORD_Get()�ɒ��ڎw�肵�Ēl���X�V/�Q�Ƃ��邱�Ƃ͋֎~�ł�
*/

#define LARGE_REC_START   1

//�ȉ��́A4�o�C�g�̃J�E���^������
#define RECID_REPORT_COUNT      (LARGE_REC_START+ 0)   ///< @���|�[�g�� 1
#define RECID_CLEAR_TIME        (LARGE_REC_START+ 1)   ///< @�N���A�^�C��
#define RECID_RIDE_CYCLE        (LARGE_REC_START+ 2)   ///< @���]�Ԃɏ������
#define RECID_BATTLE_COUNT      (LARGE_REC_START+ 3)   ///< @�S�퓬��
#define RECID_BTL_ENCOUNT       (LARGE_REC_START+ 4)   ///< @�G���J�E���g�퓬��
#define RECID_BTL_TRAINER       (LARGE_REC_START+ 5)   ///< @�g���[�i�[�퓬��
#define RECID_CAPTURE_POKE      (LARGE_REC_START+ 6)   ///< @�ߊl�����|�P�����̐�
#define RECID_FISHING_SUCCESS   (LARGE_REC_START+ 7)   ///< @�ނ肠�����|�P�����̉�
#define RECID_TAMAGO_HATCHING   (LARGE_REC_START+ 8)  ///< @�^�}�S��z������
#define RECID_POKE_EVOLUTION    (LARGE_REC_START+ 9)  ///< @�i���������|�P�����̈��� 10
#define RECID_RECOVERY_PC       (LARGE_REC_START+10)  ///< @�|�P�Z���ŋx�񂾉�
#define RECID_COMM_TRADE        (LARGE_REC_START+11)  ///< @���C�����X�ʐM������������
#define RECID_COMM_BATTLE       (LARGE_REC_START+12)  ///< @���C�����X�ʐM�ΐ��������
#define RECID_COMM_BTL_WIN      (LARGE_REC_START+13)  ///< @���C�����X�ʐM�ΐ�̏�����
#define RECID_COMM_BTL_LOSE     (LARGE_REC_START+14)  ///< @���C�����X�ʐM�ΐ�̔s�k��
#define RECID_WIFI_TRADE        (LARGE_REC_START+15)  ///< @WiFi�ʐM������������
#define RECID_WIFI_BATTLE       (LARGE_REC_START+16)  ///< @WiFi�ʐM�ΐ��������
#define RECID_WIFI_BTL_WIN      (LARGE_REC_START+17)  ///< @WiFi�ʐM�ΐ�̏�����
#define RECID_WIFI_BTL_LOSE     (LARGE_REC_START+18)  ///< @WiFi�ʐM�ΐ�̔s�k��
#define RECID_WIFI_BTL_DRAW     (LARGE_REC_START+19)  ///< @WiFi�ʐM�ΐ�̈��������� 20
#define RECID_SHOPPING_CNT      (LARGE_REC_START+20)  ///< @��������������
#define RECID_SHOPPING_MONEY    (LARGE_REC_START+21)  ///< @���������������v���z
#define RECID_WATCH_TV          (LARGE_REC_START+22)  ///< @�e���r��������
#define RECID_SODATEYA_CNT      (LARGE_REC_START+23)  ///< @��ĉ��Ƀ|�P������a������
#define RECID_KILL_POKE         (LARGE_REC_START+24)  ///< @�|�����|�P�����̐�
#define RECID_DAYMAX_EXP        (LARGE_REC_START+25)  ///< @1���ɉ҂����o���l�ő�l
#define RECID_DAYCNT_EXP        (LARGE_REC_START+26)  ///< @1���ɉ҂����o���l
#define RECID_GTS_PUT           (LARGE_REC_START+27)  ///< @GTS�Ƀ|�P������a������
#define RECID_MAIL_WRITE        (LARGE_REC_START+28)  ///< @���[������������
#define RECID_NICKNAME          (LARGE_REC_START+29)  ///< @�|�P�����Ƀj�b�N�l�[����t������ 30
#define RECID_PREMIUM_BALL      (LARGE_REC_START+30)  ///< @��������v���~�A�{�[���̐�
#define RECID_LEADERHOUSE_BATTLE  (LARGE_REC_START+31)  ///< @���[�_�[�n�E�X�Ő������
#define RECID_WIN_BP            (LARGE_REC_START+32)  ///< @���������BP
#define RECID_USE_BP            (LARGE_REC_START+33)  ///< @�g����BP
#define RECID_SURECHIGAI_NUM    (LARGE_REC_START+34)  ///< @���ꂿ�����ʐM�������񐔁����g�p
#define RECID_IRC_TRADE         (LARGE_REC_START+35)  ///< @�ԊO���ʐM������������
#define RECID_IRC_BATTLE        (LARGE_REC_START+36)  ///< @�ԊO���ʐM�ΐ��������
#define RECID_IRC_BTL_WIN       (LARGE_REC_START+37)  ///< @�ԊO���ʐM�ΐ�̏�����
#define RECID_IRC_BTL_LOSE      (LARGE_REC_START+38)  ///< @�ԊO���ʐM�ΐ�̔s�k��
#define RECID_PALACE_TIME       (LARGE_REC_START+39)  ///< @�p���X�K�⎞�ԍ��v 40(���g�p�j
#define RECID_PALACE_MISSION     (LARGE_REC_START+40) ///< @�p���X�N���A�����~�b�V�����̐�(���g�p�j

#define RECID_LARGE_42   ( LARGE_REC_START+41 )   ///< 4�o�C�g���R�[�h ���g�p�̈�̒�`
#define RECID_LARGE_43   ( LARGE_REC_START+42 )   ///< �����p�Ɂc
#define RECID_LARGE_44   ( LARGE_REC_START+43 )
#define RECID_LARGE_45   ( LARGE_REC_START+44 )
#define RECID_LARGE_46   ( LARGE_REC_START+45 )
#define RECID_LARGE_47   ( LARGE_REC_START+46 )
#define RECID_LARGE_48   ( LARGE_REC_START+47 )
#define RECID_LARGE_49   ( LARGE_REC_START+48 )
#define RECID_LARGE_50   ( LARGE_REC_START+49 )
#define RECID_LARGE_51   ( LARGE_REC_START+50 )
#define RECID_LARGE_52   ( LARGE_REC_START+51 )
#define RECID_LARGE_53   ( LARGE_REC_START+52 )
#define RECID_LARGE_54   ( LARGE_REC_START+53 )
#define RECID_LARGE_55   ( LARGE_REC_START+54 )
#define RECID_LARGE_56   ( LARGE_REC_START+55 )
#define RECID_LARGE_57   ( LARGE_REC_START+56 )
#define RECID_LARGE_58   ( LARGE_REC_START+57 )
#define RECID_LARGE_59   ( LARGE_REC_START+58 )
#define RECID_LARGE_60   ( LARGE_REC_START+59 )
#define RECID_LARGE_61   ( LARGE_REC_START+60 )
#define RECID_LARGE_62   ( LARGE_REC_START+61 )
#define RECID_LARGE_63   ( LARGE_REC_START+62 )
#define RECID_LARGE_64   ( LARGE_REC_START+63 )
#define RECID_LARGE_65   ( LARGE_REC_START+64 )
#define RECID_LARGE_66   ( LARGE_REC_START+65 )
#define RECID_LARGE_67   ( LARGE_REC_START+66 )
#define RECID_LARGE_68   ( LARGE_REC_START+67 )
#define RECID_LARGE_69   ( LARGE_REC_START+68 )
#define RECID_LARGE_70   ( LARGE_REC_START+69 )

#define LARGE_REC_NUM    ( LARGE_REC_START+70 )  ///< WB��41+1�g�p�B�c��29��
#define LARGE_REC_MAX    ( LARGE_REC_NUM ) 

//�ȉ��́A2�o�C�g�����J�E���^�������Ȃ�
#define SMALL_REC_START   LARGE_REC_MAX

#define RECID_DENDOU_CNT          (SMALL_REC_START+ 0) ///<@�a�����肵����  71
#define RECID_RECOVERY_HOME       (SMALL_REC_START+ 1) ///<@�����̉Ƃŋx�񂾉�
#define RECID_WAZA_HANERU         (SMALL_REC_START+ 2) ///< @�u�͂˂�v���g������
#define RECID_WAZA_WARUAGAKI      (SMALL_REC_START+ 3) ///< @�u��邠�����v���g������
#define RECID_WAZA_MUKOU          (SMALL_REC_START+ 4)  ///< @�����ʂ킴���o������
#define RECID_TEMOTI_KIZETU       (SMALL_REC_START+ 5)  ///< @�C�₳�����莝���̈���
#define RECID_TEMOTI_MAKIZOE      (SMALL_REC_START+ 6)  ///< @�����Ƀ_���[�W��^������
#define RECID_NIGERU_SIPPAI       (SMALL_REC_START+ 7)  ///< @���������Ȃ����� 
#define RECID_NIGERARETA          (SMALL_REC_START+ 8)  ///< @������ꂽ�|�P�����̐�
#define RECID_FISHING_FAILURE     (SMALL_REC_START+ 9)  ///< @�葹�Ȃ����� 80
#define RECID_DAYMAX_KILL         (SMALL_REC_START+10)  ///< @1���ɓ|�����|�P�����̍ō���
#define RECID_DAYCNT_KILL         (SMALL_REC_START+11)  ///< @1���ɓ|�����|�P�����̐�
#define RECID_DAYMAX_CAPTURE      (SMALL_REC_START+12)  ///< @1���ɕߊl�����|�P�����̍ō���
#define RECID_DAYCNT_CAPTURE      (SMALL_REC_START+13)  ///< @1���ɕߊl�����|�P�����̐�
#define RECID_DAYMAX_TRAINER_BATTLE   (SMALL_REC_START+14)  ///< @1���Ƀg���[�i�[�킵���ō���
#define RECID_DAYCNT_TRAINER_BATTLE   (SMALL_REC_START+15)  ///< @1���Ƀg���[�i�[�킵����
#define RECID_DAYMAX_EVOLUTION    (SMALL_REC_START+16)  ///< @1���Ƀ|�P�����i���������ō���
#define RECID_DAYCNT_EVOLUTION    (SMALL_REC_START+17)  ///< @1���Ƀ|�P�����i����������
#define RECID_FOSSIL_RESTORE      (SMALL_REC_START+18)  ///< @�|�P�����̉��΂𕜌�������
#define RECID_GURUGURU_COUNT      (SMALL_REC_START+19)  ///< @���邮������������� 90
#define RECID_RANDOM_FREE_SINGLE_WIN   (SMALL_REC_START+20)  ///< ��(���g�p�j
#define RECID_RANDOM_FREE_SINGLE_LOSE  (SMALL_REC_START+21)  ///< �����_���}�b�`����̏��s�J�E���g��(���g�p�j
#define RECID_RANDOM_FREE_DOUBLE_WIN   (SMALL_REC_START+22)  ///< ���肾�������ʂ̏ꏊ��(���g�p�j
#define RECID_RANDOM_FREE_DOUBLE_LOSE  (SMALL_REC_START+23)  ///< �m�ۂ���Ă����̂Ŗ��g�p(���g�p�j
#define RECID_RANDOM_FREE_ROTATE_WIN   (SMALL_REC_START+24)  ///< (���g�p�j
#define RECID_RANDOM_FREE_ROTATE_LOSE  (SMALL_REC_START+25)  ///< (���g�p�j
#define RECID_RANDOM_FREE_TRIPLE_WIN   (SMALL_REC_START+26)  ///< (���g�p�j
#define RECID_RANDOM_FREE_TRIPLE_LOSE  (SMALL_REC_START+27)  ///< (���g�p�j
#define RECID_RANDOM_FREE_SHOOTER_WIN  (SMALL_REC_START+28)  ///< (���g�p�j
#define RECID_RANDOM_FREE_SHOOTER_LOSE (SMALL_REC_START+29)  ///< (���g�p�j
#define RECID_RANDOM_RATE_SINGLE_WIN   (SMALL_REC_START+30)  ///< (���g�p�j
#define RECID_RANDOM_RATE_SINGLE_LOSE  (SMALL_REC_START+31)  ///< (���g�p�j
#define RECID_RANDOM_RATE_DOUBLE_WIN   (SMALL_REC_START+32)  ///< (���g�p�j
#define RECID_RANDOM_RATE_DOUBLE_LOSE  (SMALL_REC_START+33)  ///< (���g�p�j
#define RECID_RANDOM_RATE_ROTATE_WIN   (SMALL_REC_START+34)  ///< (���g�p�j
#define RECID_RANDOM_RATE_ROTATE_LOSE  (SMALL_REC_START+35)  ///< (���g�p�j
#define RECID_RANDOM_RATE_TRIPLE_WIN   (SMALL_REC_START+36)  ///< (���g�p�j
#define RECID_RANDOM_RATE_TRIPLE_LOSE  (SMALL_REC_START+37)  ///< (���g�p�j
#define RECID_RANDOM_RATE_SHOOTER_WIN  (SMALL_REC_START+38)  ///< (���g�p�j
#define RECID_RANDOM_RATE_SHOOTER_LOSE (SMALL_REC_START+39)  ///< ��(���g�p�j
#define RECID_USE_SHOOTER_COUNT        (SMALL_REC_START+40)  ///< @�V���[�^�[�g�p��
#define RECID_EFFECT_ENCOUNT_NUM       (SMALL_REC_START+41)  ///< @�G�t�F�N�g�G���J�E���g��������
#define RECID_AFFINITY_CHECK_NUM       (SMALL_REC_START+42)  ///< @�����`�F�b�N��������
#define RECID_MUSICAL_PLAY_NUM         (SMALL_REC_START+43)  ///< @�~���[�W�J���Q����
#define RECID_MUSICAL_WINNER_NUM       (SMALL_REC_START+44)  ///< @�~���[�W�J��������
#define RECID_MUSICAL_COMM_NUM         (SMALL_REC_START+45)  ///< @�~���[�W�J���ʐM�Q����
#define RECID_MUSICAL_COMM_WINNER_NUM  (SMALL_REC_START+46)  ///< @�~���[�W�J���ʐM������
#define RECID_MUSICAL_TOTAL_POINT      (SMALL_REC_START+47)  ///< @�~���[�W�J���ʎZ�擾���_
#define RECID_PDW_SLEEP_POKEMON        (SMALL_REC_START+48)  ///< @PDW�Ń|�P������Q��������
#define RECID_POKESHIFTER_COUNT        (SMALL_REC_START+49)  ///< @�|�P�V�t�^�[�������� 120
#define RECID_TRIALHOUSE_COUNT         (SMALL_REC_START+50)  ///< @�g���C�A���n�E�X��������
#define RECID_SURECHIGAI_THANKS        (SMALL_REC_START+51)  ///< @���ꂿ�����ʐM�ł�����󂯂��񐔁����g�p
#define RECID_TRIALHOUSE_RANK          (SMALL_REC_START+52)  ///< @�g���C�A���n�E�X�ō������N
#define RECID_TRIALHOUSE_SCORE         (SMALL_REC_START+53)  ///< @�g���C�A���n�E�X�ō��X�R�A 124

#define RECID_SMALL_54                 (SMALL_REC_START+54)  ///< 2�o�C�g���R�[�h�@���g�p�̈��`
#define RECID_SMALL_55                 (SMALL_REC_START+55)  ///< �����p
#define RECID_SMALL_56                 (SMALL_REC_START+56)
#define RECID_SMALL_57                 (SMALL_REC_START+57)
#define RECID_SMALL_58                 (SMALL_REC_START+58)
#define RECID_SMALL_59                 (SMALL_REC_START+59)
#define RECID_SMALL_60                 (SMALL_REC_START+60)
#define RECID_SMALL_61                 (SMALL_REC_START+61)
#define RECID_SMALL_62                 (SMALL_REC_START+62)
#define RECID_SMALL_63                 (SMALL_REC_START+63)
#define RECID_SMALL_64                 (SMALL_REC_START+64)
#define RECID_SMALL_65                 (SMALL_REC_START+65)
#define RECID_SMALL_66                 (SMALL_REC_START+66)
#define RECID_SMALL_67                 (SMALL_REC_START+67)
#define RECID_SMALL_68                 (SMALL_REC_START+68)
#define RECID_SMALL_69                 (SMALL_REC_START+69)
#define RECID_SMALL_70                 (SMALL_REC_START+70)
#define RECID_SMALL_71                 (SMALL_REC_START+71)
#define RECID_SMALL_72                 (SMALL_REC_START+72)
#define RECID_SMALL_73                 (SMALL_REC_START+73)
#define RECID_SMALL_74                 (SMALL_REC_START+74)
#define RECID_SMALL_75                 (SMALL_REC_START+75)
#define RECID_SMALL_76                 (SMALL_REC_START+76)
#define RECID_SMALL_77                 (SMALL_REC_START+77)
#define RECID_SMALL_78                 (SMALL_REC_START+78)
#define RECID_SMALL_79                 (SMALL_REC_START+79)
#define RECID_SMALL_80                 (SMALL_REC_START+80)
#define RECID_SMALL_81                 (SMALL_REC_START+81)
#define RECID_SMALL_82                 (SMALL_REC_START+82)
#define RECID_SMALL_83                 (SMALL_REC_START+83)
#define RECID_SMALL_84                 (SMALL_REC_START+84)
#define RECID_SMALL_85                 (SMALL_REC_START+85)
#define RECID_SMALL_86                 (SMALL_REC_START+86)
#define RECID_SMALL_87                 (SMALL_REC_START+87)
#define RECID_SMALL_88                 (SMALL_REC_START+88)
#define RECID_SMALL_89                 (SMALL_REC_START+89)
#define RECID_SMALL_90                 (SMALL_REC_START+90)
#define RECID_SMALL_91                 (SMALL_REC_START+91)
#define RECID_SMALL_92                 (SMALL_REC_START+92)
#define RECID_SMALL_93                 (SMALL_REC_START+93)
#define RECID_SMALL_94                 (SMALL_REC_START+94)
#define RECID_SMALL_95                 (SMALL_REC_START+95)


#define SMALL_REC_NUM                   (96)  // 54+42 42�ӏ����]��    
#define SMALL_REC_MAX                   (SMALL_REC_NUM+LARGE_REC_MAX)




// --------------------------------------------------------------------------------------
// MAX�m�F�p�t���O��`
// 0: 999,999,999
// 1:     999,999
// 2:      65,536
// 3:       9,999
// 4:         999
// 5:           7
// --------------------------------------------------------------------------------------

#define RECID_WALK_COUNT_FLAG       ( 0 )  ///< ����������  999,999,999
#define RECID_REPORT_COUNT_FLAG     ( 0 )  ///< ���|�[�g�� 999,999,999
#define RECID_CLEAR_TIME_FLAG       ( 1 )  ///< �N���A�^�C�� 999,59,59
#define RECID_RIDE_CYCLE_FLAG       ( 0 )  ///< ���]�Ԃɏ������ 999,999,999
#define RECID_BATTLE_COUNT_FLAG     ( 0 )  ///< �S�퓬�� 999,999,999
#define RECID_BTL_ENCOUNT_FLAG      ( 0 )  ///< �G���J�E���g�퓬�� 999,999,999
#define RECID_BTL_TRAINER_FLAG      ( 0 )  ///< �g���[�i�[�퓬�� 999,999,999
#define RECID_CAPTURE_POKE_FLAG     ( 1 )  ///< �ߊl�����|�P�����̐� 999,999
#define RECID_FISHING_SUCCESS_FLAG  ( 1 )  ///< �ނ肠�����|�P�����̉� 999,999
#define RECID_TAMAGO_HATCHING_FLAG  ( 1 )  ///< �^�}�S��z������ 999,999
#define RECID_POKE_EVOLUTION_FLAG   ( 0 )  ///< �i���������|�P�����̈���  999,999,999
#define RECID_RECOVERY_PC_FLAG      ( 0 )  ///< �|�P�Z���ŋx�񂾉�  999,999,999
#define RECID_COMM_TRADE_FLAG       ( 0 )  ///< ���C�����X�ʐM������������  999,999,999
#define RECID_COMM_BATTLE_FLAG      ( 0 )  ///< ���C�����X�ʐM�ΐ��������  999,999,999
#define RECID_COMM_BTL_WIN_FLAG     ( 0 )  ///< ���C�����X�ʐM�ΐ�̏����� 999,999,999
#define RECID_COMM_BTL_LOSE_FLAG    ( 0 )  ///< ���C�����X�ʐM�ΐ�̔s�k�� 999,999,999
#define RECID_WIFI_TRADE_FLAG       ( 0 )  ///< WiFi�ʐM������������ 999,999,999
#define RECID_WIFI_BATTLE_FLAG      ( 0 )  ///< WiFi�ʐM�ΐ�������� 999,999,999
#define RECID_WIFI_BTL_WIN_FLAG     ( 0 )  ///< WiFi�ʐM�ΐ�̏����� 999,999,999
#define RECID_WIFI_BTL_LOSE_FLAG    ( 0 )  ///< WiFi�ʐM�ΐ�̔s�k�� 999,999,999
#define RECID_WIFI_BTL_DRAW_FLAG    ( 0 )  ///< WiFi�ʐM�ΐ�̈��������� 999,999,999
#define RECID_SHOPPING_CNT_FLAG     ( 0 )  ///< �������������� 999,999,999
#define RECID_SHOPPING_MONEY_FLAG   ( 0 )  ///< ���������������v���z 999,999,999
#define RECID_WATCH_TV_FLAG         ( 0 )  ///< �e���r�������� 999,999,999
#define RECID_SODATEYA_CNT_FLAG     ( 0 )  ///< ��ĉ��Ƀ|�P������a������ 999,999,999
#define RECID_KILL_POKE_FLAG        ( 0 )  ///< �|�����|�P�����̐� 999,999,999
#define RECID_DAYMAX_EXP_FLAG       ( 0 )  ///< 1���ɉ҂����o���l�ő�l 999,999,999
#define RECID_DAYCNT_EXP_FLAG       ( 0 )  ///< 1���ɉ҂����o���l 999,999,999
#define RECID_GTS_PUT_FLAG          ( 1 )  ///< GTS�Ƀ|�P������a������ 999,999
#define RECID_MAIL_WRITE_FLAG       ( 1 )  ///< ���[������������ 999,999
#define RECID_NICKNAME_FLAG         ( 1 )  ///< �|�P�����Ƀj�b�N�l�[����t������ 999,999
#define RECID_PREMIUM_BALL_FLAG     ( 1 )  ///< ��������v���~�A�{�[���̐� 999,999
#define RECID_LEADERHOUSE_BATTLE_FLAG ( 0 )  ///< ���[�_�[�n�E�X�Ő������  999,999,999
#define RECID_WIN_BP_FLAG           ( 0 )  ///< ���������BP 999,999,999
#define RECID_USE_BP_FLAG           ( 0 )  ///< �g����BP 999,999,999
#define RECID_SURECHIGAI_NUM_FLAG   ( 0 )  ///< ���ꂿ�����ʐM�������� 99,999�����g�p
#define RECID_IRC_TRADE_FLAG        ( 0 )  ///< �ԊO���ʐM������������ 999,999,999
#define RECID_IRC_BATTLE_FLAG       ( 0 )  ///< �ԊO���ʐM�ΐ�������� 999,999,999
#define RECID_IRC_BTL_WIN_FLAG      ( 0 )  ///< �ԊO���ʐM�ΐ�̏����� 999,999,999
#define RECID_IRC_BTL_LOSE_FLAG     ( 0 )  ///< �ԊO���ʐM�ΐ�̔s�k�� 999,999,999
#define RECID_PALACE_TIME_FLAG      ( 1 )  ///< �p���X�K�⎞�ԍ��v 999,59,59
#define RECID_PALACE_MISSION_FLAG   ( 1 )  ///< �p���X�N���A�����~�b�V�����̐� 999,59,59

#define RECID_LARGE_42_FLAG   ( 1 )   ///< 4�o�C�g���R�[�h ���g�p�̈��MAX��`
#define RECID_LARGE_43_FLAG   ( 1 )   ///< 0 origin
#define RECID_LARGE_44_FLAG   ( 1 )
#define RECID_LARGE_45_FLAG   ( 1 )
#define RECID_LARGE_46_FLAG   ( 1 )
#define RECID_LARGE_47_FLAG   ( 1 )
#define RECID_LARGE_48_FLAG   ( 1 )
#define RECID_LARGE_49_FLAG   ( 1 )
#define RECID_LARGE_50_FLAG   ( 1 )
#define RECID_LARGE_51_FLAG   ( 1 )
#define RECID_LARGE_52_FLAG   ( 1 )
#define RECID_LARGE_53_FLAG   ( 1 )
#define RECID_LARGE_54_FLAG   ( 1 )
#define RECID_LARGE_55_FLAG   ( 1 )
#define RECID_LARGE_56_FLAG   ( 1 )
#define RECID_LARGE_57_FLAG   ( 1 )
#define RECID_LARGE_58_FLAG   ( 1 )
#define RECID_LARGE_59_FLAG   ( 1 )
#define RECID_LARGE_60_FLAG   ( 1 )
#define RECID_LARGE_61_FLAG   ( 1 )
#define RECID_LARGE_62_FLAG   ( 1 )
#define RECID_LARGE_63_FLAG   ( 1 )
#define RECID_LARGE_64_FLAG   ( 1 )
#define RECID_LARGE_65_FLAG   ( 1 )
#define RECID_LARGE_66_FLAG   ( 1 )
#define RECID_LARGE_67_FLAG   ( 1 )
#define RECID_LARGE_68_FLAG   ( 1 )
#define RECID_LARGE_69_FLAG   ( 1 )
#define RECID_LARGE_70_FLAG   ( 1 )


#define RECID_DENDOU_CNT_FLAG         ( 3 ) ///<�a�����肵����          
#define RECID_RECOVERY_HOME_FLAG      ( 3 ) ///<�����̉Ƃŋx�񂾉�
#define RECID_WAZA_HANERU_FLAG        ( 3 ) ///< �u�͂˂�v���g������
#define RECID_WAZA_WARUAGAKI_FLAG     ( 3 ) ///< �u��邠�����v���g������
#define RECID_WAZA_MUKOU_FLAG         ( 3 )  ///< �����ʂ킴���o������
#define RECID_TEMOTI_KIZETU_FLAG      ( 3 )  ///< �C�₳�����莝���̈���
#define RECID_TEMOTI_MAKIZOE_FLAG     ( 3 )  ///< �����Ƀ_���[�W��^������
#define RECID_NIGERU_SIPPAI_FLAG      ( 3 )  ///< ���������Ȃ�����
#define RECID_NIGERARETA_FLAG         ( 3 )  ///< ������ꂽ�|�P�����̐�  
#define RECID_FISHING_FAILURE_FLAG    ( 3 )  ///< �葹�Ȃ�����        80
#define RECID_DAYMAX_KILL_FLAG        ( 3 )  ///< 1���ɓ|�����|�P�����̍ō���
#define RECID_DAYCNT_KILL_FLAG        ( 3 )  ///< 1���ɓ|�����|�P�����̐�
#define RECID_DAYMAX_CAPTURE_FLAG     ( 3 )  ///< 1���ɕߊl�����|�P�����̍ō���
#define RECID_DAYCNT_CAPTURE_FLAG     ( 3 )  ///< 1���ɕߊl�����|�P�����̐�
#define RECID_DAYMAX_TRAINER_BATTLE_FLAG  ( 3 )  ///< 1���Ƀg���[�i�[�킵���ō���
#define RECID_DAYCNT_TRAINER_BATTLE_FLAG  ( 3 )  ///< 1���Ƀg���[�i�[�킵����
#define RECID_DAYMAX_EVOLUTION_FLAG   ( 3 )  ///< 1���Ƀ|�P�����i���������ō���
#define RECID_DAYCNT_EVOLUTION_FLAG   ( 3 )  ///< 1���Ƀ|�P�����i����������
#define RECID_FOSSIL_RESTORE_FLAG     ( 3 )  ///< �|�P�����̉��΂𕜌�������  
#define RECID_GURUGURU_COUNT_FLAG     ( 3 )  ///< ���邮�������������        90
#define RECID_RANDOM_FREE_SINGLE_WIN_FLAG  ( 2 )  ///< 
#define RECID_RANDOM_FREE_SINGLE_LOSE_FLAG ( 2 )  ///< 
#define RECID_RANDOM_FREE_DOUBLE_WIN_FLAG  ( 2 )  ///< 
#define RECID_RANDOM_FREE_DOUBLE_LOSE_FLAG ( 2 )  ///< 
#define RECID_RANDOM_FREE_ROTATE_WIN_FLAG  ( 2 )  ///< 
#define RECID_RANDOM_FREE_ROTATE_LOSE_FLAG ( 2 )  ///< 
#define RECID_RANDOM_FREE_TRIPLE_WIN_FLAG  ( 2 )  ///< 
#define RECID_RANDOM_FREE_TRIPLE_LOSE_FLAG ( 2 )  ///< 
#define RECID_RANDOM_FREE_SHOOTER_WIN_FLAG ( 2 )  ///< 
#define RECID_RANDOM_FREE_SHOOTER_LOSE_FLAG ( 2 )  ///<
#define RECID_RANDOM_RATE_SINGLE_WIN_FLAG  ( 2 )  ///< 
#define RECID_RANDOM_RATE_SINGLE_LOSE_FLAG ( 2 )  ///< 
#define RECID_RANDOM_RATE_DOUBLE_WIN_FLAG  ( 2 )  ///< 
#define RECID_RANDOM_RATE_DOUBLE_LOSE_FLAG ( 2 )  ///< 
#define RECID_RANDOM_RATE_ROTATE_WIN_FLAG  ( 2 )  ///< 
#define RECID_RANDOM_RATE_ROTATE_LOSE_FLAG ( 2 )  ///< 
#define RECID_RANDOM_RATE_TRIPLE_WIN_FLAG  ( 2 )  ///< 
#define RECID_RANDOM_RATE_TRIPLE_LOSE_FLAG ( 2 )  ///< 
#define RECID_RANDOM_RATE_SHOOTER_WIN_FLAG ( 2 )  ///< 
#define RECID_RANDOM_RATE_SHOOTER_LOSE_FLAG ( 2 )  ///< �����_�����[�g�V���[�^�[�s�k�� 110
#define RECID_USE_SHOOTER_COUNT_FLAG       ( 3 )  ///< �V���[�^�[�g�p��
#define RECID_EFFECT_ENCOUNT_NUM_FLAG      ( 3 )  ///< �G�t�F�N�g�G���J�E���g��������
#define RECID_AFFINITY_CHECK_NUM_FLAG      ( 3 )  ///< �����`�F�b�N��������
#define RECID_MUSICAL_PLAY_NUM_FLAG        ( 3 )  ///< �~���[�W�J���Q����
#define RECID_MUSICAL_WINNER_NUM_FLAG      ( 3 )  ///< �~���[�W�J��������
#define RECID_MUSICAL_COMM_NUM_FLAG        ( 3 )  ///< �~���[�W�J���ʐM�Q����
#define RECID_MUSICAL_COMM_WINNER_NUM_FLAG ( 3 )  ///< �~���[�W�J���ʐM������
#define RECID_MUSICAL_TOTAL_POINT_FLAG     ( 2 )  ///< �~���[�W�J���ʎZ�擾���_
#define RECID_PDW_SLEEP_POKEMON_FLAG       ( 3 )  ///< PDW�Ń|�P������Q�������� 
#define RECID_POKESHIFTER_COUNT_FLAG       ( 4 )  ///< �|�P�V�t�^�[�������� 120
#define RECID_TRIALHOUSE_COUNT_FLAG        ( 3 )  ///< �g���C�A���n�E�X��������
#define RECID_SURECHIGAI_THANKS_FLAG       ( 3 )  ///< ���ꂿ�����ʐM�ł�����󂯂��񐔁����g�p
#define RECID_TRIALHOUSE_RANK_FLAG         ( 5 )  ///< �g���C�A���n�E�X�ō������N
#define RECID_TRIALHOUSE_SCORE_FLAG        ( 3 )  ///< �g���C�A���n�E�X�ō��X�R�A 124

#define RECID_SMALL_54_FLAG                ( 2 )  ///< 2�o�C�g���R�[�h�@���g�p�̈�MAX��`
#define RECID_SMALL_55_FLAG                ( 2 )  ///< 0 origin
#define RECID_SMALL_56_FLAG                ( 2 )
#define RECID_SMALL_57_FLAG                ( 2 )
#define RECID_SMALL_58_FLAG                ( 2 )
#define RECID_SMALL_59_FLAG                ( 2 )
#define RECID_SMALL_60_FLAG                ( 2 )
#define RECID_SMALL_61_FLAG                ( 2 )
#define RECID_SMALL_62_FLAG                ( 2 )
#define RECID_SMALL_63_FLAG                ( 2 )
#define RECID_SMALL_64_FLAG                ( 2 )
#define RECID_SMALL_65_FLAG                ( 2 )
#define RECID_SMALL_66_FLAG                ( 2 )
#define RECID_SMALL_67_FLAG                ( 2 )
#define RECID_SMALL_68_FLAG                ( 2 )
#define RECID_SMALL_69_FLAG                ( 2 )
#define RECID_SMALL_70_FLAG                ( 2 )
#define RECID_SMALL_71_FLAG                ( 2 )
#define RECID_SMALL_72_FLAG                ( 2 )
#define RECID_SMALL_73_FLAG                ( 2 )
#define RECID_SMALL_74_FLAG                ( 2 )
#define RECID_SMALL_75_FLAG                ( 2 )
#define RECID_SMALL_76_FLAG                ( 2 )
#define RECID_SMALL_77_FLAG                ( 2 )
#define RECID_SMALL_78_FLAG                ( 2 )
#define RECID_SMALL_79_FLAG                ( 2 )
#define RECID_SMALL_80_FLAG                ( 2 )
#define RECID_SMALL_81_FLAG                ( 2 )
#define RECID_SMALL_82_FLAG                ( 2 )
#define RECID_SMALL_83_FLAG                ( 2 )
#define RECID_SMALL_84_FLAG                ( 2 )
#define RECID_SMALL_85_FLAG                ( 2 )
#define RECID_SMALL_86_FLAG                ( 2 )
#define RECID_SMALL_87_FLAG                ( 2 )
#define RECID_SMALL_88_FLAG                ( 2 )
#define RECID_SMALL_89_FLAG                ( 2 )
#define RECID_SMALL_90_FLAG                ( 2 )
#define RECID_SMALL_91_FLAG                ( 2 )
#define RECID_SMALL_92_FLAG                ( 2 )
#define RECID_SMALL_93_FLAG                ( 2 )
#define RECID_SMALL_94_FLAG                ( 2 )
#define RECID_SMALL_95_FLAG                ( 2 )

#endif  /* __RECORD_H__ */

