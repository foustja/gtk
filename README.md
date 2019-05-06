C files making use of gtk+3 widgets

Compile with:<br/>
```gcc `pkg-config --cflags gtk+-3.0` -o <output file> <input file.c> `pkg-config --libs gtk+-3.0` -lm```

Note: must have gcc and gtk+3

Source code may also be compiled following extraction from tarballs with the following:<br/>
```$ ./configure```<br/>
```$ make```<br/>
```$ make install```


