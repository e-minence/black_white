//============================================================================================
/**
 * @file		zukan_savedata.c
 * @brief		�}�ӃZ�[�u�f�[�^�A�N�Z�X�����Q�\�[�X
 * @author	mori / tamada GAME FREAK inc.
 * @date		2009.10.26
 *
 * �W���E�g���C�b�V�����H�Ƃ��̓x�ɖ��O������������̂��Ȃ�Ȃ̂ŁA
 * �S���́uZENKOKU�v�A�n���}�ӂ̎��́uLOCAL�v�ƋL�q���鎖�ɂ��܂��B
 * �R�����g�ł��u�S���}�Ӂv�u�n���}�Ӂv�ƋL�q���܂��B
 *
 */
//============================================================================================
#include <gflib.h>

#include "pm_version.h"
#include "arc_def.h"
#include "system/gfl_use.h"
#include "savedata/save_tbl.h"
#include "savedata/save_control.h"

#include "gamesystem/game_data.h"
#include "savedata/zukan_savedata.h"
#include "poke_tool/poke_personal.h"

#include "zukan_data.naix"


//============================================================================================
//	�萔��`
//============================================================================================
#include "zukan_savedata_local.h"   //������ԃf�[�^�^��POKEZUKAN_ARRAY_LEN�ȂǓ�����`


//============================================================================================
//	�O���[�o��
//============================================================================================

// �t�H�����e�[�u��
static const u16 FormTable[][2] =
{
	// �|�P�����ԍ�, �t�H������
	{ MONSNO_ANNOON, FORM_MAX_ANNOON },						// �A���m�[��

	{ MONSNO_DEOKISISU, FORM_MAX_DEOKISISU },			// �f�I�L�V�X

	{ MONSNO_SHEIMI, FORM_MAX_SHEIMI },						// �V�F�C�~
	{ MONSNO_GIRATHINA, FORM_MAX_GIRATHINA },			// �M���e�B�i
	{ MONSNO_ROTOMU, FORM_MAX_ROTOMU },						// ���g��
	{ MONSNO_KARANAKUSI, FORM_MAX_KARANAKUSI },		// �J���i�N�V
	{ MONSNO_TORITODON, FORM_MAX_TORITODON },			// �g���g�h��
	{ MONSNO_MINOMUTTI, FORM_MAX_MINOMUTTI },			// �~�m���b�`
	{ MONSNO_MINOMADAMU, FORM_MAX_MINOMADAMU },		// �~�m�}�_��

	{ MONSNO_POWARUN, FORM_MAX_POWARUN },					// �|������ ������
	{ MONSNO_THERIMU, FORM_MAX_THERIMU },					// �`�F���� ������

	{ MONSNO_511, FORMNO_511_MAX },				        // �V�L�W�J ���V�K
	{ MONSNO_527, FORMNO_527_MAX },				        // �A���g���X ���V�K
	{ MONSNO_655, FORMNO_655_MAX },								// �����f�B�A ���V�K
	{ MONSNO_HIHIDARUMA, FORM_MAX_HIHIDARUMA },		// �q�q�_���} ���V�K
	{ MONSNO_BASURAO, FORM_MAX_BASURAO },					// �o�X���I ���V�K

	{ 0, 0 },
};



//============================================================================================
//	�����֐�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�����񃏁[�N�̐������`�F�b�N
 *
 * @param		�}�ӃZ�[�u�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static inline void zukan_incorrect(const ZUKAN_SAVEDATA * zw)
{
  GF_ASSERT(zw->zukan_magic == MAGIC_NUMBER);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����i���o�[�͈̔̓`�F�b�N
 *
 * @param		monsno		�|�P�����ԍ�
 *
 * @retval	"TRUE = �͈͊O"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
static BOOL monsno_incorrect(u16 monsno)
{
  if (monsno == 0 || monsno > MONSNO_END) {
    GF_ASSERT_MSG(0, "�|�P�����i���o�[�ُ�F%d\n", monsno);
    return TRUE;
  } else {
    return FALSE;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ėp�r�b�g�`�F�b�N
 *
 * @param		array			�r�b�g�e�[�u��
 * @param		flag_id		�t���O�h�c
 *
 * @retval	"TRUE = ON"
 * @retval	"FALSE = OFF"
 */
//--------------------------------------------------------------------------------------------
static inline BOOL check_bit(const u8 * array, u16 flag_id)
{
  return 0 != (array[flag_id >> 3] & (1 << (flag_id & 7)));
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ėp�r�b�g�`�F�b�N�i�t���O = �|�P�����ԍ��j
 *
 * @param		array			�r�b�g�e�[�u��
 * @param		flag_id		�t���O�h�c
 *
 * @retval	"TRUE = ON"
 * @retval	"FALSE = OFF"
 */
//--------------------------------------------------------------------------------------------
static inline BOOL check_bit_mons(const u8 * array, u16 flag_id)
{
  flag_id --;
	return check_bit( array, flag_id );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ėp�r�b�g�Z�b�g
 *
 * @param		array			�r�b�g�e�[�u��
 * @param		flag_id		�t���O�h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static inline void set_bit(u8 * array, u16 flag_id)
{
  array[flag_id >> 3] |= 1 << (flag_id & 7);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ėp�r�b�g�Z�b�g�i�t���O = �|�P�����ԍ��j
 *
 * @param		array			�r�b�g�e�[�u��
 * @param		flag_id		�t���O�h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static inline void set_bit_mons(u8 * array, u16 flag_id)
{
  flag_id --;
	set_bit( array, flag_id );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ėp�r�b�g���Z�b�g
 *
 * @param		array			�r�b�g�e�[�u��
 * @param		flag_id		�t���O�h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static inline void reset_bit( u8 * array, u16 flag_id )
{
  array[flag_id >> 3] &= (1 << (flag_id & 7)) ^ 0xff;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ėp�r�b�g���Z�b�g�i�t���O = �|�P�����ԍ��j
 *
 * @param		array			�r�b�g�e�[�u��
 * @param		flag_id		�t���O�h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static inline void reset_bit_mons( u8 * array, u16 flag_id )
{
	flag_id--;
	reset_bit( array, flag_id );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�݂��r�b�g�ݒ�
 *
 * @param		zw				�}�ӃZ�[�u�f�[�^
 * @param		flag_id		�|�P�����ԍ�
 * @param		type			����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static inline void set_see_bit( ZUKAN_SAVEDATA * zw, u16 flag_id, u16 type )
{
	set_bit_mons( (u8*)zw->sex_flag[type], flag_id );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���܂����r�b�g�ݒ�
 *
 * @param		zw				�}�ӃZ�[�u�f�[�^
 * @param		flag_id		�|�P�����ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static inline void set_get_bit(ZUKAN_SAVEDATA * zw, u16 flag_id)
{
  set_bit_mons( (u8*)zw->get_flag, flag_id );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�݂��r�b�gON/OFF�`�F�b�N
 *
 * @param		zw				�}�ӃZ�[�u�f�[�^
 * @param		monsno		�|�P�����ԍ�
 * @param		type			����
 *
 * @retval	"TRUE = ON"
 * @retval	"FALSE = OFF"
 */
//--------------------------------------------------------------------------------------------
static inline BOOL check_see_bit( const ZUKAN_SAVEDATA * zw, u16 monsno, u16 type )
{
	return check_bit_mons( (const u8*)zw->sex_flag[type], monsno );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���܂����r�b�gON/OFF�`�F�b�N
 *
 * @param		zw				�}�ӃZ�[�u�f�[�^
 * @param		monsno		�|�P�����ԍ�
 *
 * @retval	"TRUE = ON"
 * @retval	"FALSE = OFF"
 */
//--------------------------------------------------------------------------------------------
static inline BOOL check_get_bit(const ZUKAN_SAVEDATA * zw, u16 monsno )
{
  return check_bit_mons((const u8*)zw->get_flag, monsno );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ő̗�����ݒ�
 *
 * @param		zw				�}�ӃZ�[�u�f�[�^
 * @param		monsno		�|�P�����ԍ�
 * @param		rand			������
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static inline void SetZukanRandom( ZUKAN_SAVEDATA * zw, u16 monsno, u32 rand )
{
  if( monsno == MONSNO_PATTIIRU && zw->PachiRandom == 0 ){
    zw->PachiRandom = rand;
  }
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_ZUKAN)
  SVLD_SetCrc(GMDATA_ID_ZUKAN);
#endif //CRC_LOADCHECK
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�e�L�X�g�o�[�W�����t���O�ݒ�
 *
 * @param		zw				�}�ӃZ�[�u�f�[�^
 * @param		monsno		�|�P�����ԍ�
 * @param		lang			���R�[�h
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetZukanTextVersionUp( ZUKAN_SAVEDATA * zw, u16 monsno, u32 lang )
{
	// ���R�[�h�ő� or ���ԃR�[�h
	if( lang > 8 || lang == 6 ){
		return;
	}

	// �V�K�|�P�����͕\���s��
	if( monsno > POKEZUKAN_TEXT_POSSIBLE ){
		return;
	}

	// ����U�Ԃ����ԂȂ̂ŁA�X�y�C����ȍ~��-1
	if( lang >= LANG_SPAIN ){
		lang -= 1;
	}

	set_bit( zw->TextVersionUp, (monsno-1)*TEXTVER_VER_MAX+(lang-1) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�S���}�ӊ����ɕK�v�ȃ|�P�������`�F�b�N
 *
 * @param		monsno		�|�P�����ԍ�
 *
 * @retval	"TRUE = �K�v"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
static BOOL check_ZenkokuCompMonsno( u16 monsno )
{
  int i;
  static const u16 cut_check_monsno[] = {
    MONSNO_MYUU,				// �~���E
    MONSNO_SEREBHI,			// �Z���r�B
    MONSNO_ZIRAATI,			// �W���[�`
    MONSNO_DEOKISISU,		// �f�I�L�V�X
    MONSNO_FIONE,				// �t�B�I�l
    MONSNO_MANAFI,			// �}�i�t�B
    MONSNO_DAAKURAI,		// �_�[�N���C
    MONSNO_SHEIMI,			// �V�F�C�~
    MONSNO_ARUSEUSU,		// �A���Z�E�X
		MONSNO_652,					// �c�`�m�J�~
		MONSNO_653,					// ���C
		MONSNO_654,					// �_���^�j�X
		MONSNO_655,					// �����f�B�A
		MONSNO_656,					// �C���Z�N�^
		MONSNO_657,					// �r�N�e�B
  };

  // �`�F�b�N���O�|�P�����`�F�b�N
  for( i=0; i<NELEMS(cut_check_monsno); i++ ){
    if( cut_check_monsno[i] == monsno ){
      return FALSE;
    }
  }
  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�n���}�ӊ����ɕK�v�ȃ|�P�������`�F�b�N
 *
 * @param		monsno		�|�P�����ԍ�
 *
 * @retval	"TRUE = �K�v"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
static BOOL check_LocalCompMonsno( u16 monsno )
{
  int i;
  static const u16 cut_check_monsno[] = {
		MONSNO_652,			// �c�`�m�J�~
		MONSNO_653,			// ���C
		MONSNO_654,			// �_���^�j�X
		MONSNO_655,			// �����f�B�A
		MONSNO_656,			// �C���Z�N�^
		MONSNO_657,			// �r�N�e�B
  };

  // �`�F�b�N���O�|�P�����`�F�b�N
  for( i=0; i<NELEMS(cut_check_monsno); i++ ){
    if( cut_check_monsno[i] == monsno ){
      return FALSE;
    }
  }
  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�H�����ݒ�r�b�g���擾
 *
 * @param		mons			�|�P�����ԍ�
 *
 * @retval	"-1 != �t�H�����ݒ�r�b�g"
 * @retval	"-1 = �ʃt�H�������Ȃ��|�P����"
 */
//--------------------------------------------------------------------------------------------
static s32 GetPokeFormBit( u16 mons )
{
	s32	cnt = 0;
	s32	i = 0;

	while( FormTable[i][0] != 0 ){
		if( FormTable[i][0] == mons ){
			return cnt;
		}
		cnt += FormTable[i][1];
		i++;
	}
	return -1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�H�����e�[�u���ʒu���擾
 *
 * @param		mons			�|�P�����ԍ�
 *
 * @retval	"-1 != �t�H�����e�[�u���ʒu"
 * @retval	"-1 = �ʃt�H�������Ȃ��|�P����"
 */
//--------------------------------------------------------------------------------------------
static s32 GetPokeFormTablePos( u16 mons )
{
	s32	i = 0;

	while( FormTable[i][0] != 0 ){
		if( FormTable[i][0] == mons ){
			return i;
		}
		i++;
	}
	return -1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ʏ�̌����t���O�ݒ�
 *
 * @param		zw      �}�Ӄ��[�N�ւ̃|�C���^
 * @param		mons		�|�P�����ԍ�
 * @param		sex			����
 * @param		rare		���A�t���O TRUE = RARE
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetNormalSeeFlag( ZUKAN_SAVEDATA * zw, u32 mons, u32 sex, BOOL rare )
{
	if( sex == PTL_SEX_MALE ){
		if( rare == TRUE ){
			set_see_bit( zw, mons, SEE_FLAG_M_RARE );
		}else{
			set_see_bit( zw, mons, SEE_FLAG_MALE );
		}
	}else if( sex == PTL_SEX_FEMALE ){
		if( rare == TRUE ){
			set_see_bit( zw, mons, SEE_FLAG_F_RARE );
		}else{
			set_see_bit( zw, mons, SEE_FLAG_FEMALE );
		}
	}else{
		// ���ʂ��Ȃ��ꍇ�́��̃t���O�𗧂Ă�
		if( rare == TRUE ){
			set_see_bit( zw, mons, SEE_FLAG_M_RARE );
		}else{
			set_see_bit( zw, mons, SEE_FLAG_MALE );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�H�����݂��t���O�ݒ�
 *
 * @param		zw      �}�Ӄ��[�N�ւ̃|�C���^
 * @param		mons		�|�P�����ԍ�
 * @param		sex			����
 * @param		rare		���A�t���O TRUE = RARE
 * @param		form		�t�H����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetFormSeeFlag( ZUKAN_SAVEDATA * zw, u32 mons, u32 sex, BOOL rare, u32 form )
{
	s32	pos = GetPokeFormBit( mons );

	if( pos != -1 ){
		u16	max;
		u16	i;
		// �ꕔ�̃|�P�����͑S�Ẵt�H������ON�ɂ���
		// �|������
		if( mons == MONSNO_POWARUN ){
			form = 0;
			max  = FORM_MAX_POWARUN;
		// �`�F����
		}else if( mons == MONSNO_THERIMU ){
			form = 0;
			max  = FORM_MAX_THERIMU;
		// �q�q�_���}
		}else if( mons == MONSNO_HIHIDARUMA ){
			form = 0;
			max  = FORM_MAX_HIHIDARUMA;
		// �����f�B�A
		}else if( mons == MONSNO_MERODHIA ){
			form = 0;
			max  = FORM_MAX_MERODHIA;
		}else{
			max  = 1;
		}

		if( rare == TRUE ){
			for( i=0; i<max; i++ ){
				set_bit( zw->form_flag[COLOR_RARE], pos+form+i );
			}
		}else{
			for( i=0; i<max; i++ ){
				set_bit( zw->form_flag[COLOR_NORMAL], pos+form+i );
			}
		}
	}
}


//============================================================================================
//	�������񑀍�p�̊O�����J�֐�
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		�}�ӃZ�[�u�f�[�^�̃T�C�Y�擾
 *
 * @param		none
 *
 * @return	�}�ӃZ�[�u�f�[�^�̃T�C�Y
 */
//--------------------------------------------------------------------------------------------
int ZUKANSAVE_GetWorkSize(void)
{
  return sizeof(ZUKAN_SAVEDATA);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�}�ӃZ�[�u�f�[�^�̏�����
 *
 * @param		zw		�}�ӃZ�[�u�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZUKANSAVE_Init( ZUKAN_SAVEDATA * zw )
{
	GFL_STD_MemClear( zw, sizeof(ZUKAN_SAVEDATA) );
  zw->zukan_magic = MAGIC_NUMBER;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�}�ӃZ�[�u�f�[�^�擾�i�Z�[�u�f�[�^����j
 *
 * @param		sv		�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 *
 * @return  �}�ӃZ�[�u�f�[�^
 *
 * @li	�ʏ�͎g�p���Ȃ��ł��������I
 */
//--------------------------------------------------------------------------------------------
ZUKAN_SAVEDATA * ZUKAN_SAVEDATA_GetZukanSave( SAVE_CONTROL_WORK * sv )
{
	return SaveControl_DataPtrGet( sv, GMDATA_ID_ZUKAN );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�}�ӃZ�[�u�f�[�^�擾�iGAMEDETA����j
 *
 * @param		gamedata		GAMEDATA
 *
 * @return  �}�ӃZ�[�u�f�[�^
 *
 * @li	�Q�[�����͂�������g�p���Ă�������
 */
//--------------------------------------------------------------------------------------------
ZUKAN_SAVEDATA * GAMEDATA_GetZukanSave( GAMEDATA * gamedata )
{
	return ZUKAN_SAVEDATA_GetZukanSave( GAMEDATA_GetSaveControlWork(gamedata) );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�S���}�Ӄ��[�h���Z�b�g
 *
 * @param		zw		�}�ӃZ�[�u�f�[�^
 *
 * @return	none
 *
 *	�S���}�ӂ�\���\�ɂ���
 */
//--------------------------------------------------------------------------------------------
void ZUKANSAVE_SetZenkokuZukanFlag( ZUKAN_SAVEDATA * zw )
{
  zukan_incorrect(zw);
  zw->zenkoku_flag = TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�S�������񃂁[�h���ǂ����H�̖₢���킹
 *
 * @param		zw		�}�ӃZ�[�u�f�[�^
 *
 * @retval	"TRUE = �S���}�Ӊ�"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ZUKANSAVE_GetZenkokuZukanFlag( const ZUKAN_SAVEDATA * zw )
{
  zukan_incorrect(zw);
  return zw->zenkoku_flag;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief  �O���t�B�b�N�o�[�W�����p�@�\�g���t���O�ݒ�
 *
 * @param		zw		�}�ӃZ�[�u�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZUKANSAVE_SetGraphicVersionUpFlag( ZUKAN_SAVEDATA * zw )
{
  zukan_incorrect(zw);
  zw->ver_up_flg = TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�O���t�B�b�N�o�[�W�����p�@�\�g���t���O�擾
 *
 * @param		zw		�}�ӃZ�[�u�f�[�^
 *
 * @retval	"TRUE = �o�[�W�����A�b�v�ς�"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ZUKANSAVE_GetGraphicVersionUpFlag( const ZUKAN_SAVEDATA * zw )
{
  zukan_incorrect(zw);
  return zw->ver_up_flg;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�����̐}�Ӄ��[�h��ݒ�
 *
 * @param		zw		�}�ӃZ�[�u�f�[�^
 * @param		mode	�}�Ӄ��[�h TRUE = �S��, FALSE = �n��
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZUKANSAVE_SetZukanMode( ZUKAN_SAVEDATA * zw, BOOL mode )
{
  zukan_incorrect(zw);
	zw->zukan_mode = mode;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�����̐}�Ӄ��[�h���擾
 *
 * @param		zw		�}�ӃZ�[�u�f�[�^
 *
 * @retval	"TRUE = �S��"
 * @retval	"FALSE = �n��"
 */
//--------------------------------------------------------------------------------------------
BOOL ZUKANSAVE_GetZukanMode( const ZUKAN_SAVEDATA * zw )
{
  zukan_incorrect(zw);
	return zw->zukan_mode;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�����̃|�P�����ԍ���ݒ�
 *
 * @param		zw		�}�ӃZ�[�u�f�[�^
 * @param		mons	�|�P�����ԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZUKANSAVE_SetDefaultMons( ZUKAN_SAVEDATA * zw, u16 mons )
{
  zukan_incorrect(zw);
	zw->defaultMonsNo = mons;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�����̃|�P�����ԍ����擾
 *
 * @param		zw		�}�ӃZ�[�u�f�[�^
 *
 * @return	�|�P�����ԍ�
 */
//--------------------------------------------------------------------------------------------
u16 ZUKANSAVE_GetDefaultMons( const ZUKAN_SAVEDATA * zw )
{
  zukan_incorrect(zw);
	return zw->defaultMonsNo;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�����f�[�^�ݒ�
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 * @param		mons		�|�P�����ԍ�
 * @param		sex			����
 * @param		rare		���A�t���O TRUE = RARE
 * @param		form		�t�H����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZUKANSAVE_SetDrawData( ZUKAN_SAVEDATA * zw, u16 mons, u32 sex, BOOL rare, u32 form )
{
	{	// �{�����t�H�����f�[�^
		s32	cnt = 0;
		s32	i = 0;

		while( FormTable[i][0] != 0 ){
			if( FormTable[i][0] == mons ){
				u32	j;
				for( j=0; j<FormTable[i][1]; j++ ){
					reset_bit( (u8 *)zw->draw_form[0], cnt+j );
					reset_bit( (u8 *)zw->draw_form[1], cnt+j );
				}
				if( rare == TRUE ){
					set_bit( (u8 *)zw->draw_form[COLOR_RARE], cnt+form );
				}else{
					set_bit( (u8 *)zw->draw_form[COLOR_NORMAL], cnt+form );
				}
				break;
			}
			cnt += FormTable[i][1];
			i++;
		}
	}

	{	// �{�����ʏ�f�[�^
		u32	i;

		for( i=0; i<SEE_FLAG_MAX; i++ ){
			reset_bit_mons( (u8 *)zw->draw_sex[i], mons );
		}
		if( sex == PTL_SEX_MALE ){
			if( rare == TRUE ){
				set_bit_mons( (u8 *)zw->draw_sex[SEE_FLAG_M_RARE], mons );
			}else{
				set_bit_mons( (u8 *)zw->draw_sex[SEE_FLAG_MALE], mons );
			}
		}else if( sex == PTL_SEX_FEMALE ){
			if( rare == TRUE ){
				set_bit_mons( (u8 *)zw->draw_sex[SEE_FLAG_F_RARE], mons );
			}else{
				set_bit_mons( (u8 *)zw->draw_sex[SEE_FLAG_FEMALE], mons );
			}
		}else{
			// ���ʂ��Ȃ��ꍇ�́��̃t���O�𗧂Ă�
			if( rare == TRUE ){
				set_bit_mons( (u8 *)zw->draw_sex[SEE_FLAG_M_RARE], mons );
			}else{
				set_bit_mons( (u8 *)zw->draw_sex[SEE_FLAG_MALE], mons );
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ő�t�H�������擾
 *
 * @param		mons		�|�P�����ԍ�
 *
 * @return	�ő�t�H������
 */
//--------------------------------------------------------------------------------------------
u32	ZUKANSAVE_GetFormMax( u16 mons )
{
	s32	pos = GetPokeFormTablePos( mons );

	if( pos != -1 ){
		return FormTable[pos][1];
	}
	return 1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�\������t�H�����f�[�^���擾
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 * @param		mons		�|�P�����ԍ�
 * @param		sex			����
 * @param		rare		���A�t���O TRUE = RARE
 * @param		form		�t�H����
 * @param		heapID	�q�[�v�h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZUKANSAVE_GetDrawData( ZUKAN_SAVEDATA * zw, u16 mons, u32 * sex, BOOL * rare, u32 * form, HEAPID heapID )
{
	POKEMON_PERSONAL_DATA * ppd;
	s32	tp;
	s32	bit;
	u32	i;

	tp = GetPokeFormTablePos( mons );

	if( tp != -1 ){
		bit = GetPokeFormBit( mons );
		for( i=0; i<FormTable[tp][1]; i++ ){
			if( check_bit( zw->draw_form[COLOR_NORMAL], bit+i ) ){
				*form = i;
				*rare = FALSE;
				break;
			}
			if( check_bit( zw->draw_form[COLOR_RARE], bit+i ) ){
				*form = i;
				*rare = TRUE;
				break;
			}
		}
		if( check_bit_mons( (const u8 *)zw->draw_sex[SEE_FLAG_MALE], mons ) ||
				check_bit_mons( (const u8 *)zw->draw_sex[SEE_FLAG_M_RARE], mons ) ){
			*sex  = PTL_SEX_MALE;
		}
		if( check_bit_mons( (const u8 *)zw->draw_sex[SEE_FLAG_FEMALE], mons ) ||
				check_bit_mons( (const u8 *)zw->draw_sex[SEE_FLAG_F_RARE], mons ) ){
			*sex  = PTL_SEX_FEMALE;
		}
	}else{
		*form = 0;
		if( check_bit_mons( (const u8 *)zw->draw_sex[SEE_FLAG_MALE], mons ) ){
			*sex  = PTL_SEX_MALE;
			*rare = FALSE;
		}
		if( check_bit_mons( (const u8 *)zw->draw_sex[SEE_FLAG_FEMALE], mons ) ){
			*sex  = PTL_SEX_FEMALE;
			*rare = FALSE;
		}
		if( check_bit_mons( (const u8 *)zw->draw_sex[SEE_FLAG_M_RARE], mons ) ){
			*sex  = PTL_SEX_MALE;
			*rare = TRUE;
		}
		if( check_bit_mons( (const u8 *)zw->draw_sex[SEE_FLAG_F_RARE], mons ) ){
			*sex  = PTL_SEX_FEMALE;
			*rare = TRUE;
		}
	}

	ppd = POKE_PERSONAL_OpenHandle( mons, *form, heapID );
	if( POKE_PERSONAL_GetParam( ppd, POKEPER_ID_sex ) == POKEPER_SEX_UNKNOWN ){
		*sex  = PTL_SEX_UNKNOWN;
	}
	POKE_PERSONAL_CloseHandle( ppd );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�ߊl�f�[�^�Z�b�g
 *
 * @param		zw		�}�ӃZ�[�u�f�[�^
 * @param		pp		�ߊl�|�P�����f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZUKANSAVE_SetPokeGet( ZUKAN_SAVEDATA * zw, POKEMON_PARAM * pp )
{
	BOOL	fast;
	BOOL	rare;
  u32 rand;
	u32	form;
  u32 lang;
  u32 sex;
  u16 mons;

  zukan_incorrect(zw);

	fast = PP_FastModeOn( pp );
	mons = PP_Get( pp, ID_PARA_monsno, NULL );
	rand = PP_Get( pp, ID_PARA_personal_rnd, NULL );
	form = PP_Get( pp, ID_PARA_form_no, NULL );
  lang = PP_Get( pp, ID_PARA_country_code, NULL );
	sex  = PP_GetSex( pp );
	rare = PP_CheckRare( pp );
	PP_FastModeOff( pp, fast );

  if( monsno_incorrect(mons) ){
    return;
  }

	// �������ƂȂ��ꍇ
	if( ZUKANSAVE_GetPokeSeeFlag( zw, mons ) == FALSE ){
		ZUKANSAVE_SetDrawData( zw, mons, sex, rare, form );
	}

	// �ő̗���
	SetZukanRandom( zw, mons, rand );
	// �ʏ�
	SetNormalSeeFlag( zw, mons, sex, rare );
	// �t�H��������
	SetFormSeeFlag( zw, mons, sex, rare, form );
	// ���R�[�h�Z�b�g
	SetZukanTextVersionUp( zw, mons, lang );
	// �ߊl�t���O�Z�b�g
  set_get_bit( zw, mons );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ߊl�`�F�b�N
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 * @param		monsno	�|�P�����ԍ�
 *
 * @retval	"TRUE = �ߊl�ς�"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ZUKANSAVE_GetPokeGetFlag( const ZUKAN_SAVEDATA * zw, u16 monsno )
{
  zukan_incorrect(zw);

  if( monsno_incorrect(monsno) ) {
    return FALSE;
  }
  if( check_get_bit(zw,monsno) ){
    return TRUE;
  }
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���݂̐}�Ӄ��[�h�̃|�P�����ߊl���擾
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 * @param		heapID	�q�[�v�h�c
 *
 * @return	�ߊl��
 */
//--------------------------------------------------------------------------------------------
u16 ZUKANSAVE_GetZukanPokeGetCount( const ZUKAN_SAVEDATA * zw, HEAPID heapID )
{
  if( ZUKANSAVE_GetZenkokuZukanFlag( zw ) ){
    return ZUKANSAVE_GetPokeGetCount( zw );
  }
  return ZUKANSAVE_GetLocalPokeGetCount( zw, heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����ߊl���擾�i�S���j
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 *
 * @return	�ߊl��
 */
//--------------------------------------------------------------------------------------------
u16 ZUKANSAVE_GetPokeGetCount( const ZUKAN_SAVEDATA * zw )
{
  int count;
  int i;

  zukan_incorrect(zw);

  count= 0;
  for( i=1; i<=MONSNO_END; i++ ){
    if( ZUKANSAVE_GetPokeGetFlag( zw, i ) == TRUE ){
      count++;
    }
  }
  return count;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����ߊl���擾�i�n���j
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 * @param		heapID	�q�[�v�h�c
 *
 * @return	�ߊl��
 */
//--------------------------------------------------------------------------------------------
u16 ZUKANSAVE_GetLocalPokeGetCount( const ZUKAN_SAVEDATA * zw, HEAPID heapID )
{
  u16 * buf;
  u16 i;
  u16 num=0;

  zukan_incorrect(zw);

	buf = POKE_PERSONAL_GetZenkokuToChihouArray( heapID, NULL );
  num = 0;

  for( i=1; i<=MONSNO_END; i++ ){
    if( ZUKANSAVE_GetPokeGetFlag( zw, i ) == TRUE ){
      // �n���}�ӂɂ��邩�`�F�b�N
      if( buf[i] != POKEPER_CHIHOU_NO_NONE ){
        num++;
      }
    }
  }

  GFL_HEAP_FreeMemory( buf );

  return num;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�����f�[�^�Z�b�g
 *
 * @param		zw		�}�ӃZ�[�u�f�[�^
 * @param		pp		�����|�P�����̃f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZUKANSAVE_SetPokeSee( ZUKAN_SAVEDATA * zw, POKEMON_PARAM * pp )
{
	BOOL	fast;
	BOOL	rare;
  u32 rand;
	u32	form;
  u32 sex;
  u16 mons;

  zukan_incorrect(zw);

	fast = PP_FastModeOn( pp );
	mons = PP_Get( pp, ID_PARA_monsno, NULL );
	rand = PP_Get( pp, ID_PARA_personal_rnd, NULL );
	form = PP_Get( pp, ID_PARA_form_no, NULL );
	sex  = PP_GetSex( pp );
	rare = PP_CheckRare( pp );
	PP_FastModeOff( pp, fast );

  if( monsno_incorrect(mons) ){
    return;
  }

	// �������ƂȂ��ꍇ
	if( ZUKANSAVE_GetPokeSeeFlag( zw, mons ) == FALSE ){
		ZUKANSAVE_SetDrawData( zw, mons, sex, rare, form );
	}

	// �ő̗���
	SetZukanRandom( zw, mons, rand );
	// �ʏ�
	SetNormalSeeFlag( zw, mons, sex, rare );
	// �t�H��������
	SetFormSeeFlag( zw, mons, sex, rare, form );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�����`�F�b�N
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 * @param		monsno	�|�P�����ԍ�
 *
 * @retval	"TRUE = ����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ZUKANSAVE_GetPokeSeeFlag( const ZUKAN_SAVEDATA * zw, u16 monsno )
{
	u32	i;

  zukan_incorrect(zw);
  if( monsno_incorrect(monsno) ){
    return FALSE;
  }
	for( i=0; i<SEE_FLAG_MAX; i++ ){
		if( check_see_bit(zw,monsno,i) ){
			return TRUE;
		}
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���݂̐}�Ӄ��[�h�̃|�P�������������擾
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 * @param		heapID	�q�[�v�h�c
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
u16 ZUKANSAVE_GetZukanPokeSeeCount( const ZUKAN_SAVEDATA * zw, HEAPID heapID )
{
  if( ZUKANSAVE_GetZenkokuZukanFlag( zw ) ){
    return ZUKANSAVE_GetPokeSeeCount( zw );
  }
  return ZUKANSAVE_GetLocalPokeSeeCount( zw, heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�������������擾�i�S���j
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
u16 ZUKANSAVE_GetPokeSeeCount( const ZUKAN_SAVEDATA * zw )
{
  int count;
  int i;

  zukan_incorrect(zw);

  count= 0;
  for( i=1; i<=MONSNO_END; i++ ){
    if( ZUKANSAVE_GetPokeSeeFlag( zw, i ) == TRUE ){
      count++;
    }
  }
  return count;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�������������擾�i�n���j
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 * @param		heapID	�q�[�v�h�c
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
u16 ZUKANSAVE_GetLocalPokeSeeCount( const ZUKAN_SAVEDATA * zw, HEAPID heapID )
{
  u16 * buf;
  u16 i;
  u16 num=0;

  zukan_incorrect(zw);

	buf = POKE_PERSONAL_GetZenkokuToChihouArray( heapID, NULL );
  num = 0;

  for( i=1; i<=MONSNO_END; i++ ){
    if( ZUKANSAVE_GetPokeSeeFlag( zw, i ) == TRUE ){
      // �n���}�ӂɂ��邩�`�F�b�N
      if( buf[i] != POKEPER_CHIHOU_NO_NONE ){
        num++;
      }
    }
  }

  GFL_HEAP_FreeMemory( buf );

  return num;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�w��^�C�v�̃|�P������������
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 * @param		monsno	�|�P�����ԍ�
 * @param		sex			����
 * @param		rare		���A
 * @param		form		�t�H����
 *
 * @retval	"TRUE = ����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ZUKANSAVE_CheckPokeSeeForm( const ZUKAN_SAVEDATA * zw, u16 monsno, int sex, int rare, int form )
{
  zukan_incorrect(zw);

  if( monsno_incorrect(monsno) ){
    return ZUKANSAVE_GET_SEX_ERR;
  }

	{	// �t�H��������
		s32	pos = GetPokeFormBit( monsno );
		if( pos != -1 ){
			return check_bit( (const u8*)zw->form_flag[rare], pos+form );
		}
	}

	// �ʏ�
	if( sex == PTL_SEX_MALE ){
		if( rare == 1 ){
			return check_see_bit( zw, monsno, SEE_FLAG_M_RARE );
		}else{
			return check_see_bit( zw, monsno, SEE_FLAG_MALE );
		}
	}else if( sex == PTL_SEX_FEMALE ){
		if( rare == 1 ){
			return check_see_bit( zw, monsno, SEE_FLAG_F_RARE );
		}else{
			return check_see_bit( zw, monsno, SEE_FLAG_FEMALE );
		}
	}else{
		if( rare == 1 ){
			if( check_see_bit( zw, monsno, SEE_FLAG_M_RARE ) == TRUE ||
					check_see_bit( zw, monsno, SEE_FLAG_F_RARE ) == TRUE ){
				return TRUE;
			}
		}else{
			if( check_see_bit( zw, monsno, SEE_FLAG_MALE ) == TRUE ||
					check_see_bit( zw, monsno, SEE_FLAG_FEMALE ) == TRUE ){
				return TRUE;
			}
		}
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		����|�P�����̌������擾
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 * @param		monsno	�|�P�����ԍ�
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
u32 ZUKANSAVE_GetPokeRandomFlag( const ZUKAN_SAVEDATA * zw, u8 monsno )
{
  u32 random;

  zukan_incorrect(zw);

	// �p�b�`�[���̂�
	if( monsno == ZUKANSAVE_RANDOM_PACHI ){
		return zw->PachiRandom;
	}
	GF_ASSERT_MSG( 0, "monsno = %d\n", monsno );
	return 0;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�S���}�ӊ����`�F�b�N
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 *
 * @retval	"TRUE = ����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ZUKANSAVE_CheckZenkokuComp( const ZUKAN_SAVEDATA * zw )
{
  u16 num;

  // �[���R�N�}�ӊ����ɕK�v�ȃ|�P���������C�߂܂�����
  num = ZUKANSAVE_GetZenkokuGetCompCount( zw );

  if( num >= ZUKANSAVE_ZENKOKU_COMP_NUM ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�n���}�ӊ����`�F�b�N�i�ߊl���j
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 * @param		heapID	�q�[�v�h�c
 *
 * @retval	"TRUE = ����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ZUKANSAVE_CheckLocalGetComp( const ZUKAN_SAVEDATA * zw, HEAPID heapID )
{
  u16 num;

  // �n���}�ӊ����ɕK�v�ȃ|�P���������C�߂܂�����
  num = ZUKANSAVE_GetLocalGetCompCount( zw, heapID );

  if( num >= ZUKANSAVE_LOCAL_COMP_NUM ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�n���}�ӊ����`�F�b�N�i�������j
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 * @param		heapID	�q�[�v�h�c
 *
 * @retval	"TRUE = ����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ZUKANSAVE_CheckLocalSeeComp( const ZUKAN_SAVEDATA * zw, HEAPID heapID )
{
  u16 num;

  // �n���}�ӊ����ɕK�v�ȃ|�P���������C�߂܂�����
  num = ZUKANSAVE_GetLocalSeeCompCount( zw, heapID );

  if( num >= ZUKANSAVE_LOCAL_COMP_NUM ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�S���}�ӊ����ɕK�v�ȃ|�P�����̕ߊl��
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 *
 * @return	�ߊl��
 */
//--------------------------------------------------------------------------------------------
u16 ZUKANSAVE_GetZenkokuGetCompCount( const ZUKAN_SAVEDATA * zw )
{
  int i;
  u16 num;

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

//--------------------------------------------------------------------------------------------
/**
 * @brief		�n���}�ӊ����ɕK�v�ȃ|�P�����̕ߊl��
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 * @param		heapID	�q�[�v�h�c
 *
 * @return	�ߊl��
 */
//--------------------------------------------------------------------------------------------
u16 ZUKANSAVE_GetLocalGetCompCount( const ZUKAN_SAVEDATA * zw, HEAPID heapID )
{
  u16 * buf;
  u16 i;
  u16 num=0;

	buf = POKE_PERSONAL_GetZenkokuToChihouArray( heapID, NULL );

  for( i=1; i<=ZUKANSAVE_ZENKOKU_MONSMAX; i++ ){
    if( ZUKANSAVE_GetPokeGetFlag( zw, i ) == TRUE ){
      if( buf[i] != POKEPER_CHIHOU_NO_NONE ){
        if( check_LocalCompMonsno( i ) == TRUE ){
          num++;
        }
      }
    }
  }

  GFL_HEAP_FreeMemory( buf );

  return num;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�n���}�ӊ����ɕK�v�ȃ|�P������������
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 * @param		heapID	�q�[�v�h�c
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
u16 ZUKANSAVE_GetLocalSeeCompCount( const ZUKAN_SAVEDATA * zw, HEAPID heapID )
{
  u16 * buf;
  u16 i;
  u16 num=0;

	buf = POKE_PERSONAL_GetZenkokuToChihouArray( heapID, NULL );

  for( i=1; i<=ZUKANSAVE_ZENKOKU_MONSMAX; i++ ){
    if( ZUKANSAVE_GetPokeSeeFlag( zw, i ) == TRUE ){
      if( buf[i] != POKEPER_CHIHOU_NO_NONE ){
        if( check_LocalCompMonsno( i ) == TRUE ){
          num++;
        }
      }
    }
  }

  GFL_HEAP_FreeMemory( buf );

  return num;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�}�Ӄe�L�X�g�o�[�W�����`�F�b�N
 *
 * @param		zw						�}�ӃZ�[�u�f�[�^
 * @param		monsno				�|�P�����ԍ�
 * @param		country_code	���R�[�h
 *
 * @retval	"TRUE = �\����"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ZUKANSAVE_GetTextVersionUpFlag( const ZUKAN_SAVEDATA * zw, u16 monsno, u32 country_code )
{
  zukan_incorrect(zw);

	// ���R�[�h�ő� or ���ԃR�[�h
	if( country_code > 8 || country_code == 6 ){
		return FALSE;
	}

	// �V�K�ǉ��|�P�����͕\���ł��Ȃ�
	if( monsno > POKEZUKAN_TEXT_POSSIBLE ){
		return FALSE;
	}

	// ����U�Ԃ����ԂȂ̂ŁA�X�y�C����ȍ~��-1
	if( country_code >= LANG_SPAIN ){
		country_code -= 1;
	}

	return check_bit( zw->TextVersionUp, (monsno-1)*TEXTVER_VER_MAX+(country_code-1) );
}





//============================================================================================
//	�f�o�b�O����
//============================================================================================
#ifdef	PM_DEBUG

static void DebugDataClearCore( ZUKAN_SAVEDATA * zw, u16 mons )
{
	u32	i;

	reset_bit_mons( (u8 *)zw->get_flag, mons );				// �ߊl�t���O�N���A

	for( i=SEE_FLAG_MALE; i<=SEE_FLAG_F_RARE; i++ ){
		reset_bit_mons( (u8 *)zw->sex_flag[i], mons );	// �����t���O�N���A
		reset_bit_mons( (u8 *)zw->draw_sex[i], mons );	// �{���������t���O�N���A
	}

	{	// �t�H�����f�[�^�N���A
		s32	cnt = 0;
		i = 0;
		while( FormTable[i][0] != 0 ){
			if( FormTable[i][0] == mons ){
				u16	j, k;
				for( j=0; j<FormTable[i][1]; j++ ){
					for( k=COLOR_NORMAL; k<=COLOR_RARE; k++ ){
						reset_bit( (u8 *)zw->form_flag[k], cnt+j );			// �t�H�����t���O�N���A
						reset_bit( (u8 *)zw->draw_form[k], cnt+j );			// �{�����t�H�����t���O�N���A
					}
				}
			}
			cnt += FormTable[i][1];
			i++;
		}
	}

	// ����t���O�N���A
	if( mons <= POKEZUKAN_TEXT_POSSIBLE ){
		for( i=0; i<TEXTVER_VER_MAX; i++ ){
			reset_bit( zw->TextVersionUp, (mons-1)*TEXTVER_VER_MAX+i );
		}
	}

	// �p�b�`�[���p�������N���A
  if( mons == MONSNO_PATTIIRU ){
		zw->PachiRandom = 0;
	}
}

// �f�[�^�N���A
void ZUKANSAVE_DebugDataClear( ZUKAN_SAVEDATA * zw, u16 start, u16 end )
{
	u32	i;

	if( zw->defaultMonsNo >= start && zw->defaultMonsNo <= end ){
		zw->defaultMonsNo = 0;		// �{�����̃|�P�����ԍ��N���A
	}

	for( i=start; i<=end; i++ ){
		DebugDataClearCore( zw, i );
	}
}

// �����ݒ�
void ZUKANSAVE_DebugDataSetSee( ZUKAN_SAVEDATA * zw, u16 start, u16 end, HEAPID heapID )
{
	POKEMON_PARAM * pp;
	u32	i;

	for( i=start; i<=end; i++ ){
		DebugDataClearCore( zw, i );
    pp = PP_Create( i, 50, 0, heapID );
		ZUKANSAVE_SetPokeSee( zw, pp );
    GFL_HEAP_FreeMemory( pp );
	}
}

// �ߊl�ݒ�
void ZUKANSAVE_DebugDataSetGet( ZUKAN_SAVEDATA * zw, u16 start, u16 end, HEAPID heapID )
{
	POKEMON_PARAM * pp;
	u32	i;

	for( i=start; i<=end; i++ ){
		DebugDataClearCore( zw, i );
    pp = PP_Create( i, 50, 0, heapID );
		ZUKANSAVE_SetPokeGet( zw, pp );
    GFL_HEAP_FreeMemory( pp );
	}
}

#endif	// PM_DEBUG