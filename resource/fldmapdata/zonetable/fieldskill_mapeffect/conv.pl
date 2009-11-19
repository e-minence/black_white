#################################################
#
#
#
#   �t�B�[���h�Z�@���ʁ@�R���o�[�g
#
# conv.pl excel_csv zoneid_header output
#
#################################################


if( @ARGV < 3 )
{
  print( "conv.pl excel_csv zoneid_header output\n" );
  exit(1);
}

###################################
#
# �}�b�v�G�t�F�N�g��ԃ}�X�N
#
################################### 
$FIELDSKILL_MAPEFF_MSK_FLASH_NEAR   = 1 << 0;    #// �t���b�V�����ʁ@NEAR���
$FIELDSKILL_MAPEFF_MSK_FLASH_FAR    = 1 << 1;    #// �t���b�V�����ʁ@FAR���


  

#Excel�ǂݍ���
open( FILEIN, $ARGV[0] );
@EXCEL = <FILEIN>;
close( FILEIN );

#ZoneID�ǂݍ���
open( FILEIN, $ARGV[1] );
@ZONID_DATA = <FILEIN>;
close( FILEIN );


$input = 0;
$count = 0;
$msk = 0;

@ZONEID_TBL = undef;
@EFFECT_MSK = undef;

#���̎擾
foreach $one ( @EXCEL )
{
  @data = split( /,/, $one );
  
  if( $input == 0 )
  {
    if( $data[0] =~ /#START/ )
    {
      $input = 1;
    }
  }
  elsif( $input == 1 )
  {
    if( $data[0] =~ /#END/ )
    {
      $input = 0;
    }
    else
    {
      $ZONEID_TBL[ $count ] = &GetZoneID( $data[0] );
      
      #�}�X�N���쐬
      $msk = 0;

      #�t���b�V��
      if( $data[1] =~ /��/ )
      {
        $msk |= $FIELDSKILL_MAPEFF_MSK_FLASH_NEAR;
      }
      
      $EFFECT_MSK[ $count ] = $msk;
      $count ++;
    }
  }
}


#�o��
open( FILEOUT, ">".$ARGV[2] );
binmode( FILEOUT );

for( $i=0; $i<$count; $i++ )
{
  print( FILEOUT pack( "S", $ZONEID_TBL[ $i ] ) );
  print( FILEOUT pack( "S", $EFFECT_MSK[ $i ] ) );
}

close( FILEOUT );


exit(0);


sub GetZoneID
{
  my( $name ) = @_;
  my( $one );
  
  foreach $one ( @ZONID_DATA )
  {
    #�X�y�[�X�J�b�g
    $one =~ s/\s//g;
    
    if( $one =~ /#define*$name\(([0-9]*)\)/ )
    {
      return $1;
    }
  }

  print( "ZoneId���݂���܂���B $name\n" );
  exit(1);
}

