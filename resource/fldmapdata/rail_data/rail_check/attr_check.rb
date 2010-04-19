
#!/usr/bin/ruby

################################################
#
# AttrCheck�@�A�g���r���[�g�������`�F�b�N
#
# ruby attr_check.rb pokewb.3at rail�t�H���_�[(../rail/)
#
################################################

ATTR_CHECK_RET_ERR = 0
ATTR_CHECK_RET_ERR_BIT = 1
ATTR_CHECK_RET_ERR_VALUE = 2
ATTR_CHECK_RET_ERR_COLOR = 3
ATTR_CHECK_RET_ERR_COMMENT = 4


ATTR_DATA_INIT_TYPE_3AT = 0
ATTR_DATA_INIT_TYPE_RAIL = 1

def debug_puts str

  #puts str
  
end


### �A�g���r���[�g�N���X
class AttrData
  attr :bit
  attr :value
  attr :color
  attr :comment


  def initialize str, init_type

    str = str.sub(/\r\n/, "")
    str = str.sub(/\n/, "")

    if init_type == ATTR_DATA_INIT_TYPE_3AT then
      datas = str.split( /\s/ )

      debug_puts "add 3at data"+str

      @bit = datas[ 2 ]
      @value = datas[ 3 ]
      @color = datas[ 1 ]
      @comment = datas[ 4 ]
    else
      str = str.sub( /::/, "\s" )
      datas = str.split( /\s/ )

      debug_puts "add rail data"+str

      @bit = datas[ 2 ]
      @value = datas[ 3 ]
      @color = datas[ 1 ]
      @comment = datas[ 4 ]
    end
    
  end

  def checkComment check_com
  
    if @comment == check_com then
      return TRUE
    else
      return FALSE
    end
  end

  def checkAttr bit, value, color
    ret = [0,0,0,0,0]

    #�r�b�g���Ⴄ
    if bit != @bit then
      ret[ATTR_CHECK_RET_ERR_BIT] = 1
      ret[ATTR_CHECK_RET_ERR] = 1
    end

    #�o�����[���Ⴄ
    if value != @value then
      ret[ATTR_CHECK_RET_ERR_VALUE] = 1
      ret[ATTR_CHECK_RET_ERR] = 1
    end

    #�F�ݒ肪�Ⴄ
    if color != @color then
      ret[ATTR_CHECK_RET_ERR_COLOR] = 1
      ret[ATTR_CHECK_RET_ERR] = 1
    end
    
    return ret
  end
  
end

class AttrArray
  attr :attr_buf

  def initialize filedata

    @attr_buf = Array.new
    filedata.each{|line|
      if line.index( "galv" ) != nil then
        @attr_buf << AttrData.new( line, ATTR_DATA_INIT_TYPE_3AT ) 
      end
    }
  end


  def checkAttr value, bit, comment, color

    @attr_buf.each{|obj|
      if obj.checkComment( comment ) then
        debug_puts("#{comment} check")
        return obj.checkAttr( bit, value, color )
      end
    }

    debug_puts("comment hit")
    return [1,0,0,0,1]#comment���݂���Ȃ�
  end
  
end


### RailLine�N���X
class RailLine

  attr :gridx
  attr :gridz
  attr :attr_buf
  attr :linename

  def initialize filedata

    datain = 0
    count = 0

    @attr_buf = Array.new

    while (line = filedata.gets)

      #���C�����擾
      if datain == 0 then
        if line.index("--NAME") == nil then
          #�G���[
          puts("Data Format Error --Name")
          exit(1)
        end

        @linename = line.sub( /--NAME::/, "" )
        datain = 1
        
      #X�T�C�Y�擾
      elsif datain == 1 then

        @gridx  = line.sub( /--X::/,"" )
        datain = 2
        
      #Z�T�C�Y�擾
      elsif datain == 2 then

        @gridz  = line.sub( /--X::/,"" )
        datain = 3
        
      else

        if line.index( "--LINEATTR::" ) != nil then
          @attr_buf << AttrData.new( line, ATTR_DATA_INIT_TYPE_RAIL ) 
          count += 1

          #�����`�F�b�N
          if count >= (@gridx.to_i * @gridz.to_i) then
            break
         end
       else
          #�G���[
          puts("Data Format Error --LINEATTR")
          exit(1)

       end
        
      end
      
    end
    
  end


  def outputAttr attrArray, output

    count = 0
    ret = [0,0,0,0,0]
    
    debug_puts( "#{@linename} check..." )
    output.printf( " %s check...\n", @linename )
    
    @attr_buf.each{|obj|
      
      ret = attrArray.checkAttr( obj.value, obj.bit, obj.comment, obj.color )
      
      #�G���[�H
      if ret[ATTR_CHECK_RET_ERR] == 1 then

        index_x = (count % @gridx.to_i)
        index_z = (count / @gridx.to_i)
        output.printf( "  attrerr gridx[#{index_x}] gridz[#{index_z}] comment[#{obj.comment}] " )

        debug_puts( "#{ret}" )

        if ret[ATTR_CHECK_RET_ERR_COMMENT] == 1 then
          debug_puts( "  comment #{obj.comment} ���݂���܂���B" )
          output.printf( "  comment #{obj.comment} ���݂���܂���B" )

        else

          if ret[ATTR_CHECK_RET_ERR_BIT] == 1 then
            debug_puts( "  bit #{obj.bit} �ɈႢ������܂��B" )
            output.printf( "  bit #{obj.bit} �ɈႢ������܂��B" )
          end

          if ret[ATTR_CHECK_RET_ERR_VALUE] == 1 then
            debug_puts( "  value #{obj.value} �ɈႢ������܂��B" )
            output.printf( "  value #{obj.value} �ɈႢ������܂��B" )
          end

          if ret[ATTR_CHECK_RET_ERR_COLOR] == 1 then
            debug_puts( "  color #{obj.color} �ɈႢ������܂��B" )
            output.printf( "  color #{obj.color} �ɈႢ������܂��B" )
          end

        end
        
        output.printf( "\n" )

      end

      count += 1
    }


  end
  

end

### Rail�t�@�C���N���X
class FileRailData

  attr :line
  attr :filename

  def initialize filedata, filename

    @filename = filename

    @line = Array.new
    while line = filedata.gets
      if line.index( "+LINE" ) != nil then
        @line << RailLine.new( filedata ) 
      end
    end

  end


  def outputAttr attrArray, output

    
    output.printf( "---------------------\n" )
    output.printf( "�� %s check...\n", @filename )

    @line.each{|obj|
      obj.outputAttr( attrArray, output )
    }
    
    output.printf( "---------------------\n" )
  end
  
end


def outputRail dir, output, attrArray
  dir = dir.sub(/\/$/, '') # �Ō�� / ������Ύ��
  
  Dir::foreach(dir){ |filename|
    if filename == '..'
      next
    end
    if filename == '.'
      next
    end
    if filename == '.svn'
      next
    end
    if filename =~ /\.attr$/
      debug_puts("#{dir}/#{filename}..")
      File.open( "#{dir}/#{filename}" ){|file|
        fileRail = FileRailData.new( file, "#{dir}/#{filename}" )
        
        fileRail.outputAttr( attrArray, output )
      }
    end
    if true == FileTest::directory?(dir + '/' + filename)
      debug_puts "dir  #{filename}..."
      outputRail( dir + '/' + filename , output, attrArray) #�ċA�I�Ƀ`�F�b�N
    end
  }
end


### ���C��
begin

  attrArray = nil

  #3at��ǂݍ��ށB
  File.open( ARGV[0] ){|file|
    attrArray = AttrArray.new( file )
  }

  File.open( "attr_check_result.txt", "w" ){|ofile|
    
    outputRail( ARGV[1], ofile, attrArray )
  }
  
    
  
rescue => errs
  p errs
  exit(1)
else
end
  

