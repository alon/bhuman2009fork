SOURCES=constants.rb \
errors.rb \
language_elements.rb \
symbol_table.rb \
symbols.rb \
syntax_tree_builder.rb \
tokenizer.rb \
translator.rb \
translator_base.rb \
xml_parser.rb \
xabsl.rb \
vcproj.rb

.PHONY: basic parser clean 

all: basic

basic: parser timestamp

timestamp: $(SOURCES) Makefile
	touch timestamp
	
doc:
	./doc.sh

parser: xabsl_parser.tab.rb

xabsl_parser.tab.rb: xabsl_parser.y.rb
	ruby racc/racc -v xabsl_parser.y.rb

clean:
	rm -f *~ *.bak testcases/*~ testcases/*.bak testcases/*.xml testcases/*.dtd
	rm -f xabsl_parser.output xabsl_parser.tab.rb
	rm -rf test1 test2 test3 test4
	rm -f testcases/*.xml 
	rm -rf doc
	rm -f timestamp
