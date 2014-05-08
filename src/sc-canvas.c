#include<gtk/gtk.h>
#include"sc-canvas.h"
#include"sc-operable.h"
#include"sc-arrow.h"
#include"sc-shape.h"



static SCCanvas* ref_canvas;
#define BUF_SIZE 100




enum{

    PROP_0,
    PROP_X,
    PROP_Y,
    PROP_WIDTH,
    PROP_HEIGHT,
    PROP_PIXBUF

};






struct _SCCanvasPriv{

    GList* pixbufs;
    GtkWidget*appwin;
//    gint current_pixbuf;
//    GtkAllocation position;
    GdkRectangle position;
    
    GtkWidget*operator;
    GdkWindow*window;

    GtkWidget*menu;//GtkBox
    GdkWindow*menuwindow;

    GtkWidget*toolmenu;//GtkBox
    GdkWindow*toolmenuwindow;

    GtkWidget*operable_box;
//    gboolean show_menu;

//    SCOperable* registered_operables[N_OPERABLES];
//    GtkWidget* act_buttons[N_OPERABLES];

};



static void sc_canvas_set_property(GObject*obj,guint prop_id,const GValue*value,GParamSpec*pspec);
static void sc_canvas_get_property(GObject*obj,guint prop_id,GValue*value,GParamSpec*pspec);

static void sc_canvas_realize(GtkWidget*widget);
static void sc_canvas_unrealize(GtkWidget*widget);
static void sc_canvas_size_allocate(GtkWidget*widget,GtkAllocation*allocation);
static void sc_canvas_get_preferred_width(GtkWidget*widget,gint *min,gint *nat);
static void sc_canvas_get_preferred_height(GtkWidget*widget,gint *min,gint *nat);

static void sc_canvas_map(GtkWidget*widget);
static void sc_canvas_unmap(GtkWidget*widget);


//static void sc_canvas_add(GtkContainer*container,GtkWidget*w);
//static void sc_canvas_remove(GtkContainer*container,GtkWidget*w);
//static void sc_canvas_forall(GtkContainer*container,gboolean include_internal,GtkCallback callback, gpointer callback_data);

static gboolean sc_canvas_draw(GtkWidget* widget,cairo_t*cr);



G_DEFINE_TYPE(SCCanvas,sc_canvas,GTK_TYPE_BOX)



static void sc_canvas_class_init(SCCanvasClass* scklass)
{

    GtkWidgetClass*wclass=GTK_WIDGET_CLASS(scklass);

    GObjectClass*oclass=G_OBJECT_CLASS(scklass);

    GtkContainerClass*cclass=GTK_CONTAINER_CLASS(scklass);



    oclass->set_property=sc_canvas_set_property;
    oclass->get_property=sc_canvas_get_property;

    
    wclass->realize=sc_canvas_realize;
    wclass->unrealize=sc_canvas_unrealize;

    wclass->map=sc_canvas_map;
    wclass->unmap=sc_canvas_unmap;
    wclass->size_allocate=sc_canvas_size_allocate;
    wclass->draw=sc_canvas_draw;
/*
    cclass->add=sc_canvas_add;
    cclass->remove=sc_canvas_remove;
    cclass->forall=sc_canvas_forall;
*/
    g_object_class_install_property(oclass,PROP_X,
            g_param_spec_int("x","X","Position.X",0,G_MAXINT,0,G_PARAM_READWRITE));


    g_object_class_install_property(oclass,PROP_Y,
            g_param_spec_int("y","Y","Position.Y",0,G_MAXINT,0,G_PARAM_READWRITE));


    g_object_class_install_property(oclass,PROP_WIDTH,
            g_param_spec_int("width","Width","Position.width",0,G_MAXINT,0,G_PARAM_READWRITE));


    g_object_class_install_property(oclass,PROP_HEIGHT,
            g_param_spec_int("height","Height","Position.height",0,G_MAXINT,0,G_PARAM_READWRITE));


    g_object_class_install_property(oclass,PROP_PIXBUF,
            g_param_spec_pointer("pixbuf","Pixbuf","pixbuf of current selected rectangle",G_PARAM_READWRITE));

    g_type_class_add_private(scklass,sizeof(SCCanvasPriv));

}





static void sc_canvas_init(SCCanvas* scobj)
{
    
    SCCanvasPriv*priv=SC_CANVAS_GET_PRIV(scobj);

    scobj->priv=priv;

    GtkWidget*widget=GTK_WIDGET(scobj);

    gtk_widget_set_has_window(widget,FALSE);


    priv->pixbufs=NULL;
//    priv->current_pixbuf=0;
    priv->operator=NULL;
    priv->menu=NULL;
//    priv->show_menu=FALSE;




}



static void sc_canvas_set_property(GObject*obj,guint prop_id,const GValue*value,GParamSpec*pspec)
{

    SCCanvas*canvas=SC_CANVAS(obj);
    SCCanvasPriv*priv=canvas->priv;

    GdkPixbuf*pf;

    switch(prop_id)
    {
    
        case PROP_X:
            priv->position.x=g_value_get_int(value);
            break;

        case PROP_Y:
            priv->position.y=g_value_get_int(value);

            break;

        case PROP_WIDTH:
            priv->position.width=g_value_get_int(value);

            break;
        case PROP_HEIGHT:
            priv->position.height=g_value_get_int(value);

            break;

        case PROP_PIXBUF:
            pf=gdk_pixbuf_copy((GdkPixbuf*)g_value_get_pointer(value));
            priv->pixbufs=g_list_prepend(priv->pixbufs,pf);
            
            break;
        default:

            G_OBJECT_WARN_INVALID_PROPERTY_ID(obj,prop_id,pspec);

    }

    gtk_widget_queue_resize(GTK_WIDGET(obj));


}


static void sc_canvas_get_property(GObject*obj,guint prop_id,GValue*value,GParamSpec*pspec)
{


    SCCanvas*canvas=SC_CANVAS(obj);

    SCCanvasPriv*priv=canvas->priv;

    GdkPixbuf*pf;
    GList*lastl;


    switch(prop_id)
    {
    
        case PROP_X:
            g_value_set_int(value,priv->position.x);
            break;

        case PROP_Y:
            g_value_set_int(value,priv->position.y);

            break;

        case PROP_WIDTH:
            g_value_set_int(value,priv->position.width);

            break;
        case PROP_HEIGHT:
            g_value_set_int(value,priv->position.height);

            break;
        case PROP_PIXBUF:
//            lastl=g_list_first(priv->pixbufs);
            pf=gdk_pixbuf_copy(priv->pixbufs->data);
            g_value_set_pointer(value,pf);
            break;

        default:

            G_OBJECT_WARN_INVALID_PROPERTY_ID(obj,prop_id,pspec);

    
    }


}


static void sc_canvas_realize(GtkWidget*widget)
{
    gtk_widget_set_realized(widget,TRUE);

    GTK_WIDGET_CLASS(sc_canvas_parent_class)->realize(widget);


    GtkAllocation allocation;

    GdkWindow* parent_window;
    GdkWindow* window;
    GdkWindowAttr attributes;
    gint attributes_mask;

    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;

    gtk_widget_get_allocation(widget,&allocation);


    attributes.window_type=GDK_WINDOW_CHILD;

        attributes.x=priv->position.x;
        attributes.y=priv->position.y;
        attributes.width=priv->position.width;
        attributes.height=priv->position.height;
    
    
    attributes.wclass=GDK_INPUT_OUTPUT;
    attributes.visual=gtk_widget_get_visual(widget);
    attributes.event_mask=gtk_widget_get_events(widget)|
        GDK_EXPOSURE_MASK|
        GDK_BUTTON_PRESS_MASK|
        GDK_BUTTON_RELEASE_MASK|
        GDK_POINTER_MOTION_MASK;

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_FLEUR);

    attributes_mask=GDK_WA_X|GDK_WA_Y|GDK_WA_VISUAL|GDK_WA_CURSOR;

    parent_window=gtk_widget_get_parent_window(widget);

    window=gdk_window_new(parent_window,&attributes,attributes_mask);


    gtk_widget_register_window(widget,window);
    priv->window=window;

    g_object_unref(attributes.cursor);
    if(priv->operator){
        gtk_widget_set_parent_window(priv->operator,priv->window);
//        gtk_widget_set_parent(priv->operator,widget);
    }

//    gtk_style_context_set_background (gtk_widget_get_style_context (widget), window);

//    gtk_container_forall(GTK_CONTAINER(widget),set_parent_win,widget);

/* menuwindow */

        attributes.x=10;
        attributes.y=10;
        attributes.width=1;
        attributes.height=1;

    attributes.cursor=gdk_cursor_new_for_display(gtk_widget_get_display(widget),GDK_FLEUR);
    attributes_mask=GDK_WA_X|GDK_WA_Y|GDK_WA_VISUAL|GDK_WA_CURSOR;

    priv->menuwindow=gdk_window_new(parent_window,&attributes,attributes_mask);
    gtk_widget_register_window(widget,priv->menuwindow);

    if(priv->menu){
        gtk_widget_set_parent_window(priv->menu,priv->menuwindow);
//        gtk_widget_set_parent(priv->menu,widget);
    }


}



static void sc_canvas_unrealize(GtkWidget*widget)
{

    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;
    gtk_widget_set_realized(widget,FALSE);


    gtk_widget_unregister_window(widget,priv->window);
    gdk_window_destroy(priv->window);
    priv->window=NULL;


    gtk_widget_unregister_window(widget,priv->menuwindow);
    gdk_window_destroy(priv->menuwindow);
    priv->menuwindow=NULL;



    GTK_WIDGET_CLASS(sc_canvas_parent_class)->unrealize(widget);


}






static void sc_canvas_size_allocate(GtkWidget*widget,GtkAllocation*allocation)
{

    SCCanvasPriv* priv=SC_CANVAS(widget)->priv;
    
    gtk_widget_set_allocation(widget,allocation);

//    gtk_container_forall(GTK_CONTAINER(widget), allocate_child,widget);
    GTK_WIDGET_CLASS(sc_canvas_parent_class)->size_allocate(widget,allocation);

    if(!gtk_widget_get_realized(widget))
        return;

     g_message("GDK Window Move& Resize");
///     g_print("x:%d, y:%d.. width:%d, height:%d\n\n",
//             allocation->x+priv->position.x,
//             allocation->y+priv->position.y,
//             priv->position.width,
//             priv->position.height);

     gdk_window_move_resize(priv->window,
                 allocation->x+priv->position.x,
                 allocation->y+priv->position.y,
                 priv->position.width,
                 priv->position.height);


     GtkAllocation child_allocation;

     child_allocation.x=0;//allocation->x+priv->position.x;
     child_allocation.y=0;//allocation->y+priv->position.y;
     child_allocation.width= priv->position.width;
     child_allocation.height= priv->position.height;

     if(priv->operator)
         gtk_widget_size_allocate(priv->operator,&child_allocation);
//            gtk_widget_size_allocate(priv->operator,&child_allocation);
 


    GtkRequisition menuR;
    GtkAllocation menuA;


    if(priv->menu)
    {
        g_message("Allocate menu_window");
        gtk_widget_get_preferred_size(priv->menu,&menuR,NULL);

    gdk_window_move_resize(priv->menuwindow,
         allocation->x+10,//+priv->position.x,
         allocation->y+10,//+priv->position.y,
         menuR.width,
         menuR.height);
    
    
    menuA.x=0;//allocation->x;
    menuA.y=0;//allocation->y;
    menuA.width=menuR.width;
    menuA.height=menuR.height;

    gtk_widget_size_allocate(priv->menu,&menuA);
    }

}


static gboolean sc_canvas_draw(GtkWidget* widget,cairo_t*cr)
{

    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;


//    if(priv->show_menu){
    cairo_t*tcr=gdk_cairo_create(priv->menuwindow);
    cairo_set_source_rgba(tcr,0.5,0.5,0.9,0.9);

    cairo_paint(tcr);

 //   g_print("ooo Priv->menu:%x\n",priv->menu);
//    gtk_container_propagate_draw(GTK_CONTAINER(widget),priv->menu,cr);
//        g_message("Show menu");
//    }

    GList*l=priv->pixbufs;
    if(l){
    gdk_cairo_set_source_pixbuf(cr,l->data,priv->position.x,priv->position.y);
    cairo_paint(cr);
    }


    GTK_WIDGET_CLASS(sc_canvas_parent_class)->draw(widget,cr);


    /*
    if(priv->operator){
        gtk_container_propagate_draw(GTK_CONTAINER(widget),priv->operator,cr);
//        g_message("Draw..() has child..");
    }
    */
    return FALSE;

}



static void sc_canvas_map(GtkWidget*widget)
{

    g_message("Mapping...");
    GTK_WIDGET_CLASS(sc_canvas_parent_class)->map(widget);


    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;

//    gtk_container_forall(GTK_CONTAINER(widget),map_child,widget);

    if(priv->window)
        gdk_window_show(priv->window);

    if(priv->menuwindow)
        gdk_window_show(priv->menuwindow);


}


static void sc_canvas_unmap(GtkWidget*widget)
{

    SCCanvasPriv*priv=SC_CANVAS(widget)->priv;

//    gtk_container_forall(GTK_CONTAINER(widget),unmap_child,widget);

    if(priv->window)
        gdk_window_hide(priv->window);

    if(priv->menuwindow)
        gdk_window_hide(priv->menuwindow);


    GTK_WIDGET_CLASS(sc_canvas_parent_class)->unmap(widget);


}




static void sc_canvas_set_child(SCCanvas*canvas,GtkWidget**child,GdkWindow*childwin,GtkWidget*widget)
{

    GtkWidget*oldchild=*child;

    if(oldchild){
        g_object_ref(oldchild);
        gtk_container_remove(GTK_CONTAINER(canvas),oldchild);
    }

    *child=widget;
    if(*child){
        gtk_widget_set_parent_window(widget,childwin);
        gtk_container_add(GTK_CONTAINER(canvas),*child);
    
    }
    if(oldchild){
        g_object_unref(oldchild);
    }

}



/*
static void sc_canvas_add(GtkContainer*container,GtkWidget*w)
{

    SCCanvasPriv*priv=SC_CANVAS(container)->priv;
   
//    gtk_widget_set_parent_window(w,priv->window);
//    GTK_CONTAINER_CLASS(sc_canvas_parent_class)->add(container,w);
  priv->operator=w;
  gtk_widget_set_parent(w,GTK_WIDGET(container));

    gtk_widget_queue_resize(GTK_WIDGET(container));

}





static void sc_canvas_remove(GtkContainer*container,GtkWidget*w)
{

    SCCanvasPriv*priv=SC_CANVAS(container)->priv;
 
//    GTK_CONTAINER_CLASS(sc_canvas_parent_class)->remove(container,w);

    gtk_widget_unparent(w);
    priv->operator=NULL;
//    gtk_widget_set_parent_window(w,NULL);

    gtk_widget_queue_resize(GTK_WIDGET(container));

}




static void sc_canvas_forall(GtkContainer*container,gboolean include_internal,GtkCallback callback, gpointer callback_data)
{

    SCCanvasPriv*priv=SC_CANVAS(container)->priv;

    if(priv->operator)
        (*callback)(priv->operator,callback_data);

    if(priv->menu)
        (*callback)(priv->menu,callback_data);

}



void sc_canvas_reset(SCCanvas*canvas,GtkWidget*w){

    //GtkWidget*child=gtk_bin_get_child(GTK_BIN(canvas));
    SCCanvasPriv*priv=canvas->priv;
    
    
    if(priv->operator){
    gtk_widget_set_parent_window(priv->operator,NULL);
    }
    
    gtk_widget_set_parent(w,GTK_WIDGET(canvas));
    gtk_widget_set_parent_window(w,priv->window);
    priv->operator=w;

    gtk_widget_queue_resize(GTK_WIDGET(canvas));

}
*/


void sc_canvas_add_op(SCCanvas*canvas,GtkWidget* op)
{

    SCCanvasPriv*priv=canvas->priv;

sc_canvas_set_child(canvas,&priv->operator,priv->window,op);
/*
    if(priv->operator)
        gtk_container_remove(GTK_CONTAINER(canvas),priv->operator);


    gtk_widget_set_parent_window(op,priv->window);
    gtk_container_add(GTK_CONTAINER(canvas),op);
    priv->operator=op;
*/
    gtk_widget_queue_resize(GTK_WIDGET(canvas));

}

void sc_canvas_add_me(SCCanvas*canvas,GtkWidget* me)
{
    SCCanvasPriv*priv=canvas->priv;

sc_canvas_set_child(canvas,&priv->menu,priv->menuwindow,me);
/*
    if(priv->menu)
        gtk_container_remove(GTK_CONTAINER(canvas),priv->menu);


    gtk_widget_set_parent_window(me,priv->menuwindow);
    gtk_container_add(GTK_CONTAINER(canvas),me);
    priv->menu=me;
*/
    gtk_widget_queue_resize(GTK_WIDGET(canvas));



}





GtkWidget*sc_canvas_new(int x,int y,int width, int height)
{

    return (GtkWidget*)g_object_new(SC_TYPE_CANVAS,"x",x,"y",y,"width",width,"height",height,NULL);

}




static void undo_cb(GtkWidget*widget,gpointer d)
{
    SCCanvas* canvas=SC_CANVAS(d);
    sc_canvas_undo(canvas);

}

static void save_cb(GtkWidget*widget,gpointer d)
{
    SCCanvas* canvas=SC_CANVAS(d);
    sc_canvas_save(canvas);
    sc_canvas_exit(canvas);

}


static void exit_cb(GtkWidget*widget,gpointer d)
{
    SCCanvas* canvas=SC_CANVAS(d);
    sc_canvas_exit(canvas);

}




GtkWidget* sc_canvas_get_menu(SCCanvas*canvas)//,GtkWidget*menu)//SCOperator* op)
//GtkWidget*sc_canvas_add_menu(SCCanvas*canvas)
{

    SCCanvasPriv*priv=SC_CANVAS(canvas)->priv;


    if(priv->menu)
        return priv->menu;

//    priv->show_menu=TRUE;

    priv->menu=gtk_grid_new();
//        gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);

    //Add MENU ITEMS 
    priv->operable_box= gtk_box_new(GTK_ORIENTATION_HORIZONTAL,1);
    GtkWidget*item_undo= gtk_button_new_with_label("Undo");
    GtkWidget*item_save= gtk_button_new_with_label("Save");
    GtkWidget*item_done= gtk_button_new_with_label("Done");
    GtkWidget*item_exit= gtk_button_new_with_label("Exit");

    g_signal_connect(G_OBJECT(item_undo),"clicked",G_CALLBACK(undo_cb),canvas);
    g_signal_connect(G_OBJECT(item_save),"clicked",G_CALLBACK(save_cb),canvas);
    g_signal_connect(G_OBJECT(item_exit),"clicked",G_CALLBACK(exit_cb),canvas);

//    gtk_box_pack_start(GTK_BOX(priv->menu),priv->operable_box,TRUE,TRUE,0);

    gtk_grid_attach(GTK_GRID(priv->menu), priv->operable_box,0,0,1,1);
    gtk_grid_attach_next_to(GTK_GRID(priv->menu),item_undo,priv->operable_box,GTK_POS_RIGHT,1,1);
    gtk_grid_attach_next_to(GTK_GRID(priv->menu),item_save,item_undo,GTK_POS_RIGHT,1,1);
    gtk_grid_attach_next_to(GTK_GRID(priv->menu),item_done,item_save,GTK_POS_RIGHT,1,1);
    gtk_grid_attach_next_to(GTK_GRID(priv->menu),item_exit,item_done,GTK_POS_RIGHT,1,1);

/*
    gtk_widget_show(priv->operable_box);
    gtk_widget_show(item_undo);
    gtk_widget_show(item_save);
    gtk_widget_show(item_done);
    gtk_widget_show(item_exit);
    gtk_widget_show_all(priv->menu);
*/
    //for propagate_draw();
//    gtk_widget_set_parent(priv->menu,GTK_WIDGET(canvas));
//    gtk_widget_set_parent_window(priv->menu,priv->menuwindow);

    //g_message("Returning Menu");
    return priv->menu;

}




/*
static void operable_clicked(GtkWidget*widget,gpointer d)
{


    SCCanvasPriv*priv=ref_canvas->priv;

    int pos=GPOINTER_TO_INT(d);

    sc_canvas_add_op(ref_canvas,GTK_WIDGET(priv->registered_operables[pos]));

    gtk_widget_show(GTK_WIDGET(priv->registered_operables[pos]));


}

GtkWidget* sc_canvas_add_operater(SCCanvas*canvas, GtkWidget*operable,int pos)
{

    SCCanvasPriv*priv=SC_CANVAS(canvas)->priv;

    if(priv->menu==NULL){
        sc_canvas_get_menu(canvas);
//        g_message("priv->menu:%x,   priv->operble:%x",priv->menu,priv->operable_box);
    }
//  xxx/x* 
    if(priv->registered_operables[pos])
        g_object_unref(priv->registered_operables[pos]);
    
    priv->registered_operables[pos]=operable;

//x* /
//    g_object_ref(operable);
 //   gtk_widget_show(GTK_WIDGET(operable));

//    GtkWidget*toolbutton=SC_OPERABLE_GET_INTERFACE(operable)->toolbutton;
//    gtk_widget_show(toolbutton);

//    priv->act_buttons[pos]=toolbutton;


    ref_canvas=canvas;


    gtk_box_pack_end(GTK_BOX(priv->operable_box),toolbutton,FALSE,FALSE,5);

    g_signal_connect(G_OBJECT(toolbutton),"clicked",G_CALLBACK(operable_clicked),GINT_TO_POINTER(pos));//&info);

//    gtk_widget_queue_resize(GTK_WIDGET(canvas));
    return toolbutton;

}
*/

/*


static void shape_clicked(GtkWidget*widget,gpointer d)
{

SCCanvas*canvas=SC_CANVAS(d);

    GtkWidget*shape=sc_shape_new(canvas);

    sc_canvas_add_op(canvas,shape);
    gtk_widget_show_all(canvas);

}

static void arrow_clicked(GtkWidget*widget,gpointer d)
{

SCCanvas*canvas=SC_CANVAS(d);

    GtkWidget*arrow=sc_arrow_new(canvas);

    sc_canvas_add_op(canvas,arrow);
    gtk_widget_show_all(canvas);

}


static void painter_clicked(GtkWidget*widget,gpointer d)
{

SCCanvas*canvas=SC_CANVAS(d);

    GtkWidget*arrow=sc_painter_new(canvas);

    sc_canvas_add_op(canvas,arrow);
    gtk_widget_show_all(canvas);

}



void sc_canvas_register_operables(SCCanvas*canvas)
{

    SCCanvasPriv*priv=SC_CANVAS(canvas)->priv;

    if(!priv->menu){
    //    sc_canvas_get_menu(canvas);
    sc_canvas_add_me(canvas,sc_canvas_get_menu(canvas));
    
    }
//    ref_canvas=canvas;
//    SCOperable*operable=sc_arrow_new();
//    GtkWidget*bt=sc_canvas_add_operater(canvas,operable,OP_ARROW);


//    operable=sc_shape_new(canvas);

//    bt=sc_canvas_add_operater(canvas,operable,OP_RECT);

    GtkWidget*shape=gtk_button_new_with_label("shape");
    gtk_box_pack_end(GTK_BOX(priv->operable_box),shape,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(shape),"clicked",G_CALLBACK(shape_clicked),canvas);//&info);
/ *
    GtkWidget*arrow=gtk_button_new_with_label("arrow");
    gtk_box_pack_end(GTK_BOX(priv->operable_box),arrow,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(arrow),"clicked",G_CALLBACK(arrow_clicked),canvas);//&info);
* /
    GtkWidget*painter=gtk_button_new_with_label("painter");
    gtk_box_pack_end(GTK_BOX(priv->operable_box),painter,FALSE,FALSE,0);
    g_signal_connect(G_OBJECT(painter),"clicked",G_CALLBACK(painter_clicked),canvas);//&info);


}


*/


void sc_canvas_destroy(SCCanvas *canvas)
{
    g_object_unref(G_OBJECT(canvas));

}


void sc_canvas_set(SCCanvas*canvas,int x, int y, int width, int height)
{

g_object_set(G_OBJECT(canvas),"x",x,"y",y,"width",width,"height",height,NULL);


}

void sc_canvas_set_appwin(SCCanvas*canvas,GtkWidget*appwin)
{
    g_assert(GTK_IS_APPLICATION_WINDOW(appwin));

    SCCanvasPriv*priv=canvas->priv;

    priv->appwin=appwin;

}

void sc_canvas_get(SCCanvas*canvas,int* x, int* y, int* width, int* height)
{

g_object_get(G_OBJECT(canvas),"x",x,"y",y,"width",width,"height",height,NULL);


}



void sc_canvas_set_pixbuf(SCCanvas*canvas ,GdkPixbuf*pf)
{
g_object_set(G_OBJECT(canvas),"pixbuf",pf,NULL);


}


GdkPixbuf* sc_canvas_get_pixbuf(SCCanvas*canvas)
{
    GdkPixbuf* pf;
g_object_get(G_OBJECT(canvas),"pixbuf",&pf,NULL);

return pf;

}


void sc_canvas_step_done(SCCanvas* canvas)
{

    SCCanvasPriv*priv=canvas->priv;

//    g_message("Priv::%x..",priv);
    //take snapshot of current & save to GdkPixbuf
    //append the pixbuf to priv->pixbufs;
    //reset current SCOperable 's content;

/*
    GdkScreen*screen=gtk_widget_get_screen(GTK_WIDGET(canvas));
    GdkWindow*root_win=gdk_screen_get_root_window(screen);

    GdkPixbuf*pf=gdk_pixbuf_get_from_window(root_win,priv->position.x,priv->position.y,
            priv->position.width, priv->position.height);
*/

    GdkPixbuf*pf=gdk_pixbuf_get_from_window(priv->window,0,0,
            priv->position.width, priv->position.height);

    if(pf){
        sc_canvas_set_pixbuf(canvas,pf);
        g_object_unref(pf);
    }

    gtk_widget_queue_resize(GTK_WIDGET(canvas));

}




void sc_canvas_undo(SCCanvas* canvas)
{

    SCCanvasPriv*priv=canvas->priv;

    GList*llink=priv->pixbufs;
    if(llink==NULL)
        return;
    priv->pixbufs=g_list_remove_link(priv->pixbufs,llink);

    //free pixbuf;
    g_object_unref(llink->data);
    g_list_free(llink);


    gtk_widget_queue_resize(GTK_WIDGET(canvas));


}

void sc_canvas_done(SCCanvas* canvas)
{

    SCCanvasPriv*priv=canvas->priv;
//TODO 
//save to clipborder


}

void sc_canvas_save(SCCanvas* canvas)
{
    SCCanvasPriv*priv=canvas->priv;

    extern time_t Timeval;
    struct tm* tmm=localtime(&Timeval);
    char timespec[BUF_SIZE];

    strftime(timespec,BUF_SIZE,"%H%M%S",tmm);
    


    GtkWidget*dialog=gtk_file_chooser_dialog_new("Save Picture",GTK_WINDOW(priv->appwin),GTK_FILE_CHOOSER_ACTION_SAVE,
            "_Save",GTK_RESPONSE_ACCEPT,"_Cancel",GTK_RESPONSE_CANCEL,NULL);

gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog),TRUE);

char*filename=g_strdup_printf("screencaputer-%s.png",timespec);

    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog),filename);

    g_free(filename);

    char*savfilename;

    int res=gtk_dialog_run(GTK_DIALOG(dialog));

    if(res==GTK_RESPONSE_ACCEPT){
    
        savfilename=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        g_message("Filename::%s",filename);

    }else if(res==GTK_RESPONSE_CANCEL){
    
    
    }

    gtk_widget_destroy(dialog);


    GdkPixbuf*savepf=sc_canvas_get_pixbuf(canvas);

    gdk_pixbuf_save(savepf,savfilename,"png",NULL,NULL);

    g_free(savfilename);
    g_object_unref(savepf);



}

void sc_canvas_exit(SCCanvas* canvas)
{
    SCCanvasPriv*priv=canvas->priv;

    GtkApplication*app=gtk_window_get_application(GTK_WINDOW(priv->appwin));
    g_application_quit(G_APPLICATION(app));

}



GtkWidget*sc_canvas_get_operable_box(SCCanvas*canvas)
{
    return canvas->priv->operable_box;

}



