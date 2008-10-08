/**********************************************************************

Copyright (c) 2006 Polygon Magic, Inc.

************************************************************************/
/*!
	file	npBpcImporter.h
	author	kubota
*/
#ifndef NPBPCIMPORTER_H
#define NPBPCIMPORTER_H
/*
#ifdef __cplusplus
extern "C" {
#endif
*/
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// include
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "npfx.h"
#include "core/npConst.h"
#include "npImporterDef.h"
#include "npBpc.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// define
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// enum
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//���W�n
typedef enum __NP_BPC_COORDINATESYSTEM
{
	NP_BPC_LEFT_HANDED,
	NP_BPC_RIGHT_HANDED
} NP_BPC_COORDINATESYSTEM;

typedef enum __NP_BPC_KINEMATICS
{
	NP_BPC_TRANSLATION,
	NP_BPC_ROTATION
} NP_BPC_KINEMATICS;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// typedef
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//! �ϊ���񃌃R�[�h
typedef struct _CONVRECORD
{
	npS32	bpcfileNum;		// ����(file side number)
	npS32	engineNum;		// �E��(engine side number)
}_CONVRECORD;

//!	�t�@�C���C���|�[�g�p�R���e�L�X�g
/*!	
	\brief		�ǂݍ��݃f�[�^�̒��ԕۑ��ꏊ�B
				����́A�ǂݍ��񂾂��̂́A�܂������ƂȂ�I�u�W�F�N�g��
				�����ł��Ȃ��ꍇ���L��ׁB
*/
typedef struct NPBPCTEXTURE_CONTEXT
{
	//texture �Ɗ֌W�Ȃ��� version �ꎞ�ۊǏꏊ
	npU32		version;
	// �e�N�X�`���J�b�g��񒆊ԕۑ��ꏊ
	// �A�g���r���[�g��̕������ۑ����邱�Ƃ��l���Ă��Ȃ��̂Œ��ӁB
	// ���̃A�g���r���[�g�ݒ肷��O�Ɏg�p���Ă����Ȃ��Ə㏑�������B
	npS32		texCutNum;								//!< �e�N�X�`���J�b�g�̐�
	npS32		reserved0;										
	npS32		reserved1;										
	npS32		texCutBankTbl[NP_BPC_KEYFRAME_MAX];
	npFVECTOR	texCutUVTbl[NP_BPC_KEYFRAME_MAX];	//!< �e�N�X�`���J�b�g�̂t�u�e�[�u��
} NPBPCTEXTURE_CONTEXT;

// �e�N�X�`���J�b�g�f�[�^
typedef struct _BPC_TEXCUT
{
	npS32		bank;
	npFVECTOR	rect;
} _BPC_TEXCUT;

typedef struct _BPC_VELOCITY
{
	npFVECTOR	iniValue;
	npFVECTOR	iniVelocity;
	npFVECTOR	iniAcc;
	npFVECTOR	iniRc;
} _BPC_VELOCITY;

typedef struct _BPC_PARTICLEFCURVE
{
	npFVECTOR	ini;
	npFVECTOR	dir;
	npS32		type;
	npS32		elementNum;
	npS32		sequenceNum;
	npS32		mode;
	npFLOAT		hermiteRate;
	
} _BPC_PARTICLEFCURVE;
// �q�X�g�O�����^����
typedef struct _BPC_HISTGRAMGEN
{
	npS32		loop;
	npS32		randomFlag;
	npS32		sequenceNum;
	//npS32		num = sequenceNum;

} _BPC_HISTGRAMGEN;
// �X�N���[���^����
typedef struct _BPC_SCROLLGEN
{
	npFVECTOR		initialPos;
	npFLOAT			velocity[ 2 ];
	npS32			loop[ 2 ];
	npS32			textureBank;
} _BPC_SCROLLGEN;

// ���܂��܂Ȑݒ���i�[����
typedef struct __NP_BPC_DESC
{
	NP_BPC_COORDINATESYSTEM		coordinateSystem;// ���W�n�𔽉f����
	npU32						fps;
} NP_BPC_DESC;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// BlendMagic tool relation
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifdef NP_TOOL_USER
// �c���[�m�[�h�̐�
#define DEF_BPC_PARTICLE_COUNT	256

//! particle name hanger
typedef struct __NP_BPC_PARTICLE_CONTENTS 
{
	char	sParticleName[DEF_BPC_PARTICLE_COUNT][256];//particle name
	npU32	iParticleCount;// particle
	npU32	iParticleCounter;
} NP_BPC_PARTICLE_CONTENTS;

// texture cut list
typedef struct __NP_BPC_PARTICLE_TEXLIST
{
	npU16		iParticleCount;
	npU16		iParticleCounter;
	npU16		nNumOfUVList[DEF_BPC_PARTICLE_COUNT];
	npFVECTOR	uvList[DEF_BPC_PARTICLE_COUNT][NP_PARTICLE_KEYFRAME_MAX];
	npU16		nNumOfkey[DEF_BPC_PARTICLE_COUNT];
	npU16		keyIdList[DEF_BPC_PARTICLE_COUNT][NP_PARTICLE_KEYFRAME_MAX];
	npU16		keyframeList[DEF_BPC_PARTICLE_COUNT][NP_PARTICLE_KEYFRAME_MAX];

} NP_BPC_PARTICLE_TEXLIST;

//! Particle name is allocated.
//! Because a tree for the tool needs particle name.
extern	NP_BPC_PARTICLE_CONTENTS bpcContents;
// texture cut list ��ێ����Ă���
extern	NP_BPC_PARTICLE_TEXLIST bpcTexCutList;

#endif


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// function
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//�����������
// file ��ǂݍ��݁Aeffect attribute ���Z�b�g����
npBOOL npUtilParticleLoadBpc(void* pBuf ,npSIZE nSize ,NP_BPC_DESC*	pDesc ,npPARTICLEEMITCYCLE** ppRoot );

/*
���[�g�A�g���r���[�g���������B
���[�g�A�g���r���[�g�ɏ����������Ă���̂ŁA���p�̂Ŏ�����Ă��镔�����������ł���B
*/
npBOOL initRootParticleAttribute( npPARTICLEATTRIBUTE* pAttr );
/*
�A�g���r���[�g��������
�ǂݍ��݃f�[�^�ɂ��A�����\�����I������鋤�p�̂̕����͏������o���Ȃ�
*/
npBOOL initParticleAttribute( npPARTICLEATTRIBUTE *pAttr );
//�I�u�W�F�N�g��������
npBOOL initObject( npOBJECT *pObj, npS32 type, npU32 state );
//�����p�����[�^��������
npBOOL initKinematics( npPARTICLEKINEMATICS *pKine, npS32 type );

//root ��translation ,rotation �̐ݒ�
void* setupRootKinematics( npPARTICLEKINEMATICS *pKine, void* pBuf );
void* setupRootVelocity( npPARTICLEVELOCITY *pVelo, void* pBuf );
//root �Œ�l�̐ݒ�
void* setupRootFixed( npPARTICLEFIXED *pFix, void* pBuf );

//�A�g���r���[�g�̓��e���\�z(�ċA�I�ɖ��[�̎q���܂ō\�z���Ă���)
npBOOL buildParticleAttribute( npPARTICLEATTRIBUTE **ppChild,NPBPCTEXTURE_CONTEXT *pTexContext , NP_BPC_DESC* pDesc, void *pBuf );
//�e�N�X�`���J�b�g����ǂݍ���ŕۑ����Ēu��
void* readKeepTexCutInfo( NPBPCTEXTURE_CONTEXT *pTexContext, void *pBuf );

//child �̔����̎d��
void* setupParticleFrameCycle( npPARTICLEFRAMECYCLE *pCycle, void *pBuf );
//�����^�C�v:constant
void* setupParticleCnstCycle( npPARTICLECNSTCYCLE *pCycle, void *pBuf );
//�����^�C�v:sequence
void* setupParticleSeqCycle( npPARTICLESEQCYCLE *pCycle, void *pBuf );
//�����^�C�v����
void* setupParticleFrameCycleCommon( npPARTICLEFRAMECYCLE *pCycle, void *pBuf );

//�A�g���r���[�g�̓��e���\�z(�ċA�I�ɖ��[�̎q���܂ō\�z���Ă���)
npBOOL buildParticleAttributeContent( npPARTICLEATTRIBUTE** ppAttribute, NPBPCTEXTURE_CONTEXT *pTexContext , NP_BPC_DESC* pDesc ,void *pBuf );

npBOOL buildParticleItem( npPARTICLEITEM** ppItem, NPBPCTEXTURE_CONTEXT *pTexContext, NP_BPC_DESC* pDesc, void *pBuf );
npBOOL buildParticleQuad( npPARTICLEQUAD** ppQuad, NPBPCTEXTURE_CONTEXT *pTexContext, NP_BPC_DESC* pDesc, void* pBuf );
npBOOL buildParticleEmitCycle( npPARTICLEEMITCYCLE **ppEmitCycle, NPBPCTEXTURE_CONTEXT *pTexContext, NP_BPC_DESC* pDesc , void* pBuf );
//���o�^�C�~���O�I�u�W�F�N�g�̃Z�b�g�A�b�v
void* setupParticleEmitCycle( npPARTICLEEMITCYCLE *pEmitCycle, NPBPCTEXTURE_CONTEXT *pTexContext, NP_BPC_DESC* pDesc , void *pBuf );
void* setupParticleKinematics( npPARTICLEKINEMATICS *pKine, npPARTICLEATTRIBUTE *pAttr, NPBPCTEXTURE_CONTEXT *pTexContext, void* pBuf );

void* setupParticleVelocity( npPARTICLEVELOCITY *pVelo, void* pBuf );
void* setupVelocity( npPARTICLEVELOCITY *pVelo, void* pBuf );
void* setupParticleFixed( npPARTICLEFIXED *pFix, void* pBuf );
//�Œ�l�̐ݒ�
void* setupFixed( npPARTICLEFIXED *pFixed, void* pBuf );
//BPC�q�X�g�O�����i�p���p���A�j���j����NPF�J�[�u�����f�[�^���Z�b�g�A�b�v
void* setupParticleTwoFCurveFromHistgram( npPARTICLEKINEMATICS *pUVCoord, npPARTICLEITEM *pItem, NPBPCTEXTURE_CONTEXT *pTexContext, void *pBuf );
void* setupParticleFCurveFromSeqLine( npPARTICLEKINEMATICS *pUVCoord, npPARTICLEITEM *pItem, NPBPCTEXTURE_CONTEXT *pTexContext, void *pBuf );
//�X�N���[����񂩂��̃x���V�e�B����ݒ肷��
void* setupParticleTwoVelocityFromScroll( npPARTICLEKINEMATICS *pUVCoord, npPARTICLEITEM *pItem, NPBPCTEXTURE_CONTEXT *pTexContext,void *pBuf );
//�p�[�e�B�N��F�J�[�u�p�����[�^�Z�b�g�A�b�v
void* setupParticleFCurve( npPARTICLEFCURVE *pFCrv, void *pBuf );
//F�J�[�u�p�����[�^�Z�b�g�A�b�v
void* setupFCurve( npPARTICLEFCURVE *pFCrv, void *pBuf );
//�p�[�e�B�N��NULL�p�����[�^����FIXED�p�����[�^���Z�b�g�A�b�v
void* setupParticleFixedFromNull( npPARTICLEFIXED *pFixed, void *pBuf );
// ���W�n�Ή� //////////////////////////////////////////////
// left handed type
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void* setupRootLeftKinematics( npPARTICLEKINEMATICS *pKine, NP_BPC_KINEMATICS kineType , void* pBuf );
void* setupParticleLeftKinematics( npPARTICLEKINEMATICS *pKine, npPARTICLEATTRIBUTE *pAttr, NPBPCTEXTURE_CONTEXT *pTexContext, NP_BPC_KINEMATICS kineType , void* pBuf );
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void* setupRootLeftFixed( npPARTICLEFIXED *pFix, NP_BPC_KINEMATICS kineType , void* pBuf );
void* setupParticleLeftFixed( npPARTICLEFIXED *pFix, NP_BPC_KINEMATICS kineType , void* pBuf );
void* setupLeftFixed( npPARTICLEFIXED *pFixed, NP_BPC_KINEMATICS kineType , void* pBuf );
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void* setupRootLeftVelocity( npPARTICLEVELOCITY *pVelo, NP_BPC_KINEMATICS kineType , void* pBuf );
void* setupParticleLeftVelocity( npPARTICLEVELOCITY *pVelo, NP_BPC_KINEMATICS kineType , void* pBuf );
void* setupLeftVelocity( npPARTICLEVELOCITY *pVelo, NP_BPC_KINEMATICS kineType , void* pBuf );
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void* setupParticleLeftFCurve( npPARTICLEFCURVE *pFCrv, NP_BPC_KINEMATICS kineType , void *pBuf );
void* setupLeftFCurve( npPARTICLEFCURVE *pFCrv, NP_BPC_KINEMATICS kineType , void *pBuf );

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
/*!

	�e�m�[�h�̍Ō�Ɏ��g�̃m�[�h��ǉ�����

	@param	pParent	�e�m�[�h
	@param	pNode	�ǉ�����m�[�h

	@note	pNode �̓��[�g( �e���Ȃ� )�ł���K�v������

*/
void npParticleAppendNode( npPARTICLENODE* pParent, npPARTICLENODE* pNode );
/*
#ifdef __cplusplus
}
#endif
*/
#endif //NPBPCIMPORTER_H

