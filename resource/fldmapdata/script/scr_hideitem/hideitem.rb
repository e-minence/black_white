#----------------------------------------------------------------------------
#
#   隠しアイテムデータコンバーター
#
#   2009.12.18  tamada GAMEFREAK inc.
#
#----------------------------------------------------------------------------
COL_MAPNAME = 0
COL_MAPID = 1
COL_POSX  = 2
COL_POSZ  = 3
COL_ITEM  = 4
COL_NUM   = 5
COL_RESPONSE = 6
COL_SPECIAL = 7
COL_ID = 8

#C形式ヘッダ読み込みのため
require "#{ENV["PROJECT_ROOT"]}tools/headerdata.rb"
require "#{ENV["PROJECT_ROOT"]}tools/hash/item_hash.rb"

class ItemSymbolError < Exception; end

def debug_puts( str )
  STDERR.puts str
end

#------------------------------------------------------------
#------------------------------------------------------------
class Dict
  def Dict.getItemSymbol( namae )
    if $item_hash.has_key?( namae ) then
      return $item_hash[ namae ]
    end
    raise ItemSymbolError, "#アイテム\"{namae}\" は存在しません！！"
  end

  def Dict.readZoneID( filename )
    zone_dict = HeaderDataArray.new
    zone_dict.load( filename )
  end

  def Dict.checkZoneID( zone_symbol )
    return @@zone_dict.search( zone_symbol )
  end

  @@zone_dict = Dict.readZoneID( "../../zonetable/zone_id.h" )
end

#------------------------------------------------------------
#------------------------------------------------------------
class HideItem
  attr_reader  :mapname, :mapid, :posx, :posz, :itemno, :num, :response, :special, :id

  def initialize(mapname, mapid, x, z, itemno, num, response, special, id)
    @mapname = mapname
    @mapid = Dict.checkZoneID( mapid )
    @posx = x
    @posz = z
    @itemno = Dict.getItemSymbol( itemno )
    @num = num
    @response = response
    @special = special
    @id = id
  end
end

def readItemData( filename )
  debug_puts filename
  items = Array.new

  lines = File.open(filename).readlines
  # read header
  while line = lines.shift
    column = line.split(",")
    if column.length >= 10 then break end
    debug_puts("search title:#{line}")
=begin
    if column[0 .. 6] == [
      "マップ名",	"MAPラベル","X","Y",
      "アイテム名",	"個数",	"反応",	"特殊",	"番号" ]
      break
    end
=end
  end

  while line = lines.shift
    column = line.split(",")
    if column.length < 10 then
      break
    end
    items << HideItem.new( column[COL_MAPNAME], column[COL_MAPID], column[COL_POSX], column[COL_POSZ],
                          column[COL_ITEM], column[COL_NUM], column[COL_RESPONSE],
                          column[COL_SPECIAL], column[COL_ID] )
  end
  return items
end

#------------------------------------------------------------
#------------------------------------------------------------
def getCDAT( items )
  output = ""
  output += sprintf("static const HIDE_ITEM hide_item_data = {\n")
  output += sprintf("\n")
  items.each{|item|
    output += sprintf("\t { %-03d, %-2d }, // %s (%d,%d)\n",
                      item.itemno, item.num, item.mapname, item.posx, item.posz )
  }
  output += sprintf( "}; // end of hide_item_data\n" )
  return output
end

#------------------------------------------------------------
#------------------------------------------------------------
def getEV( items )
  output =<<HEADSTRING
//======================================================================
//
//
//    hide_item.ev
//    スクリプトファイル：hide_item用
//
//======================================================================

  .text

  .include  "scr_seq_def.h"
 	.include "msg_item_get_scr.h
  //.include  "hide_item.h"

//--------------------------------------------------------------
//               スクリプトテーブル
//--------------------------------------------------------------
HEADSTRING

  items.each_with_index{|item, index|
    output += sprintf("ev_hide_item_%03d:\n", index)
  }
  output += <<TAILSTRING
//--------------------------------------------------------------
//               スクリプト本体
//--------------------------------------------------------------
TAILSTRING

  return output
end

#------------------------------------------------------------
#
#   メイン
#
#------------------------------------------------------------
items = readItemData( ARGV[0] )

File.open(ARGV[1],"w"){|file|
  output = getCDAT( items )
  file.write output
}

File.open(ARGV[2],"w"){|file|
  output = getEV( items )
  file.write output
}




