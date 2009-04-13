require "fileutils"

def put_result name
	if FileTest.exists? name then
		puts "#{name} exists!"
	else
		dmy_file = File.dirname(name) + "/map_dummy" + File.extname(name)
		puts "#{dmy_file} --> #{name}"
		FileUtils.cp(dmy_file, name)
		#puts "#{name} not found!"
	end
end
def map_file_check prefix
	#path = "../land_res/" + prefix + "c"
	path = "../land_res/" + prefix
	put_result( path + "\.imd" )
	put_result( path + "\.bin")
	put_result( path + "\.3dmd" )
end

File.open("field_list.csv") {|file|
	flag = false
	while line = file.gets do

		#�@#END��������I��
		if line =~ /\#END/ then
			break
		end

		#No.����ǂݎn��
		if line =~ /^No\./ then
			flag = true
			next
		end
		if flag == false then next end

		item = line.split(/,/)
		#map_out�`�͓ǂݔ�΂�
		if item[1] =~ /map_out/ then next end

		map_file_check item[1]
	end
}
