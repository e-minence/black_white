#4バイト境界に合わせて０をつめる
wname = ARGV[0].sub(/h\.bhc/,"\.bhc")

File.open(wname, "wb") {|wf|
	File.open(ARGV[0],"rb") {|f|
		wf.write(f.read)
		padding = File.size(ARGV[0]) % 4
		if padding != 0 then
			padding.times{|dmy| wf.write(0)}
		end
	}
}
