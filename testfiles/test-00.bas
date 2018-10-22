const a = 10
dim b = 1

sub main
	dim a = 1
	let a = a + b * b + 2
	if a = 1 + 1 - 1 then
		call blah
	end if
end sub

sub blah
	print "hello world"
end sub