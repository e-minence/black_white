#============================================================================
#
#
#   イベントデータコンバーター
#
#   2009.05.24    tamada
#   2009.10.07    tomoya  pos_typeをあわせて出力するように変更
#
#
#============================================================================

#============================================================================
#============================================================================
class DataFormatError < Exception; end

class ReadWMSError < Exception; end

def debug_puts str
  #puts str
end

GRID_SIZE   =   16    #1グリッドのユニット数
BLOCK_SIZE  =   32    #地形ブロック当たりのグリッド数

#ポジションタイプ定数
EVENTDATA_POS_TYPE_GRID = "EVENTDATA_POSTYPE_GRID"
EVENTDATA_POS_TYPE_RAIL = "EVENTDATA_POSTYPE_RAIL"
MMDL_POS_TYPE_GRID = "MMDL_HEADER_POSTYPE_GRID"
MMDL_POS_TYPE_RAIL = "MMDL_HEADER_POSTYPE_RAIL"


#============================================================================
#============================================================================
#ヘッダー情報を扱うクラス
class HeaderData
  
  def initialize filename

    #定義情報
    @define         = Hash.new("none")
    @enum_in        = 0
    @enum_count     = 0

    #ヘッダーを読み込み　定数を取得する
    load( filename )
  end


  #読み込む
  def load filename
    File.open(filename){|file|
      file.each{|line|

        #追加インクルード
        if line =~ /#include[\s]+\"([^\"]+)\"/
          next_file = Regexp.last_match[1]
         debug_puts "hit include #{next_file}";
          #そのファイルをみつけられる？
          if File.exist?(next_file)
            debug_puts "include #{next_file}";
            load( next_file )
          end
        end
        
        #型の定義SEARCH
        # defile
        if line =~ /#define[\s\t]+([^\s\t]+)[\s\t\(]+([xabcdefABCDEF\d]+)/
          
          #正規表現にマッチした部分を取得
          debug_puts "define #{Regexp.last_match[1]} = #{Regexp.last_match[2]}"
          index = Regexp.last_match[1]
          number = Regexp.last_match[2]
          
          #まだ入ってないときにだけ入れる
          if @define.member?(index) == false
            
            if number =~ /x/
              @define[ index ] = number.hex
            else
              @define[ index ] = number.to_i
            end

          end
        end

        # enum
        if @enum_in == 0
          
          if line =~ /enum/
            @enum_in = 1
            @enum_count = 0
          end

        else

          #終了チェック
          if line =~ /\}/
            @enum_in = 0
          #入力
          else
            if line =~ /[\s\t]+([^\s\t,]+)/
              @enum_name = Regexp.last_match[1]

              if line =~ /[\s\t]+[^\s\t,]+[\s\t]*=[\D]*([\d]+)/
                @enum_count = Regexp.last_match[1].to_i
              end

              #まだ入ってないときにだけ入れる
              if @define.member?(index) == false
            
                debug_puts "enum #{@enum_name} = #{@enum_count}"
                @define[ @enum_name ] = @enum_count
              end

              @enum_count += 1
            end
          end

        end
      }
    }
  end


  #定義を探す 
  #戻り値
  #0or1 , 数値
  def sarch name

    @define.each do |key, value| 
      if key == name
        return [1,value]
      end
    end
      
    [0,name]
  end
  
end


#ヘッダーデータ配列クラス
class HeaderDataArray
  def initialize
    @headerArray = Array.new
    @count = 0
  end

  def load filename
    @headerArray[ @count ] = HeaderData.new( filename )
    @count += 1
  end

  def getCount
    return @count
  end

  #全体SEARCH
  def sarch name

    #数値クラスならそのまま返す
    if name.instance_of?( String ) == false
      return name
    end

    #数字文字列なら、そのまま返す
    if name =~ /^[+-]*\d/
      debug_puts "number out #{name}"
      return name.to_i
    end

    #余分なスペースなど破棄
    name = name.gsub( /\s/,"" )
    
    @headerArray.each do |headerData|
      result, value = headerData.sarch( name )
      if result == 1
        return value
      end
    end

    puts "#{name}がみつかりません" 
    raise
  end
end



#============================================================================
#============================================================================
class EventHeader

  attr_reader :blockSizeX, :blockSizeY

  def initialize stream
    @blockSizeX   =   BLOCK_SIZE
    @blockSizeY   =   BLOCK_SIZE

    line = stream.gets
    raise DataFormatError unless line =~ /^#event data/
    line = stream.gets
    raise DataFormatError unless line =~ /^#save date/
    line = stream.gets
    raise DataFormatError unless line =~/# linked worldmap file name/
    line = stream.gets.chomp!  #*.wmsファイル名
    if FileTest.exist? line then
    else
      raise ReadWMSError, "#{line}がみつかりません"
    end
    File.open(line){|file|
      readWMS file
    }
  end

  def read lines, exp
    line = lines.gets
    if line =~ exp then
      return lines.gets
    else
      raise DataFormatError
    end
  end

  def readWMS lines
    line = lines.gets
    raise ReadWMSError unless line =~/WorldMap SaveData/
    line = lines.gets
    raise ReadWMSError unless line =~/save date:/
    @width = read(lines, /^#Width/)
    @height = read(lines, /^#Height/)
    @x_ofs = Integer(read(lines, /^#block x offset/))
    @z_ofs = Integer(read(lines, /^#block z offset/))
    line = lines.gets
    raise ReadWMSError unless line =~/#3dp file names/
    (Integer(@width) * Integer(@height)).times {lines.gets}

    begin 
      line = lines.gets
      if line =~/#block size/ then
        col = lines.gets.split
        @blockSizeX = Integer(col[0])
        @blockSizeY = Integer(col[1])
        debug_puts "BLOCK SIZE  #{blockSizeX},#{blockSizeY}"
      end
    end
    
  end

  def calcXofs x
    return  x + (@blockSizeX / 2 + @x_ofs * @blockSizeX) * GRID_SIZE
  end

  def calcZofs z
    return -z + (@blockSizeY / 2 + @z_ofs * @blockSizeY) * GRID_SIZE
  end

  def calcXgridOfs x
    return x / GRID_SIZE + (@blockSizeX / 2 + @x_ofs * @blockSizeX)
    #return x / GRID_SIZE + (@blockSizeX / 2 + @x_ofs * @blockSizeX) - 1
  end

  def calcZgridOfs z
    return (- z) / GRID_SIZE + (@blockSizeY / 2 + @z_ofs * @blockSizeY)
  end

end

#============================================================================
#
#   イベントデータクラス
#
#============================================================================
class EventData

  attr :zonename
  attr :path
  attr :section_name
  attr :version
  attr :num
  attr :type
  attr :header

  attr :items

  def initialize stream, section_name, header
    @header = header
    @zonename = File.basename(stream.path,".*").upcase
    @path = stream.path
    @section_name = section_name
    @items = Array.new
    flag = false
    text_start = Regexp.new "\#SECTION START:#{@section_name}"
    text_end = Regexp.new "\#SECTION End:#{@section_name}"

    while(true) 
      line = stream.gets
      if line == nil then
        debug_puts "stream read end"
        break
      end
      if line =~text_start then
        debug_puts "SECTION START: #{@section_name}"
        flag = true
        next
      end
      if line =~text_end then
        debug_puts "SECTION END: #{@section_name}"
        break
      end
      if flag == false then
        next
      end

      if line =~/Map Event List data/ then
        debug_puts "START METHOD read_items"
        read_items stream
      end

    end
    if @version == nil || @num == nil || @type == nil then
      raise DataFormatError
    end
  end


  def read_items stream

    if @num == 0 then return end

    dummy = stream.gets #save data
    @version = Integer(stream.gets) #version
    debug_puts "Version = #{version}"

    line = stream.gets
    if line =~ /^#hold event type:/ then
      @type = Integer(stream.gets) #event type
      debug_puts "event type = #{type}"
    else
      raise DataFormatError, line
    end

    if stream.gets =~ /^#hold event number:/ then
      @num = Integer(stream.gets) #event number
      debug_puts "event num = #{@num}"
    else
      raise DataFormatError 
    end

    count = 0
    line = stream.gets

    while (count < @num )
      debug_puts "count:#{count} < @num:#{@num}"
      string = ""
      raise DataFormatError unless line =~/^#event number/
      string += line

      while(true)
        line = stream.gets
        if line =~/^\#event number/ then break end
        if line =~/^\#Map Event List Data End/ then break end
        string += line
      end

      @items << string
      count += 1
    end
    debug_puts "end of read_items"
  end


  def dump output
    debug_puts "number of events:#{@num}"
    @items.each{|item|
      output.puts"#{item}"
    }
  end

  def getStructName
    if @items.length > 0 then
      "#{@section_name}_#{@zonename}"
    else
      "NULL"
    end
  end

  def putStructName output
    if @items.length > 0 then
      output.puts "\t(void *)&#{getStructName},"
    else
      output.puts "\t(void *)NULL,"
    end
  end

end   # end of class EventData

#============================================================================
#============================================================================
class AllEvent
  attr :header

  def initialize lines, header
    @header = header
    #readEventNumber lines
  end

  def read lines, exp
    line = lines.shift
    if line =~ exp then
      return lines.shift
    else
      raise DataFormatError, "\"#{line}\", but need #{exp.to_s}"
    end
  end

  def readEventNumber lines
    line = lines.shift
    raise DataFormatError unless line =~/#event number:/
    column = line.split
    return Integer(column[2])
  end

  def readPosition lines
    line = read( lines, /#position/)
    column = line.split
    return [Integer(column[0]), Integer(column[1]), Integer(column[2])]
  end

  def calc_ofs x, z
    [@header.calcXofs(x), @header.calcZofs(z)]
  end

  def calc_grid_ofs x, z
    [@header.calcXgridOfs(x), @header.calcZgridOfs(z)]
  end

  def readSize lines
    line = read( lines, /#size/ )
    column = line.split
    return [Integer(column[0]), Integer(column[1])]
  end

  #ポジションタイプの情報があるのかチェックする
  def isPosType lines
    if lines[0] =~ /#Pos Type/
      return true;
    end
    return false;
  end
end

#============================================================================
#
# ドアイベントクラス
#
#============================================================================
class DoorEvent < AllEvent

  attr :number
  attr :door_id
  attr :next_zone_id
  attr :next_door_id
  attr :door_dir
  attr :door_type
  attr :pos_type
  attr :posx 
  attr :posy
  attr :posz
  attr :sizex
  attr :sizez
  attr :rail_index
  attr :rail_front
  attr :rail_side
  attr :rail_front_size
  attr :rail_side_size


  def initialize lines, header
    super
    @number = readEventNumber( lines )
    @door_id = read( lines, /#Door Event Label/)
    @next_zone_id = read( lines, /#Next Zone ID Name/)
    @next_door_id = read(lines, /#Next Door ID Name/)
    @door_dir = read(lines, /#Door Direction/)
    @door_type = read(lines, /#Door Type/)

    if isPosType( lines ) == false
      #今は、POSTYPEが出力されないので、こうしておく
      @pos_type = EVENTDATA_POS_TYPE_GRID
      @x, @y, @z = readPosition(lines)
      @sizex = 1
      @sizez = 1
    else
      @pos_type = read(lines, /#Pos Type/)
      if @pos_type == EVENTDATA_POS_TYPE_GRID
        @x, @y, @z = readPosition(lines)
        @sizex, @sizez = readSize(lines)
      else
        @rail_index, @rail_front, @rail_side = readPosition(lines)
        @rail_front_size, @rail_side_size = readSize(lines)
      end
    end
  end

  def dump output,headerArray
    #output.puts "\t{//#{@door_id} = #{@number}"
    #output.puts "\t\t//#{@x}, #{@z} -- GRID(#{@header.blockSizeX},#{@header.blockSizeY})"
    #output.puts "\t\t//{#{gx/GRID_SIZE}, #{@y/GRID_SIZE}, #{gz/GRID_SIZE}},"
    #output.puts "\t\t{#{@x}, #{@y}, #{@z}},"
    output.write [headerArray.sarch( @next_zone_id )].pack( "S" )
    output.write [headerArray.sarch( @next_door_id )].pack( "S" )
    output.write [headerArray.sarch( @door_dir )].pack( "C" )
    output.write [headerArray.sarch( @door_type )].pack( "C" )
    output.write [headerArray.sarch( @pos_type )].pack( "S" ) #ポジションタイプ

    if @pos_type == EVENTDATA_POS_TYPE_GRID
      gx,gz = calc_ofs @x, @z
      #ポジションタイプグリッド
      output.write [headerArray.sarch( gx )].pack("s")
      output.write [headerArray.sarch( @y )].pack("s")
      output.write [headerArray.sarch( gz )].pack("s")
      output.write [headerArray.sarch( @sizex )].pack("S")   #サイズX
      output.write [headerArray.sarch( @sizez )].pack("S")   #サイズZ
      output.write [0].pack("S")   #パディング
    else
      #ポジションタイプレール
      output.write [headerArray.sarch( @rail_index )].pack("S")     #index
      output.write [headerArray.sarch( @rail_front )].pack("S")     #front grid
      output.write [headerArray.sarch( @rail_side )].pack("S")     #side grid
      output.write [headerArray.sarch( @rail_front_size )].pack("S")     #front_siz
      output.write [headerArray.sarch( @rail_side_size )].pack("S")     #side_siz
      output.write [0].pack("S")
    end
  end

  def dumpHeader output
    output.printf("\#define %-32s %2d\n", @door_id, @number)
  end
end

#============================================================================
#
# OBJイベントクラス
#
#============================================================================
class ObjEvent < AllEvent
  attr :number
  attr :type
  attr :id              #u16
  attr :obj_code        #u16
  attr :move_code       #u16
  attr :event_type      #u16
  attr :event_flag      #u16
  attr :event_id        #u16
  attr :dir             #s16
  attr :param0          #u16
  attr :param1          #u16
  attr :param2          #u16
  attr :move_limit_x    #s16
  attr :move_limit_z    #s16
  attr :pos_type
  attr :gx              #u16 grid x
  attr :gz              #u16 grid z
  attr :y               #fx32 y
  attr :rail_index
  attr :rail_front
  attr :rail_side

  def initialize lines, header
    super
    @number = readEventNumber( lines )
    @type = read(lines, /#type/)
    @id = read(lines, /#ID name/)
    @obj_code = read(lines, /#OBJ CODE Number/)
    @move_code = read(lines, /#MOVE CODE Number/)
    @event_type = read(lines, /#EVENT TYPE Number/)
    @event_flag = read(lines, /#Flag Name/)
    @event_id = read(lines, /#Event Script Name/)
    @dir = read(lines, /#Direction Type Number/)
    @param0 = read(lines, /#Parameter 0 Number/)
    @param1 = read(lines, /#Parameter 1 Number/)
    @param2 = read(lines, /#Parameter 2 Number/)
    @move_limit_x = read(lines, /#Move Limit X Number/)
    @move_limit_z = read(lines, /#Move Limit Z Number/)

    if isPosType(lines) == false
      @pos_type = "MMDL_HEADER_POSTYPE_GRID"
      @gx, @y, @gz = readPosition( lines )
      #@gx, @gy, @z = read(lines, /#position/).split
    else
      @pos_type = read( lines, /#Pos Type/ )
      if @pos_typ == MMDL_POS_TYPE_GRID
        @gx, @y, @gz = readPosition( lines )
      else
        @rail_index, @rail_front, @rail_side = readPosition( lines )
      end
    end
  end
 
  def dump output,headerArray

    output.write [headerArray.sarch( @id )].pack("S")
    output.write [headerArray.sarch( @obj_code )].pack("S")
    output.write [headerArray.sarch( @move_code )].pack("S")
    output.write [headerArray.sarch( @event_type )].pack("S")
    output.write [headerArray.sarch( @event_flag )].pack("S")
    output.write [headerArray.sarch( @event_id )].pack("S")
    output.write [headerArray.sarch( @dir )].pack("s")
    output.write [headerArray.sarch( @param0 )].pack("S")
    output.write [headerArray.sarch( @param1 )].pack("S")
    output.write [headerArray.sarch( @param2 )].pack("S")
    output.write [headerArray.sarch( @move_limit_x )].pack("s")
    output.write [headerArray.sarch( @move_limit_z )].pack("s")
    output.write [headerArray.sarch( @pos_type )].pack("I")

    if @pos_type == MMDL_POS_TYPE_GRID
      gx,gz = calc_grid_ofs @gx, @gz
      output.write [headerArray.sarch( gx )].pack("S")
      output.write [headerArray.sarch( gz )].pack("S")
      fx32_y = @y << 12;
      output.write [headerArray.sarch( fx32_y )].pack("i")
    else
      output.write [headerArray.sarch( @rail_index )].pack("S")  #index
      output.write [headerArray.sarch( @rail_front )].pack("S")  #front
      output.write [headerArray.sarch( @rail_side )].pack("S")   #side
      output.write [0].pack("S")
    end
  end

  def dumpHeader output
    output.printf( "\#define %-32s  %2d\n", @id, @number)
  end
end

#============================================================================
#
# POSイベントクラス
#
#============================================================================
class PosEvent < AllEvent

  attr :number
  attr :pos_id
  attr :event_id
  attr :workname
  attr :workvalue
  attr :pos_type
  attr :sizex
  attr :sizez
  attr :x 
  attr :y
  attr :z
  attr :rail_index
  attr :rail_front
  attr :rail_side
  attr :rail_front_size
  attr :rail_side_size


  def initialize lines, header
    super
    @number = readEventNumber( lines )
    @pos_id = read( lines, /#Pos Event Label/)
    @event_id = read(lines, /#Pos Script Name/)
    @workname = read(lines, /#Event Trigger Work Name/)
    @workvalue = read(lines, /#Event Trigger Work Value/)
    
    if isPosType( lines ) == false
      #今は、POSTYPEが出力されないので、こうしておく
      @pos_type = EVENTDATA_POS_TYPE_GRID
      @sizex = read(lines, /#Pos Event Size X/)
      @sizez = read(lines, /#Pos Event Size Z/)
      @x, @y, @z = readPosition(lines)
    else
      @pos_type = read(lines, /#Pos Type/)
      if @pos_type == EVENTDATA_POS_TYPE_GRID
        @sizex = read(lines, /#Pos Event Size X/)
        @sizez = read(lines, /#Pos Event Size Z/)
        @x, @y, @z = readPosition(lines)
      else
        @rail_index, @rail_front, @rail_side = readPosition(lines)
        @rail_front_size, @rail_side_size = readSize(lines)
      end
    end
  end

  def dump output,headerArray

    output.write [headerArray.sarch( @event_id )].pack("S")
    output.write [headerArray.sarch( @workvalue )].pack("S")
    output.write [headerArray.sarch( @workname )].pack("S")
    output.write [headerArray.sarch( @pos_type )].pack("S")

    if @pos_type == EVENTDATA_POS_TYPE_GRID
      gx,gz = calc_grid_ofs @x, @z
      output.write [headerArray.sarch( gx )].pack("S")
      output.write [headerArray.sarch( gz )].pack("S")
      output.write [headerArray.sarch( @sizex )].pack("S")
      output.write [headerArray.sarch( @sizez )].pack("S")
      output.write [headerArray.sarch( @y )].pack("s")
      output.write [0].pack("S")
    else

      output.write [headerArray.sarch( @rail_index )].pack("S")     #index
      output.write [headerArray.sarch( @rail_front )].pack("S")     #front grid
      output.write [headerArray.sarch( @rail_side )].pack("S")     #side grid
      output.write [headerArray.sarch( @rail_front_size )].pack("S")     #front_siz
      output.write [headerArray.sarch( @rail_side_size )].pack("S")     #side_siz
      output.write [0].pack("S")
    end

  end

  def dumpHeader output
    output.printf("\#define %-32s %2d\n", @pos_id, @number)
  end
end

#============================================================================
#
# BGイベントクラス
#
#============================================================================
class BgEvent < AllEvent

  attr :number
  attr :bg_id
  attr :bg_type
  attr :bg_dir
  attr :event_id
  attr :pos_type
  attr :x 
  attr :y
  attr :z
  attr :rail_index
  attr :rail_front
  attr :rail_side


  def initialize lines, header
    super
    @number = readEventNumber( lines )
    @bg_id = read( lines, /#Bg Event Label/)
    @bg_type = read(lines, /#Bg Event Type/)
    @bg_dir = read(lines, /#Bg Event Reaction Direction ID/)
    @event_id = read(lines, /#Bg Script Name/)

    if isPosType( lines ) == false
      #今は、POSTYPEが出力されないので、こうしておく
      @pos_type = EVENTDATA_POS_TYPE_GRID
      @x, @y, @z = readPosition(lines)
    else

      @pos_type = read(lines, /#Pos Type/)

      if @pos_type == EVENTDATA_POS_TYPE_GRID
        @x, @y, @z = readPosition(lines)
      else
        @rail_index, @rail_front, @rail_side = readPosition(lines)
      end

    end
  end

  def dump output,headerArray


    output.write [headerArray.sarch( @event_id )].pack("S")
    output.write [headerArray.sarch( @bg_type )].pack("S")
    output.write [headerArray.sarch( @bg_dir )].pack("S")
    output.write [headerArray.sarch( @pos_type )].pack( "S" ) #ポジションタイプ

    if @pos_type == EVENTDATA_POS_TYPE_GRID
      gx,gz = calc_grid_ofs @x, @z
      output.write [headerArray.sarch( gx )].pack("i")
      output.write [headerArray.sarch( gz )].pack("i")
      output.write [headerArray.sarch( @y )].pack("i")
    else
      output.write [headerArray.sarch( @rail_index )].pack("S")     #index
      output.write [headerArray.sarch( @rail_front )].pack("S")     #front grid
      output.write [headerArray.sarch( @rail_side )].pack("S")     #side grid
      output.write [0].pack("S")
      output.write [0].pack("S")
      output.write [0].pack("S")
    end
  end

  def dumpHeader output
    output.printf("\#define %-32s %2d\n", @bg_id, @number)
  end
end

#============================================================================
#
# ドアイベントデータクラス
#
#============================================================================
class DoorEventData < EventData

  def initialize stream, section_name, header
    super
    @doors = Array.new
    @items.each{|item|
      lines = item.split(/\r\n/)
      @doors << DoorEvent.new(lines, header)
    }
  end

  def dump output,headerArray
    if @doors.length == 0 then
      return
    end
    @doors.each{|door| door.dump(output,headerArray) }
  end

  def dumpHeader output
    incguard = "ZONE_#{@zonename.upcase}_EVD_H"
    output.puts "//このファイルは#{@path}から自動生成されました"
    output.puts "#ifndef #{incguard}"
    output.puts "#define #{incguard}"
    output.puts ""
    @doors.each{|item| item.dumpHeader( output ) }
    output.puts "\n#endif //#{incguard}"
  end

end


#============================================================================
#============================================================================
class ObjEventData < EventData

  def initialize stream, section_name, header
    super
    @objs = Array.new
    @items.each{|item|
      lines = item.split(/\r\n/)
      @objs << ObjEvent.new(lines, header)
    }
  end

  def dump output,headerArray
    if @objs.length == 0 then return end
    @objs.each{|obj| obj.dump(output, headerArray) }
  end

  def dumpHeader output
    incguard = "ZONE_#{@zonename.upcase}_EVC_H"
    output.puts "//このファイルは#{@path}から自動生成されました"
    output.puts "#ifndef #{incguard}"
    output.puts "#define #{incguard}"
    output.puts ""
    @objs.each{|item| item.dumpHeader( output ) }
    output.puts "\n#endif //#{incguard}"
  end

end

class BgEventData < EventData

  def initialize stream, section_name, header
    super
    @bgs = Array.new
    @items.each{|item|
      lines = item.split(/\r\n/)
      @bgs << BgEvent.new(lines, header)
    }
  end

  def dump output,headerArray
    if @bgs.length == 0 then return end
    @bgs.each{|bg| bg.dump(output, headerArray) }
  end

  def dumpHeader output
  end
end

class PosEventData < EventData

  def initialize stream, section_name, header
    super
    @poss = Array.new
    @items.each{|item|
      lines = item.split(/\r\n/)
      @poss << PosEvent.new(lines, header)
    }
  end

  def dump output,headerArray
    if @poss.length == 0 then return end
    @poss.each{|pos| pos.dump(output, headerArray) }
  end

  def dumpHeader output
  end
end

#============================================================================
#
# コンバート本体
#
#============================================================================
begin
  
  headerArray = HeaderDataArray.new();
  allHeader = ARGV[1,64] #とりあえず64個まで
  
  #必要ヘッダーの読み込み
  allHeader.each{|headername|
    headerArray.load( headername )
  }


  
  File.open(ARGV[0]){|file|
    header = EventHeader.new(file)
    obj_events = ObjEventData.new(file, "OBJ_EVENT", header)
    bg_events = BgEventData.new(file, "BG_EVENT", header)
    pos_events = PosEventData.new(file, "POS_EVENT", header)
    door_events = DoorEventData.new(file, "DOOR_EVENT", header)

    id = "event_" + door_events.zonename.downcase
    ofilename = id.sub(/event_/,"../tmp/")

    headerArray.load( "../../script/#{door_events.zonename.downcase}_def.h" )

    File.open("#{ofilename}.bin", "wb"){|file|

      file.write [bg_events.items.length].pack("C")
      file.write [obj_events.items.length].pack("C")
      file.write [door_events.items.length].pack("C")
      file.write [pos_events.items.length].pack("C")

      bg_events.dump(file, headerArray)
      obj_events.dump(file, headerArray)
      door_events.dump(file, headerArray)
      pos_events.dump(file, headerArray)
    }
  }
end




