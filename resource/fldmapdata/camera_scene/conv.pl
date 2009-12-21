#################################################
#
# conv.pl tab_file output
#   �G�N�Z�����e��tools/rail_editor/scenearea_conv.pl
#   �ŃR���o�[�g�ł���`���ɂ��ďo��
#
#
#################################################

if( @ARGV < 2 )
{
  print( "conv.pl tab_file output\n" );
  exit(1);
}


#�p�����[�^��`
$PARA_TAG = 0;
$PARA_AREA_GRID_X = 1;
$PARA_AREA_GRID_Z = 2;
$PARA_AREA_GRID_SIZX = 3;
$PARA_AREA_GRID_SIZZ = 4;
$PARA_AREA_GRID_DEPTH = 5;
$PARA_AREA_GRID_IN_PITCH =6;
$PARA_AREA_GRID_IN_YAW = 7;
$PARA_AREA_GRID_IN_LEN = 8;
$PARA_AREA_GRID_OUT_PITCH = 9;
$PARA_AREA_GRID_OUT_YAW = 10;
$PARA_AREA_GRID_OUT_LEN = 11;
$PARA_AREA_MAX = 12;


#�G�N�Z�����擾
@EXCEL_FILE = undef;
open( FILEIN, $ARGV[0] );
@EXCEL_FILE = <FILEIN>;
close( FILEIN );


#�A���O���`�F���W�@�V�[���@�̏����擾
@GRID_ANGLE_CHANGE = undef;
$GRID_ANGLE_CHANGE_MAX  = 0;
$data_in = 0;
foreach $one ( @EXCEL_FILE )
{
  $one =~ s/\r\n//g;
  $one =~ s/\n//g;

  @line = split( /\t/, $one );

  if( $data_in == 0 )
  {
    if( "".$line[0] eq "TAG" )
    {
      $data_in = 1;
    }
  }
  else
  {
    if( "".$line[0] eq "#END" )
    {
      $data_in = 0;
    }
    else
    {
      #������
      for( $i=0; $i<$PARA_AREA_MAX; $i++ )
      {
        $GRID_ANGLE_CHANGE[ ($GRID_ANGLE_CHANGE_MAX * $PARA_AREA_MAX) + $i ] = $line[$i];
      }
      $GRID_ANGLE_CHANGE_MAX ++;
    }
  }
}



#�擾�����g���ď����o��
open( FILEOUT, ">".$ARGV[1] );
  
for( $i=0; $i<$GRID_ANGLE_CHANGE_MAX; $i++ )
{
  $index = $i * $PARA_AREA_MAX;
  $output = 0;
  
  print( FILEOUT "+SA\n" );
  print( FILEOUT "--NAME::".$GRID_ANGLE_CHANGE[ $index+$PARA_TAG ]."\n" );
  print( FILEOUT "--AREACHECK::FLD_SCENEAREA_AREACHECK_GRID_RECT\n" );
  print( FILEOUT "--UPDATE::FLD_SCENEAREA_UPDATE_ANGLECHANGE\n" );
  print( FILEOUT "--INSIDE::FLD_SCENEAREA_UPDATE_ANGLECHANGE_IN\n" );
  print( FILEOUT "--OUTSIDE::FLD_SCENEAREA_UPDATE_ANGLECHANGE_OUT\n" );
  print( FILEOUT "--2BYTE::".$GRID_ANGLE_CHANGE[ $index+$PARA_AREA_GRID_X ]."\n" );
  print( FILEOUT "--2BYTE::".$GRID_ANGLE_CHANGE[ $index+$PARA_AREA_GRID_Z ]."\n" );
  print( FILEOUT "--2BYTE::".$GRID_ANGLE_CHANGE[ $index+$PARA_AREA_GRID_SIZX ]."\n" );
  print( FILEOUT "--2BYTE::".$GRID_ANGLE_CHANGE[ $index+$PARA_AREA_GRID_SIZZ ]."\n" );
  print( FILEOUT "--2BYTE::".$GRID_ANGLE_CHANGE[ $index+$PARA_AREA_GRID_DEPTH ]."\n" );
  print( FILEOUT "--2BYTE::0\n" );  #�p�f�B���O
  $output = hex($GRID_ANGLE_CHANGE[ $index+$PARA_AREA_GRID_IN_PITCH ]);
  print( FILEOUT "--2BYTE::".$output."\n" );
  $output = hex($GRID_ANGLE_CHANGE[ $index+$PARA_AREA_GRID_IN_YAW ]);
  print( FILEOUT "--2BYTE::".$output."\n" );
  $output = hex($GRID_ANGLE_CHANGE[ $index+$PARA_AREA_GRID_IN_LEN ]);
  print( FILEOUT "--4BYTE::".$output."\n" );
  $output = hex($GRID_ANGLE_CHANGE[ $index+$PARA_AREA_GRID_OUT_PITCH ]);
  print( FILEOUT "--2BYTE::".$output."\n" );
  $output = hex($GRID_ANGLE_CHANGE[ $index+$PARA_AREA_GRID_OUT_YAW ]);
  print( FILEOUT "--2BYTE::".$output."\n" );
  $output = hex($GRID_ANGLE_CHANGE[ $index+$PARA_AREA_GRID_OUT_LEN ]);
  print( FILEOUT "--4BYTE::".$output."\n" );
}


close( FILEOUT );


