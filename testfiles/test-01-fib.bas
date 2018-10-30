'fibonnaci sequence
'
const max_level = 10
dim level = 0
dim a = 0
dim b = 1

sub main
	call fib
end sub

sub fib
	dim temp
	print "fib" b
	if level < max_level then
		let level = level + 1
		let temp = b
		let b = b + a
		let a = temp
		call fib
	end if
end sub