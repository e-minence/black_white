################################################################
#
#
#   WFBC  �J�������R���o�[�g
#
#   wfbc_camera_conv.pl excel_tab�@output
#
#
#
################################################################


#�����@�`�F�b�N
if( @ARGV < 2 )
{
  print( "wfbc_camera_conv.pl excel_tab output\n" );
  exit(1);
}



#�G�N�Z���^�u
open( FILEIN, $ARGV[0] );

@EXCEL_FILE = <FILEIN>;

close( FILEIN );

#�p�����[�^
$PARA_PEOPLE_NUM  = 0;
$PARA_CAMERA_NO   = 1;
$PARA_MAX   = 2;


#���̎擾
@CAMERA_DATA = undef;
$CAMERA_DATA_MAX = 0;
$data_in = 0;
foreach $one ( @EXCEL_FILE )
{
  $one =~ s/\r\n//g;
  $one =~ s/\n//g;
  
  @line= split( /\t/, $one );
  
  if( $data_in == 0 )
  {
    if( "".$line[0] eq "PeopleNum" )
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
        $CAMERA_DATA[ ($CAMERA_DATA_MAX*$PARA_MAX) + $i ] = $line[$i];
      }
      $CAMERA_DATA_MAX++;
    }
  }
}




#�o��
open( FILEOUT, ">".$ARGV[1] );
binmode( FILEOUT );
for( $i=0; $i<$CAMERA_DATA_MAX; $i++ )
{
  print( FILEOUT pack( "S", $CAMERA_DATA[ ($i * $PARA_MAX) + $PARA_CAMERA_NO ] ) ); 
}
close( FILEOUT );

exit(0);
