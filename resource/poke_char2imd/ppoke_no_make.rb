#�A������|�P�����̃O���t�B�b�N����肷�����dat�t�@�C���Ƃ��ďo�͂���

$KCODE = "SJIS"

csv_file = open(ARGV.shift,"r")
dat_file = open("pair_poke_idx.dat","w")

line = csv_file.gets
line = csv_file.gets

val = 0;
#�n�b�V���쐬
hash = Hash.new
form_hash = Hash.new
sexdif_hash = Hash.new
while line = csv_file.gets
	form_flg = 0;
	column = line.split ","
	name = column[1]
	#�|�P�������́u�E�v�܂ł��擾
	m = /�E/.match("#{name}")
	if m != nil then
		poke_name = m.pre_match
		form_flg = 1;
	else
		#�|�P�������́u�^�v�܂ł��擾
		m = /�^/.match("#{name}")
		if m != nil then
			poke_name = m.pre_match
		else
			poke_name = name
		end
	end
	
	#�n�b�V���ɒǉ�
	if hash.key?(poke_name) == false then
		#�ǉ�
		hash[poke_name] = val
		form_hash[poke_name] = 0;
		sexdif_hash[poke_name] = 0;
	else
		if form_flg == 1 then
			form_hash[poke_name] = form_hash[poke_name] + 1;
		else
			sexdif_hash[poke_name] = 1;
		end
	end
	
	val+=1
end

sorted = hash.sort_by{|key,val| val}

dat_file.printf("//���̃t�@�C���̓R���o�[�^�[�ɂ��o�͂���܂�\n\n")
dat_file.printf("static const u16 PairPokeIdxTbl[] = {\n")

dat_file.printf( "\t0,\t\t//�_�~�[\n" )

sorted.each_with_index{ | i,idx |
	no = idx+1
	dat_file.printf( "\t#{i[1]},\t\t//No.#{no} #{i[0]}\n" )
}

dat_file.printf("};")


dat_file.printf("\n\n")
#�t�H��������z�񉻂���

dat_file.printf("static const u16 PairPokeFormNum[] = {\n")

sorted.each_with_index{ | i,idx |
	no = idx+1
	poke_name = i[0]
	dat_file.printf( "\t#{form_hash[poke_name]},\t\t//No.#{no} #{i[0]}\n" )
}

dat_file.printf("};")

dat_file.printf("\n\n")
#���ʂ̗L����z�񉻂���

dat_file.printf("static const u16 PairPokeSexDif[] = {\n")

sorted.each_with_index{ | i,idx |
	no = idx+1
	poke_name = i[0]
	dat_file.printf( "\t#{sexdif_hash[poke_name]},\t\t//No.#{no} #{i[0]}\n" )
}

dat_file.printf("};")
