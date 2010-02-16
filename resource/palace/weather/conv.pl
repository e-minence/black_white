#####################################################################
#
# �p���X�@�V�C�iFOG�j���f���B�t���O�@�f�[�^�o��
#     conv.pl excel_tab output_cdat output_header
#
#
#
#
#####################################################################


if( @ARGV < 2 ){
  print( "conv.pl excel_tab output_cdat \n" );
  exit(1);
}


#�ǂݍ���
open( FILEIN, $ARGV[0] );
@EXCEL_FILE = <FILEIN>;
close( FILEIN );


#�f�[�^
@DATA_ARRAY = undef;
$DATA_NUM = 0;

$DATA_IDX_ZONE_ID = 0;
$DATA_IDX_CHECK_TOWN_ID = 1;
$DATA_IDX_MAX = 2;

$data_in = 0;
$data_index = 0;


#���
foreach $one ( @EXCEL_FILE )
{
  @line = split( /\t/, $one );

  if( $data_in == 0 )
  {
    if( "".$line[0] eq "#start" )
    {
      $data_in = 1;
      $data_index = 0;
    }
  }
  else
  {
    if( "".$line[0] eq "#end" )
    {
      $data_in = 0;
    }
    elsif( $data_index > 1 ){
      
      $DATA_ARRAY[ $DATA_NUM * $DATA_IDX_MAX ] = $line[0];
      $DATA_ARRAY[ ($DATA_NUM * $DATA_IDX_MAX) + 1 ] = $line[1];
      $DATA_NUM ++;
    }

    $data_index ++;
  }
}

#�o��
open( FILEOUT, ">".$ARGV[1] );

print( FILEOUT "// ���̃t�@�C����resource/palace/weather/conv.pl�Ő�������Ă��܂��B\n" );
print( FILEOUT "// �p���X��̃]�[���ɖ��̓V�C�������邩���`�F�b�N���铞���t���O�e�[�u���ł��B\n" );
print( FILEOUT "#include \"arc/fieldmap/zone_id.h\"\n" );
print( FILEOUT "#include \"../../../resource/fldmapdata/flagwork/flag_define.h\"\n\n" );

print( FILEOUT "typedef struct {\n" );
print( FILEOUT "  u16 zone_id;\n" );
print( FILEOUT "  u16 sys_flag;\n" );
print( FILEOUT "} PALACE_WEATHER_DATA;\n\n" );

print( FILEOUT "static const PALACE_WEATHER_DATA sc_PALACE_WEATHER_DATA[ $DATA_NUM ] = {\n" );


for( $i=0; $i<$DATA_NUM; $i++ )
{
  print( FILEOUT "  { ZONE_ID_".$DATA_ARRAY[ ($i * $DATA_IDX_MAX) ].", SYS_FLAG_ARRIVE_".$DATA_ARRAY[ ($i * $DATA_IDX_MAX) + 1 ]." },\n" );
  
}

print( FILEOUT "};\n\n" );

close( FILEOUT );

exit(0);
