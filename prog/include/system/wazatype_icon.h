//==============================================================================
/**
 * @file	wazatype_icon.h
 * @brief	�Z�^�C�v�A�C�R���̃w�b�_
 * @author	matsuda
 * @date	2005.10.26(��)
 */
//==============================================================================
#ifndef __WAZATYPE_ICON_H__
#define __WAZATYPE_ICON_H__


//==============================================================================
//	�萔��`
//==============================================================================
///�A�C�R���^�C�v(�Z�^�C�v�ȊO�̃^�C�v)
enum{
	ICONTYPE_STYLE = 18,	///<�i�D�悳
	ICONTYPE_BEAUTIFUL,		///<������
	ICONTYPE_CUTE,			///<����
	ICONTYPE_INTELLI,		///<����
	ICONTYPE_STRONG,		///<痂���
};

///�Z�^�C�v�A�C�R���Ŏg�p����p���b�g�{��
#define WTI_PLTT_NUM		(3)
///�Z���ރA�C�R���Ŏg�p����p���b�g�{��
#define WKI_PLTT_NUM		(3)

///�Z�^�C�v�A�C�R���O���t�B�b�N�̈��k�t���O(�L�����N�^)
#define WAZATYPEICON_COMP_CHAR		(0)
///�Z�^�C�v�A�C�R���O���t�B�b�N�̈��k�t���O(�p���b�g)
#define WAZATYPEICON_COMP_PLTT		(0)
///�Z�^�C�v�A�C�R���O���t�B�b�N�̈��k�t���O(�Z��)
#define WAZATYPEICON_COMP_CELL		(0)
///�Z�^�C�v�A�C�R���O���t�B�b�N�̈��k�t���O(�Z���A�j��)
#define WAZATYPEICON_COMP_CELLANM	(0)

///�Z���ރA�C�R���O���t�B�b�N�̈��k�t���O(�L�����N�^)
#define WAZAKINDICON_COMP_CHAR		(0)
///�Z���ރA�C�R���O���t�B�b�N�̈��k�t���O(�p���b�g)
#define WAZAKINDICON_COMP_PLTT		(0)
///�Z���ރA�C�R���O���t�B�b�N�̈��k�t���O(�Z��)
#define WAZAKINDICON_COMP_CELL		(0)
///�Z���ރA�C�R���O���t�B�b�N�̈��k�t���O(�Z���A�j��)
#define WAZAKINDICON_COMP_CELLANM	(0)

///�Z�^�C�v�A�C�R����OAM�^�C�v��`
#define WAZATYPEICON_OAMTYPE		(OAMTYPE_32x16)


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern ARCDATID WazaTypeIcon_CgrIDGet(int waza_type);
extern ARCDATID WazaTypeIcon_PlttIDGet(void);
extern ARCDATID WazaTypeIcon_CellIDGet(void);
extern ARCDATID WazaTypeIcon_CellAnmIDGet(void);
extern u8 WazaTypeIcon_PlttOffsetGet(int waza_type);
extern ARCID WazaTypeIcon_ArcIDGet(void);

extern ARCDATID WazaKindIcon_CgrIDGet(int waza_kind);
extern ARCDATID WazaKindIcon_PlttIDGet(void);
extern ARCDATID WazaKindIcon_CellIDGet(void);
extern ARCDATID WazaKindIcon_CellAnmIDGet(void);
extern u8 WazaKindIcon_PlttOffsetGet(int waza_kind);
extern ARCID WazaKindIcon_ArcIDGet(void);


#endif	//__WAZATYPE_ICON_H__

