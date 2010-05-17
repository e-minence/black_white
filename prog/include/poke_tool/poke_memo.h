//======================================================================
/**
 * @file	poke_memo.h
 * @brief	�g���[�i�[�����ݒ�E�\���֐�
 * @author	ariizumi
 * @data	10/03/13
 *
 * ���W���[�����FPOKE_MEMO
 */
//======================================================================
#include "poke_tool/poke_tool.h"

#include "msg/msg_place_name.h"
#include "msg/msg_place_name_out.h"
#include "msg/msg_place_name_per.h"
#include "msg/msg_place_name_spe.h"

//�g���[�i�[���� �t�@�C������
#define POKE_PLACE_START_SPECIAL_PLACE   (30001)
#define POKE_PLACE_START_OUTER_PLACE     (40001)
#define POKE_PLACE_START_PERSON_NAME     (60001)

//����g���[�i�[�����p�ꏊ��`
#define POKE_MEMO_PLACE_POKE_SHIFTER  (POKE_PLACE_START_SPECIAL_PLACE+MAPNAME_POKESIHUTAH)  //�|�P�V�t�^�[(�ߋ���]��
#define POKE_MEMO_PLACE_GAME_TRADE    (POKE_PLACE_START_SPECIAL_PLACE+MAPNAME_NAIBUTUUSINKOUKAN)  //�����񂱂�����(�Q�[����)
#define POKE_MEMO_PLACE_HUMAN_TRADE   (POKE_PLACE_START_SPECIAL_PLACE+MAPNAME_TUUSINKOUKAN)  //�����񂱂�����(�l�ƒʐM)
#define POKE_MEMO_PLACE_KANTOU        (POKE_PLACE_START_SPECIAL_PLACE+MAPNAME_KANTO)  //�J���g�[
#define POKE_MEMO_PLACE_JYOUTO        (POKE_PLACE_START_SPECIAL_PLACE+MAPNAME_ZYOTO)  //�W���E�g
#define POKE_MEMO_PLACE_HOUEN         (POKE_PLACE_START_SPECIAL_PLACE+MAPNAME_HOUEN)  //�z�E�G��
#define POKE_MEMO_PLACE_SHINOU        (POKE_PLACE_START_SPECIAL_PLACE+MAPNAME_SINOH)  //�V���I�E
#define POKE_MEMO_PLACE_FAR_PLACE     (POKE_PLACE_START_SPECIAL_PLACE+MAPNAME_TOOKUHANARETATOTI)  //�Ƃ����Ƃ�(XD�E�R���V�A��
#define POKE_MEMO_PLACE_UNKNOWN       (POKE_PLACE_START_SPECIAL_PLACE+MAPNAME_HUSEINAKOHDO)  //----------(�s��
#define POKE_MEMO_PERSON_SODATEYA     (POKE_PLACE_START_PERSON_NAME+MAPNAME_SODATEYA)  //��ĉ��v�w
#define POKE_MEMO_PERSON_TOREZYAHANTA (POKE_PLACE_START_PERSON_NAME+MAPNAME_TOREZYAHANTA)  //�g���W���[�n���^�[

#define POKE_MEMO_PLACE_SEREBIXI_BEFORE  (POKE_PLACE_START_SPECIAL_PLACE+MAPNAME_2010HAIHU_EV1_01)   //�Z���r�B�]����
#define POKE_MEMO_PLACE_SEREBIXI_AFTER   (POKE_PLACE_START_SPECIAL_PLACE+MAPNAME_2010HAIHU_EV1_02)   //�Z���r�B�C�x���g��
#define POKE_MEMO_PLACE_ENRAISUI_BEFORE  (POKE_PLACE_START_SPECIAL_PLACE+MAPNAME_2010HAIHU_EV2_01)   //�G���e�C�E���C�R�E�E�X�C�N���]����
#define POKE_MEMO_PLACE_ENRAISUI_AFTER   (POKE_PLACE_START_SPECIAL_PLACE+MAPNAME_2010HAIHU_EV2_02)   //�G���e�C�E���C�R�E�E�X�C�N����
#define POKE_MEMO_PLACE_PALACE           (POKE_PLACE_START_SPECIAL_PLACE+MAPNAME_PARESUKYOUTUU)   //�p���X

//�s���l�p
#define POKE_MEMO_UNKNOWN_PLACE       (MAPNAME_TOOIBASYO)
#define POKE_MEMO_UNKNOWN_PERSON      (POKE_PLACE_START_PERSON_NAME+MAPNAME_TOOKUNIIRUHITO)

//DPPtHGSS�ł̓���Ȓn��(�f��Z���r�B�E�G�����C�X�C
#define POKE_MEMO_2010_MOVIE   (3008)

//�|�P�����擾���g���[�i�[�������l������
typedef enum
{
  POKE_MEMO_SET_CAPTURE,  //�ߊl
  POKE_MEMO_GAME_TRADE,   //�Q�[��������
  POKE_MEMO_INCUBATION,   //�^�}�S�z��
  POKE_MEMO_POKE_SHIFTER, //�|�P�V�t�^�[(�ߋ���]��
  POKE_MEMO_DISTRIBUTION, //�z�z ���z�z�̐e���E�eID�͊O�ŃZ�b�g���Ă��������B
                          //     ���^�}�S�́����g���Ă��������B

  POKE_MEMO_EGG_FIRST,    //���߂Ď���ꂽ�^�}�S(��Ă�E�Q�[�����C�x���g�E�O���z�z
  POKE_MEMO_EGG_TRADE,    //��������^�}�S(�ʐM�����E���邮������E�Q�[��������
                          //place�ɂ�POKE_MEMO_PLACE_GAME_TRADE��POKE_MEMO_PLACE_HUMAN_TRADE���Z�b�g���Ă��������B
}POKE_MEMO_SET_TYPE;

//�C�x���g�|�P�`�F�b�N�p���
typedef enum
{
  POKE_MEMO_EVENT_2010MOVIE_SEREBIXI_BEF,   //2010�f��Z���r�B(�C�x���g�O
  POKE_MEMO_EVENT_2010MOVIE_SEREBIXI_AFT,   //2010�f��Z���r�B(�C�x���g��
  POKE_MEMO_EVENT_2010MOVIE_ENRAISUI_BEF,   //2010�f��G���e�C�E���C�R�E�E�X�C�N��(�C�x���g�O
  POKE_MEMO_EVENT_2010MOVIE_ENRAISUI_AFT,   //2010�f��G���e�C�E���C�R�E�E�X�C�N��(�C�x���g��
  POKE_MEMO_EVENT_DARUTANISU,           //�z�z�_���^�j�X
  POKE_MEMO_EVENT_MERODHIA,             //�z�z�����f�B�A
  POKE_MEMO_EVENT_INSEKUTA,             //�z�z�C���Z�N�^
  POKE_MEMO_EVENT_SHEIMI,               //�z�z�V�F�C�~

}POKE_MEMO_EVENT_TYPE;


//�|�P�����擾������
//�ݒ肵�Ă���̂͐��܂ꂽ/�߂܂����ꏊ/���ԁE�ߊl���x���E�e���ERom�o�[�W�����ł��B
extern void POKE_MEMO_SetTrainerMemoPP( POKEMON_PARAM *pp , const POKE_MEMO_SET_TYPE type ,
                                  const MYSTATUS* my, const u32 place , const HEAPID heapId );
extern void POKE_MEMO_SetTrainerMemoPPP( POKEMON_PASO_PARAM *ppp , const POKE_MEMO_SET_TYPE type ,
                                  const MYSTATUS* my, const u32 place , const HEAPID heapId );
//�]�v�Ȋ֐��Ă΂Ȃ����߂ɓƗ�(������ɐe�@�̎��ԂōĐݒ�
extern void POKE_MEMO_SetTrainerMemoPokeShifter( POKEMON_PASO_PARAM *ppp );
extern void POKE_MEMO_SetTrainerMemoPokeShifterAfterTrans( POKEMON_PASO_PARAM *ppp );
//���t�ݒ�̂��ߓƗ�
extern void POKE_MEMO_SetTrainerMemoPokeDistribution( POKEMON_PASO_PARAM *ppp , const u32 place , const u32 year , const u32 month , const u32 day );

//�C�x���g�p�|�P�����`�F�b�N
extern const BOOL POKE_MEMO_CheckEventPokePP( POKEMON_PARAM *pp , const POKE_MEMO_EVENT_TYPE type );
extern const BOOL POKE_MEMO_CheckEventPokePPP( POKEMON_PASO_PARAM *ppp , const POKE_MEMO_EVENT_TYPE type );

//�C�x���g��̏������s��(2010�f��z�z�p
extern void POKE_MEMO_SetEventPoke_AfterEventPP( POKEMON_PARAM *pp , const POKE_MEMO_EVENT_TYPE type );
extern void POKE_MEMO_SetEventPoke_AfterEventPPP( POKEMON_PASO_PARAM *ppp , const POKE_MEMO_EVENT_TYPE type );

//�n�������b�Z�[�W��datId��messageId
const ARCDATID POKE_PLACE_GetMessageDatId( const u32 place );
const u32 POKE_PLACE_GetMessageId( const u32 place );
