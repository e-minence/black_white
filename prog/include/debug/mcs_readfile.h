//============================================================================================
/**
 * @file  msc_readfile.h
 * @brief
 * @date  2010.01.18
 */
//============================================================================================

#pragma once

//----------------------------------------------------------------
//----------------------------------------------------------------
extern void GF_MCS_FILE_Init(void);

//----------------------------------------------------------------
//----------------------------------------------------------------
extern BOOL GF_MCS_FILE_Read( const char * path, void * buf, u32 buf_size );

