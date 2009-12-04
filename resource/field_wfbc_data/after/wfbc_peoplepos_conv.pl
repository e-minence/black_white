################################################################
#
#
#   WFBC  �l���̈ʒu���R���o�[�g
#
#   wfbc_peoplepos_conv.pl excel_tab�@output
#
#
#
################################################################


#�����@�`�F�b�N
if( @ARGV < 2 )
{
  print( "wfbc_people_conv.pl excel_tab output\n" );
  exit(1);
}



#�G�N�Z���^�u
open( FILEIN, $ARGV[0] );

@EXCEL_FILE = <FILEIN>;

close( FILEIN );

#�p�����[�^
$PARA_DUMY  = 0;
$PARA_00    = 1;
$PARA_63    = 64;
$PARA_MAX   = 65;


#���̎擾
@POS_DATA = undef;
$POS_DATA_MAX = 0;
$data_in = 0;
foreach $one ( @EXCEL_FILE )
{
  $one =~ s/\r\n//g;
  $one =~ s/\n//g;
  
  @line= split( /\t/, $one );
  
  if( $data_in == 0 )
  {
    if( "".$line[0] eq "#START" )
    {
      $data_in = 1;
    }
  }
  else
  {
    if( "".$line[0] eq "#END" )
    {
      $data_in  = 0;
    }
    else
    {

      for( $i=0; $i<$PARA_MAX; $i++ )
      {
        $POS_DATA[ ($POS_DATA_MAX*$PARA_MAX) + $i ] = $line[$i];
      }
      $POS_DATA_MAX++;
    }
  }
}


#0�`19�̈ʒu���`�F�b�N
#X0,Z0, X1,Z1, ...
@PEOPLE_POS = (0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0 );

$PEOPLE_NUM = 20;

$PEOPLE_POS_X_IDX = 0;
$PEOPLE_POS_Z_IDX = 1;
$PEOPLE_POS_IDX_NUM = 2;

for( $i=0; $i<$POS_DATA_MAX; $i++ )
{
  for( $j=$PARA_00; $j<$PARA_MAX; $j++ )
  {
    $index = ($i * $PARA_MAX) + $j;
    
    #�������`�F�b�N
    if( $POS_DATA[ $index ] =~ /[0-9]/ )
    {
      $input =$POS_DATA[ $index ];

      $PEOPLE_POS[ ($input * $PEOPLE_POS_IDX_NUM) + $PEOPLE_POS_X_IDX ] = $j-1;
      $PEOPLE_POS[ ($input * $PEOPLE_POS_IDX_NUM) + $PEOPLE_POS_Z_IDX ] = $i;
    }
  }
}



#�o��
open( FILEOUT, ">".$ARGV[1] );
binmode( FILEOUT );
for( $i=0; $i<$PEOPLE_NUM; $i++ )
{
  #print( "people $i x=".$PEOPLE_POS[ ($i * $PEOPLE_POS_IDX_NUM) + $PEOPLE_POS_X_IDX ]." z=".$PEOPLE_POS[ ($i * $PEOPLE_POS_IDX_NUM) + $PEOPLE_POS_Z_IDX ]."\n" );
  
  print( FILEOUT pack( "C", $PEOPLE_POS[ ($i * $PEOPLE_POS_IDX_NUM) + $PEOPLE_POS_X_IDX ] ) ); 
  print( FILEOUT pack( "C", $PEOPLE_POS[ ($i * $PEOPLE_POS_IDX_NUM) + $PEOPLE_POS_Z_IDX ] ) ); 
}
close( FILEOUT );

exit(0);
