/*
 *  @file   effect_encount_def.h
 *  @brief  �G�t�F�N�g�G���J�E���g�@���e������`
 *  @author Miyuki Iwasawa
 *  @date   10.03.04
 */

#pragma once

#define EFFENC_DEFAULT_INTERVAL (50)  //�G�t�F�N�g���I�f�t�H���g�C���^�[�o��
#define EFFENC_DEFAULT_PROB (20)  //�G�t�F�N�g���I�m���f�t�H���g

//======================================================================
//  define
//======================================================================
typedef enum{
 EFFENC_TYPE_SGRASS_NORMAL,  ///<�Z����(�m�[�}��)
 EFFENC_TYPE_SGRASS_SNOW,  ///<�Z����(����n��)
 EFFENC_TYPE_SGRASS_WARM,  ///<�Z����(��t)
 EFFENC_TYPE_LGRASS,  ///<������
 EFFENC_TYPE_CAVE,    ///<���A
 EFFENC_TYPE_WATER,   ///<�W��
 EFFENC_TYPE_SEA,     ///<�C
 EFFENC_TYPE_BRIDGE,  ///<��
 EFFENC_TYPE_MAX,
}EFFENC_TYPE_ID;

