#!ruby -Ks
#!/usr/bin/ruby
#
# CGEAR�p�^�[�����f�[�^��
# 2010.03.18 k.ohno
#
#

require "jcode"
require 'nkf'



PANEL_X = 9
PANEL_Y = 7

OUTPUT_PANEL_X = 9
OUTPUT_PANEL_Y = 4

def DEBUG_Puts str 
  #puts str;
end

class PanelPattern
  
  def initialize(str)
    @PatternName = str
    @ArrayData=[]
    @HashPattern={"��"=>1, "��"=>2, "��"=>3, "��"=>4, "����"=>1+128, "����"=>2+128, "����"=>3+128, "��"=>8, } #�o�͎���8��0�ɏ��������܂��B
  end
  
  
  def panelRead(line, no)
    
    buffer = line.split(',')
    count = 0
    buffer.each{| pattern |

      
      if  count < PANEL_X 
        #�����̏���ۑ�
        if @HashPattern[ pattern ] != nil 
          @ArrayData[ no*PANEL_X + count ] = @HashPattern[ pattern ]
        else
          @ArrayData[ no*PANEL_X + count ] = 0
        end
        count += 1
      end
    }
    
  end
  
  #�c�����ɂ߂�
  def packData
  
    for x in 0..(PANEL_X-1)

      for y in 0..(PANEL_Y-1)
        DEBUG_Puts( "x=#{x}  y=#{y}" )

        if @ArrayData[ (y*PANEL_X) + x ] == 0
          #�f�[�^���߂�B
          for get_y in (y+1)..(PANEL_Y-1)
            if @ArrayData[ (get_y*PANEL_X) + x ] != 0
              DEBUG_Puts( "�߂� y=#{get_y}" )
              break
            end
          end

          #�߂�B
          if y != get_y
            DEBUG_Puts( "before = #{@ArrayData[ (y*PANEL_X) + x ]} after = #{@ArrayData[ (get_y*PANEL_X) + x ]}" )
            @ArrayData[ (y*PANEL_X) + x ] = @ArrayData[ (get_y*PANEL_X) + x ]
            @ArrayData[ (get_y*PANEL_X) + x ] = 0
          end
        end
      end
        
    end
    
  end
  
  #�o�͂���B
  def outPutBinary

    output = ""

    for x in 0..OUTPUT_PANEL_X-1
      for y in 0..OUTPUT_PANEL_Y-1
        data = @ArrayData[ (y*OUTPUT_PANEL_X)+x ]
        DEBUG_Puts( "x=#{x} y=#{y} data= #{data}" )
        if @ArrayData[ (y*OUTPUT_PANEL_X)+x ] == 8
          output += [ 0 ].pack("C")
        else
          output += [ @ArrayData[ (y*OUTPUT_PANEL_X)+x ] ].pack("C")
        end
      end
      
    end

    File.open(@PatternName+".bin", "wb"){|file|
      file.write output
    }
  end
  
  
  
end

begin
  
  
  fh = File.new(ARGV[0],"r")
  loopflg = 0
  dep = PanelPattern.new("pattern0")
  
  fh.each{ |line|
    if line =~ /^pattern(\d+)/
      dep = PanelPattern.new("pattern"+$1)
      loopflg = 0
    elsif loopflg < 7
      dep.panelRead( line ,loopflg)
      loopflg=loopflg+1

      if loopflg == 7

        #�o��
        dep.packData()
        dep.outPutBinary()
      end
        
    end
  }
  exit(0)
end
