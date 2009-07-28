//=============================================================================================
/**
 * @file  btl_util.c
 * @brief �|�P����WB �o�g���V�X�e�� ���L�c�[���Q
 * @author  taya
 *
 * @date  2008.09.25  �쐬
 */
//=============================================================================================

#include "btl_common.h"
#include "btl_util.h"

#ifdef BTL_PRINT_SYSTEM_ENABLE

static BtlPrintType _PrintType;

static void print_type( void )
{
  switch( _PrintType ){
  case BTL_PRINTTYPE_SERVER:      OS_TPrintf("SV/"); break;
  case BTL_PRINTTYPE_CLIENT:      OS_TPrintf("CL/"); break;
  case BTL_PRINTTYPE_STANDALONE:  OS_TPrintf("SA/"); break;
  case BTL_PRINTTYPE_UNKNOWN:
  default:
    OS_TPrintf("UN/");
    break;
  }
}

static void print_file_info( const char* filename, int line )
{
  static const struct {
    char* longName;
    char* shortName;
  }names[] = {
    { "btl_main.c",       "MAI" },
    { "btl_server.c",     "SVR" },
    { "btl_client.c",     "CLI" },
    { "btl_adapter.c",    "ADP" },
    { "btl_string.c",     "STR" },
    { "btl_net.c",        "NET" },
    { "btl_event.c",      "EVE" },
    { "btl_pokeparam.c",  "BPP" },
    { "btl_server_cmd.c", "CMD" },
    { "btl_server_flow.c","FLW" },
    { "btl_field.c",      "FLD" },
    { "btlv_core.c",      "VIW" },
    { "btlv_scu.c",       "scU" },
    { "btlv_scd.c",       "scD" },
    { NULL,               "OTR" },
  };
  u32 i;

  for(i=0; names[i].longName!=NULL; ++i)
  {
    if( !strcmp(names[i].longName, filename) )
    {
      break;
    }
  }
  OS_TPrintf( "[%s-%4d]", names[i].shortName, line);
}

void BTL_UTIL_SetPrintType( BtlPrintType type )
{
  _PrintType = type;
}

void BTL_UTIL_Printf( const char* filename, int line, const char* fmt, ... )
{
  /*
  print_type();
  print_file_info( filename, line );

  {
    va_list vlist;
    va_start( vlist, fmt );
    OS_TVPrintf( fmt, vlist );
    va_end( vlist );
  }
  */
}

void BTL_UTIL_DumpPrintf( const char* caption, const void* data, u32 size )
{
  OS_TPrintf( "[DUMP] %s (%dbytes)\n", caption, size );

  if( size > 8 ){ size = 8; }

  {
    const u8* p = data;
    u32 i;
    for(i=0; i<size; i++)
    {
      OS_TPrintf("%02x ", p[i]);
    }
    OS_TPrintf("\n");
  }
}

#endif  /* #ifdef BTL_PRINT_SYSTEM_ENABLE */


