#################################################
#
#   �ˑ��`�F�b�N�@�X�N���v�g
#
# �ˑ��ݒ�
#   xls mev1 mev2 mev3 mev4
#
# check_mev.pl check_file
#
#################################################
if( @ARGV < 1 )
{
  print( "check_mev.pl check_file\n" );
  exit(1);
}

#�t�@�C�������邩�`�F�b�N
if( -e $ARGV[0] )
{
}
else
{
  exit(0);
}

#�t�@�C���ǂݍ���
open( FILEIN, $ARGV[0] );
@CHECK_LIST_FILE = <FILEIN>;
close( FILEIN );



#�ˑ��`�F�b�N
foreach $one ( @CHECK_LIST_FILE )
{
  $one =~ s/\r\n//g;
  $one =~ s/\n//g;
  @list = split( " ", $one );

  $touch_file = 0;

  #�t�@�C���X�V���ԃ`�F�b�N
  if( -e $list[0] )
  {
    #�����t�@�C�����Ȃ� or �����t�@�C�����̎��Ԃ��ς���Ă��鎞�@$list[0]���X�V

    for( $i=1; $i<@list; $i++ )
    {
      #print( $list[$i]." check\n" );
      if( -e $list[$i] )
      {
            
        #�ŏI�X�V���ԃ`�F�b�N
        open( FILEIN, $list[$i] );
        foreach $mev ( <FILEIN> )
        {
          if( $mev =~ /#save date/ )
          {
            if( "".$mev ne "#save date:2000/00/00 00:00:00\r\n" )
            {
              #���t���ς�����I
              $touch_file = 1;
            }
          }
        }
        close( FILEIN );
      }
      else
      {
        #�Ȃ��̂ŁA�^�b�`
        $touch_file = 1;
      }
    }
  }
  else
  {
    print( "check file ".$list[0]." not found\n" );
  }

  if( $touch_file )
  {
    $cmd = "touch ".$list[0];
    system( $cmd );
  }
}


exit(0);





