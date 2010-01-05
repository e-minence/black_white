###########################################################
#
#
# conv.pl   excel_file output_extension
#
# ���f���J���[�����f�[�^�̏o��
#
###########################################################


if( @ARGV < 1 )
{
  print( "conv.pl excel_file output_extension\n" );
  exit(1);
}


#�t�@�C���ǂݍ���
open( FILEIN, $ARGV[0] );
@EXCEL_FILE = <FILEIN>;
close( FILEIN );

#�e���i�[��
@TAG_TBL = undef;
@NORM_X = undef;
@NORM_Y = undef;
@NORM_Z = undef;
@DEF_R  = undef;
@DEF_G  = undef;
@DEF_B  = undef;
@AMB_R  = undef;
@AMB_G  = undef;
@AMB_B  = undef;
@SPQ_R  = undef;
@SPQ_G  = undef;
@SPQ_B  = undef;
@EMI_R  = undef;
@EMI_G  = undef;
@EMI_B  = undef;

$DATA_IN = 0;
foreach $one ( @EXCEL_FILE )
{
  $one =~ s/\r\n//g;
  $one =~ s/\n//g;

  @line = split( "\t", $one );

  if( $DATA_IN == 0 )
  {
    #�J�n�`�F�b�N
    if( "".$line[0] eq "���x����"  )
    {
      $DATA_IN = 1;
      $TBL_COUNT = 0;
      $DATA_COUNT = 0;
    }
  }
  
  if( $DATA_IN == 1 )
  {
    #�I���`�F�b�N
    if( "".$line[0] eq "#END" )
    {
      $DATA_IN = 0;
    }
    else
    {
      #�f�[�^�i�[
      if( $TBL_COUNT == 0 )
      {

        while( "".$line[$DATA_COUNT + 1] ne "#END" )
        {
          print( "TAG INPUT ".$line[$DATA_COUNT + 1]."\n" ); 
          #�^�O�̎��W
          $TAG_TBL[ $DATA_COUNT ] = $line[$DATA_COUNT + 1];
          $DATA_COUNT ++;
        }
      }
      ## �@���i�[ ##
      elsif( $TBL_COUNT == 1 )
      {
        #�@��X�̊i�[
        for( $i=0; $i<$DATA_COUNT; $i++ )
        {
          $NORM_X[ $i ] = $line[$i + 1];
        }
      }
      elsif( $TBL_COUNT == 2 )
      {
        #�@��Y�̊i�[
        for( $i=0; $i<$DATA_COUNT; $i++ )
        {
          $NORM_Y[ $i ] = $line[$i + 1];
        }
      }
      elsif( $TBL_COUNT == 3 )
      {
        #�@��Z�̊i�[
        for( $i=0; $i<$DATA_COUNT; $i++ )
        {
          $NORM_Z[ $i ] = $line[$i + 1];
        }
      }
      ## �f�B�t���[�Y�i�[ ##
      elsif( $TBL_COUNT == 4 )
      {
        #�f�B�t���[�YR�̊i�[
        for( $i=0; $i<$DATA_COUNT; $i++ )
        {
          $DEF_R[ $i ] = $line[$i + 1];
        }
      }
      elsif( $TBL_COUNT == 5 )
      {
        #�f�B�t���[�YG�̊i�[
        for( $i=0; $i<$DATA_COUNT; $i++ )
        {
          $DEF_G[ $i ] = $line[$i + 1];
        }
      }
      elsif( $TBL_COUNT == 6 )
      {
        #�f�B�t���[�YB�̊i�[
        for( $i=0; $i<$DATA_COUNT; $i++ )
        {
          $DEF_B[ $i ] = $line[$i + 1];
        }
      }
      ## �A���r�G���g�i�[ ##
      elsif( $TBL_COUNT == 7 )
      {
        #�A���r�G���gR�̊i�[
        for( $i=0; $i<$DATA_COUNT; $i++ )
        {
          $AMB_R[ $i ] = $line[$i + 1];
        }
      }
      elsif( $TBL_COUNT == 8 )
      {
        #�A���r�G���gG�̊i�[
        for( $i=0; $i<$DATA_COUNT; $i++ )
        {
          $AMB_G[ $i ] = $line[$i + 1];
        }
      }
      elsif( $TBL_COUNT == 9 )
      {
        #�A���r�G���gB�̊i�[
        for( $i=0; $i<$DATA_COUNT; $i++ )
        {
          $AMB_B[ $i ] = $line[$i + 1];
        }
      }
      ## �X�y�L�����[�i�[ ##
      elsif( $TBL_COUNT == 10 )
      {
        #�X�y�L�����[R�̊i�[
        for( $i=0; $i<$DATA_COUNT; $i++ )
        {
          $SPQ_R[ $i ] = $line[$i + 1];
        }
      }
      elsif( $TBL_COUNT == 11 )
      {
        #�X�y�L�����[G�̊i�[
        for( $i=0; $i<$DATA_COUNT; $i++ )
        {
          $SPQ_G[ $i ] = $line[$i + 1];
        }
      }
      elsif( $TBL_COUNT == 12 )
      {
        #�X�y�L�����[B�̊i�[
        for( $i=0; $i<$DATA_COUNT; $i++ )
        {
          $SPQ_B[ $i ] = $line[$i + 1];
        }
      }
      ## �G�~�b�V�����i�[ ##
      elsif( $TBL_COUNT == 13 )
      {
        #�G�~�b�V����R�̊i�[
        for( $i=0; $i<$DATA_COUNT; $i++ )
        {
          $EMI_R[ $i ] = $line[$i + 1];
        }
      }
      elsif( $TBL_COUNT == 14 )
      {
        #�G�~�b�V����G�̊i�[
        for( $i=0; $i<$DATA_COUNT; $i++ )
        {
          $EMI_G[ $i ] = $line[$i + 1];
        }
      }
      elsif( $TBL_COUNT == 15 )
      {
        #�G�~�b�V����B�̊i�[
        for( $i=0; $i<$DATA_COUNT; $i++ )
        {
          $EMI_B[ $i ] = $line[$i + 1];
        }
      }

      #���̃e�[�u���i�[��
      $TBL_COUNT ++;
    }
  }

}






##############
# �o�͕�
##############

for( $i=0; $i<$DATA_COUNT; $i++ )
{
  $filename = lc( $TAG_TBL[$i] ).".".$ARGV[1];
  
  open( FILEOUT, ">$filename" );
  binmode( FILEOUT );

  #�@������
  print( FILEOUT pack( "I", hex( $NORM_X[$i] ) ) );
  print( FILEOUT pack( "I", hex( $NORM_Y[$i] ) ) );
  print( FILEOUT pack( "I", hex( $NORM_Z[$i] ) ) );
  #�f�B�t���[�Y�J��
  print( FILEOUT pack( "S", $DEF_R[$i] ) );
  print( FILEOUT pack( "S", $DEF_G[$i] ) );
  print( FILEOUT pack( "S", $DEF_B[$i] ) );

  #�A���r�G���g�J���[
  print( FILEOUT pack( "S", $AMB_R[$i] ) );
  print( FILEOUT pack( "S", $AMB_G[$i] ) );
  print( FILEOUT pack( "S", $AMB_B[$i] ) );
  #�X�y�L�����[�J���[
  print( FILEOUT pack( "S", $SPQ_R[$i] ) );
  print( FILEOUT pack( "S", $SPQ_G[$i] ) );
  print( FILEOUT pack( "S", $SPQ_B[$i] ) );
  #�G�~�b�V�����J���[
  print( FILEOUT pack( "S", $EMI_R[$i] ) );
  print( FILEOUT pack( "S", $EMI_G[$i] ) );
  print( FILEOUT pack( "S", $EMI_B[$i] ) );

  
  
  close( FILEOUT );
}


exit(0);
