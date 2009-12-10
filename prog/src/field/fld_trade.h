//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		  fld_trade.h
 *	@brief		�Q�[��������
 *	@author		tomoya takahasi, miyuki iwasawa	(obata)
 *	@data		  2006.05.15 (2009.12.09 HGSS���ڐA)
 *
 *	���̃\�[�X�� field_trade�I�[�o�[���C�̈�ɑ����Ă��܂�
 *	���̃\�[�X�ɒ�`���ꂽ�S�֐��� field_trade�I�[�o�[���C�̈�𖾎��I�Ƀ��[�h���Ă���łȂ���
 *	���p�ł��Ȃ��̂Œ��ӂ��Ă��������I
 *
 *	�I�[�o�[���C���[�h���A�����[�h�\��
 *
 *	#include "system/pm_overlay.h"
 *
 *	FS_EXTERN_OVERLAY(field_trade);
 *	Overlay_Load( FS_OVERLAY_ID( field_trade ), OVERLAY_LOAD_NOT_SYNCHRONIZE );
 *	Overlay_UnloadID( FS_OVERLAY_ID( field_trade ) );
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __FIELD_TRADE_H__
#define __FIELD_TRADE_H__

#if 0
//�A�Z���u����include����Ă���ꍇ�́A���̐錾�𖳎��ł���悤��ifndef�ň͂�ł���
#ifndef	__ASM_NO_DEF_
#include "demo/demo_trade.h"
#include "savedata/mail_util.h"
#endif // __ASM_NO_DEF_
#endif

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	�����f�[�^�i���o�[
//=====================================
#define	FLD_TRADE_MADATUBOMI	(0)
#define	FLD_TRADE_SURIIPU		(1)
#define	FLD_TRADE_KURABU		(2)
#define	FLD_TRADE_HAKURYUU		(3)
#define	FLD_TRADE_KUSAIHANA		(4)
#define	FLD_TRADE_RAKKII		(5)
#define	FLD_TRADE_TUBOTUBO		(6)
#define	FLD_TRADE_ONISUZUME		(7)
#define	FLD_TRADE_DAGUTORIO		(8)
#define	FLD_TRADE_GOOSUTO		(9)
#define	FLD_TRADE_PIKACHU		(10)
#define	FLD_TRADE_USOHATI		(11)
#define	FLD_TRADE_SEIDOO		(12)

#define	FLD_TRADE_NUM			(13)

//-------------------------------------
//	�a����|�P�����ԋp�`�F�b�N�G���[�R�[�h
//=====================================
#define KEEP_POKE_RERR_NONE		(0)	//�G���[�Ȃ�
#define KEEP_POKE_RERR_NGPOKE	(1)	//�Ⴄ�|�P����
#define KEEP_POKE_RERR_ITEM		(2)	//�A�C�e���������Ă�
#define KEEP_POKE_RERR_CBALL	(3)	//�{�[���J�v�Z�����Z�b�g����Ă�
#define KEEP_POKE_RERR_ONLYONE	(4)	//�莝������C�������Ȃ�

//�A�Z���u����include����Ă���ꍇ�́A���̐錾�𖳎��ł���悤��ifndef�ň͂�ł���
#ifndef	__ASM_NO_DEF_

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
typedef struct _FLD_TRADE_WORK FLD_TRADE_WORK;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *
 *		����
 *			FLD_TRADE_WORK*  ���A���b�N���Ă��������
 *			���̊֐��Ńf�[�^���擾�ł��܂��B
 *	extern u32 FLD_TRADE_WORK_GetTradeMonsno( const FLD_TRADE_WORK* cp_work );
 * 	extern u32FLD_TRADE_WORK_GetChangeMonsno( const FLD_TRADE_WORK* cp_work );
 *	extern u32 FLD_TRADE_WORK_GetTradeMonsOyaSex( const FLD_TRADE_WORK* cp_work );
 *
 *			��������|�P���������܂�����
 *			EventCmd_FldTrade���R�[�����Ă��������B(field_trade_cmd.h)
 *			
 *			�������I��������
 *			FLD_TRADE_WORK*��j�����Ă�������
 *
 */
//-----------------------------------------------------------------------------
#include "field/fieldmap_proc.h"  // for FIELDMAP_WORK


extern FLD_TRADE_WORK* FLD_TRADE_WORK_Create( u32 heapID, u32 trade_no );
extern void FLD_TRADE_WORK_Delete( FLD_TRADE_WORK* p_work ); 


//----------------------
// �������f�[�^�擾�֌W
//----------------------
// �����ł��炤�|�P�����i���o�[
extern u32 FLD_TRADE_WORK_GetTradeMonsno( const FLD_TRADE_WORK* cp_work );		
// �����ɕK�v�ȃ|�P�����i���o�[
extern u32 FLD_TRADE_WORK_GetChangeMonsno( const FLD_TRADE_WORK* cp_work );	
// �����ɕK�v�ȃ|�P�����̐���
extern u32 FLD_TRADE_WORK_GetChangeMonsSex( const FLD_TRADE_WORK* cp_work );	
// �e�̐���
extern u32 FLD_TRADE_WORK_GetTradeMonsOyaSex( const FLD_TRADE_WORK* cp_work );		


// �����̎��s
#if 0
extern void FLD_Trade( FIELDMAP_WORK* fieldmap, FLD_TRADE_WORK* p_work, int party_pos );
#endif

#if 0
// �f���f�[�^�쐬
extern void FLD_TradeDemoDataMake( FIELDMAP_WORK* fieldmap, FLD_TRADE_WORK* p_work, int party_pos, DEMO_TRADE_PARAM* p_demo, POKEMON_PARAM* sendPoke, POKEMON_PARAM* recvPoke );
#endif

#if 0
//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h�@�|�P�����a����(���l���`�̃|�P������Ⴄ)
 *
 *	@param	heapID		�q�[�vID
 *	@param	trade_no	�����ԍ�
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
extern void FLD_KeepPokemonAdd( FIELDMAP_WORK* fieldmap, u8 trade_no ,u8 level,u16 zoneID);
#endif

#if 0
//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h�@���[���|�P�����C�x���g��p���[���f�[�^�𐶐�(�I�j�X�Y���̃��[����p)
 *
 *	@param	heapID		�q�[�vID
 *	@param	trade_no	�����ԍ�
 *
 *	@return	���[�N
 *
 *	@com	��������Alloc���ĕԂ��̂ŁA�Ăяo������������邱�ƁI
 */
//-----------------------------------------------------------------------------
extern MAIL_DATA* FLD_MakeKeepPokemonEventMail( void );
#endif


#if 0
//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h�@�a����|�P�����ԋp�s�`�F�b�N
 *
 *	�莝���̃|�P�������a����|�P�����ƈ�v���邩��A�A�C�e���̗L�����`�F�b�N
 *
 *	@param	trade_no	�����ԍ�
 *	@param	pos			�`�F�b�N����莝���̃|�P�����ʒu
 *
 *	@retval	KEEP_POKE_RERR_NONE		�Ԃ���
 *	@retval	KEEP_POKE_RERR_NGPOKE	�Ⴄ�|�P����������Ԃ��Ȃ�
 *	@retval	KEEP_POKE_RERR_ITEM		�A�C�e���������Ă邩��Ԃ��Ȃ�
 *	@retval	KEEP_POKE_RERR_CBALL	�J�X�^���{�[�����Z�b�g����Ă��邩��Ԃ��Ȃ�
 */
//-----------------------------------------------------------------------------
extern u16 FLD_KeepPokemonReturnCheck( FIELDMAP_WORK* fieldmap, u8 trade_no ,u8 pos); 
#endif

#endif	__ASM_NO_DEF_

#endif		// __FIELD_TRADE_H__

