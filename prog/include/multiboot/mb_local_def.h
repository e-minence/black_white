//======================================================================
/**
 * @file	mb_local_def.h
 * @brief	�}���`�u�[�g ���ʒ�`
 * @author	ariizumi
 * @data	09/11/13
 *
 * ���W���[�����FMB_DEF
 */
//======================================================================
#pragma once

#include "pm_version.h"

#if 1
#define MB_TPrintf(...) (void)((OS_TPrintf(__VA_ARGS__)))
#define MB_Printf(...)  (void)((OS_Printf(__VA_ARGS__)))
#else
#define MB_TPrintf(...) ((void)0)
#define MB_Printf(...)  ((void)0)
#endif //DEB_ARI

#define MB_DEF_GGID (SYACHI_NETWORK_GGID)

//�e�q�ʒ�`
#ifndef MULTI_BOOT_MAKE

#include "font/font.naix"
#include "message.naix"
#include "msg/msg_multiboot_parent.h"
#include "msg/msg_multiboot_child.h"
#define ARCID_FONT_MB (ARCID_FONT)
#define ARCID_MESSAGE_MB (ARCID_MESSAGE)
#define FILE_MSGID_MB (NARC_message_multiboot_parent_dat)
#define FILE_FONT_LARGE_MB (NARC_font_large_gftr)
#define FILE_FONT_PLT_MB (NARC_font_default_nclr)
#define MSGID_YES_MB (MSG_MB_PAERNT_SYS_01)
#define MSGID_NO_MB (MSG_MB_PAERNT_SYS_02)

#else

#ifdef MB_TYPE_POKE_SHIFTER
//�|�P�V�t�^�[
#include "font/font_dl.naix"
#include "message_dl.naix"
#include "msg/msg_multiboot_child.h"
#define ARCID_FONT_MB (ARCID_FONT)
#define ARCID_MESSAGE_MB (ARCID_MESSAGE)
#define FILE_MSGID_MB (NARC_message_dl_multiboot_child_dat)
#define FILE_FONT_LARGE_MB (NARC_font_dl_large_gftr)
#define FILE_FONT_PLT_MB (NARC_font_dl_default_nclr)
#define MSGID_YES_MB (MSG_MB_CHILD_SYS_01)
#define MSGID_NO_MB (MSG_MB_CHILD_SYS_02)

#else   // MB_TYPE_POKE_SHIFTER
//�f��z�M
#include "font/font_dl.naix"
#include "message_dl_movie.naix"
#include "msg/msg_mb_child_movie.h"
#define ARCID_FONT_MB (ARCID_FONT)
#define ARCID_MESSAGE_MB (ARCID_MESSAGE)
#define FILE_MSGID_MB (NARC_message_dl_movie_mb_child_movie_dat)
#define FILE_FONT_LARGE_MB (NARC_font_dl_large_gftr)
#define FILE_FONT_PLT_MB (NARC_font_dl_default_nclr)
#define MSGID_YES_MB (MSG_MB_MOVIE_SYS_01)
#define MSGID_NO_MB (MSG_MB_MOVIE_SYS_02)

#endif  // MB_TYPE_POKE_SHIFTER

#endif //MULTI_BOOT_MAKE

//���ʒ�`
//�ߊl�|�P��
#define MB_CAP_POKE_NUM (6)

#define MB_POKE_BOX_TRAY (18)
#define MB_POKE_BOX_POKE (30)

