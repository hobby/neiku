

# how to fastcgi

## get fastcgi
official => https://github.com/odis-project/fastcgi
backup   => https://github.com/hobby/fastcgi

## build
```
$ ./configure --prefix=$HOME/opt/fastcgi/ --disable-shared # libfcgi.a only
$ make
```

## install headers/libs
```
$ make install # to --prefix (~/opt/fastcgi)
$ tree # --prefix (~/opt/fastcgi)
├── bin
│   └── cgi-fcgi
├── include
│   ├── fastcgi.h
│   ├── fcgi_config.h
│   ├── fcgi_stdio.h
│   ├── fcgiapp.h
│   ├── fcgimisc.h
│   ├── fcgio.h
│   └── fcgios.h
└── lib
    ├── libfcgi++.a
    ├── libfcgi++.la
    ├── libfcgi.a
    └── libfcgi.la
```

## clean
```
$ make clean
$ make distclean
```

