//======================================================================
/**
 * @file	name_input.h
 * @brief	���O����
 * @author	Toru=Nagihashi
 * @data	09/10/07
 */
//======================================================================

#ifndef NAME_INPUT_H__
#define NAME_INPUT_H__

#include "savedata\config.h"
#include "savedata\box_savedata.h"
#include "savedata/save_control_intr.h"
#include "savedata/misc.h"

//=============================================================================
/**
 *	��`
 */
//=============================================================================

//���[�h��NAMEIN_FRIENDNAME�̂Ƃ�param1�ւ����ƁA�F�B�蒠�̏������͗p�̂͂Ă�OBJ���o�܂�
#define NAMEIN_TRAINER_VIEW_UNKNOWN  (0xFFFF)


//-------------------------------------
///	���O���̓��[�h  
//=====================================
typedef enum 
{
	NAMEIN_MYNAME = 0,	// �����̖��O	param1�����̐���
	NAMEIN_POKEMON,			// �|�P�����̖��O param1�����X�^�[�ԍ� param2�t�H�����ԍ�
	NAMEIN_BOX,					// �{�b�N�X�̖��O
	NAMEIN_RIVALNAME,		// ���C�o���l�[�� WB�ł͎g��Ȃ��Ȃ�܂���
	NAMEIN_FRIENDNAME,  // WIFI�Ƃ������蒠�ɏ������ނƂ������̖��O param1 ���j�I���̌�����(NAMEIN_TRAINERVIEW_UNKNOWN�ł���΂Ƃ������蒠�̏������͗p�́H���o�܂�)
  NAMEIN_GREETING_WORD, // ���ꂿ�������A���t param1�����̐���
  NAMEIN_THANKS_WORD,   // ���ꂿ�������猾�t param1�����̐���
  NAMEIN_FREE_WORD,     // ���ꂿ�����t���[���[�hparam1�����̐���

	NAMEIN_MAX,
}NAMEIN_MODE;

//-------------------------------------
///	���͕�����
//    �ȉ�EOM�܂܂Ȃ������ł�
//=====================================
// �|�P�������O���͂̒���
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_POKEMON_LENGTH	(  5 )
#else
#define NAMEIN_POKEMON_LENGTH	( 10 )
#endif

// �g���[�i�[�����͂̒���
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_PERSON_LENGTH	(  5 )
#else
#define NAMEIN_PERSON_LENGTH	( 7 )
#endif


// �O���[�v�����͂̒���
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_GROUP_LENGTH		(  5 )
#else
#define NAMEIN_GROUP_LENGTH		( 7 )
#endif

// �{�b�N�X���͂̒���
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_BOX_LENGTH		( 8 )
#else
#define NAMEIN_BOX_LENGTH		( 8 )
#endif

// ����Ⴂ�u���A�v���͂̒���
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_GREETING_WORD_LENGTH		( 8 )
#else
#define NAMEIN_GREETING_WORD_LENGTH		( 8 )
#endif

// ����Ⴂ�u����v���͂̒���
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_THANKS_WORD_LENGTH		( 8 )
#else
#define NAMEIN_THANKS_WORD_LENGTH		( 8 )
#endif

// ����Ⴂ�@�t���[���[�h
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_FREE_WORD_LENGTH		( 8 )
#else
#define NAMEIN_FREE_WORD_LENGTH		( 8 )
#endif

//=============================================================================
/**
 *	�\����
 */
//=============================================================================
//-------------------------------------
///	PROC�ɓn���p�����[�^
//=====================================
typedef struct
{
	NAMEIN_MODE mode;		//[in]���̓��[�h(enum�Q�Ɓj

	union								//[in]��ނ��ƂɈႤ�����P
	{	
		u16 param1;				//�e���[�h�ɂĕK�v
		u16 hero_sex;			//NAMEIN_MYNAME			�c��l���̐���
		u16 mons_no;			//NAMEIN_POKEMON		�c�����X�^�[�ԍ�
		u16 fld_person_id;//NAMEIN_FRIENDNAME	�c�F�B�̎p�i�t�B�[���h�RDOBJ�j
	};

	union								//[in]��ނ��ƂɈႤ�����Q
	{	
		u16 param2;				//NAMEIN_POKEMON�ȊO�͕K�v�Ȃ�
		u16 form;					//NAMEIN_POKEMON	�c�t�H�����ԍ�
	};
	const POKEMON_PARAM *pp;	//[in]�j�b�N�l�[���o�^��PP����쐬����ꍇ

  const STRBUF       *box_strbuf;   // [in] �|�P�����ߊl�Ń{�b�N�X�]���ɂȂ������ɕK�v
                                    // ( mode==NAMEIN_POKEMON && box_strbuf!=NULL ) �̂Ƃ��A�|�P�����ߊl��
  const BOX_MANAGER  *box_manager;  // [in] box_strbuf!=NULL�̂Ƃ��̂ݗL��
  u32                box_tray;      // [in] box_strbuf!=NULL�̂Ƃ��̂ݗL��

	u32 wordmax;		// [in]���͕����ő吔

	BOOL cancel;		// [out]���O���͂��I���������ɔ��f�����t���O�B
									// ���͕������O�����������B�������͍ŏ��Ɠ����������ꍇ�͂��̃t���O��TRUE

	STRBUF *strbuf; // [in/out]����STRBUF�ɂ͂Q�̈Ӗ������݂���B
									// �P�͖��O���͉�ʂ���f�[�^���󂯎�郏�[�N�ł��邱�ƁB
									// ���O���͂��I������炱������R�s�[����
									// �����P�́A���O���͂ɍs���Ƃ��ɂ����ɕ�������i�[���Ă�����,
									// ���O���͉�ʂ͂��̕������\�����Ȃ���J�n����B�����Ƃ��Ă��@�\����

  INTR_SAVE_CONTROL *p_intr_sv;  // [in]�����ŁA�Z�[�u���Ȃ��疼�O���͂���(�C���g����ʗp)

  MISC              *p_misc;    //  [in]�����ŃL�[�{�[�h�^�C�v��ۑ����邽�߂̃Z�[�u�f�[�^ NULL��n���ƃZ�[�u���܂���
                                                                                          //��NULL��n���ėǂ��̂̓C���g�������ł�

} NAMEIN_PARAM;

//=============================================================================
/**
 *	PROC
 */
//=============================================================================
//-------------------------------------
///	NAMEIN�I�[�o�[���C�֌W
//=====================================
FS_EXTERN_OVERLAY(namein);

//-------------------------------------
///	PROC�f�[�^
//=====================================
extern const GFL_PROC_DATA NameInputProcData;

//=============================================================================
/**
 *	�O���Q��  �ȉ��֐���notwifi�ɂ����Ă���܂�
 */
//=============================================================================
//-------------------------------------
///	PARAM�쐬�A�j��
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	NAMEIN_PARAM���쐬
 *					����͉��L�ANameIn_FreeParam�ōs���Ă�������
 *
 *	@param	HEAPID		�q�[�vID
 *	@param	mode			���͂̎��
 *	@param	param1		��ނɂ�����1
 *	@param	param2		��ނɂ�����2
 *	@param	wordmax		�������͐�
 *	@param	default_str	�����ɓ��͂���Ă��镶���B����Ȃ��ꍇ��NULL
 *
 *	@return	NAMEIN_PARAM
 */
//-----------------------------------------------------------------------------
extern NAMEIN_PARAM *NAMEIN_AllocParam( HEAPID heapId, NAMEIN_MODE mode, int param1, int param2, int wordmax, const STRBUF *default_str, MISC *p_misc );
//----------------------------------------------------------------------------
/**
 *	@brief	NAMEIN_PARAM���
 *
 *	@param		NAMEIN_PARAM
 */
//-----------------------------------------------------------------------------
extern void NAMEIN_FreeParam( NAMEIN_PARAM *param );
//-------------------------------------
///	�|�P�����j�b�N�l�[����p�p�����[�^�쐬�A�j��
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����j�b�N�l�[���o�^��PP����s���ꍇ��NAMEIN_PARAM�쐬
 *					�����NAMEIN_FreeParam�ōs���Ă�������
 *
 *	@param	HEAPID		�q�[�vID
 *	@param	wordmax		�������͐�
 *	@param	default_str	�����ɓ��͂���Ă��镶���B����Ȃ��ꍇ��NULL
 *
 *	@return	NAMEIN_PARAM
 */
//-----------------------------------------------------------------------------
extern NAMEIN_PARAM *NAMEIN_AllocParamPokemonByPP( HEAPID heapId, const POKEMON_PARAM *pp, int wordmax, const STRBUF *default_str, MISC *p_misc );

//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����j�b�N�l�[���o�^��PP����s���ꍇ��NAMEIN_PARAM�쐬
 *	        �|�P�����ߊl���͂�������g�p���Ă�������
 *					�����NAMEIN_FreeParam�ōs���Ă�������
 *
 *	@param	HEAPID		�q�[�vID
 *	@param	wordmax		�������͐�
 *	@param	default_str	�����ɓ��͂���Ă��镶���B����Ȃ��ꍇ��NULL
 *	@param  box_strbuf   [�|�P�����j�b�N�l�[��]�́����̃p�\�R����[�{�b�N�X��]�ɓ]�����ꂽ�I
 *	@param  box_manager  �{�b�N�X�}�l�[�W��(box_strbuf!=NULL�̂Ƃ������g���Ȃ�) 
 *	@param  box_tray     �{�b�N�X�g���C�i���o�[(box_strbuf!=NULL�̂Ƃ������g���Ȃ�) 
 *
 *	@return	NAMEIN_PARAM
 */
//-----------------------------------------------------------------------------
extern NAMEIN_PARAM *NAMEIN_AllocParamPokemonCapture( HEAPID heapId, const POKEMON_PARAM *pp, int wordmax, const STRBUF *default_str,
                                                      const STRBUF *box_strbuf, const BOX_MANAGER *box_manager, u32 box_tray, MISC *p_misc );

//------------------------------------
///	PARAM����̎擾
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief		NAMEIN_PARAM����̕�����擾
 *
 *	@param		param		NAMEIN_PARAM
 *	@param		strbuf	�󂯎�蕶����o�b�t�@
 */
//-----------------------------------------------------------------------------
extern void NAMEIN_CopyStr( const NAMEIN_PARAM *param, STRBUF *strbuf );
//----------------------------------------------------------------------------
/**
 *	@brief		NAMEIN_PARAM����̕�����R�s�[�쐬
 *
 *	@param		param		NAMEIN_PARAM
 *	@param		HEAPID	������쐬�q�[�vID
 *
 *	@return		�R�s�[���č쐬����STRBUF
 */
//-----------------------------------------------------------------------------
extern STRBUF* NAMEIN_CreateCopyStr( const NAMEIN_PARAM *param, HEAPID heapID );
//----------------------------------------------------------------------------
/**
 *	@brief	NAMEIN_PROC�̏I���擾
 *
 *	@param	param	NAMEIN_PARAM
 *
 *	@retval	TRUE�Ńf�t�H���g�����Ɠ���������or�������͂����I��
 *	@retval	FALSE�Œʏ���͂ŏI��
 */
//-----------------------------------------------------------------------------
extern BOOL NAMEIN_IsCancel( const NAMEIN_PARAM *param );


//=============================================================================
/**
 *    SE�v���Z�b�g
 */
//=============================================================================
extern const u32 NAMEIN_SE_PresetData[];
extern const u32 NAMEIN_SE_PresetNum;


#endif	//NAME_INPUT_H__
