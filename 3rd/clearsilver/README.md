## 编译安装
```bash
$ tar -zxvf clearsilver-0.10.5.tar.gz
$ cd clearsilver-0.10.51
$ ./configure --prefix=/usr/local/clearsilver --disable-wdb --disable-apache --disable-python --disable-perl --disable-java --disable-csharp --disable-ruby
$ sed -i 's/test_crc.cs//g' cs/Makefile
$ make
$ sudo make install
$ tree /usr/local/clearsilver
```

## 版本变更

### clearsilver-0.10.5-post-content-type-bugfix.tar.gz

1. 更新日期
2017-05-18

2. 更新内容
- 对于POST请求，修复content-type判断逻辑，允许出现charset

支持如下POST请求头（原来只支持下面第一种）:
content-type: application/x-www-form-urlencoded
content-type: application/x-www-form-urlencoded; charset=UTF-8
content-type: application/x-www-form-urlencoded***

详见：clearsilver-0.10.5/cgi/cgi.c@643

### clearsilver-0.10.5.tar.gz

1. 更新日志
2017-04-07

2. 更新内容
原始版本，v0.10.5
