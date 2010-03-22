/*===========================================================================*
  Project:  Syachi PokemonGDS Network Library
  File:     poke_net_gds_debug.c

  $NoKeywords: $
 *===========================================================================*/
/*! @file
	@brief	Syachi Pokemon GDS WiFi ���C�u����

*/
//===========================================================================

#include "poke_net.h"
#include "poke_net_gds.h"
#include "poke_net_common.h"					// Windows/Linux/DS���ʃw�b�_


//====================================
// DEBUG:���[�U�[�v���t�B�[���\��
//====================================
//====================================
/*!
	�|�P���� WiFi GDS �v���t�B�[�����e�̃f�o�b�O�\��

	@param[in]	_response	�v���t�B�[���\���̂̐擪�A�h���X
*/
//====================================
void POKE_NET_GDS_DEBUG_PrintProfile(GDS_PROFILE* _profile)
{
	int i = 0;
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"***** profile >> **********************\n");

//	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "name >> Memory dump\n");
//	for(i = 0; i < PLW_TRAINER_NAME_SIZE; i++)
//	{
//		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "%04x ", _profile->name[i]);
//	}
//	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "\ndump end.\n");

	// GDS_EOM��0�ɕύX
	for(i = 0; i < PLW_TRAINER_NAME_SIZE; i++)
	{
		if(_profile->name[i] == 0xFFFF)
		{
			_profile->name[i] = 0;
			break;
		}
	}

	POKE_NET_DebugPrintfW(POKE_NET_DEBUGLEVEL_NORMAL, L"name          :%ls\n", (wchar_t*)_profile->name);	// STRCODE
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"player_id     :%d\n" ,_profile->player_id);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"player_sex    :%d\n" ,_profile->player_sex);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"birthday_month:%d\n" ,_profile->birthday_month);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"trainer_view  :%d\n" ,_profile->trainer_view);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"country_code  :%d\n" ,_profile->country_code);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"local_code    :%d\n" ,_profile->local_code);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"version_code  :%d\n" ,_profile->version_code);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"language      :%d\n" ,_profile->language);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"egg_flag      :%d\n" ,_profile->egg_flag);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"form_no       :%d\n" ,_profile->form_no);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"mons_sex      :%d\n" ,_profile->mons_sex);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"monsno        :%d\n" ,_profile->monsno);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"message_flag  :%d\n" ,_profile->message_flag);

	if(_profile->message_flag == 0)
	{
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"self_introduction.sentence_type:%d\n" ,_profile->self_introduction.sentence_type);
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"self_introduction.sentence_id:%d\n" ,_profile->self_introduction.sentence_id);
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"self_introduction.word[0]:%d\n" ,_profile->self_introduction.word[0]);
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"self_introduction.word[1]:%d\n" ,_profile->self_introduction.word[1]);
	}
	else
	{
//		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "event_self_introduction >> Memory dump\n");
//		for(i = 0; i < EVENT_SELF_INTRO; i++)
//		{
//			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "%04x ", _profile->event_self_introduction[i]);
//		}

		// GDS_EOM��0�ɕύX
		for(i = 0; i < EVENT_SELF_INTRO; i++)
		{
			if(_profile->event_self_introduction[i] == 0xFFFF)
			{
				_profile->event_self_introduction[i] = 0;
				break;
			}
		}

		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "\ndump end.\n");
		POKE_NET_DebugPrintfW(POKE_NET_DEBUGLEVEL_NORMAL ,L"STRCODE event_self_introduction:%ls\n" , (wchar_t*)_profile->event_self_introduction);	// STRCODE
	}

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"********************** << profile *****\n");
}


//====================================
/*!
	�|�P���� WiFi GDS �v���t�B�[�����e�̃f�o�b�O�\��(��񏭂Ȃ�)

	@param[in]	_response	�v���t�B�[���\���̂̐擪�A�h���X
*/
//====================================
void POKE_NET_GDS_DEBUG_PrintProfile_Light(GDS_PROFILE* _profile)
{
	int i = 0;

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"***** profile >> **********************\n");

//	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "name >> Memory dump\n");
//	for(i = 0; i < PLW_TRAINER_NAME_SIZE; i++)
//	{
//		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "%04x ", _profile->name[i]);
//	}
//	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "\ndump end.\n");
	
	// GDS_EOM��0�ɕύX
	for(i = 0; i < PLW_TRAINER_NAME_SIZE; i++)
	{
		if(_profile->name[i] == 0xFFFF)
		{
			_profile->name[i] = 0;
			break;
		}
	}

	POKE_NET_DebugPrintfW(POKE_NET_DEBUGLEVEL_NORMAL ,L"name          :%ls\n" , (wchar_t*)_profile->name);	// STRCODE
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"player_id     :%d\n" ,_profile->player_id);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"message_flag  :%d\n" ,_profile->message_flag);

	if(_profile->message_flag != 0)
	{
		// GDS_EOM��0�ɕύX
		for(i = 0; i < EVENT_SELF_INTRO; i++)
		{
			if(_profile->event_self_introduction[i] == 0xFFFF)
			{
				_profile->event_self_introduction[i] = 0;
				break;
			}
		}

		POKE_NET_DebugPrintfW(POKE_NET_DEBUGLEVEL_NORMAL ,L"STRCODE event_self_introduction:%ls\n" ,(wchar_t*)_profile->event_self_introduction);	// STRCODE
	}

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"********************** << profile *****\n");
}


//====================================
/*!
	�|�P���� WiFi GDS �~���[�W�J���V���b�g���e�̕\��

	@param[in]	_response	MUSICAL_SHOT_DATA�\���̂̐擪�A�h���X
*/
//====================================
void POKE_NET_GDS_DEBUG_PrintMusicalshot(MUSICAL_SHOT_DATA* _pMusical)
{
	int i = 0;
	int j = 0;

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"***** Musicalshot >> ******************\n");

	// MUSICAL_SHOT_DATA�^�̃_���v
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "bgNo=%d, ",		_pMusical->bgNo		);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "spotBit=0x%02X\n",_pMusical->spotBit	);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "year=%d, ",		_pMusical->year		);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "month=%d, ",		_pMusical->month	);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "day=%d\n",		_pMusical->day		);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "player=%d, ",		_pMusical->player	);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "musVer=%d\n",		_pMusical->musVer	);// �~���[�W�J���o�[�W����

//	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "title >> Memory dump\n");
//	for(i = 0; i < MUSICAL_PROGRAM_NAME_MAX; i++)
//	{
//		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "%04x ", _pMusical->title[i]);
//	}
//	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "\ndump end.\n");

	// GDS_EOM��0�ɕύX
	for(i = 0; i < MUSICAL_PROGRAM_NAME_MAX; i++)
	{
		if(_pMusical->title[i] == 0xFFFF)
		{
			_pMusical->title[i] = 0;
			break;
		}
	}

	POKE_NET_DebugPrintfW(POKE_NET_DEBUGLEVEL_NORMAL, L"title=%ls\n",	(wchar_t*)_pMusical->title	);	// STRCODE
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "pmVersion=%d\n",	_pMusical->pmVersion);// PM_VERSION
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "pmLang=%d\n",		_pMusical->pmLang	);// PM_LANG

	// �|�P�������(MUSICAL_SHOT_POKE)�~4�̕��̃_���v
	for(i = 0; i < MUSICAL_POKE_MAX; i++)
	{
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "shotPoke[%d]::monsno=%d\n",		i, _pMusical->shotPoke[i].monsno		);
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "shotPoke[%d]::sex=%d\n",			i, _pMusical->shotPoke[i].sex			);
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "shotPoke[%d]::rare=%d\n",			i, _pMusical->shotPoke[i].rare			);
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "shotPoke[%d]::form=%d\n",			i, _pMusical->shotPoke[i].form			);

//		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "trainerName >> Memory dump\n");
//		for(j = 0; j < MUSICAL_TRAINER_NAME_LEN; j++)
//		{
//			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "%04x ", _pMusical->shotPoke[i].trainerName[j]);
//		}
//		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "\ndump end.\n");

		// GDS_EOM��0�ɕύX
		for(j = 0; j < MUSICAL_TRAINER_NAME_LEN; j++)
		{
			if(_pMusical->shotPoke[i].trainerName[j] == 0xFFFF)
			{
				_pMusical->shotPoke[i].trainerName[j] = 0;
				break;
			}
		}
		
		POKE_NET_DebugPrintfW(POKE_NET_DEBUGLEVEL_NORMAL, L"shotPoke[%d]::trainerName=%ls\n",	i, (wchar_t*)_pMusical->shotPoke[i].trainerName	);	// STRCODE

		// �|�P�������̑����f�[�^(MUSICAL_SHOT_POKE_EQUIP)�~8�̃_���v
		for(j = 0; j < MUSICAL_ITEM_EQUIP_MAX; j++)
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "shotPoke[%d]::equip[%d]::itemNo:%d\n",	i, j, _pMusical->shotPoke[i].equip[j].itemNo	);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "shotPoke[%d]::equip[%d]::angle:%d\n",		i, j, _pMusical->shotPoke[i].equip[j].angle		);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "shotPoke[%d]::equip[%d]::equipPos:%d\n",	i, j, _pMusical->shotPoke[i].equip[j].equipPos	);
		}
	}

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"****************** << Musicalshot *****\n");
}


//====================================
/*!
	�|�P���� WiFi GDS �o�g���w�b�_���e�̕\��

	@param[in]	_response	BATTLE_REC_HEADER�\���̂̐擪�A�h���X
*/
//====================================
void POKE_NET_GDS_DEBUG_PrintBattleHeader(BATTLE_REC_HEADER* _pBattleHeader)
{
	int i = 0;

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"***** BattleHeader >> *****************\n");

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "battle_counter=%d\n", _pBattleHeader->battle_counter	);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "mode=0x%04X\n",		_pBattleHeader->mode			);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "magic_key=0x%04X\n",	_pBattleHeader->magic_key		);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "secure=%d\n",			_pBattleHeader->secure			);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "server_vesion=%d\n",	_pBattleHeader->server_vesion	);

#ifdef ___POKE_NET_BUILD_WINDOWS___
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "data_number=%I64d\n", _pBattleHeader->data_number		);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "data_number(Decoded)=%I64d\n", POKE_NET_DecodePlayDataID(_pBattleHeader->data_number));
#else
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "data_number=%lld\n",	_pBattleHeader->data_number		);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "data_number(Decoded)=%lld\n", POKE_NET_DecodePlayDataID(_pBattleHeader->data_number));
#endif

	for(i = 0; i < HEADER_MONSNO_MAX; i++)
	{
		POKE_NET_DebugPrintf(
			POKE_NET_DEBUGLEVEL_NORMAL,
			"monsno[%2d]=%3d, form_no_and_sex[%2d]=0x%02x\n",
			i,
			_pBattleHeader->monsno[i],
			i,
			_pBattleHeader->form_no_and_sex[i]
		);
	}

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"***************** << BattleHeader *****\n");
}


//====================================
/*!
	�|�P���� WiFi GDS �o�g���r�f�I�{�̂̕\��
	�ꕔ�\�����ȗ����Ă���f�[�^������܂��B(�Y�����ڂ�"Display omission"�ƕ\��)

	@param[in]	_response	�v���t�B�[���\���̂̐擪�A�h���X
*/
//====================================
void POKE_NET_GDS_DEBUG_PrintBattleData(BATTLE_REC_WORK* _pBattleData)
{
	int i = 0;
	int j = 0;

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"***** BattleData >> *******************\n");

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "magic_key=%d\n", _pBattleData->magic_key);

	// BTLREC_SETUP_SUBSET�\����
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "--BTLREC_SETUP_SUBSET--\n");
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "->musicDefault=%d\n", _pBattleData->setupSubset.musicDefault);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "->musicPinch=%d\n", _pBattleData->setupSubset.musicPinch);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "->debugFlagBit=%d\n", _pBattleData->setupSubset.debugFlagBit);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "->competitor=%d\n", _pBattleData->setupSubset.competitor);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "->myCommPos=%d\n", _pBattleData->setupSubset.myCommPos);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "->rule=%d\n", _pBattleData->setupSubset.rule);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "->fMultiMode=%d\n", _pBattleData->setupSubset.fMultiMode);

	// BTLREC_SETUP_SUBSET::GFL_STD_RandContext�\����
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "-> --GFL_STD_RandContext--\n");
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->x=%I64d\n", _pBattleData->setupSubset.randomContext.x);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->mul=%I64d\n", _pBattleData->setupSubset.randomContext.mul);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->add=%I64d\n", _pBattleData->setupSubset.randomContext.add);

	// BTLREC_SETUP_SUBSET::CONFIG�\����
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "-> --CONFIG--\n");
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->msg_speed=%d\n", _pBattleData->setupSubset.config.msg_speed);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->sound_mode=%d\n", _pBattleData->setupSubset.config.sound_mode);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->battle_rule=%d\n", _pBattleData->setupSubset.config.battle_rule);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->wazaeff_mode=%d\n", _pBattleData->setupSubset.config.wazaeff_mode);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->moji_mode=%d\n", _pBattleData->setupSubset.config.moji_mode);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->wirelesssave_mode=%d\n", _pBattleData->setupSubset.config.wirelesssave_mode);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->network_search=%d\n", _pBattleData->setupSubset.config.network_search);

	// BTLREC_SETUP_SUBSET::BTL_FIELD_SITUATION�\����
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "-> --BTL_FIELD_SITUATION--\n");
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->bgType=%d\n", _pBattleData->setupSubset.fieldSituation.bgType);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->bgAttr=%d\n", _pBattleData->setupSubset.fieldSituation.bgAttr);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->weather=%d\n", _pBattleData->setupSubset.fieldSituation.weather);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->timeZone=%d\n", _pBattleData->setupSubset.fieldSituation.timeZone);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->season=%d\n", _pBattleData->setupSubset.fieldSituation.season);

	// BTLREC_OPERATION_BUFFER�\����
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "--BTLREC_OPERATION_BUFFER--\n");
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->size=%d\n", _pBattleData->opBuffer.size);
	// buffer�͕\�����Ȃ��B�\��������Ȃ��̂ŁB
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   ->buffer=\"Display omission\"\n");

	// BTLREC_CLIENT_STATUS�\����
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "--BTLREC_CLIENT_STATUS--\n");
	for(i = 0; i < BTL_CLIENT_MAX; i++)
	{
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[%d]->type=%d\n", i, _pBattleData->clientStatus[i].type);
		
		if(_pBattleData->clientStatus[i].type == BTLREC_CLIENTSTATUS_PLAYER)
		{
			// BTLREC_CLIENT_STATUS::MYSTATUS
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[%d]-> --MYSTATUS--\n", i);

//			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "name >> Memory dump\n");
//			for(j = 0; j < PERSON_NAME_SIZE + EOM_SIZE; j++)
//			{
//				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "%04x ", _pBattleData->clientStatus[i].player.name[j]);
//			}
//			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "\ndump end.\n");

			// GDS_EOM��0�ɕύX
			for(j = 0; j < PERSON_NAME_SIZE + EOM_SIZE; j++)
			{
				if(_pBattleData->clientStatus[i].player.name[j] == 0xFFFF)
				{
					_pBattleData->clientStatus[i].player.name[j] = 0;
					break;
				}
			}

			POKE_NET_DebugPrintfW(POKE_NET_DEBUGLEVEL_NORMAL, L"   [%d]->name=%ls\n", i, (wchar_t*)_pBattleData->clientStatus[i].player.name);	// STRCODE
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->id=%d\n", i, _pBattleData->clientStatus[i].player.id);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->profileID=%d\n", i, _pBattleData->clientStatus[i].player.profileID);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->nation=%d\n", i, _pBattleData->clientStatus[i].player.nation);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->area=%d\n", i, _pBattleData->clientStatus[i].player.area);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->region_code=%d\n", i, _pBattleData->clientStatus[i].player.region_code);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->rom_code=%d\n", i, _pBattleData->clientStatus[i].player.rom_code);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->trainer_view=%d\n", i, _pBattleData->clientStatus[i].player.trainer_view);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->sex=%d\n", i, _pBattleData->clientStatus[i].player.sex);
		}
		else if(_pBattleData->clientStatus[i].type == BTLREC_CLIENTSTATUS_TRAINER)
		{
			// BTLREC_CLIENT_STATUS::BTLREC_TRAINER_STATUS
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[%d]-> --BTLREC_TRAINER_STATUS--\n", i);

//			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "name >> Memory dump\n");
//			for(j = 0; j < BUFLEN_PERSON_NAME; j++)
//			{
//				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "%04x ", _pBattleData->clientStatus[i].trainer.name[j]);
//			}
//			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "\ndump end.\n");

			// GDS_EOM��0�ɕύX
			for(j = 0; j < BUFLEN_PERSON_NAME; j++)
			{
				if(_pBattleData->clientStatus[i].trainer.name[j] == 0xFFFF)
				{
					_pBattleData->clientStatus[i].trainer.name[j] = 0;
					break;
				}
			}

			POKE_NET_DebugPrintfW(POKE_NET_DEBUGLEVEL_NORMAL, L"   [%d]->name=%ls\n", i, (wchar_t*)_pBattleData->clientStatus[i].trainer.name);	// STRCODE
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->ai_bit=%d\n", i, _pBattleData->clientStatus[i].trainer.ai_bit);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->tr_id=%d\n", i, _pBattleData->clientStatus[i].trainer.tr_id);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->tr_type=%d\n", i, _pBattleData->clientStatus[i].trainer.tr_type);

			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->use_item[0]=%d\n", i, _pBattleData->clientStatus[i].trainer.use_item[0]);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->use_item[1]=%d\n", i, _pBattleData->clientStatus[i].trainer.use_item[1]);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->use_item[2]=%d\n", i, _pBattleData->clientStatus[i].trainer.use_item[2]);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->use_item[3]=%d\n", i, _pBattleData->clientStatus[i].trainer.use_item[3]);

			// �����Z���t
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->win_word.sentence_type=%d\n", i, _pBattleData->clientStatus[i].trainer.win_word.sentence_type);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->win_word.sentence_id=%d\n", i, _pBattleData->clientStatus[i].trainer.win_word.sentence_id);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->win_word.word[0]=%d\n", i, _pBattleData->clientStatus[i].trainer.win_word.word[0]);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->win_word.word[1]=%d\n", i, _pBattleData->clientStatus[i].trainer.win_word.word[1]);

			// �����Z���t
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->lose_word.sentence_type=%d\n", i, _pBattleData->clientStatus[i].trainer.lose_word.sentence_type);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->lose_word.sentence_id=%d\n", i, _pBattleData->clientStatus[i].trainer.lose_word.sentence_id);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->lose_word.word[0]=%d\n", i, _pBattleData->clientStatus[i].trainer.lose_word.word[0]);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "   [%d]->lose_word.word[1]=%d\n", i, _pBattleData->clientStatus[i].trainer.lose_word.word[1]);
		}
	}

	// REC_POKEPARTY�\����
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "--REC_POKEPARTY--\n");
	for(i = 0; i < BTL_CLIENT_MAX; i++)
	{
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[%d]->PokeCountMax=%d\n", i, _pBattleData->rec_party[i].PokeCountMax);
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[%d]->PokeCount=%d\n", i, _pBattleData->rec_party[i].PokeCount);
		// REC_POKEPARA�͕\�����Ȃ��B�\��������Ȃ��̂ŁB
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[%d]->member=\"Display omission\"\n", i);
	}

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"******************* << BattleData *****\n");
}


//====================================
/*!
	�|�P���� WiFi GDS ���X�|���X���e�̃f�o�b�O�\��

	@param[in]	_response	���X�|���X�\���̂̐擪�A�h���X
	@param[in]	_size		���X�|���X�T�C�Y
*/
//====================================
void POKE_NET_GDS_DEBUG_PrintResponse(POKE_NET_RESPONSE *_response ,long _size)
{
	long i;
#ifdef ___POKE_NET_BUILD_WINDOWS___
	long j;
#endif

	// �T�C�Y
	long paramsize = (long)(_size - sizeof(POKE_NET_RESPONSE));

	// ------------ ��������o�� ------------

	// �w�b�_�[�o��
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "\n###### POKE_NET_GDS_DEBUG_PrintResponse >>> ######\n");

	// ���ʏ����o��
	POKE_NET_DebugPrintf(
		POKE_NET_DEBUGLEVEL_NORMAL,
		"ReqCode=%d, Size=%d, ParamSize=%d, Result=%d\n",
		_response->ReqCode,
		_size,
		paramsize,
		_response->Result
	);

	//////////////////////////////////////////////////////////
	// �f�o�b�O�p���N�G�X�g
	//////////////////////////////////////////////////////////
	if(_response->ReqCode == POKE_NET_GDS_REQCODE_DEBUG_MESSAGE)
	{
		POKE_NET_GDS_RESPONSE_DEBUG_MESSAGE *_responseparam;
		if( paramsize >= sizeof(POKE_NET_GDS_RESPONSE_DEBUG_MESSAGE) )
		{
			_responseparam = (POKE_NET_GDS_RESPONSE_DEBUG_MESSAGE *)_response->Param;
			
			// �\��
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_DEBUG_MESSAGE] Success\n");
			POKE_NET_DebugPrintf(
				POKE_NET_DEBUGLEVEL_NORMAL,
				"ResultMessage >> ------------------\n%s\n------------------ << ResultMessage\n",
				_responseparam->ResultMessage
			);
		}
		else
		{
			// �\��
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_DEBUG_MESSAGE] Error.\n");
		}
	}

	//////////////////////////////////////////////////////////
	// �~���[�W�J���V���b�g�o�^
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST)
	{
		POKE_NET_GDS_RESPONSE_MUSICALSHOT_REGIST *_responseparam;

		if( _response->Result != POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_SUCCESS )
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST] Error\n");
		}
		else
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST] Success\n");
			_responseparam = (POKE_NET_GDS_RESPONSE_MUSICALSHOT_REGIST *)_response->Param;

#ifdef ___POKE_NET_BUILD_WINDOWS___
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code:%I64d\n" ,_responseparam->Code);
#else
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code:%lld\n" ,_responseparam->Code);
#endif
		}
	}

	//////////////////////////////////////////////////////////
	// �~���[�W�J���V���b�g�擾
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_MUSICALSHOT_GET)
	{
		POKE_NET_GDS_RESPONSE_MUSICALSHOT_GET *_responseparam;

		if( _response->Result != POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_GET_SUCCESS )
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"[RESPONSE:POKE_NET_GDS_REQCODE_MUSICALSHOT_GET] Error\n");
		}
		else
		{
			_responseparam = (POKE_NET_GDS_RESPONSE_MUSICALSHOT_GET *)_response->Param;

			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_MUSICALSHOT_GET] Success\n");
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "HitCount:%d\n", _responseparam->HitCount);

			for(i = 0; i < _responseparam->HitCount; i++)
			{
				// POKE_NET_GDS_RESPONSE_MUSICALSHOT_RECVDATA�^�̃_���v
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Data[%d]\n",	i							);
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "PID:%d\n",	_responseparam->Data[i].PID	);

#ifdef ___POKE_NET_BUILD_WINDOWS___
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code:%I64d\n",_responseparam->Data[i].Code);
#else
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code:%lld\n",	_responseparam->Data[i].Code);
#endif
				
				// �v���t�B�[���̃_���v
				POKE_NET_GDS_DEBUG_PrintProfile(&_responseparam->Data[i].Data.profile);

				// MUSICAL_SHOT_DATA�^�̃_���v
				POKE_NET_GDS_DEBUG_PrintMusicalshot(&_responseparam->Data[i].Data.mus_shot);
			}
		}
	}

	//////////////////////////////////////////////////////////
	// �o�g���r�f�I�f�[�^�o�^
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST)
	{
		POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST *_responseparam;
		_responseparam = (POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST *)_response->Param;

		if( _response->Result != POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_SUCCESS )
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST] Error\n");
		}
		else
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST] Success\n");

#ifdef ___POKE_NET_BUILD_WINDOWS___
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code=%I64d\n",			_responseparam->Code							);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code(Decoded)=%I64d\n",	POKE_NET_DecodePlayDataID(_responseparam->Code)	);
#else
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code=%lld\n",				_responseparam->Code							);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code(Decoded)=%lld\n",	POKE_NET_DecodePlayDataID(_responseparam->Code)	);
#endif

		}
	}

	//////////////////////////////////////////////////////////
	// �o�g���r�f�I�f�[�^����
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH)
	{
		POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH *_responseparam;
		POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_RECVDATA *_responsedata;

		if( _response->Result != POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_SUCCESS )
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH] Error\n");
		}
		else
		{
			_responseparam = (POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH *)_response->Param;
			
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH] Success\n");
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "HitCount:%d\n", _responseparam->HitCount);

			for(i = 0; i < _responseparam->HitCount; i++)
			{
				_responsedata = &_responseparam->Data[i];

				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Data[%d]\n",	i					);
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "PID:%d\n",	_responsedata->PID	);

				// �f�[�^��ID�\��
#ifdef ___POKE_NET_BUILD_WINDOWS___
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code=%I64d\n",			_responsedata->Code								);
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code(Decoded)=%I64d\n",	POKE_NET_DecodePlayDataID(_responsedata->Code)	);
#else
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code=%lld\n",				_responsedata->Code								);
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code(Decoded)=%lld\n",	POKE_NET_DecodePlayDataID(_responsedata->Code)	);
#endif

				// �v���t�B�[���\��
				POKE_NET_GDS_DEBUG_PrintProfile_Light(&_responsedata->Data.profile);

				// �o�g���r�f�I�w�b�_�\��
				POKE_NET_GDS_DEBUG_PrintBattleHeader(&_responsedata->Data.head);
			}
		}
	}

	//////////////////////////////////////////////////////////
	// �o�g���r�f�I�f�[�^�擾
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_BATTLEDATA_GET)
	{
		POKE_NET_GDS_RESPONSE_BATTLEDATA_GET *_responseparam;
		_responseparam = (POKE_NET_GDS_RESPONSE_BATTLEDATA_GET *)_response->Param;

		if( _response->Result != POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_SUCCESS )
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"[RESPONSE:POKE_NET_GDS_REQCODE_BATTLEDATA_GET] Error\n");
		}
		else
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"[RESPONSE:POKE_NET_GDS_REQCODE_BATTLEDATA_GET] Success\n");

			// �\��
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "PID:%d\n",	_responseparam->PID	);
			
			// �f�[�^��ID�\��
#ifdef ___POKE_NET_BUILD_WINDOWS___
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code=%I64d\n",			_responseparam->Code							);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code(Decoded)=%I64d\n",	POKE_NET_DecodePlayDataID(_responseparam->Code)	);
#else
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code=%lld\n",				_responseparam->Code							);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Code(Decoded)=%lld\n",	POKE_NET_DecodePlayDataID(_responseparam->Code)	);
#endif

			// �v���t�B�[���\��
			POKE_NET_GDS_DEBUG_PrintProfile(&_responseparam->Data.profile);
			
			// �o�g���r�f�I�w�b�_�\��
			POKE_NET_GDS_DEBUG_PrintBattleHeader(&_responseparam->Data.head);

			// �o�g���f�[�^�{�̕\��
			POKE_NET_GDS_DEBUG_PrintBattleData(&_responseparam->Data.rec);
		}
	}

	//////////////////////////////////////////////////////////
	// �o�g���r�f�I�f�[�^���C�ɓ���o�^
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE)
	{
		if( _response->Result != POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_SUCCESS )
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE] Error\n");
		}
		else
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE] Success\n");
		}
	}


#ifdef ___POKE_NET_BUILD_WINDOWS___

	////////////////////////////////////////////////////////////
	//// �T�[�o�[�X�e�[�^�X�擾
	////////////////////////////////////////////////////////////
	//else if(_response->ReqCode == POKE_NET_GDS_REQCODE_ADMIN_SVR_STATUS)
	//{
	//	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUS *_responseparam;
	//	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA *_responseparamdata;

	//	if( _response->Result != POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_STATUS_SUCCESS )
	//	{
	//		POKE_NET_DebugPrintf(
	//			POKE_NET_DEBUGLEVEL_NORMAL,
	//			"[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_SVR_STATUS] Error\n"
	//		);
	//	}
	//	else
	//	{
	//		_responseparam = (POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUS *)_response->Param;
	//		POKE_NET_DebugPrintf(
	//			POKE_NET_DEBUGLEVEL_NORMAL,
	//			"[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_SVR_STATUS] SVRCount:%d\n",
	//			_responseparam->ServerCount
	//		);

	//		for(i = 0; i < _responseparam->ServerCount; i++)
	//		{
	//			_responseparamdata = &_responseparam->Status[i];
	//			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL," -IP:%d.%d.%d.%d\n" ,(_responseparamdata->IP >> 24) & 0xFF ,(_responseparamdata->IP >> 16) & 0xFF ,(_responseparamdata->IP >> 8) & 0xFF ,(_responseparamdata->IP >> 0) & 0xFF );
	//			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"  TYPE  :%d\n" ,_responseparamdata->Type);
	//			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"  STATUS:%d\n" ,_responseparamdata->Status);
	//			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"  MAXCON:%d\n" ,_responseparamdata->MaxConnectCount);
	//			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"  CON   :%d\n" ,_responseparamdata->ConnectCount);
	//			POKE_NET_DebugPrintf(
	//				POKE_NET_DEBUGLEVEL_NORMAL,
	//				"  UpDate:%04I64d/%02I64d/%02I64d %02I64d:%02I64d:%02I64d\n" ,
	//				(_responseparamdata->LastUpdateTime >> 40) & 0xFFFF ,
	//				(_responseparamdata->LastUpdateTime >> 32) & 0xFF ,
	//				(_responseparamdata->LastUpdateTime >> 24) & 0xFF ,
	//				(_responseparamdata->LastUpdateTime >> 16) & 0xFF ,
	//				(_responseparamdata->LastUpdateTime >>  8) & 0xFF ,
	//				(_responseparamdata->LastUpdateTime >>  0) & 0xFF );
	//			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"  Req   :%d\n" ,_responseparamdata->Request);
	//		}
	//	}
	//}

	////////////////////////////////////////////////////////////
	//// �T�[�o�[�ւ̖���
	////////////////////////////////////////////////////////////
	//else if(_response->ReqCode == POKE_NET_GDS_REQCODE_ADMIN_SVR_COMMAND)
	//{
	//	if(_response->Result != POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND_SUCCESS)
	//	{
	//		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_SVR_COMMAND] Error\n");
	//	}
	//	else
	//	{
	//		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_SVR_COMMAND] Success\n");
	//	}
	//}

	//////////////////////////////////////////////////////////
    //  �~���[�W�J���V���b�g�F�S�̏��
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_ADMIN_MUSICALSHOT_STATUS)
	{
        POKE_NET_GDS_RESPONSE_ADMIN_MUSICALSHOT_STATUS *_responseparam;

        if( _response->Result != POKE_NET_GDS_RESPONSE_RESULT_ADMIN_MUSICALSHOT_STATUS_SUCCESS )
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_MUSICALSHOT_STATUS] Error\n");
        } 
		else 
		{
			_responseparam = (POKE_NET_GDS_RESPONSE_ADMIN_MUSICALSHOT_STATUS *)_response->Param;

			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_MUSICALSHOT_STATUS] Success\n");
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "TotalCount=%d\n", _responseparam->TotalCount);
        }
	}

	//////////////////////////////////////////////////////////
    //  �~���[�W�J���V���b�g�F�폜
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_ADMIN_MUSICALSHOT_DELETE)
	{
        if( _response->Result != POKE_NET_GDS_RESPONSE_RESULT_ADMIN_MUSICALSHOT_DELETE_SUCCESS )
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_MUSICALSHOT_DELETE] Error\n");
        } 
		else 
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_MUSICALSHOT_DELETE] Success\n");
        }
	}

	//////////////////////////////////////////////////////////
	// �o�g���r�f�I�f�[�^�̑S�̏����擾
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_STATUS)
	{
		POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_STATUS* pRes;

		if(_response->Result != POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_STATUS_SUCCESS)
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_STATUS] Error\n");
		}
		else
		{
			pRes = (POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_STATUS*)_response->Param;

			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_STATUS] Success\n");

			// ���̕\��
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "TotalRegistCount=%d",	pRes->TotalRegistCount	);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "TotalEventCount=%d\n",pRes->TotalEventCount	);
		}
	}

	//////////////////////////////////////////////////////////
	// �o�g���r�f�I�f�[�^�폜
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_DELETE)
	{
		if( _response->Result != POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_DELETE_SUCCESS )
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_DELETE] Error\n");
		}
		else
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_DELETE] Success\n");
		}
	}

	//////////////////////////////////////////////////////////
	// �o�g���r�f�I�����L���O�����̎擾
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_RANKHISTORY)
	{
		int Index = 0;
		POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY* pHistory;
		POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_RANKING* pRankList;
		POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_PARAM* pRankParam;

		if( _response->Result != POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_RANKHISTORY_SUCCESS )
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_RANKHISTORY] Error\n");
		}
		else
		{
			pHistory = (POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY*)_response->Param;

			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_RANKHISTORY] Success\n");
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"HitCount=%d\n", pHistory->HitCount);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"RankType=%d", pHistory->Type);

			Index = 0;
			for(i = 0; i < pHistory->HitCount; i++)
			{
				pRankList = (POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_RANKING*)&pHistory->RankingList[Index];

				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"RankingList[%d].RankListCount=%d\n", i, pRankList->Count);
				
				for(j = 0; j < pRankList->Count; j++)
				{
					pRankParam = (POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_PARAM*)&pRankList->List[j];

					// �e��p�����[�^�\��
					POKE_NET_DebugPrintf(
						POKE_NET_DEBUGLEVEL_NORMAL,
						"PID=%d, Code=%I64d(DecodedCode=%I64d), Point=%I64d, RegistTime=%I64d, FavoriteCount=%d\n",
						pRankParam->PID,
						pRankParam->Code,
						POKE_NET_DecodePlayDataID(pRankParam->Code),
						pRankParam->Point,
						pRankParam->RegistTime,
						pRankParam->FavoriteCount
					);

					// �v���t�B�[���\��
					POKE_NET_GDS_DEBUG_PrintProfile(&pRankParam->Data.profile);

					// �o�g���r�f�I�w�b�_���_���v
					POKE_NET_GDS_DEBUG_PrintBattleHeader(&pRankParam->Data.head);
				}

				Index += pRankList->Count * sizeof(POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_PARAM)
					+ sizeof(POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_RANKING);
			}
		}
	}

	//////////////////////////////////////////////////////////
	// �o�g���r�f�I�C�x���g�o�^
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EVENTFLAG)
	{
		if( _response->Result != POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFLAG_SUCCESS )
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EVENTFLAG] Error\n");
		}
		else
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EVENTFLAG] Success\n");
		}
	}

	//////////////////////////////////////////////////////////
	// �C�x���g�o�g���r�f�I�̕ҏW
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EVENTFREEWORD)
	{
		if( _response->Result != POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFREEWORD_SUCCESS )
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EVENTFREEWORD] Error\n");
		}
		else
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EVENTFREEWORD] Success\n");
		}
	}

	//////////////////////////////////////////////////////////
	// �o�g���r�f�I�f�[�^�̐ݒ�p�����[�^���擾
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_GETPARAM)
	{
		if(_response->Result != POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_GETPARAM_SUCCESS)
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_GETPARAM] Error\n");
		}
		else
		{
			POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_GETPARAM* pRes = (POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_GETPARAM*)_response->Param;

			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_GETPARAM] Success\n");

			// ���̕\��
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "InitPoint=%I64d\n",	pRes->InitPoint	);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "Bias=%I64d\n",		pRes->Bias		);

			// ���ʂɂ��d��
			for(i = 0; i < POKE_NET_GDS_BATTLEDATA_SETPARAM_WEIGHT_MAXNUM; i++)
			{
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL,
					"Weight[%d]=%d\n",
					i,
					pRes->Weight[i]
				);
			}

			// �o�ߓ����ɂ�链�_�{��
			for(i = 0; i < POKE_NET_GDS_BATTLEDATA_SETPARAM_SCALING_MAXNUM; i++)
			{
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL,
					"Scaling[%d]=%d\n",
					i,
					pRes->Scaling[i]
				);
			}

		}
	}

	//////////////////////////////////////////////////////////
	// �o�g���r�f�I�f�[�^�̐ݒ�p�����[�^��ݒ�
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_SETPARAM)
	{
		if( _response->Result != POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_SETPARAM_SUCCESS )
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_SETPARAM] Error\n");
		}
		else
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_SETPARAM] Success\n");
		}
	}

	//////////////////////////////////////////////////////////
	// �o�g���r�f�I�f�[�^�̋����m�菈��
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EXECDECISION)
	{
		if( _response->Result != POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EXECDECISION_SUCCESS )
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EXECDECISION] Error\n");
		}
		else
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EXECDECISION] Success\n");
		}
	}

	//////////////////////////////////////////////////////////
	// �T�[�o�[�X�e�[�^�X�擾
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_ADMIN_SVR_STATUS)
	{
		if(_response->Result != POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_STATUS_SUCCESS)
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_SVR_STATUS] Error\n");
		}
		else
		{
			POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUS* pRes = (POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUS*)_response->Param;
			POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA StatusData;

			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_SVR_STATUS] Success\n");

			// ���̕\��
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "ServerCount=%d\n", pRes->ServerCount);

			for(i = 0; i < pRes->ServerCount; i++)
			{
				StatusData = pRes->Status[i];

				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "--------------------------\n");

				POKE_NET_DebugPrintf(
					POKE_NET_DEBUGLEVEL_NORMAL,
					"[%d]IP = %d.%d.%d.%d\n",
					i,
					(StatusData.IP >> 24) & 0xFF,
					(StatusData.IP >> 16) & 0xFF,
					(StatusData.IP >> 8) & 0xFF,
					(StatusData.IP >> 0) & 0xFF
				);

				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[%d]Type = %d\n",				i, StatusData.Type				);
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[%d]Status = %d\n",			i, StatusData.Status			);
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[%d]MaxConnectCount = %d\n",	i, StatusData.MaxConnectCount	);
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[%d]ConnectCount = %d\n",		i, StatusData.ConnectCount		);
				POKE_NET_DebugPrintf(
					POKE_NET_DEBUGLEVEL_NORMAL,
					"[%d]LastUpdateTime = %04I64d/%02I64d/%02I64d %02I64d:%02I64d:%02I64d\n",
					i,
					(StatusData.LastUpdateTime >> 40) & 0xFFFF,
					(StatusData.LastUpdateTime >> 32) & 0xFF,
					(StatusData.LastUpdateTime >> 24) & 0xFF,
					(StatusData.LastUpdateTime >> 16) & 0xFF,
					(StatusData.LastUpdateTime >>  8) & 0xFF,
					(StatusData.LastUpdateTime >>  0) & 0xFF 
				);

				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[%d]Request = %d\n",			i, pRes->Status[i].Request			);
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "--------------------------\n");
			}
		}
	}

	//////////////////////////////////////////////////////////
	// �s���`�F�b�N���O:�擾
	//////////////////////////////////////////////////////////
	else if(_response->ReqCode == POKE_NET_GDS_REQCODE_ADMIN_DHOCHECKLOG_GET)
	{
		if(_response->Result != POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DHOCHECKLOG_GET_SUCCESS)
		{
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_DHOCHECKLOG_GET] Error\n");
		}
		else
		{
			POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET* pRes = (POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET*)_response->Param;
			POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_PARAM Param;

			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[RESPONSE:POKE_NET_GDS_REQCODE_ADMIN_DHOCHECKLOG_GET] Success\n");

			// ���̕\��
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "ServerCount=%d\n", pRes->HitCount);

			for(i = 0; i < pRes->HitCount; i++)
			{
				Param = pRes->List[i];

				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "----------------------------\n");
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[%d].time = %s\n",	i, Param.Time	);
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "[%d].Content = %s\n", i, Param.Content);
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "----------------------------\n");
			}
		}
	}
#endif // ___POKE_NET_BUILD_WINDOWS___


	// �t�b�^�[�o��
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"###### <<< POKE_NET_GDS_DEBUG_PrintResponse ######\n\n");


}
