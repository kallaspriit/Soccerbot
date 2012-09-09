#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.135                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC =  -I/usr/local/include/websocketpp -Iinclude -Ilibraries
CFLAGS =  -Wall -fexceptions
RESINC = 
LIBDIR =  -L/usr/local/lib
LIB =  -lwebsocketpp -lboost_system -lboost_date_time -lboost_program_options -lboost_thread -lboost_regex -lpthread
LDFLAGS = 

INC_DEBUG =  $(INC)
CFLAGS_DEBUG =  $(CFLAGS) -g
RESINC_DEBUG =  $(RESINC)
RCFLAGS_DEBUG =  $(RCFLAGS)
LIBDIR_DEBUG =  $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG =  $(LDFLAGS)
OBJDIR_DEBUG = obj/debug
DEP_DEBUG = 
OUT_DEBUG = ./soccerbotd

INC_RELEASE =  $(INC)
CFLAGS_RELEASE =  $(CFLAGS) -O2
RESINC_RELEASE =  $(RESINC)
RCFLAGS_RELEASE =  $(RCFLAGS)
LIBDIR_RELEASE =  $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE =  $(LDFLAGS) -s
OBJDIR_RELEASE = obj/release
DEP_RELEASE = 
OUT_RELEASE = ./soccerbot

OBJ_DEBUG = $(OBJDIR_DEBUG)/src/Command.o $(OBJDIR_DEBUG)/src/main.o $(OBJDIR_DEBUG)/src/Wheel.o $(OBJDIR_DEBUG)/src/WebSocketServer.o $(OBJDIR_DEBUG)/src/Util.o $(OBJDIR_DEBUG)/src/SoccerBot.o $(OBJDIR_DEBUG)/src/Serial.o $(OBJDIR_DEBUG)/src/Robot.o $(OBJDIR_DEBUG)/src/Math.o $(OBJDIR_DEBUG)/src/JsonResponse.o $(OBJDIR_DEBUG)/src/JSON.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/src/Command.o $(OBJDIR_RELEASE)/src/main.o $(OBJDIR_RELEASE)/src/Wheel.o $(OBJDIR_RELEASE)/src/WebSocketServer.o $(OBJDIR_RELEASE)/src/Util.o $(OBJDIR_RELEASE)/src/SoccerBot.o $(OBJDIR_RELEASE)/src/Serial.o $(OBJDIR_RELEASE)/src/Robot.o $(OBJDIR_RELEASE)/src/Math.o $(OBJDIR_RELEASE)/src/JsonResponse.o $(OBJDIR_RELEASE)/src/JSON.o

all: debug release

clean: clean_debug clean_release

before_debug: 
	test -d . || mkdir -p .
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src

after_debug: 

debug: before_debug out_debug after_debug

out_debug: $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LDFLAGS_DEBUG) $(LIBDIR_DEBUG) $(OBJ_DEBUG) $(LIB_DEBUG) -o $(OUT_DEBUG)

$(OBJDIR_DEBUG)/src/Command.o: src/Command.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Command.cpp -o $(OBJDIR_DEBUG)/src/Command.o

$(OBJDIR_DEBUG)/src/main.o: src/main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/main.cpp -o $(OBJDIR_DEBUG)/src/main.o

$(OBJDIR_DEBUG)/src/Wheel.o: src/Wheel.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Wheel.cpp -o $(OBJDIR_DEBUG)/src/Wheel.o

$(OBJDIR_DEBUG)/src/WebSocketServer.o: src/WebSocketServer.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/WebSocketServer.cpp -o $(OBJDIR_DEBUG)/src/WebSocketServer.o

$(OBJDIR_DEBUG)/src/Util.o: src/Util.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Util.cpp -o $(OBJDIR_DEBUG)/src/Util.o

$(OBJDIR_DEBUG)/src/SoccerBot.o: src/SoccerBot.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/SoccerBot.cpp -o $(OBJDIR_DEBUG)/src/SoccerBot.o

$(OBJDIR_DEBUG)/src/Serial.o: src/Serial.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Serial.cpp -o $(OBJDIR_DEBUG)/src/Serial.o

$(OBJDIR_DEBUG)/src/Robot.o: src/Robot.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Robot.cpp -o $(OBJDIR_DEBUG)/src/Robot.o

$(OBJDIR_DEBUG)/src/Math.o: src/Math.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Math.cpp -o $(OBJDIR_DEBUG)/src/Math.o

$(OBJDIR_DEBUG)/src/JsonResponse.o: src/JsonResponse.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/JsonResponse.cpp -o $(OBJDIR_DEBUG)/src/JsonResponse.o

$(OBJDIR_DEBUG)/src/JSON.o: src/JSON.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/JSON.cpp -o $(OBJDIR_DEBUG)/src/JSON.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf .
	rm -rf $(OBJDIR_DEBUG)/src

before_release: 
	test -d . || mkdir -p .
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src

after_release: 

release: before_release out_release after_release

out_release: $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LDFLAGS_RELEASE) $(LIBDIR_RELEASE) $(OBJ_RELEASE) $(LIB_RELEASE) -o $(OUT_RELEASE)

$(OBJDIR_RELEASE)/src/Command.o: src/Command.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Command.cpp -o $(OBJDIR_RELEASE)/src/Command.o

$(OBJDIR_RELEASE)/src/main.o: src/main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/main.cpp -o $(OBJDIR_RELEASE)/src/main.o

$(OBJDIR_RELEASE)/src/Wheel.o: src/Wheel.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Wheel.cpp -o $(OBJDIR_RELEASE)/src/Wheel.o

$(OBJDIR_RELEASE)/src/WebSocketServer.o: src/WebSocketServer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/WebSocketServer.cpp -o $(OBJDIR_RELEASE)/src/WebSocketServer.o

$(OBJDIR_RELEASE)/src/Util.o: src/Util.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Util.cpp -o $(OBJDIR_RELEASE)/src/Util.o

$(OBJDIR_RELEASE)/src/SoccerBot.o: src/SoccerBot.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/SoccerBot.cpp -o $(OBJDIR_RELEASE)/src/SoccerBot.o

$(OBJDIR_RELEASE)/src/Serial.o: src/Serial.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Serial.cpp -o $(OBJDIR_RELEASE)/src/Serial.o

$(OBJDIR_RELEASE)/src/Robot.o: src/Robot.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Robot.cpp -o $(OBJDIR_RELEASE)/src/Robot.o

$(OBJDIR_RELEASE)/src/Math.o: src/Math.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Math.cpp -o $(OBJDIR_RELEASE)/src/Math.o

$(OBJDIR_RELEASE)/src/JsonResponse.o: src/JsonResponse.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/JsonResponse.cpp -o $(OBJDIR_RELEASE)/src/JsonResponse.o

$(OBJDIR_RELEASE)/src/JSON.o: src/JSON.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/JSON.cpp -o $(OBJDIR_RELEASE)/src/JSON.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf .
	rm -rf $(OBJDIR_RELEASE)/src

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

