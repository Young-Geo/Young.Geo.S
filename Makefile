#########create by yekai 20160729##############
SrcPcFiles=$(wildcard *.pc)
SrcFiles=$(patsubst %.pc,%.c,$(SrcPcFiles))
TargetFiles=$(patsubst %.pc,%,$(SrcPcFiles))
IncPath=$(ORACLE_HOME)/precomp/public
LibPath=$(ORACLE_HOME)/lib
Pubso=-lclntsh
CC=gcc

all:$(TargetFiles)
%.c:%.pc
	proc iname=$? oname=$@

$(TargetFiles):%:%.c
	$(CC) -o $@ $? -I$(IncPath) -L$(LibPath) $(Pubso)


test:
	echo $(SrcPcFiles)
	echo $(TargetFiles)
	echo $(SrcFiles)

clean:
	rm -f $(TargetFiles)
	rm -f *.lis
	rm -f $(SrcFiles)

