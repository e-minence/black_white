#################################################
#
# WFBC �C�x���g�f�[�^���o��
#
#  wfbc_event_conv.pl�@tab_file land_patch_header bc_block_header wf_block_header event_bc_header event_wf_header out_bc_bin out_wf_bin
#
#
#################################################


#�����`�F�b�N
if( @ARGV < 8 )
{
  print( "wfbc_event_conv.pl�@tab_file land_patch_header bc_block_header wf_block_header event_bc_header event_wf_header out_bc_bin out_wf_bin\n" );
  exit(1);
}

#�p�����[�^
$PARA_TAG     = 0;
$PARA_IF_BLOCK_TAG  = 1;
$PARA_IF_BLOCK_ID   = 2;
$PARA_DOOR_ID  = 3;
$PARA_MAX     = 4;




#�G�N�Z���f�[�^�ǂݍ���
open( FILEIN, $ARGV[0] );
@EXCEL_FILE = <FILEIN>;
close( FILEIN );

#land_data_patch�w�b�_�[
open( FILEIN, $ARGV[1] );
@LAND_DATA_PATCH = <FILEIN>;
close( FILEIN );

#bc_block_header
open( FILEIN, $ARGV[2] );
@BC_BLOCK_HEADER = <FILEIN>;
close( FILEIN );

#wf_block_header
open( FILEIN, $ARGV[3] );
@WF_BLOCK_HEADER = <FILEIN>;
close( FILEIN );

#bc_event_header
open( FILEIN, $ARGV[4] );
@BC_EVENT_HEADER = <FILEIN>;
close( FILEIN );

#wf_event_header
open( FILEIN, $ARGV[5] );
@WF_EVENT_HEADER = <FILEIN>;
close( FILEIN );


#���̎擾
@EVENT_DATA_WF = undef;
$EVENT_DATA_WF_MAX = 0;

@EVENT_DATA_BC = undef;
$EVENT_DATA_BC_MAX = 0;

#���
$data_in = 0;
$data_Type = 0; #�ŏ���BC
foreach $one ( @EXCEL_FILE )
{
  $one =~ s/\r\n//g;
  $one =~ s/\n//g;

  @line = split( /\t/, $one );

  if( $data_in == 0 )
  {
    if( "".$line[0] =~ "TAG" )
    {
      $data_in = 1;
    }
  }
  else
  {
    if( "".$line[0] =~ "#END" )
    {
      $data_in = 0;
      $data_Type ++;
    }
    else
    {
      for( $i=0; $i<$PARA_MAX; $i++ )
      {
        if( $data_Type == 0 )
        {
          $EVENT_DATA_BC[ ($EVENT_DATA_BC_MAX * $PARA_MAX) + $i ] = $line[ $i ];
        }
        else
        {
          $EVENT_DATA_WF[ ($EVENT_DATA_WF_MAX * $PARA_MAX) + $i ] = $line[ $i ];
        }
      }

      if( $data_Type == 0 )
      {
        $EVENT_DATA_BC_MAX ++;
      }
      else
      {
        $EVENT_DATA_WF_MAX ++;
      }
    }
  }
}



#�C�x���g�̏o���������o��
for( $i=0; $i<2; $i++ )
{

  open( FILEOUT, ">".$ARGV[ 6+$i ] );
  binmode( FILEOUT );


  if( $i == 0 )
  {
    for( $j=0; $j<$EVENT_DATA_BC_MAX; $j++ )
    {
      $data_index = $j * $PARA_MAX;
      print( FILEOUT pack( "S", &getBlackBlockTag( $EVENT_DATA_BC[$data_index + $PARA_IF_BLOCK_TAG] ) ) );
      print( FILEOUT pack( "S", &getLandDataPatchIdx( $EVENT_DATA_BC[$data_index + $PARA_IF_BLOCK_ID] ) ) );
      print( FILEOUT pack( "I", &getBCDoorID( $EVENT_DATA_BC[$data_index + $PARA_DOOR_ID] ) ) );
    }
  }
  else
  {
    for( $j=0; $j<$EVENT_DATA_WF_MAX; $j++ )
    {
      $data_index = $j * $PARA_MAX;
      print( FILEOUT pack( "S", &getWhiteBlockTag( $EVENT_DATA_WF[$data_index + $PARA_IF_BLOCK_TAG] ) ) );
      print( FILEOUT pack( "S", &getLandDataPatchIdx( $EVENT_DATA_WF[$data_index + $PARA_IF_BLOCK_ID] ) ) );
      print( FILEOUT pack( "I", &getWFDoorID( $EVENT_DATA_WF[$data_index + $PARA_DOOR_ID] ) ) );
    }
  }
  
  close( FILEOUT );
}

exit(0);



sub getLandDataPatchIdx
{
  my( $tag ) = @_;
  my( $label, @label_line );

  foreach $label ( @LAND_DATA_PATCH )
  {
    $label =~ s/\t/\s/g;
    
    @label_line = split( " ", $label );
    
    if( "".$label_line[1] eq "LAND_DATA_PATCH_".$tag )
    {
      return $label_line[3];
    }
  }

  print( "getLandDataPatchIdx $tag not found\n" );
  exit(1);
}


sub getBlackBlockTag
{
  my( $tag ) = @_;
  my( $label, @label_line );

  foreach $label ( @BC_BLOCK_HEADER )
  {
    $label =~ s/\t/\s/g;
    
    @label_line = split( " ", $label );

    if( "".$label_line[1] eq "".$tag )
    {
      return $label_line[3];
    }
  }
  
  print( "getBlackBlockTag $tag not found\n" );
  exit(1);
}

sub getWhiteBlockTag
{
  my( $tag ) = @_;
  my( $label, @label_line );

  foreach $label ( @WF_BLOCK_HEADER )
  {
    $label =~ s/\t/\s/g;
    
    @label_line = split( " ", $label );
    
    if( "".$label_line[1] eq "".$tag )
    {
      return $label_line[3];
    }
  }
  
  print( "getWhiteBlockTag $tag not found\n" );
  exit(1);
}

sub getBCDoorID
{
  my( $tag ) = @_;
  my( $label, @label_line );

  foreach $label ( @BC_EVENT_HEADER )
  {
    $label =~ s/\t/\s/g;
    
    @label_line = split( " ", $label );
    
    if( "".$label_line[1] eq "".$tag )
    {
      return $label_line[2];
    }
  }
  
  print( "getBCDoorID $tag not found\n" );
  exit(1);
}

sub getWFDoorID
{
  my( $tag ) = @_;
  my( $label, @label_line );

  foreach $label ( @WF_EVENT_HEADER )
  {
    $label =~ s/\t/\s/g;
    
    @label_line = split( " ", $label );
    
    if( "".$label_line[1] eq "".$tag )
    {
      return $label_line[2];
    }
  }
  
  print( "getBCDoorID $tag not found\n" );
  exit(1);
}
