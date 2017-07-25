#########create by yekai 20160729##############
dircpp=afx/*.cpp Server/*.cpp Game/*.cpp ./*.cpp
src=$(wildcard $(dircpp))
obj=$(patsubst %.cpp,%.o,$(src))
inc=-Iafx -IServer -IGame
lib=-levent -lpthread
tag=-g
CC=g++
tar=demo

all:$(tar)

$(tar):$(obj)
	$(CC) $^ $(inc) $(lib) $(tag) -o $@
    
%.o:%.cpp
	$(CC) -c $(tag) $< $(inc) -o $@

test:
	echo $(src)
	echo $(obj)
	echo $(tar)

clean:
	rm -f $(obj)
	rm -f $(tar)

