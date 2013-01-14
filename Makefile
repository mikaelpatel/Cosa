all: CLASS.png

CLASS.png: CLASS.dot
	dot -Tpng CLASS.dot -o CLASS.png

