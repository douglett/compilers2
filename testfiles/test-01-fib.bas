'fibonnaci sequence
'
dim a = 1
dim b = 1
dim level = 5

sub main
	call fib
end sub

sub fib
	print "fib"
	if level >= 0 then
		call fib
		let level = level - 1
	end if
end sub