//=============================================================================================
/**
 * @file  poke_personal.c
 * @brief �|�P�����p�[�\�i���f�[�^�A�N�Z�X�֐��Q
 * @author  taya
 *
 * @date  2008.11.06  �쐬
 */
//=============================================================================================
#include <gflib.h>

#include "poke_tool/poke_tool.h"
#include "poke_personal_local.h"

#include "arc_def.h"
#include "personal/personal.naix"


/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  DEOKISISU_OTHER_FORM_PERSONAL =   (496-1),  // �f�I�L�V�X�̕ʃt�H�����p�[�\�i���̊J�n�i���o�[
  MINOMESU_OTHER_FORM_PERSONAL =    (499-1),  // �~�m���X�̕ʃt�H�����p�[�\�i���̊J�n�i���o�[
  GIRATINA_OTHER_FORM_PERSONAL =    (501-1),  // �M���e�B�i�̕ʃt�H�����p�[�\�i���̊J�n�i���o�[
  SHEIMI_OTHER_FORM_PERSONAL =    (502-1),  // �V�F�C�~�̕ʃt�H�����p�[�\�i���̊J�n�i���o�[
  ROTOMU_OTHER_FORM_PERSONAL =    (503-1),  // ���g���̕ʃt�H�����p�[�\�i���̊J�n�i���o�[
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static u16 get_personalID( u16 mons_no, u16 form_no );
extern void POKE_PERSONAL_InitSystem( HEAPID heapID );
extern void POKE_PERSONAL_GetData( u16 mons_no, u16 form_no, POKEMON_PERSONAL_DATA* ppd );

/*--------------------------------------------------------------------------*/
/* Globals                                                                  */
/*--------------------------------------------------------------------------*/
static ARCHANDLE*  ArcHandle = NULL;


//=============================================================================================
/**
 * �V�X�e���������i�v���O�����N����ɂP�x�����Ăяo���j
 *
 * @param   heapID    �V�X�e���������p�q�[�vID�i�A�[�J�C�u�n���h���p�ɐ��\�o�C�g�K�v�j
 *
 */
//=============================================================================================
void POKE_PERSONAL_InitSystem( HEAPID heapID )
{
  if( ArcHandle == NULL )
  {
    ArcHandle = GFL_ARC_OpenDataHandle( ARCID_PERSONAL, heapID );
  }
}




//=============================================================================================
/**
 * �p�[�\�i���f�[�^����������Ƀ��[�h�ipoke_tool�O���[�v���j
 *
 * @param   monsno    �����X�^�[�i���o�[
 * @param   form_no   �t�H�����i���o�[
 * @param   ppd     [out] �f�[�^�擾�p�\���̃A�h���X
 *
 */
//=============================================================================================
void POKE_PERSONAL_LoadData( u16 mons_no, u16 form_no, POKEMON_PERSONAL_DATA* ppd )
{
  u32 data_ofs;
  u16 personalID = get_personalID( mons_no, form_no );

  NAGI_Printf( "mons_no%d form_no%d\n", mons_no, form_no );

  data_ofs = GFL_ARC_GetDataOfsByHandle( ArcHandle, personalID );
  GFL_ARC_SeekDataByHandle( ArcHandle, data_ofs );
  GFL_ARC_LoadDataByHandleContinue( ArcHandle, sizeof(POKEMON_PERSONAL_DATA), ppd );
}

//=============================================================================================
/**
 * �킴���ڂ��e�[�u���擾�ipoke_tool�O���[�v���j
 *
 * @param   mons_no   �����X�^�[�i���o�[
 * @param   form_no   �t�H�����i���o�[
 * @param   dst     [out] �킴�o���e�[�u���擾�p�o�b�t�@�A�h���X
 *
 */
//=============================================================================================
void POKE_PERSONAL_LoadWazaOboeTable( u16 mons_no, u16 form_no, POKEPER_WAZAOBOE_CODE* dst )
{
  u16 personalID = get_personalID( mons_no, form_no );
  GFL_ARC_LoadData( dst, ARCID_WOTBL, personalID );
}



//=============================================================================================-
//=============================================================================================-

//=============================================================================================
/**
 * �p�[�\�i���f�[�^�n���h���I�[�v��
 *
 * @param   mons_no   �����X�^�[�i���o�[
 * @param   form_no   �t�H�����i���o�[
 * @param   heapID    �n���h�������p�q�[�vID
 *
 * @retval  POKEMON_PERSONAL_DATA*    �p�[�\�i���f�[�^�n���h��
 */
//=============================================================================================
POKEMON_PERSONAL_DATA*  POKE_PERSONAL_OpenHandle( u16 mons_no, u16 form_no, HEAPID heapID )
{
  POKEMON_PERSONAL_DATA* ppd = GFL_HEAP_AllocMemory( heapID, sizeof(POKEMON_PERSONAL_DATA) );
  POKE_PERSONAL_LoadData( mons_no, form_no, ppd );
  return ppd;
}

//=============================================================================================
/**
 * �p�[�\�i���f�[�^�n���h���N���[�Y
 *
 * @param   handle    �p�[�\�i���f�[�^�n���h��
 *
 */
//=============================================================================================
void POKE_PERSONAL_CloseHandle( POKEMON_PERSONAL_DATA* handle )
{
  GFL_HEAP_FreeMemory( handle );
}

//==============================================================================
/**
 * �p�[�\�i���f�[�^����p�����[�^�擾
 *
 * @param   ppd     �p�[�\�i���f�[�^�n���h��
 * @param   paramID   �擾�������p�����[�^ID
 *
 * @retval  u32     �p�����[�^
 */
//==============================================================================
u32 POKE_PERSONAL_GetParam( POKEMON_PERSONAL_DATA *ppd, PokePersonalParamID paramID )
{
  u32 ret;

  GF_ASSERT( ppd );

  switch( paramID ){
  case POKEPER_ID_basic_hp:         //��{�g�o
    ret = ppd->basic_hp;
    break;
  case POKEPER_ID_basic_pow:        //��{�U����
    ret = ppd->basic_pow;
    break;
  case POKEPER_ID_basic_def:        //��{�h���
    ret = ppd->basic_def;
    break;
  case POKEPER_ID_basic_agi:        //��{�f����
    ret = ppd->basic_agi;
    break;
  case POKEPER_ID_basic_spepow:     //��{����U����
    ret = ppd->basic_spepow;
    break;
  case POKEPER_ID_basic_spedef:     //��{����h���
    ret = ppd->basic_spedef;
    break;
  case POKEPER_ID_type1:            //�����P
    ret = ppd->type1;
    break;
  case POKEPER_ID_type2:            //�����Q
    ret = ppd->type2;
    break;
  case POKEPER_ID_get_rate:         //�ߊl��
    ret = ppd->get_rate;
    break;
  case POKEPER_ID_give_exp:         //���^�o���l
    ret = ppd->give_exp;
    break;
  case POKEPER_ID_pains_hp:         //���^�w�͒l�g�o
    ret = ppd->pains_hp;
    break;
  case POKEPER_ID_pains_pow:        //���^�w�͒l�U����
    ret = ppd->pains_pow;
    break;
  case POKEPER_ID_pains_def:        //���^�w�͒l�h���
    ret = ppd->pains_def;
    break;
  case POKEPER_ID_pains_agi:        //���^�w�͒l�f����
    ret = ppd->pains_agi;
    break;
  case POKEPER_ID_pains_spepow:     //���^�w�͒l����U����
    ret = ppd->pains_spepow;
    break;
  case POKEPER_ID_pains_spedef:     //���^�w�͒l����h���
    ret = ppd->pains_spedef;
    break;
  case POKEPER_ID_no_jump:          //�͂˂�֎~�t���O
    ret = ppd->no_jump;
    break;
  case POKEPER_ID_item1:            //�A�C�e���P
    ret = ppd->item1;
    break;
  case POKEPER_ID_item2:            //�A�C�e���Q
    ret = ppd->item2;
    break;
  case POKEPER_ID_item3:            //�A�C�e���R
    ret = ppd->item3;
    break;
  case POKEPER_ID_sex:              //���ʃx�N�g��
    ret = ppd->sex;
    break;
  case POKEPER_ID_egg_birth:        //�^�}�S�̛z������
    ret = ppd->egg_birth;
    break;
  case POKEPER_ID_friend:           //�Ȃ��x�����l
    ret = ppd->friend;
    break;
  case POKEPER_ID_grow:             //�����Ȑ�����
    ret = ppd->grow;
    break;
  case POKEPER_ID_egg_group1:       //���Â���O���[�v1
    ret = ppd->egg_group1;
    break;
  case POKEPER_ID_egg_group2:       //���Â���O���[�v2
    ret = ppd->egg_group2;
    break;
  case POKEPER_ID_speabi1:          //����\�͂P
    ret = ppd->speabi1;
    break;
  case POKEPER_ID_speabi2:          //����\�͂Q
    ret = ppd->speabi2;
    break;
  case POKEPER_ID_speabi3:          //����\�͂R
    ret = ppd->speabi3;
    break;
  case POKEPER_ID_escape:           //�����闦
    ret = ppd->escape;
    break;
  case POKEPER_ID_form_index:       //�ʃt�H�����p�[�\�i���f�[�^�J�n�ʒu
    ret = ppd->form_index;
    break;
  case POKEPER_ID_form_gra_index:   //�ʃt�H�����O���t�B�b�N�f�[�^�J�n�ʒu
    ret = ppd->form_gra_index;
    break;
  case POKEPER_ID_form_max:         //�ʃt�H����MAX
    ret = ppd->form_max;
    break;
  case POKEPER_ID_color:            //�F�i�}�ӂŎg�p�j
    ret = ppd->color;
    break;
  case POKEPER_ID_reverse:          //���]�t���O
    ret = ppd->reverse;
    break;
  case POKEPER_ID_pltt_only:        //�ʃt�H�������p���b�g�̂ݕω�
    ret = ppd->pltt_only;
    break;
  case POKEPER_ID_rank:             //�|�P���������N
    ret = ppd->rank;
    break;
  case POKEPER_ID_height:           //����
    ret = ppd->height;
    break;
  case POKEPER_ID_weight:           //�d��
    ret = ppd->weight;
    break;
  case POKEPER_ID_machine1:         //�Z�}�V���t���O�P
    ret = ppd->machine1;
    break;
  case POKEPER_ID_machine2:         //�Z�}�V���t���O�Q
    ret = ppd->machine2;
    break;
  case POKEPER_ID_machine3:         //�Z�}�V���t���O�R
    ret = ppd->machine3;
    break;
  case POKEPER_ID_machine4:         //�Z�}�V���t���O�S
    ret = ppd->machine4;
    break;
  case POKEPER_ID_waza_oshie1:      //�Z�����P
    ret = ppd->waza_oshie1;
    break;
  default:
    GF_ASSERT(0);
    ret = 0;
    break;
  }
  return ret;
}

//==============================================================================
/**
 * �p�[�\�i���f�[�^�̐��ʃx�N�g���l����A���ʃx�N�g���̃^�C�v��Ԃ�
 *
 * @param   sex_vec   �p�[�\�i���̐��ʃx�N�g���l
 *
 * @retval  POKEPER_SEX_TYPE_RND  �����_��
 * @retval  POKEPER_SEX_TYPE_FIX  �Œ�(�I�X�E���X�Œ� or ���ʕs��)
 */
//==============================================================================
POKEPER_SEX_TYPE PokePersonal_SexVecTypeGet( POKEPER_SEX sex_vec )
{
  if( sex_vec == POKEPER_SEX_MALE ||
      sex_vec == POKEPER_SEX_FEMALE ||
      sex_vec == POKEPER_SEX_UNKNOWN){
    return POKEPER_SEXTYPE_FIX;
  }
  return POKEPER_SEXTYPE_RND;
}


//------------------------------------------------------------------
/**
 *  @brief          �S���}�Ӕԍ�����n���}�Ӕԍ��𓾂邱�Ƃ��ł��郊�X�g�𓾂�
 *
 *  @param[in]      heap_id     �q�[�vID
 *  @param[out]     num         �n���}�ӂɓo�ꂷ��|�P�����̐�(NULL��)
 *                              �߂�l�̔z��̗v�f���ł͂���܂���B
 *
 *  @retval         �n���}�Ӕԍ��̔z��
 *
 *  @note           �߂�l�̔z��͌Ăяo�����ŉ�����ĉ������B
 *  @note           �߂�l��u16* list�Ŏ󂯎�����Ƃ���ƁA
 *                  list[monsno]�Œn���}�Ӕԍ���������(0<=monsno<=MONSNO_END)�B
 *                  POKEPER_CHIHOU_NO_NONE�̂Ƃ��͂���monsno�̃|�P�����͒n���}�ӂɓo�ꂵ�Ȃ��B
 *
 */
//------------------------------------------------------------------
u16* POKE_PERSONAL_GetZenkokuToChihouArray( HEAPID heap_id, u16* num )
{
  u16  chihou_appear_num = 0;
  u16  chihou_num;
  u16* chihou_list;
  u32  size;

  chihou_list = GFL_ARCHDL_UTIL_LoadEx( ArcHandle, NARC_personal_zenkoku_to_chihou_bin, FALSE, heap_id, &size);
  chihou_num = size / sizeof(u16);

  if( num )
  {
    u16 i;
    for( i=0; i<chihou_num; i++ )
    {
      if( chihou_list[i] != POKEPER_CHIHOU_NO_NONE ) chihou_appear_num++;
    }
    *num = chihou_appear_num;
  }

  return chihou_list;
}




//--------------------------------------------------------------------------
/**
 * �����X�^�[�i���o�[���t�H�����i���o�[����p�[�\�i���f�[�^ID���擾
 *
 * @param   mons_no   �|�P�����i���o�[
 * @param   form_no   �t�H�����i���o�[
 *
 * @retval  u16     �p�[�\�i���f�[�^ID
 */
//--------------------------------------------------------------------------
static u16 get_personalID( u16 mons_no, u16 form_no )
{
  POKEMON_PERSONAL_DATA ppd;
  u32 data_ofs;

  //�^�}�S�ɂ̓p�[�\�i�����Ȃ��̂ŁA0�̃_�~�[�f�[�^��ǂݍ��܂���悤�ɂ���
  //GF_ASSERT( ( mons_no != MONSNO_TAMAGO ) && ( mons_no != MONSNO_DAMETAMAGO ) );
  if( ( mons_no == MONSNO_TAMAGO ) ||
      ( mons_no == MONSNO_DAMETAMAGO ) )
  { 
    return 0;
  }

  data_ofs = GFL_ARC_GetDataOfsByHandle( ArcHandle, mons_no );
  GFL_ARC_SeekDataByHandle( ArcHandle, data_ofs );
  GFL_ARC_LoadDataByHandleContinue( ArcHandle, sizeof(POKEMON_PERSONAL_DATA), &ppd );

  if( ( ppd.form_index == 0 ) || ( ppd.form_max <= form_no ) || ( form_no == 0 ) )
  {
    return mons_no;
  }

  mons_no = ppd.form_index + form_no - 1;

  return mons_no;

}

