#----------------------------------------------------------------------------
#
#   �B���A�C�e���f�[�^�R���o�[�^�[
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
COL_WORLDMAP = 6
COL_REVIVAL = 7
COL_ID = 8
COL_THROUGH_NUMBER = 9

COL_MAX = 10

#C�`���w�b�_�ǂݍ��݂̂���
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
    raise ItemSymbolError, "�A�C�e��\"#{namae}\" �͑��݂��܂���I�I"
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
  attr_reader  :mapname, :mapid, :posx, :posz, :itemno, :num, :world_flag, :revival_flag, :id

  attr_reader :itemname, :mapid_name

  def initialize(mapname, mapid, x, z, itemname, num, world_flag, revival_flag, id)
    @mapname = mapname
    @mapid = Dict.checkZoneID( "ZONE_ID_" + mapid.upcase )
    @posx = x
    @posz = z
    @itemno = Dict.getItemSymbol( itemname )
    @num = num
    @world_flag = world_flag
    @revival_flag = revival_flag
    @id = id

    @mapid_name = mapid
    @itemname = itemname
  end
end

def readItemData( filename )
  debug_puts filename
  items = Array.new

  lines = File.open(filename).readlines

  #�w�b�_�����ǂݔ�΂�
  while line = lines.shift
    column = line.split(",")
	if column.length >= COL_MAX then break end
    debug_puts("search title:#{line}")
=begin
    if column[0 .. 6] == [
      "�}�b�v��",	"MAP���x��","X","Y",
      "�A�C�e����",	"��",	"����",	"����",	"�ԍ�" ]
      break
    end
=end
  end

  #�f�[�^�����ǂݍ���
  while line = lines.shift
    column = line.split(",")
    if column.length < COL_MAX then
      break
    end
	if column[COL_NUM] == 0 then
	  next
	end
    items << HideItem.new( column[COL_MAPNAME], column[COL_MAPID],
                          column[COL_POSX], column[COL_POSZ],
                          column[COL_ITEM], column[COL_NUM], column[COL_WORLDMAP],
                          column[COL_REVIVAL], column[COL_ID] )
  end
  return items
end

#------------------------------------------------------------
#
# �B���A�C�e���p�f�[�^�t�@�C���쐬
#
#------------------------------------------------------------
def getCDAT( items )
  output = ""
  #output += sprintf("static const u16 hide_item_data[%d] = {\n", items.length)
  output += sprintf("static const HIDE_ITEM_DATA hide_item_data[%d] = {\n", items.length)
  output += sprintf("\n")
  items.each{|item|
    output += sprintf("\t{ // %s\n", item.itemname )
    output += sprintf("\t\t%3d, \n", item.id )
    output += sprintf("\t\t%d,%d, // flags\n", item.world_flag, item.revival_flag )
    #output += sprintf("\t\t%3d, // %s %s\n", item.mapid, item.mapid_name, item.mapname )
    output += sprintf("\t\tZONE_ID_%s, // %s\n", item.mapid_name.upcase, item.mapname )
    output += sprintf("\t\t%3d,%3d, // x, y\n", item.posx, item.posz )
    output += sprintf("\t},\n")

  }
  output += sprintf( "}; // end of hide_item_data\n" )
  return output
end

#------------------------------------------------------------
#
# �B���A�C�e���p�X�N���v�g�t�@�C������
#
#------------------------------------------------------------
def getEV( items )
  output =<<HEADSTRING
//======================================================================
//
//
//    hide_item.ev
//
//    �X�N���v�g�t�@�C���Fhide_item�p
//    ���̃t�@�C����hide_item.xls���玩����������Ă��܂��B
//
//======================================================================

  .text

  .include  "scr_seq_def.h"
 	//.include "msg_item_get_scr.h
  //.include  "hide_item.h"

//--------------------------------------------------------------
//               �X�N���v�g�e�[�u��
//--------------------------------------------------------------

HEADSTRING
  items.each_with_index{|item, index|
    output += sprintf("_EVENT_DATA  ev_hide_item_%03d\n", index )
  }

  output += <<MIDDLESTRING
//--------------------------------------------------------------
//               �X�N���v�g�{�� �J�n
//--------------------------------------------------------------
MIDDLESTRING

  items.each_with_index{|item, index|
    output += sprintf("EVENT_START ev_hide_item_%03d\n", index)
    output += sprintf("//%s ( %d, %d )\n", item.mapname, item.posx, item.posz )
    output += sprintf("\t_ITEM_EVENT_HIDEITEM( %d, %d, FLAG_HIDEITEM_%03d )// %s\n",
                      item.itemno, item.num, item.id, item.itemname )
    output += sprintf("EVENT_END\n\n")
  }

  output += <<TAILSTRING
//--------------------------------------------------------------
//               �X�N���v�g�{�� �I��
//--------------------------------------------------------------
TAILSTRING
  return output
end

#------------------------------------------------------------
#
#   ���C��
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




