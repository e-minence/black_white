lib_path1 = File.dirname(__FILE__).gsub(/\\/,"/") + "/../ncg2imd/ncg_parser"
lib_path2 = File.dirname(__FILE__).gsub(/\\/,"/") + "/../ncg2imd/ncl_parser"
lib_path3 = File.dirname(__FILE__).gsub(/\\/,"/") + "/../ncg2imd/rewrite_imd"
require lib_path1
require lib_path2
require lib_path3

def GetFileName(pathfile)
	last = pathfile.rindex("/")
	
	file = pathfile.dup
	file[0,last+1] = ""
	p pathfile
	p last
	p file
	return file
end

def GetAddPath(path,word)
	size = path.size
	tmp = path.rindex(word)
  if tmp == nil then
    last = 0
  else
    last = tmp
  end
	del_size = size-last
	add_path = path.dup
	add_path[last,del_size] = ""
	p add_path
	return add_path
end

def FixPalText(inPaletteData, ary)
	i = 0
	while i < 1
		n = 0
		str = ""
		while n < 16
			if (n == 0)||(n==8) then
				str += "\n\t\t"
			end
			col = sprintf("%04x ",inPaletteData[i*16+n])
			str += col
			n+=1
		end
		str += "\n\t"
		ary << str
		i+=1
	end
end

class FIX_TEX_DATA_MAKER
	def initialize(w,h,flg = TRUE)
		line_num = h*8
		if flg == TRUE then
			@LineData = (0...line_num).map{ Array.new }
		else
			@LineData = []
		end
		@TexW = w		#�L�����N�^�T�C�Y�w��
		@TexH = h		#�L�����N�^�T�C�Y�w��
	end

	def PushData(inTex)
		char_no = 0
		char_floor = 0
		
		inTex.each { | char |
			space_flg = 0
			char_line = 0
			dot_count = 0
			line_ofs = char_floor*8
			char.each { | data |
				#2�h�b�g�������i16�F�̂Ƃ�����j
				dot_count+=2
	
				#���ݏ������̃��C�����v�Z
				line = line_ofs + char_line
	
				#����4�r�b�g���擾
				low = (data & 0x0f)
				#���4�r�b�g���擾
				hi = ((data>>4) & 0x0f)
				#������Ƃ��āA�f�[�^���i�[
				new_str = sprintf("%x%x",hi,low)
				#�i�[�X�y�[�X�̂���f�[�^������Ȃ�A������擾���Ă����ɃZ�b�g
				#�Ȃ��ꍇ�́A�V�K�ɍ��
				if space_flg == 1 then		#�󂫃A��
					array = @LineData[line]
					str = array.pop
					str = new_str + str
					array << str
					#�X�y�[�X�����܂���/AR
					space_flg = 0
				else						#�󂫃i�V
					array = @LineData[line]
					array << new_str
					#�X�y�[�X���ł���
					space_flg = 1
				end
	
				if dot_count%8 == 0 then
					#���C���X�V
					char_line+=1
				end
			} #end data
			char_no+=1
			#�����L�������A�w��e�N�X�`���̃L�������T�C�Y���I�[�o�[���Ă���Ώ����L�����K�w���X�V����
			if (char_no % @TexW == 0) then
				char_floor+=1
			end
		} #end char
	end

	def PushLineData(inLineData)
		@LineData << inLineData
	end

	#���]�e�N�X�`�����쐬
	def MakeReverseTex
		fix_data_maker = FIX_TEX_DATA_MAKER.new(@TexW, @TexH, FALSE)

		@LineData.each { | line |
			#���C���f�[�^���t�]�����č��
			rev = line.reverse
			new_line = Array.new(line.length,"")
			count = 0
			rev.each { | s |
				s_split = s.scan(/.{1,1}/m)
				s_rev = s_split.reverse
				new_line[count] = s_rev.join
				count+=1
			}
			fix_data_maker.PushLineData(new_line)
		}
		return fix_data_maker
	end

	def MakeText
		str = ""
		count = 0
		@LineData.each { | line |
			str += "\n\t\t\t"
			line.each { | l |
				str += l
				str += " "
			}
		}
		str += "\n\t\t"
		return str
	end
	
	def Dump
		@LineData.each { | line |
			p line
		}
	end
end

p File.dirname(__FILE__).gsub(/\\/,"/")

#�R�}���h����
#NCG�t�@�C���A�o�͐�A1�R�}�̕��i�L�����j�A1�R�}�̍����i�L�����j�A�e�N�X�`����
ncg_file_name = ARGV.shift
output_path = ARGV.shift
use_dmy_imd = ARGV.shift
one_tex_width = ARGV.shift.to_i
one_tex_height = ARGV.shift.to_i
tex_num_max = ARGV.shift.to_i

ncg_parser = NCG_STRUCT.new
ncg_parser.Pars(ncg_file_name)
#���΃p�X�̕����𒊏o
add_path = GetAddPath(ncg_file_name,"/")

char_data = ncg_parser.GetCharData
ncg_width = ncg_parser.GetWidth

tex_data_array = []

#floor:		NCG�Ń}�b�s���O����Ă���1�R�}�̃L�����f�[�^�̗���̊K�w
#			NCG�̉��T�C�Y���g���؂��Ă��A�R�}������Ȃ��ꍇ�́A
#			�܂�Ԃ��āA1�R�}�ڂ̉��i1�R�}�c64�h�b�g�������ꍇ�͏c��64�h�b�g�����������j����}�b�s���O�����
#			���̐܂�Ԃ��񐔂�floor�Ƃ���
#pat:		�����т̃R�}�ɑ΂��āA���R�}�ڂ��������Ă��邩������
#			�R�}�܂�Ԃ��ifloor�̒l��������j�����������ꍇ�Apat��0�ɖ߂�
#height:	1�R�}���̉��L������ɂ����āA����ڂ��������Ă��邩������
#width:		1�R�}���̉��L������ɂ����āA���L�����ڂ��������Ă��邩������
#byte_par_char:
#			1�L�������o�C�g��������

byte_par_char = 0
floor = 0
pat = 0

tex_num = 0

if (1) then
	byte_par_char = 32
else
	byte_par_char = 64
end

while tex_num < tex_num_max
	tex_data = []
	floor_ofs = floor*one_tex_height*ncg_width*byte_par_char
	pat_ofs = pat*one_tex_width*byte_par_char
	
	height = 0
	#1�R�}����
	while height < one_tex_height
		height_ofs = height*ncg_width*byte_par_char
		width = 0
		while width < one_tex_width
			char_dat = []
			#�L�����擾
			char_ofs = width*byte_par_char
			shift = 0
			while shift < byte_par_char
				ofs = shift + floor_ofs + height_ofs + pat_ofs + char_ofs
				char_dat << char_data[ofs]
				shift+=1
			end
			width+=1
			tex_data << char_dat
		end
		height+=1
	end
	#floor�Epat���X�V
	pat+=1
	if ((pat+1)*one_tex_width > ncg_width) then
		pat = 0
		floor+=1
	end
	
	tex_num += 1
	tex_data_array << tex_data
end

#�e�N�X�`���A���C�����߂���IMD�ɓK�p�ł���`�ɉ��H����
maker = []

tex_data_array.each { | tex |
	fix_data_maker = FIX_TEX_DATA_MAKER.new(one_tex_width, one_tex_height)
	fix_data_maker.PushData(tex)
	maker << fix_data_maker
}

#maker.each { | i |  i.Dump}
ncl_name = add_path + "/" + ncg_parser.GetPalFileName
p "search #{ncl_name}"
if File.exist?(ncl_name) == false then
  ncl_name2 = add_path + "/" + File::basename(ncg_file_name,'.ncg') + ".ncl" 
  p "Warning! #{ncl_name} don't exist. the next candidate is #{ncl_name2}"
  p "second search #{ncl_name2}"
  ncl_name = ncl_name2
end
ncl_parser = NCL_STRUCT.new(1)
ncl_parser.Pars(ncl_name)

palette_data = ncl_parser.GetPaletteData
#�p���b�g�e�L�X�g�𐮌`����
fix_pal_text_ary = Array.new(0)
FixPalText(palette_data,fix_pal_text_ary)
#�e�N�X�`���C���[�W�ƃp���b�g�f�[�^������������
tex_text_array = []
maker.each { | i |
	tex_text_array << i.MakeText
}

output_file_name = GetFileName( ncg_file_name.sub(/.ncg/,".imd") )

rewrite = REWRITE_TEX.new(tex_text_array,fix_pal_text_ary)

#�e���v���[�g��I��
template = ""
template = use_dmy_imd

rewrite.OutPut(template, output_path + "/" + output_file_name)

