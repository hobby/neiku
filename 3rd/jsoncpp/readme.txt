
获取jsoncpp.h/jsoncpp.cpp:
tar -zxvf jsoncpp-0.10.5.tar.gz
cd jsoncpp-0.10.5/
python amalgamate.py
mv dist/json/json.h dist/jsoncpp.h
cd dist/
sed -i -r 's/json\/json.h/jsoncpp.h/g' jsoncpp.cpp 
