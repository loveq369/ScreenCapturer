
#CC=clang -g 
CFLAGS=`pkg-config --cflags gtk+-3.0`
LIBS=`pkg-config --libs gtk+-3.0` -lm
OBJS= main.o sc-app.o sc-window.o sc-canvas.o sc-operable.o sc-operators.o\
	  sc-arrow.o sc-shape.o sc-text.o sc-painter.o floatBorder.o sc-utils.o\
	  sc-colorchooser.o sc-widthchooser.o sc-fontsizechooser.o\
	  sc-shapechooser.o sc-widthsetter.o

VPATH=menus:floatBorder

.PHONY :clean_src



scapturer:${OBJS}
	${CC} ${CFLAGS} $^ ${LIBS} -o $@

main.o:main.c 
	${CC} ${CFLAGS} -c $< -o $@

sc-app.o: sc-app.c sc-app.h
	${CC} ${CFLAGS} -c $< -o $@

sc-window.o: sc-window.c sc-window.h sc-utils.h ../config.h
	${CC} ${CFLAGS} -c $< -o $@

sc-canvas.o: sc-canvas.c sc-canvas.h sc-utils.h ../config.h
	${CC} ${CFLAGS} -c $< -o $@

sc-operators.o: sc-operators.c sc-operators.h sc-utils.h
	${CC} ${CFLAGS} -c $< -o $@

sc-operble.o: sc-operable.c sc-operable.h
	${CC} ${CFLAGS} -c $< -o $@

sc-shape.o: sc-shape.c sc-shape.h
	${CC} ${CFLAGS} -c $< -o $@

sc-text.o: sc-text.c sc-text.h
	${CC} ${CFLAGS} -c $< -o $@

sc-arrow.o: sc-arrow.c sc-arrow.h
	${CC} ${CFLAGS} -c $< -o $@

sc-painter.o: sc-painter.c sc-painter.h
	${CC} ${CFLAGS} -c $< -o $@

sc-utils.o: sc-utils.c sc-utils.h
	${CC} ${CFLAGS} -c $< -o $@
#
#menus/sc-colorchooser.o: menus/sc-colorchooser.c menus/sc-colorchooser.h
#	${CC} ${CFLAGS} -c $< -o $@
#
#menus/sc-widthchooser.o: menus/sc-widthchooser.c menus/sc-widthchooser.h
#	${CC} ${CFLAGS} -c $< -o $@
#
#menus/sc-shapechooser.o: menus/sc-shapechooser.c menus/sc-shapechooser.h
#	${CC} ${CFLAGS} -c $< -o $@
#
#menus/sc-fontsizechooser.o: menus/sc-fontsizechooser.c menus/sc-fontsizechooser.h
#	${CC} ${CFLAGS} -c $< -o $@
#
#menus/sc-widthsetter.o: menus/sc-widthsetter.c menus/sc-widthsetter.h
#	${CC} ${CFLAGS} -c $< -o $@
#

#VPATH=menus:floatBorder


include menus/Makefile
include floatBorder/Makefile


#floatBorder/floatBorder.o: floatBorder/floatBorder.c floatBorder/floatBorder.h
#	${CC} ${CFLAGS} -c $< -o $@


clean_src: 
	cd menus && ${MAKE} clean_menus
	cd floatBorder && ${MAKE} clean_fb
	-rm *.o
	-rm scapturer





