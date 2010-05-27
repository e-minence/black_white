//============================================================================================
/**
 * @file		zukan_savedata.h
 * @brief		�}�ӃZ�[�u�f�[�^�A�N�Z�X�����Q�w�b�_
 * @author	mori / tamada GAME FREAK inc.
 * @date		2009.10.26
 *
 * �W���E�g���C�b�V�����H�Ƃ��̓x�ɖ��O������������̂��Ȃ�Ȃ̂ŁA
 * ���ꂩ��͑S���́uZENKOKU�v�A�n���}�ӂ̎��́uLOCAL�v�ƋL�q���鎖�ɂ��܂��B
 *
 */
//============================================================================================
#pragma	once

#include "gamesystem/gamedata_def.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"


//============================================================================================
//	�萔��`
//============================================================================================

#define ISSHU_MAX ( ZUKANNO_ISSHU_END+1 )     // �C�b�V���}�Ӑ��i�r�N�e�B���{�P�j

enum{
  ZUKANSAVE_RANDOM_PACHI = 0,   // �p�b�`�[��������

  // �|�P�������ʎ擾�p
  ZUKANSAVE_SEX_FIRST = 0,
  ZUKANSAVE_SEX_SECOND,

  // ZUKANSAVE_GetPokeSexFlag�n�֐��̃G���[�R�[�h
  ZUKANSAVE_GET_SEX_ERR = 0xffffffff,

  // �S���}�Ӎő�
  ZUKANSAVE_ZENKOKU_MONSMAX   = MONSNO_END,

  // �C�b�V���}�Ӎő吔
  ZUKANSAVE_LOCAL_MONSMAX = ISSHU_MAX,

  // �[���R�N�}�ӊ����ɕK�v�ȃ|�P�����̐�
  ZUKANSAVE_ZENKOKU_COMP_NOPOKE_NUM = 15,  // �s�K�v�ȃ|�P������
  ZUKANSAVE_ZENKOKU_COMP_NUM  = ZUKANSAVE_ZENKOKU_MONSMAX - ZUKANSAVE_ZENKOKU_COMP_NOPOKE_NUM,

  // �C�b�V���}�ӊ����ɕK�v�ȃ|�P�����̐�
  ZUKANSAVE_LOCAL_COMP_NOPOKE_NUM = 6,  // �s�K�v�ȃ|�P������
  ZUKANSAVE_LOCAL_COMP_NUM  = ZUKANSAVE_LOCAL_MONSMAX - ZUKANSAVE_LOCAL_COMP_NOPOKE_NUM,
};

//----------------------------------------------------------
// ����J�^ �\���̐錾
//----------------------------------------------------------
typedef struct ZUKAN_SAVEDATA ZUKAN_SAVEDATA;


//============================================================================================
//	�v���g�^�C�v�錾
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
extern int ZUKANSAVE_GetWorkSize(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		�}�ӃZ�[�u�f�[�^�̏�����
 *
 * @param		zw		�}�ӃZ�[�u�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZUKANSAVE_Init( ZUKAN_SAVEDATA * zw );

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
extern ZUKAN_SAVEDATA * ZUKAN_SAVEDATA_GetZukanSave( SAVE_CONTROL_WORK * sv );

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
extern ZUKAN_SAVEDATA * GAMEDATA_GetZukanSave( GAMEDATA * gamedata );

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
extern void ZUKANSAVE_SetZenkokuZukanFlag( ZUKAN_SAVEDATA * zw );

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
extern BOOL ZUKANSAVE_GetZenkokuZukanFlag( const ZUKAN_SAVEDATA * zw );

//--------------------------------------------------------------------------------------------
/**
 * @brief  �O���t�B�b�N�o�[�W�����p�@�\�g���t���O�ݒ�
 *
 * @param		zw		�}�ӃZ�[�u�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZUKANSAVE_SetGraphicVersionUpFlag( ZUKAN_SAVEDATA * zw );

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
extern BOOL ZUKANSAVE_GetGraphicVersionUpFlag( const ZUKAN_SAVEDATA * zw );

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
extern void ZUKANSAVE_SetZukanMode( ZUKAN_SAVEDATA * zw, BOOL mode );

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
extern BOOL ZUKANSAVE_GetZukanMode( const ZUKAN_SAVEDATA * zw );

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
extern void ZUKANSAVE_SetDefaultMons( ZUKAN_SAVEDATA * zw, u16 mons );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�{�����̃|�P�����ԍ����擾
 *
 * @param		zw		�}�ӃZ�[�u�f�[�^
 *
 * @return	�|�P�����ԍ�
 */
//--------------------------------------------------------------------------------------------
extern u16 ZUKANSAVE_GetDefaultMons( const ZUKAN_SAVEDATA * zw );

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
extern void ZUKANSAVE_SetDrawData( ZUKAN_SAVEDATA * zw, u16 mons, u32 sex, BOOL rare, u32 form );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�ő�t�H�������擾
 *
 * @param		mons		�|�P�����ԍ�
 *
 * @return	�ő�t�H������
 */
//--------------------------------------------------------------------------------------------
extern u32	ZUKANSAVE_GetFormMax( u16 mons );

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
extern void ZUKANSAVE_GetDrawData( ZUKAN_SAVEDATA * zw, u16 mons, u32 * sex, BOOL * rare, u32 * form, HEAPID heapID );

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
extern void ZUKANSAVE_SetPokeGet( ZUKAN_SAVEDATA * zw, POKEMON_PARAM * pp );

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
extern BOOL ZUKANSAVE_GetPokeGetFlag( const ZUKAN_SAVEDATA * zw, u16 monsno );

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
extern u16 ZUKANSAVE_GetZukanPokeGetCount( const ZUKAN_SAVEDATA * zw, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����ߊl���擾�i�S���j
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 *
 * @return	�ߊl��
 */
//--------------------------------------------------------------------------------------------
extern u16 ZUKANSAVE_GetPokeGetCount( const ZUKAN_SAVEDATA * zw );

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
extern u16 ZUKANSAVE_GetLocalPokeGetCount( const ZUKAN_SAVEDATA * zw, HEAPID heapID );

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
extern void ZUKANSAVE_SetPokeSee( ZUKAN_SAVEDATA * zw, POKEMON_PARAM * pp );

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
extern BOOL ZUKANSAVE_GetPokeSeeFlag( const ZUKAN_SAVEDATA * zw, u16 monsno );

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
extern u16 ZUKANSAVE_GetZukanPokeSeeCount( const ZUKAN_SAVEDATA * zw, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�������������擾�i�S���j
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
extern u16 ZUKANSAVE_GetPokeSeeCount( const ZUKAN_SAVEDATA * zw );

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
extern u16 ZUKANSAVE_GetLocalPokeSeeCount( const ZUKAN_SAVEDATA * zw, HEAPID heapID );

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
extern BOOL ZUKANSAVE_CheckPokeSeeForm( const ZUKAN_SAVEDATA * zw, u16 monsno, int sex, int rare, int form );

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
extern u32 ZUKANSAVE_GetPokeRandomFlag( const ZUKAN_SAVEDATA * zw, u8 monsno );

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
extern BOOL ZUKANSAVE_CheckZenkokuComp( const ZUKAN_SAVEDATA * zw );

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
extern BOOL ZUKANSAVE_CheckLocalGetComp( const ZUKAN_SAVEDATA * zw, HEAPID heapID );

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
extern BOOL ZUKANSAVE_CheckLocalSeeComp( const ZUKAN_SAVEDATA * zw, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * @brief		�S���}�ӊ����ɕK�v�ȃ|�P�����̕ߊl��
 *
 * @param		zw			�}�ӃZ�[�u�f�[�^
 *
 * @return	�ߊl��
 */
//--------------------------------------------------------------------------------------------
extern u16 ZUKANSAVE_GetZenkokuGetCompCount( const ZUKAN_SAVEDATA * zw );

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
extern u16 ZUKANSAVE_GetLocalGetCompCount( const ZUKAN_SAVEDATA * zw, HEAPID heapID );

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
extern u16 ZUKANSAVE_GetLocalSeeCompCount( const ZUKAN_SAVEDATA * zw, HEAPID heapID );

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
extern BOOL ZUKANSAVE_GetTextVersionUpFlag( const ZUKAN_SAVEDATA * zw, u16 monsno, u32 country_code );



//============================================================================================
//  �f�o�b�O�p
//============================================================================================
#ifdef	PM_DEBUG
extern void ZUKANSAVE_DebugDataClear( ZUKAN_SAVEDATA * zw, u16 start, u16 end );
extern void ZUKANSAVE_DebugDataSetSee( ZUKAN_SAVEDATA * zw, u16 start, u16 end, HEAPID heapID );
extern void ZUKANSAVE_DebugDataSetGet( ZUKAN_SAVEDATA * zw, u16 start, u16 end, HEAPID heapID );
#endif	// PM_DEBUG

