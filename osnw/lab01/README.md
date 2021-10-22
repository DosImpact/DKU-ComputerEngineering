## 컴파일 명령어
.c 파일 3개 만들어서 합쳐보기

```
gcc -c main.c
gcc -c id.c
gcc -c name.c

gcc -o helloworld main.o id.o name.o
./helloworld
```

## Makefile 작성

### makefile 작성시 주의점 (애러수정)

- makefile은 작성시 명령어부분은 TAB으로 뛰어써야한다. (공백 4번이면 오류가 난다.)

>makefile:4: *** missing separator.  Stop.


>makefile:3: *** recipe commences before first target.  Stop.
