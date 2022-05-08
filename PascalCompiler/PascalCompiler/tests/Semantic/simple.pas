var a,b,c,i,n:integer;
	
begin
	i:= 1;
	n:= 10;
	a:= 0;
	b:= 1;
	c:= 1;

	while i < n do
	begin
		c := a + b;
		a := b;
		b := c;
		i := i+1;
	end;
	writeln(n,c,i)

end.