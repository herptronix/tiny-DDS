

#include "gui_w_radio_img.h"


/**
 * radio
 */
typedef struct {

  const uint8_t *pCurrentId;  /*pointer to the radio id*/
  uint8_t id;                 /*radio id*/
  uint8_t state;              /*tick / untick*/
  gui_img_t img;

} radio_img_st;


/**
 * local functions
 */
static void RadioDraw(void *g_obj, void *obj);
static void RadioRefresh(void *g_obj, void *obj);


g_obj_st /*@null@*/ *GUI_W_RadioImgAdd(const rect_st *rec, gui_img_t img, const uint8_t *pCurrentId, uint8_t id) {

  g_obj_st *g_obj = NULL, *res = NULL;
  radio_img_st *radio = NULL;
  rect_st lrec;

  /*check parameters*/
  if(rec != NULL) {

    /*allocate a generic object*/
    g_obj = GUI_AddGenericObject();
    if(g_obj != NULL) {

      /*allocate & init the radio*/
      radio = salloc(sizeof(radio_img_st));
      if(radio != NULL) {

        lrec.x = rec->x;
        lrec.y = rec->y;
        lrec.w = lrec.h = SpriteGetWidth(img);

        radio->img = img;
        radio->pCurrentId = pCurrentId;
        radio->id = id;
        if(pCurrentId != NULL) {
          radio->state = *pCurrentId == id? 1: 0;
        }
        else {
          radio->state = 0;
        }

        /*linkage between generic obj & radio*/
        g_obj->rec = lrec;
        g_obj->draw = RadioDraw;
        g_obj->task = RadioRefresh;
        g_obj->obj = radio;
        GUI_ObjSetFocusable(g_obj, true); /*radio is a focusable object*/
        res = g_obj;
      }
    }
  }
  return res;
}


/**
 * @function RadioRefresh
 * @brief radio task; force refresh if radio state is not coherent with the current selected id
 * @param void *_g_obj: generic object
 * @param void *_obj: radio object
 * @return none
 */
static void RadioRefresh(void *_g_obj, void *_obj) {

  g_obj_st *g_obj;
  radio_img_st *radio;
  bool bRefresh = false;

  if(_g_obj != NULL && _obj != NULL) {
    radio = (radio_img_st*) _obj;

    if(radio->pCurrentId != NULL) {
      /*radio id == selected id && state == untick: -> state = tick, force refresh*/
      if(radio->state == 0 && radio->id == *(radio->pCurrentId)) {
        radio->state = 1;
        bRefresh = true;
      }
      /*radio id != selected id && state == tick: -> state = untick, force refresh*/
      else if(radio->state != 0 && radio->id != *(radio->pCurrentId)) {
        radio->state = 0;
        bRefresh = true;
      }
    }
    else {
      radio->state = 0;
    }


    if(bRefresh) {
      g_obj = (g_obj_st *) _g_obj;
      GUI_ObjSetNeedRefresh(g_obj, true);
    }
  }
}


/**
 * @function RadioDraw
 * @brief radio draw function
 * @param void *_g_obj: generic object
 * @param void *_obj: radio object
 * @return none
 */
static void RadioDraw(void *_g_obj, void *_obj) {

  g_obj_st *g_obj;
  radio_img_st *radio;
  uint8_t imgOffset = 0;

  /*retreive generic & specific object*/
  if(_g_obj != NULL && _obj != NULL) {
    g_obj = (g_obj_st *) _g_obj;
    radio =  (radio_img_st*) _obj;

    /*disabled object?*/
    if(GUI_ObjIsDisabled(g_obj)) {
      imgOffset = 4;
    }
    /*pressed?*/
    else if(GUI_ObjIsPressed(g_obj)) {
      if(radio->state) imgOffset = 3;
      else imgOffset = 1;
    }
    /*normal*/
    else {
      if(radio->state) imgOffset = 2;
    }

    /*P2D configuration, display sprite*/
    P2D_SetDisplayMode(DISPLAY_TRANSPARENT);
    Sprite(g_obj->rec.x, g_obj->rec.y, radio->img + imgOffset);
  }
}
