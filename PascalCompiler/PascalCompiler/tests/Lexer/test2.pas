program gcd;

type 
	Bptr = ^boolean;

var a,b,c:integer;
	inpt: string;
	d: Bptr;
	e: boolean;
	f,h: real;

inpt:string;

{
	function to find greatest common divisor
}



function GCD(m,n:integer):integer;
	begin
		while m<>n do 
		if m>n then m:=m-n else n:=n-m;
		GCD:=m;
	end;


begin
	inpt := 'Input 3 numbers';
	writeln(inpt);

	e:= true;
	d := e;

	f := 123.4567;

	t:= ' 14214214214';

	read(a,b,c);

	writeln('The gcd of 3 numbers is:',GCD(GCD(a,b),c));
	
	a := (a+b)*c - d/ ( 312 + 40);
	if ---12 + 13 >= ((4*5)+10 > 5) then
	begin
		writeln('Fine');
	end;
end.


