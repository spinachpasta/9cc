from test import check, checkandlink

assert check("return 0;", 0)

assert check("return 42;", 42)

assert check("return 0+10+3;", 0+10+3)

assert check("return 111+10-42;", 111+10-42)

assert check("return    111   + 10 -     42;", 111+10-42)

assert check("return    0 +    10+    3;",  0 + 10 + 3)

assert check("return 10*2;", 10*2)

assert check("return 10+1*2;", 10+1*2)
assert check("return 10+3*2+10-5;", 10+3*2+10-5)

assert check("return (10+3)*2+10-5;", (10+3)*2+10-5)
assert check("return (10+1)*2;", (10+1)*2)


assert check("return (10+1)/2;", (10+1)//2)
assert check("return (15+1)/2+3;", (15+1)//2+3)
assert check("return 10+1 /2/5;", 10+1//2//5)

# unary
assert check("return -10+1 /2/5+30;", -10+1//2//5+30)
assert check("return +10+1 /2/5;", +10+1//2//5)
assert check("return -2*-3;", -2*-3)

# equality
assert check("return 1==0;", 0)
assert check("return 1==1;", 1)
assert check("return 1==1+5;", 0)
assert check("return 1+(1+1==1+1);", 2)

assert check("return 1!=0;", 1)
assert check("return 1!=1;", 0)
assert check("return 1!=1+5;", 1)
assert check("return 1+(1+1!=1+1);", 1)


# relational
assert check("return 1>0;", 1)
assert check("return 1>1;", 0)
assert check("return 1<0;", 0)
assert check("return 1<1;", 0)
assert check("return 1>=0;", 1)
assert check("return 1>=1;", 1)
assert check("return 1<=0;", 0)
assert check("return 1<=1;", 1)


# semicolon
assert check("1+1;return 5-2;", 3)

# variables
assert check("a=3;return a;", 3)
assert check("a=3;b=4;return a+b;", 7)

assert check("ab=3;bd=4;return ab+bd;", 7)
assert check("abz=3;bdz =4;return abz+bdz;", 7)

assert check("return 1;return 2;", 1)
assert check("return 1;return 2+3;", 1)
assert check("a=0;if(1)a=1;return a;", 1)
assert check("a=0;if(0)a=1;return a;", 0)

assert check("a=1;if(a)a=5;return a;", 5)
assert check("a=0;if(a)a=5;return a;", 0)

assert check("a=1;if(a)return 5;return 10;", 5)
assert check("a=0;if(a)return 5;return 10;", 10)

assert check("a=0;if(a)return 5;a=1;if(a)return 3;return 10;", 3)
assert check("a=0;while(a)return 1; return 3;", 3)
assert check("a=0;while(a<5)a=a+1; return a;", 5)

assert check("a=0;if(a)return 5;else a=10;return a;", 10)
assert check("a=1;if(a)a=0;else return 10;return a;", 0)

assert check("for(a=0;a<10;a=a+1)b=a;return b;", 9)
assert check("for(;;)return 0;", 0)


assert check("b=0;for(a=0;a<10;a=a+1){b=b+3;}return b;", 30)
assert check("b=0;{b=b+3;}return b;", 3)
assert check("b=0;for(a=0;a<10;a=a+1){b=b+1;b=b+2;}return b;", 30)

assert checkandlink("return abc();", 3)
assert checkandlink("return three();", 3)
assert checkandlink("return identity(5);", 5)
assert checkandlink("return identity(three());", 3)
assert checkandlink("return identity(three()*2);", 6)
assert checkandlink("return add(1,2);", 3)
assert checkandlink("return addsix(1,2,3,4,5,6);", 21)
assert checkandlink("return firstarg(1,2,3,4,5,6);", 1)
assert checkandlink("return secondarg(1,2,3,4,5,6);", 2)
assert checkandlink("return thirdarg(1,2,3,4,5,6);", 3)
assert checkandlink("return fourtharg(1,2,3,4,5,6);", 4)
assert checkandlink("return fiftharg(1,2,3,4,5,6);", 5)
assert checkandlink("return sixtharg(1,2,3,4,5,6);", 6)
