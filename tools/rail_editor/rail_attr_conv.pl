##-----------------------------------------------------------------------------
#	.rail�t�@�C����Binary���ɃR���o�[�g
#
#
#	rail_attr_conv.pl	.attr
#
#
#
##-----------------------------------------------------------------------------

##-----------------------------------------------------------------------------
#�@�萔
##-----------------------------------------------------------------------------
$DEF_RAIL_ATTR_SEQ_ALLDATA = 0;
$DEF_RAIL_ATTR_SEQ_SEARCHLINE = 1;
$DEF_RAIL_ATTR_SEQ_LINEX = 2;
$DEF_RAIL_ATTR_SEQ_LINEZ = 3;
$DEF_RAIL_ATTR_SEQ_ATTR = 4;

##-----------------------------------------------------------------------------
#�@�f�[�^
##-----------------------------------------------------------------------------
# �ǂݍ��ݏ��
@RAIL_ATTR_DATA = undef;

##-----------------------------------------------------------------------------
#�@���C�����[�`��
##-----------------------------------------------------------------------------

#�����`�F�b�N
if( @ARGV < 1 )
{
	print( "rail_attr_conv.pl	.attr \n" );
	exit(1);
}


#���̓ǂݍ���
open( FILEIN, $ARGV[0] );
@RAIL_ATTR_DATA = <FILEIN>;
close( FILEIN );

&output( \@RAIL_ATTR_DATA, $ARGV[0] );

exit(0);


#�o��
sub output
{
  my( $data, $name ) = @_;
  my( $one, $seq, $linenum, $read_line, @attr );
  
  #�g���q���������
  $name =~ s/\..*//;

	open( FILEOUT, ">".$name.".atdat" );
  binmode( FILEOUT );
  
  #�e���L��
  $seq = $DEF_RAIL_ATTR_SEQ_ALLDATA;
  $linenum = 0;
  $read_line = 0;
  #���̎擾�{�o��
  foreach $one ( @$data )
  {
    #���C�����̎擾
    if( $seq == $DEF_RAIL_ATTR_SEQ_ALLDATA )
    {
      #���C�������o�͂���
      if( $one =~ /--LINENUM::([0-9]*)/ )
      {
        $linenum = $1;

        print( "linenum = $linenum\n" );
        print( FILEOUT pack( "I", $linenum ) );

        #LINE�f�[�^SEARCH
        $seq = $DEF_RAIL_ATTR_SEQ_SEARCHLINE;
      }
    }
    elsif( $seq == $DEF_RAIL_ATTR_SEQ_SEARCHLINE )
    {
      if( $one =~ /--NAME::(.*)/ )
      {
        $read_line++;
        print( "out linename = $1\n" );
        $seq = $DEF_RAIL_ATTR_SEQ_LINEX;
      }
    }
    elsif( $seq == $DEF_RAIL_ATTR_SEQ_LINEX )
    {
      if( $one =~ /--X::([0-9]*)/ )
      {
        print( "x=$1\n" );
        print( FILEOUT pack( "S", $1 ) );
        $seq = $DEF_RAIL_ATTR_SEQ_LINEZ;
      }
    }
    elsif( $seq == $DEF_RAIL_ATTR_SEQ_LINEZ )
    {
      if( $one =~ /--Z::([0-9]*)/ )
      {
        print( "z=$1\n" );
        print( FILEOUT pack( "S", $1 ) );
        $seq = $DEF_RAIL_ATTR_SEQ_ATTR;
      }
    }
    elsif( $seq == $DEF_RAIL_ATTR_SEQ_ATTR )
    {
      #�I���`�F�b�N
      if( $one =~ /\+LINE/ )
      {
        $seq = $DEF_RAIL_ATTR_SEQ_SEARCHLINE;
      }
      elsif( $one =~ /--LINEATTR::(.*)/ )
      {
        @attr = split( /\s/, $1 );
        print( "flag = ".$attr[1]." data = ".$attr[2]."\n" );
        print( FILEOUT pack( "I", ($attr[1]<<16) | $attr[2] ) );
      }
    }
  }



  close( FILEOUT );

  if(  $read_line != $linenum )
  {
    print( "ERR ALL LineNum = $linenum  Read LineNum = $read_line\n" );
    exit(1);
  }
}
