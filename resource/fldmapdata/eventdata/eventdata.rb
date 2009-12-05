#============================================================================
#
#
#   �C�x���g�f�[�^�R���o�[�^�[
#
#   2009.05.24    tamada
#   2009.10.07    tomoya  pos_type�����킹�ďo�͂���悤�ɕύX
#   2009.11.29    tamada  ��������Ă����̂��w�b�_�E�ˑ��t�@�C���E�o�C�i���̂��ׂĂ������悤�ɏW��
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

GRID_SIZE   =   16    #1�O���b�h�̃��j�b�g��
BLOCK_SIZE  =   32    #�n�`�u���b�N������̃O���b�h��

#�|�W�V�����^�C�v�萔
EVENTDATA_POS_TYPE_GRID = "EVENTDATA_POSTYPE_GRID"
EVENTDATA_POS_TYPE_RAIL = "EVENTDATA_POSTYPE_RAIL"


require "#{ENV["PROJECT_ROOT"]}tools/headerdata.rb"

#============================================================================
#
# *.mev�t�@�C���̃w�b�_��������������N���X
#
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
    line = stream.gets.chomp!  #*.wms�t�@�C����
    if FileTest.exist? line then
    else
      raise ReadWMSError, "#{line}���݂���܂���"
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

  #���[���h�}�b�v�f�[�^�̓ǂݍ���
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
  end

  def calcZgridOfs z
    return (- z) / GRID_SIZE + (@blockSizeY / 2 + @z_ofs * @blockSizeY)
  end

end

#============================================================================
#
#   �C�x���g��ޖ��̂܂Ƃ܂�̊��N���X
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


  def dumpBinary output
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
#
#   �X�̃C�x���g�f�[�^�̊��N���X
#
#============================================================================
class AllEvent
  attr :header
  attr :zonename

  def initialize lines, header, zonename
    @header = header
    @zonename = zonename
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

  #�|�W�V�����^�C�v�̏�񂪂���̂��`�F�b�N����
  def isPosType lines
    if lines[0] =~ /#Pos Type/
      return true;
    end
    return false;
  end
end

#============================================================================
#
# �h�A�C�x���g�N���X
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


  def initialize lines, header, zonename
    super
    @number = readEventNumber( lines )
    @door_id = read( lines, /#Door Event Label/)
    @next_zone_id = read( lines, /#Next Zone ID Name/)
    @next_door_id = read(lines, /#Next Door ID Name/)
    @door_dir = read(lines, /#Door Direction/)
    @door_type = read(lines, /#Door Type/)

    if isPosType( lines ) == false
      #���́APOSTYPE���o�͂���Ȃ��̂ŁA�������Ă���
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

  def dumpBinary output,headerArray
    #output.puts "\t{//#{@door_id} = #{@number}"
    #output.puts "\t\t//#{@x}, #{@z} -- GRID(#{@header.blockSizeX},#{@header.blockSizeY})"
    #output.puts "\t\t//{#{gx/GRID_SIZE}, #{@y/GRID_SIZE}, #{gz/GRID_SIZE}},"
    #output.puts "\t\t{#{@x}, #{@y}, #{@z}},"
    output.write [headerArray.search( @next_zone_id, @door_id )].pack( "S" )
    output.write [headerArray.search( @next_door_id, @door_id )].pack( "S" )
    output.write [headerArray.search( @door_dir, @door_id )].pack( "C" )
    output.write [headerArray.search( @door_type, @door_id )].pack( "C" )
    output.write [headerArray.search( @pos_type, @door_id )].pack( "S" ) #�|�W�V�����^�C�v

    if @pos_type == EVENTDATA_POS_TYPE_GRID
      gx,gz = calc_ofs @x, @z
      #�|�W�V�����^�C�v�O���b�h
      output.write [headerArray.search( gx, @door_id )].pack("s")
      output.write [headerArray.search( @y, @door_id )].pack("s")
      output.write [headerArray.search( gz, @door_id )].pack("s")
      output.write [headerArray.search( @sizex, @door_id )].pack("S")   #�T�C�YX
      output.write [headerArray.search( @sizez, @door_id )].pack("S")   #�T�C�YZ
      output.write [0].pack("S")   #�p�f�B���O
    else
      #�|�W�V�����^�C�v���[��
      output.write [headerArray.search( @rail_index, @door_id )].pack("S")     #index
      output.write [headerArray.search( @rail_front, @door_id )].pack("S")     #front grid
      output.write [headerArray.search( @rail_side, @door_id )].pack("s")     #side grid
      output.write [headerArray.search( @rail_front_size, @door_id )].pack("S")     #front_siz
      output.write [headerArray.search( @rail_side_size, @door_id )].pack("S")     #side_siz
      output.write [0].pack("S")
    end
  end

  def dumpHeader output
    output.printf("\#define %-32s %2d\n", @door_id, @number)
  end
  
  def getIncludeHeader header
  
    #zone_id.h
    header.push( "../../zonetable/zone_id.h " )
  
    #door_id��̃w�b�_
    if @next_door_id =~ /DOOR_ID_([^_]+)_/
      zone = $1
      zone = zone.downcase
      header.push( "../tmp/#{zone}.h " )
    else
      raise
    end
  
    #eventdata_type.h
    header.push( "../../../../prog/include/field/eventdata_type.h " )
  end
end

#============================================================================
#
# OBJ�C�x���g�N���X
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

  def initialize lines, header, zonename
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
      @pos_type = EVENTDATA_POS_TYPE_GRID
      @gx, @y, @gz = readPosition( lines )
      #@gx, @gy, @z = read(lines, /#position/).split
    else
      @pos_type = read( lines, /#Pos Type/ )
      if @pos_type == EVENTDATA_POS_TYPE_GRID
        @gx, @y, @gz = readPosition( lines )
      else
        @rail_index, @rail_front, @rail_side = readPosition( lines )
      end
    end
  end
 
  def dumpBinary output,headerArray

    output.write [headerArray.search( @id, @id )].pack("S")
    output.write [headerArray.search( @obj_code, @id )].pack("S")
    output.write [headerArray.search( @move_code, @id )].pack("S")
    output.write [headerArray.search( @event_type, @id )].pack("S")
    output.write [headerArray.search( @event_flag, @id )].pack("S")
    output.write [headerArray.search( @event_id, @id )].pack("S")
    output.write [headerArray.search( @dir, @id )].pack("s")
    output.write [headerArray.search( @param0, @id )].pack("S")
    output.write [headerArray.search( @param1, @id )].pack("S")
    output.write [headerArray.search( @param2, @id )].pack("S")
    output.write [headerArray.search( @move_limit_x, @id )].pack("s")
    output.write [headerArray.search( @move_limit_z, @id )].pack("s")
    output.write [headerArray.search( @pos_type, @id )].pack("I")

    if @pos_type == EVENTDATA_POS_TYPE_GRID
      gx,gz = calc_grid_ofs @gx, @gz
      output.write [headerArray.search( gx, @id )].pack("S")
      output.write [headerArray.search( gz, @id )].pack("S")
      fx32_y = @y << 12;
      output.write [headerArray.search( fx32_y, @id )].pack("i")
    else
      output.write [headerArray.search( @rail_index, @id )].pack("S")  #index
      output.write [headerArray.search( @rail_front, @id )].pack("S")  #front
      output.write [headerArray.search( @rail_side, @id )].pack("s")   #side
      output.write [0].pack("S")
    end
  end

  def dumpHeader output
    output.printf( "\#define %-32s  %2d\n", @id, @number)
  end

  def getIncludeHeader header

    #fldmmdl_objcode.h
    header.push( "../../../../prog/arc/fieldmap/fldmmdl_objcode.h " )

    #fldmmdl_code.h
    header.push( "../../../../prog/src/field/fldmmdl_code.h " )

    #field_dir
    header.push( "../../../../prog/include/field/field_dir.h " )

    #fldmmdl
    header.push( "../../../../prog/src/field/fldmmdl.h " )

    #flag_define.h
    header.push( "../../flagwork/flag_define.h " )

    header.push( "../../../../prog/include/field/eventdata_type.h " )

    #script
    header.push( "../../script/#{@zonename}_def.h " )
  end
end

#============================================================================
#
# POS�C�x���g�N���X
#
#============================================================================
class PosEvent < AllEvent

  attr :number
  attr :pos_id
  attr :event_id
  attr :check_type
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


  def initialize lines, header, zonename
    super
    @number = readEventNumber( lines )
    @pos_id = read( lines, /#Pos Event Label/)
    @event_id = read(lines, /#Pos Script Name/)
    if lines[0] =~ /#Pos Check Type/ then
      @check_type = read( lines,/#Pos Check Type/ )
    else
      @check_type = "POS_CHECK_TYPE_NORMAL"
    end
    @workname = read(lines, /#Event Trigger Work Name/)
    @workvalue = read(lines, /#Event Trigger Work Value/)
    
    if isPosType( lines ) == false
      #���́APOSTYPE���o�͂���Ȃ��̂ŁA�������Ă���
      @pos_type = EVENTDATA_POS_TYPE_GRID
      @sizex = read(lines, /#Pos Event Size X/)
      @sizez = read(lines, /#Pos Event Size Z/)
      @x, @y, @z = readPosition(lines)
    else
      @pos_type = read(lines, /#Pos Type/)
      if @pos_type == EVENTDATA_POS_TYPE_GRID
        #@sizex = read(lines, /#Pos Event Size X/)
        #@sizez = read(lines, /#Pos Event Size Z/)
        @x, @y, @z = readPosition(lines)
        @sizex, @sizez = readSize(lines)
      else
        @rail_index, @rail_front, @rail_side = readPosition(lines)
        @rail_front_size, @rail_side_size = readSize(lines)
      end
    end
  end

  def dumpBinary output,headerArray

    output.write [headerArray.search( @event_id, @pos_id )].pack("S")
    output.write [headerArray.search( @workvalue, @pos_id )].pack("S")
    output.write [headerArray.search( @workname, @pos_id )].pack("S")
    output.write [headerArray.search( @check_type, @pos_id )].pack("S")
    output.write [headerArray.search( @pos_type, @pos_id )].pack("S")

    if @pos_type == EVENTDATA_POS_TYPE_GRID
      gx,gz = calc_grid_ofs @x, @z
      output.write [headerArray.search( gx, @pos_id )].pack("S")
      output.write [headerArray.search( gz, @pos_id )].pack("S")
      output.write [headerArray.search( @sizex, @pos_id )].pack("S")
      output.write [headerArray.search( @sizez, @pos_id )].pack("S")
      output.write [headerArray.search( @y, @pos_id )].pack("s")
      output.write [0].pack("S")
    else

      output.write [headerArray.search( @rail_index, @pos_id )].pack("S")     #index
      output.write [headerArray.search( @rail_front, @pos_id )].pack("S")     #front grid
      output.write [headerArray.search( @rail_side, @pos_id )].pack("s")     #side grid
      output.write [headerArray.search( @rail_front_size, @pos_id )].pack("S")     #front_siz
      output.write [headerArray.search( @rail_side_size, @pos_id )].pack("S")     #side_siz
      output.write [0].pack("S")
    end

  end

  def dumpHeader output
    output.printf("\#define %-32s %2d\n", @pos_id, @number)
  end

  def getIncludeHeader header
    #work_define.h
    header.push( "../../flagwork/work_define.h " )

    #eventdata_type.h
    header.push( "../../../../prog/include/field/eventdata_type.h " )

    #script
    header.push( "../../script/#{@zonename}_def.h " )
  end
end

#============================================================================
#
# BG�C�x���g�N���X
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


  def initialize lines, header, zonename
    super
    @number = readEventNumber( lines )
    @bg_id = read( lines, /#Bg Event Label/)
    @bg_type = read(lines, /#Bg Event Type/)
    @bg_dir = read(lines, /#Bg Event Reaction Direction ID/)
    @event_id = read(lines, /#Bg Script Name/)

    if isPosType( lines ) == false
      #���́APOSTYPE���o�͂���Ȃ��̂ŁA�������Ă���
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

  def dumpBinary output,headerArray


    output.write [headerArray.search( @event_id, @bg_id )].pack("S")
    output.write [headerArray.search( @bg_type, @bg_id )].pack("S")
    output.write [headerArray.search( @bg_dir, @bg_id )].pack("S")
    output.write [headerArray.search( @pos_type, @bg_id )].pack( "S" ) #�|�W�V�����^�C�v

    if @pos_type == EVENTDATA_POS_TYPE_GRID
      gx,gz = calc_grid_ofs @x, @z
      output.write [headerArray.search( gx, @bg_id )].pack("i")
      output.write [headerArray.search( gz, @bg_id )].pack("i")
      output.write [headerArray.search( @y, @bg_id )].pack("i")
    else
      output.write [headerArray.search( @rail_index, @bg_id )].pack("S")     #index
      output.write [headerArray.search( @rail_front, @bg_id )].pack("S")     #front grid
      output.write [headerArray.search( @rail_side, @bg_id )].pack("s")     #side grid
      output.write [0].pack("S")
      output.write [0].pack("S")
      output.write [0].pack("S")
    end
  end

  def dumpHeader output
    output.printf("\#define %-32s %2d\n", @bg_id, @number)
  end

  def getIncludeHeader header

    #eventdata_type.h
    header.push( "../../../../prog/include/field/eventdata_type.h " )

    #script
    header.push( "../../script/#{@zonename}_def.h " )
  end
end

#============================================================================
#
#   �C�x���g��ޖ��̂܂Ƃ܂�������N���X
#
#============================================================================
#------------------------------------------------------------------------------
# �h�A�C�x���g�f�[�^�N���X
#------------------------------------------------------------------------------
class DoorEventData < EventData

  def initialize stream, section_name, header
    super
    @doors = Array.new
    @items.each{|item|
      lines = item.split(/\r\n/)
      @doors << DoorEvent.new(lines, header, @zonename)
    }
  end

  def dumpBinary output,headerArray
    if @doors.length == 0 then
      return
    end
    @doors.each{|door| door.dumpBinary(output,headerArray) }
  end

  def dumpHeader output
    incguard = "ZONE_#{@zonename.upcase}_EVD_H"
    output.puts "//���̃t�@�C����#{@path}���玩����������܂���"
    output.puts "#ifndef #{incguard}"
    output.puts "#define #{incguard}"
    output.puts ""
    @doors.each{|item| item.dumpHeader( output ) }
    output.puts "\n#endif //#{incguard}"
  end

  def getIncludeHeader header
    if @doors.length == 0 then
      return
    end

    @doors.each{|door| door.getIncludeHeader( header ) }
  end

end


#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
class ObjEventData < EventData

  def initialize stream, section_name, header
    super
    @objs = Array.new
    @items.each{|item|
      lines = item.split(/\r\n/)
      @objs << ObjEvent.new(lines, header, @zonename)
    }
  end

  def dumpBinary output,headerArray
    if @objs.length == 0 then return end
    @objs.each{|obj| obj.dumpBinary(output, headerArray) }
  end

  def dumpHeader output
    incguard = "ZONE_#{@zonename.upcase}_EVC_H"
    output.puts "//���̃t�@�C����#{@path}���玩����������܂���"
    output.puts "#ifndef #{incguard}"
    output.puts "#define #{incguard}"
    output.puts ""
    @objs.each{|item| item.dumpHeader( output ) }
    output.puts "\n#endif //#{incguard}"
  end

  def getIncludeHeader header
    if @objs.length == 0 then
      return
    end
    @objs.each{|obj| obj.getIncludeHeader( header ) }
  end

end

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
class BgEventData < EventData

  def initialize stream, section_name, header
    super
    @bgs = Array.new
    @items.each{|item|
      lines = item.split(/\r\n/)
      @bgs << BgEvent.new(lines, header, @zonename)
    }
  end

  def dumpBinary output,headerArray
    if @bgs.length == 0 then return end
    @bgs.each{|bg| bg.dumpBinary(output, headerArray) }
  end

  def dumpHeader output
  end

  def getIncludeHeader header
    if @bgs.length == 0 then
      return
    end
    @bgs.each{|bg| bg.getIncludeHeader( header ) }
  end
end

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
class PosEventData < EventData

  def initialize stream, section_name, header
    super
    @poss = Array.new
    @items.each{|item|
      lines = item.split(/\r\n/)
      @poss << PosEvent.new(lines, header, @zonename)
    }
  end

  def dumpBinary output,headerArray
    if @poss.length == 0 then return end
    @poss.each{|pos| pos.dumpBinary(output, headerArray) }
  end

  def dumpHeader output
  end

  def getIncludeHeader header
    if @poss.length == 0 then
      return
    end
    @poss.each{|pos| pos.getIncludeHeader( header ) }
  end
end

#============================================================================
#
# �R���o�[�g�{��
#
#============================================================================
#------------------------------------------------------------------------------
# �w�b�_�[�t�@�C���ƈˑ��t�@�C���̐���
#------------------------------------------------------------------------------
def make_depend( filename )
  begin

    ofilename = "";
    
    File.open( filename ){|file|
      header = EventHeader.new(file)
      obj_events = ObjEventData.new(file, "OBJ_EVENT", header)
      bg_events = BgEventData.new(file, "BG_EVENT", header)
      pos_events = PosEventData.new(file, "POS_EVENT", header)
      door_events = DoorEventData.new(file, "DOOR_EVENT", header)


      zonename = File.basename(filename,".*").downcase
      ofilename = "../tmp/" + zonename

      File.open("#{ofilename}.h", "w"){|file|
        door_events.dumpHeader(file)
        obj_events.dumpHeader(file)
        bg_events.dumpHeader(file)
        pos_events.dumpHeader(file)
      }


      #�ˑ��֌W
      depend_header = Array.new

      depend_header.push( "$(UNIQ_SCRIPTHEADER) " )

      depend_header.push( "../tmp/#{zonename}.h " )

      door_events.getIncludeHeader( depend_header )
      obj_events.getIncludeHeader( depend_header )
      bg_events.getIncludeHeader( depend_header )
      pos_events.getIncludeHeader( depend_header )

      #�d�����Ȃ���
      depend_header = depend_header.uniq()

      File.open( "#{ofilename}.tmp", "w" ){|file|
        file.puts "##{ARGV[0]}"
        file.puts "#{ofilename}.bin:#{ARGV[0]} #{depend_header}"
        file.puts "\t@echo #{ARGV[0]}"
        file.puts "\t@ruby make_binary.rb $< #{depend_header}"

        file.puts ""
      }

    }
  rescue => errors
    p errors
    #��O��
    if File.exist?( "#{ofilename}.tmp" )
      File.delete( "#{ofilename}.tmp" );
    end
    exit(1)
  else
  end

end

#------------------------------------------------------------------------------
# �o�C�i���t�@�C���̐���
#------------------------------------------------------------------------------
def make_binary( filename, allHeader )
  begin
    
    headerArray = HeaderDataArray.new();

    ofilename = "";
    
    #�K�v�w�b�_�[�̓ǂݍ���
    allHeader.each{|headername|
      headerArray.load( headername )
    }
    
    File.open( filename ){|file|
      header = EventHeader.new(file)
      obj_events = ObjEventData.new(file, "OBJ_EVENT", header)
      bg_events = BgEventData.new(file, "BG_EVENT", header)
      pos_events = PosEventData.new(file, "POS_EVENT", header)
      door_events = DoorEventData.new(file, "DOOR_EVENT", header)

      zonename = File.basename(filename,".*").downcase
      ofilename = "../tmp/" + zonename

      #headerArray.load( "../../script/#{zonename}_def.h" ) Door�C�x���g�݂̂�mev�̏ꍇ�A�X�N���v�g�͊֌W�Ȃ��̂ŁE

      File.open("#{ofilename}.bin", "wb"){|file|

        file.write [bg_events.items.length].pack("C")
        file.write [obj_events.items.length].pack("C")
        file.write [door_events.items.length].pack("C")
        file.write [pos_events.items.length].pack("C")

        bg_events.dumpBinary(file, headerArray)
        obj_events.dumpBinary(file, headerArray)
        door_events.dumpBinary(file, headerArray)
        pos_events.dumpBinary(file, headerArray)
      }
    }

  rescue => errors
    p errors
    #��O��
    if File.exist?( "#{ofilename}.bin" )
      File.delete( "#{ofilename}.bin" );
    end
    exit(1)
  else
  end
end


