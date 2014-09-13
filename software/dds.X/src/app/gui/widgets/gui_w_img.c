

#include "gui_w_img.h"


/**
 * frame
 */
typedef struct {

  gui_img_t img;
  dmode_t mode;

} img_st;


/**
 * local functions
 */
static void ImgDraw(void *g_obj, void *obj);



g_obj_st /*@null@*/*GUI_W_ImgAdd(const rect_st *rec, gui_img_t img, dmode_t displayMode) {

  g_obj_st *g_obj = NULL, *res = NULL;
  img_st *wimg = NULL;

  /*check parameters*/
  if(rec != NULL) {

    /*allocate a generic object*/
    g_obj = GUI_AddGenericObject();
    if(g_obj != NULL) {

      /*allocate & init the frame*/
      wimg = salloc(sizeof(img_st));
      if(wimg != NULL) {

        wimg->mode = displayMode;
        wimg->img = img;

        /*linkage between generic obj & img*/
        g_obj->rec = *rec;
        g_obj->draw = ImgDraw;
        g_obj->task = NULL;
        g_obj->obj = wimg;
        GUI_ObjSetStatic(g_obj, true); /*frame is static: no need to refresh when pressed*/
        res = g_obj;
      }
    }
  }
  return res;
}


/**
 * @function ImgDraw
 * @brief img draw function
 * @param void *_g_obj: generic object
 * @param void *_obj: img object
 * @return none
 */
static void ImgDraw(void *_g_obj, void *_obj) {

  g_obj_st *g_obj;
  img_st *wimg;

  /*retreive generic & specific object*/
  if(_g_obj != NULL && _obj != NULL) {
    g_obj = (g_obj_st *) _g_obj;
    wimg =  (img_st*) _obj;

    /*P2D configuration*/
    P2D_SetDisplayMode(wimg->mode);

    if(wimg->img != 0) {
      Sprite(g_obj->rec.x, g_obj->rec.y, wimg->img);
    }
    else {
      DrawBox(&(g_obj->rec), 0);
    }

  }
}
