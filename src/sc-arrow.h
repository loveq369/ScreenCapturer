#ifndef _SC_H_ARROW_

#define _SC_H_ARROW_



#include<gtk/gtk.h>
#include"sc-canvas.h"

#define SC_TYPE_ARROW (sc_arrow_get_type())
#define SC_ARROW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),SC_TYPE_ARROW,SCArrow))
#define SC_IS_ARROW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),SC_TYPE_ARROW))





typedef struct _SCArrow{

    GtkWidget parent;

    GdkWindow* event_window;

    SCCanvas* canvas;

    int arrow_type;
    GdkRectangle rectangle;
    int x0,y0;
    int x1,y1;

    GdkRGBA color;
    gint line_width;

    gboolean pressed;


}SCArrow;



typedef struct _SCArrowClass{

    GtkWidgetClass parent_class;



}SCArrowClass;







GType sc_arrow_get_type(void);


SCOperable* sc_arrow_new();



#endif
