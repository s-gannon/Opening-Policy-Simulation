.PHONY: all clean
CXX = g++
TARGET = op_sim
CXXFLAGS = -Wall -pedantic
SRCS = SIR_multiple_locations.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

op_sim: $(OBJS)
	#$(CXX) -o $@ $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	#$(CXX) -c $< -o $@
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -- $(OBJS) $(TARGET)
