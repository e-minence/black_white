//==============================================================================
/**
 * @file    scrcmd_symbol.h
 * @brief   �V���{���|�P�����F�X�N���v�g�R�}���h
 * @author  matsuda
 * @date    2010.03.16(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern VMCMD_RESULT EvCmdSymbolPokeBattle( VMHANDLE *core, void *wk );
extern VMCMD_RESULT EvCmdSymbolPokeSet( VMHANDLE *core, void *wk );

extern VMCMD_RESULT EvCmdSymbolMapWarp( VMHANDLE * core, void *wk );
extern VMCMD_RESULT EvCmdSymbolMapMovePokemon( VMHANDLE * core, void * wk );
extern VMCMD_RESULT EvCmdSymbolMapGetInfo( VMHANDLE * core, void * wk );
extern VMCMD_RESULT EvCmdSymbolMapSetMonsName( VMHANDLE * core, void * wk );
