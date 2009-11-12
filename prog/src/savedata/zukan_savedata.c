//============================================================================================
/**
 * @file  zukan_savedata.c
 * @brief �}�ӃZ�[�u�f�[�^�A�N�Z�X�����Q�\�[�X
 * @author  mori / tamada GAME FREAK inc.
 * @date  2009.10.26
 */
//============================================================================================

#include <gflib.h>

#include "pm_version.h"
#include "system/gfl_use.h"
#include "savedata/save_tbl.h"
#include "savedata/save_control.h"

#include "gamesystem/game_data.h"
#include "savedata/zukan_savedata.h"
#include "poke_tool/poke_personal.h"


//#include "include/application/zukanlist/zkn_world_text_data.h"

// �C�O�Ő}�Ӄe�L�X�g���������݂��Ă�����P�A����������O
#define FOREIGN_TEXT_FUNC   ( 0 )

// �C�b�V���}�Ӄe�[�u���������Ԃ͑S���̐}�ӏ�񂩂琔����悤�ɂ���(1:���� 0:�C�b�V���}�ӂ���)
#define ZUKAN_ISSHU_TABLE_NONE  ( 1 )


//============================================================================================
//============================================================================================
enum {
  POKEZUKAN_ARRAY_LEN = 21, ///<�|�P�����}�Ӄt���O�z��̂�������16 * 32 = 512�܂ő��v

  POKEZUKAN_UNKNOWN_NUM = UNK_END,///<�A���m�[����

  MAGIC_NUMBER = 0xbeefcafe,

  POKEZUKAN_DEOKISISU_MSK = 0xf,  // bit�}�X�N  POKEZUKAN_ARRAY_LEN�̌��15bit�����f�I�L�V�X�Ɏg�p���Ă܂��B

  POKEZUKAN_TEXT_POSSIBLE  = 493, // �C�O�}�Ӄe�L�X�g���ǂ߂�ő吔�i���V���I�E�|�P�����܂Łj

  POKEZUKAN_TEXTVER_UP_NUM = POKEZUKAN_TEXT_POSSIBLE + 1, //  �����ڃA�b�v�f�[�g�o�b�t�@��  +1�̓p�f�B���O

};

//----------------------------------------------------------
/**
 * @brief ������ԃf�[�^�^��`
 */
//----------------------------------------------------------
struct ZUKAN_SAVEDATA {
  u32 zukan_magic;                      ///<�}�W�b�N�i���o�[
  u32 get_flag[POKEZUKAN_ARRAY_LEN];    ///<�߂܂����t���O�p���[�N
  u32 see_flag[POKEZUKAN_ARRAY_LEN];    ///<�������t���O�p���[�N
  u32 sex_flag[2][POKEZUKAN_ARRAY_LEN]; ///<�I�X���X�t���O�p���[�N
  u32 PachiRandom;            ///<�p�b�`�[���p�������ێ����[�N
  u8 SiiusiTurn;              ///<�V�[�E�V�p���������ێ����[�N    2bit�K�v  1bit*2���
  u8 SiidorugoTurn;           ///<�V�[�h���S�p���������ێ����[�N  2bit�K�v  1bit*2���
  u8 MinomuttiTurn;           ///<�~�m���b�`�p���������ێ����[�N  6bit�K�v  2bit*3���
  u8 MinomesuTurn;            ///<�~�m���X�p���������ێ����[�N    6bit�K�v  2bit*3���
  u8 UnknownTurn[ POKEZUKAN_UNKNOWN_NUM ];///<�A���m�[�����������ԍ�
  u8 UnknownGetTurn[ POKEZUKAN_UNKNOWN_NUM ];///<�A���m�[����ߊl�����ԍ�
  u8 TextVersionUp[ POKEZUKAN_TEXTVER_UP_NUM ];///<����o�[�W�����A�b�v�}�X�N
  u8 GraphicVersionUp;          ///<�O���t�B�b�N�o�[�W�����p�@�\�g���t���O
  u8 TextVersionUpMasterFlag;       ///<����o�[�W�����A�b�vMaster�t���O
  u8 zukan_get;             ///<������擾�t���O
  u8 zenkoku_flag;            ///<�S���}�ӕێ��t���O
  ///<���g���p���������ԕێ����[�N    18bit�K�v 3bit*6���
  ///<�V�F�C�~�p���������ԕێ����[�N  2bit�K�v  1bit*2���
  ///<�M���e�B�i�p���������ԕێ����[�N  2bit�K�v  1bit*2���
  u32 rotomu_turn;
  u8  syeimi_turn;
  u8  giratyina_turn;
  u8  pityuu_turn;  ///< �s�`���[�p���������ێ����[�N 2bit*3���( ��/��/�M�U�~�~ )
  u8  turn_pad;   ///< �]��
};

//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief ������f�[�^�p���[�N�̃T�C�Y�擾
 * @return  int   ZUKAN_SAVEDATA�̃T�C�Y
 */
//----------------------------------------------------------
int ZUKANSAVE_GetWorkSize(void)
{
  return sizeof(ZUKAN_SAVEDATA);
}
//----------------------------------------------------------
/**
 * @brief ������f�[�^�p���[�N�̐�������
 * @param heapID    �g�p����q�[�v�̎w��
 * @return  ZUKAN_SAVEDATA  ��������ZUKAN_SAVEDATA�ւ̃|�C���^
 */
//----------------------------------------------------------
ZUKAN_SAVEDATA * ZUKANSAVE_AllocWork( HEAPID heapID )
{
  ZUKAN_SAVEDATA * zw;
  zw = GFL_HEAP_AllocClearMemory( heapID, sizeof(ZUKAN_SAVEDATA));
  ZUKANSAVE_Init(zw);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc( GMDATA_ID_ZUKAN );
#endif //CRC_LOADCHECK
  return zw;
}

//----------------------------------------------------------
/**
 * @brief ������f�[�^�p���[�N�̃R�s�[
 * @param from  �R�s�[��ZUKAN_SAVEDATA�ւ̃|�C���^
 * @param to    �R�s�[��ZUKAN_SAVEDATA�ւ̃|�C���^
 */
//----------------------------------------------------------
void ZUKANSAVE_Copy(const ZUKAN_SAVEDATA * from, ZUKAN_SAVEDATA * to)
{
  MI_CpuCopy8(from, to, sizeof(ZUKAN_SAVEDATA));
}

//============================================================================================
//
//        �����֐�
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief �����񃏁[�N�̐������`�F�b�N
 */
//----------------------------------------------------------
static inline void zukan_incorrect(const ZUKAN_SAVEDATA * zw)
{
  GF_ASSERT(zw->zukan_magic == MAGIC_NUMBER);
}
//----------------------------------------------------------
/**
 * @brief �|�P�����i���o�[�͈̔̓`�F�b�N
 */
//----------------------------------------------------------
static BOOL monsno_incorrect(u16 monsno)
{
  if (monsno == 0 || monsno > MONSNO_END) {
    GF_ASSERT_MSG(0, "�|�P�����i���o�[�ُ�F%d\n", monsno);
    return TRUE;
  } else {
    return FALSE;
  }
}

//----------------------------------------------------------
/**
 * @brief �ėp�r�b�g�`�F�b�N
 */
//----------------------------------------------------------
static inline BOOL check_bit(const u8 * array, u16 flag_id)
{
  flag_id --;
  return 0 != (array[flag_id >> 3] & (1 << (flag_id & 7)));
}
//----------------------------------------------------------
/**
 * @brief �ėp�r�b�g�Z�b�g
 */
//----------------------------------------------------------
static inline void set_bit(u8 * array, u16 flag_id)
{
  flag_id --;
  array[flag_id >> 3] |= 1 << (flag_id & 7);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
}
//----------------------------------------------------------
/**
 * @brief �ėp1�r�b�g�f�[�^�Z�b�g
 * ���̃r�b�g���N���[���ɂ��Ă���ݒ肵�܂�
 */
//----------------------------------------------------------
static inline void setnum_bit(u8 * array, u8 num, u16 flag_id)
{
  GF_ASSERT( num < 2 );
  flag_id --;

  array[flag_id >> 3] &= ~(1 << (flag_id & 7));
  array[flag_id >> 3] |= num << (flag_id & 7);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
}

#if 0
static inline void reset_bit(u8 * array, u16 flag_id)
{
  array[flag_id >> 3] &= (1 << (flag_id & 7)) ^ 0xff;
}
#endif
//----------------------------------------------------------
/**
 * @brief �ėp�r�b�g���J�E���g����
 */
//----------------------------------------------------------
static u16 count_bit(const u32 * array, u32 array_max)
{
  int count, idx;
  u32 item;
  count = 0;
  for (idx = 0; idx < array_max; idx ++) {
    for (item = array[idx]; item != 0; item >>= 1) {
      if (item & 1) {
        count ++;
      }
    }
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
  return count;
}

//----------------------------------------------------------
/**
 * @brief �ėp2�r�b�g�`�F�b�N
 * ��1bit�ł�1�`x���w�肷��悤�ɂȂ��Ă�����
 * 2bit�ł�0�`x���w�肷��悤�ɂ���
 */
//----------------------------------------------------------
static inline u32 check_bit2(const u8 * array, u16 flag_id)
{
  return (array[flag_id >> 2] >> ((flag_id & 3)*2)) & 0x3;
}
//----------------------------------------------------------
/**
 * @brief �ėp2�r�b�g�f�[�^�Z�b�g
 * ���̃r�b�g���N���[���ɂ��Ă���ݒ肵�܂�
 */
//----------------------------------------------------------
static inline void setnum_bit2(u8 * array, u8 num, u16 flag_id)
{
  GF_ASSERT( num < 4 );

  array[flag_id >> 2] &= ~(0x3 << ((flag_id & 3)*2) );
  array[flag_id >> 2] |= num << ((flag_id & 3)*2);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
}


//----------------------------------------------------------
/**
 * @brief �݂��r�b�g�ݒ�
 */
//----------------------------------------------------------
static inline void set_see_bit(ZUKAN_SAVEDATA * zw, u16 flag_id)
{
  set_bit((u8*)zw->see_flag, flag_id);
}
//----------------------------------------------------------
/**
 * @brief ���܂����r�b�g�ݒ�
 */
//----------------------------------------------------------
static inline void set_get_bit(ZUKAN_SAVEDATA * zw, u16 flag_id)
{
  set_bit((u8*)zw->get_flag, flag_id);
}
//----------------------------------------------------------
/**
 * @brief ���ʃr�b�g�ݒ�  ���ۂɐݒ�
 * �ŏ��̐��ʐݒ��2�v�f�ڂ̔z��ɂ������l��ݒ肷��
 * ����ɂ��2�v�f�ڂ������l�̎��͈Ⴄ���ʂ̃|�P�������݂Ă��Ȃ�
 * �Ƃ������ƂɂȂ�
 */
//----------------------------------------------------------
static void set_sex_bit_first_second(ZUKAN_SAVEDATA * zw, u8 num, u8 no, u16 flag_id)
{
  if( no == ZUKANSAVE_SEX_FIRST ){
    setnum_bit((u8*)zw->sex_flag[ZUKANSAVE_SEX_SECOND], num, flag_id);
  }
  setnum_bit((u8*)zw->sex_flag[no], num, flag_id);
}
//----------------------------------------------------------
/**
 * @brief ���ʃr�b�g�ݒ�
 */
//----------------------------------------------------------
static void set_sex_bit(ZUKAN_SAVEDATA * zw, u8 num, u8 no, u16 monsno)
{
  GF_ASSERT_MSG( num <= 2, "num = %d", num ); // �j�@���@�s���ȊO�����܂���

  if( num == PTL_SEX_UNKNOWN )   // UNKNOW�͒j�ɂ���
  {
    num = PTL_SEX_MALE;
  }
  set_sex_bit_first_second(zw, num, no, monsno);
}
//----------------------------------------------------------
/**
 * @brief �݂��r�b�gONOFF�`�F�b�N
 */
//----------------------------------------------------------
static inline BOOL check_see_bit(const ZUKAN_SAVEDATA * zw, u16 flag_id)
{
  return check_bit((const u8*)zw->see_flag, flag_id);
}
//----------------------------------------------------------
/**
 * @brief ���܂����r�b�gONOFF�`�F�b�N
 */
//----------------------------------------------------------
static inline BOOL check_get_bit(const ZUKAN_SAVEDATA * zw, u16 flag_id)
{
  return check_bit((const u8*)zw->get_flag, flag_id);
}
//----------------------------------------------------------
/**
 * @brief ���ʃr�b�gONOFF�`�F�b�N
 */
//----------------------------------------------------------
static inline u8 check_sex_bit(const ZUKAN_SAVEDATA * zw, u16 flag_id, u8 first_second)
{
  return check_bit((const u8*)zw->sex_flag[first_second], flag_id);
}

//----------------------------------------------------------
/**
 * @brief �ő̗�����ݒ�
 */
//----------------------------------------------------------
static inline void SetZukanRandom(ZUKAN_SAVEDATA * zw, u16 monsno, u32 rand)
{
  if (monsno == MONSNO_PATTIIRU) {
    zw->PachiRandom = rand;
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
}

//----------------------------------------------------------
/**
 * @brief �����t�H���������|�P�����̌������ێ����[�N�|�C���^��Ԃ�(�A���m�[���ȊO)
 */
//----------------------------------------------------------
static const u8* get_turn_arry(const ZUKAN_SAVEDATA* zw,u32 monsno)
{
  switch( monsno ){
  case MONSNO_KARANAKUSI:
    return &zw->SiiusiTurn;
  case MONSNO_TORITODON:
    return &zw->SiidorugoTurn;
  case MONSNO_SHEIMI:
    return &zw->syeimi_turn;
  case MONSNO_GIRATHINA:
    return &zw->giratyina_turn;
  case MONSNO_MINOMUTTI:
    return &zw->MinomuttiTurn;
  case MONSNO_MINOMADAMU:
    return &zw->MinomesuTurn;
  case MONSNO_PITYUU:
    return &zw->pityuu_turn;
  }
  GF_ASSERT(0);
  return NULL;
}

//----------------------------------------------------------
/**
 * @brief �A���m�[���̌��ݐݒ萔���擾
 */
//----------------------------------------------------------
static int get_zukan_unknown_turn_num( const ZUKAN_SAVEDATA * zw ,BOOL get_f)
{
  int i,num = 0;
  u8* data;

  if(get_f){
    data = (u8*)zw->UnknownGetTurn;
  }else{
    data = (u8*)zw->UnknownTurn;
  }
  for( i=0; i<POKEZUKAN_UNKNOWN_NUM; i++ ){
    if( data[i] == 0xff ){
      break;
    }
    num++;
  }
  return num;
}
//----------------------------------------------------------
/**
 * @brief �A���m�[���t�H�[���ݒ�ς݂��`�F�b�N
 */
//----------------------------------------------------------
static BOOL check_zukan_unknown_turn_set( const ZUKAN_SAVEDATA * zw, u8 form ,BOOL get_f)
{
  int i;
  u8* data;

  if(get_f){
    data = (u8*)zw->UnknownGetTurn;
  }else{
    data = (u8*)zw->UnknownTurn;
  }
  for( i=0; i<POKEZUKAN_UNKNOWN_NUM; i++ ){
    if( data[i] == form ){
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------
/**
 * @brief �A���m�[��������(�߂܂���)���f�[�^�̍Ō�Ɍ`��ID��ݒ�
 */
//----------------------------------------------------------
static void SetZukanUnknownTurn( ZUKAN_SAVEDATA * zw, int form, BOOL get_f)
{
  int set_idx;
  u8* data;

  if(get_f){
    data = zw->UnknownGetTurn;
  }else{
    data = zw->UnknownTurn;
  }

  // �ݒ�ς݂��`�F�b�N
  if( check_zukan_unknown_turn_set( zw, form, get_f ) ){
    return;
  }

  set_idx = get_zukan_unknown_turn_num( zw, get_f );
  if( set_idx < POKEZUKAN_UNKNOWN_NUM ){
    data[ set_idx ] = form;
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
}

//----------------------------------------------------------
/**
 * @brief �V�[�E�V�@�V�[�h���S �V�F�C�~�@�M���e�B�i�̌��ݐݒ萔���擾
 */
//----------------------------------------------------------
static int get_twoform_poke_turn_num( const ZUKAN_SAVEDATA * zw, u32 monsno )
{
  u32 first_form;
  u32 second_form;
  const u8* arry;
  GF_ASSERT( (monsno == MONSNO_KARANAKUSI) || (monsno == MONSNO_TORITODON) || (monsno == MONSNO_SHEIMI) || (monsno == MONSNO_GIRATHINA) );

  // ���Ă����Ȃ��Ƃ���0
  if( ZUKANSAVE_GetPokeSeeFlag( zw, monsno ) == FALSE ){
    return 0;
  }
#if 0
  switch( monsno ){
  case MONSNO_KARANAKUSI:
    arry = &zw->SiiusiTurn;
    break;

  case MONSNO_TORITODON:
    arry = &zw->SiidorugoTurn;
    break;

  case MONSNO_SHEIMI:
    arry = &zw->syeimi_turn;
    break;

  case MONSNO_GIRATHINA:
    arry = &zw->giratyina_turn;
    break;
  }
#else
  arry = get_turn_arry(zw,monsno);
#endif
  first_form = check_bit( arry, 1 );
  second_form = check_bit( arry, 2 );

  if( first_form == second_form ){  // �ŏ��ƂQ�߂��ꏏ�Ȃ�P�񂾂�
    return 1;
  }
  return 2;
}
//----------------------------------------------------------
/**
 * @brief �~�m���b�`�@�~�m���X�@�V�F�C�~�@�M���e�B�i�@�t�H�[���ݒ�ς݂��`�F�b�N
 * @param form  0 or 1
 * *���� 0or1����ō쐬���Ă��܂��B
 */
//----------------------------------------------------------
static BOOL check_twoform_poke_turn_set( const ZUKAN_SAVEDATA * zw, u32 monsno, u8 form )
{
  u32 get_form;
  u32 i;
  u32 roop_num;
  const u8* arry;
  GF_ASSERT( (monsno == MONSNO_KARANAKUSI) || (monsno == MONSNO_TORITODON) || (monsno == MONSNO_SHEIMI) || (monsno == MONSNO_GIRATHINA) );

  // ���Ă����Ȃ��Ƃ��͐ݒ肳��Ă���킯���Ȃ�
  if( ZUKANSAVE_GetPokeSeeFlag( zw, monsno ) == FALSE ){
    return FALSE;
  }

#if 0
  switch( monsno ){
  case MONSNO_KARANAKUSI:
    arry = &zw->SiiusiTurn;
    break;

  case MONSNO_TORITODON:
    arry = &zw->SiidorugoTurn;
    break;

  case MONSNO_SHEIMI:
    arry = &zw->syeimi_turn;
    break;

  case MONSNO_GIRATHINA:
    arry = &zw->giratyina_turn;
    break;
  }
#else
  arry = get_turn_arry(zw,monsno);
#endif
  // �ݒ萔���`�F�b�N
  // �P�񂵂����Ă��Ȃ��Ƃ��͂Q�ڂ̃r�b�g�́A
  // �_�~�[�f�[�^�Ȃ̂ŁA�����������Ȃ�
  roop_num = get_twoform_poke_turn_num( zw, monsno );
  for( i=0; i<roop_num; i++ ){
    get_form = check_bit( arry, i+1 );
    if( get_form == form ){
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------
/**
 * @brief �V�[�E�V�@�V�[�h���S�@�V�F�C�~�@�M���e�B�i���������f�[�^�̍Ō�Ɍ`��ID��ݒ�
 * *���� 0or1����ō쐬���Ă��܂��B
 */
//----------------------------------------------------------
static void SetZukanTwoFormPokeTurn( ZUKAN_SAVEDATA * zw, u32 monsno, int form)
{
  int set_idx;
  u8* arry;
  GF_ASSERT( (monsno == MONSNO_KARANAKUSI) || (monsno == MONSNO_TORITODON) || (monsno == MONSNO_SHEIMI) || (monsno == MONSNO_GIRATHINA) );

  // �ݒ�ς݂��`�F�b�N
  if( check_twoform_poke_turn_set( zw, monsno, form ) ){
    return;
  }
#if 0
  switch( monsno ){
  case MONSNO_KARANAKUSI:
    arry = &zw->SiiusiTurn;
    break;

  case MONSNO_TORITODON:
    arry = &zw->SiidorugoTurn;
    break;

  case MONSNO_SHEIMI:
    arry = &zw->syeimi_turn;
    break;

  case MONSNO_GIRATHINA:
    arry = &zw->giratyina_turn;
    break;
  }
#else
  arry = (u8*)get_turn_arry(zw,monsno);
#endif
  set_idx = get_twoform_poke_turn_num( zw, monsno );
  if( set_idx < 2 ){  // 0�����Ȃ�ŏ��ɐݒ�@1�Ȃ�2�߂ɐݒ� 2�Ȃ�ݒ肵�Ȃ�
    setnum_bit( arry, form, set_idx+1 );
    if( set_idx == 0 ){
      setnum_bit( arry, form, set_idx+2 );  // ���������G�ŕۑ�����
    }
  }
}

//----------------------------------------------------------
/**
 * @brief �~�m���b�`�A�~�m���X�A�s�`���[�̌��ݐݒ萔���擾
 * ���`��0�`2����
 */
//----------------------------------------------------------
static int get_threeform_poke_turn_num( const ZUKAN_SAVEDATA * zw, u32 monsno )
{
  u32 get_form;
  int i;
  const u8* arry;

  GF_ASSERT( (monsno == MONSNO_MINOMUTTI) || (monsno == MONSNO_MINOMADAMU) || (monsno == MONSNO_PITYUU) );
  // ���Ă����Ȃ��Ƃ���0
  if( ZUKANSAVE_GetPokeSeeFlag( zw, monsno ) == FALSE ){
    return 0;
  }

#if 0
  switch( monsno ){
  case MONSNO_MINOMUTTI:
    arry = &zw->MinomuttiTurn;
    break;
  case MONSNO_MINOMADAMU:
    arry = &zw->MinomesuTurn;
    break;
  case MONSNO_PITYUU:
    arry = &zw->pityuu_turn;
    break;
  }
#else
  arry = get_turn_arry(zw,monsno);
#endif
  for( i=0; i<3; i++ ){
    get_form = check_bit2( arry, i );
    if( get_form == 3 ){
      break;
    }
  }
  return i;
}
//----------------------------------------------------------
/**
 * @brief �~�m���b�`�A�~�m���X�A�s�`���[�̃t�H�����ݒ�ς݂��`�F�b�N
 * @param form  0 or 1 or 2
 * ���`��0�`2����
 */
//----------------------------------------------------------
static BOOL check_threeform_poke_turn_set( const ZUKAN_SAVEDATA * zw, u32 monsno, u8 form )
{
  u32 get_form;
  int i;
  const u8* arry;

  GF_ASSERT( (monsno == MONSNO_MINOMUTTI) || (monsno == MONSNO_MINOMADAMU) || (monsno == MONSNO_PITYUU) );
  // ���Ă����Ȃ��Ƃ���FALSE
  if( ZUKANSAVE_GetPokeSeeFlag( zw, monsno ) == FALSE ){
    return FALSE;
  }

#if 0
  switch( monsno ){
  case MONSNO_MINOMUTTI:
    arry = &zw->MinomuttiTurn;
    break;
  case MONSNO_MINOMADAMU:
    arry = &zw->MinomesuTurn;
    break;
  case MONSNO_PITYUU:
    arry = &zw->pityuu_turn;
    break;
  }
#else
  arry = get_turn_arry(zw,monsno);
#endif

  for( i=0; i<3; i++ ){
    get_form = check_bit2( arry, i );
    if( get_form == form ){
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------
/**
 * @brief �~�m���b�`�A�~�m���X�A�s�`���[�̌��������f�[�^�̍Ō�Ɍ`��ID��ݒ�
 * ���`��0�`2����
 */
//----------------------------------------------------------
static void SetZukanThreeFormPokeTurn( ZUKAN_SAVEDATA * zw, u32 monsno, int form)
{
  int set_idx;
  u8* arry;
  GF_ASSERT( (monsno == MONSNO_MINOMUTTI) || (monsno == MONSNO_MINOMADAMU) || (monsno == MONSNO_PITYUU) );

  // �ݒ�ς݂��`�F�b�N
  if( check_threeform_poke_turn_set( zw, monsno, form ) ){
    return;
  }

#if 0
  switch( monsno ){
  case MONSNO_MINOMUTTI:
    arry = &zw->MinomuttiTurn;
    break;
  case MONSNO_MINOMADAMU:
    arry = &zw->MinomesuTurn;
    break;
  case MONSNO_PITYUU:
    arry = &zw->pityuu_turn;
    break;
  }
#else
  arry = (u8*)get_turn_arry(zw,monsno);
#endif
  set_idx = get_threeform_poke_turn_num( zw, monsno );
  if( set_idx < 3 ){
    setnum_bit2( arry, form, set_idx );
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  �f�I�L�V�X�t�H�����ݒ�
 *
 *  @param  p_arry  �i�[�̈�
 *  @param  no  �f�I�L�V�X�t�H����
 *  @param  num ���������Ԑݒ�
 */
//-----------------------------------------------------------------------------
static void setDeokisisuFormNoCore( u32* p_arry, u8 no, u8 num )
{
  u32 shift = (24 + (num * 4));
  u32 msk = ~(POKEZUKAN_DEOKISISU_MSK << shift);
  p_arry[ POKEZUKAN_ARRAY_LEN - 1 ] &= msk;
  p_arry[ POKEZUKAN_ARRAY_LEN - 1 ] |= (no << shift);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
}

//----------------------------------------------------------------------------
/**
 *  @brief  �f�I�L�V�X�t�H�����ݒ�
 *
 *  @param  zw  �}�Ӄ��[�N
 *  @param  no  �f�I�L�V�X�t�H����
 *  @param  num ���������Ԑݒ�
 */
//-----------------------------------------------------------------------------
static void setDEOKISISUFormNo( ZUKAN_SAVEDATA * zw, u8 no, u8 num )
{
  GF_ASSERT( num < ZUKANSAVE_DEOKISISU_FORM_NUM );
  GF_ASSERT( no <= POKEZUKAN_DEOKISISU_INIT );
  // �f�I�L�V�X�i���o�[��ݒ�
  // �߂܂����t���O�̌��15bit�󂢂Ă���̂�
  // ���4bit���Ƀf�[�^��o�^���Ă���
  if( num < ZUKANSAVE_DEOKISISU_FORM_SAVEDATA_CUTNUM ){
    setDeokisisuFormNoCore( zw->get_flag, no, num );
  }else{
    setDeokisisuFormNoCore( zw->see_flag, no, num-ZUKANSAVE_DEOKISISU_FORM_SAVEDATA_CUTNUM );
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  �f�I�L�V�X�t�H�����擾
 *
 *  @param  p_arry  �i�[�̈�
 *  @param  num ���������Ԑݒ�
 *
 *  @retval �t�H�����i���o�[
 */
//-----------------------------------------------------------------------------
static inline u32 getDeokisisuFormNoCore( const u32* p_arry, u8 num )
{
  u32 shift = (24 + (num * 4));
  u32 ret = (p_arry[ POKEZUKAN_ARRAY_LEN - 1 ] >> shift) & POKEZUKAN_DEOKISISU_MSK;
  return ret;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �f�I�L�V�X�t�H�����擾
 *
 *  @param  zw  �}�Ӄ��[�N
 *  @retval �f�I�L�V�X�t�H����
 */
//-----------------------------------------------------------------------------
static u32 getDEOKISISUFormNo( const ZUKAN_SAVEDATA * zw, u8 num )
{
  u32 form;
  // �f�I�L�V�X�i���o�[��ݒ�
  // �߂܂����t���O�̌��15bit�󂢂Ă���̂�
  // ���4bit�Ƀf�[�^��o�^���Ă���
  if( num < ZUKANSAVE_DEOKISISU_FORM_SAVEDATA_CUTNUM ){
    form = getDeokisisuFormNoCore( zw->get_flag, num );
  }else{
    form = getDeokisisuFormNoCore( zw->see_flag, num - ZUKANSAVE_DEOKISISU_FORM_SAVEDATA_CUTNUM );
  }
  return form;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���܂łɌ����f�I�L�V�X�̐���Ԃ�
 *
 *  @param  zw  �}�Ӄ��[�N
 *
 *  @return ���܂łɌ����f�I�L�V�X�̐���Ԃ�
 */
//-----------------------------------------------------------------------------
static u32 nowGetDeokisisuFormNum( const ZUKAN_SAVEDATA * zw )
{
  int i;

  for( i=0; i<ZUKANSAVE_DEOKISISU_FORM_NUM; i++ ){
    if( getDEOKISISUFormNo( zw, i ) == POKEZUKAN_DEOKISISU_INIT  ){
      break;
    }
  }
  return i;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ȑO�ɂ��̌`��̃f�I�L�V�X�����Ă��邩�`�F�b�N
 *
 *  @param  zw  �}�Ӄ��[�N
 *  @param  formno �`��
 *
 *  @retval TRUE  ����
 *  @retval FALSE ���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL checkSeeDeokisisuFormNo( const ZUKAN_SAVEDATA * zw, u32 formno )
{
  int i;
  for( i=0; i<ZUKANSAVE_DEOKISISU_FORM_NUM; i++ ){
    if( getDEOKISISUFormNo( zw, i ) == formno  ){
      return TRUE;
    }
  }
  return FALSE;
}



//----------------------------------------------------------------------------
/**
 *  @brief  �t�H�[���i���o�[�ݒ�@�p�b�N
 */
//-----------------------------------------------------------------------------
static void SetDeokisisuFormNo( ZUKAN_SAVEDATA * zw, u16 monsno, const POKEMON_PARAM* pp  )
{
  u8 form = PP_Get( pp, ID_PARA_form_no, NULL );
  u32 setno;

  if( monsno == MONSNO_DEOKISISU ){
    // �܂����Ă��Ȃ����`�F�b�N
    if( checkSeeDeokisisuFormNo( zw, form ) == FALSE ){
      setno = nowGetDeokisisuFormNum( zw ); // �ݒ�ʒu
      setDEOKISISUFormNo( zw, form, setno );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �f�I�L�V�X�t�H�����i���o�[�o�b�t�@������
 *
 *  @param  zw  �}�Ӄ��[�N
 */
//-----------------------------------------------------------------------------
static void InitDeokisisuFormNo( ZUKAN_SAVEDATA * zw )
{
  int i;
  for( i=0; i<ZUKANSAVE_DEOKISISU_FORM_NUM; i++ ){
    setDEOKISISUFormNo( zw, 0xf, i ); // 0xf�ŏ���������
  }
}


// u32�f�[�^3bit���ƎQ��
#define ZUKAN_3BIT_MAX  (0x7)
static inline u32 get_3bit_data( u32 data, u32 idx )
{
  return ( data >> (idx*3) ) & ZUKAN_3BIT_MAX;
}

static inline void set_3bit_data( u32* p_data, u32 idx, u32 data )
{
  GF_ASSERT( data < ZUKAN_3BIT_MAX );
  (*p_data) &= ~(ZUKAN_3BIT_MAX << (idx*3));
  (*p_data) |= (data << (idx*3));
}

//----------------------------------------------------------
/**
 * @brief ���g���̌��ݐݒ萔���擾
 */
//----------------------------------------------------------
static int get_rotom_turn_num( const ZUKAN_SAVEDATA * zw, u32 monsno )
{
  u32 form;
  int i, num;

  GF_ASSERT( monsno == MONSNO_ROTOMU );

  // ���Ă����Ȃ��Ƃ���0
  if( ZUKANSAVE_GetPokeSeeFlag( zw, monsno ) == FALSE ){
    return 0;
  }

  // �O�ƌ�낪���������ɂȂ�܂Ő��𐔂���
  num = 0;
  for( i=0; i<ZUKANSAVE_ROTOM_FORM_NUM; i++ ){
    form = get_3bit_data( zw->rotomu_turn, i );
    if( form != ZUKAN_3BIT_MAX ){
      num++;
    }else{
      break;
    }
  }

  return num;
}
//----------------------------------------------------------
/**
 * @brief ���g���ݒ�ς݂��`�F�b�N
 * @param form  0�`5
 */
//----------------------------------------------------------
static BOOL check_rotom_turn_set( const ZUKAN_SAVEDATA * zw, u32 monsno, u8 form )
{
  int i;
  u32 roop_num;
  u32 get_form;

  GF_ASSERT( (monsno == MONSNO_ROTOMU) );

  // ���Ă����Ȃ��Ƃ��͐ݒ肳��Ă���킯���Ȃ�
  if( ZUKANSAVE_GetPokeSeeFlag( zw, monsno ) == FALSE ){
    return FALSE;
  }

  // �ݒ萔���`�F�b�N
  // �P�񂵂����Ă��Ȃ��Ƃ��͂Q�ڂ̃r�b�g�́A
  // �_�~�[�f�[�^�Ȃ̂ŁA�����������Ȃ�
  roop_num = get_rotom_turn_num( zw, monsno );
  for( i=0; i<roop_num; i++ ){
    get_form = get_3bit_data( zw->rotomu_turn, i );
    if( get_form == form ){
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------
/**
 * @brief ���g�����������f�[�^�̍Ō�Ɍ`��ID��ݒ�
 */
//----------------------------------------------------------
static void SetZukanRotomTurn( ZUKAN_SAVEDATA * zw, u32 monsno, int form)
{
  int set_idx;
  GF_ASSERT( (monsno == MONSNO_ROTOMU) );

  // �ݒ�ς݂��`�F�b�N
  if( check_rotom_turn_set( zw, monsno, form ) ){
    return;
  }

  set_idx = get_rotom_turn_num( zw, monsno );
  if( set_idx < ZUKANSAVE_ROTOM_FORM_NUM ){
    set_3bit_data( &zw->rotomu_turn, set_idx, form );
  }
}




//----------------------------------------------------------
/**
 * @brief �����ȃ|�P�����̌��������f�[�^�̕ۑ�
 */
//----------------------------------------------------------
static void SetZukanSpecialTurn(ZUKAN_SAVEDATA * zw, u16 monsno, const POKEMON_PARAM* pp)
{
  int form;

  form = PP_Get( pp, ID_PARA_form_no, NULL );

  switch( monsno ){
  //  �A���m�[��
  case MONSNO_ANNOON:
    SetZukanUnknownTurn( zw, form, FALSE );
    break;
  // �~�m���b�`�E�~�m���X
  case MONSNO_MINOMUTTI:
  case MONSNO_MINOMADAMU:
    SetZukanThreeFormPokeTurn( zw, monsno, form );
    break;
  // �s�`���[
  case MONSNO_PITYUU:
    // ���A���A�M�U�~�~�̏��ɂ���
    if( form == 0 ){
      if( PP_GetSex(pp) == PTL_SEX_MALE ){
        form = 0;
      }else{
        form = 1;
      }
    }else{
      form = 2;
    }
    SetZukanThreeFormPokeTurn( zw, monsno, form );
    break;
  // �E�~�E�V�E�V�[�h���S
  case MONSNO_KARANAKUSI:
  case MONSNO_TORITODON:
    SetZukanTwoFormPokeTurn( zw, monsno, form );
    break;
  // �f�I�L�V�X
  case MONSNO_DEOKISISU:
    SetDeokisisuFormNo( zw, monsno, pp ); // �f�I�L�V�X�̃t�H������ۑ�
    break;
  // �V�F�C�~�E�M���e�B�i
  case MONSNO_SHEIMI:
  case MONSNO_GIRATHINA:
    SetZukanTwoFormPokeTurn( zw, monsno, form );
    break;
  // ���g��
  case MONSNO_ROTOMU:
    SetZukanRotomTurn( zw, monsno, form );
    break;

  default:
    break;
  }
}

//----------------------------------------------------------
/**
 * @brief �e�L�X�g�o�[�W�����z��C���f�b�N�X�������X�^�[�i���o�[����擾
 */
//----------------------------------------------------------
static inline int get_zukan_textversion_idx(u16 monsno)
{

#if FOREIGN_TEXT_FUNC
  return ZKN_WT_GetMonsNo_TEXTVERPokeNum( monsno );
#else
  return 0;
#endif

}
//----------------------------------------------------------
/**
 * @brief �e�L�X�g�o�[�W�����t���O�ݒ�
 */
//----------------------------------------------------------
static void SetZukanTextVersionUp(ZUKAN_SAVEDATA * zw, u16 monsno, u32 lang)
{
  int idx;
  int zkn_world_text_code=0;


//  idx = get_zukan_textversion_idx( monsno );
  idx = monsno;

#if FOREIGN_TEXT_FUNC
  zkn_world_text_code = ZKN_WT_GetLANG_Code_ZKN_WORLD_TEXT_Code( lang );
#endif

  // ���̃|�P�����ɊO����f�[�^�͂Ȃ�
//  if(idx == ZKN_WORLD_TEXT_POKE_NUM){ return; }

#if FOREIGN_TEXT_FUNC
  // ���̍��̊O����f�[�^�͂Ȃ�
  if(zkn_world_text_code == ZKN_WORLD_TEXT_NUM){
    return;
  }
#endif

  // �V���I�E�ȏ�̃|�P�����̊C�O�e�L�X�g�͖���
  if(monsno > POKEZUKAN_TEXT_POSSIBLE)
  {
    return;
  }

  zw->TextVersionUp[ idx ] |= 1 << zkn_world_text_code;
}
//----------------------------------------------------------
/**
 * @brief �ʏ�̃|�P�������ʎ擾
 */
//----------------------------------------------------------
static u32 get_normal_poke_sex( const ZUKAN_SAVEDATA * zw, u16 monsno, int first_second, HEAPID heapId )
{
  u32 sex_first, sex_second;
  u32 ret_sex,sex;
  POKEMON_PERSONAL_DATA *ppd;

  // �|�P��������肦�鐫�ʂ��擾
  ppd = POKE_PERSONAL_OpenHandle( monsno, 0, heapId );
  sex = PokePersonal_SexGet( ppd, monsno, 0);
  POKE_PERSONAL_CloseHandle( ppd );

  // ��΂ɕs���ȃ|�P������first�͕s����second�̓G���[��Ԃ�
  if( sex == PTL_SEX_UNKNOWN ){
    if( first_second == ZUKANSAVE_SEX_FIRST ){
      return PTL_SEX_UNKNOWN;
    }else{
      return ZUKANSAVE_GET_SEX_ERR;
    }
  }

  // ���Ɍ������ʂ��ݒ肳��Ă���̂��`�F�b�N���邽�߂�
  // �ǂ����̏ꍇ�ł��ŏ��Ɍ������ʂ��擾
  sex_first = check_sex_bit( zw, monsno, ZUKANSAVE_SEX_FIRST );

  // ���Ɍ������ʂ��擾����Ȃ玟�Ɍ������ʂ��擾
  if( first_second == ZUKANSAVE_SEX_SECOND ){
    sex_second = check_sex_bit( zw, monsno, ZUKANSAVE_SEX_SECOND );

    // �ŏ��Ɍ������ʂƈꏏ==�܂����Ă��Ȃ�
    if( sex_second == sex_first ){
      ret_sex = ZUKANSAVE_GET_SEX_ERR;
    }else{
      ret_sex = sex_second;
    }
  }else{
    ret_sex = sex_first;
  }

  return ret_sex;
}

//----------------------------------------------------------
/**
 * @brief �A���m�[��������(�߂܂���)���Ԏ擾
 */
//----------------------------------------------------------
static inline int get_unknown_form( const ZUKAN_SAVEDATA * zw, int idx ,BOOL get_f)
{
  if(get_f){
    return zw->UnknownGetTurn[ idx ];
  }
  return zw->UnknownTurn[ idx ];
}

//----------------------------------------------------------
/**
 * @brief �V�[�E�V�@�V�[�h���S�@�V�F�C�~�@�M���e�B�i���������Ԏ擾
 *���� 0or1����ō쐬���Ă��܂��B
 */
//----------------------------------------------------------
static int get_twoform_poke_see_form( const ZUKAN_SAVEDATA * zw, u32 monsno, int idx )
{
  const u8* arry;
  GF_ASSERT( (monsno == MONSNO_KARANAKUSI) || (monsno == MONSNO_TORITODON) || (monsno == MONSNO_SHEIMI) || (monsno == MONSNO_GIRATHINA) );
  GF_ASSERT( idx < 2 );

#if 0
  switch( monsno ){
  case MONSNO_KARANAKUSI:
    arry = &zw->SiiusiTurn;
    break;

  case MONSNO_TORITODON:
    arry = &zw->SiidorugoTurn;
    break;

  case MONSNO_SHEIMI:
    arry = &zw->syeimi_turn;
    break;

  case MONSNO_GIRATHINA:
    arry = &zw->giratyina_turn;
    break;
  }
#else
  arry = get_turn_arry(zw,monsno);
#endif
  return check_bit( arry, idx+1 );
}

//----------------------------------------------------------
/**
 * @brief ���g�����������Ԏ擾
 */
//----------------------------------------------------------
static int get_rotom_see_form( const ZUKAN_SAVEDATA * zw, u32 monsno, int idx )
{
  GF_ASSERT( (monsno == MONSNO_ROTOMU) );
  GF_ASSERT( idx < ZUKANSAVE_ROTOM_FORM_NUM );

  return get_3bit_data( zw->rotomu_turn, idx );
}

//----------------------------------------------------------
/**
 * @brief �~�m���b�`�A�~�m���X�A�s�`���[�̌��������Ԏ擾
 *���� 0�`2����ō쐬���Ă��܂��B
 */
//----------------------------------------------------------
static int get_threeform_poke_see_form( const ZUKAN_SAVEDATA * zw, u32 monsno, int idx )
{
  const u8* arry;
  GF_ASSERT( (monsno == MONSNO_MINOMUTTI) || (monsno == MONSNO_MINOMADAMU) || (monsno == MONSNO_PITYUU) );
  GF_ASSERT( idx < 3 );

#if 0
  switch( monsno ){
  case MONSNO_MINOMUTTI:
    arry = &zw->MinomuttiTurn;
    break;
  case MONSNO_MINOMADAMU:
    arry = &zw->MinomesuTurn;
    break;
  case MONSNO_PITYUU:
    arry = &zw->pityuu_turn;
    break;
  }
#else
  arry = get_turn_arry(zw,monsno);
#endif
  return check_bit2( arry, idx );
}


//----------------------------------------------------------------------------
/**
 *  @brief  �[���R�N�}�ӊ����ɕK�v�ȃ|�P�������`�F�b�N
 */
//-----------------------------------------------------------------------------
static BOOL check_ZenkokuCompMonsno( u16 monsno )
{
  int i;
  BOOL cut_check;
  static const u16 cut_check_monsno[ ZUKANSAVE_ZENKOKU_COMP_NOPOKE_NUM ] = {
    MONSNO_MYUU,
    MONSNO_SEREBHI,
    MONSNO_ZIRAATI,
    MONSNO_DEOKISISU,
    MONSNO_FIONE,
    MONSNO_MANAFI,
    MONSNO_DAAKURAI,
    MONSNO_SHEIMI,
    MONSNO_ARUSEUSU,
  };

  // �`�F�b�N���O�|�P�����`�F�b�N
  cut_check = TRUE;
  for( i=0; i<ZUKANSAVE_ZENKOKU_COMP_NOPOKE_NUM; i++ ){
    if( cut_check_monsno[ i ] == monsno ){
      cut_check = FALSE;
    }
  }
  return cut_check;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �W���E�g�}�ӊ����ɕK�v�ȃ|�P�������`�F�b�N
 */
//-----------------------------------------------------------------------------
static BOOL check_JohtoCompMonsno( u16 monsno )
{
  int i;
  BOOL cut_check;
  static const u16 cut_check_monsno[ZUKANSAVE_JOHTO_COMP_NOPOKE_NUM] = {
    MONSNO_MYUU,
    MONSNO_SEREBHI,
  };

  // �`�F�b�N���O�|�P�����`�F�b�N
  cut_check = TRUE;
  for( i=0; i<ZUKANSAVE_JOHTO_COMP_NOPOKE_NUM; i++ ){
    if( cut_check_monsno[ i ] == monsno ){
      cut_check = FALSE;
    }
  }
  return cut_check;
}

//============================================================================================
//
//      �������񑀍�p�̊O�����J�֐�
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief   �����񃏁[�N�̏�����
 * @param zw    �����񃏁[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
void ZUKANSAVE_Init(ZUKAN_SAVEDATA * zw)
{
  MI_CpuClear( zw, sizeof(ZUKAN_SAVEDATA));
  zw->zukan_magic    = MAGIC_NUMBER;
  zw->zenkoku_flag   = FALSE;

  // �߂܂������ԃt���O�Ƀ_�~�[�f�[�^�ݒ�
  MI_CpuFillFast( zw->UnknownTurn,    0xff, sizeof(u8)*POKEZUKAN_UNKNOWN_NUM );
  MI_CpuFillFast( zw->UnknownGetTurn, 0xff, sizeof(u8)*POKEZUKAN_UNKNOWN_NUM );
  zw->SiiusiTurn     = 0xff;
  zw->SiidorugoTurn  = 0xff;
  zw->MinomuttiTurn  = 0xff;
  zw->MinomesuTurn   = 0xff;

  zw->rotomu_turn    = 0xffffffff;
  zw->syeimi_turn    = 0xff;
  zw->giratyina_turn = 0xff;
  zw->pityuu_turn    = 0xff;

  // �f�I�L�V�X�t�H�����^�C�v�ۑ��̈�̏�����
  InitDeokisisuFormNo( zw );
}

//----------------------------------------------------------
/**
 * @brief �|�P���������܂������̎擾
 * @param zw    �����񃏁[�N�ւ̃|�C���^
 * @return  u16   �߂܂�����
 */
//----------------------------------------------------------
u16 ZUKANSAVE_GetPokeGetCount(const ZUKAN_SAVEDATA * zw)
{
  int i;
  int count;

  zukan_incorrect(zw);
  count= 0;
  for( i=1; i<=MONSNO_END; i++ ){
    if( ZUKANSAVE_GetPokeGetFlag( zw, i ) == TRUE ){
      count ++;
    }
  }
  return count;

  // �����ł����A�f�I�L�V�X�t���O��get_flag�̌��ɓ��ꂽ�̂ŁA�g�p�ł��܂���
//  return count_bit(zw->get_flag, POKEZUKAN_ARRAY_LEN);

}

//----------------------------------------------------------
/**
 * @brief �|�P���������������̎擾
 * @param zw    �����񃏁[�N�ւ̃|�C���^
 * @return  u16   ��������
 */
//----------------------------------------------------------
u16 ZUKANSAVE_GetPokeSeeCount(const ZUKAN_SAVEDATA * zw)
{
  int i;
  int count;

  zukan_incorrect(zw);
  count= 0;
  for( i=1; i<=MONSNO_END; i++ ){
    if( ZUKANSAVE_GetPokeSeeFlag( zw, i ) == TRUE ){
      count ++;
    }
  }
  return count;

  // �����ł����A�f�I�L�V�X�t���O��get_flag�̌��ɓ��ꂽ�̂ŁA�g�p�ł��܂���
//  return count_bit(zw->see_flag, POKEZUKAN_ARRAY_LEN);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���݂̐}��Ӱ�ނ̃|�P������߂܂������擾
 *  @param  zw    �}�Ӄ��[�N�ւ̃|�C���^
 *  @return �߂܂�����
 */
//-----------------------------------------------------------------------------
u16 ZUKANSAVE_GetZukanPokeGetCount(const ZUKAN_SAVEDATA * zw)
{
  if( ZUKANSAVE_GetZenkokuZukanFlag( zw ) ){
    return ZUKANSAVE_GetPokeGetCount( zw );
  }
  return ZUKANSAVE_GetJohtoPokeGetCount( zw );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���݂̐}��Ӱ�ނ̃|�P�������������擾
 *  @param  zw    �}�Ӄ��[�N�ւ̃|�C���^
 *  @return ������
 */
//-----------------------------------------------------------------------------
u16 ZUKANSAVE_GetZukanPokeSeeCount(const ZUKAN_SAVEDATA * zw)
{
  if( ZUKANSAVE_GetZenkokuZukanFlag( zw ) ){
    return ZUKANSAVE_GetPokeSeeCount( zw );
  }
  return ZUKANSAVE_GetJohtoPokeSeeCount( zw );
}

//----------------------------------------------------------
/**
 * �y�W���E�g�z
 * @brief �|�P���������܂������̎擾
 * @param zw    �����񃏁[�N�ւ̃|�C���^
 * @return  u16   �߂܂�����
 */
//----------------------------------------------------------
u16 ZUKANSAVE_GetJohtoPokeGetCount(const ZUKAN_SAVEDATA * zw)
{
  u16 * buf;
  u16 i;
  u16 num=0;

  zukan_incorrect(zw);


#if ZUKAN_ISSHU_TABLE_NONE
  for( i=1; i<=MONSNO_END; i++ ){
    if( ZUKANSAVE_GetPokeGetFlag( zw, i ) == TRUE ){
      num++;
    }
  }
#else
  buf = PokeZukanNo2JohtoNoTblLoad();   // @TODO �}��NO�e�[�u�����܂����݂��Ȃ�
  num = 0;

  for( i=1; i<=MONSNO_END; i++ ){
    if( ZUKANSAVE_GetPokeGetFlag( zw, i ) == TRUE ){
      // �W���E�g�}�ӂɂ��邩�`�F�b�N
      if( buf[i] != 0 ){
        num++;
      }
    }
  }

  GFL_HEAP_FreeMemory( buf );
#endif

  return num;

}

//----------------------------------------------------------
/**
 * �y�W���E�g�z
 * @brief �|�P���������������̎擾
 * @param zw    �����񃏁[�N�ւ̃|�C���^
 * @return  u16   ��������
 */
//----------------------------------------------------------
u16 ZUKANSAVE_GetJohtoPokeSeeCount(const ZUKAN_SAVEDATA * zw)
{
  u16 * buf;
  u16 i;
  u16 num=0;

  zukan_incorrect(zw);

#if ZUKAN_ISSHU_TABLE_NONE

  for( i=1; i<=MONSNO_END; i++ ){
    if( ZUKANSAVE_GetPokeSeeFlag( zw, i ) == TRUE ){
       num++;
    }
  }
#else
  buf = PokeZukanNo2JohtoNoTblLoad();
  num = 0;

  for( i=1; i<=MONSNO_END; i++ ){
    if( ZUKANSAVE_GetPokeSeeFlag( zw, i ) == TRUE ){
      // �W���E�g�}�ӂɂ��邩�`�F�b�N
      if( buf[i] != 0 ){
        num++;
      }
    }
  }

  GFL_HEAP_FreeMemory( buf );
#endif
  return num;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �S���}�ӂ������������`�F�b�N����
 *
 *  @param  zw  �}�Ӄ��[�N
 *
 *  @retval TRUE  ����
 *  @retval FALSE ������
 */
//-----------------------------------------------------------------------------
BOOL ZUKANSAVE_CheckZenkokuComp(const ZUKAN_SAVEDATA * zw)
{
  u16 num;

  // �[���R�N�}�ӊ����ɕK�v�ȃ|�P���������C�߂܂�����
  num = ZUKANSAVE_GetZenkokuGetCompCount( zw );

  if( num >= ZUKANSAVE_ZENKOKU_COMP_NUM ){
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �W���E�g�}�ӂ������������`�F�b�N����
 *
 *  @param  zw  �}�Ӄ��[�N
 *
 *  @retval TRUE  ����
 *  @retval FALSE ������
 */
//-----------------------------------------------------------------------------
BOOL ZUKANSAVE_CheckJohtoComp(const ZUKAN_SAVEDATA * zw)
{
  u16 num;

  // �W���E�g�}�ӊ����ɕK�v�ȃ|�P���������C�߂܂�����
  num = ZUKANSAVE_GetJohtoGetCompCount( zw );

  if( num >= ZUKANSAVE_JOHTO_COMP_NUM ){
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �[���R�N�}�Ӂ@�����ɕK�v�ȃ|�P������߂܂�����
 *
 *  @param  zw    �}�Ӄ��[�N
 *  @return �����ɕK�v�ȃ|�P������߂܂�����
 */
//-----------------------------------------------------------------------------
u16 ZUKANSAVE_GetZenkokuGetCompCount(const ZUKAN_SAVEDATA * zw)
{
  int i;
  u16 num;
  // �G���t�B�@�}�i�t�B�@�_�[�N�@�G���E�X�@�A�E�X
  // �ȊO��߂܂��Ă�����OK
  num = 0;
  for( i=1; i<=ZUKANSAVE_ZENKOKU_MONSMAX; i++ ){
    if( ZUKANSAVE_GetPokeGetFlag( zw, i ) == TRUE ){
      if( check_ZenkokuCompMonsno( i ) == TRUE ){
        num ++;
      }
    }
  }
  return num;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �W���E�g�}�Ӂ@�����ɕK�v�ȃ|�P������߂܂�����
 *
 *  @param  zw    �}�Ӄ��[�N
 *  @return �����ɕK�v�ȃ|�P��������������
 */
//-----------------------------------------------------------------------------
u16 ZUKANSAVE_GetJohtoGetCompCount(const ZUKAN_SAVEDATA * zw)
{
  u16 * buf;
  u16 i;
  u16 num=0;


#if ZUKAN_ISSHU_TABLE_NONE

  for( i=1; i<=MONSNO_END; i++ ){
    if( ZUKANSAVE_GetPokeGetFlag( zw, i ) == TRUE ){
      num++;
    }
  }

#else

  buf = PokeZukanNo2JohtoNoTblLoad();
  // �~���E�ƃZ���r�B
  // �ȊO��߂܂��Ă�����OK
  for( i=1; i<=ZUKANSAVE_ZENKOKU_MONSMAX; i++ ){
    if( ZUKANSAVE_GetPokeGetFlag( zw, i ) == TRUE ){
      if( buf[i] != 0 ){
        if( check_JohtoCompMonsno( i ) == TRUE ){
          num++;
        }
      }
    }
  }
  OS_Printf("�W���E�g�R���v���[�g�`�F�b�N num = %d\n",num);
  GFL_HEAP_FreeMemory( buf );

#endif

  return num;
}

//----------------------------------------------------------
/**
 * @brief �|�P������߂܂������ǂ����̃t���O�擾
 * @param zw    �����񃏁[�N�ւ̃|�C���^
 * @param monsno  �|�P�����i���o�[
 * @return  BOOL  �t���O����
 */
//----------------------------------------------------------
BOOL ZUKANSAVE_GetPokeGetFlag(const ZUKAN_SAVEDATA * zw, u16 monsno)
{
  zukan_incorrect(zw);
  if (monsno_incorrect(monsno)) {
    return FALSE;
  }
  if (check_get_bit(zw, monsno) && check_see_bit(zw, monsno)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

//----------------------------------------------------------
/**
 * @brief �|�P���������������ǂ����̃t���O�擾
 * @param zw    �����񃏁[�N�ւ̃|�C���^
 * @param monsno  �|�P�����i���o�[
 * @return  BOOL  �t���O����
 */
//----------------------------------------------------------
BOOL ZUKANSAVE_GetPokeSeeFlag(const ZUKAN_SAVEDATA * zw, u16 monsno)
{
  zukan_incorrect(zw);
  if (monsno_incorrect(monsno)) {
    return FALSE;
  }
  return check_see_bit(zw, monsno);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ����|�P�����̌������擾
 *
 *  @param  zw      �}�Ӄ��[�N�ւ̃|�C���^
 *  @param  random_poke �擾����|�P����������
 *
 *  @return ������
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeRandomFlag(const ZUKAN_SAVEDATA * zw, u8 random_poke)
{
  u32 random;

  zukan_incorrect(zw);

  switch( random_poke ){
  case ZUKANSAVE_RANDOM_PACHI:    // �p�b�`�[��������
    random = zw->PachiRandom;
    break;

  default:
    GF_ASSERT_MSG( 0, "random_poke = %d\n", random_poke );
    break;
  }

  return random;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �|�P������\������Ƃ��̐��ʎ擾
 *
 *  @param  zw        �����񃏁[�N�ւ̃|�C���^
 *  @param  monsno      �|�P�����i���o�[
 *  @param  first_second  �ŏ��Ɍ������ʂ����Ɍ������ʂ�
 *
 *  @retval PTL_SEX_MALE    �I�X
 *  @retval PTL_SEX_FEMALE    ���X
 *  @retval PTL_SEX_UNKNOWN   �s��
 *  @retval ZUKANSAVE_GET_SEX_ERR �������ʂ��܂�����
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeSexFlag(const ZUKAN_SAVEDATA * zw, u16 monsno, int first_second, HEAPID heapId )
{
  zukan_incorrect(zw);
  if (monsno_incorrect(monsno)) {
    return ZUKANSAVE_GET_SEX_ERR;
  }
  // ���ĂȂ��Ƃ��̓G���[�R�[�h��Ԃ�
  if( !check_see_bit(zw, monsno) ){
    return ZUKANSAVE_GET_SEX_ERR;
  }

  // �m�[�}���|�P����
  return get_normal_poke_sex( zw, monsno, first_second, heapId );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �A���m�[�����������ݒ�
 *
 *  @param  zw    �}�Ӄ��[�N
 *  @param  count �A���m�[������  (0�`27)
 *  @param  get_f FALSE:����������,TRUE:�ߊl�ς�
 *
 *  @retval �A���m�[���t�H�[���i���o�[
 *  @retval ZUKANSAVE_GET_SEX_ERR ������(�ߊl����)���Ȃ�
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeAnnoonForm(const ZUKAN_SAVEDATA * zw, int count, BOOL get_f)
{
  zukan_incorrect(zw);
  // ���̐��̃A���m�[���t�H���������Ă��邩�`�F�b�N
  if( get_zukan_unknown_turn_num( zw, get_f ) <= count ){
    return ZUKANSAVE_GET_SEX_ERR;
  }

  // �t�H�����i���o�[��Ԃ�
  return get_unknown_form( zw, count , get_f );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �A���m�[��������(�߂܂���)��
 *
 *  @param  zw    �}�Ӄ��[�N
 *
 *  @return ������
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeAnnoonNum(const ZUKAN_SAVEDATA * zw, BOOL get_f)
{
  zukan_incorrect(zw);
  return get_zukan_unknown_turn_num( zw, get_f );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �V�[�E�V�t�H�����擾
 *
 *  @param  zw    �}�Ӄ��[�N
 *  @param  count ���������Ԑ�
 *
 *  @retval ���̏��ԂɌ������t�H����
 *  @retval ZUKANSAVE_GET_SEX_ERR �����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeSiiusiForm(const ZUKAN_SAVEDATA * zw, int count)
{
  zukan_incorrect(zw);

  // ���̐��̃V�[�E�V�t�H���������Ă��邩�`�F�b�N
  if( get_twoform_poke_turn_num( zw, MONSNO_KARANAKUSI ) <= count ){
    return ZUKANSAVE_GET_SEX_ERR;
  }
  return get_twoform_poke_see_form( zw, MONSNO_KARANAKUSI, count );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �V�[�E�V�̃t�H������������
 *
 *  @param  zw  �}�Ӄ��[�N
 *
 *  @return �t�H������������
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeSiiusiSeeNum(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return get_twoform_poke_turn_num( zw, MONSNO_KARANAKUSI );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �V�[�h���S�t�H�����i���o�[�擾
 *
 *  @param  zw    �}�Ӄ��[�N
 *  @param  count ���������Ԑ�
 *
 *  @retval ���̏��ԂɌ������t�H�����i���o�[
 *  @retval ZUKANSAVE_GET_SEX_ERR �����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeSiidorugoForm(const ZUKAN_SAVEDATA * zw, int count)
{
  zukan_incorrect(zw);

  // ���̐��̃V�[�h���S�t�H���������Ă��邩�`�F�b�N
  if( get_twoform_poke_turn_num( zw, MONSNO_TORITODON ) <= count ){
    return ZUKANSAVE_GET_SEX_ERR;
  }
  return get_twoform_poke_see_form( zw, MONSNO_TORITODON, count );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �V�[�h���S�̃t�H������������
 *
 *  @param  zw  �}�Ӄ��[�N
 *
 *  @return �t�H������������
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeSiidorugoSeeNum(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return get_twoform_poke_turn_num( zw, MONSNO_TORITODON );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �~�m���b�`�t�H�����i���o�[�擾
 *
 *  @param  zw    �}�Ӄ��[�N
 *  @param  count ���������Ԑ�
 *
 *  @retval ���̏��ԂɌ������t�H�����i���o�[
 *  @retval ZUKANSAVE_GET_SEX_ERR �����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeMinomuttiForm(const ZUKAN_SAVEDATA * zw, int count)
{
  zukan_incorrect(zw);

  // ���̐��̃~�m���b�`�t�H���������Ă��邩�`�F�b�N
  if( get_threeform_poke_turn_num( zw, MONSNO_MINOMUTTI ) <= count ){
    return ZUKANSAVE_GET_SEX_ERR;
  }
  return get_threeform_poke_see_form( zw, MONSNO_MINOMUTTI, count );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �~�m���b�`�̃t�H������������
 *
 *  @param  zw  �}�Ӄ��[�N
 *
 *  @return �t�H������������
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeMinomuttiSeeNum(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return get_threeform_poke_turn_num( zw, MONSNO_MINOMUTTI );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �~�m���X�n�`�t�H�����i���o�[�擾
 *
 *  @param  zw    �}�Ӄ��[�N
 *  @param  count ���������Ԑ�
 *
 *  @retval ���̏��ԂɌ������t�H�����i���o�[
 *  @retval ZUKANSAVE_GET_SEX_ERR �����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeMinomesuForm(const ZUKAN_SAVEDATA * zw, int count)
{
  zukan_incorrect(zw);

  // ���̐��̃~�m���X�t�H���������Ă��邩�`�F�b�N
  if( get_threeform_poke_turn_num( zw, MONSNO_MINOMADAMU ) <= count ){
    return ZUKANSAVE_GET_SEX_ERR;
  }
  return get_threeform_poke_see_form( zw, MONSNO_MINOMADAMU, count );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �~�m���X�̃t�H������������
 *
 *  @param  zw  �}�Ӄ��[�N
 *
 *  @return �t�H������������
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeMinomesuSeeNum(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return get_threeform_poke_turn_num( zw, MONSNO_MINOMADAMU );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �s�`���[�n�`�t�H�����i���o�[�擾
 *
 *  @param  zw    �}�Ӄ��[�N
 *  @param  count ���������Ԑ�
 *
 *  @retval ���̏��ԂɌ������t�H�����i���o�[
 *  @retval ZUKANSAVE_GET_SEX_ERR �����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokePityuuForm(const ZUKAN_SAVEDATA * zw, int count)
{
  zukan_incorrect(zw);

  // ���̐��̃~�m���X�t�H���������Ă��邩�`�F�b�N
  if( get_threeform_poke_turn_num( zw, MONSNO_PITYUU ) <= count ){
    return ZUKANSAVE_GET_SEX_ERR;
  }
  return get_threeform_poke_see_form( zw, MONSNO_PITYUU, count );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �s�`���[�̃t�H������������
 *
 *  @param  zw  �}�Ӄ��[�N
 *
 *  @return �t�H������������
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokePityuuSeeNum(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return get_threeform_poke_turn_num( zw, MONSNO_PITYUU );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �f�I�L�V�X�t�H�����@���擾����
 *
 *  @param  zw  �}�Ӄ��[�N
 *  @param  count�J�E���g
 *
 *  @return �f�I�L�V�X�t�H�����i���o�[
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeDeokisisuForm(const ZUKAN_SAVEDATA * zw, int count)
{
  zukan_incorrect(zw);
  return getDEOKISISUFormNo( zw, count );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �f�I�L�V�X�t�H�����@���������擾
 *
 *  @param  zw  �}�Ӄ��[�N
 *
 *  @return ������
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeDeokisisuFormSeeNum(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return nowGetDeokisisuFormNum( zw );
}


//----------------------------------------------------------
/**
 * @brief ������f�[�^�Z�b�g�i�������j
 * @param zw    �����񃏁[�N�ւ̃|�C���^
 * @param pp    �������|�P������POKEMON_PARAM�ւ̃|�C���^
 */
//----------------------------------------------------------
void ZUKANSAVE_SetPokeSee(ZUKAN_SAVEDATA * zw, const POKEMON_PARAM * pp)
{
  u16 monsno = PP_Get(pp, ID_PARA_monsno, NULL);
  u32 rand = PP_Get(pp, ID_PARA_personal_rnd, NULL);
  u32 sex = PP_GetSex(pp);
  u32 first_sex;  // �ŏ��Ɍ�������

  zukan_incorrect(zw);
  if (monsno_incorrect(monsno)) {
    return;
  }
  if (!check_see_bit(zw, monsno)) {
    SetZukanRandom(zw, monsno, rand);     // �ő̗���
    set_sex_bit( zw, sex, ZUKANSAVE_SEX_FIRST, monsno );      // �ŏ��Ɍ�������
  }else{
    // �ŏ��Ɍ������ʂƈႤ�Ƃ��Z�J���h�ɓo�^����
    first_sex = check_sex_bit( zw, monsno, ZUKANSAVE_SEX_FIRST );
    if( first_sex != sex ){
      set_sex_bit( zw, sex, ZUKANSAVE_SEX_SECOND, monsno );     // ���Ɍ�������
    }
  }
  SetZukanSpecialTurn(zw, monsno, pp);    // ����|�P�����̌��������Ԑݒ�
  set_see_bit(zw, monsno);
}

//----------------------------------------------------------
/**
 * @brief ������f�[�^�Z�b�g�i�߂܂����j
 * @param zw    �����񃏁[�N�ւ̃|�C���^
 * @param pp    �߂܂����|�P������POKEMON_PARAM�ւ̃|�C���^
 */
//----------------------------------------------------------
void ZUKANSAVE_SetPokeGet(ZUKAN_SAVEDATA * zw, const POKEMON_PARAM * pp)
{
  u16 monsno = PP_Get(pp, ID_PARA_monsno, NULL);
  u32 lang = PP_Get(pp, ID_PARA_country_code, NULL);
  u32 rand = PP_Get(pp, ID_PARA_personal_rnd, NULL);
  u32 sex = PP_GetSex(pp);
  u32 first_sex;  // �ŏ��Ɍ�������

  zukan_incorrect(zw);
  if (monsno_incorrect(monsno)) {
    return;
  }
  if (!check_see_bit(zw, monsno)) {
    SetZukanRandom(zw, monsno, rand);     // �ő̗���
    set_sex_bit( zw, sex, ZUKANSAVE_SEX_FIRST, monsno );      // �\������
  }else{
    // �ŏ��Ɍ������ʂƈႤ�Ƃ��Z�J���h�ɓo�^����
    first_sex = check_sex_bit( zw, monsno, ZUKANSAVE_SEX_FIRST );
    if( first_sex != sex ){
      set_sex_bit( zw, sex, ZUKANSAVE_SEX_SECOND, monsno );     // �\������
    }
  }
  SetZukanSpecialTurn(zw, monsno, pp);    // ����|�P�����̌��������Ԑݒ�
  //�A���m�[���߂܂������Ԑݒ�
  if(monsno == MONSNO_ANNOON){
    SetZukanUnknownTurn( zw, PP_Get( pp, ID_PARA_form_no, NULL), TRUE );
  }

  SetZukanTextVersionUp(zw, monsno, lang);  // ���R�[�h
  // �ꍑ��ȊO���o�^���ꂽ�ꍇ�A����o�^�}�X�^�[�t���O���Z�b�g
  if( lang != PM_LANG ){
    ZUKANSAVE_SetTextVersionUpMasterFlag( zw );
  }
  set_get_bit(zw, monsno);
  set_see_bit(zw, monsno);
}


//----------------------------------------------------------
/**
 * @brief �S�������񃂁[�h���Z�b�g
 * @param zw    �����񃏁[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
void ZUKANSAVE_SetZenkokuZukanFlag(ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  zw->zenkoku_flag = TRUE;
}

//----------------------------------------------------------
/**
 * @brief �S�������񃂁[�h���ǂ����H�̖₢���킹
 * @param zw    �����񃏁[�N�ւ̃|�C���^
 * @retval  TRUE  �S�������񃂁[�h�I�[�v��
 * @retval  FALSE �W���E�g�����񃂁[�h
 */
//----------------------------------------------------------
BOOL ZUKANSAVE_GetZenkokuZukanFlag(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return zw->zenkoku_flag;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �O���t�B�b�N�o�[�W�����p�@�\�g���t���O�擾
 *
 *  @param  zw    �����񃏁[�N�ւ̃|�C���^
 *
 *  @retval TRUE  �o�[�W�����A�b�v����
 *  @retval FALSE �o�[�W�����A�b�v������
 */
//-----------------------------------------------------------------------------
BOOL ZUKANSAVE_GetGraphicVersionUpFlag(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return zw->GraphicVersionUp;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �O���t�B�b�N�o�[�W�����p�@�\�g���t���O�ݒ�
 *
 *  @param  zw    �}�Ӄ��[�N�ւ̃|�C���^
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
void ZUKANSAVE_SetGraphicVersionUpFlag(ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  zw->GraphicVersionUp = 1;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �}�Ӄe�L�X�g�o�[�W�����`�F�b�N
 *
 *  @param  zw
 *  @param  monsno
 *  @param  country_code    ���R�[�h
 *
 *  @retval TRUE  ���̍��̃o�[�W������\�����Ă悢
 *  @retval FALSE ���̍��̃o�[�W������\�����Ă͂����Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL ZUKANSAVE_GetTextVersionUpFlag(const ZUKAN_SAVEDATA * zw, u16 monsno, u32 country_code)
{
  int idx;


  GF_ASSERT( country_code < 8 );    // ���R�[�h�ő吔

  zukan_incorrect(zw);
  // �e�L�X�g�o�[�W�����̂���|�P�������`�F�b�N
//  idx = get_zukan_textversion_idx(monsno);
//  if( idx == ZKN_WORLD_TEXT_POKE_NUM ){ return FALSE; }
  idx = monsno;


#if FOREIGN_TEXT_FUNC
  // �O����e�L�X�g�p�i���o�[�ɕϊ�
  country_code = ZKN_WT_GetLANG_Code_ZKN_WORLD_TEXT_Code( country_code );
#endif

  // �V���I�E�ȏ�̃|�P�����̊C�O�R�[�h�`�F�b�N�͑S��FALSE
  if( monsno > POKEZUKAN_TEXT_POSSIBLE ){
    return FALSE;
  }

  // ���̌���̃t���O�������Ă��邩�`�F�b�N
  if( (zw->TextVersionUp[idx] & (1<<country_code)) != 0 ){
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �}�ӊO����o�[�W�����@Master�t���O
 *
 *  @param  zw  �}�Ӄ��[�N
 *  ���̃t���O�������Ă��Ȃ��ƁA�O����}�ӂ̓I�[�v������Ȃ�
 */
//-----------------------------------------------------------------------------
void ZUKANSAVE_SetTextVersionUpMasterFlag( ZUKAN_SAVEDATA * zw )
{
  zw->TextVersionUpMasterFlag = TRUE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �}�ӊO����o�[�W�����@Master�t���O�擾
 *
 *  @param  zw  �}�Ӄ��[�N
 *
 *  @retval TRUE  �o�[�W�����A�b�v�ς�
 *  @retval FALSE �o�[�W�����A�b�v����ĂȂ�
 */
//-----------------------------------------------------------------------------
BOOL ZUKANSAVE_GetTextVersionUpMasterFlag(const ZUKAN_SAVEDATA * zw)
{
  return zw->TextVersionUpMasterFlag;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �}�ӕێ��t���O�擾
 *
 *  @param  zw  �}�Ӄ��[�N�ւ̃|�C���^
 *
 *  @retval TRUE  �ێ�
 *  @retval FALSE �ێ����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL ZUKANSAVE_GetZukanGetFlag(const ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  return zw->zukan_get;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �}�ӕێ��t���O�ݒ�
 *
 *  @param  zw  �}�Ӄ��[�N
 *
 *  @return none
 */
//-----------------------------------------------------------------------------
void ZUKANSAVE_SetZukanGetFlag(ZUKAN_SAVEDATA * zw)
{
  zukan_incorrect(zw);
  zw->zukan_get = TRUE;
}

//============================================================================================
//
//  �Z�[�u�f�[�^�擾�̂��߂̊֐�
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief ������ԃf�[�^�ւ̃|�C���^�擾
 * @param sv      �Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @return  ZUKAN_SAVEDATA  �����񃏁[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
ZUKAN_SAVEDATA * GAMEDATA_GetZukanSave( GAMEDATA *gamedata )
{
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork( gamedata );
  ZUKAN_SAVEDATA    *zs = SaveControl_DataPtrGet( sv, GMDATA_ID_ZUKAN );

  return zs;
}

//============================================================================================
//============================================================================================
// �t�H��������|�P�����̌��������Ԃ����Ă�
static void set_debag_special_poke_see_no( ZUKAN_SAVEDATA * zw, int monsno )
{
  int i;

  switch( monsno ){
  case MONSNO_ANNOON:
    for( i=0; i<POKEZUKAN_UNKNOWN_NUM; i++ ){
      SetZukanUnknownTurn( zw, i, FALSE );
    }
    break;

  case MONSNO_MINOMUTTI:
  case MONSNO_MINOMADAMU:
  case MONSNO_PITYUU:
    for( i=ZUKANSAVE_MINOMUSHI_FORM_NUM - 1; i>=0; i-- ){
      SetZukanThreeFormPokeTurn( zw, monsno, i );
    }
    break;

  case MONSNO_KARANAKUSI:
  case MONSNO_TORITODON:
  case MONSNO_GIRATHINA:
    for( i=ZUKANSAVE_UMIUSHI_FORM_NUM - 1; i>=0; i-- ){
      SetZukanTwoFormPokeTurn( zw, monsno, i );
    }
    break;

  case MONSNO_SHEIMI:   // �V�F�C�~���������ړo�^�����t�ɂ���
    for( i=0; i<ZUKANSAVE_UMIUSHI_FORM_NUM; i++ ){
      setnum_bit( &zw->syeimi_turn, i, i+1 );
    }
    break;

  case MONSNO_DEOKISISU:
    for( i=ZUKANSAVE_DEOKISISU_FORM_NUM - 1; i>=0; i-- ){
      setDEOKISISUFormNo( zw, i, ZUKANSAVE_DEOKISISU_FORM_NUM-(i+1) );
    }
    break;

  case MONSNO_ROTOMU:
    for( i=0; i<ZUKANSAVE_ROTOM_FORM_NUM; i++ ){
      SetZukanRotomTurn( zw, monsno, i );
    }
    break;
  }
}

// ���ʐݒ�
static void set_debug_poke_sex( ZUKAN_SAVEDATA * zw, int monsno, HEAPID heapId )
{
  int sex;
  POKEMON_PERSONAL_DATA *ppd;

  // �|�P��������肦�鐫�ʂ��擾
  ppd = POKE_PERSONAL_OpenHandle( monsno, 0, heapId );
  sex = PokePersonal_SexGet( ppd, monsno, 0);
  POKE_PERSONAL_CloseHandle( ppd );

  if( sex == PTL_SEX_MALE ){
    // ���ʐݒ�
    set_sex_bit( zw, PTL_SEX_MALE, ZUKANSAVE_SEX_FIRST, monsno );
    return;
  }
  if( sex == PTL_SEX_FEMALE ){
    // ���ʐݒ�
    set_sex_bit( zw, PTL_SEX_FEMALE, ZUKANSAVE_SEX_FIRST, monsno );
    return;
  }
  if( sex == PTL_SEX_UNKNOWN ){
    // ���ʐݒ�
    set_sex_bit( zw, PTL_SEX_UNKNOWN, ZUKANSAVE_SEX_FIRST, monsno );
    return;
  }

  // ���ʐݒ�
  if( monsno % 2 ){
    set_sex_bit( zw, PTL_SEX_MALE, ZUKANSAVE_SEX_FIRST, monsno );
    set_sex_bit( zw, PTL_SEX_FEMALE, ZUKANSAVE_SEX_SECOND, monsno );
  }else{
    set_sex_bit( zw, PTL_SEX_FEMALE, ZUKANSAVE_SEX_FIRST, monsno );
    set_sex_bit( zw, PTL_SEX_MALE, ZUKANSAVE_SEX_SECOND, monsno );
  }
}

//----------------------------------------------------------
/**
 * @brief �f�o�b�O�p�F������f�[�^����
 * @param zw      �����񃏁[�N�ւ̃|�C���^
 * @param option
 */
//----------------------------------------------------------
void Debug_ZUKANSAVE_Make(ZUKAN_SAVEDATA * zw, int start, int end, BOOL see_flg, HEAPID heapId)
{
  int i;
  zukan_incorrect(zw);

  GF_ASSERT( start > 0 );
  GF_ASSERT( end <= MONSNO_END );

  for (i = start; i <= end; i++) {
    if( see_flg == TRUE ){
      set_see_bit(zw, i);
    }else{

      set_get_bit(zw, i);
      set_see_bit(zw, i);

      // �O���}�Ӑݒ�
      SetZukanTextVersionUp(zw, i, LANG_JAPAN);
      SetZukanTextVersionUp(zw, i, LANG_ENGLISH);
      SetZukanTextVersionUp(zw, i, LANG_FRANCE);
      SetZukanTextVersionUp(zw, i, LANG_ITALY);
      SetZukanTextVersionUp(zw, i, LANG_GERMANY);
      SetZukanTextVersionUp(zw, i, LANG_SPAIN);
    }

    // �������ݒ�
    SetZukanRandom( zw, i, 0 );

    // ���ꌩ�������Ԑݒ�
    set_debag_special_poke_see_no( zw, i );

    // ����
    set_debug_poke_sex( zw, i, heapId);
  }

  // �O���t�B�b�N�o�[�W�����A�b�v
  ZUKANSAVE_SetGraphicVersionUpFlag(zw);
  ZUKANSAVE_SetTextVersionUpMasterFlag(zw);
}

void Debug_ZUKANSAVE_Make_PM_LANG(ZUKAN_SAVEDATA * zw, int start, int end, BOOL see_flg, HEAPID heapId)
{
  int i;
  zukan_incorrect(zw);

  GF_ASSERT( start > 0 );
  GF_ASSERT( end <= MONSNO_END );

  for (i = start; i <= end; i++) {
    if( see_flg == TRUE ){
      set_see_bit(zw, i);
    }else{

      set_get_bit(zw, i);
      set_see_bit(zw, i);

      // �O���}�Ӑݒ�
      SetZukanTextVersionUp(zw, i, PM_LANG);
    }

    // �������ݒ�
    SetZukanRandom( zw, i, 0 );

    // ���ꌩ�������Ԑݒ�
    set_debag_special_poke_see_no( zw, i );

    // ����
    set_debug_poke_sex( zw, i, heapId );
  }

  // �O���t�B�b�N�o�[�W�����A�b�v
  ZUKANSAVE_SetGraphicVersionUpFlag(zw);
}

void Debug_ZUKANSAVE_LangSetRand( ZUKAN_SAVEDATA * zw, int start, int end )
{
  int i;

  zukan_incorrect(zw);

  GF_ASSERT( start > 0 );
  GF_ASSERT( end <= MONSNO_END );

  for( i=start; i<=end; i++ ){
    if( check_get_bit( zw, i ) == FALSE ){ continue; }
    if(  GFUser_GetPublicRand(2) ){
      SetZukanTextVersionUp( zw, i, LANG_ENGLISH );
    }
    if( GFUser_GetPublicRand(2) ){
      SetZukanTextVersionUp( zw, i, LANG_FRANCE );
    }
    if( GFUser_GetPublicRand(2) ){
      SetZukanTextVersionUp( zw, i, LANG_ITALY );
    }
    if( GFUser_GetPublicRand(2) ){
      SetZukanTextVersionUp( zw, i, LANG_GERMANY );
    }
    if( GFUser_GetPublicRand(2) ){
      SetZukanTextVersionUp( zw, i, LANG_SPAIN );
    }
  }
  ZUKANSAVE_SetTextVersionUpMasterFlag(zw);
}

void Debug_ZUKANSAVE_LangSetAll( ZUKAN_SAVEDATA * zw, int start, int end )
{
  int i;

  zukan_incorrect(zw);

  GF_ASSERT( start > 0 );
  GF_ASSERT( end <= MONSNO_END );

  for( i=start; i<=end; i++ ){
    if( check_get_bit( zw, i ) == FALSE ){ continue; }
    SetZukanTextVersionUp( zw, i, LANG_ENGLISH );
    SetZukanTextVersionUp( zw, i, LANG_FRANCE );
    SetZukanTextVersionUp( zw, i, LANG_ITALY );
    SetZukanTextVersionUp( zw, i, LANG_GERMANY );
    SetZukanTextVersionUp( zw, i, LANG_SPAIN );
  }
  ZUKANSAVE_SetTextVersionUpMasterFlag(zw);
}

void Debug_ZUKANSAVE_LangSet( ZUKAN_SAVEDATA * zw, int start, int end, u8 lang )
{
  int i;

  zukan_incorrect(zw);

  GF_ASSERT( start > 0 );
  GF_ASSERT( end <= MONSNO_END );

  for( i=start; i<=end; i++ ){
    if( check_get_bit( zw, i ) == FALSE ){ continue; }
    SetZukanTextVersionUp( zw, i, lang );
  }
  ZUKANSAVE_SetTextVersionUpMasterFlag(zw);
}

void Debug_ZUKANSAVE_AnnoonGetSet( ZUKAN_SAVEDATA * zw, int start, int end, HEAPID heapId )
{
  int i;
  zukan_incorrect(zw);

  set_get_bit(zw, MONSNO_ANNOON);
  set_see_bit(zw, MONSNO_ANNOON);

  // �O���}�Ӑݒ�
  SetZukanTextVersionUp(zw, MONSNO_ANNOON, PM_LANG);

  // �������ݒ�
  SetZukanRandom( zw, MONSNO_ANNOON, 0 );

  // ���������E�߂܂������ݒ�
  for( i=start; i<end; i++ ){
    SetZukanUnknownTurn( zw, i, FALSE );
    SetZukanUnknownTurn( zw, i, TRUE );
  }

  // ����
  set_debug_poke_sex( zw, MONSNO_ANNOON, heapId );

  // �O���t�B�b�N�o�[�W�����A�b�v
  ZUKANSAVE_SetGraphicVersionUpFlag(zw);
  ZUKANSAVE_SetTextVersionUpMasterFlag(zw);
}


//----------------------------------------------------------------------------
/**
 *  @brief  �f�I�L�V�X�o�b�N�A�b�v
 *
 *  @param  zw  �}�Ӄ��[�N
 */
//-----------------------------------------------------------------------------
void Debug_ZUKANSAVE_DeokisisuBuckUp( ZUKAN_SAVEDATA * zw )
{
  // �f�I�L�V�X�t�H����ID�o�b�t�@������
  InitDeokisisuFormNo( zw );

  // �f�I�L�V�X�ێ����Ă��邩�`�F�b�N
  if( ZUKANSAVE_GetPokeSeeFlag( zw, MONSNO_DEOKISISU ) == TRUE ){
    // �t�H�����i���o�[�O�ŃZ�[�u
    setDEOKISISUFormNo( zw, 0, 0 );
  }
}


// �O���Q�ƃC���f�b�N�X����鎞�̂ݗL��(�Q�[�����͖���)
#ifdef CREATE_INDEX
void *Index_Get_Zukan_Offset(ZUKAN_SAVEDATA *zw){ return &zw->zukan_get; }
void *Index_Get_Zenkoku_Zukan_Offset(ZUKAN_SAVEDATA *zw){ return &zw->zenkoku_flag; }
void *Index_Get_Get_Flag_Offset(ZUKAN_SAVEDATA *zw){ return &zw->get_flag; }
void *Index_Get_See_Flag_Offset(ZUKAN_SAVEDATA *zw){ return &zw->see_flag; }
void *Index_Get_Sex_Flag_Offset(ZUKAN_SAVEDATA *zw){ return &zw->sex_flag; }
#endif




//------------------------------------------------------------------
/**
 * �S�|�P�����́A���������t�H�����i���o�[��Ԃ�
 *
 * @param   zw
 * @param   monsno
 * @param   count
 *
 * @retval  u32
 */
//------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeForm( const ZUKAN_SAVEDATA* zw, int monsno, int count )
{
//  OS_TPrintf("monsno:%d\n", monsno);
  zukan_incorrect(zw);

  switch( monsno ){
  case MONSNO_ANNOON:
//    OS_TPrintf("  Annoon\n");
    if( count < ZUKANSAVE_GetPokeAnnoonNum(zw, FALSE ) )
    {
//      OS_TPrintf("   CheckForm\n");
      return ZUKANSAVE_GetPokeAnnoonForm( zw, count, FALSE );
    }
    break;

  case MONSNO_KARANAKUSI:
    if( count < ZUKANSAVE_GetPokeSiiusiSeeNum(zw) )
    {
      return ZUKANSAVE_GetPokeSiiusiForm( zw, count );
    }
    break;

  case MONSNO_TORITODON:
    if( count < ZUKANSAVE_GetPokeSiidorugoSeeNum(zw) )
    {
      return ZUKANSAVE_GetPokeSiidorugoForm( zw, count );
    }
    break;

  case MONSNO_MINOMUTTI:
    if( count < ZUKANSAVE_GetPokeMinomuttiSeeNum(zw) )
    {
      return ZUKANSAVE_GetPokeMinomuttiForm( zw, count );
    }
    break;

  case MONSNO_MINOMADAMU:
    if( count < ZUKANSAVE_GetPokeMinomesuSeeNum(zw) )
    {
      return ZUKANSAVE_GetPokeMinomesuForm( zw, count );
    }
    break;

  case MONSNO_PITYUU:
    if( count < ZUKANSAVE_GetPokePityuuSeeNum(zw) )
    {
      return ZUKANSAVE_GetPokePityuuForm( zw, count );
    }
    break;

  case MONSNO_DEOKISISU:
    if( count < ZUKANSAVE_GetPokeDeokisisuFormSeeNum(zw) )
    {
      return ZUKANSAVE_GetPokeDeokisisuForm( zw, count );
    }
    break;

  // �V�F�C�~�E�M���e�B�i
  case MONSNO_SHEIMI:
  case MONSNO_GIRATHINA:
    if( count < get_twoform_poke_turn_num( zw, monsno ) ){
      return get_twoform_poke_see_form( zw, monsno, count );
    }
    break;

  // ���g��
  case MONSNO_ROTOMU:
    if( count < get_rotom_turn_num( zw, MONSNO_ROTOMU ) ){
      return get_rotom_see_form( zw, MONSNO_ROTOMU, count );
    }
    break;

  default:
    break;
  }

  return 0;

}

//----------------------------------------------------------------------------
/**
 *  @brief  �}��  �������Ƃ̂���t�H��������Ԃ�
 *
 *  @param  zw      �}�Ӄ��[�N
 *  @param  monsno    �����X�^�[�i���o�[
 *
 *  @return �����t�H������
 */
//-----------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeFormNum( const ZUKAN_SAVEDATA* zw, int monsno )
{
//  OS_TPrintf("monsno:%d\n", monsno);
  zukan_incorrect(zw);

  switch( monsno ){
  case MONSNO_ANNOON:
    return ZUKANSAVE_GetPokeAnnoonNum(zw, FALSE);

  case MONSNO_KARANAKUSI:
    return ZUKANSAVE_GetPokeSiiusiSeeNum(zw);

  case MONSNO_TORITODON:
    return ZUKANSAVE_GetPokeSiidorugoSeeNum(zw);

  case MONSNO_MINOMUTTI:
    return ZUKANSAVE_GetPokeMinomuttiSeeNum(zw);

  case MONSNO_MINOMADAMU:
    return ZUKANSAVE_GetPokeMinomesuSeeNum(zw);

  case MONSNO_PITYUU:
    return ZUKANSAVE_GetPokePityuuSeeNum(zw);

  case MONSNO_DEOKISISU:
    return ZUKANSAVE_GetPokeDeokisisuFormSeeNum(zw);

  // �V�F�C�~
  case MONSNO_SHEIMI:
    return get_twoform_poke_turn_num( zw, MONSNO_SHEIMI );

  // �M���e�B�i
  case MONSNO_GIRATHINA:
    return get_twoform_poke_turn_num( zw, MONSNO_GIRATHINA );

  // ���g��
  case MONSNO_ROTOMU:
    return get_rotom_turn_num( zw, MONSNO_ROTOMU );

  default:
    break;
  }

  return 1;
}
