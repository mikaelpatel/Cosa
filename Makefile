all: CLASS.png doxygen

CLASS.png: CLASS.dot
	dot -Tpng CLASS.dot -o CLASS.png

doxygen: doc log
	doxygen 

doc:
	mkdir doc

log:
	mkdir log

