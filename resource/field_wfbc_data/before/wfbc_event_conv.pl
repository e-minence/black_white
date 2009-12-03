#################################################
#
# WFBC イベントデータを出力
#
#  wfbc_event_conv.pl　tab_file out_bc out_wf out_bc_bin out_wf_bin
#
#
#################################################


#引数チェック
if( @ARGV < 5 )
{
  print( "wfbc_event_conv.pl tab_file out_wf out_bc out_wf_bin out_bc_bin\n" );
  exit(1);
}

#パラメータ
$PARA_TAG     = 0;
$PARA_IF      = 1;
$PARA_GRID_X  = 2;
$PARA_GRID_Z  = 3;
$PARA_SIZE_X  = 4;
$PARA_SIZE_Z  = 5;
$PARA_NextZoneID  = 6;
$PARA_NextDoorID  = 7;
$PARA_ExitDir     = 8;
$PARA_DoorType    = 9;
$PARA_MAX     = 10;




#エクセルデータ読み込み
open( FILEIN, $ARGV[0] );
@EXCEL_FILE = <FILEIN>;
close( FILEIN );


#情報の取得
@EVENT_DATA_WF = undef;
$EVENT_DATA_WF_MAX = 0;

@EVENT_DATA_BC = undef;
$EVENT_DATA_BC_MAX = 0;

#情報
$data_in = 0;
$data_Type = 0; #最初がBC
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



#wev情報の出力
for( $i=0; $i<2; $i++ )
{
  open( FILEOUT, ">".$ARGV[ 1+$i ] );

  print( FILEOUT "#event data\r\n" );
  print( FILEOUT "#save date:2009/10/29 17:41:31\r\n" );
  print( FILEOUT "# linked worldmap file name\r\n" );
  $link_file = $ARGV[ 1+$i ];
  $link_file =~ s/mev/wms/g;
  print( FILEOUT "C:\\home\\pokemon_wb\\resource\\fldmapdata\\eventdata\\data\\$link_file\r\n" );
  print( FILEOUT "#SECTION START:OBJ_EVENT\r\n" );
  print( FILEOUT "#Map Event List data\r\n" );
  print( FILEOUT "#save date:2009/10/13 21:30:10\r\n" );
  print( FILEOUT "2\r\n" );
  print( FILEOUT "#hold event type:\r\n" );
  print( FILEOUT "1\r\n" );
  print( FILEOUT "#hold event number:\r\n" );
  print( FILEOUT "0\r\n" );
  print( FILEOUT "#Map Event List Data End\r\n" );
  print( FILEOUT "#SECTION End:OBJ_EVENT\r\n" );
  print( FILEOUT "#SECTION START:BG_EVENT\r\n" );
  print( FILEOUT "#Map Event List data\r\n" );
  print( FILEOUT "#save date:2009/10/13 21:30:10\r\n" );
  print( FILEOUT "2\r\n" );
  print( FILEOUT "#hold event type:\r\n" );
  print( FILEOUT "1\r\n" );
  print( FILEOUT "#hold event number:\r\n" );
  print( FILEOUT "0\r\n" );
  print( FILEOUT "#Map Event List Data End\r\n" );
  print( FILEOUT "#SECTION End:BG_EVENT\r\n" );
  print( FILEOUT "#SECTION START:POS_EVENT\r\n" );
  print( FILEOUT "#Map Event List data\r\n" );
  print( FILEOUT "#save date:2009/10/13 21:30:10\r\n" );
  print( FILEOUT "2\r\n" );
  print( FILEOUT "#hold event type:\r\n" );
  print( FILEOUT "1\r\n" );
  print( FILEOUT "#hold event number:\r\n" );
  print( FILEOUT "0\r\n" );
  print( FILEOUT "#Map Event List Data End\r\n" );
  print( FILEOUT "#SECTION End:POS_EVENT\r\n" );

  #ここを追加
  print( FILEOUT "#SECTION START:DOOR_EVENT\r\n" );
  print( FILEOUT "#Map Event List data\r\n" );
  print( FILEOUT "#save date:2009/10/13 21:30:10\r\n" );
  print( FILEOUT "2\r\n" );
  print( FILEOUT "#hold event type:\r\n" );
  print( FILEOUT "1\r\n" );
  print( FILEOUT "#hold event number:\r\n" );
  if( $i == 0 )
  {
    print( FILEOUT "$EVENT_DATA_BC_MAX\r\n" );

    for( $j=0; $j<$EVENT_DATA_BC_MAX; $j++ )
    {
      $data_index = $j * $PARA_MAX;
      
      print( FILEOUT "#event number: $j\r\n" );
      print( FILEOUT "#Door Event Label\r\n" );
      print( FILEOUT "DOOR_ID_".$EVENT_DATA_BC[ $data_index + $PARA_TAG ]."\r\n" );
      print( FILEOUT "#Next Zone ID Name\r\n" );
      print( FILEOUT $EVENT_DATA_BC[ $data_index + $PARA_NextZoneID ]."\r\n" );
      print( FILEOUT "#Next Door ID Name\r\n" );
      print( FILEOUT $EVENT_DATA_BC[ $data_index + $PARA_NextDoorID ]."\r\n" );
      print( FILEOUT "#Door Direction\r\n" );
      print( FILEOUT $EVENT_DATA_BC[ $data_index + $PARA_ExitDir ]."\r\n" );
      print( FILEOUT "#Door Type\r\n" );
      print( FILEOUT $EVENT_DATA_BC[ $data_index + $PARA_DoorType ]."\r\n" );
      print( FILEOUT "#Pos Type\r\n" );
      print( FILEOUT "EVENTDATA_POSTYPE_GRID\r\n" );
      print( FILEOUT "#position\r\n" );
      $pos_x = ($EVENT_DATA_BC[ $data_index + $PARA_GRID_X ] - 16) * 16;
      $pos_z = -($EVENT_DATA_BC[ $data_index + $PARA_GRID_Z ] - 16) * 16;
      print( FILEOUT $pos_x." 0 ".$pos_z."\r\n" );
      print( FILEOUT "#size\r\n" );
      $size_x = $EVENT_DATA_BC[ $data_index + $PARA_SIZE_X ];
      $size_z = $EVENT_DATA_BC[ $data_index + $PARA_SIZE_Z ];
      print( FILEOUT $size_x." ".$size_z."\r\n" );
    }
  }
  else
  {
    print( FILEOUT "$EVENT_DATA_WF_MAX\r\n" );

    for( $j=0; $j<$EVENT_DATA_WF_MAX; $j++ )
    {
      $data_index = $j * $PARA_MAX;
      
      print( FILEOUT "#event number: $j\r\n" );
      print( FILEOUT "#Door Event Label\r\n" );
      print( FILEOUT "DOOR_ID_".$EVENT_DATA_WF[ $data_index + $PARA_TAG ]."\r\n" );
      print( FILEOUT "#Next Zone ID Name\r\n" );
      print( FILEOUT $EVENT_DATA_WF[ $data_index + $PARA_NextZoneID ]."\r\n" );
      print( FILEOUT "#Next Door ID Name\r\n" );
      print( FILEOUT $EVENT_DATA_WF[ $data_index + $PARA_NextDoorID ]."\r\n" );
      print( FILEOUT "#Door Direction\r\n" );
      print( FILEOUT $EVENT_DATA_WF[ $data_index + $PARA_ExitDir ]."\r\n" );
      print( FILEOUT "#Door Type\r\n" );
      print( FILEOUT $EVENT_DATA_WF[ $data_index + $PARA_DoorType ]."\r\n" );
      print( FILEOUT "#Pos Type\r\n" );
      print( FILEOUT "EVENTDATA_POSTYPE_GRID\r\n" );
      print( FILEOUT "#position\r\n" );
      $pos_x = ($EVENT_DATA_WF[ $data_index + $PARA_GRID_X ] - 16) * 16;
      $pos_z = -($EVENT_DATA_WF[ $data_index + $PARA_GRID_Z ] - 16) * 16;
      print( FILEOUT $pos_x." 0 ".$pos_z."\r\n" );
      print( FILEOUT "#size\r\n" );
      $size_x = $EVENT_DATA_WF[ $data_index + $PARA_SIZE_X ];
      $size_z = $EVENT_DATA_WF[ $data_index + $PARA_SIZE_Z ];
      print( FILEOUT $size_x." ".$size_z."\r\n" );
    }
  }


  print( FILEOUT "#Map Event List Data End\r\n" );
  print( FILEOUT "#SECTION End:DOOR_EVENT\r\n" );


  close( FILEOUT );
}


#イベントの出現条件を出力
for( $i=0; $i<2; $i++ )
{

  open( FILEOUT, ">".$ARGV[ 3+$i ] );
  binmode( FILEOUT );


  if( $i == 0 )
  {
    for( $j=0; $j<$EVENT_DATA_BC_MAX; $j++ )
    {
      $data_index = $j * $PARA_MAX;
      print( FILEOUT pack( "I", $EVENT_DATA_BC[$data_index + $PARA_IF] ) );
    }
  }
  else
  {
    for( $j=0; $j<$EVENT_DATA_WF_MAX; $j++ )
    {
      $data_index = $j * $PARA_MAX;
      print( FILEOUT pack( "I", $EVENT_DATA_WF[$data_index + $PARA_IF] ) );
    }
  }
  
  close( FILEOUT );
}

exit(0);

