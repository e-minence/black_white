#------------------------------------------------------------------------------
#
#   �G�b�W�}�[�L���O�e�[�u������
#   
#
#   2009.06.17    tamada
#
#------------------------------------------------------------------------------

OUTPUTDIR = "wkdir"
OUTPUTSYM = "edge"
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
class InputFileError < Exception; end
class InputDataError < Exception; end

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
def reader( inputfilename )

  book = Array.new
  sheet = Array.new
  book << sheet

  #book�ɑ΂��ăV�[�g���Ƃɕ��f���ĕۑ�
  File.open(inputfilename, "r"){|input_file|
    while line = input_file.gets
      column = line.split
      #�J�������Ȃ����ʃV�[�g�Ɣ���
      if column.length < 2 then
        if sheet.length > 1
          sheet = Array.new
          book << sheet
        else
          break
        end
      else
        sheet << line
      end
    end
  }
  return book
end

def getRGB(line)
  unless line =~ /\d+\s\d+\s\d/ then
    raise InputDataError, "\"#{line}\"�͐������f�[�^�ł͂Ȃ�"
  end
  column = line.split
  r = Integer(column[0])
  if r >= 32 then raise InputDataError, "R�̒l���������z���Ă��܂�" end
  g = Integer(column[1])
  if r >= 32 then raise InputDataError, "G�̒l���������z���Ă��܂�" end
  b = Integer(column[2])
  if r >= 32 then raise InputDataError, "B�̒l���������z���Ă��܂�" end
  value = Integer( r * 1024 + g * 32 + b )
  puts "R=#{r} G=#{g} B=#{b} rgb = #{sprintf("%04x",value)}"
  return value
end

def write_edgedata(wfile, sheet)
  #p sheet
  wfile.write( [ getRGB(sheet[1]) ].pack("S") )
  wfile.write( [ getRGB(sheet[2]) ].pack("S") )
  wfile.write( [ getRGB(sheet[3]) ].pack("S") )
  wfile.write( [ getRGB(sheet[4]) ].pack("S") )
  wfile.write( [ getRGB(sheet[5]) ].pack("S") )
  wfile.write( [ getRGB(sheet[6]) ].pack("S") )
  wfile.write( [ getRGB(sheet[7]) ].pack("S") )
  wfile.write( [ getRGB(sheet[8]) ].pack("S") )
end

#------------------------------------------------------------------------------
#------------------------------------------------------------------------------

begin
  arc_list = File.open("#{OUTPUTDIR}/#{OUTPUTSYM}.list", "w")
  book = reader("edgemarking.txt")
  book.each_index{|sheet_idx|
    filename = sprintf("%s/%s%02d.bin", OUTPUTDIR, OUTPUTSYM, sheet_idx + 1)
    arc_list.puts "\"#{filename}\""

    File.open(filename,"wb"){|file|
      write_edgedata(file, book[sheet_idx])
    }

  }
  arc_list.close
end





