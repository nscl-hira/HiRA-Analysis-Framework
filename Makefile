CFLAGS    :=`root-config --cflags --libs`
SYSLIB    := -lz -l TreePlayer -lMathMore
ROOTCLINGORCINT   := rootcling

SrcSuf    := cpp
ObjSuf    := o
DepSuf    := h
PcmSuf    := _rdict.pcm

OBJS := HiRAReader.$(ObjSuf) EnergyLossModule.$(ObjSuf) nuclear_masses.$(ObjSuf) HiRACsICalibration.$(ObjSuf) HTHiRARootEvent.$(ObjSuf)
DEPS := $(_OBJS:.$(ObjSuf)=.$(DepSuf))

ROOTHTHIRA_HDRS  := HTHiRARootEvent.h HTHiRALinkDef.h
ROOTHTHIRA_DICT  := HTHiRADict.$(SrcSuf)
ROOTHTHIRA_DICTH := $(ROOTHTHIRA_DICT:.$(SrcSuf)=.h)
ROOTHTHIRA_DICTO := $(ROOTHTHIRA_DICT:.$(SrcSuf)=.$(ObjSuf))
ROOTHTHIRA_PCM   := HTHiRADict$(PcmSuf)

INCLUDES  := -I./include
INCLUDES  += -I./Nuclear_Masses

PROG      := $(wildcard exec_*.$(SrcSuf))
PROG      := $(patsubst %.$(SrcSuf), %, $(PROG))

CXXFLAGS  += $(INCLUDES) -std=c++11 -fPIC -O3

all: $(PROG)

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(PcmSuf)

$(PROG): $(OBJS) $(ROOTHTHIRA_DICTO)
	$(CXX) $(CXXFLAGS) -o ${@} ${@}.cpp $^ $(SYSLIB) $(CFLAGS) $(RLIBS)

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(CFLAGS)

$(ROOTHTHIRA_DICT):
	@echo "Generating dictionary $@..."
	$(ROOTCLINGORCINT) -f $@ -p $(INCLUDES) $(ROOTHTHIRA_HDRS)

.PHONY: clean
clean:
	@$(RM) -f $(OBJS) $(ROOTHTHIRA_DICT) $(ROOTHTHIRA_PCM) $(ROOTHTHIRA_DICTO) $(ROOTHTHIRA_DICTH)
	@echo "$(RM) -f $(OBJS) $(ROOTHTHIRA_DICT) $(ROOTHTHIRA_PCM) $(ROOTHTHIRA_DICTO) $(ROOTHTHIRA_DICTH)"
