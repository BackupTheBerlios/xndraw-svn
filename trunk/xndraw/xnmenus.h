//-*- c++ -*-

/*  Copyright (C) 2006-2011 John O'Driscoll
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef XNDRAW_XNMENUS_H
#define XNDRAW_XNMENUS_H

#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif


#include "io_uncollide.h"
#include <gtkmm.h>
#include <gtkmm/actiongroup.h>
namespace  xNDRAW{

class h_frame;


  class xNdrawAction:public Gtk::Action{
  public:
    typedef Glib::RefPtr<xNdrawAction> refptr_type;

    

  protected:
    Glib::ustring label;
    Glib::ustring tooltip;

    // why no get_label method in Gtk::Action
  public:
    xNdrawAction();
    virtual ~xNdrawAction();

    xNdrawAction(const Glib::ustring& name_, 
		 Gtk::StockID stock_id_=Gtk::StockID(), 
		 const Glib::ustring& label_=Glib::ustring(), 
		 const Glib::ustring& tooltip_=Glib::ustring());
    
    static refptr_type
    create(const Glib::ustring& name_, 
	   const Glib::ustring& label_=Glib::ustring(), 
	   const Glib::ustring& tooltip_=Glib::ustring());

    static refptr_type create(const Glib::ustring& name_, 
      const Gtk::StockID id, 
      const Glib::ustring& label_=Glib::ustring(), 
      const Glib::ustring& tooltip_=Glib::ustring());
    /*refptr_type create(){
      refptr_type newaction= refptr_type(new xNdrawAction);
      newaction->reference();
      return newaction;
      }*/
    typedef Glib::RefPtr<Gtk::Action> base_refptr_type;
    base_refptr_type get_gtkaction();

    Glib::ustring& get_label();
    Glib::ustring& get_tooltip();

  };
  //doesn't add much
  class actiongroup:public Gtk::ActionGroup{

  public:
    actiongroup(const Glib::ustring& name = Glib::ustring()):Gtk::ActionGroup(name){}
    virtual ~actiongroup(){COUT("virtual ~actiongroup()");}
    
    Glib::RefPtr<Gtk::ActionGroup> get_actiongroup();//useless?
    
    Glib::RefPtr<xNdrawAction> get_action_by_label(const Glib::ustring& s_label);
    bool set_slot(Glib::RefPtr<xNdrawAction>& action_, sigc::slot<void>& slot_);

  };
  
  
  class xNmenu: public Gtk::Menu{};

  class MenuActions : public actiongroup{

  protected:
    h_frame* parent;

    Glib::RefPtr<xNdrawAction> action_filemenu;
    Glib::RefPtr<xNdrawAction> action_file_newmenu;
    Glib::RefPtr<xNdrawAction> action_file_new_drawaddikt;
    Glib::RefPtr<xNdrawAction> action_file_exportmenu;
    Glib::RefPtr<xNdrawAction> action_file_importmenu;
    Glib::RefPtr<xNdrawAction> action_file_quitter;
    Glib::RefPtr<xNdrawAction> action_editmenu;
    Glib::RefPtr<xNdrawAction> action_edit_clear_path;
    //////
    Glib::RefPtr<xNdrawAction> action_edit_sys_copy;
    Glib::RefPtr<xNdrawAction> action_edit_sys_cut;
    Glib::RefPtr<xNdrawAction> action_edit_sys_paste;
    Glib::RefPtr<xNdrawAction> action_edit_sys_clear;
Glib::RefPtr<xNdrawAction> action_edit_rand_pathcombine_OR;
Glib::RefPtr<xNdrawAction> action_edit_rand_pathcombine_AND;
Glib::RefPtr<xNdrawAction> action_edit_rand_pathcombine_NOTAND;
    Glib::RefPtr<xNdrawAction> action_edit_rand_pathcombine_DIFF;
    ////
    Glib::RefPtr<xNdrawAction> action_patternmenu;

    Glib::RefPtr<xNdrawAction> action_choose_patternsubmenu;
    Glib::RefPtr<xNdrawAction>action_choose_pattern_draw;
    Glib::RefPtr<xNdrawAction>action_choose_pattern_drawset;
    Glib::RefPtr<xNdrawAction>action_choose_pattern_rosy;

    Glib::RefPtr<xNdrawAction> action_pattern_flipped;
    Glib::RefPtr<xNdrawAction> action_pattern_reversed;
    Glib::RefPtr<xNdrawAction> action_pattern_skip_odd;
    Glib::RefPtr<xNdrawAction> action_pattern_skip_even;
    Glib::RefPtr<xNdrawAction> action_pattern_drawset_correct;
    Glib::RefPtr<xNdrawAction> submenu_pattern_select;
    Glib::RefPtr<xNdrawAction> action_windowmenu;
    Glib::RefPtr<xNdrawAction> action_window_view_paths;
    Glib::RefPtr<xNdrawAction> action_window_npathframe;
    Glib::RefPtr<xNdrawAction> action_cairomenu;
    Glib::RefPtr<xNdrawAction> action_cairo_fill;
    Glib::RefPtr<xNdrawAction> action_cairo_stroke;
    Glib::RefPtr<xNdrawAction> action_cairo_stroke_preserve;
    Glib::RefPtr<xNdrawAction> action_cairo_fill_preserve;

    Glib::RefPtr<xNdrawAction> action_helpmenu;
    Glib::RefPtr<xNdrawAction> action_help_aboutxndraw;
    //mechanism to load dynamically -Maybe in gtkmm?
  public:
    friend class h_frame;
  public:
    MenuActions(h_frame* parent_);
    virtual ~MenuActions(){COUT("virtual ~MenuActions()");}

    static Glib::RefPtr<MenuActions> create(h_frame* parent_);

    Glib::RefPtr<Gtk::ActionGroup> get_actiongroup();

    Glib::RefPtr<xNdrawAction> get_quitaction();
  };
  class ExportActions : public actiongroup{
  public:
    friend class h_frame;
  protected:
    h_frame* parent;
    Glib::RefPtr<xNdrawAction> action_export_clickpath;
    Glib::RefPtr<xNdrawAction> action_export_nodemap;
    Glib::RefPtr<xNdrawAction> action_export_pngsave;
    Glib::RefPtr<xNdrawAction> action_export_pngsave_bg;

    Glib::RefPtr<xNdrawAction> action_export_svgsave;
    Glib::RefPtr<xNdrawAction> action_export_svgsave_bg;
    Glib::RefPtr<xNdrawAction> action_export_pngbung;
    Glib::RefPtr<xNdrawAction> action_svg_native_convert;
    Glib::RefPtr<xNdrawAction> action_import_nodemap;
    Glib::RefPtr<xNdrawAction>    action_import_clickpath;//svg path
    Glib::RefPtr<xNdrawAction>    action_import_xndraw_clickpath;//xndraw native
    Glib::RefPtr<xNdrawAction>    action_import_png;
  public:
    ExportActions(h_frame* parent_);
    virtual ~ ExportActions(){COUT("virtual ~ ExportActions()");}
    static Glib::RefPtr<ExportActions> create(h_frame* parent_);

   
  
  };

class uiManager: public Gtk::UIManager{

   h_frame* parent;

    Glib::RefPtr<ExportActions> export_actiongroup;
    Glib::RefPtr<MenuActions> menu_actiongroup;

    ui_merge_id UI_id;
  public:
    uiManager(h_frame* parent_);
  virtual ~uiManager(){COUT("virtual ~uiManager()");}

  static Glib::RefPtr<uiManager> create(h_frame* parent_);

  Glib::RefPtr<MenuActions>& get_menuactions();
  Glib::RefPtr<ExportActions> get_exportactions();

  };

}/*namespace  xNDRAW*/

#endif /* XNDRAW_XNMENUS_H*/



