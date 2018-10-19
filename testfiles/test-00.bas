const a = 10
dim b = 1

sub main
	dim a = 1
	a = a + b
	if a = 1 then
		call blah
	end if
end sub

sub blah
	print "hello world"
end sub