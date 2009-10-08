//============================================================================================
/**
 * @brief	box_savedata.h
 * @brief	�{�b�N�X�Z�[�u�f�[�^�n
 * @date	2008.11.07
 * @author	ariizumi
 */
//============================================================================================

#pragma once

#include "poke_tool/poke_tool.h"
#include "savedata/save_control.h"

//============================================================================================
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	�{�b�N�X�f�[�^�̕s���S�^��`
 */
//---------------------------------------------------------------------------
typedef struct _BOX_MANAGER BOX_MANAGER;
typedef struct _BOX_SAVEDATA BOX_SAVEDATA;
typedef struct _BOX_TRAY_DATA BOX_TRAY_DATA;

//------------------------------------------------------------
/**
 *   �萔
 */
//------------------------------------------------------------
#define BOX_MAX_RAW				(5)
#define BOX_MAX_COLUMN			(6)
#define BOX_MAX_TRAY			(24)
#define BOX_TRAYNAME_MAXLEN		(8)
#define BOX_TRAYNAME_BUFSIZE	(20)	// ���{��W�����{EOM�B�C�O�ŗp�̗]�T�����Ă��̒��x�B
#define BOX_MAX_POS				(BOX_MAX_RAW*BOX_MAX_COLUMN)


#define BOX_NORMAL_WALLPAPER_MAX	(16)
#define BOX_EX_WALLPAPER_MAX		(8)
#define BOX_PL_EX_WALLPAPER_MAX		(8)	//�v���`�i�Œǉ����ꂽ�閧�ǎ�
#define BOX_GS_EX_WALLPAPER_MAX		(8)	//����Œǉ����ꂽ�閧�ǎ�
#define BOX_TOTAL_WALLPAPER_MAX		(BOX_NORMAL_WALLPAPER_MAX+BOX_EX_WALLPAPER_MAX)
#define BOX_TOTAL_WALLPAPER_MAX_PL	(BOX_NORMAL_WALLPAPER_MAX+BOX_EX_WALLPAPER_MAX+BOX_PL_EX_WALLPAPER_MAX)
#define BOX_TOTAL_WALLPAPER_MAX_GS	(BOX_NORMAL_WALLPAPER_MAX+BOX_EX_WALLPAPER_MAX+BOX_PL_EX_WALLPAPER_MAX+BOX_GS_EX_WALLPAPER_MAX)

#define BOXDAT_TRAYNUM_CURRENT		(0xffffffff)	///< �g���C�i���o�[�������Ɏ��֐��Ŏw�肷��ƁA�J�����g���w�肵�����ƂɂȂ�B
#define BOXDAT_TRAYNUM_ERROR		(BOX_MAX_TRAY)	///< �󂫂̂���g���C�T�[�`�ŁA������Ȃ��������Ȃǂ̖߂�l


//------------------------------------------------------------------
/**
 * �{�b�N�X�f�[�^�̈���쐬�i�Z�[�u�f�[�^�ǂݍ��ݎ��ɂP�񂾂��j
 *
 * @param   heapID		�쐬��q�[�v�h�c
 *
 * @retval  BOX_MANAGER*	�쐬���ꂽ�{�b�N�X�f�[�^�̃|�C���^
 */
//------------------------------------------------------------------
extern BOX_MANAGER* BOXDAT_Create( u32 heapID );


//------------------------------------------------------------------
/**
 * �{�b�N�X�f�[�^�̓��e������������
 *
 * @param   boxdat		�{�b�N�X�f�[�^�̃|�C���^
 */
//------------------------------------------------------------------
extern void BOXDAT_Init( BOX_SAVEDATA* boxdat );

//------------------------------------------------------------------
/**
 * �{�b�N�X�f�[�^�̈�̑��T�C�Y��Ԃ��i�Z�[�u�f�[�^�Ǘ��p�j
 *
 * @retval  u32		�{�b�N�X�f�[�^���T�C�Y
 */
//------------------------------------------------------------------
extern u32 BOXDAT_GetTotalSize( void );


//BOX_MANAGER��gamedata�o�R�̃A�N�Z�X�ɂȂ�܂����B
//------------------------------------------------------------------
/**
 * �{�b�N�X�Ǘ��}�l�[�W���[�̎擾
 */
//------------------------------------------------------------------
extern BOX_MANAGER * BOX_DAT_InitManager( const HEAPID heapId , SAVE_CONTROL_WORK * sv);
//------------------------------------------------------------------
/**
 * �{�b�N�X�Ǘ��}�l�[�W���[�̊J��
 */
//------------------------------------------------------------------
extern void BOX_DAT_ExitManager( BOX_MANAGER *box );

//------------------------------------------------------------------
/**
 * �{�b�N�X�S�̂��炩��󂫗̈��T���ă|�P�����f�[�^���i�[
 *
 * @param   box			�{�b�N�X�f�[�^�|�C���^
 * @param   poke		�|�P�����f�[�^
 *
 * @retval  BOOL		TRUE=�i�[���ꂽ�^FALSE=�󂫂�����
 */
//------------------------------------------------------------------
extern BOOL BOXDAT_PutPokemon( BOX_MANAGER* box, POKEMON_PASO_PARAM* poke );

//------------------------------------------------------------------
/**
 * �{�b�N�X���w�肵�ă|�P�����f�[�^�i�[
 *
 * @param   box			�{�b�N�X�f�[�^�|�C���^
 * @param   boxNum		���Ԗڂ̃{�b�N�X�Ɋi�[���邩
 * @param   poke		�|�P�����f�[�^
 *
 * @retval  BOOL		TRUE=�i�[���ꂽ�^FALSE=�󂫂�����
 */
//------------------------------------------------------------------
extern BOOL BOXDAT_PutPokemonBox( BOX_MANAGER* box, u32 boxNum, POKEMON_PASO_PARAM* poke );

//------------------------------------------------------------------
/**
 * �{�b�N�X�A�ʒu���w�肵�ă|�P�����f�[�^�i�[
 *
 * @param   box			�{�b�N�X�f�[�^�|�C���^
 * @param   boxNum		���Ԗڂ̃{�b�N�X�Ɋi�[���邩
 * @param   x			�{�b�N�X���̈ʒu�w
 * @param   y			�{�b�N�X���̈ʒu�x
 * @param   poke		�|�P�����f�[�^
 *
 * @retval  BOOL		TRUE=�i�[���ꂽ�^FALSE=�󂫂�����
 */
//------------------------------------------------------------------
extern BOOL BOXDAT_PutPokemonPos( BOX_MANAGER* box, u32 boxNum, u32 pos, POKEMON_PASO_PARAM* poke );


//------------------------------------------------------------------
/**
 * �P�ł��󂫂̂���g���C�i���o�[��Ԃ��i�J�����g���猟���J�n����j
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 *
 * @retval  u32		�󂫂̂���g���C�i���o�[�^������Ȃ���� BOXDAT_TRAYNUM_ERROR ���Ԃ�
 */
//------------------------------------------------------------------
extern u32 BOXDAT_GetEmptyTrayNumber( const BOX_MANAGER* box );

//------------------------------------------------------------------
/**
 * �w��ʒu�̃|�P���������ւ���
 *
 * @param   box			�{�b�N�X�f�[�^�|�C���^
 * @param   trayNum		���Ԗڂ̃{�b�N�X��
 * @param   pos1		�{�b�N�X���̈ʒu
 * @param   pos2		�{�b�N�X���̈ʒu
 *
 */
//------------------------------------------------------------------
extern void BOXDAT_ChangePokeData( BOX_MANAGER * box, u32 trayNum, u32 pos1, u32 pos2 );

//------------------------------------------------------------------
/**
 * �ʒu���w�肵�ăJ�����g�{�b�N�X�̃|�P�����f�[�^�N���A
 *
 * @param   box			�{�b�N�X�f�[�^�|�C���^
 * @param   trayNumber	���Ԗڂ̃{�b�N�X��
 * @param   pos			�{�b�N�X���̈ʒu
 *
 */
//------------------------------------------------------------------
extern void BOXDAT_ClearPokemon( BOX_MANAGER* box, u32 trayNum, u32 pos );

//------------------------------------------------------------------
/**
 * �{�b�N�X�S�̂Ɋ܂܂��|�P��������Ԃ�
 *
 * @param   box		�{�b�N�X�f�[�^�|�C���^
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
extern u32 BOXDAT_GetPokeExistCountTotal( const BOX_MANAGER* box );
extern u32 BOXDAT_GetPokeExistCount2Total( const BOX_MANAGER* box );

extern u32 BOXDAT_GetEmptySpaceTotal( const BOX_MANAGER* box );
extern u32 BOXDAT_GetEmptySpaceTray( const BOX_MANAGER* box, u32 trayNum );

extern u32 BOXDAT_GetCureentTrayNumber( const BOX_MANAGER* box );
extern void BOXDAT_SetCureentTrayNumber( BOX_MANAGER* box, u32 num );
extern u32 BOXDAT_GetWallPaperNumber( const BOX_MANAGER* box, u32 trayNumber );
extern void BOXDAT_SetWallPaperNumber( BOX_MANAGER* box, u32 trayNum, u32 wallPaperNumber );
extern void BOXDAT_GetBoxName( const BOX_MANAGER* box, u32 trayNumber, STRBUF* buf );
extern void BOXDAT_SetBoxName( BOX_MANAGER* box, u32 trayNumber, const STRBUF* src );
extern u32 BOXDAT_GetPokeExistCount( const BOX_MANAGER* box, u32 trayNumber );
extern u32 BOXDAT_GetPokeExistCount2( const BOX_MANAGER* box, u32 trayNumber );
extern POKEMON_PASO_PARAM* BOXDAT_GetPokeDataAddress( const BOX_MANAGER* box, u32 boxNum, u32 pos );
extern BOOL BOXDAT_GetEmptyTrayNumberAndPos( const BOX_MANAGER* box, int* trayNum, int* pos );
extern u32 BOXDAT_PokeParaGet( const BOX_MANAGER* box, u32 trayNum, u32 pos, int param, void* buf );
extern void BOXDAT_PokeParaPut( BOX_MANAGER* box, u32 trayNum, u32 pos, int param, u32 arg );


//==============================================================================================
// ���������N���u�ǎ�
//==============================================================================================
extern void BOXDAT_SetDaisukiKabegamiFlag( BOX_MANAGER* box, u32 number );
extern BOOL BOXDAT_GetDaisukiKabegamiFlag( const BOX_MANAGER* box, u32 number );
extern u32  BOXDAT_GetDaiukiKabegamiCount( const BOX_MANAGER* box );


extern void BOXDAT_SetTrayUseBit(BOX_MANAGER* box, const u8 inTrayIdx);
extern void BOXDAT_SetTrayUseBitAll(BOX_MANAGER* box);
extern void BOXDAT_ClearTrayUseBits(BOX_MANAGER* box);
extern u32 BOXDAT_GetTrayUseBits(const BOX_MANAGER* box);
extern u32 BOXDAT_GetOneBoxDataSize(void);
extern void BOXDAT_CheckBoxDummyData(BOX_MANAGER* box);


extern void BOXDAT_SetPPPData_Tray( u8 trayIdx , void *dataPtr , BOX_MANAGER *boxData );

//==============================================================================================
// �{�b�N�X�f�[�^��������
//==============================================================================================
extern void BOXTRAYDAT_Init( BOX_TRAY_DATA* traydat );
extern u32 BOXTRAYDAT_GetTotalSize( void );
