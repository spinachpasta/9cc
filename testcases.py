from test import check, checkandlink

assert check("int main(){return 0;}", 0)

assert check("int main(){return 42;}", 42)

assert check("int main(){return 0+10+3;}", 0+10+3)

assert check("int main(){return 111+10-42;}", 111+10-42)

assert check("int main(){return    111   + 10 -     42;}", 111+10-42)

assert check("int main(){return    0 +    10+    3;}",  0 + 10 + 3)

assert check("int main(){return 10*2;}", 10*2)

assert check("int main(){return 10+1*2;}", 10+1*2)
assert check("int main(){return 10+3*2+10-5;}", 10+3*2+10-5)

assert check("int main(){return (10+3)*2+10-5;}", (10+3)*2+10-5)
assert check("int main(){return (10+1)*2;}", (10+1)*2)


assert check("int main(){return (10+1)/2;}", (10+1)//2)
assert check("int main(){return (15+1)/2+3;}", (15+1)//2+3)
assert check("int main(){return 10+1 /2/5;}", 10+1//2//5)

# unary
assert check("int main(){return -10+1 /2/5+30;}", -10+1//2//5+30)
assert check("int main(){return +10+1 /2/5;}", +10+1//2//5)
assert check("int main(){return -2*-3;}", -2*-3)

# equality
assert check("int main(){return 1==0;}", 0)
assert check("int main(){return 1==1;}", 1)
assert check("int main(){return 1==1+5;}", 0)
assert check("int main(){return 1+(1+1==1+1);}", 2)

assert check("int main(){return 1!=0;}", 1)
assert check("int main(){return 1!=1;}", 0)
assert check("int main(){return 1!=1+5;}", 1)
assert check("int main(){return 1+(1+1!=1+1);}", 1)


# relational
assert check("int main(){return 1>0;}", 1)
assert check("int main(){return 1>1;}", 0)
assert check("int main(){return 1<0;}", 0)
assert check("int main(){return 1<1;}", 0)
assert check("int main(){return 1>=0;}", 1)
assert check("int main(){return 1>=1;}", 1)
assert check("int main(){return 1<=0;}", 0)
assert check("int main(){return 1<=1;}", 1)


# semicolon
assert check("int main(){1+1;return 5-2;}", 3)

# variables
assert check("int main(){int a;a=3;return a;}", 3)
assert check("int main(){int a;int b;a=3;b=4;return a+b;}", 7)

assert check("int main(){int ab;int bd;ab=3;bd=4;return ab+bd;}", 7)
assert check("int main(){int abz;int bdz;abz=3;bdz =4;return abz+bdz;}", 7)

assert check("int main(){return 1;return 2;}", 1)
assert check("int main(){return 1;return 2+3;}", 1)
assert check("int main(){int a; a=0;if(1)a=1;return a;}", 1)
assert check("int main(){int a; a=0;if(0)a=1;return a;}", 0)

assert check("int main(){int a;a=1;if(a)a=5;return a;}", 5)
assert check("int main(){int a;a=0;if(a)a=5;return a;}", 0)

assert check("int main(){int a;a=1;if(a)return 5;return 10;}", 5)
assert check("int main(){int a;a=0;if(a)return 5;return 10;}", 10)

assert check(
    "int main(){int a;a=0;if(a)return 5;a=1;if(a)return 3;return 10;}", 3)
assert check("int main(){int a;a=0;while(a)return 1; return 3;}", 3)
assert check("int main(){int a;a=0;while(a<5)a=a+1; return a;}", 5)

assert check("int main(){int a;a=0;if(a)return 5;else a=10;return a;}", 10)
assert check("int main(){int a;a=1;if(a)a=0;else return 10;return a;}", 0)

assert check("int main(){int a;int b;for(a=0;a<10;a=a+1)b=a;return b;}", 9)
assert check("int main(){for(;;)return 0;}", 0)


assert check(
    "int main(){int b;int a;b=0;for(a=0;a<10;a=a+1){b=b+3;}return b;}", 30)
assert check("int main(){int b;b=0;{b=b+3;}return b;}", 3)
assert check(
    "int main(){int b; int a;b=0;for(a=0;a<10;a=a+1){b=b+1;b=b+2;}return b;}", 30)

assert checkandlink("int main(){return abc();}", 3)
assert checkandlink("int main(){return three();}", 3)
assert checkandlink("int main(){return identity(5);}", 5)
assert checkandlink("int main(){return identity(three());}", 3)
assert checkandlink("int main(){return identity(three()*2);}", 6)
assert checkandlink("int main(){return add(1,2);}", 3)
assert checkandlink("int main(){return addsix(1,2,3,4,5,6);}", 21)
assert checkandlink("int main(){return firstarg(1,2,3,4,5,6);}", 1)
assert checkandlink("int main(){return secondarg(1,2,3,4,5,6);}", 2)
assert checkandlink("int main(){return thirdarg(1,2,3,4,5,6);}", 3)
assert checkandlink("int main(){return fourtharg(1,2,3,4,5,6);}", 4)
assert checkandlink("int main(){return fiftharg(1,2,3,4,5,6);}", 5)
assert checkandlink("int main(){return sixtharg(1,2,3,4,5,6);}", 6)


assert check("int myfunc(){return 10;} int main(){return myfunc();}", 10)
assert check("int myfunc(int a){return 10;} int main(){return myfunc(1);}", 10)
assert check("int myident(int a){return a;} int main(){return myident(3);}", 3)
assert check(
    "int myadd(int a,int b){return a+b;} int main(){return myadd(3,2);}", 5)
assert check(
    "int mysub(int a,int b){return a-b;} int main(){return mysub(3,2);}", 1)

assert check(
    "int mysub3(int a,int b,int c){return a-b-c;} int main(){return mysub3(3,1,1);}", 1)
assert check(
    "int myfunc6(int a,int b,int c,int d,int e,int f){return a-b-c+d*e*f;} int main(){return myfunc6(3,1,1,1,2,3);}", 7)

assert check("int main(){int x;int* y;x=3;y=&x;return *y;}", 3)

assert checkandlink("int main(){return *pointerTest();}", 11)

assert check("int main(){int x;int *y;y = &x;*y = 3;return x;}", 3)

assert checkandlink("int main(){return abc()+2;}", 5)

assert checkandlink("int main(){int* p;allocfour(&p);return *(1+p);}", 12)
assert checkandlink('''int main(){int *p;allocfour(&p);int *q;q = 2 + p;return *q;}''', 13)

assert checkandlink('''int main(){int *p;allocfour(&p);int *q;q = p + 2;return *q;}''', 13)

assert checkandlink('''int main(){int *p;allocfour(&p);int *q;q = p + 2;*q;q = p + 3;return *q;}''', 14)