//======================================================================
/**
 * @file	scrcmd_pokemon_fld.h
 * @brief	�X�N���v�g�R�}���h�F�|�P�����֘A�F�t�B�[���h�ł����g�p���Ȃ�����
 * @date  2010.01.23
 * @author	tamada GAMEFREAK inc.
 *
 * �T�u�C�x���g�Ō���g�p�������̂ȂǁA�펞�Ăяo���Ȃ��Ă����v�Ȃ��̂�
 * scrcmd_pokemon.c�łȂ�������ɔz�u����
 */
//======================================================================
#pragma once

//======================================================================
//  extern 
//======================================================================

extern VMCMD_RESULT EvCmdSetFavoritePoke( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdSetGymVictoryInfo( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdGetGymVictoryInfo( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdGetBreederJudgeResult( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdGetZukanHyouka( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdGetZukanComplete( VMHANDLE * core, void *wk );

extern VMCMD_RESULT EvCmdSkillTeachCheckParty( VMHANDLE* core, void* wk );
extern VMCMD_RESULT EvCmdSkillTeachCheckPokemon( VMHANDLE* core, void* wk );
extern VMCMD_RESULT EvCmdSkillTeachSelectPokemon( VMHANDLE * core, void * wk );
extern VMCMD_RESULT EvCmdSkillTeachGetWazaID( VMHANDLE * core, void * wk );

extern VMCMD_RESULT EvCmdCheckTemotiPokerus( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdGetMezameruPowerType( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdPartyPokeNameInput( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdPartyPokeWazaSelect( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdPartyPokeEggBirth( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdFieldPokeTradeCheck( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdFieldPokeTrade( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdWazaMachineLot( VMHANDLE * core, void * wk );
