#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "json-parser.h"

void print_level(int level, const char* str, ...) {
	va_list	args;
	for( int i = 0; i < level; ++i ) {
		printf("    ");
	}

	va_start(args, str);
	vprintf(str, args);
	va_end(args);
}

void
dump(json_value_t* v, int level) {
	switch( v->tag ) {
	case JSON_ARRAY:
		print_level(level, "[");
		for( size_t i = 0; i < v->array.count; ++i ) {
			dump(v->array.array[i], 0);

			if( i < v->array.count - 1) {
				printf(", ");
			}
		}

		print_level(level, "]\n");
		break;

	case JSON_BOOLEAN:
		if( v->boolean ) {
			print_level(level, "true");
		} else {
			print_level(level, "false");
		}
		break;

	case JSON_NONE:		print_level(level, "none"); break;

	case JSON_NUMBER:	print_level(level, "%lf", v->number); break;

	case JSON_OBJECT:
		print_level(level, "{\n");
		for( size_t i = 0; i < v->members.count; ++i ) {
			print_level(0, "\"");
			for( size_t s = 0; s < v->members.array[i].key.len; ++s ) {
				printf("%c", v->members.array[i].key.start[s]);
			}
			printf("\"");

			printf(" : ");
			dump(v->members.array[i].value, 0);


			if( i < v->members.count - 1) {
				printf(", ");
			}
		}

		print_level(level, "\n}");
		break;

	case JSON_STRING:
		print_level(level, "\"");
		for( size_t s = 0; s < v->string.len; ++s ) {
			printf("%c", v->string.start[s]);
		}
		printf("\"");
		break;

	default: break;
	}
}

void
test_simple_object() {
	const char*		json_str	= "{\n\"key\":\"value\"\n}";
	json_value_t*	val			= json_parse(json_str).value;
	dump(val, 0);
	printf("\n");
	json_free(val);
}

void test_simple_string(char* json_str) {
    json_value_t*	val			= json_parse(json_str).value;
    if (val) {
        dump(val, 0);
        printf("\n");
        json_free(val);
    }
}

void
test_simple_object2() {
	const char*		json_str	= "{\n\"key\":\"value\",\n\"key2\":\"value2\"\n,\"key3\":23.4}";
	json_value_t*	val			= json_parse(json_str).value;
	dump(val, 0);
	printf("\n");
	json_free(val);
}

char*
load_file(const char* filename) {
	FILE*	f = fopen(filename, "rb");
	if( f ) {
		fseek(f, 0, SEEK_END);
		size_t	size	= ftell(f);
		fseek(f, 0, SEEK_SET);
		char*	buffer	= (char*)malloc(size + 1);
		fread(buffer, 1, size, f);
		buffer[size]	= '\0';
		fclose(f);
		return buffer;
	} else {
		return NULL;
	}
}

static const char*
pass_tests[] = {
	"twitter.json",
	"canada.json",
	"citm_catalog.json",
};

void
test_pass() {
	for( size_t t = 0; t < sizeof(pass_tests) / sizeof(char*); ++t ) {
		char*	test = load_file(pass_tests[t]);
		json_value_t*	val	= json_parse(test).value;
		if(val != NULL) {
			fprintf(stderr, "test %s is passing\n", pass_tests[t]);
			json_free(val);
		} else {
			fprintf(stderr, "test %s failing!\n", pass_tests[t]);
		}
		free(test);
	}
}


static const char*
fail_tests[] = {
	"jsonchecker/fail01_EXCLUDE.json",
	"jsonchecker/fail02.json",
	"jsonchecker/fail03.json",
	"jsonchecker/fail04.json",
	"jsonchecker/fail05.json",
	"jsonchecker/fail06.json",
	"jsonchecker/fail07.json",
	"jsonchecker/fail08.json",
	"jsonchecker/fail09.json",
	"jsonchecker/fail10.json",
	"jsonchecker/fail11.json",
	"jsonchecker/fail12.json",
	"jsonchecker/fail13.json",
	"jsonchecker/fail14.json",
	"jsonchecker/fail15.json",
	"jsonchecker/fail16.json",
	"jsonchecker/fail17.json",
	"jsonchecker/fail18_EXCLUDE.json",
	"jsonchecker/fail19.json",
	"jsonchecker/fail20.json",
	"jsonchecker/fail21.json",
	"jsonchecker/fail22.json",
	"jsonchecker/fail23.json",
	"jsonchecker/fail24.json",
	"jsonchecker/fail25.json",
	"jsonchecker/fail26.json",
	"jsonchecker/fail27.json",
	"jsonchecker/fail28.json",
	"jsonchecker/fail29.json",
	"jsonchecker/fail30.json",
	"jsonchecker/fail31.json",
	"jsonchecker/fail32.json",
	"jsonchecker/fail33.json",
	"jsonchecker/fail34.json",
	"jsonchecker/fail35.json",

};


void
test_fail() {
	for( size_t t = 0; t < sizeof(fail_tests) / sizeof(char*); ++t ) {
		char*	test = load_file(fail_tests[t]);
		json_value_t*	val	= json_parse(test).value;
		if(val != NULL) {
			fprintf(stderr, "test %s is passing! should be failing!\n", fail_tests[t]);
			json_free(val);
		} else {
			fprintf(stderr, "test %s is passing by failing\n", fail_tests[t]);
		}
		free(test);
	}
}

int
main(int argc, char* argv[]) {
	test_simple_object();
	test_simple_object2();
    test_simple_string("123");    // json parse is passing when input is integer(123).
    test_simple_string("\"123\"");    // json parse is passing when input is string("123").
    test_simple_string("true");   // json parse is passing when input is boolean(true).
    test_simple_string("\true\"");    // json parse is passing when input is string("true").
    test_simple_string("false");  // json parse is passing when input is boolean(false).
    test_simple_string("\"false\"");  // json parse is passing when input is string("false").
    test_simple_string("null");   // json parse is passing when input is null.
    test_simple_string("\"null\"");   // json parse is passing when input is string("null").
	test_pass();
	test_fail();
	return 0;
}
