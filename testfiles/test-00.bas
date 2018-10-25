const a = 10
const c = 5
dim b = 1

sub main
	dim a = 1
	let a = a + b * c + 2
	if a = 1 + 1 + 2 then
		call blah
	end if
	let c = 10
end sub

sub blah
	print "hello world"
end sub