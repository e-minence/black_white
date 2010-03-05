#
#   �������@�팸�̂��߂̃R���o�[�^�[
# 
#  �A�g���r���[�g�������@*.bin��
#   �@���AD�l�e�[�u���ƃe�[�u���C���f�b�N�X�̏���2�ɕ�������B
#
#�@�N�����Ƀ��[�h��n�����ƂŁA�������ς��܂��B
#
# .bin���璸�_�e�[�u���𐶐�
#   ruby wb_bin_conv.rb TBL .bin 
#
# ���ׂĂ�.tbl���}�[�W��.cdat�쐬
#   ruby wb_bin_conv.rb CDAT outfile
#
# .alltbl���g�p���A�C���f�b�N�X�`���̍��������o��
#   ruby wb_bin_conv.rb BIN .bin
#
# .alltbl���g�p���A�C���f�b�N�X�`���̍��������o�� ���̌���
#   ruby wb_bin_conv.rb CROSSBIN .bin _ex.bin
#

class OutPutError < Exception; end
class ConvertError < Exception; end
class FormatError < Exception; end

#============================================================================
# �f�o�b�N�o��
#============================================================================
def debug_puts str
  #puts str
end



#============================================================================
# �萔��
#============================================================================

#�f�[�^�t���O
WB_NORMALVTX_TRYANGLE_ONE=0         ## �O�p�`�P�̃f�[�^
WB_NORMALVTX_TRYANGLE_TWO=1         ## �O�p�`�Q�̃f�[�^
WB_NORMALVTX_TRYANGLE_TWO_INDEX=2   ## �O�p�`�Q�̃f�[�^�̃C���f�b�N�X�i�T�C�Y��WB_NORMALVTXST_TR1�ƈꏏ�j
WB_NORMALVTX_TRYANGLE_DATA_NONE=3   ## �O�p�`�Q�̃f�[�^�̃C���f�b�N�X�i�T�C�Y��WB_NORMALVTXST_TR1�ƈꏏ�j

#�t�H���_��
TMPFOLDER = "bintmp"
OUTFOLDER = "binout"


#============================================================================
# �N���X�錾
#============================================================================

#=================================================
#�@���ED�l�@�e�[�u�����
#=================================================
class NORMAL_D_TBL
  attr:header
  attr:normalbuff
  attr:dbuff

  def initialize file
    if file == nil then
      @header = Array.new
      @header[0] = 0
      @header[1] = 0

      @normalbuff = Array.new
      @dbuff = Array.new
    else
      loadFile( file )
    end
  end

  def loadFile file
    @header  = file.read(4).unpack( "SS" )

    debug_puts("tbl normal=#{@header[0]} d=#{@header[1]}")

    @normalbuff = Array.new
    for i in (0 ... @header[0]) do
      vec = file.read( 6 ).unpack("SSS")
      @normalbuff << vec
      debug_puts ("load #{vec.length}")
      debug_puts ("x#{vec[0]} y#{vec[1]} z#{vec[2]}")
    end

    size = @header[1] * 4
    @dbuff = file.read( size ).unpack( "I#{@header[1]}" )
  end

  def setNormal x, y, z

    #�Ȃ���Γ����
    result = isNormalIn( x, y, z )
    if result[0] == false then
      #�o�^
      @normalbuff << [x, y, z]
      debug_puts ("add #{[x, y, z].length}")
      debug_puts ("x#{x} y#{y} z#{z}")

      @header[0] += 1
    end
  end

  def setD d
    result = isDIn( d )
    if result[0] == false then
      #�o�^
      @dbuff << d
      @header[1] += 1
    end
  end


  #�C���f�b�N�X�̎擾
  def getNormalIndex x, y, z

    result = isNormalIn( x, y, z )
    if result[0] == false then
      puts( "alltbl �� #{x} #{y} #{z}�̖@�����Ȃ�" )
      raise ConvertError
    end
    return result[1]
  end

  def getDIndex d

    result = isDIn( d )
    if result[0] == false then
      puts( "alltbl �� #{d}�̕��ʂ̈ʒu���Ȃ�" )
      raise ConvertError
    end

    return result[1]
  end

  #binary�o��
  def outputBinary
    output = ""

    output = ""
    #�܂��A���ꂼ��̃T�C�Y
    output += [@header[0]].pack("S")
    output += [@header[1]].pack("S")


    #�X�̏��
    @normalbuff.each{|item|
      vec = item
      debug_puts ("x #{vec[0]} y #{vec[1]} z#{vec[2]}")
      output += [vec[0]].pack("S")
      output += [vec[1]].pack("S")
      output += [vec[2]].pack("S")
    }
    @dbuff.each{|item|
      output += [item].pack("I")
    }
    return output
  end


  def dump
    puts "NormalTbl"
    roop_num = @header[0]
    for i in (0 ... roop_num) do
      vec = @normalbuff[i]
      puts( "  #{vec[0]}, #{vec[1]}, #{vec[2]}," )
    end

    puts "D_Tbl"
    @dbuff.each{|item|
      puts( "  #{item}," )
    }
  end

  def outputCDAT output
    
    output.printf( "#pragma once\n" )
    output.printf( "// ���̃t�@�C����resource/fldmapdata/land_data/wb_bin_conv.rb����o�͂���܂����B\n" )
    output.printf( "\n" )
    output.printf( "\n" )
    output.printf( "#define WB_NORMAL_TBL_MAX ( #{@header[0]} )\n" )
    output.printf( "#define WB_PLANE_D_TBL_MAX ( #{@header[1]} )\n" )
    output.printf( "\n" )
    output.printf( "\n" )
    output.printf( "// �@���e�[�u��\n" )
    output.printf( "const u16 WB_NORMAL_TBL[] = {\n" )
    
    roop_num = @header[0]
    for i in (0 ... roop_num) do
      vec = @normalbuff[i]
      output.printf( "  #{vec[0]}, #{vec[1]}, #{vec[2]},\n" )
    end
    
    output.printf( "};\n" )
    output.printf( "\n" )
    output.printf( "\n" )
    output.printf( "// ���ʂ̈ʒuD�e�[�u��\n" )
    output.printf( "const u32 WB_PLANE_D_TBL[] = {\n" )
    @dbuff.each{|item|
      output.printf( "  #{item},\n" )
    }
    output.printf( "};\n" )

    
  end


  #�d���@���̃`�F�b�N
  def isNormalIn x, y, z

    tryangle_count = 0
    @normalbuff.each{|item|
      if item == [x, y, z] then
        return [true, tryangle_count]
      end
      
      #�O�p�`�̐��𐔂���
      tryangle_count += 1
    }
    return [false, tryangle_count]
  end

  #�d��D�l�̃`�F�b�N
  def isDIn d

    d_count = 0
    @dbuff.each{|item|
      if item == d then
        return [true, d_count]
      end
      
      #���𐔂���
      d_count += 1
    }
    return [false, d_count]
  end
end


#=================================================
#�C���f�b�N�X�^�̖@���@D�l�@�e�[�u�����
#=================================================
class WB_BINFILE
  attr:format #WB_NORMALVTX_TRYANGLE_ONE�Ȃ�
  attr:vecN1  #�O�p�`�P�̖@��
  attr:vecN2  #�O�p�`�Q�̖@��
  attr:D1     #�O�p�`�P��D�l
  attr:D2     #�O�p�`�Q��D�l
  attr:attr_trtype
	attr:attr          ##�A�g���r���[�g�r�b�g    0-15��value 16-30��flg
	attr:tryangleType  ##�O�p�`�̌`�̃^�C�v  �^ = 0 ��:vecN1,�E:vecN2  �_ = 1 �E:vecN1,��:vecN2
  
  def initialize file
    if file == nil then
      @format = WB_NORMALVTX_TRYANGLE_DATA_NONE
      @vecN1 = 0
      @vecN2 = 0
      @D1 = 0
      @D2 = 0
      @attr_trtype = 0
      @attr = 0
      @tryangleType = 0
    else
      tmp = file.read(2).unpack("S")[0]
      @format = tmp & 0x3
      @vecN1  = (tmp>>2) & 0x3fff

      if (@format == WB_NORMALVTX_TRYANGLE_ONE) || (@format == WB_NORMALVTX_TRYANGLE_TWO_INDEX) then
        @D1 = file.read(2).unpack("S")[0]
        @attr_trtype = file.read(4).unpack("I")[0]
        @attr = @attr_trtype & 0x7fffffff
        @tryangleType = (@attr_trtype >> 31) & 0x00000001
      elsif @format == WB_NORMALVTX_TRYANGLE_TWO then
        
        @vecN2 = file.read(2).unpack("S")[0]
        @D1 = file.read(2).unpack("S")[0]
        @D2 = file.read(2).unpack("S")[0]
        @attr_trtype = file.read(4).unpack("I")[0]
        @attr = @attr_trtype & 0x7fffffff
        @tryangleType = (@attr_trtype >> 31) & 0x00000001
        
      else
      end
    end
  end



  #�f�[�^�ݒ�
  def setNormalFormat normal_index, d_index, attr_trtype
    @format = WB_NORMALVTX_TRYANGLE_ONE
    @vecN1  = normal_index
    @D1     = d_index
    @attr_trtype = attr_trtype
    @attr = @attr_trtype & 0x7fffffff
    @tryangleType = (@attr_trtype >> 31) & 0x00000001
  end

  def setTwoFormat n1_index, n2_index, d1_index, d2_index
    @format = WB_NORMALVTX_TRYANGLE_TWO
    @vecN1  = n1_index
    @vecN2  = n2_index
    @D1     = d1_index
    @D2     = d2_index
  end
  
  def setTwoIndexFormat dataindex, attr_trtype
    @format = WB_NORMALVTX_TRYANGLE_TWO_INDEX
    @D1     = dataindex
    @attr_trtype = attr_trtype
    @attr = @attr_trtype & 0x7fffffff
    @tryangleType = (@attr_trtype >> 31) & 0x00000001
  end


  def dump tag

    puts "tag#{tag}"
    puts "format #{@format}"
    puts "vecN1 #{@vecN1}"
    puts "vecN2 #{@vecN2}"
    puts "D1 #{@D1}"
    puts "D2 #{@D2}"
    puts "attr #{@attr}"
    puts "tryangleType #{@tryangleType}"
    
  end

  #binary�o��
  def outputBinary
    output = ""
    
    #�x�N�g���́A���̃e�[�u���C���f�b�N�X�ɂ���
    head  = @format | (@vecN1 << 2)
    output += [head].pack("S")
    
    if (@format == WB_NORMALVTX_TRYANGLE_ONE) || (@format == WB_NORMALVTX_TRYANGLE_TWO_INDEX) then
      
      output += [@D1].pack("S")
      output += [@attr_trtype].pack("I")
    elsif @format == WB_NORMALVTX_TRYANGLE_TWO then
      output += [@vecN2].pack("S")
      output += [@D1].pack("S")
      output += [@D2].pack("S")
    else

      puts( "WB_FORMAT_BINARY output err" )
      raise OutPutError
    end

    return output
  end
  
end

#=================================================
#�}�b�v�G�f�B�^�[����o�͂��ꂽbinary�t�@�C��
#=================================================
class MAPEDITOR_BINFILE

	attr:vecN1       ##�P�ڂ̎O�p�`�̖@���x�N�g��

	attr:vecN2       ##�Q�ڂ̎O�p�`�̖@���x�N�g��

	attr:vecN1_D       ## ax+by+cz+d =0 ��D�l
	attr:vecN2_D

  attr:attr_trtype
	attr:attr          ##�A�g���r���[�g�r�b�g    0-15��value 16-30��flg
	attr:tryangleType  ##�O�p�`�̌`�̃^�C�v  �^ = 0 ��:vecN1,�E:vecN2  �_ = 1 �E:vecN1,��:vecN2

  attr:tryangle_one  #1�̎O�p�`�ő��v���H
  

  def initialize file

    @vecN1 = file.read(6).unpack("SSS");

    @vecN2 = file.read(6).unpack("SSS");

    @vecN1_D = file.read(4).unpack("I")[0];
    @vecN2_D = file.read(4).unpack("I")[0];

    @attr_trtype = file.read(4).unpack("I")[0];
    @attr = @attr_trtype & 0x7fffffff
    @tryangleType = (@attr_trtype >> 31) & 0x00000001


    @tryangle_one = false
    if @vecN1 == @vecN2 then
       
      if @vecN1_D == @vecN2_D then
        @tryangle_one = true 
       end
       
     end
  end

  def dump tag
    puts "tag (#{tag})"
    puts "vecN1_x #{@vecN1[0]}"
    puts "vecN1_y #{@vecN1[1]}"
    puts "vecN1_z #{@vecN1[2]}"

    puts "vecN2_x #{@vecN2[0]}"
    puts "vecN2_y #{@vecN2[1]}"
    puts "vecN2_z #{@vecN2[2]}"

    puts "vecN1_D #{@vecN1_D}"
    puts "vecN2_D #{@vecN2_D}"

    puts "attr #{@attr}"
    puts "tryangleType #{@tryangleType}"
  end

  #�@���擾
  #�߂�l
  #index:0 �x�N�g���̌�
  #index:1�` x,y,z x2,y2,z2 ...
  def getNormal
    if @tryangle_one then
      return [ 1, @vecN1[0], @vecN1[1], @vecN1[2] ]
    end
    
    return [ 2, @vecN1[0], @vecN1[1], @vecN1[2], @vecN2[0], @vecN2[1], @vecN2[2] ]
  end

  #D�l�擾
  #�߂�l
  #index:0 D�l�̌�
  #index:1�` d0, d1, d2 ...
  def getD
    if @tryangle_one then
      return [ 1, @vecN1_D ]
    end
    
    return [ 2, @vecN1_D, @vecN2_D ]
  end
  
end

#=================================================
#�}�b�v�G�f�B�^�[����o�͂��ꂽbinary�ǂݍ��݃N���X
#=================================================
class MAPEDITOR_BINFILE_LOADER

  attr:header
  attr:dats

  attr:binaryTbl

  attr:wbformat_tbl
  
  #�ǂݍ���
  def initialize file

    @dats = Array.new
    @wbformat_tbl = Array.new

    @binaryTbl = NORMAL_D_TBL.new(nil)
    
    @header = file.read(4).unpack("SS")

    #�ǂݍ��݃��[�v
    debug_puts "#{@header[0]} #{@header[1]}"
    loop_num = @header[0] * @header[1]
    for i in  (0 ... loop_num) do
      @dats << MAPEDITOR_BINFILE.new( file )
    end
    
  end

  def addBinFile file
    header = file.read(4).unpack("SS")

    #�ǂݍ��݃��[�v
    debug_puts "#{header[0]} #{header[1]}"
    loop_num = header[0] * header[1]

    if header[0] != @header[0] then
      puts( "cross bin �u���b�N�T�C�Y�ُ� x=#{header[0]} z#{header[1]}" )
      raise FormatError
    end
    if header[1] != @header[1] then
      puts( "cross bin �u���b�N�T�C�Y�ُ� x=#{header[0]} z#{header[1]}" )
      raise FormatError
    end
    
    @header[1] += header[1] #Z�����Ƀf�[�^�𑫂�
    for i in  (0 ... loop_num) do
      @dats << MAPEDITOR_BINFILE.new( file )
    end
  end

  #�S�@���@����D�e�[�u���ǂݍ���
  def loadBinaryTbl file
    @binaryTbl.loadFile file
  end

  #�f�[�^��
  def getArrayNum
    return @dats.size
  end

  #�_���vAll
  def dump

    puts "BIN Dump"
    count = 0
    @dats.each{|obj|
      obj.dump(count)
      count += 1
    }

    puts "TBL Dump"
    @binaryTbl.dump

    puts "WBBIN Dump"
    count = 0
    @wbformat_tbl.each{|obj|
      obj.dump(count)
      count += 1
    }
  end

  #�e�[�u������
  def makeTbl

    @dats.each{|obj|
      normal_tbl = obj.getNormal()
      d_tbl = obj.getD()

      #�}�[�W�o�^
      #�@��
      i = 0
      for i in (0 ... normal_tbl[0]) do
        @binaryTbl.setNormal( normal_tbl[ 1+(i*3)+0 ], normal_tbl[ 1+(i*3)+1 ], normal_tbl[ 1+(i*3)+2 ] )
      end

      #D�l
      i = 0
      for i in (0 ... d_tbl[0]) do
        @binaryTbl.setD( d_tbl[1+i] )
      end
    }
  end

  #�e�[�u���o��
  def outputTbl
    debug_puts "output Tbl"
    return @binaryTbl.outputBinary
  end

  #�f�[�^���C���f�b�N�X�`���ɕύX
  def convertWbFormat alltblfile
    debug_puts("convert")

    #�S�@���AD�l�e�[�u��
    loadBinaryTbl( alltblfile )

    @wbformat_tbl.clear()

    twoArray = Array.new

    #�R���o�[�g
    @dats.each{|obj|

      wbformat_bin = WB_BINFILE.new(nil)

      if obj.tryangle_one == true then

        n1_index = @binaryTbl.getNormalIndex( obj.vecN1[0], obj.vecN1[1], obj.vecN1[2] )
        d1_index = @binaryTbl.getDIndex( obj.vecN1_D )
        
        wbformat_bin.setNormalFormat( n1_index, d1_index, obj.attr_trtype )
      else

        wbformat_bin.setTwoIndexFormat( twoArray.length, obj.attr_trtype )
        
        #�ʃo�b�t�@�ɂQ�O�p�`�f�[�^��ۑ�
        twodata = WB_BINFILE.new(nil)
        n1_index = @binaryTbl.getNormalIndex( obj.vecN1[0], obj.vecN1[1], obj.vecN1[2] )
        d1_index = @binaryTbl.getDIndex( obj.vecN1_D )
        n2_index = @binaryTbl.getNormalIndex( obj.vecN2[0], obj.vecN2[1], obj.vecN2[2] )
        d2_index = @binaryTbl.getDIndex( obj.vecN2_D )
        twodata.setTwoFormat( n1_index, n2_index, d1_index, d2_index )
        twoArray  << twodata
      end
      
      #�{�f�[�^�ɕۑ�
      @wbformat_tbl << wbformat_bin
    }
    
    #�Q�O�p�`�����I�[�ɓo�^
    twoArray.each{|obj|
      
      @wbformat_tbl << obj
    }
    
  end

  #wbformat�̏����o��
  def outputWbFormatBinary
    output = ""

    #�w�b�_�[
    output += [@header[0]].pack("S")
    output += [@header[1]].pack("S")

    @wbformat_tbl.each{|obj|
      output += obj.outputBinary()
    }

    return output
  end
end


#============================================================================
# TBL��������
#============================================================================
def make_tbl( filename )
  begin

    output = ""

    file = File.open( filename, "rb" );

    #���̓ǂݍ���
    loader = MAPEDITOR_BINFILE_LOADER.new( file )

    #�_���v
    #loader.dump
    loader.makeTbl()

    output += loader.outputTbl()

    debug_puts "ofile"
    #�o��
    ofilename = "#{TMPFOLDER}/" + File.basename(filename, ".*") + ".tbl"
    outputfile  = File.open(ofilename, "wb")
    outputfile.write output
    

  rescue => errors
    p errors
    #��O��
    exit(1)
  else
  end

end


#============================================================================
# �STBL�̃}�[�W���s��cdat�𐶐�
#============================================================================
def make_AllTbl outfile

  allTbl = NORMAL_D_TBL.new(nil)
  
  #tmp�Ȃ��̑S Tbl���}�[�W
  Dir.glob( "#{TMPFOLDER}/*.tbl" ).each{|filename|

    puts "merge #{filename}..."

    #���ǂݍ���
    file = File.open( filename, "rb" )
    tblTmp = NORMAL_D_TBL.new( file )
    file.close

    #�}�[�W
    roop_num = tblTmp.header[0]
    for i in (0 ... roop_num) do
      vec = tblTmp.normalbuff[i]
      #�@����ݒ�
      allTbl.setNormal( vec[0], vec[1], vec[2] )
    end

    for i in (0 ... tblTmp.header[1]) do
      #D�̒l��ݒ�
      allTbl.setD( tblTmp.dbuff[i] )
    end

    normal_num = allTbl.normalbuff.length
    d_num = allTbl.dbuff.length
    puts "normal num #{normal_num} d num #{d_num}"
  }

  #�S�f�[�^��CDAT���o��
  outputfile = File.open( outfile, "w" )
  allTbl.outputCDAT( outputfile )
  outputfile.close()

  #binary���o��
  output = ""
  output += allTbl.outputBinary
  outputfile  = File.open("#{TMPFOLDER}/normal_d_bin.alltbl", "wb")
  outputfile.write output
  outputfile.close()
  
end


#============================================================================
# �C���f�b�N�X�`���̃t�@�C�����o��
#============================================================================
def make_indexBinary inputfile
  begin

    output = ""


    file = File.open( inputfile, "rb" );

    alltbl = File.open( "#{TMPFOLDER}/normal_d_bin.alltbl", "rb" )

    #���̓ǂݍ���
    loader = MAPEDITOR_BINFILE_LOADER.new( file )

    #wbFORMAT�ɃR���o�[�g
    loader.convertWbFormat( alltbl )

    #�_���v
    #loader.dump

    output += loader.outputWbFormatBinary()

    debug_puts "ofile"
    #�o��
    ofilename = "#{OUTFOLDER}/" + File.basename(inputfile, ".*") + ".wbbin"
    outputfile  = File.open(ofilename, "wb")
    outputfile.write output
    

  rescue => errors
    p errors
    #��O��
    exit(1)
  else
  end
end

#============================================================================
# �C���f�b�N�X�`���̃t�@�C�����o��
#============================================================================
def make_indexCrossBinary inputfile
  begin

    output = ""

    file = File.open( inputfile[0], "rb" );
    file2 = File.open( inputfile[1], "rb" );

    alltbl = File.open( "#{TMPFOLDER}/normal_d_bin.alltbl", "rb" )

    #���̓ǂݍ���
    loader = MAPEDITOR_BINFILE_LOADER.new( file )
    loader.addBinFile( file2 )

    #wbFORMAT�ɃR���o�[�g
    loader.convertWbFormat( alltbl )

    output += loader.outputWbFormatBinary()

    debug_puts "ofile"
    #�o��
    ofilename = "#{OUTFOLDER}/" + File.basename(inputfile[0], ".*") + ".wbbin"
    outputfile  = File.open(ofilename, "wb")
    outputfile.write output
    

  rescue => errors
    p errors
    #��O��
    exit(1)
  else
  end
end

#============================================================================
# ���C������
#============================================================================

if ARGV[0] == "TBL" then
  make_tbl( ARGV[1] )
elsif ARGV[0] == "CDAT" then
  make_AllTbl( ARGV[1] )
elsif ARGV[0] == "BIN" then
  make_indexBinary( ARGV[1] )
else
  make_indexCrossBinary( [ARGV[1], ARGV[2]] )
end

exit(0)
