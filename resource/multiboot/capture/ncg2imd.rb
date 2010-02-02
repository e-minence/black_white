require '../../../tools/ncg2imd/ncg_parser'
require '../../../tools/ncg2imd/ncl_parser'
require '../../../tools/ncg2imd/rewrite_imd'

def GetFileName(pathfile)
	last = pathfile.rindex("/")
	
	file = pathfile.dup
  if( last != nil )
  	file[0,last+1] = ""
  end
	return file
end

def GetAddPath(path,word)
	size = path.size
	last = path.rindex(word)
  if( last != nil )
  	del_size = size-last
  	add_path = path.dup
  	add_path[last,del_size] = ""
  else
    add_path = "."
  end
	return add_path
end

def FixPalText(inPaletteData, ary)
	i = 0
	while i < 2
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
					#�X�y�[�X�����܂���
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

#�R�}���h����
#NCG�t�@�C���A�o�͐�
ncg_file_name = ARGV.shift
output_path = ARGV.shift

ncg_parser = NCG_STRUCT.new
ncg_parser.Pars(ncg_file_name)
#���΃p�X�̕����𒊏o
add_path = GetAddPath(ncg_file_name,"/")

char_data = ncg_parser.GetCharData
ncg_width = ncg_parser.GetWidth
ncg_height = ncg_parser.GetHeight

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

tex_num_max = 1

while tex_num < tex_num_max
	tex_data = []
	floor_ofs = floor*ncg_height*ncg_width*byte_par_char
	pat_ofs = pat*ncg_width*byte_par_char
	
	height = 0
	#1�R�}����
	while height < ncg_height
		height_ofs = height*ncg_width*byte_par_char
		width = 0
		while width < ncg_width
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
	if ((pat+1)*ncg_width > ncg_width) then
		pat = 0
		floor+=1
	end
	
	tex_num += 1
	tex_data_array << tex_data
end

#�e�N�X�`���A���C�����߂���IMD�ɓK�p�ł���`�ɉ��H����
maker = []

tex_data_array.each { | tex |
	fix_data_maker = FIX_TEX_DATA_MAKER.new(ncg_width, ncg_height)
	fix_data_maker.PushData(tex)
	maker << fix_data_maker
}

#maker.each { | i |  i.Dump}
ncl_parser = NCL_STRUCT.new(1)

ncl_parser.Pars(add_path + "/" + ncg_parser.GetPalFileName)

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
if (ncg_width == 4 && ncg_height == 4) then
	template = "dummy_32x32.imd"
elsif (ncg_width == 8 && ncg_height == 4) then
	template = "dummy_64x32.imd"
elsif (ncg_width == 16 && ncg_height == 4) then
	template = "dummy_128x32.imd"
elsif (ncg_width == 32 && ncg_height == 4) then
	template = "dummy_256x32.imd"
elsif (ncg_width == 4 && ncg_height == 8) then
	template = "dummy_32x64.imd"
elsif (ncg_width == 4 && ncg_height == 16) then
	template = "dummy_32x128.imd"
elsif (ncg_width == 16 && ncg_height == 16) then
	template = "dummy_128x128.imd"
else
  printf( "w[%d] h[%d]\n",ncg_width,ncg_height )
	p "OutputFailed!!"
	return
end

#org_imd_path = GetAddPath(add_path,/tsure_data/)
org_imd_path = "./"
#rewrite.OutPut(org_imd_path + template, output_path + "/" + output_file_name)
rewrite.OutPut(org_imd_path + template, output_path + output_file_name)

