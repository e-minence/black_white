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
extern u32 RECORD_GetThScore(RECORD * rec);
extern void RECORD_SetThRank(RECORD * rec, u8 inRank);
extern u8 RECORD_GetThRank(RECORD * rec);

#ifdef PM_DEBUG
//�X�R�A�̃Z�b�g
extern void RECORD_Score_DebugSet(RECORD * rec, const u32 inScore);
#endif

//----------------------------------------------------------
//----------------------------------------------------------
//�X�R�A�̉��Z����
extern void RECORD_Score_Add(RECORD * rec, int id);
//�X�R�A���擾����
extern u32 RECORD_Score_Get(RECORD * rec);

//FIXME �}�Ӗ��Ή�
//�}�ӃX�R�A���Z
//extern void RECORD_Score_AddZukanScore(RECORD * rec, const ZUKAN_WORK * zw, u16 const inMonsno);

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
#define RECID_REPORT_COUNT      (LARGE_REC_START+ 0)   ///< @���|�[�g��
#define RECID_CLEAR_TIME        (LARGE_REC_START+ 1)   ///< @�N���A�^�C��
#define RECID_RIDE_CYCLE        (LARGE_REC_START+ 2)   ///< @���]�Ԃɏ������
#define RECID_BATTLE_COUNT      (LARGE_REC_START+ 3)   ///< @�S�퓬��
#define RECID_BTL_ENCOUNT       (LARGE_REC_START+ 4)   ///< @�G���J�E���g�퓬��
#define RECID_BTL_TRAINER       (LARGE_REC_START+ 5)   ///< @�g���[�i�[�퓬��

#define RECID_CAPTURE_POKE      (LARGE_REC_START+ 6)   ///< @�ߊl�����|�P�����̐�
#define RECID_FISHING_SUCCESS   (LARGE_REC_START+ 7)   ///< @�ނ肠�����|�P�����̉�
#define RECID_TAMAGO_HATCHING   (LARGE_REC_START+ 8)  ///< @�^�}�S��z������
#define RECID_POKE_EVOLUTION    (LARGE_REC_START+ 9)  ///< @�i���������|�P�����̈���
#define RECID_RECOVERY_PC       (LARGE_REC_START+10)  ///< @�|�P�Z���ŋx�񂾉�

#define RECID_COMM_TRADE        (LARGE_REC_START+11)  ///< @���C�����X�ʐM������������
#define RECID_COMM_BATTLE       (LARGE_REC_START+12)  ///< @���C�����X�ʐM�ΐ��������
#define RECID_COMM_BTL_WIN      (LARGE_REC_START+13)  ///< @���C�����X�ʐM�ΐ�̏�����
#define RECID_COMM_BTL_LOSE     (LARGE_REC_START+14)  ///< @���C�����X�ʐM�ΐ�̔s�k��

#define RECID_WIFI_TRADE        (LARGE_REC_START+15)  ///< @WiFi�ʐM������������
#define RECID_WIFI_BATTLE       (LARGE_REC_START+16)  ///< @WiFi�ʐM�ΐ��������
#define RECID_WIFI_BTL_WIN      (LARGE_REC_START+17)  ///< @WiFi�ʐM�ΐ�̏�����
#define RECID_WIFI_BTL_LOSE     (LARGE_REC_START+18)  ///< @WiFi�ʐM�ΐ�̔s�k��
#define RECID_WIFI_BTL_DRAW     (LARGE_REC_START+19)  ///< @WiFi�ʐM�ΐ�̈���������

#define RECID_SHOPPING_CNT      (LARGE_REC_START+20)  ///< @��������������
#define RECID_SHOPPING_MONEY    (LARGE_REC_START+21)  ///< @���������������v���z
#define RECID_WATCH_TV          (LARGE_REC_START+22)  ///< @�e���r��������
#define RECID_SODATEYA_CNT      (LARGE_REC_START+23)  ///< @��ĉ��Ƀ|�P������a������

#define RECID_KILL_POKE         (LARGE_REC_START+24)  ///< @�|�����|�P�����̐�
#define RECID_DAYMAX_EXP        (LARGE_REC_START+25)  ///< @1���ɉ҂����o���l�ő�l
#define RECID_DAYCNT_EXP        (LARGE_REC_START+26)  ///< @1���ɉ҂����o���l

#define RECID_GTS_PUT           (LARGE_REC_START+27)  ///< @GTS�Ƀ|�P������a������
#define RECID_MAIL_WRITE        (LARGE_REC_START+28)  ///< @���[������������
#define RECID_NICKNAME          (LARGE_REC_START+29)  ///< @�|�P�����Ƀj�b�N�l�[����t������
#define RECID_PREMIUM_BALL      (LARGE_REC_START+30)  ///< @��������v���~�A�{�[���̐�
#define RECID_LEADERHOUSE_BATTLE  (LARGE_REC_START+31)  ///< @���[�_�[�n�E�X�Ő������
#define RECID_WIN_BP            (LARGE_REC_START+32)  ///< @���������BP
#define RECID_USE_BP            (LARGE_REC_START+33)  ///< @�g����BP

#define RECID_SURECHIGAI_NUM    (LARGE_REC_START+34)  ///< @���ꂿ�����ʐM��������
#define RECID_IRC_TRADE         (LARGE_REC_START+35)  ///< @�ԊO���ʐM������������
#define RECID_IRC_BATTLE        (LARGE_REC_START+36)  ///< @�ԊO���ʐM�ΐ��������
#define RECID_IRC_BTL_WIN       (LARGE_REC_START+37)  ///< @�ԊO���ʐM�ΐ�̏�����
#define RECID_IRC_BTL_LOSE      (LARGE_REC_START+38)  ///< @�ԊO���ʐM�ΐ�̔s�k��
#define RECID_PALACE_TIME       (LARGE_REC_START+39)  ///< @�p���X�K�⎞�ԍ��v
#define RECID_PALACE_MISSION     (LARGE_REC_START+40)  ///< @�p���X�N���A�����~�b�V�����̐�

#define LARGE_REC_NUM           (70+LARGE_REC_START)  ///< WB��41+1�g�p�B�c��29��
#define LARGE_REC_MAX           (LARGE_REC_NUM) 

//�ȉ��́A2�o�C�g�����J�E���^�������Ȃ�
#define SMALL_REC_START   LARGE_REC_MAX

#define RECID_DENDOU_CNT          (SMALL_REC_START+ 0) ///<@�a�����肵����
#define RECID_RECOVERY_HOME       (SMALL_REC_START+ 1) ///<@�����̉Ƃŋx�񂾉�

#define RECID_WAZA_HANERU         (SMALL_REC_START+ 2) ///< @�u�͂˂�v���g������
#define RECID_WAZA_WARUAGAKI      (SMALL_REC_START+ 3) ///< @�u��邠�����v���g������

#define RECID_WAZA_MUKOU          (SMALL_REC_START+ 4)  ///< @�����ʂ킴���o������
#define RECID_TEMOTI_KIZETU       (SMALL_REC_START+ 5)  ///< @�C�₳�����莝���̈���
#define RECID_TEMOTI_MAKIZOE      (SMALL_REC_START+ 6)  ///< @�����Ƀ_���[�W��^������
#define RECID_NIGERU_SIPPAI       (SMALL_REC_START+ 7)  ///< @���������Ȃ�����
#define RECID_NIGERARETA          (SMALL_REC_START+ 8)  ///< @������ꂽ�|�P�����̐�
#define RECID_FISHING_FAILURE     (SMALL_REC_START+ 9)  ///< @�葹�Ȃ�����
#define RECID_DAYMAX_KILL         (SMALL_REC_START+10)  ///< @1���ɓ|�����|�P�����̍ō���
#define RECID_DAYCNT_KILL         (SMALL_REC_START+11)  ///< @1���ɓ|�����|�P�����̐�
#define RECID_DAYMAX_CAPTURE      (SMALL_REC_START+12)  ///< @1���ɕߊl�����|�P�����̍ō���
#define RECID_DAYCNT_CAPTURE      (SMALL_REC_START+13)  ///< @1���ɕߊl�����|�P�����̐�
#define RECID_DAYMAX_TRAINER_BATTLE   (SMALL_REC_START+14)  ///< @1���Ƀg���[�i�[�킵���ō���
#define RECID_DAYCNT_TRAINER_BATTLE   (SMALL_REC_START+15)  ///< @1���Ƀg���[�i�[�킵����
#define RECID_DAYMAX_EVOLUTION    (SMALL_REC_START+16)  ///< @1���Ƀ|�P�����i���������ō���
#define RECID_DAYCNT_EVOLUTION    (SMALL_REC_START+17)  ///< @1���Ƀ|�P�����i����������
#define RECID_FOSSIL_RESTORE      (SMALL_REC_START+18)  ///< @�|�P�����̉��΂𕜌�������
#define RECID_GURUGURU_COUNT      (SMALL_REC_START+19)  ///< @���邮�������������

#define RECID_RANDOM_FREE_SINGLE_WIN   (SMALL_REC_START+20)  ///< @�����_���t���[�V���O��������
#define RECID_RANDOM_FREE_SINGLE_LOSE  (SMALL_REC_START+21)  ///< @�����_���t���[�V���O���s�k��
#define RECID_RANDOM_FREE_DOUBLE_WIN   (SMALL_REC_START+22)  ///< @�����_���t���[�_�u��������
#define RECID_RANDOM_FREE_DOUBLE_LOSE  (SMALL_REC_START+23)  ///< @�����_���t���[�_�u���s�k��
#define RECID_RANDOM_FREE_ROTATE_WIN   (SMALL_REC_START+24)  ///< @�����_���t���[���[�e�[�V����������
#define RECID_RANDOM_FREE_ROTATE_LOSE  (SMALL_REC_START+25)  ///< @�����_���t���[���[�e�[�V�����s�k��
#define RECID_RANDOM_FREE_TRIPLE_WIN   (SMALL_REC_START+26)  ///< @�����_���t���[�g���v��������
#define RECID_RANDOM_FREE_TRIPLE_LOSE  (SMALL_REC_START+27)  ///< @�����_���t���[�g���v���s�k��
#define RECID_RANDOM_FREE_SHOOTER_WIN  (SMALL_REC_START+28)  ///< @�����_���t���[�V���[�^�[������
#define RECID_RANDOM_FREE_SHOOTER_LOSE (SMALL_REC_START+29)  ///< @�����_���t���[�V���[�^�[�s�k��
#define RECID_RANDOM_RATE_SINGLE_WIN   (SMALL_REC_START+30)  ///< @�����_�����[�g�V���O��������
#define RECID_RANDOM_RATE_SINGLE_LOSE  (SMALL_REC_START+31)  ///< @�����_�����[�g�V���O���s�k��
#define RECID_RANDOM_RATE_DOUBLE_WIN   (SMALL_REC_START+32)  ///< @�����_�����[�g�_�u��������
#define RECID_RANDOM_RATE_DOUBLE_LOSE  (SMALL_REC_START+33)  ///< @�����_�����[�g�_�u���s�k��
#define RECID_RANDOM_RATE_ROTATE_WIN   (SMALL_REC_START+34)  ///< @�����_�����[�g���[�e�[�V����������
#define RECID_RANDOM_RATE_ROTATE_LOSE  (SMALL_REC_START+35)  ///< @�����_�����[�g���[�e�[�V�����s�k��
#define RECID_RANDOM_RATE_TRIPLE_WIN   (SMALL_REC_START+36)  ///< @�����_�����[�g�g���v��������
#define RECID_RANDOM_RATE_TRIPLE_LOSE  (SMALL_REC_START+37)  ///< @�����_�����[�g�g���v���s�k��
#define RECID_RANDOM_RATE_SHOOTER_WIN  (SMALL_REC_START+38)  ///< @�����_�����[�g�V���[�^�[������
#define RECID_RANDOM_RATE_SHOOTER_LOSE (SMALL_REC_START+39)  ///< @�����_�����[�g�V���[�^�[�s�k��
#define RECID_USE_SHOOTER_COUNT        (SMALL_REC_START+40)  ///< @�V���[�^�[�g�p��
#define RECID_SYMBOL_ENCOUNT_NUM       (SMALL_REC_START+41)  ///< @�V���{���G���J�E���g��������
#define RECID_AFFINITY_CHECK_NUM       (SMALL_REC_START+42)  ///< @�����`�F�b�N��������
#define RECID_MUSICAL_PLAY_NUM         (SMALL_REC_START+43)  ///< @�~���[�W�J���Q����
#define RECID_MUSICAL_WINNER_NUM       (SMALL_REC_START+44)  ///< @�~���[�W�J��������
#define RECID_MUSICAL_COMM_NUM         (SMALL_REC_START+45)  ///< @�~���[�W�J���ʐM�Q����
#define RECID_MUSICAL_COMM_WINNER_NUM  (SMALL_REC_START+46)  ///< @�~���[�W�J���ʐM������
#define RECID_MUSICAL_TOTAL_POINT      (SMALL_REC_START+47)  ///< @�~���[�W�J���ʎZ�擾���_
#define RECID_PDW_SLEEP_POKEMON        (SMALL_REC_START+48)  ///< @PDW�Ń|�P������Q��������
#define RECID_RENSHOU_SINGLE           (SMALL_REC_START+49)   ///< @�^���[:�V���O���o�g���A����
#define RECID_RENSHOU_SINGLE_CNT       (SMALL_REC_START+50)  ///< @�^���[:�V���O���o�g���A����
#define RECID_RENSHOU_S_SINGLE         (SMALL_REC_START+51)   ///< @�^���[:�X�[�p�[�V���O���o�g���A����
#define RECID_RENSHOU_S_SINGLE_CNT     (SMALL_REC_START+52)  ///< @�^���[:�X�[�p�[�V���O���o�g���A����
#define RECID_RENSHOU_DOUBLE           (SMALL_REC_START+53)  ///< @�^���[:�_�u���o�g���A����
#define RECID_RENSHOU_DOUBLE_CNT       (SMALL_REC_START+54)  ///< @�^���[:�_�u���o�g���A����
#define RECID_RENSHOU_S_DOUBLE         (SMALL_REC_START+55)  ///< @�^���[:�X�[�p�[�_�u���o�g���A����
#define RECID_RENSHOU_S_DOUBLE_CNT     (SMALL_REC_START+56)  ///< @�^���[:�X�[�p�[�_�u���o�g���A����
#define RECID_RENSHOU_MULTI            (SMALL_REC_START+57)  ///< @�^���[:�}���`�o�g���A����
#define RECID_RENSHOU_MULTI_CNT        (SMALL_REC_START+58)  ///< @�^���[:�}���`�o�g���A����
#define RECID_RENSHOU_S_MULTI          (SMALL_REC_START+59)  ///< @�^���[:�X�[�p�[�}���`�o�g���A����
#define RECID_RENSHOU_S_MULTI_CNT      (SMALL_REC_START+60)  ///< @�^���[:�X�[�p�[�}���`�o�g���A����
#define RECID_RENSHOU_WIFI             (SMALL_REC_START+61)  ///< @�^���[:Wi-Fi�o�g���ő�A����
#define RECID_RENSHOU_WIFI_CNT         (SMALL_REC_START+62)  ///< @�^���[:Wi-Fi�o�g���A����
#define RECID_SURECHIGAI_THANKS        (SMALL_REC_START+63)  ///< @���ꂿ�����ʐM�ł�����󂯂���
#define RECID_RENSHOU_WIFI_CNT         (SMALL_REC_START+62)  ///< @�^���[:Wi-Fi�o�g���A����
#define RECID_SURECHIGAI_THANKS        (SMALL_REC_START+63)  ///< @���ꂿ�����ʐM�ł�����󂯂���
#define RECID_TRIALHOUSE_RANK          (SMALL_REC_START+64)  ///< @�g���C�A���n�E�X�ō������N
#define RECID_TRIALHOUSE_SCORE         (SMALL_REC_START+65)  ///< @�g���C�A���n�E�X�ō��X�R�A


#define SMALL_REC_NUM ( 96 )  // 66+30 30�ӏ����]��    
#define SMALL_REC_MAX (SMALL_REC_NUM+LARGE_REC_MAX)


#endif  /* __RECORD_H__ */

