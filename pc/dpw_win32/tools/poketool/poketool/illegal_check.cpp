/*---------------------------------------------------------------------------*
  Project:  Pokemon DP WiFi
  File:     illegal_check.c

  Copyright 2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include "monsno.h"
#include "illegal_check.h"
#include "dpw_assert.h"
#include "trans_pokecode.h"

#ifdef WIN32
#include <windows.h>
#include <winnls.h>
#endif

/*-----------------------------------------------------------------------*
					�^�E�萔�錾
 *-----------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*
					�֐��v���g�^�C�v�錾
 *-----------------------------------------------------------------------*/
/*
BOOL PokeTool_LoadWazaData(const char* wazaFileName, const char* wazaLvFileName);
PokeToolCheckResult PokeTool_CheckWaza(const u32 monsno, const u16* waza, const char* absWazaFilePath, const u32 level, const u32 form_no);
PokeToolCheckResult PokeTool_CheckDoryoku(const u32 paraDoryoku[POKETOOL_DORYOKU_COUNT_MAX]);
PokeToolCheckResult PokeTool_CheckKotai(const u32 paraKotai[POKETOOL_KOTAI_COUNT_MAX]);
PokeToolCheckResult PokeTool_CheckSpeabi(const u32 monsno, const u32 paraSpeabino);
PokeToolCheckResult PokeTool_CheckItem(const u32 item);
PokeToolCheckResult PokeTool_CheckFlag(const u32 monsno, const BOOL paraEventGet, const BOOL rare);
PokeToolCheckResult PokeTool_CheckCountryCode(const u32 countryCode);
PokeToolCheckResult PokeTool_CheckPokemonLv(const u32 monsno, const u32 level);
u32 PokeTool_GetPokemonMinimumLv(const u32 monsno);
*/
/*-----------------------------------------------------------------------*
					�O���[�o���ϐ���`
 *-----------------------------------------------------------------------*/

static BOOL initialized = FALSE;
static u8 pokemon_waza_data[POKETOOL_MONS_WAZADATA_MAX][POKETOOL_WAZA_MAX];
//static u8 pokemon_waza_lv_data[MONSNO_END][POKETOOL_WAZA_MAX];
static u8 pokemon_speabi_data[POKETOOL_MONS_WAZADATA_MAX][POKETOOL_SPEABI_COUNT_MAX];

// �z�z�Z���ȏ�o���Ă���|�P�����ԍ��������ɋL�q����
// 2004�N���X�}�X�z�z��_�[�N���C�A�q�g�J�Q�A���U�[�h�A���U�[�h���Ȃǂ��܂܂��
static const u32 twoHaifuMonsno[] = { 4, 5, 6, 83, 96, 97, 102, 103, 108, 113, 115, 242, 491 };

// �o�g���^���[�ł̂݋֎~����|�P�����Bbt_pokemon_ng_list.csv���B
static const u32 bt_notAllowMonsno[] = { 150, 151, 250, 249, 251, 382, 383, 384, 385, 386, 483, 484, 487, 489, 490, 491, 492, 493 };



/*-----------------------------------------------------------------------*
					�O���[�o���֐���`
 *-----------------------------------------------------------------------*/
/*!
	��{�I�ȃ|�P�����f�[�^��ǂݎ��܂��B
    COM�C���^�t�F�[�X�p�ɍ쐬���܂����B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	@param pp	�s���`�F�b�N����|�P�����f�[�^�BID��^���Ē��g���Q�Ƃ��Ȃ��Ƃ����Ȃ��B
    @param spd  [out] �擾�����|�P�����f�[�^�B

	@retval POKETOOL_CHECKRESULT_OK				�^����ꂽ�|�P�����f�[�^�͐���
	@retval POKETOOL_CHECKRESULT_ILLEGAL_DATA	�^����ꂽ�|�P�����f�[�^�����Ă���i�����̃G���R�[�h���ł��Ȃ������Ƃ��Ȃǁj
*/
PokeToolCheckResult PokeTool_GetSimplePokemonData(POKEMON_PARAM *pp, SimplePokemonData* spd)
{
	u32 i;

	// ID�ǂݏo��
	spd->idNo = PokeTool_GetPokeParam(pp, ID_PARA_id_no, NULL);

    // ����
	spd->sex = PokeTool_GetPokeParam(pp, ID_PARA_sex, NULL);

	// �|�P����No�ǂݏo��
	spd->monsno = PokeTool_GetPokeParam(pp, ID_PARA_monsno, NULL);

	// �Z�f�[�^�ǂݏo��
	for(i = 0; i<WAZA_TEMOTI_MAX; i++){
		spd->waza[i] = (u16)PokeTool_GetPokeParam(pp, (PokeToolParamID)(ID_PARA_waza1+i), NULL);
	}

	// �w�͒l�ǂݏo��
	spd->paraDoryoku[0] = PokeTool_GetPokeParam(pp, ID_PARA_hp_exp, NULL);
	spd->paraDoryoku[1] = PokeTool_GetPokeParam(pp, ID_PARA_pow_exp, NULL);
	spd->paraDoryoku[2] = PokeTool_GetPokeParam(pp, ID_PARA_def_exp, NULL);
	spd->paraDoryoku[3] = PokeTool_GetPokeParam(pp, ID_PARA_agi_exp, NULL);
	spd->paraDoryoku[4] = PokeTool_GetPokeParam(pp, ID_PARA_spepow_exp, NULL);
	spd->paraDoryoku[5] = PokeTool_GetPokeParam(pp, ID_PARA_spedef_exp, NULL);

	// �̒l�ǂݏo��
	spd->paraKotai[0] = PokeTool_GetPokeParam(pp, ID_PARA_hp_rnd, NULL);
	spd->paraKotai[1] = PokeTool_GetPokeParam(pp, ID_PARA_pow_rnd, NULL);
	spd->paraKotai[2] = PokeTool_GetPokeParam(pp, ID_PARA_def_rnd, NULL);
	spd->paraKotai[3] = PokeTool_GetPokeParam(pp, ID_PARA_agi_rnd, NULL);
	spd->paraKotai[4] = PokeTool_GetPokeParam(pp, ID_PARA_spepow_rnd, NULL);
	spd->paraKotai[5] = PokeTool_GetPokeParam(pp, ID_PARA_spedef_rnd, NULL);
	
	// ����No�ǂݏo��
	spd->paraItem = PokeTool_GetPokeParam(pp, ID_PARA_item, NULL);

	// ����\�͂̓ǂݏo��
	spd->paraSpeabino = PokeTool_GetPokeParam(pp, ID_PARA_speabino, NULL);

	// �z�z�t���O�̓ǂݏo��
	spd->paraEventGet = PokeTool_GetPokeParam(pp, ID_PARA_event_get_flag, NULL);

	// ���R�[�h�ǂݏo��
	spd->countryCode = PokeTool_GetPokeParam(pp, ID_PARA_country_code, NULL);

	// ���x���ǂݏo��
	spd->level = PokeTool_GetPokeParam(pp, ID_PARA_level, NULL);

    // �ߊl�ꏊ�ǂݏo��
    spd->place = PokeTool_GetPokeParam(pp, ID_PARA_get_place, NULL);

    spd->birthPlace = PokeTool_GetPokeParam(pp, ID_PARA_birth_place, NULL);

    // �ߊl�J�Z�b�g�ǂݏo��
    //cassette = PokeTool_GetPokeParam(pp, ID_PARA_get_cassette, NULL);

	// �t�H�[��No�ǂݏo��
	if (spd->monsno == MONSNO_AUSU) {
		spd->form_no = 0;
	} else {
		spd->form_no = PokeTool_GetPokeParam(pp, ID_PARA_form_no, NULL);
	}

    // �j�b�N�l�[���t���O�ǂݏo��
    spd->nicknameFlag = PokeTool_GetPokeParam(pp, ID_PARA_nickname_flag, NULL);

    // ����̐���
    spd->oyasex = PokeTool_GetPokeParam(pp, ID_PARA_oyasex, NULL);
    return POKETOOL_CHECKRESULT_OK;
}

/*!
	�|�P�����f�[�^���s���Ȃ��̂��ۂ���Ԃ��܂��B
	�Í������������f�[�^�ɑ΂��Ă̂ݎg�p�ł��܂��B

	@param pp	�s���`�F�b�N����|�P�����f�[�^�BID��^���Ē��g���Q�Ƃ��Ȃ��Ƃ����Ȃ��B
	@param absWazaFilePath �Z�f�[�^��`�t�@�C���ւ̃p�X

	@retval POKETOOL_CHECKRESULT_OK				�^����ꂽ�|�P�����f�[�^�͐���
	@retval POKETOOL_CHECKRESULT_ILLEGAL_DATA	�^����ꂽ�|�P�����f�[�^�����Ă���i�����̃G���R�[�h���ł��Ȃ������Ƃ��Ȃǁj
	@retval POKETOOL_CHECKRESULT_CHEAT_DATA		�^����ꂽ�|�P�����f�[�^�͕s��
	@retval POKETOOL_CHECKRESULT_NO_WAZA_DATA	�Z�f�[�^��`�t�@�C����������Ȃ�
*/
PokeToolCheckResult PokeTool_CheckData(POKEMON_PARAM *pp, const char* absWazaFilePath)
{
	u16 waza[WAZA_TEMOTI_MAX];
	u32 monsno;
	//u32 level;
	u32 i;
	u32 paraDoryoku[POKETOOL_DORYOKU_COUNT_MAX];
	u32 paraKotai[POKETOOL_KOTAI_COUNT_MAX];
	u32 paraItem;
	u32 paraSpeabino;
	u32 paraEventGet;
	u32 countryCode;
	u32 level;
    u32 place;
    u32 birthPlace;
    //u32 cassette;
	u32 form_no;
    u32 classic_ribbon;
	PokeToolCheckResult result;

	// �|�P����No�ǂݏo��
	monsno = PokeTool_GetPokeParam(pp, ID_PARA_monsno, NULL);

	// �Z�f�[�^�ǂݏo��
	for(i = 0; i<WAZA_TEMOTI_MAX; i++){
		waza[i] = (u16)PokeTool_GetPokeParam(pp, (PokeToolParamID)(ID_PARA_waza1+i), NULL);
	}

	// �w�͒l�ǂݏo��
	paraDoryoku[0] = PokeTool_GetPokeParam(pp, ID_PARA_hp_exp, NULL);
	paraDoryoku[1] = PokeTool_GetPokeParam(pp, ID_PARA_pow_exp, NULL);
	paraDoryoku[2] = PokeTool_GetPokeParam(pp, ID_PARA_def_exp, NULL);
	paraDoryoku[3] = PokeTool_GetPokeParam(pp, ID_PARA_agi_exp, NULL);
	paraDoryoku[4] = PokeTool_GetPokeParam(pp, ID_PARA_spepow_exp, NULL);
	paraDoryoku[5] = PokeTool_GetPokeParam(pp, ID_PARA_spedef_exp, NULL);

	// �̒l�ǂݏo��
	paraKotai[0] = PokeTool_GetPokeParam(pp, ID_PARA_hp_rnd, NULL);
	paraKotai[1] = PokeTool_GetPokeParam(pp, ID_PARA_pow_rnd, NULL);
	paraKotai[2] = PokeTool_GetPokeParam(pp, ID_PARA_def_rnd, NULL);
	paraKotai[3] = PokeTool_GetPokeParam(pp, ID_PARA_agi_rnd, NULL);
	paraKotai[4] = PokeTool_GetPokeParam(pp, ID_PARA_spepow_rnd, NULL);
	paraKotai[5] = PokeTool_GetPokeParam(pp, ID_PARA_spedef_rnd, NULL);
	
	// ����No�ǂݏo��
	paraItem = PokeTool_GetPokeParam(pp, ID_PARA_item, NULL);

	// ����\�͂̓ǂݏo��
	paraSpeabino = PokeTool_GetPokeParam(pp, ID_PARA_speabino, NULL);

	// �z�z�t���O�̓ǂݏo��
	paraEventGet = PokeTool_GetPokeParam(pp, ID_PARA_event_get_flag, NULL);

	// ���R�[�h�ǂݏo��
	countryCode = PokeTool_GetPokeParam(pp, ID_PARA_country_code, NULL);

	// ���x���ǂݏo��
	level = PokeTool_GetPokeParam(pp, ID_PARA_level, NULL);

    // �ߊl�ꏊ�ǂݏo��
    place = PokeTool_GetPokeParam(pp, ID_PARA_get_place, NULL);

    birthPlace = PokeTool_GetPokeParam(pp, ID_PARA_birth_place, NULL);

    // �ߊl�J�Z�b�g�ǂݏo��
    //cassette = PokeTool_GetPokeParam(pp, ID_PARA_get_cassette, NULL);

	// �t�H�[��No�ǂݏo��
	if (monsno == MONSNO_AUSU) {
		form_no = 0;
	} else {
		form_no = PokeTool_GetPokeParam(pp, ID_PARA_form_no, NULL);
	}

	// �Z�̃`�F�b�N
	result = PokeTool_CheckWaza(monsno, waza, absWazaFilePath, 0 /*level*/, form_no);
	if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}

	// �w�͒l�̃`�F�b�N
	result = PokeTool_CheckDoryoku(paraDoryoku);
	if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}

	// �̒l�̃`�F�b�N
	result = PokeTool_CheckKotai(paraKotai);
	if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}

	// ����No�̃`�F�b�N
    result = PokeTool_CheckItem(paraItem);
    if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}

	// ����\�͂̃`�F�b�N
	result = PokeTool_CheckSpeabi(monsno, paraSpeabino);
	if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}

	// �t���O�̃`�F�b�N
	result = PokeTool_CheckFlag(monsno, paraEventGet, PokeTool_IsRare(pp));
	if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}

	// ���R�[�h�̃`�F�b�N
	result = PokeTool_CheckCountryCode(countryCode);
	if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}

	// ���[�����̍��R�[�h�̃`�F�b�N
	if(PokeTool_HasMail(pp)){
		result = PokeTool_CheckCountryCode(PokeTool_GetMailLang(pp));
		if(result != POKETOOL_CHECKRESULT_OK){
			return result;
		}
	}

	// �ʏ�̃��x���`�F�b�N
	if(level == 0 || level >100){
		return POKETOOL_CHECKRESULT_CHEAT_DATA;
	}

	// �|�P�����̍Œ჌�x���`�F�b�N
	result = PokeTool_CheckPokemonLv(monsno, level);
	if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}

	// �|�P�����̕ߊl�ꏊ�`�F�b�N
    result = (PokeToolCheckResult)PokeTool_CheckCapturePlace(monsno, place, birthPlace);
	if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}
    
/*
	// �|�P�����̕ߊl�J�Z�b�g�`�F�b�N
    result = PokeTool_CheckCaptureCassette(monsno, cassette);
	if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}
*/    
    // �_�[�N���C�̂Ƃ��N���V�b�N���{�������Ă��邱�Ƃ��m�F����
    if(monsno == 491){
        // �N���V�b�N���{���̓ǂݏo��
        classic_ribbon = PokeTool_GetPokeParam(pp, ID_PARA_sinou_classic_ribbon, NULL);
        if(classic_ribbon == 0){
            return POKETOOL_CHECKRESULT_CHEAT_DATA;
        }
    }

	return POKETOOL_CHECKRESULT_OK;
}

/*!
	�o�g���^���[�̃|�P�����f�[�^���s���Ȃ��̂��ۂ���Ԃ��܂��B

	@param btp	�s���`�F�b�N����o�g���^���[�̃|�P�����f�[�^
	@param absWazaFilePath �Z�f�[�^��`�t�@�C���ւ̃p�X

	@retval POKETOOL_CHECKRESULT_OK				�^����ꂽ�|�P�����f�[�^�͐���
	@retval POKETOOL_CHECKRESULT_ILLEGAL_DATA	�^����ꂽ�|�P�����f�[�^�����Ă���i�����̃G���R�[�h���ł��Ȃ������Ƃ��Ȃǁj
	@retval POKETOOL_CHECKRESULT_CHEAT_DATA		�^����ꂽ�|�P�����f�[�^�͕s��
	@retval POKETOOL_CHECKRESULT_NO_WAZA_DATA	�Z�f�[�^��`�t�@�C����������Ȃ�
*/
PokeToolCheckResult PokeTool_BtCheckData(B_TOWER_POKEMON* btp, const char* absWazaFilePath)
{
	u32 paraDoryoku[POKETOOL_DORYOKU_COUNT_MAX];
	u32 paraKotai[POKETOOL_KOTAI_COUNT_MAX];
	PokeToolCheckResult result;
    int i;

	// �w�͒l�ǂݏo��
	paraDoryoku[0] = btp->hp_exp;
	paraDoryoku[1] = btp->pow_exp;
	paraDoryoku[2] = btp->def_exp;
	paraDoryoku[3] = btp->agi_exp;
	paraDoryoku[4] = btp->spepow_exp;
	paraDoryoku[5] = btp->spedef_exp;

	// �̒l�ǂݏo��
	paraKotai[0] = btp->hp_rnd;
	paraKotai[1] = btp->pow_rnd;
	paraKotai[2] = btp->def_rnd;
	paraKotai[3] = btp->agi_rnd;
	paraKotai[4] = btp->spepow_rnd;
	paraKotai[5] = btp->spedef_rnd;

	// �Z�A�|�P����No�̃`�F�b�N
	result = PokeTool_CheckWaza(btp->mons_no, btp->waza, absWazaFilePath, 0/* ���x���Ȃ� */, btp->form_no);
	if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}

	// �w�͒l�̃`�F�b�N
	result = PokeTool_CheckDoryoku(paraDoryoku);
	if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}

	// �̒l�̃`�F�b�N
	result = PokeTool_CheckKotai(paraKotai);
	if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}

	// ����No�̃`�F�b�N
    result = PokeTool_CheckItem(btp->item_no);
    if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}

	// ����\��(����)�̃`�F�b�N
	result = PokeTool_CheckSpeabi(btp->mons_no, btp->tokusei);
	if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}

	// �t���O�̃`�F�b�N
	result = PokeTool_CheckFlag(btp->mons_no, TRUE/* �t���O�Ȃ� */, PokeTool_BtIsRare(btp));
	if(result != POKETOOL_CHECKRESULT_OK){
		return result;
	}

	// BT�̂ݒe���|�P�������`�F�b�N
	for(i=0; i<sizeof(bt_notAllowMonsno)/sizeof(bt_notAllowMonsno[0]); i++){
		if(btp->mons_no == bt_notAllowMonsno[i]){
			return POKETOOL_CHECKRESULT_CHEAT_DATA;
		}
	}
	return POKETOOL_CHECKRESULT_OK;
}

#ifdef WIN32

#define MATH_MIN(a,b) (((a) <= (b)) ? (a) : (b))

/*!
	�|�P�����Ŏg�p����閼�O�𓝐����܂��B
	�S�p�����͔��p�ɁA�啶���͏������ɁA�J�^�J�i�͂Ђ炪�Ȃɕϊ�����܂��B
	�o�̓o�b�t�@�́AMONS_NAME_SIZE+EOM_SIZE�̂��̂�^���Ă��������B

	@param in	���͂��閼�O�ł��BMONS_NAME_SIZE�ȉ��̒����ŁANULL�I�[���ꂽ���̂ɂ��Ă��������B
	@param out	�ԊҌ�̏o�͐�ł��B
*/
void PokeTool_UnifyName(const u16* in, u16* out)
{
	u16 temp_str[MONS_NAME_SIZE+EOM_SIZE];

	LCMapStringW(MAKELCID(MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT), SORT_DEFAULT),
		LCMAP_HALFWIDTH | LCMAP_HIRAGANA,
		(LPCWSTR)in, -1, (LPWSTR)temp_str, (MONS_NAME_SIZE+EOM_SIZE)*sizeof(u16));

	LCMapStringW(MAKELCID(MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT), SORT_DEFAULT),
		LCMAP_LOWERCASE,
		(LPCWSTR)temp_str, -1, (LPWSTR)out, (MONS_NAME_SIZE+EOM_SIZE)*sizeof(u16));
}

#endif

/*-----------------------------------------------------------------------*
					���[�J���֐���`
 *-----------------------------------------------------------------------*/

/*!
	�|�P�����̋Z�f�[�^��ǂݍ��݂܂��B

	@param wazaFileName		�|�P�����Z�t���O�f�[�^�o�C�i���t�@�C����
	@param wazaLvFileName	�|�P�����Z���x���f�[�^�o�C�i���t�@�C����

	@retval TRUE				����ɓǂݍ��݊���
	@retval FALSE				�ǂݍ��ݎ��s
*/
BOOL PokeTool_LoadWazaData(const char* wazaFilePath, const char* wazaLvFilePath)
{
	FILE *fp;
	u8 c;
	u32 i;

	// �Z�t���O�f�[�^�̓ǂݍ���
	fp = fopen(wazaFilePath, "rb");
	if(!fp){
		printf("cannot open file: %s\n", wazaFilePath);
		return FALSE;
	}

	for(i = 0; i<POKETOOL_MONS_WAZADATA_MAX; i++){
		if((fread(pokemon_speabi_data[i], sizeof(u8), POKETOOL_SPEABI_COUNT_MAX, fp )) != POKETOOL_SPEABI_COUNT_MAX	// �|�P�������Ƃ̓�����ǂݍ���
		|| (fread(pokemon_waza_data[i], sizeof(u8), POKETOOL_WAZA_MAX, fp )) != POKETOOL_WAZA_MAX					// �|�P�������Ƃ̋Z�f�[�^�ǂݍ���
		|| (fread(&c, sizeof(u8), 1, fp )) != 1																		// ��؂蕶����ǂ�
		|| c != 0xFF){																								// ��؂蕶���̃`�F�b�N
			fclose(fp);
			printf("invalid file format: %s\n", wazaFilePath);
			return FALSE;
		}
	}
	fclose(fp);
/*
	// �Z���x���f�[�^�̓ǂݍ���
	fp = fopen(wazaLvFileName, "rb");
	if(!fp){
		printf("cannot open file: %s\n", wazaLvFilePath);
		return FALSE;
	}

	for(i = 0; i<MONSNO_END; i++){
		if((fread(pokemon_waza_lv_data[i], sizeof(u8), POKETOOL_WAZA_MAX, fp )) != POKETOOL_WAZA_MAX				// �|�P�������Ƃ̋ZLv�f�[�^�ǂݍ���
		|| (fread(&c, sizeof(u8), 1, fp )) != 1																		// ��؂蕶����ǂ�
		|| c != 0xFF){																								// ��؂蕶���̃`�F�b�N
			fclose(fp);
			printf("invalid file format: %s\n", wazaLvFileName);
			return FALSE;
		}
	}

	fclose(fp);
*/
	return TRUE;
}

/*!
	�ǂݍ��񂾃|�P�����̋Z�f�[�^���e�L�X�g�ŏo�͂��܂��B(�f�o�b�O�p)

	@param outFileName	�o�̓t�@�C����

	@retval TRUE				����ɏo�͊���
	@retval FALSE				�o�͎��s
*/
BOOL PokeTool_PrintWazaData(const char* outFileName)
{
	FILE *fp;
	u32 i, j;

	if(!initialized){
		printf("PokeTool_PrintWazaData: ERROR - not initialized.\n");
	}

	fp = fopen(outFileName, "w");
	if(!fp){
		printf("cannot open file: %s\n", outFileName);
		return FALSE;
	}

	for(i = 0; i<POKETOOL_MONS_WAZADATA_MAX; i++){
		for(j = 0; j<POKETOOL_WAZA_MAX; j++){
			fprintf(fp, "%u", pokemon_waza_data[i][j]);
		}
		fputs("\n", fp);
	}

	fclose(fp);
	return TRUE;
}

/*!
	�|�P�����̋Z�f�[�^���s���Ȃ��̂��ۂ���Ԃ��܂��B

	@param monsno	�s���`�F�b�N����|�P����No
	@param waza		�s���`�F�b�N����|�P�����̋Z�f�[�^
	@param absWazaFilePath	�s���`�F�b�N����|�P�����̋Z�f�[�^�t�@�C���p�X
	@param level	�s���`�F�b�N����|�P�����̃��x��
	@param form_no	�s���`�F�b�N����|�P�����̃t�H�[��No

	@retval POKETOOL_CHECKRESULT_OK				�^����ꂽ�|�P�����f�[�^�͐���
	@retval POKETOOL_CHECKRESULT_ILLEGAL_DATA	�^����ꂽ�|�P�����f�[�^�����Ă���i�����̃G���R�[�h���ł��Ȃ������Ƃ��Ȃǁj
	@retval POKETOOL_CHECKRESULT_CHEAT_DATA		�^����ꂽ�|�P�����f�[�^�͕s��
	@retval POKETOOL_CHECKRESULT_NO_WAZA_DATA	�Z�f�[�^���ǂݍ��߂Ȃ�����
*/
PokeToolCheckResult PokeTool_CheckWaza(const u32 monsno, const u16* waza, const char* absWazaFilePath, const u32 level, const u32 form_no)
{
	int i, j;
	u8 wazaResult[WAZA_TEMOTI_MAX];
	int wazaNoneCount = 0;
	BOOL bXmas2004Haifu = FALSE;
	u32 table_no;

	// �Z�f�[�^�̓ǂݍ���
	if(!initialized){
		if(PokeTool_LoadWazaData(absWazaFilePath, NULL /* WAZA_LV_FILE_NAME */)){
			initialized = TRUE;
		}else{
			return POKETOOL_CHECKRESULT_NO_WAZA_DATA;
		}
	}

	// �|�P����No�̃`�F�b�N
	if (monsno == 0 || monsno > MONSNO_END) {
		return POKETOOL_CHECKRESULT_ILLEGAL_DATA;
	}

	// ����̃|�P�����̏ꍇ�́A�Q�Ƃ���\��No��ύX����
	if (monsno == MONSNO_DEOKISISU) {
		switch (form_no) {
		case 0:
			table_no = monsno;
			break;
		case 1:
			table_no = 496;
			break;
		case 2:
			table_no = 497;
			break;
		case 3:
			table_no = 498;
			break;
		default:
			return POKETOOL_CHECKRESULT_ILLEGAL_DATA;
		}
	} else if (monsno == MONSNO_MINOMESU) {
		switch (form_no) {
		case 0:
			table_no = monsno;
			break;
		case 1:
			table_no = 499;
			break;
		case 2:
			table_no = 500;
			break;
		default:
			return POKETOOL_CHECKRESULT_ILLEGAL_DATA;
		}
	} else {
		table_no = monsno;
	}

	// �ZNo�̃`�F�b�N
	for(i = 0; i<WAZA_TEMOTI_MAX; i++){
		if(!(waza[i] <= POKETOOL_WAZA_MAX)){
			return POKETOOL_CHECKRESULT_ILLEGAL_DATA;
		}
	}

	// ����Z�������Ă��Ȃ��ꍇ���`�F�b�N
	for(i = 0; i<WAZA_TEMOTI_MAX; i++){
		if(waza[i] == 0){
			wazaNoneCount++;
		}
	}
	if(wazaNoneCount == WAZA_TEMOTI_MAX){
		return POKETOOL_CHECKRESULT_ILLEGAL_DATA;
	}

	// �����Z�������Ă��Ȃ����`�F�b�N
	for(i = 0; i<WAZA_TEMOTI_MAX-1; i++){
		for(j = i+1; j<WAZA_TEMOTI_MAX; j++){
			if(waza[i] == waza[j]){
				if(waza[i] != 0){
					return POKETOOL_CHECKRESULT_ILLEGAL_DATA;
				}
			}
		}
	}

	// �Z0�̌�ɋZ�������Ă��Ȃ����`�F�b�N
	for(i = WAZA_TEMOTI_MAX-1; i>=0; i--){
		if(waza[i] != 0){
			break;
		}
	}
	for(i = i-1; i>=0; i--){
		if(waza[i] == 0){
			return POKETOOL_CHECKRESULT_ILLEGAL_DATA;
		}
	}

	// �����Ă���Z�f�[�^���Q�Ƃ���
	for(i=0; i<WAZA_TEMOTI_MAX; i++){
		if(waza[i] == 0){
			// �Z�������Ă��Ȃ��ꍇ�͖������邽�ߕ��ʂ̋Z�������Ă��邱�Ƃɂ���
			wazaResult[i] = POKETOOL_WAZA_KIND_NORMAL;
		}else{
			wazaResult[i] = pokemon_waza_data[table_no-1][waza[i]-1];
		}
	}

	// �h�[�u�� �� 235 �����ʈ����B�u��邠������165�v�u������ׂ聨448�v�u�V�[�h�t���A��465�v�u���΂��̂Ԃā�449�v�ȊO�̑S�Ă̋Z���o����
	if(table_no == 235){
		// �o������Z���̃`�F�b�N
		for(i=0; i<WAZA_TEMOTI_MAX; i++){
			if(waza[i] == 165 || waza[i] == 448 || waza[i] == 465 || waza[i] == 449){
				return POKETOOL_CHECKRESULT_CHEAT_DATA;
			}
		}
		// �h�[�u���͂����Ń`�F�b�N�I��
		return POKETOOL_CHECKRESULT_OK;
	}

	// �o������Z���̃`�F�b�N
	for(i=0; i<WAZA_TEMOTI_MAX; i++){
		if(!wazaResult[i]){
			return POKETOOL_CHECKRESULT_CHEAT_DATA;
		}
	}

	// 2004�N�N���X�}�X�z�z�̂ݕʈ���
	// �z�z�Z�������Ă���ꍇ��OK
	for(i=0; i<sizeof(twoHaifuMonsno)/sizeof(twoHaifuMonsno[0]); i++){
		if(table_no == twoHaifuMonsno[i]){
			bXmas2004Haifu = TRUE;
			break;
		}
	}

	{
		int haifuWazaCount = 0;
		int xdWazaCount = 0;
		for(i=0; i<WAZA_TEMOTI_MAX; i++){
			if(!(wazaResult[i] & POKETOOL_WAZA_KIND_NORMAL)){
				if(wazaResult[i] & POKETOOL_WAZA_KIND_HAIFU){
					haifuWazaCount++;
				}
				if(wazaResult[i] & POKETOOL_WAZA_KIND_XD){
					xdWazaCount++;
				}
			}
		}
		if(bXmas2004Haifu){
			if(haifuWazaCount > 2 || (haifuWazaCount > 0 && xdWazaCount > 0)){
				return POKETOOL_CHECKRESULT_CHEAT_DATA;
			}
		}else{
			if(haifuWazaCount > 1 || (haifuWazaCount > 0 && xdWazaCount > 0)){
				return POKETOOL_CHECKRESULT_CHEAT_DATA;
			}
		}
	}
/*
	// �Z���x���`�F�b�N
	for(i=0; i<WAZA_TEMOTI_MAX; i++){
		if(wazaResult[i] & POKETOOL_WAZA_KIND_NORMAL){
			continue;
		}
		if(level < pokemon_waza_lv_data[table_no-1][waza[i]-1]){
			return POKETOOL_CHECKRESULT_CHEAT_DATA;
		}
	}
*/
	return POKETOOL_CHECKRESULT_OK;
}

/*!
	�|�P�����̓w�͒l���s���Ȃ��̂��ۂ���Ԃ��܂��B
	6�̍��v��510�A��̔\�͒l�l��0�`255

	@param paraDoryoku		�s���`�F�b�N����|�P�����̓w�͒l

	@retval POKETOOL_CHECKRESULT_OK				�^����ꂽ�|�P�����f�[�^�͐���
	@retval POKETOOL_CHECKRESULT_ILLEGAL_DATA	�^����ꂽ�|�P�����f�[�^�����Ă���i�����̃G���R�[�h���ł��Ȃ������Ƃ��Ȃǁj
	@retval POKETOOL_CHECKRESULT_CHEAT_DATA		�^����ꂽ�|�P�����f�[�^�͕s��
*/
PokeToolCheckResult PokeTool_CheckDoryoku(const u32 paraDoryoku[POKETOOL_DORYOKU_COUNT_MAX])
{
	int i;
	u32 sumDoryoku = 0;
	for(i = 0; i<POKETOOL_DORYOKU_COUNT_MAX; i++){
		sumDoryoku += paraDoryoku[i];
		if(paraDoryoku[i] > 255){
			return POKETOOL_CHECKRESULT_CHEAT_DATA;
		}
	}

	if(sumDoryoku > 510){
		return POKETOOL_CHECKRESULT_CHEAT_DATA;
	}

	return POKETOOL_CHECKRESULT_OK;
}

/*!
	�|�P�����̌̒l���s���Ȃ��̂��ۂ���Ԃ��܂��B
	��̔\�͒l��0�`0x1f

	@param paraKotai		�s���`�F�b�N����|�P�����̌̒l

	@retval POKETOOL_CHECKRESULT_OK				�^����ꂽ�|�P�����f�[�^�͐���
	@retval POKETOOL_CHECKRESULT_ILLEGAL_DATA	�^����ꂽ�|�P�����f�[�^�����Ă���i�����̃G���R�[�h���ł��Ȃ������Ƃ��Ȃǁj
	@retval POKETOOL_CHECKRESULT_CHEAT_DATA		�^����ꂽ�|�P�����f�[�^�͕s��
*/
PokeToolCheckResult PokeTool_CheckKotai(const u32 paraKotai[POKETOOL_KOTAI_COUNT_MAX])
{
	int i;
	for(i = 0; i<POKETOOL_KOTAI_COUNT_MAX; i++){
		if(paraKotai[i] > 0x1f){
			return POKETOOL_CHECKRESULT_CHEAT_DATA;
		}
	}

	return POKETOOL_CHECKRESULT_OK;
}

/*!
	�|�P�����̓������s���Ȃ��̂��ۂ���Ԃ��܂��B

	@param monsno	�s���`�F�b�N����|�P����No
	@param paraSpeabino		�s���`�F�b�N����|�P�����̓���No

	@retval POKETOOL_CHECKRESULT_OK				�^����ꂽ�|�P�����f�[�^�͐���
	@retval POKETOOL_CHECKRESULT_ILLEGAL_DATA	�^����ꂽ�|�P�����f�[�^�����Ă���i�����̃G���R�[�h���ł��Ȃ������Ƃ��Ȃǁj
	@retval POKETOOL_CHECKRESULT_CHEAT_DATA		�^����ꂽ�|�P�����f�[�^�͕s��
*/
PokeToolCheckResult PokeTool_CheckSpeabi(const u32 monsno, const u32 paraSpeabino)
{
	int i;

	// �����������Ă��Ȃ��ꍇ���`�F�b�N
	if (paraSpeabino == 0) {
		return POKETOOL_CHECKRESULT_ILLEGAL_DATA;
	}

	// �s���ȓ����łȂ����`�F�b�N
	for(i = 0; i<POKETOOL_SPEABI_COUNT_MAX ; i++){
		if(pokemon_speabi_data[monsno-1][i] == paraSpeabino){
			return POKETOOL_CHECKRESULT_OK;
		}
	}

	return POKETOOL_CHECKRESULT_CHEAT_DATA;
}

/*!
	�A�C�e�����s���Ȃ��̂��ۂ���Ԃ��܂��B

	@param item

	@retval POKETOOL_CHECKRESULT_OK				�^����ꂽ�|�P�����f�[�^�͐���
	@retval POKETOOL_CHECKRESULT_ILLEGAL_DATA	�^����ꂽ�|�P�����f�[�^�����Ă���i�����̃G���R�[�h���ł��Ȃ������Ƃ��Ȃǁj
	@retval POKETOOL_CHECKRESULT_CHEAT_DATA		�^����ꂽ�|�P�����f�[�^�͕s��
*/
PokeToolCheckResult PokeTool_CheckItem(const u32 item)
{
	// ����No�̃`�F�b�N
	if(item > POKETOOL_MAX_ALLOWED_ITEMNO
		|| (item >= POKETOOL_ITEMNO_RESERVED_START && item <= POKETOOL_ITEMNO_RESERVED_END)
        || item == 16   // �v���V���X�{�[��
        || item == 209  // �~�N���̂�
        || item == 210  // �C�o���̂�
        || item == 211  // �W���{�̂�
        || item == 212) // �����v�̂�
	{
		return POKETOOL_CHECKRESULT_CHEAT_DATA;
	}

	return POKETOOL_CHECKRESULT_OK;
}

/*!
	�|�P�����ɕs���ȃt���O�������Ă��邩�ۂ���Ԃ��܂��B

	@param monsno		�s���`�F�b�N����|�P����No
	@param paraEventGet	�s���`�F�b�N����|�P�����̔z�z�t���O
	@param rare			�s���`�F�b�N����|�P�����̃��A�t���O

	@retval POKETOOL_CHECKRESULT_OK				�^����ꂽ�|�P�����f�[�^�͐���
	@retval POKETOOL_CHECKRESULT_ILLEGAL_DATA	�^����ꂽ�|�P�����f�[�^�����Ă���i�����̃G���R�[�h���ł��Ȃ������Ƃ��Ȃǁj
	@retval POKETOOL_CHECKRESULT_CHEAT_DATA		�^����ꂽ�|�P�����f�[�^�͕s��
*/
PokeToolCheckResult PokeTool_CheckFlag(const u32 monsno, const BOOL paraEventGet, const BOOL rare)
{
	// �Z���r�B �� 251
	// �W���[�` �� 385
	// �}�i�t�B �� 490
	// ���A�t���O�`�F�b�N
	if(monsno == 251 || monsno == 385 || monsno == 490){
		if(rare){
			return POKETOOL_CHECKRESULT_CHEAT_DATA;
		}
	}

	// �~���E �� 151
	// �f�I�L�V�X �� 386
	// �}�i�t�B �� 490
	// �z�z�t���O�`�F�b�N
	if(monsno == 151 || monsno == 386 || monsno == 490){
		if(!paraEventGet){
			return POKETOOL_CHECKRESULT_CHEAT_DATA;
		}
	}

	return POKETOOL_CHECKRESULT_OK;
}

/*!
	�|�P�����̍��R�[�h���`�F�b�N���܂��B

	@param countryCode		�s���`�F�b�N����|�P�����̍��R�[�h

	@retval POKETOOL_CHECKRESULT_OK				�^����ꂽ�|�P�����f�[�^�͐���
	@retval POKETOOL_CHECKRESULT_ILLEGAL_DATA	�^����ꂽ�|�P�����f�[�^�����Ă���i�����̃G���R�[�h���ł��Ȃ������Ƃ��Ȃǁj
	@retval POKETOOL_CHECKRESULT_CHEAT_DATA		�^����ꂽ�|�P�����f�[�^�͕s��
*/
PokeToolCheckResult PokeTool_CheckCountryCode(const u32 countryCode)
{
	if(countryCode == 0 || countryCode == 6 || countryCode >= 9){
		return POKETOOL_CHECKRESULT_CHEAT_DATA;
	}

	return POKETOOL_CHECKRESULT_OK;
}

/*!
	�`���n�|�P�����̍Œ჌�x�����`�F�b�N���܂��B

	@param monsno		�s���`�F�b�N����|�P����No
	@param level		�s���`�F�b�N����|�P�����̃��x��

	@retval POKETOOL_CHECKRESULT_OK				�^����ꂽ�|�P�����f�[�^�͐���
	@retval POKETOOL_CHECKRESULT_ILLEGAL_DATA	�^����ꂽ�|�P�����f�[�^�����Ă���i�����̃G���R�[�h���ł��Ȃ������Ƃ��Ȃǁj
	@retval POKETOOL_CHECKRESULT_CHEAT_DATA		�^����ꂽ�|�P�����f�[�^�͕s��
*/
PokeToolCheckResult PokeTool_CheckPokemonLv(const u32 monsno, const u32 level)
{
	if(level < PokeTool_GetPokemonMinimumLv(monsno)){
		return POKETOOL_CHECKRESULT_CHEAT_DATA;
	}
	return POKETOOL_CHECKRESULT_OK;
}

/*!
	�|�P�����̍Œ჌�x�����擾���܂��B

	@param monsno		�s���`�F�b�N����|�P����No

	@retval �w�肳�ꂽ�|�P����No�̍Œ჌�x���B�`���n�ȊO��No���^����ꂽ�ꍇ��0�B
*/
u32 PokeTool_GetPokemonMinimumLv(const u32 monsno)
{
	switch(monsno){
		case 1:
  			return 1;
  		case 2:
  			return 16;
  		case 3:
  			return 32;
  		case 4:
  			return 1;
  		case 5:
  			return 16;
  		case 6:
  			return 36;
  		case 7:
  			return 1;
  		case 8:
  			return 16;
  		case 9:
  			return 36;
  		case 25:
  			return 2;
  		case 26:
  			return 2;
  		case 65:
  			return 16;
  		case 68:
  			return 28;
  		case 94:
  			return 16;
  		case 130:
  			return 5;
  		case 139:
  			return 40;
  		case 141:
  			return 40;
  		case 144:
  			return 50;
  		case 145:
  			return 50;
  		case 146:
  			return 50;
  		case 149:
  			return 55;
  		case 150:
  			return 70;
  		case 151:
  			return 10;
  		case 152:
  			return 1;
  		case 153:
  			return 16;
  		case 154:
  			return 32;
  		case 155:
  			return 1;
  		case 156:
  			return 14;
  		case 157:
  			return 36;
  		case 158:
  			return 1;
  		case 159:
  			return 18;
  		case 160:
  			return 30;
  		case 217:
  			return 30;
  		case 230:
  			return 25;
  		case 243:
  			return 40;
  		case 244:
  			return 40;
  		case 245:
  			return 40;
  		case 248:
  			return 55;
  		case 249:
  			return 50;
  		case 250:
  			return 70;
  		case 251:
  			return 10;
  		case 252:
  			return 1;
  		case 253:
  			return 16;
  		case 254:
  			return 36;
  		case 255:
  			return 1;
  		case 256:
  			return 16;
  		case 257:
  			return 36;
  		case 258:
  			return 1;
  		case 259:
  			return 16;
  		case 260:
  			return 36;
  		case 272:
  			return 14;
  		case 275:
  			return 14;
  		case 282:
  			return 30;
  		case 286:
  			return 23;
  		case 287:
  			return 1;
  		case 288:
  			return 18;
  		case 289:
  			return 36;
  		case 290:
  			return 1;
  		case 291:
  			return 20;
  		case 292:
  			return 20;
  		case 323:
  			return 33;
  		case 330:
  			return 45;
  		case 346:
  			return 40;
  		case 348:
  			return 40;
  		case 354:
  			return 27;
  		case 356:
  			return 37;
  		case 362:
  			return 42;
  		case 365:
  			return 44;
  		case 373:
  			return 50;
  		case 376:
  			return 45;
  		case 377:
  			return 40;
  		case 378:
  			return 40;
  		case 379:
  			return 40;
  		case 380:
  			return 40;
  		case 381:
  			return 40;
  		case 382:
  			return 45;
  		case 383:
  			return 45;
  		case 384:
  			return 70;
  		case 385:
  			return 5;
  		case 386:
  			return 30;
  		case 387:
  			return 1;
  		case 388:
  			return 18;
  		case 389:
  			return 32;
  		case 390:
  			return 1;
  		case 391:
  			return 14;
  		case 392:
  			return 36;
  		case 393:
  			return 1;
  		case 394:
  			return 16;
  		case 395:
  			return 36;
  		case 398:
  			return 34;
  		case 405:
  			return 30;
  		case 409:
  			return 30;
  		case 411:
  			return 30;
  		case 437:
  			return 33;
  		case 445:
  			return 48;
  		case 450:
  			return 34;
  		case 452:
  			return 22;
  		case 460:
  			return 40;
  		case 462:
  			return 27;
  		case 464:
  			return 42;
  		case 466:
  			return 10;
  		case 467:
  			return 10;
  		case 473:
  			return 34;
  		case 475:
  			return 6;
  		case 477:
  			return 37;
  		case 479:
  			return 1;
  		case 480:
  			return 50;
  		case 481:
  			return 50;
  		case 482:
  			return 50;
  		case 483:
  			return 47;
  		case 484:
  			return 47;
  		case 485:
  			return 70;
  		case 486:
  			return 70;
  		case 487:
  			return 70;
  		case 488:
  			return 50;
  		case 489:
  			return 1;
  		case 490:
  			return 1;
  		case 491:
  			return 50;
  		case 492:
  			return 30;
  		case 493:
  			return 80;
  		default:
  			return 0;
  	}
}

/*!
	�|�P�����̕ߊl�ꏊ���`�F�b�N���܂��B

    �Ȃ����ϐ������t�ŁADP�̃f�o�b�O���[�h���o�O���Ă���B
    �f�o�b�O���[�h�Ŕz�z�����|�P���������łɂ���̂�place��birthPlace�̂����ꂩ���߂܂����ꏊ�Ȃ�悢���ƂƂ���

	@param monsno		�s���`�F�b�N����|�P����No
    @param place        �ߊl�ꏊ(���ۂ͐��܂ꂽ�ꏊ������)
    @param birthPlace   ���܂ꂽ�ꏊ(���ۂ͕ߊl�ꏊ������)

	@retval POKETOOL_CHECKRESULT_OK				�^����ꂽ�|�P�����f�[�^�͐���
	@retval POKETOOL_CHECKRESULT_CHEAT_DATA		�^����ꂽ�|�P�����f�[�^�͕s��
*/
u32 PokeTool_CheckCapturePlace(const u32 monsno, u32 place, u32 birthPlace)
{
    u32 defaultPlace = PokeTool_GetCapturePlace(monsno);
    if(defaultPlace == 0 || defaultPlace == place || defaultPlace == birthPlace){
        return POKETOOL_CHECKRESULT_OK;
    }else{
        return POKETOOL_CHECKRESULT_CHEAT_DATA;
    }
}

/*!
	�|�P�����̓K�؂ȕߊl�ꏊ���擾���܂��B

	@param monsno		�擾����|�P����No

	@retval �w�肳�ꂽ�|�P����No�̓K�؂ȕߊl�ꏊ�B
	@retval 0 �ߊl�ꏊ�̔�������Ȃ��B
*/
u32 PokeTool_GetCapturePlace(u32 monsno)
{
	switch(monsno){
		case 144:
  			return 55;
  		case 145:
  			return 55;
  		case 146:
  			return 55;
  		case 150:
  			return 55;
  		case 243:
  			return 55;
  		case 244:
  			return 55;
  		case 245:
  			return 55;
  		case 249:
  			return 55;
  		case 250:
  			return 55;
  		case 251:
  			return 55;
  		case 377:
  			return 55;
  		case 378:
  			return 55;
  		case 379:
  			return 55;
  		case 380:
  			return 55;
  		case 381:
  			return 55;
  		case 382:
  			return 55;
  		case 383:
  			return 55;
  		case 384:
  			return 55;
  		case 480:
  			return 89;
  		case 482:
  			return 88;
  		case 483:
  			return 51;
  		case 484:
  			return 51;
  		case 485:
  			return 84;
  		case 486:
  			return 64;
  		case 487:
  			return 62;
  		case 491:
  			return 3005;
  		default:
  			return 0;
  	}
}

/*!
	�|�P�����̕ߊl�J�Z�b�g���`�F�b�N���܂��B

	@param monsno		�s���`�F�b�N����|�P����No

	@retval POKETOOL_CHECKRESULT_OK				�^����ꂽ�|�P�����f�[�^�͐���
	@retval POKETOOL_CHECKRESULT_CHEAT_DATA		�^����ꂽ�|�P�����f�[�^�͕s��
*/
u32 PokeTool_CheckCaptureCassette(const u32 monsno, u32 cassette)
{
    u32 defaultCassette = PokeTool_GetCaptureCassette(monsno);
    if(defaultCassette & cassette){
        return POKETOOL_CHECKRESULT_OK;
    }else{
        return POKETOOL_CHECKRESULT_CHEAT_DATA;
    }
}

/*!
	�|�P�����̓K�؂ȕߊl�J�Z�b�g���擾���܂��B

	@param monsno		�擾����|�P����No

	@retval �w�肳�ꂽ�|�P����No�̓K�؂ȕߊl�J�Z�b�g�̃r�b�g�t���O�B
    @retval 0xffffffff �C�ӂ̏ꏊ��OK�B
*/
u32 PokeTool_GetCaptureCassette(u32 monsno)
{
	switch(monsno){
		case 144:
  			return 16415;
  		case 145:
  			return 16415;
  		case 146:
  			return 16415;
  		case 150:
  			return 24;
  		case 151:
  			return 31;
  		case 243:
  			return 16415;
  		case 244:
  			return 16415;
  		case 245:
  			return 16415;
  		case 249:
  			return 16412;
  		case 250:
  			return 16412;
  		case 251:
  			return 16415;
  		case 377:
  			return 7;
  		case 378:
  			return 7;
  		case 379:
  			return 7;
  		case 380:
  			return 31;
  		case 381:
  			return 31;
  		case 382:
  			return 7;
  		case 383:
  			return 7;
  		case 384:
  			return 7;
  		case 385:
  			return 31;
  		case 386:
  			return 31;
  		default:
  			return 0xffffffff;
  	}
}