#!/usr/bin/ruby
#[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
#
#
#	@file		  shooter_item_conv.rb
#	@brief    �V���[�^�[�A�C�e���R���o�[�^
#	@author		Toru=Nagihashi
#	@date		  2010.03.09
#
#
#]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#-------------------------------------
# �G�N�Z���R���o�[�g
#=====================================
class CsvConvert
  require "csv"

  def initialize
    @buffer

    @row_length  = 0          #�s
    @col_length = Array.new   #��
  end

  def load( filename )
    #�����擾
    CSV.open( filename, 'r' ) do |row|
      @row_length  += 1
      @col_length.push( row.size )
    end

    #�񎟌��z��
    @buffer = Array.new(@row_length)
    @buffer.each_index{ |i|
      @buffer[i] = Array.new(@col_length[i])
    }

    #�f�[�^�擾
    i = 0
    j = 0
    CSV.open( filename, 'r' ) do |row|
      row.each do |cell|
        @buffer[j][i] = cell
        i += 1
      end
      j += 1
      i = 0
    end
  end

  def get( row, col )
    return @buffer[row][col]
  end

  def row_length
    return @row_length
  end

  def col_length( row )
    return @col_length[ row ]
  end
end


#-------------------------------------
# �A�C�e���w�b�_�ǂݍ���
#=====================================
class ItemHeader
  def initialize
    @define_name    = Array.new  #��`��
    @define_number  = Array.new  #���l
    @define_item    = Array.new  #�A�C�e����
  end

  #�A�C�e���w�b�_��ǂݍ���
  def load( filename )
    File.open(filename, 'r'){|file|
      file.each{|line|
        if line =~ /#define[\s\t]+([^\s\t]+)[\s\t\(]+(\d+)[\s\t\)]+\/\/[\s\t]+(.+).+/
          #���K�\���Ƀ}�b�`�����������擾
          name = Regexp.last_match[1]
          number = Regexp.last_match[2]
          item  = Regexp.last_match[3]
          #puts name 
          #puts number
          #puts item
          @define_name.push( name ) 
          @define_number.push( number ) 
          @define_item.push( item ) 
        end
      }
    }
  end

  #�A�C�e�������L�[�ɂ��āA��`���ƒ�`���Ă��鐔�l�����o��
  def search( itemname ) 
    index = @define_item.index( itemname )
    if index == nil
      puts itemname
      puts "������Ȃ�����"
      exit(1)
    end
    return @define_name[index], @define_number[ index ]
  end
end

#-------------------------------------
# �w�b�_�쐬
#=====================================
class HeaderBuilder
  def initialize
    @buffer       = String.new
    @header_name
  end

  def write_header( header_name, brief, date, author, note )
    @header_name  = header_name.gsub(/\./,'_');
    write( "//============================================================================================\n" )
    write( "/**\n" )
    write( " * @file " + header_name + "\n" )
    if brief != nil 
      write( " * @brief " + brief + "\n" )
    end
    if date != nil 
      write( " * @author " + author + "\n" )
    end
    if author != nil 
      write( " * @date " + date + "\n" )
    end
    if note != nil
      write( " * @note " + note + "\n" )
    end
    write( "*/\n" )
    write( "//============================================================================================\n" )
    write( "#ifndef __" + @header_name.upcase + "__\n" )
    write( "#define __" + @header_name.upcase + "__\n" )
  end

  def write_fotter
    write( "#endif //__" + @header_name.upcase + "__\n" )
  end

  def write( str )
    @buffer << str
  end

  def genelate( filename )
    newfile = File.new( filename, "w" )
    @buffer.each{ |line|
      newfile.puts( line )
    }
    newfile.close
  end
end

#-------------------------------------
# ���C��
#=====================================
begin
  CREATE_HEADER_NAME  = "shooter_item_data.h"
  CREATE_CDAT_NAME  = "shooter_item_data.cdat"
  CREATE_RB_HASH_NAME  = "shooter_item_hash.rb"
  

  #�����G���[
  if ARGV[0] == nil
    puts "����1��csv�̃t�@�C�������w�肵�Ă�������" 
    exit(1)
  end
  if ARGV[1] == nil
    puts "����2��itemsym.h�̃p�X���w�肵�Ă�������" 
    exit(1)
  end
  unless FileTest.exists?(ARGV[0])
    puts "�t�@�C���̏ꏊ#{ARGV[0]}��������܂���" 
    exit(1)
  end
  unless FileTest.exists?(ARGV[1])
    puts "itemsym.h�̏ꏊ#{ARGV[1]}��������܂���" 
    exit(1)
  end

  #�G�N�Z���R���o�[�^
  csv_conv  = CsvConvert.new
  csv_conv.load( ARGV[0] )

  #�A�C�e���w�b�_�ǂݍ���
  item_header = ItemHeader.new
  item_header.load( ARGV[1] )

  #�������ݓ��e�̂��߃f�[�^�T�[�`
  #�A�C�e������define�Ɛ��l���w�b�_�֏�������
  name_buffer   = Array.new( csv_conv.row_length )
  define_buffer  = Array.new( csv_conv.row_length )
  number_buffer  = Array.new( csv_conv.row_length )
  cost_buffer  = Array.new( csv_conv.row_length )
  for i in 0..csv_conv.row_length-1
    itemname  = csv_conv.get( i, 0 )  #�A�C�e���̖��O
    cost  = csv_conv.get( i, 1 )      #�R�X�g
    define , number = item_header.search( itemname )
 
    name_buffer[ i ]    = itemname;
    define_buffer[ i ]  = define;
    number_buffer[ i ]  = number;
    cost_buffer[ i ]    = cost;
  end

  #�w�b�_��������
  header_build  = HeaderBuilder.new
  day = Time.now
  header_build.write_header( CREATE_HEADER_NAME, "�V���[�^�[�p�A�C�e���g�p���r�b�g", day.to_s, "toru=nagihashi", "���̃t�@�C����"+__FILE__+"�ɂĎ�����������Ă���܂�" )
  header_build.write( "\n" )

  #���e
  for i in 0..csv_conv.row_length-1
    header_build.write( "#define\tSHOOTER_" + define_buffer[ i ] + "\t\t(" + i.to_s + ")\n" )
  end
  header_build.write( "#define\tSHOOTER_ITEM_MAX\t\t(" + (i+1).to_s + ")\n" )
  header_build.write( "\n" )

  #�t�b�^��������
  header_build.write_fotter

  #�w�b�_�쐬
  header_build.genelate( CREATE_HEADER_NAME )



  #C�_�b�g��������
  cdat_build  = HeaderBuilder.new
  day = Time.now
  cdat_build.write_header( CREATE_CDAT_NAME, "�V���[�^�[�p�A�C�e������A�C�e���ւ̕ϊ��e�[�u��", day.to_s, "toru=nagihashi", "���̃t�@�C����"+__FILE__+"�ɂĎ�����������Ă���܂�" )
  cdat_build.write( "\n" )

  #���e
  cdat_build.write( "static const struct {\n" )
  cdat_build.write( "u16 name;\n" )
  cdat_build.write( "u16 cost;\n" )
  cdat_build.write( "} sc_shooter_item_to_item[SHOOTER_ITEM_MAX] = {\n" )
  for i in 0..csv_conv.row_length-1
    cdat_build.write( "\t{#{number_buffer[i]}, #{cost_buffer[i]} }, //#{name_buffer[i]}\n" )
  end
  cdat_build.write( "};\n" )
  cdat_build.write( "\n" )

  #�t�b�^��������
  cdat_build.write_fotter

  #�_�b�g�쐬
  cdat_build.genelate( CREATE_CDAT_NAME )


  #�V���[�^�[�A�C�e���n�b�V����������
  rbhash_build  = HeaderBuilder.new
  
  rbhash_build.write( "#! ruby -Ks\n" )
  rbhash_build.write( "\n" )
  rbhash_build.write( "$shooter_item_hash = {\n" )
  for i in 0..csv_conv.row_length-1
    rbhash_build.write( "\t\"#{name_buffer[i]}\"=>#{number_buffer[i]},\n" )
  end
  rbhash_build.write( "}\n" )

  #�V���[�^�[�A�C�e���n�b�V���쐬
  rbhash_build.genelate( CREATE_RB_HASH_NAME )

end
