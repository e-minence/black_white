###########################################################
#
# �C�x���gGroup�ݒ�
#   conv.pl excel_tab switch(0or1) outdir
#   
# excel_tab:tab_out_direct.rb�ŃR���o�[�g��������
# switch�F0=newfile 1=overwrite
#
# newfile�͍��W�Ȃǂ𑀍삵�Ă��܂��܂��B
# overwrite�́A�p�����[�^�̂ݏ㏑�����܂��B
#
#   �G�N�Z�����e��mev�ɔ��f����B
#
###########################################################

if( @ARGV < 3 )
{
  print( "conv.pl excel_tab switch(0or1) outdir\n" );
  print( "0=newfile 1=overwrite\n" );
  exit(1);
}

#-----------------------------
# �\�[�X���O���[�o��
#-----------------------------
@Exceldata = undef;
$ExcelIndex = 0;

#����I�u�W�F�̏��
@OBJEVENT_KEY = ( "OBJID", "OBJCODE", "MOVECODE", "EVENTTYPE", "EVENTNAME", "FLAG_NAME", "Parameter0", "Parameter1", "Parameter2" );
@BGEVENT_KEY = ( "BGID", "BG_TYPE", "EVENT_NAME" );
@POSEVENT_KEY = ( "POSID", "EVENT_NAME", "WORK_NAME", "WORK_EQUAL_VALUE" );
@DOOREVENT_KEY = ( "DOORID", "Next Zone ID", "Next Door ID", "Door Type" );

@ZONENAME_KEY = ( "ZONE_NAME" );

#��{���
#KEY�̏��ԂɁA�C�x���g���l�܂��Ă��܂��B    �z����̃f�[�^����/KEY�̑������C�x���g��
@OBJEVENT_COMMON = ("UNDEF");
@BGEVENT_COMMON = ("UNDEF");
@POSEVENT_COMMON = ("UNDEF");
@DOOREVENT_COMMON = ("UNDEF");

#�]�[�����Ƃ̏��
@ZONE_NAME = ("UNDEF");
@OBJEVENT_ZONE = ("UNDEF");
@BGEVENT_ZONE = ("UNDEF");
@POSEVENT_ZONE = ("UNDEF");
@DOOREVENT_ZONE = ("UNDEF");


#-----------------------------
# MEV���
#-----------------------------
@MEV_OBJEVENT_KEY = ( "TYPE", "OBJID", "OBJCODE", "MOVECODE", "EVENTTYPE", "FLAG_NAME", "EVENTNAME", "Direction", "Parameter0", "Parameter1", "Parameter2", "MoveLimit X", "MoveLimit Z", "Position Type", "Position X", "Position Y", "Position Z" );
@MEV_BGEVENT_KEY = ( "BGID", "BG_TYPE", "BG_DIR", "EVENT_NAME", "Position Type", "Position X", "Position Y", "Position Z" );
@MEV_POSEVENT_KEY = ( "POSID", "EVENT_NAME", "WORK_NAME", "WORK_EQUAL_VALUE", "Position Type", "Position X", "Position Y", "Position Z", "SizeX", "SizeZ" );
@MEV_DOOREVENT_KEY = ( "DOORID", "Next Zone ID", "Next Door ID", "Door Direction", "Door Type", "Position Type", "Position X", "Position Y", "Position Z", "SizeX", "SizeZ" );

#�]�[�����Ƃ̏��
@OBJEVENT_MEV = ("UNDEF");
@BGEVENT_MEV = ("UNDEF");
@POSEVENT_MEV = ("UNDEF");
@DOOREVENT_MEV = ("UNDEF");


#-----------------------------
# ���f�[�^
#-----------------------------
$DUMMY_DIR      = "DIR_DOWN";
$DUMMY_POSTYPE  = "EVENTDATA_POSTYPE_GRID";
$DUMMY_MOVELIMIT  = 0;

$DUMMY_POS_GRID     = 16;
$DUMMY_POS_GRID_Z   = -16;
$DUMMY_POS_X_MAX    = 8;
$DUMMY_POS_Y        = 0;

$DUMMY_BG_DIR        = "BG_TALK_DIR_ALL";

$DUMMY_SIZE  = 1;

$DUMMY_DOOR_DIR  = "EXIT_DIR_NON";



#-----------------------------
# ���C������
#-----------------------------
#�G�N�Z���ǂݍ���
open( FILEIN, $ARGV[0] );
@Exceldata = <FILEIN>;
close( FILEIN );


#�R�A�f�[�^�ǂݍ���
#OBJEVENT
$ExcelIndex = &ReadEvent( \@Exceldata, $ExcelIndex, \@OBJEVENT_COMMON, \@OBJEVENT_KEY );
#BGEVENT
$ExcelIndex = &ReadEvent( \@Exceldata, $ExcelIndex, \@BGEVENT_COMMON, \@BGEVENT_KEY );
#POSEVENT
$ExcelIndex = &ReadEvent( \@Exceldata, $ExcelIndex, \@POSEVENT_COMMON, \@POSEVENT_KEY );
#DOOREVENT
$ExcelIndex = &ReadEvent( \@Exceldata, $ExcelIndex, \@DOOREVENT_COMMON, \@DOOREVENT_KEY );

#�e�]�[���̏����쐬���A�t�@�C���㏑��
while( &CheckNext( \@Exceldata, $ExcelIndex, \@OBJEVENT_KEY ) )
{
  @ZONE_NAME = undef;
  @OBJEVENT_ZONE = undef;
  @BGEVENT_ZONE = undef;
  @POSEVENT_ZONE = undef;
  @DOOREVENT_ZONE = undef;
  @ZONE_NAME = ("UNDEF");
  @OBJEVENT_ZONE = ("UNDEF");
  @BGEVENT_ZONE = ("UNDEF");
  @POSEVENT_ZONE = ("UNDEF");
  @DOOREVENT_ZONE = ("UNDEF");

  #ZONE_NAME
  $ExcelIndex = &ReadEvent( \@Exceldata, $ExcelIndex, \@ZONE_NAME, \@ZONENAME_KEY );
  #OBJEVENT
  $ExcelIndex = &ReadEvent( \@Exceldata, $ExcelIndex, \@OBJEVENT_ZONE, \@OBJEVENT_KEY );
  #BGEVENT
  $ExcelIndex = &ReadEvent( \@Exceldata, $ExcelIndex, \@BGEVENT_ZONE, \@BGEVENT_KEY );
  #POSEVENT
  $ExcelIndex = &ReadEvent( \@Exceldata, $ExcelIndex, \@POSEVENT_ZONE, \@POSEVENT_KEY );
  #DOOREVENT
  $ExcelIndex = &ReadEvent( \@Exceldata, $ExcelIndex, \@DOOREVENT_ZONE, \@DOOREVENT_KEY );

 
  #�I�[�o�[���C�g�̏ꍇ�́A�����ǂݍ���ł��āA����������B
  $ZONE_NAME[0] = lc( $ZONE_NAME[0] );  #�������ɂ���

  #mev�p�o�b�t�@���N���A
  @OBJEVENT_MEV = undef;
  @BGEVENT_MEV = undef;
  @POSEVENT_MEV = undef;
  @DOOREVENT_MEV = undef;
  @OBJEVENT_MEV = ("UNDEF");
  @BGEVENT_MEV = ("UNDEF");
  @POSEVENT_MEV = ("UNDEF");
  @DOOREVENT_MEV = ("UNDEF");

  if( $ARGV[1] )
  {
    if( -e $ARGV[2]."/".$ZONE_NAME[0].".mev" )
    {
      @MEV_FILE = undef;
      open( FILEIN, $ARGV[2]."/".$ZONE_NAME[0].".mev" );
      @MEV_FILE = <FILEIN>;
      close( FILEIN );
    }
    else
    {
      print( $ARGV[2]."/".$ZONE_NAME[0].".mev�@���݂���܂���\n" );
      exit(1);
    }

    &ReadMevData_OBJEVENT( \@MEV_FILE, \@OBJEVENT_MEV );
    &ReadMevData_BGEVENT( \@MEV_FILE, \@BGEVENT_MEV );
    &ReadMevData_POSEVENT( \@MEV_FILE, \@POSEVENT_MEV );
    &ReadMevData_DOOREVENT( \@MEV_FILE, \@DOOREVENT_MEV );

    &debug_put( "OBJEVENT_MEV write\n" );
    &debug_array_put( \@OBJEVENT_MEV );
    &debug_put( "OBJEVENT_MEV write end\n" );

    &debug_put( "BGEVENT_MEV write\n" );
    &debug_array_put( \@BGEVENT_MEV );
    &debug_put( "BGEVENT_MEV write end\n" );

    &debug_put( "POSEVENT_MEV write\n" );
    &debug_array_put( \@POSEVENT_MEV );
    &debug_put( "POSEVENT_MEV write end\n" );

    &debug_put( "DOOREVENT_MEV write\n" );
    &debug_array_put( \@DOOREVENT_MEV );
    &debug_put( "DOOREVENT_MEV write end\n" );
  }

  #�}�[�W���Ȃ���AOBJEVENT_MEV������������B
  #COMMON
  &SetMevData_OBJEVENT( \@OBJEVENT_MEV, \@OBJEVENT_COMMON );
  &SetMevData_BGEVENT( \@BGEVENT_MEV, \@BGEVENT_COMMON );
  &SetMevData_POSEVENT( \@POSEVENT_MEV, \@POSEVENT_COMMON );
  &SetMevData_DOOREVENT( \@DOOREVENT_MEV, \@DOOREVENT_COMMON, "_".uc($ZONE_NAME[0]) );
  #ZONE
  &SetMevData_OBJEVENT( \@OBJEVENT_MEV, \@OBJEVENT_ZONE );
  &SetMevData_BGEVENT( \@BGEVENT_MEV, \@BGEVENT_ZONE );
  &SetMevData_POSEVENT( \@POSEVENT_MEV, \@POSEVENT_ZONE );
  &SetMevData_DOOREVENT( \@DOOREVENT_MEV, \@DOOREVENT_ZONE, "" );

  #�t�@�C���ɏo��
  open( FILEOUT, ">".$ARGV[2]."/".$ZONE_NAME[0].".mev" );

  &WriteMevHeader( $ZONE_NAME[0], $ARGV[2] );

  &WriteMev_OBJEVENT( \@OBJEVENT_MEV );
  &WriteMev_BGEVENT( \@BGEVENT_MEV );
  &WriteMev_POSEVENT( \@POSEVENT_MEV );
  &WriteMev_DOOREVENT( \@DOOREVENT_MEV );

  close( FILEOUT );
}

exit(0);

#-----------------------------------------------------------------------------
#
#   �f�o�b�N�\��
#   DEBUG
#
#-----------------------------------------------------------------------------
sub debug_put
{
  my( $str ) = @_;

  #print( $str );
}
sub debug_array_put
{
  my( $str ) = @_;

  #print( @$str );
}

#-----------------------------------------------------------------------------
#
#   �z�񐔂̎擾
#
#-----------------------------------------------------------------------------
sub GetArrayNum
{
  my( $array ) = @_;
  my( $num );
  
  if( "".$$array[0] eq "UNDEF" )
  {
    return 0;
  }
  
  $num = scalar @$array;
  return $num;
}

#-----------------------------------------------------------------------------
#
#   1���̎擾
#
#-----------------------------------------------------------------------------
sub ReadEvent
{
  my( $Excel, $index, $array, $key ) = @_;
  my( $start_word, @line, $i, $count );

  $start_word = $$key[ 0 ];

  &debug_put( "ReadEvent startword=".$start_word."\n" );
  #���܂�index��i�߂�
  do
  {
    @line = split( /\t/, $$Excel[$index] );
    
    &debug_put( "check".$line[0]." == ".$start_word."\n" );
    $index ++;
  }while( !("".$line[0] eq "$start_word") );
 
  #�����i�[���Ă���
  $count = 0;
  do
  {
    $line[0] = $$Excel[$index];

    $line[0] =~ s/\r\n//g;
    $line[0] =~ s/\n//g;
    @line = split( /\t/,  $line[0] );

    if( !($line[0] =~ /#END/) )
    {
      #����ݒ�
      for( $i=0; $i<@$key; $i++ )
      {
        $$array[$count] = $line[$i];
        &debug_put( $$key[$i]."=".$$array[$count]."\n" );
        $count ++;
      }
    }
    
    $index ++;
  }while( !($line[0] =~ /#END/) );

  &debug_put( "array write\n" );
  &debug_array_put( \@$array );
  &debug_put( "array write end\n" );

  return $index;
}


sub CheckNext
{
  my( $Excel, $index, $key ) = @_;
  my( $start_word, @line, $i, $roopnum );

  $start_word = $$key[ 0 ];

  #���܂�index��i�߂�
  $roopnum = &GetArrayNum( \@$Excel );
  for( $i=$index; $i<$roopnum; $i++ )
  {
    @line = split( /\t/,  $$Excel[$i] );

    &debug_put( "CheckNext check ".$line[0]." == ".$start_word."\n" );
    if( ("".$line[0] eq "$start_word") )
    {
      return 1; #����
    }
  }
  return 0; #�Ȃ�
}


#-----------------------------------------------------------------------------
#
#   KEY�ɂ��l�̎擾
#
#   $index = KeyArray���̃p�����[�^���P�Ƃ����P��
#
#-----------------------------------------------------------------------------
sub GetArrayKeyNum
{
  my( $array, $key_array, $index, $key ) = @_;
  my( $i, $key_index, $roopnum );

  #�L�[�C���f�b�N�X��SEARCH
  $key_index = -1;
  $roopnum = &GetArrayNum(\@$key_array);
  for( $i=0; $i<$roopnum; $i++ )
  {
    if( "".$$key_array[ $i ] eq "$key" )
    {
      $key_index = $i;
    }
  }
  if( $key_index < 0 )
  {
    print( "GetArrayKeyNum $key ������܂���\n" );
    exit(1);
  }


  #index�Ԗڂ̗v�f��$key�̒l�����߂�
  $i = $index * &GetArrayNum(\@$key_array);
  $i += $key_index;

  if( $i < &GetArrayNum(\@$array) )
  {
    return $$array[ $i ];
  }

  print( "GetArrayKeyNum $index ���s���ł��Bindex:".$i." >= array:".@$array." key=".$key."\n" );
  exit(1);
}

#�㏑���ݒ�
sub OverWriteArrayKeyNum
{
  my( $array, $key_array, $index, $key, $num ) = @_;
  my( $i, $key_index, $roopnum );

  #�L�[�C���f�b�N�X��SEARCH
  $key_index = -1;
  $roopnum = &GetArrayNum( \@$key_array );
  for( $i=0; $i<$roopnum; $i++ )
  {
    if( "".$$key_array[ $i ] eq "$key" )
    {
      $key_index = $i;
    }
  }
  if( $key_index < 0 )
  {
    print( "OverWriteArrayKeyNum $key ������܂���\n" );
    exit(1);
  }


  #index�Ԗڂ̗v�f��$key�̒l�����߂�
  $i = $index * &GetArrayNum(\@$key_array);
  $i += $key_index;

  if( $i < &GetArrayNum(\@$array) )
  {
    $$array[ $i ] = $num;
    return ;
  }

  print( "OverWriteArrayKeyNum $index ���s���ł��Bindex:".$i." >= array:".@$array."\n" );
  exit(1);
}

#�ʏ�ݒ�
sub SetArrayKeyNum
{
  my( $array, $key_array, $index, $key, $num ) = @_;
  my( $i, $key_index, $roopnum );

  #�L�[�C���f�b�N�X��SEARCH
  $key_index = -1;
  $roopnum = &GetArrayNum(\@$key_array);
  for( $i=0; $i<$roopnum; $i++ )
  {
    if( "".$$key_array[ $i ] eq "$key" )
    {
      $key_index = $i;
    }
  }
  if( $key_index < 0 )
  {
    print( "SetArrayKeyNum $key ������܂���\n" );
    exit(1);
  }


  #index�Ԗڂ̗v�f��$key�̒l�����߂�
  $i = $index * &GetArrayNum(\@$key_array);
  $i += $key_index;

  $$array[ $i ] = $num;
}

#-----------------------------------------------------------------------------
#
#   MEV���̑���
#
#-----------------------------------------------------------------------------
#MEV�t�@�C������A�K�v�f�[�^��ǂݍ���
sub ReadMevData_OBJEVENT
{
  my( $mev_file, $array ) = @_;
  my( $index, $input, $line, $count, $key_index, @position, $roopnum );

  $input = 0;
  $count = 0;

  $roopnum = &GetArrayNum( \@$mev_file ); 
  for( $index=0; $index<$roopnum; $index++ )
  {
    $line = $$mev_file[ $index ];
    $line =~ s/\r\n//g;
    $line =~ s/\n//g;

    &debug_put( "mev date ::".$$mev_file[ $index ]."\n" );
    
    if( $input == 0 )
    {
      if( $line =~ /#SECTION START:OBJ_EVENT/ )
      {
        $input = 1;

        #�ێ����Ă���C�x���g�������߂�
        do
        {
          $index ++;
          &debug_put( "hold event number�܂�index��i�߂� ::".$$mev_file[ $index ]."\n" );
        }while( !($$mev_file[ $index ] =~ /#hold event number/) );

        #�ێ����Ă���C�x���g�����O�Ȃ�I����
        $index ++;
        if( $$mev_file[ $index ] == 0 )
        {
          return ;
        }

        #event number�܂�index��i�߂�
        do
        {
          $index ++;
          &debug_put( "event number�܂�index��i�߂� ::".$$mev_file[ $index ]."\n" );
        }while( !($$mev_file[ $index ] =~ /#event number/) );
      }
    }
    else
    {
      #�I��
      if( $line =~ /#SECTION End:OBJ_EVENT/ )
      {
        $input = 0;
        return ;
      }

      #�i�[����
      #comment�s�̓X�L�b�v
      if( !($line =~ /^#/) )
      {
        $key_index = $count % &GetArrayNum(\@MEV_OBJEVENT_KEY);
        #���W�̂�split�̕K�v������
        if( $MEV_OBJEVENT_KEY[ $key_index ] =~ /Position X/ )
        {
          @position = split( /\s/, $line );

          $$array[ $count ] = $position[0];
          $count ++;
          $$array[ $count ] = $position[1];
          $count ++;
          $$array[ $count ] = $position[2];
          $count ++;
        }
        else
        {
          #�����i�[���邾��
          $$array[ $count ] = $line;
          $count ++;
        }
      }
    }
  }
}

#BGEVENT
sub ReadMevData_BGEVENT
{
  my( $mev_file, $array ) = @_;
  my( $index, $input, $line, $count, $roopnum );

  $input = 0;
  $count = 0;
  $roopnum = &GetArrayNum( \@$mev_file );
  for( $index=0; $index<$roopnum; $index++ )
  {
    $line = $$mev_file[ $index ];
    $line =~ s/\r\n//g;
    $line =~ s/\n//g;
    
    if( $input == 0 )
    {
      if( $line =~ /#SECTION START:BG_EVENT/ )
      {
        $input = 1;

        #�ێ����Ă���C�x���g�������߂�
        do
        {
          $index ++;
          &debug_put( "hold event number�܂�index��i�߂� ::".$$mev_file[ $index ]."\n" );
        }while( !($$mev_file[ $index ] =~ /#hold event number/) );

        #�ێ����Ă���C�x���g�����O�Ȃ�I����
        $index ++;
        if( $$mev_file[ $index ] == 0 )
        {
          return ;
        }

        #event number�܂�index��i�߂�
        do
        {
          $index ++;
          &debug_put( "event number�܂�index��i�߂� ::".$$mev_file[ $index ]."\n" );
        }while( !($$mev_file[ $index ] =~ /#event number/) );
      }
    }
    else
    {
      #�I��
      if( $line =~ /#SECTION End:BG_EVENT/ )
      {
        $input = 0;
        return ;
      }

      #�i�[����
      #comment�s�̓X�L�b�v
      if( !($line =~ /^#/) )
      {
        $key_index = $count % &GetArrayNum(\@MEV_BGEVENT_KEY);
        #���W�̂�split�̕K�v������
        if( $MEV_BGEVENT_KEY[ $key_index ] =~ /Position X/ )
        {
          @position = split( /\s/, $line );

          $$array[ $count ] = $position[0];
          $count ++;
          $$array[ $count ] = $position[1];
          $count ++;
          $$array[ $count ] = $position[2];
          $count ++;
        }
        else
        {
          #�����i�[���邾��
          $$array[ $count ] = $line;
          $count ++;
        }
      }
    }
  }
}


#POSEVENT
sub ReadMevData_POSEVENT
{
  my( $mev_file, $array ) = @_;
  my( $index, $input, $line, $count, $roopnum );

  $input = 0;
  $count = 0;
  $roopnum = &GetArrayNum( \@$mev_file );
  for( $index=0; $index<$roopnum; $index++ )
  {
    $line = $$mev_file[ $index ];
    $line =~ s/\r\n//g;
    $line =~ s/\n//g;
    
    if( $input == 0 )
    {
      if( $line =~ /#SECTION START:POS_EVENT/ )
      {
        $input = 1;

        #�ێ����Ă���C�x���g�������߂�
        do
        {
          $index ++;
          &debug_put( "hold event number�܂�index��i�߂� ::".$$mev_file[ $index ]."\n" );
        }while( !($$mev_file[ $index ] =~ /#hold event number/) );

        #�ێ����Ă���C�x���g�����O�Ȃ�I����
        $index ++;
        if( $$mev_file[ $index ] == 0 )
        {
          return ;
        }

        #event number�܂�index��i�߂�
        do
        {
          $index ++;
          &debug_put( "event number�܂�index��i�߂� ::".$$mev_file[ $index ]."\n" );
        }while( !($$mev_file[ $index ] =~ /#event number/) );
      }
    }
    else
    {
      #�I��
      if( $line =~ /#SECTION End:POS_EVENT/ )
      {
        $input = 0;
        return ;
      }

      #�i�[����
      #comment�s�̓X�L�b�v
      if( !($line =~ /^#/) )
      {
        $key_index = $count % &GetArrayNum(\@MEV_POSEVENT_KEY);
        #���W�̂�split�̕K�v������
        if( $MEV_POSEVENT_KEY[ $key_index ] =~ /Position X/ )
        {
          @position = split( /\s/, $line );

          $$array[ $count ] = $position[0];
          $count ++;
          $$array[ $count ] = $position[1];
          $count ++;
          $$array[ $count ] = $position[2];
          $count ++;
        }
        elsif ( $MEV_POSEVENT_KEY[ $key_index ] =~ /SizeX/ )
        {
          @position = split( /\s/, $line );

          $$array[ $count ] = $position[0];
          $count ++;
          $$array[ $count ] = $position[1];
          $count ++;
        }
        else
        {
          #�����i�[���邾��
          $$array[ $count ] = $line;
          $count ++;
        }
      }
    }
  }
}

#DOOREVENT
sub ReadMevData_DOOREVENT
{
  my( $mev_file, $array ) = @_;
  my( $index, $input, $line, $count, $roopnum );

  $input = 0;
  $count = 0;
  $roopnum = &GetArrayNum( \@$mev_file );
  for( $index=0; $index<$roopnum; $index++ )
  {
    $line = $$mev_file[ $index ];
    $line =~ s/\r\n//g;
    $line =~ s/\n//g;
    
    if( $input == 0 )
    {
      if( $line =~ /#SECTION START:DOOR_EVENT/ )
      {
        $input = 1;

        #�ێ����Ă���C�x���g�������߂�
        do
        {
          $index ++;
          &debug_put( "hold event number�܂�index��i�߂� ::".$$mev_file[ $index ]."\n" );
        }while( !($$mev_file[ $index ] =~ /#hold event number/) );

        #�ێ����Ă���C�x���g�����O�Ȃ�I����
        $index ++;
        if( $$mev_file[ $index ] == 0 )
        {
          return ;
        }

        #event number�܂�index��i�߂�
        do
        {
          $index ++;
          &debug_put( "event number�܂�index��i�߂� ::".$$mev_file[ $index ]."\n" );
        }while( !($$mev_file[ $index ] =~ /#event number/) );
      }
    }
    else
    {
      #�I��
      if( $line =~ /#SECTION End:DOOR_EVENT/ )
      {
        $input = 0;
        return ;
      }

      #�i�[����
      #comment�s�̓X�L�b�v
      if( !($line =~ /^#/) )
      { 
        $key_index = $count % &GetArrayNum(\@MEV_DOOREVENT_KEY);
        #���W�̂�split�̕K�v������
        if( $MEV_DOOREVENT_KEY[ $key_index ] =~ /Position X/ )
        {
          @position = split( /\s/, $line );

          $$array[ $count ] = $position[0];
          $count ++;
          $$array[ $count ] = $position[1];
          $count ++;
          $$array[ $count ] = $position[2];
          $count ++;
        }
        elsif ( $MEV_DOOREVENT_KEY[ $key_index ] =~ /SizeX/ )
        {
          @position = split( /\s/, $line );

          $$array[ $count ] = $position[0];
          $count ++;
          $$array[ $count ] = $position[1];
          $count ++;
        }
        else
        {
          #�����i�[���邾��
          $$array[ $count ] = $line;
          $count ++;
        }
      }
    }
  }
}


#-----------------------------------------------------------------------------
#
#   MEV���ɏ���merge�i�[
#
#-----------------------------------------------------------------------------
#MEV�o�b�t�@��OBJEVENT���i�[
sub SetMevData_OBJEVENT
{
  my( $mev_ev, $xls_ev ) = @_;
  my( $mev_index, $xls_index, $sarch, $xls_num, $mev_num, $merge_flag, $merge_index, $key_index, $setnum, $roopnum );

  $mev_index = &GetArrayNum(\@$mev_ev);

  $mev_num = &GetArrayNum(\@$mev_ev) / &GetArrayNum(\@MEV_OBJEVENT_KEY);
  $xls_num = &GetArrayNum(\@$xls_ev) / &GetArrayNum(\@OBJEVENT_KEY);

  &debug_put( "SetMevData_OBJEVENT mev_num=$mev_num  xls_num=$xls_num\n" );
  
  #xls�̓��e��mev_ev�Ƀ}�[�W�����Ȃ���i�[���Ă��� 
  for( $xls_index = 0; $xls_index < $xls_num; $xls_index ++ )
  {
    my( $mev_label, $xls_label );

    $xls_label = &GetArrayKeyNum( $xls_ev, \@OBJEVENT_KEY, $xls_index, "OBJID" );
    
    #$mev_ev�ɓ�����񂪂��邩�`�F�b�N
    $merge_flag = 0;
    for( $sarch=0; $sarch<$mev_num; $sarch ++ )
    {
      $mev_label = &GetArrayKeyNum( $mev_ev, \@MEV_OBJEVENT_KEY, $sarch, "OBJID" );
      if( "$mev_label" eq "$xls_label" )
      {
        $merge_flag = 1;
        $merge_index = $sarch;
      }
    }

    
    if( $merge_flag )
    {
      #merge�i�[
      $roopnum = &GetArrayNum( \@OBJEVENT_KEY );
      for( $key_index=0; $key_index<$roopnum; $key_index++ )
      {
        $setnum = &GetArrayKeyNum( $xls_ev, \@OBJEVENT_KEY, $xls_index, $OBJEVENT_KEY[ $key_index ] );
        &OverWriteArrayKeyNum( $mev_ev, \@MEV_OBJEVENT_KEY, $merge_index, $OBJEVENT_KEY[ $key_index ], $setnum );
      }
    }
    else
    {
      #�V�K�i�[
      #��ԉ��Ɋi�[����
      $merge_index = $mev_num;

      $roopnum = &GetArrayNum( \@OBJEVENT_KEY );
      for( $key_index=0; $key_index<$roopnum; $key_index++ )
      {
        $setnum = &GetArrayKeyNum( $xls_ev, \@OBJEVENT_KEY, $xls_index, $OBJEVENT_KEY[ $key_index ] );
        &SetArrayKeyNum( $mev_ev, \@MEV_OBJEVENT_KEY, $merge_index, $OBJEVENT_KEY[ $key_index ], $setnum );
      }

      #����Ȃ�������������
      &SetArrayKeyNum( $mev_ev, \@MEV_OBJEVENT_KEY, $merge_index, "TYPE", 1 );
      &SetArrayKeyNum( $mev_ev, \@MEV_OBJEVENT_KEY, $merge_index, "Direction", $DUMMY_DIR );
      &SetArrayKeyNum( $mev_ev, \@MEV_OBJEVENT_KEY, $merge_index, "Position Type", $DUMMY_POSTYPE );
      &SetArrayKeyNum( $mev_ev, \@MEV_OBJEVENT_KEY, $merge_index, "MoveLimit X", $DUMMY_MOVELIMIT );
      &SetArrayKeyNum( $mev_ev, \@MEV_OBJEVENT_KEY, $merge_index, "MoveLimit Z", $DUMMY_MOVELIMIT );

      #�f�[�^�C���f�b�N�X����|�W�V�����̎�������
      #���S����ɕ��ׂ� 8���Z�}�C�i�X�ց@Y�͂O
      {
        my( $pos_x, $pos_z );
        $pos_x = $merge_index % $DUMMY_POS_X_MAX;
        $pos_z = $merge_index / $DUMMY_POS_X_MAX;

        $pos_x = $pos_x * $DUMMY_POS_GRID;
        $pos_z = $pos_z * $DUMMY_POS_GRID_Z;

        &SetArrayKeyNum( $mev_ev, \@MEV_OBJEVENT_KEY, $merge_index, "Position X", $pos_x );
        &SetArrayKeyNum( $mev_ev, \@MEV_OBJEVENT_KEY, $merge_index, "Position Y", $DUMMY_POS_Y );
        &SetArrayKeyNum( $mev_ev, \@MEV_OBJEVENT_KEY, $merge_index, "Position Z", $pos_z );
      }
    }
  }
}

#BGEVENT
sub SetMevData_BGEVENT
{
  my( $mev_ev, $xls_ev ) = @_;
  my( $mev_index, $xls_index, $sarch, $xls_num, $mev_num, $merge_flag, $merge_index, $key_index, $setnum, $roopnum );

  $mev_index = &GetArrayNum( \@$mev_ev );

  $mev_num = &GetArrayNum(\@$mev_ev) / &GetArrayNum(\@MEV_BGEVENT_KEY);
  $xls_num = &GetArrayNum(\@$xls_ev) / &GetArrayNum(\@BGEVENT_KEY);

  &debug_put( "SetMevData_BGEVENT mev_num=$mev_num  xls_num=$xls_num\n" );
  
  #xls�̓��e��mev_ev�Ƀ}�[�W�����Ȃ���i�[���Ă��� 
  for( $xls_index = 0; $xls_index < $xls_num; $xls_index ++ )
  {
    my( $mev_label, $xls_label );

    $xls_label = &GetArrayKeyNum( $xls_ev, \@BGEVENT_KEY, $xls_index, "BGID" );
    
    #$mev_ev�ɓ�����񂪂��邩�`�F�b�N
    $merge_flag = 0;
    for( $sarch=0; $sarch<$mev_num; $sarch ++ )
    {
      $mev_label = &GetArrayKeyNum( $mev_ev, \@MEV_BGEVENT_KEY, $sarch, "BGID" );
      if( "$mev_label" =~ "$xls_label" )
      {
        $merge_flag = 1;
        $merge_index = $sarch;
      }
    }

    
    if( $merge_flag )
    {
      #merge�i�[
      $roopnum = &GetArrayNum( \@BGEVENT_KEY );
      for( $key_index=0; $key_index<$roopnum; $key_index++ )
      {
        $setnum = &GetArrayKeyNum( $xls_ev, \@BGEVENT_KEY, $xls_index, $BGEVENT_KEY[ $key_index ] );
        &OverWriteArrayKeyNum( $mev_ev, \@MEV_BGEVENT_KEY, $merge_index, $BGEVENT_KEY[ $key_index ], $setnum );
      }
    }
    else
    {
      #�V�K�i�[
      #��ԉ��Ɋi�[����
      $merge_index = $mev_num;

      $roopnum = &GetArrayNum( \@BGEVENT_KEY );
      for( $key_index=0; $key_index<$roopnum; $key_index++ )
      {
        $setnum = &GetArrayKeyNum( $xls_ev, \@BGEVENT_KEY, $xls_index, $BGEVENT_KEY[ $key_index ] );
        &SetArrayKeyNum( $mev_ev, \@MEV_BGEVENT_KEY, $merge_index, $BGEVENT_KEY[ $key_index ], $setnum );
      }

      #����Ȃ�������������
      &SetArrayKeyNum( $mev_ev, \@MEV_BGEVENT_KEY, $merge_index, "BG_DIR", $DUMMY_BG_DIR );
      &SetArrayKeyNum( $mev_ev, \@MEV_BGEVENT_KEY, $merge_index, "Position Type", $DUMMY_POSTYPE );

      #�f�[�^�C���f�b�N�X����|�W�V�����̎�������
      #���S����ɕ��ׂ� 8���Z�}�C�i�X�ց@Y�͂O
      {
        my( $pos_x, $pos_z );
        $pos_x = $merge_index % $DUMMY_POS_X_MAX;
        $pos_z = $merge_index / $DUMMY_POS_X_MAX;

        $pos_x = $pos_x * $DUMMY_POS_GRID;
        $pos_z = $pos_z * $DUMMY_POS_GRID_Z;

        &SetArrayKeyNum( $mev_ev, \@MEV_BGEVENT_KEY, $merge_index, "Position X", $pos_x );
        &SetArrayKeyNum( $mev_ev, \@MEV_BGEVENT_KEY, $merge_index, "Position Y", $DUMMY_POS_Y );
        &SetArrayKeyNum( $mev_ev, \@MEV_BGEVENT_KEY, $merge_index, "Position Z", $pos_z );
      }
    }
  }
}


#POSEVENT
sub SetMevData_POSEVENT
{
  my( $mev_ev, $xls_ev ) = @_;
  my( $mev_index, $xls_index, $sarch, $xls_num, $mev_num, $merge_flag, $merge_index, $key_index, $setnum, $roopnum );

  $mev_index = &GetArrayNum( \@$mev_ev );

  $mev_num = &GetArrayNum(\@$mev_ev) / &GetArrayNum(\@MEV_POSEVENT_KEY);
  $xls_num = &GetArrayNum(\@$xls_ev) / &GetArrayNum(\@POSEVENT_KEY);

  &debug_put( "SetMevData_POSEVENT mev_num=$mev_num  xls_num=$xls_num\n" );
  
  #xls�̓��e��mev_ev�Ƀ}�[�W�����Ȃ���i�[���Ă��� 
  for( $xls_index = 0; $xls_index < $xls_num; $xls_index ++ )
  {
    my( $mev_label, $xls_label );

    $xls_label = &GetArrayKeyNum( $xls_ev, \@POSEVENT_KEY, $xls_index, "POSID" );
    
    #$mev_ev�ɓ�����񂪂��邩�`�F�b�N
    $merge_flag = 0;
    for( $sarch=0; $sarch<$mev_num; $sarch ++ )
    {
      $mev_label = &GetArrayKeyNum( $mev_ev, \@MEV_POSEVENT_KEY, $sarch, "POSID" );
      if( "$mev_label" =~ "$xls_label" )
      {
        $merge_flag = 1;
        $merge_index = $sarch;
      }
    }

    
    if( $merge_flag )
    {
      #merge�i�[
      $roopnum = &GetArrayNum( \@POSEVENT_KEY );
      for( $key_index=0; $key_index<$roopnum; $key_index++ )
      {
        $setnum = &GetArrayKeyNum( $xls_ev, \@POSEVENT_KEY, $xls_index, $POSEVENT_KEY[ $key_index ] );
        &OverWriteArrayKeyNum( $mev_ev, \@MEV_POSEVENT_KEY, $merge_index, $POSEVENT_KEY[ $key_index ], $setnum );
      }
    }
    else
    {
      #�V�K�i�[
      #��ԉ��Ɋi�[����
      $merge_index = $mev_num;

      $roopnum = &GetArrayNum( \@POSEVENT_KEY );
      for( $key_index=0; $key_index<$roopnum; $key_index++ )
      {
        $setnum = &GetArrayKeyNum( $xls_ev, \@POSEVENT_KEY, $xls_index, $POSEVENT_KEY[ $key_index ] );
        &SetArrayKeyNum( $mev_ev, \@MEV_POSEVENT_KEY, $merge_index, $POSEVENT_KEY[ $key_index ], $setnum );
      }

      #����Ȃ�������������
      &SetArrayKeyNum( $mev_ev, \@MEV_POSEVENT_KEY, $merge_index, "Position Type", $DUMMY_POSTYPE );
      &SetArrayKeyNum( $mev_ev, \@MEV_POSEVENT_KEY, $merge_index, "SizeX", $DUMMY_SIZE );
      &SetArrayKeyNum( $mev_ev, \@MEV_POSEVENT_KEY, $merge_index, "SizeZ", $DUMMY_SIZE );
    

      #�f�[�^�C���f�b�N�X����|�W�V�����̎�������
      #���S����ɕ��ׂ� 8���Z�}�C�i�X�ց@Y�͂O
      {
        my( $pos_x, $pos_z );
        $pos_x = $merge_index % $DUMMY_POS_X_MAX;
        $pos_z = $merge_index / $DUMMY_POS_X_MAX;

        $pos_x = $pos_x * $DUMMY_POS_GRID;
        $pos_z = $pos_z * $DUMMY_POS_GRID_Z;

        &SetArrayKeyNum( $mev_ev, \@MEV_POSEVENT_KEY, $merge_index, "Position X", $pos_x );
        &SetArrayKeyNum( $mev_ev, \@MEV_POSEVENT_KEY, $merge_index, "Position Y", $DUMMY_POS_Y );
        &SetArrayKeyNum( $mev_ev, \@MEV_POSEVENT_KEY, $merge_index, "Position Z", $pos_z );
      }
    }
  }
}

#DOOREVENT
sub SetMevData_DOOREVENT
{
  my( $mev_ev, $xls_ev, $id_add_str ) = @_;
  my( $mev_index, $xls_index, $sarch, $xls_num, $mev_num, $merge_flag, $merge_index, $key_index, $setnum, $roopnum );

  $mev_index = &GetArrayNum( \@$mev_ev );

  $mev_num = &GetArrayNum(\@$mev_ev) / &GetArrayNum(\@MEV_DOOREVENT_KEY);
  $xls_num = &GetArrayNum(\@$xls_ev) / &GetArrayNum(\@DOOREVENT_KEY);

  &debug_put( "SetMevData_DOOREVENT mev_num=$mev_num  xls_num=$xls_num\n" );
  
  #xls�̓��e��mev_ev�Ƀ}�[�W�����Ȃ���i�[���Ă��� 
  for( $xls_index = 0; $xls_index < $xls_num; $xls_index ++ )
  {
    my( $mev_label, $xls_label );

    $xls_label = &GetArrayKeyNum( $xls_ev, \@DOOREVENT_KEY, $xls_index, "DOORID" ).$id_add_str;
    
    #$mev_ev�ɓ�����񂪂��邩�`�F�b�N
    $merge_flag = 0;
    for( $sarch=0; $sarch<$mev_num; $sarch ++ )
    {
      $mev_label = &GetArrayKeyNum( $mev_ev, \@MEV_DOOREVENT_KEY, $sarch, "DOORID" );
      if( "$mev_label" eq "$xls_label" )
      {
        $merge_flag = 1;
        $merge_index = $sarch;
      }
    }

    
    if( $merge_flag )
    {
      #merge�i�[
      $roopnum = &GetArrayNum( \@DOOREVENT_KEY );
      for( $key_index=0; $key_index<$roopnum; $key_index++ )
      {
        if( "".$DOOREVENT_KEY[ $key_index ] eq "DOORID" )
        {
          $setnum = &GetArrayKeyNum( $xls_ev, \@DOOREVENT_KEY, $xls_index, $DOOREVENT_KEY[ $key_index ] ).$id_add_str;
        }
        else
        {
          $setnum = &GetArrayKeyNum( $xls_ev, \@DOOREVENT_KEY, $xls_index, $DOOREVENT_KEY[ $key_index ] );
        }
        &OverWriteArrayKeyNum( $mev_ev, \@MEV_DOOREVENT_KEY, $merge_index, $DOOREVENT_KEY[ $key_index ], $setnum );
      }
    }
    else
    {
      #�V�K�i�[
      #��ԉ��Ɋi�[����
      $merge_index = $mev_num;

      $roopnum = &GetArrayNum( \@DOOREVENT_KEY );
      for( $key_index=0; $key_index<$roopnum; $key_index++ )
      {
        if( "".$DOOREVENT_KEY[ $key_index ] eq "DOORID" )
        {
          $setnum = &GetArrayKeyNum( $xls_ev, \@DOOREVENT_KEY, $xls_index, $DOOREVENT_KEY[ $key_index ] ).$id_add_str;
        }
        else
        {
          $setnum = &GetArrayKeyNum( $xls_ev, \@DOOREVENT_KEY, $xls_index, $DOOREVENT_KEY[ $key_index ] );
        }
        &SetArrayKeyNum( $mev_ev, \@MEV_DOOREVENT_KEY, $merge_index, $DOOREVENT_KEY[ $key_index ], $setnum );
      }

      #����Ȃ�������������
      &SetArrayKeyNum( $mev_ev, \@MEV_DOOREVENT_KEY, $merge_index, "Door Direction", $DUMMY_DOOR_DIR );
      &SetArrayKeyNum( $mev_ev, \@MEV_DOOREVENT_KEY, $merge_index, "Position Type", $DUMMY_POSTYPE );
      &SetArrayKeyNum( $mev_ev, \@MEV_DOOREVENT_KEY, $merge_index, "SizeX", $DUMMY_SIZE );
      &SetArrayKeyNum( $mev_ev, \@MEV_DOOREVENT_KEY, $merge_index, "SizeZ", $DUMMY_SIZE );

      #�f�[�^�C���f�b�N�X����|�W�V�����̎�������
      #���S����ɕ��ׂ� 8���Z�}�C�i�X�ց@Y�͂O
      {
        my( $pos_x, $pos_z );
        $pos_x = $merge_index % $DUMMY_POS_X_MAX;
        $pos_z = $merge_index / $DUMMY_POS_X_MAX;

        $pos_x = $pos_x * $DUMMY_POS_GRID;
        $pos_z = $pos_z * $DUMMY_POS_GRID_Z;

        &SetArrayKeyNum( $mev_ev, \@MEV_DOOREVENT_KEY, $merge_index, "Position X", $pos_x );
        &SetArrayKeyNum( $mev_ev, \@MEV_DOOREVENT_KEY, $merge_index, "Position Y", $DUMMY_POS_Y );
        &SetArrayKeyNum( $mev_ev, \@MEV_DOOREVENT_KEY, $merge_index, "Position Z", $pos_z );
      }
    }
  }
}




#-----------------------------------------------------------------------------
#
#   MEV���̏o��
#
#-----------------------------------------------------------------------------
sub WriteMevHeader
{
  my( $zone_name, $dir ) = @_;
  my($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
  $year += 1900;
  $mon += 1;
  
  print( FILEOUT "#event data\r\n" );
  #print( FILEOUT "#save date:$year/$mon/$mday ".$hour.":".$min.":".$sec."\r\n" );
  print( FILEOUT "#save date:2000/00/00 00:00:00\r\n" );
  print( FILEOUT "# linked worldmap file name\r\n" );
  print( FILEOUT "C:\\home\\pokemon_wb\\resource\\fldmapdata\\eventdata\\data\\$zone_name.wms\r\n" );
}


#OBJEVENT�̏o��
sub  WriteMev_OBJEVENT
{
  my( $mev_file ) = @_;
  my( $data_num, $i, $output );
  my($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
  $year += 1900;
  $mon += 1;

  $data_num = &GetArrayNum(\@$mev_file) / &GetArrayNum(\@MEV_OBJEVENT_KEY);
  
  print( FILEOUT "#SECTION START:OBJ_EVENT\r\n" );
  print( FILEOUT "#Map Event List data\r\n" );
#  print( FILEOUT "#save date:$year/$mon/$mday ".$hour.":".$min.":".$sec."\r\n" );
  print( FILEOUT "#save date:2000/00/00 00:00:00\r\n" );
  print( FILEOUT "2\r\n" );
  print( FILEOUT "#hold event type:\r\n" );
  print( FILEOUT "1\r\n" );
  print( FILEOUT "#hold event number:\r\n" );
  print( FILEOUT "$data_num\r\n" );
  
  for( $i =0; $i<$data_num; $i++ )
  {
    print( FILEOUT "#event number: $i\r\n" );
    print( FILEOUT "#type\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "TYPE" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#ID name\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "OBJID" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#OBJ CODE Number\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "OBJCODE" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#MOVE CODE Number\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "MOVECODE" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#EVENT TYPE Number\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "EVENTTYPE" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Flag Name\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "FLAG_NAME" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Event Script Name\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "EVENTNAME" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Direction Type Number\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "Direction" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Parameter 0 Number\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "Parameter0" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Parameter 1 Number\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "Parameter1" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Parameter 2 Number\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "Parameter2" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Move Limit X Number\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "MoveLimit X" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Move Limit Z Number\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "MoveLimit Z" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Pos Type\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "Position Type" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#position\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "Position X" );
    print( FILEOUT "$output " );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "Position Y" );
    print( FILEOUT "$output " );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_OBJEVENT_KEY, $i, "Position Z" );
    print( FILEOUT "$output\r\n" );
  }

  print( FILEOUT "#Map Event List Data End\r\n" );
  print( FILEOUT "#SECTION End:OBJ_EVENT\r\n" );
}



#BGEVENT�̏o��
sub  WriteMev_BGEVENT
{
  my( $mev_file ) = @_;
  my( $data_num, $i, $output );
  my($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
  $year += 1900;
  $mon += 1;

  $data_num = &GetArrayNum(\@$mev_file) / &GetArrayNum(\@MEV_BGEVENT_KEY);
  
  print( FILEOUT "#SECTION START:BG_EVENT\r\n" );
  print( FILEOUT "#Map Event List data\r\n" );
  #print( FILEOUT "#save date:$year/$mon/$mday ".$hour.":".$min.":".$sec."\r\n" );
  print( FILEOUT "#save date:2000/00/00 00:00:00\r\n" );
  print( FILEOUT "2\r\n" );
  print( FILEOUT "#hold event type:\r\n" );
  print( FILEOUT "1\r\n" );
  print( FILEOUT "#hold event number:\r\n" );
  print( FILEOUT "$data_num\r\n" );
  
  for( $i =0; $i<$data_num; $i++ )
  {
    print( FILEOUT "#event number: $i\r\n" );
    print( FILEOUT "#Bg Event Label\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_BGEVENT_KEY, $i, "BGID" );
    print( FILEOUT "$output\r\n" );

    print( FILEOUT "#Bg Event Type\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_BGEVENT_KEY, $i, "BG_TYPE" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Bg Event Reaction Direction ID\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_BGEVENT_KEY, $i, "BG_DIR" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Bg Script Name\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_BGEVENT_KEY, $i, "EVENT_NAME" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Pos Type\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_BGEVENT_KEY, $i, "Position Type" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#position\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_BGEVENT_KEY, $i, "Position X" );
    print( FILEOUT "$output " );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_BGEVENT_KEY, $i, "Position Y" );
    print( FILEOUT "$output " );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_BGEVENT_KEY, $i, "Position Z" );
    print( FILEOUT "$output\r\n" );
  }

  print( FILEOUT "#Map Event List Data End\r\n" );
  print( FILEOUT "#SECTION End:BG_EVENT\r\n" );
}

#POSEVENT�̏o��
sub  WriteMev_POSEVENT
{
  my( $mev_file ) = @_;
  my( $data_num, $i, $output );
  my($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
  $year += 1900;
  $mon += 1;

  $data_num = &GetArrayNum(\@$mev_file) / &GetArrayNum( \@MEV_POSEVENT_KEY );
  
  print( FILEOUT "#SECTION START:POS_EVENT\r\n" );
  print( FILEOUT "#Map Event List data\r\n" );
  #print( FILEOUT "#save date:$year/$mon/$mday ".$hour.":".$min.":".$sec."\r\n" );
  print( FILEOUT "#save date:2000/00/00 00:00:00\r\n" );
  print( FILEOUT "2\r\n" );
  print( FILEOUT "#hold event type:\r\n" );
  print( FILEOUT "1\r\n" );
  print( FILEOUT "#hold event number:\r\n" );
  print( FILEOUT "$data_num\r\n" );
  
  for( $i =0; $i<$data_num; $i++ )
  {
    print( FILEOUT "#event number: $i\r\n" );
    print( FILEOUT "#Pos Event Label\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_POSEVENT_KEY, $i, "POSID" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Pos Script Name\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_POSEVENT_KEY, $i, "EVENT_NAME" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Event Trigger Work Name\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_POSEVENT_KEY, $i, "WORK_NAME" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Event Trigger Work Value\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_POSEVENT_KEY, $i, "WORK_EQUAL_VALUE" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Pos Type\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_POSEVENT_KEY, $i, "Position Type" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#position\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_POSEVENT_KEY, $i, "Position X" );
    print( FILEOUT "$output " );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_POSEVENT_KEY, $i, "Position Y" );
    print( FILEOUT "$output " );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_POSEVENT_KEY, $i, "Position Z" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#size\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_POSEVENT_KEY, $i, "SizeX" );
    print( FILEOUT "$output " );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_POSEVENT_KEY, $i, "SizeZ" );
    print( FILEOUT "$output\r\n" );
  }

  print( FILEOUT "#Map Event List Data End\r\n" );
  print( FILEOUT "#SECTION End:POS_EVENT\r\n" );
}


#DOOREVENT�̏o��
sub  WriteMev_DOOREVENT
{
  my( $mev_file ) = @_;
  my( $data_num, $i, $output );
  my($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
  $year += 1900;
  $mon += 1;

  $data_num = &GetArrayNum(\@$mev_file) / &GetArrayNum( \@MEV_DOOREVENT_KEY );
  
  print( FILEOUT "#SECTION START:DOOR_EVENT\r\n" );
  print( FILEOUT "#Map Event List data\r\n" );
  #print( FILEOUT "#save date:$year/$mon/$mday ".$hour.":".$min.":".$sec."\r\n" );
  print( FILEOUT "#save date:2000/00/00 00:00:00\r\n" );
  print( FILEOUT "2\r\n" );
  print( FILEOUT "#hold event type:\r\n" );
  print( FILEOUT "1\r\n" );
  print( FILEOUT "#hold event number:\r\n" );
  print( FILEOUT "$data_num\r\n" );
  
  for( $i =0; $i<$data_num; $i++ )
  {
    print( FILEOUT "#event number: $i\r\n" );
    print( FILEOUT "#Door Event Label\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_DOOREVENT_KEY, $i, "DOORID" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Next Zone ID Name\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_DOOREVENT_KEY, $i, "Next Zone ID" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Next Door ID Name\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_DOOREVENT_KEY, $i, "Next Door ID" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Door Direction\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_DOOREVENT_KEY, $i, "Door Direction" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Door Type\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_DOOREVENT_KEY, $i, "Door Type" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#Pos Type\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_DOOREVENT_KEY, $i, "Position Type" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#position\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_DOOREVENT_KEY, $i, "Position X" );
    print( FILEOUT "$output " );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_DOOREVENT_KEY, $i, "Position Y" );
    print( FILEOUT "$output " );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_DOOREVENT_KEY, $i, "Position Z" );
    print( FILEOUT "$output\r\n" );
    print( FILEOUT "#size\r\n" );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_DOOREVENT_KEY, $i, "SizeX" );
    print( FILEOUT "$output " );
    $output = &GetArrayKeyNum( $mev_file, \@MEV_DOOREVENT_KEY, $i, "SizeZ" );
    print( FILEOUT "$output\r\n" );
  }

  print( FILEOUT "#Map Event List Data End\r\n" );
  print( FILEOUT "#SECTION End:DOOR_EVENT\r\n" );
}


