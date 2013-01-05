
all : layout.exe layout2.exe genset.exe ongrid.exe setinfo.exe
	echo success!

layout.exe : src/layout.cpp
	g++ $^ -o $@

layout2.exe : src/layout2.cpp
	g++ $^ -o $@ -lpthread

genset.exe : src/genset.cpp
	g++ $^ -o $@

ongrid.exe : src/ongrid.cpp
	g++ $^ -o $@

setinfo.exe : src/setinfo.cpp
	g++ $^ -o $@
