//======================================================================
/**
 * @file	debugwin_sys.h
 * @brief	�ėp�I�f�o�b�O�V�X�e��
 * @author	ariizumi
 * @data	09/05/01
 */
//======================================================================

#include "print/gf_font.h"


#ifndef DEBUGWIN_SYS_H__
#define DEBUGWIN_SYS_H__

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#if DEBUG_ONLY_FOR_ariizumi_nobuhiko
#define USE_DEBUGWIN_SYSTEM (1)
#else
#define USE_DEBUGWIN_SYSTEM (0)
#endif

#define DEBUGWIN_TRG_KEY (PAD_BUTTON_SELECT)

//�e��ޔ�̈�T�C�Y(WB�ł̓G���[��ʂ���q�؂���̂ŃT�C�Y�����킹�Ă�
#define DEBUGWIN_CHAR_TEMP_AREA (0x4000)
#define DEBUGWIN_SCRN_TEMP_AREA (0x800)
#define DEBUGWIN_PLTT_TEMP_AREA (0x20)
//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
/// ���ڑI�𒆂̃R�[���o�b�N

typedef struct _DEBUGWIN_ITEM DEBUGWIN_ITEM;
typedef struct _DEBUGWIN_GROUP DEBUGWIN_GROUP;

typedef void (*DEBUGWIN_UPDATE_FUNC)( void* userWork , DEBUGWIN_ITEM* item );
typedef void (*DEBUGWIN_DRAW_FUNC)( void* userWork , DEBUGWIN_ITEM* itemWo );

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
//--------------------------------------------------------------
//	gfl_use�ōs���������E�J���E���s����
//--------------------------------------------------------------
void DEBUGWIN_InitSystem( u8* charArea , u16* scrnArea , u16* plttArea );
void DEBUGWIN_UpdateSystem(void);
void DEBUGWIN_ExitSystem(void);
BOOL DEBUGWIN_IsActive(void);

//--------------------------------------------------------------
//	�v���b�N���ɍs���s���������E�J��
//--------------------------------------------------------------
void DEBUGWIN_InitProc( const u32 frmnum , GFL_FONT *fontHandle );
void DEBUGWIN_ExitProc( void );

//--------------------------------------------------------------
//	�O���[�v�̏���
//--------------------------------------------------------------
void DEBUGWIN_AddGroupToTop( const u8 id , const char *nameStr , const heapId );
void DEBUGWIN_AddGroupToGroup( const u8 id , const char *nameStr , const u8 parentGroupId, const heapId );

void DEBUGWIN_RemoveGroup( const u8 id );

//--------------------------------------------------------------
//	�A�C�e���̏���
//--------------------------------------------------------------
//�A�C�e�����O���[�v�ɒǉ�����(���O�Œ�
void DEBUGWIN_AddItemToGroup( const char *nameStr , DEBUGWIN_UPDATE_FUNC updateFunc , void *work , const u8 parentGroupId , const heapId );

//�A�C�e�����O���[�v�ɒǉ�����(���O�ύX��
void DEBUGWIN_AddItemToGroupEx( DEBUGWIN_UPDATE_FUNC updateFunc , DEBUGWIN_DRAW_FUNC drawFunc , void *work , const u8 parentGroupId , const heapId );

//�A�C�e���̖��O��ύX����(DEBUGWIN_DRAW_FUNC���Ŏg��
void DEBUGWIN_ITEM_SetName( DEBUGWIN_ITEM* item , const char *nameStr );
void DEBUGWIN_ITEM_SetNameV( DEBUGWIN_ITEM* item , char *nameStr , ...);

//--------------------------------------------------------------
//	�X�V
//--------------------------------------------------------------
//��ʑS�X�V
void DEBUGWIN_RefreshScreen( void );
//�Q�[����1�t���[���i�߂�
void DEBUGWIN_UpdateFrame( void );

#endif //DEBUGWIN_SYS_H__
