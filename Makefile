CXXFLAGS += -std=c++11
CXXFLAGS += -I./
CXXFLAGS += -I./deps/asio/asio/include/
CXXFLAGS += -std=c++11 -Wall -Werror -g -c -o

LIB_FILES :=-lglog -lgflags -levent  -lpthread -lssl -lcrypto -lz -lboost_system -lcppnetlib-client-connections \
	-lcppnetlib-server-parsers \
	-lcppnetlib-uri \
	-lpthread \
	-lleveldb \
	-lprotobuf \

TEST_LIB_FILES :=  -L/usr/local/lib -lgtest -lgtest_main -lgmock -lpthread

PROTOC = protoc
GRPC_CPP_PLUGIN=grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
PROTOS_PATH = ./protos

CPP_SOURCES := \
	./server/http_method.cc \
	./server/dispatcher.cc \
	./server/response_writer.cc \
	./server/request_reader.cc \
	./base/encryptor.cc \
	\
	./base/string_encode.cc \
	./base/stringpiece.cc \
	./base/status.cc \
	./base/arena.cc \
	./base/bitmap.cc \
	./base/histogram.cc \
	\
	./base/monitoring/registry.cc \
	./base/monitoring/util/protobuf.cc \
	./base/monitoring/prometheus/metrics.pb.cc \
	./base/monitoring/prometheus/exporter.cc \
	\
	./base/strings/stringprintf.cc \
	./base/strings/numbers.cc \
	./base/strings/scanner.cc \
	./base/strings/strcat.cc \
	./base/strings/str_util.cc \
	./base/strings/base64.cc \
	\
	./base/ini/ini.cc \
	./base/ini/ini_reader.cc \
	\
	./base/random/distribution_sampler.cc \
	./base/random/random.cc \
	./base/random/simple_philox.cc \
	./base/random/weighted_picker.cc \
	\
	./base/platform/mem.cc \
	\
	./log/logged_entry.cc \
	./log/leveldb_database.cc \

CPP_OBJECTS := $(CPP_SOURCES:.cc=.o)


TESTS := \
	./server/handler_chain_unittest \
	\
	./base/arena_unittest \
	./base/bitmap_unittest \
	./base/histogram_unittest \
	./base/ref_counted_unittest \
	./base/math_util_unittest \
	./base/gtl/array_slice_unittest \
	./base/gtl/cleanup_unittest \
	./base/gtl/edit_distance_unittest \
	./base/gtl/inlined_vector_unittest \
	./base/gtl/int_type_unittest \
	./base/gtl/iterator_range_unittest \
	./base/gtl/manual_constructor_unittest \
	./base/gtl/map_util_unittest \
	./base/gtl/top_n_unittest \
	\
	./base/strings/numbers_unittest \
	./base/strings/scanner_unittest \
	./base/strings/strcat_unittest \
	./base/strings/stringprintf_unittest \
	./base/strings/str_util_unittest \
	\
	./base/random/distribution_sampler_unittest \
	./base/random/philox_random_unittest \
	./base/random/random_distributions_unittest \
	./base/random/random_unittest \
	./base/random/simple_philox_unittest \
	./base/random/weighted_picker_unittest \
	\
	./base/monitoring/counter_unittest \
	./base/monitoring/gauge_unittest \
	./base/monitoring/registry_unittest \

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

## base
./base/arena_unittest: ./base/arena_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/arena_unittest.o: ./base/arena_unittest.cc \
	./base/arena.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/bitmap_unittest: ./base/bitmap_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/bitmap_unittest.o: ./base/bitmap_unittest.cc \
	./base/bitmap.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/histogram_unittest: ./base/histogram_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/histogram_unittest.o: ./base/histogram_unittest.cc \
	./base/histogram.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/ref_counted_unittest: ./base/ref_counted_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/ref_counted_unittest.o: \
	./base/ref_counted_unittest.cc \
	./base/ref_counted.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/math_util_unittest: ./base/math_util_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/math_util_unittest.o: ./base/math_util_unittest.cc \
	./base/math_util.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/gtl/array_slice_unittest: ./base/gtl/array_slice_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/array_slice_unittest.o: ./base/gtl/array_slice_unittest.cc \
	./base/gtl/array_slice_internal.h \
	./base/gtl/array_slice.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/cleanup_unittest: ./base/gtl/cleanup_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/cleanup_unittest.o: ./base/gtl/cleanup_unittest.cc \
	./base/gtl/cleanup.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/edit_distance_unittest: ./base/gtl/edit_distance_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/edit_distance_unittest.o: ./base/gtl/edit_distance_unittest.cc \
	./base/gtl/edit_distance.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/inlined_vector_unittest: ./base/gtl/inlined_vector_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/inlined_vector_unittest.o: ./base/gtl/inlined_vector_unittest.cc \
	./base/gtl/inlined_vector.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/int_type_unittest: ./base/gtl/int_type_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/int_type_unittest.o: ./base/gtl/int_type_unittest.cc \
	./base/gtl/int_type.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/iterator_range_unittest: ./base/gtl/iterator_range_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/iterator_range_unittest.o: ./base/gtl/iterator_range_unittest.cc \
	./base/gtl/iterator_range.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/manual_constructor_unittest: ./base/gtl/manual_constructor_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/manual_constructor_unittest.o: ./base/gtl/manual_constructor_unittest.cc \
	./base/gtl/manual_constructor.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/map_util_unittest: ./base/gtl/map_util_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/map_util_unittest.o: ./base/gtl/map_util_unittest.cc \
	./base/gtl/map_util.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/gtl/top_n_unittest: ./base/gtl/top_n_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/gtl/top_n_unittest.o: ./base/gtl/top_n_unittest.cc \
	./base/gtl/top_n.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/strings/numbers_unittest: ./base/strings/numbers_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/strings/numbers_unittest.o: ./base/strings/numbers_unittest.cc \
	./base/strings/numbers.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/strings/scanner_unittest: ./base/strings/scanner_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/strings/scanner_unittest.o: ./base/strings/scanner_unittest.cc \
	./base/strings/scanner.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/strings/stringprintf_unittest: ./base/strings/stringprintf_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/strings/stringprintf_unittest.o: ./base/strings/stringprintf_unittest.cc \
	./base/strings/stringprintf.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/strings/strcat_unittest: ./base/strings/strcat_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/strings/strcat_unittest.o: ./base/strings/strcat_unittest.cc \
	./base/strings/strcat.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/strings/str_util_unittest: ./base/strings/str_util_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/strings/str_util_unittest.o: ./base/strings/str_util_unittest.cc \
	./base/strings/str_util.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./base/random/distribution_sampler_unittest: ./base/random/distribution_sampler_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/random/distribution_sampler_unittest.o: ./base/random/distribution_sampler_unittest.cc \
	./base/random/distribution_sampler.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/random/philox_random_unittest: ./base/random/philox_random_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/random/philox_random_unittest.o: ./base/random/philox_random_unittest.cc \
	./base/random/philox_random.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/random/random_distributions_unittest: ./base/random/random_distributions_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/random/random_distributions_unittest.o: ./base/random/random_distributions_unittest.cc \
	./base/random/random_distributions.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/random/random_unittest: ./base/random/random_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/random/random_unittest.o: ./base/random/random_unittest.cc \
	./base/random/random.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/random/simple_philox_unittest: ./base/random/simple_philox_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/random/simple_philox_unittest.o: ./base/random/simple_philox_unittest.cc \
	./base/random/simple_philox.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/random/weighted_picker_unittest: ./base/random/weighted_picker_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/random/weighted_picker_unittest.o: ./base/random/weighted_picker_unittest.cc \
	./base/random/weighted_picker.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<


./base/monitoring/counter_unittest: ./base/monitoring/counter_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/monitoring/counter_unittest.o: ./base/monitoring/counter_unittest.cc \
	./base/monitoring/counter.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/monitoring/gauge_unittest: ./base/monitoring/gauge_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/monitoring/gauge_unittest.o: ./base/monitoring/gauge_unittest.cc \
	./base/monitoring/gauge.h
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<
./base/monitoring/registry_unittest: ./base/monitoring/registry_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./base/monitoring/registry_unittest.o: ./base/monitoring/registry_unittest.cc \
	./base/monitoring/registry.h
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
