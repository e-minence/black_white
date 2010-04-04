//======================================================================
/**
 * @file	scrcmd_pokemon.h
 * @brief	�X�N���v�g�R�}���h�F�|�P�����֘A
 * @date  2009.09.15
 * @author	tamada GAMEFREAK inc.
 *
 */
//======================================================================
#pragma once

#include "poke_tool/poke_tool.h"
//======================================================================
//  �c�[���֐�
//======================================================================
extern BOOL SCRCMD_GetTemotiPP( SCRCMD_WORK * work, u16 pos, POKEMON_PARAM ** poke_para );
extern u32 SCRCMD_GetTemotiPPValue( SCRCMD_WORK * work, u16 pos, int param_id );
extern BOOL SCRCMD_SetTemotiPPValue( SCRCMD_WORK *work, u16 pos, int param_id, u32 value );
extern BOOL SCRCMD_CheckTemotiWaza( const POKEMON_PARAM * pp, u16 wazano );


//======================================================================
//      �R�}���h
//======================================================================
extern VMCMD_RESULT EvCmdGetPartyPokeType( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdGetParamEXP( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdPokemonRecover( VMHANDLE * core, void *wk );

extern VMCMD_RESULT EvCmdCheckPokemonHP( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdCheckPokemonEgg( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdGetPokemonFriendValue( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdSetPokemonFriendValue( VMHANDLE * core, void *wk );

extern VMCMD_RESULT EvCmdGetPartyFrontPoke( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdGetPartyPokeCount( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdGetPartyPokeCountByMonsNo( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdGetBoxPokeCount( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdGetPartyPosByMonsNo( VMHANDLE * core, void *wk );

extern VMCMD_RESULT EvCmdGetPartyPokeMonsNo( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdGetPartyPokeFormNo( VMHANDLE * core, void *wk );

extern VMCMD_RESULT EvCmdChkPokeWaza( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdChkPokeWazaGroup( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdAddPokemonToParty( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdAddPokemonToPartyEx( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdAddPokemonToBox( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdAddPokemonToBoxEx( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdAddTamagoToParty( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdGetPokemonWazaNum( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdGetPokemonWazaID( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdSetPokemonWaza( VMHANDLE *core, void *wk );


extern VMCMD_RESULT EvCmdCheckPokeOwner( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdChgFormNo( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdChgRotomFormNo( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdCheckRemaindWaza( VMHANDLE* core, void* wk );

extern VMCMD_RESULT EvCmdCheckPartyPokeGetPlace( VMHANDLE* core, void* wk );
extern VMCMD_RESULT EvCmdGetPartyPokeGetDate( VMHANDLE* core, void* wk );

extern VMCMD_RESULT EvCmdGetPartyPokeParameter( VMHANDLE* core, void* wk );
extern VMCMD_RESULT EvCmdSetPartyPokeParameter( VMHANDLE* core, void* wk );

