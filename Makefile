all: CLASS.png doxygen

CLASS.png: CLASS.dot
	dot -Tpng CLASS.dot -o CLASS.png

doxygen: documentation log
	doxygen 

documentation:
	mkdir documentation

log:
	mkdir log

