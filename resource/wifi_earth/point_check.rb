#!/usr/bin/ruby
#===================================================================
#
# @brief  wifi_earch ���_�`�F�b�N�c�[��
#
# @data   10.05.28
# @author genya_hosaka
# 
# @note   �������W�����݂��邩���ׂ�c�[���B�i������[0,0]�̓`�F�b�N���Ȃ��j
#
#===================================================================

# �����R�[�h�w��
$KCODE = 'SJIS'

FILE_TBL = [
"ARG.txt",
"AUS.txt",
"BRA.txt",
"CAN.txt",
"CHN.txt",
"DEU.txt",
"ESP.txt",
"FIN.txt",
"FRA.txt",
"GBR.txt",
"IND.txt",
"ITA.txt",
"JPN.txt",
"NOR.txt",
"POL.txt",
"RUS.txt",
"SWE.txt",
"USA.txt",
]

FILE_WORLD = "world.txt"

#�\���̒�`
SPos = Struct.new('Pos', :file, :name, :px, :py)

#�z���`
pos = Array.new

#��s���K�v�ȏ������W
def push_line f, pos, filename, e1, e2, e3
    cnt = 0
    f.each{ |line|
      if cnt >= 3 
        s = line.split(",")
        if ( ( s[e2] == "0" && s[e3] == "0" ) == false ) && s[e1] != "" && s[e2] != "" && s[e3] != ""
          pos.push( SPos.new( filename, s[e1], s[e2], s[e3] ) )
        end      
      end 
      cnt += 1
    }
end

puts "��v�`�F�b�N�J�n..."

#�W�v
FILE_TBL.each{ |filename|
  File.open( filename, "r"){|f|
    push_line f, pos, filename, 1, 2, 3
  }
} 
  
#world
File.open( FILE_WORLD, "r" ){|f|
  push_line f, pos, FILE_WORLD, 1, 4, 5
}

puts pos

#�z��R�s�[
pos2 = pos.dup

#�d���`�F�b�N
pos.each{ |elem|
#  puts "! >>"
#  puts elem
  
  #�Ώۂ��玩�����g������
  pos2.shift
  
  pos2.each{ |elem2|
#   p elem2
    if elem.px == elem2.px && elem.py == elem2.py
      #�d�������e�[�u�����o��
      puts  "��v >> " + 
            "<" + elem.file + ", " + elem.name + " > == " +
            "<" + elem2.file + ", "+ elem2.name + " > " +
            "[" + elem.px + ", " + elem.py + "]" 
  #   p elem
  #   p elem2
    end
  }
}

puts "��v�`�F�b�N�I���I"

