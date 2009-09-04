###############################################################################
# �f�B���N�g����SEARCH
#   �t�@�C����comment���o�X�N���v�g
#
#
#     comment_salvage.pl [�^�C�v...]
#       
#         [�^�C�v]
#           -f  �֐��w�b�_
#           -F  �t�@�C���w�b�_
#           ���ݒ�@�S��
#
#
#
#
#
###############################################################################

$FUNC_COMMENT_FALG = 0;
$FILE_COMMENT_FLAG = 0;


#�w���v�\��
if( $ARGV[0] eq "-h" )
{
  print( "comment_salvage.pl [�^�C�v...]\n" ); 
  print( "[�^�C�v]\n" );
  print( "-f  �֐��w�b�_\n" );
  print( "-F  �t�@�C���w�b�_\n" );
  print( "���ݒ�  �S��\n" );
  exit(0);
}

#�����`�F�b�N
$i = 0;
for( $i=0; $i<@ARGV; $i++ )
{
  if( $ARGV[$i] eq "-f" )
  {
    $FUNC_COMMENT_FALG = 1;
  }

  if( $ARGV[$i] eq "-F" )
  {
    $FILE_COMMENT_FALG = 1;
  }
}

if( @ARGV == 0 )
{
  $FUNC_COMMENT_FALG = 1;
  $FILE_COMMENT_FALG = 1;
}


#���C�����s
call &main();


exit(0);

sub main {
  
  $root_dir = '.';

  system ("find $root_dir|grep c\$ > tmp.dat\n");
  if( open( FILE, "<tmp.dat") )
  {
    my @files = <FILE>;
    close(FILE);
    system "rm tmp.dat";
    my $cmd;

    foreach my $filename (@files)
    {
      $filename =~ s/\r\n//g;

      &output( $filename );
    }
  }



  system ("find $root_dir|grep ev\$ > tmp.dat\n");
  if( open( FILE, "<tmp.dat") )
  {
    my @files = <FILE>;
    close(FILE);
    system "rm tmp.dat";
    my $cmd;

    foreach my $filename (@files)
    {
      $filename =~ s/\r\n//g;

      &output( $filename );
    }
  }
}








sub output
{
  my( $output_file, $first, $comment_on ) = @_;

  #
  # ����
  #
  open( FILEIN, $output_file );
  @INPUT_FILE = <FILEIN>;
  close( FILEIN );


  #�t�@�C�����̏o��
  print( $output_file." output data\n" );

  $first = 0;
  $comment_on = 0;
  
  #
  foreach $one ( @INPUT_FILE )
  {
    #�擪comment�����ׂďo��  
    if( ($first == 0) && ($FILE_COMMENT_FALG == 1) )
    {
      if( $one =~ /^[\s\t]+$/ )
      {
      }
      elsif ( $one =~ /^\/\// )
      {
        print $one;
      }
      elsif ( $one =~ /^\/\*/ )
      {
        $comment_on = 1;
        print $one;
      }
      elsif ( $one =~ /\*\// )
      {
        $comment_on = 0;
        print $one;
      }
      elsif( $comment_on == 1 )
      {
        &JapanPrint( $one );
      }
      else
      {
        #�擪�o�͏I��
        $first = 1;
      }
    }
    else
    {


      #�֐�comment�̏o��  
      if( ($FUNC_COMMENT_FALG == 1) )
      {
        if ( $one =~ /^\/\// )
        {
          print $one;
        }
        elsif ( $one =~ /^\/\*/ )
        {
          $comment_on = 1;
          print $one;
        }
        elsif ( $one =~ /\*\// )
        {
          $comment_on = 0;
          print $one;
        }
        elsif( $comment_on == 1 )
        {
          &JapanPrint( $one );
        }
      }

    }
  }


  print( "\n\n" );
}



sub JapanPrint
{
  my( $outdata ) = @_;
 
    
  # $str �� ASCII �ȊO���܂܂�Ă��邩���肷��
  if ($outdata =~ /[\x8E\xA1-\xFE]/) {
    print $outdata;
  }
}

