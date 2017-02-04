/*
 * Scroom - Generic viewer for 2D data
 * Copyright (C) 2009-2017 Kees-Jan Dijkzeul
 *
 * SPDX-License-Identifier: LGPL-2.1
 */

#ifndef SIDEBARMANAGER_HH
#define SIDEBARMANAGER_HH

#include <map>
#include <string>

#include <gtk/gtk.h>

class SidebarManager
{
private:
  GtkWidget* panelWindow;
  GtkBox* panel;

  std::map<GtkWidget*, GtkWidget*> widgets;

public:
  SidebarManager();
  void setWidgets(GtkWidget* panelWindow, GtkBox* panel);

  void addSideWidget(std::string title, GtkWidget* w);
  void removeSideWidget(GtkWidget* w);
};

#endif
