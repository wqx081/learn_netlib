CXXFLAGS += -std=c++11
CXXFLAGS += -I./
CXXFLAGS += -I./deps/asio/asio/include/
CXXFLAGS += -std=c++11 -Wall -Werror -g -c -o

LIB_FILES :=-lglog -lgflags -levent  -lpthread -lssl -lcrypto -lz -lboost_system -lcppnetlib-client-connections \
	-lcppnetlib-server-parsers \
	-lcppnetlib-uri \
	-lpthread \
	-lleveldb \


TEST_LIB_FILES :=  -L/usr/local/lib -lgtest -lgtest_main -lpthread

PROTOC = protoc
GRPC_CPP_PLUGIN=grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
PROTOS_PATH = ./protos

CPP_SOURCES := \
	./server/http_method.cc \
	./server/dispatcher.cc \
	./server/response_writer.cc \
	./server/request_reader.cc \
	./base/string_encode.cc \
	./base/stringpiece.cc \
	./base/base64.cc \
	./base/encryptor.cc \
	./log/logged_entry.cc \
	./log/leveldb_database.cc \

CPP_OBJECTS := $(CPP_SOURCES:.cc=.o)


TESTS := \
	./server/handler_chain_unittest \

APP := mpr_rest_server

all: $(CPP_OBJECTS) $(APP) $(TESTS)
.cc.o:
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

$(APP): ./server/http_server.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./server/http_server.o: \
	./server/http_server.cc \
	./server/handler_context.h \
	./server/http_server.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./server/handler_chain_unittest: \
	./server/handler_chain_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./server/handler_chain_unittest.o: \
	./server/handler_chain_unittest.cc \
	./server/handler_chain.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

## /////////////////////////////

clean:
	rm -fr ./server/*.o
	rm -fr $(APP)
	@rm -fr $(TESTS)
	@echo "rm *_unittest"
	@rm -fr $(CPP_OBJECTS)
	@echo "rm *.o"
