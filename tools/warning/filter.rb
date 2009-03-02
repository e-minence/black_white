#==============================================================================#
#
#	CodeWarrior for DS�p�x���t�B���^�v���O����
#	tamada@gamefreak.co.jp
#	2009.02.12	�Ƃ肠��������
#
#==============================================================================#


#���̃t�B���^�ł̃G���[���`
class FilterError < StandardError; end


class CWWarningFilter
	def initialize
		@NORMAL_MODE = 0
		@WARN_HEADER_MODE = 1
		@WARN_CONTENT_MODE = 2

		#�����Ƀ`�F�b�N�������Ȃ��G���[�������|���鐳�K�\�����`
		@ignore_warns = [
			[false, "���ʎq�̍Ē�`", 
				Regexp::new("identifier '[a-zA-Z0-9_]+' redeclared")],
			[true, "�Öق̎Z�p�ϊ�", 
				Regexp::new("implicit arithmetic conversion from")],
			[true, "enum�ւ̈Öق̕ϊ�",
				Regexp::new("illegal implicit conversion from 'int' to\\s+\#   '\@enum")],
			[true, "�Öق̕ϊ�",
				Regexp::new("illegal implicit conversion from ")],
			[true, "���g�p�̕ϐ�/����",
				Regexp::new(/variable \/ argument '[a-zA-Z0-9_]+' is not used in function/)]
		]
		@counter = Array.new(@ignore_warns.length, 0)
	end

	def filter warn
		raise FilterError, "�x���{�̂��Ȃ�" if warn == nil
		result = false
		@ignore_warns.each_with_index{|f, idx|
			if warn =~ f[2] then
				@counter[idx] += 1
				result = true if f[0] == true
			end
		}
		return result
	end

	def put_filter_result
		@counter.each_index{|idx|
			if @counter[idx] != 0 then
				puts "#{@ignore_warns[idx][1]}:#{@counter[idx]}"
			end
		}
	end

	def make_output tfile, sfile, tpos, warn
		if tfile == nil then return end
		raise FilterError, "�x���{�̂��Ȃ�" if warn == nil
		raise FilterError, "�ꏊ���Ȃ�" if tpos == nil

		if filter(warn) == true then return end
		puts "FILE:#{tfile}"
		sfile ||= "itself"
		puts "    :#{sfile}"
		puts "#{tpos}"
		puts "#{warn}"
	end

	def warn_filter file

		tfile = nil	#target file
		sfile = nil	#sub target file
		tpos = nil	#target line position
		warn = nil	#warning explain
		mode = @NORMAL_MODE

		while line = file.gets

			case
			when line =~ /^### mwccarm.exe Compiler:/ then
				make_output(tfile, sfile, tpos, warn)
				mode = @WARN_HEADER_MODE
				tpos = nil
				warn = nil

			when line =~ /^\#    File: (.+)$/ then
				tfile = $1
				sfile = nil

			when line =~ /^\#      In: (.+)$/ then
				sfile = $1

			when line =~ /^\#    From: (.+)$/ then
				tfile = $1
				#sfile = ""

			when line =~ /^\# +\d+: (.+)$/ then
				raise FilterError, "�w�b�_�����Œ��g������" if mode != @WARN_HEADER_MODE
				mode = @WARN_CONTENT_MODE
				tpos = line

			when line =~ /^\# Warning:/ then
				raise FilterError, "�w�b�_�����Œ��g������" if mode != @WARN_CONTENT_MODE
				tpos += line

			when line =~/^\#/ then
				warn = warn ? warn + line : line

			else
			#if line =~ /^[^#]/ then
				make_output(tfile, sfile, tpos, warn)
				mode = @NORMAL_MODE
				tfile = nil
				sfile = nil
				tpos = nil
				warn = nil
			end

		end
		make_output(tfile, sfile, tpos, warn)
	end

end

###	�{��
cwf = CWWarningFilter.new

if ARGV[0] == nil then
	cwf.warn_filter $stdin
	cwf.put_filter_result
else
	File.open(ARGV[0]){|file|
		cwf.warn_filter file
		cwf.put_filter_result
	}
end

