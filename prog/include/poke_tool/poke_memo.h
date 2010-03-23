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

//����g���[�i�[�����p�ꏊ��`
#define POKE_MEMO_PLACE_POKE_SHIFTER (30001)  //�|�P�V�t�^�[(�ߋ���]��
#define POKE_MEMO_PLACE_GAME_TRADE   (30002)  //�����񂱂�����(�Q�[����)
#define POKE_MEMO_PLACE_HUMAN_TRADE   (30002)  //�����񂱂�����(�l�ƒʐM)
#define POKE_MEMO_PLACE_KANTOU       (30004)  //�J���g�[
#define POKE_MEMO_PLACE_JYOUTO       (30005)  //�W���E�g
#define POKE_MEMO_PLACE_HOUEN        (30006)  //�z�E�G��
#define POKE_MEMO_PLACE_SHINOU       (30007)  //�V���I�E
#define POKE_MEMO_PLACE_FAR_PLACE    (30008)  //�Ƃ����Ƃ�(XD�E�R���V�A��
#define POKE_MEMO_PLACE_UNKNOWN      (30009)  //----------(�s��
#define POKE_MEMO_PERSON_SODATEYA    (60002)  //��ĉ��v�w

#define POKE_MEMO_PLACE_SEREBIXI_BEFORE  (30010)   //�Z���r�B�]����
#define POKE_MEMO_PLACE_SEREBIXI_AFTER   (30011)   //�Z���r�B�C�x���g��
#define POKE_MEMO_PLACE_ENRAISUI_BEFORE  (30012)   //�G���e�C�E���C�R�E�E�X�C�N���]����
#define POKE_MEMO_PLACE_ENRAISUI_AFTER   (30013)   //�G���e�C�E���C�R�E�E�X�C�N����

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

}POKE_MEMO_EVENT_TYPE;


//�|�P�����擾������
//�ݒ肵�Ă���̂͐��܂ꂽ/�߂܂����ꏊ/���ԁE�ߊl���x���E�e���ERom�o�[�W�����ł��B
extern void POKE_MEMO_SetTrainerMemoPP( POKEMON_PARAM *pp , const POKE_MEMO_SET_TYPE type ,
                                  const MYSTATUS* my, const u32 place , const HEAPID heapId );
extern void POKE_MEMO_SetTrainerMemoPPP( POKEMON_PASO_PARAM *ppp , const POKE_MEMO_SET_TYPE type ,
                                  const MYSTATUS* my, const u32 place , const HEAPID heapId );
//�]�v�Ȋ֐��Ă΂Ȃ����߂ɓƗ�
extern void POKE_MEMO_SetTrainerMemoPokeShifter( POKEMON_PASO_PARAM *ppp );

//�C�x���g�p�|�P�����`�F�b�N
extern const BOOL POKE_MEMO_CheckEventPokePP( POKEMON_PARAM *pp , const POKE_MEMO_EVENT_TYPE type );
extern const BOOL POKE_MEMO_CheckEventPokePPP( POKEMON_PASO_PARAM *ppp , const POKE_MEMO_EVENT_TYPE type );

//�C�x���g��̏������s��(2010�f��z�z�p
extern void POKE_MEMO_SetEventPoke_AfterEventPP( POKEMON_PARAM *pp , const POKE_MEMO_EVENT_TYPE type );
extern void POKE_MEMO_SetEventPoke_AfterEventPPP( POKEMON_PASO_PARAM *ppp , const POKE_MEMO_EVENT_TYPE type );
