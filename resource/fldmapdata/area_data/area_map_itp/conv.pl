################################################
#
#	CSV�`���̃��C�g�f�[�^��Binary�ɂ��ďo��
#
#	conv.pl	csvfile
#
################################################


#�������`�F�b�N
if( @ARGV < 2 ){
	print( "need 4 param\n" );
	print( "conv.pl	csvfile outputfile itp_naix tex_naix\n" );
	exit(1);
}

@ITP_DATA = undef;
@TEX_DATA = undef;

#�f�[�^�ő吔
$ITP_DATA_MAX = 16;

#�f�[�^���Ȃ��ꍇ
$ITP_DATA_NONE = 0xff;


#�t�@�C���ǂݍ���
open( FILE, $ARGV[0] );
@CSVFILE = <FILE>;
close( FILE );

open( FILE, $ARGV[2] );
@ITP_NAIX_FILE = <FILE>;
close( FILE );
open( FILE, $ARGV[3] );
@TEX_NAIX_FILE = <FILE>;
close( FILE );



#�f�[�^�擾
$datacount = 0;
$datain = 0;
$i = 0;
foreach $one ( @CSVFILE ){
  $one =~ s/\r\n//g;
  $one =~ s/\n//g;

	@line = split( /,/, $one );

  if( $datain == 0 )
  {
    if( $line[ 1 ] eq "ITP" ){
      $datain = 1;
    }
  }
  else
  {
    $ITP_DATA[ $datacount ] = $line[1];
    $TEX_DATA[ $datacount ] = $line[2];

    if( ("".$ITP_DATA[ $datacount ] eq "none") || ("".$TEX_DATA[ $datacount ] eq "none") )
    {
      $ITP_DATA[ $datacount ] = $ITP_DATA_NONE;
      $TEX_DATA[ $datacount ] = $ITP_DATA_NONE;
    }
    else
    {
      #.��_�ɕϊ�
      $ITP_DATA[ $datacount ] =~ s/\.itp/\.itpdat/;
      $TEX_DATA[ $datacount ] =~ s/\.imd/\.nsbtx/;

      #naix���琔���ɕϊ�
      #binary���܂Ƃ߂ďo�͂���`�ɕύX
      #$ITP_DATA[ $datacount ] = &getItpNaix( $ITP_DATA[ $datacount ] );
      #$TEX_DATA[ $datacount ] = &getTexNaix( $TEX_DATA[ $datacount ] );
      $datacount ++;
    }
    
  }  
}


#�o��
$offset = 0;
open( FILE, ">".$ARGV[1] );
binmode( FILE );

syswrite( FILE, pack( "I", $datacount ), 4  );
$offset = 4 + 4 + (($datacount*4)*2);

#�S�t�@�C���̃I�t�Z�b�g
for( $i=0; $i<$datacount; $i++ ){
  $filesize = -s $ITP_DATA[ $i ];
  print( "filesize $filesize\n" );
  print( "offset $offset\n" );
	syswrite( FILE, pack( "I", $offset ), 4  );
  $offset += $filesize;

  $filesize = -s $TEX_DATA[ $i ];
  print( "filesize $filesize\n" );
  print( "offset $offset\n" );
	syswrite( FILE, pack( "I", $offset ), 4  );
  $offset += $filesize;
}
syswrite( FILE, pack( "I", $offset ), 4  );

for( $i=0; $i<$datacount; $i++ ){

  $filesize = -s $ITP_DATA[ $i ];
  open( FILEIN, $ITP_DATA[ $i ] );
  binmode( FILEIN );
  read( FILEIN, $data, $filesize );
	syswrite( FILE, $data, $filesize  );
  close( FILEIN );

  $filesize = -s $TEX_DATA[ $i ];
  open( FILEIN, $TEX_DATA[ $i ] );
  binmode( FILEIN );
  read( FILEIN, $data, $filesize );
	syswrite( FILE, $data, $filesize  );
  close( FILEIN );
}

close( FILE );


#����I��
exit(0);



#ITP�̃t�@�C���C���f�b�N�X�擾
sub getItpNaix
{
  my( $itpfile ) = @_;
  my( $one );

  foreach $one ( @ITP_NAIX_FILE ){
    if( $one =~ /$itpfile[^0-9]* = ([0-9]*)/ ){
      return $1;
    }
  }

  print "$itpfile �� �Ȃ��ł�\n";
  exit(1);
}

#TEX�̃t�@�C���C���f�b�N�X�擾
sub getTexNaix
{
  my( $texfile ) = @_;
  my( $one );

  foreach $one ( @TEX_NAIX_FILE ){
    if( $one =~ /$texfile[^0-9]* = ([0-9]*)/ ){
      return $1;
    }
  }

  print "$texfile �� �Ȃ��ł�\n";
  exit(1);
}
