int remfils_dialog_run = 0;
Window remfils_dialog_win;
GC remfils_dialog_gc;
Drawable remfils_dialog_drawable;
int remfils_w = 400;
int remfils_h = 20 + 23 * 2;

int remfils_dialog_selected_state;

static const char *shutdown_cmd[] = { "sudo", "shutdown", "-h", "now", NULL};

void remfils_draw_dialog()
{
  int i = 0;
  int x_offset = 20;
  int y_offset = 20;
  int top_pad = 18;
  int selection_height = 23;

  XClearWindow(dpy, remfils_dialog_win);

  // render selected item

  XSetBackground(dpy, remfils_dialog_gc, scheme[SchemeTagsSel][ColFg].pixel);
  XSetForeground(dpy, remfils_dialog_gc, scheme[SchemeTagsSel][ColFg].pixel);
  XFillRectangle(dpy, remfils_dialog_win, remfils_dialog_gc, 0, top_pad + 5 + y_offset * (remfils_dialog_selected_state), remfils_w, selection_height);

  // render text

  XSetBackground(dpy, remfils_dialog_gc, scheme[SchemeTagsNorm][ColBg].pixel);
  XSetForeground(dpy, remfils_dialog_gc, scheme[SchemeTagsNorm][ColFg].pixel);

  int str_len = 27;
  XDrawString(dpy, remfils_dialog_win, remfils_dialog_gc, x_offset, y_offset + top_pad * i, "Press Win+Enter to exit DWM", str_len);
  i++;

  str_len = 6;
  XDrawString(dpy, remfils_dialog_win, remfils_dialog_gc, x_offset, y_offset + top_pad * i, "Cancel", str_len);
  i++;

  str_len = 25;
  XDrawString(dpy, remfils_dialog_win, remfils_dialog_gc, x_offset, y_offset  + top_pad * i, "Press Win+Enter to shutdown", str_len);
  i++;
}

void remfils_close_window()
{
  XDestroyWindow(dpy, remfils_dialog_win);
}

static void remfils_keypress(XKeyEvent *ev)
{
  KeySym keysym = XKeycodeToKeysym(dpy, (KeyCode)ev->keycode, 0);

  if (keysym == XK_k) {
    remfils_dialog_selected_state--;
    if (remfils_dialog_selected_state < -1) remfils_dialog_selected_state = -1;

    remfils_draw_dialog();
  } else if (keysym == XK_j) {
    remfils_dialog_selected_state++;
    if (remfils_dialog_selected_state > 1) remfils_dialog_selected_state = 1;

    remfils_draw_dialog();
  } else {
    remfils_dialog_run = 0;

    if (keysym != XK_Return) {
      remfils_dialog_selected_state = 0;
    }
  }
}

void remfils_run()
{
  remfils_dialog_run = 1;

  XEvent ev;
  while (remfils_dialog_run && !XNextEvent(dpy, &ev)) {
    switch(ev.type) {
    case KeyPress:
      remfils_keypress(&ev.xkey);
    }
  }

  remfils_close_window();

  if (remfils_dialog_selected_state == -1) {
    quit(0);
  } else if (remfils_dialog_selected_state == 1) {
    Arg a[1];
    a[0].v = shutdown_cmd;
    spawn(a);
  }
}

void remfils_open_window()
{
  remfils_dialog_selected_state = 0;
  
  unsigned long black = scheme[SchemeSel][ColBg].pixel;
  unsigned long white = WhitePixel(dpy, screen);

  remfils_dialog_win = XCreateSimpleWindow(dpy, root, (sw - remfils_w) / 2 , (sh - remfils_h) / 2, remfils_w, remfils_h, 5, white, black);

  //remfils_dialog_drawable = XCreatePixmap(dpy, remfils_dialog_win, remfils_w, remfils_h, DefaultDepth(dpy, screen));
  XSetStandardProperties(dpy, remfils_dialog_win, "Closing dwm","Testing", None, NULL, 0, NULL);
    
  remfils_dialog_gc = XCreateGC(dpy, remfils_dialog_win, 0,0);
  XClearWindow(dpy, remfils_dialog_win);
  XMapRaised(dpy, remfils_dialog_win);

  remfils_draw_dialog();
  remfils_run();
}
