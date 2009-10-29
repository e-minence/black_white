//============================================================================================
/**
 * @file	el_scoreboard.h
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#pragma once

typedef struct _EL_SCOREBOARD				EL_SCOREBOARD;
typedef struct _EL_SCOREBOARD_TEX		EL_SCOREBOARD_TEX;

typedef enum {
	ELB_MODE_NONE = 0,	// �X�N���[�������Ȃ�
	ELB_MODE_S,					// ���X�N���[��
	ELB_MODE_T,					// �c�X�N���[��
}ELB_MODE;

//============================================================================================
/**
 *
 * @brief	�d���f���I�u�W�F�N�g
 *
 */
//============================================================================================
EL_SCOREBOARD*	ELBOARD_Add( const STRBUF* str, const ELB_MODE mode, HEAPID heapID );
void						ELBOARD_Delete( EL_SCOREBOARD* elb );
void						ELBOARD_Main( EL_SCOREBOARD* elb );
void						ELBOARD_Draw( EL_SCOREBOARD* elb, 
															VecFx32* trans, fx32 scale, u16 width, u16 height, 
															GFL_G3D_CAMERA* g3Dcamera, GFL_G3D_LIGHTSET* g3Dlightset );

//============================================================================================
/**
 *
 * @brief	�d���f���e�N�X�`���i�����e�N�X�`���ɓ\��t���Ďg�p�j
 *
 */
//============================================================================================
EL_SCOREBOARD_TEX*	ELBOARD_TEX_Add_Ex(
    const GFL_G3D_RES* g3Dtex, const char* tex_name, const char* plt_name, 
    const STRBUF* str, u16 xpos, u16 ypos, HEAPID heapID );
#define ELBOARD_TEX_Add( g3Dtex, str, heapID ) \
        ELBOARD_TEX_Add_Ex( g3Dtex, "elboard_core", "elboard_core_pl", str, 0, 2, heapID )
void								ELBOARD_TEX_Delete( EL_SCOREBOARD_TEX* elb_tex );
void								ELBOARD_TEX_Main( EL_SCOREBOARD_TEX* elb_tex );

