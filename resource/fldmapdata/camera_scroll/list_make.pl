#######################################################
#
#
#
#   ""�ň͂�ꂽ���̂Ƃ����łȂ����̂𔻕ʂ��Ȃ���A���X�g���쐬
#
#
#
#######################################################

if( @ARGV == 0 )
{
  print( "perl list_make.pl list.txt" );
  exit(1);
}



open( FILEIN, $ARGV[0] );
@file = <FILEIN>;
close( FILEIN );



open( FILEOUT, ">".$ARGV[0] );

foreach $one ( @file )
{
  $one =~ s/\r\n//g;
  $one =~ s/\n//g;
  
  if( $one =~ /"/ )
  {
    print( FILEOUT $one."\n" );
  }
  else
  {
    print( FILEOUT "\"$one\"\n" );
  }
}


close( FILEOUT );

exit(0);
