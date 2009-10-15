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

//=============================================================================
/**
 *	��`
 */
//=============================================================================
//-------------------------------------
///	���O���̓��[�h
//=====================================
typedef enum 
{
	NAMEIN_MYNAME = 0,	// �����̖��O	param1�����̐���
	NAMEIN_POKEMON,		// �|�P�����̖��O param1�����X�^�[�ԍ� param2�t�H�����ԍ�
	NAMEIN_BOX,			// �{�b�N�X�̖��O
	NAMEIN_RIVALNAME,	// ���C�o���l�[��
	NAMEIN_FRIENDNAME,  // WIFI�Ƃ������蒠�ɏ������ނƂ������̖��O param1 �t�B�[���h�RDOBJ
//	NAMEIN_FRIENDCODE,	// �Ƃ������R�[�h
//	NAMEIN_RANDOMGROUP, // �����̎�O���[�v�̖��O
//	NAMEIN_STONE,		// �Δ�(�z�z�j
	NAMEIN_MAX,
}NAMEIN_MODE;

//-------------------------------------
///	���͕�����
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
	NAMEIN_MODE mode;		// ���̓��[�h(enum�Q�Ɓj

	union								//��ނ��ƂɈႤ�����P
	{	
		u16 param1;				//�e���[�h�ɂĕK�v
		u16 hero_sex;			//NAMEIN_MYNAME			�c��l���̐���
		u16 mons_no;			//NAMEIN_POKEMON		�c�����X�^�[�ԍ�
		u16 fld_person_id;//NAMEIN_FRIENDNAME	�c�F�B�̎p�i�t�B�[���h�RDOBJ�j
	};

	union								//��ނ��ƂɈႤ�����Q
	{	
		u16 param2;				//NAMEIN_POKEMON�ȊO�͕K�v�Ȃ�
		u16 form;					//NAMEIN_POKEMON	�c�t�H�����ԍ�
	};

	BOOL cancel;		// ���O���͂��I���������ɔ��f�����t���O�B
									// ���͕������O�����������B�������͍ŏ��Ɠ����������ꍇ�͂��̃t���O��TRUE

	u32 wordmax;		// ���͕����ő吔
	STRBUF *strbuf; // ����STRBUF�ɂ͂Q�̈Ӗ������݂���B
									// �P�͖��O���͉�ʂ���f�[�^���󂯎�郏�[�N�ł��邱�ƁB
									// ���O���͂��I������炱������R�s�[����
									// �����P�́A���O���͂ɍs���Ƃ��ɂ����ɕ�������i�[���Ă�����,
									// ���O���͉�ʂ͂��̕������\�����Ȃ���J�n����B�����Ƃ��Ă��@�\����

	//���ݖ��g�p
#if 0
	// --------�|�P�����ߊl�Ń{�b�N�X�]���ɂȂ������ɕK�v-------------
	int get_msg_id; 	// �|�P�����ߊl�̎���BOX�]���������������̃��b�Z�[�WID������
	BOX_MANAGER  *boxdata;	// �|�P�����ߊl�̎���BOX�]���������������ɁABOX�̖��O���擾���邽�߂ɕK�v
	CONFIG    *config;  // �R���t�B�O�\����
#endif 
} NAMEIN_PARAM;

//=============================================================================
/**
 *	�O���Q��
 */
//=============================================================================
//-------------------------------------
///	PROC�f�[�^
//=====================================
extern const GFL_PROC_DATA NameInputProcData;

//-------------------------------------
///	PARAM�쐬�A�j��
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	NAMEIN_PARAM���쐬
 *					����͉��L�ANameIn_ParamDelete�ōs���Ă�������
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
extern NAMEIN_PARAM *NameIn_ParamAllocMake( HEAPID heapId, NAMEIN_MODE mode, int param1, int param2, int wordmax, const STRBUF *default_str );
//----------------------------------------------------------------------------
/**
 *	@brief	NAMEIN_PARAM���
 *
 *	@param		NAMEIN_PARAM
 */
//-----------------------------------------------------------------------------
extern void NameIn_ParamDelete( NAMEIN_PARAM *param );
//-------------------------------------
///	PARAM����̎擾
//=====================================
extern void NameIn_CopyStr( const NAMEIN_PARAM *param, STRBUF *strbuf );
extern STRBUF* NameIn_CreateCopyStr( const NAMEIN_PARAM *param, HEAPID heapID );
extern BOOL NameIn_IsCancel( const NAMEIN_PARAM *param );
#endif	//NAME_INPUT_H__
