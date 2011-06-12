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

#include "xnmenus.h"
#include "hframe.h"


using namespace xNDRAW;

xNdrawAction::xNdrawAction(const Glib::ustring& name_, 
			   Gtk::StockID stock_id_, 
			   const Glib::ustring& label_, 
			   const Glib::ustring& tooltip_):
  Gtk::Action(name_, stock_id_, label_, tooltip_), 
  label(label_), 
  tooltip(tooltip_){}

Glib::RefPtr<xNdrawAction>
xNdrawAction::create(const Glib::ustring& name_, 
		     const Glib::ustring& label_, 
		     const Glib::ustring& tooltip_){
  Glib::ustring lab= label_;
  if(!lab.size()){
    lab=name_;
  }
  refptr_type newaction= refptr_type(new xNdrawAction(name_, Gtk::StockID(), lab, tooltip_));
  newaction->reference();
  return newaction;
}

xNdrawAction::xNdrawAction():label(Glib::ustring()), 
			     tooltip(Glib::ustring()){}
xNdrawAction::~xNdrawAction(){
  COUT("virtual ~xNdrawAction()");
}

Glib::RefPtr<xNdrawAction> 
xNdrawAction::create(const Glib::ustring& name_, 
		     const Gtk::StockID id, 
		     const Glib::ustring& label_, 
		     const Glib::ustring& tooltip_){
  Glib::ustring lab= label_;
  if(!lab.size()){
    lab=name_;
  }
  refptr_type newaction= refptr_type(new xNdrawAction(name_, id, lab, tooltip_));
  newaction->reference();
  return newaction;
}


Glib::RefPtr<Gtk::Action>
xNdrawAction::get_gtkaction(){
  return base_refptr_type((Gtk::Action*)this);
}

Glib::ustring& xNdrawAction::get_label(){
  return label;
}

Glib::ustring& xNdrawAction::get_tooltip(){
  return tooltip;
}

//useless
Glib::RefPtr<Gtk::ActionGroup> actiongroup::get_actiongroup(){
  return Glib::RefPtr<Gtk::ActionGroup>((Gtk::ActionGroup*)this);
}

Glib::RefPtr<xNdrawAction> actiongroup::get_action_by_label(const Glib::ustring& s_label){
  std::deque< Glib::RefPtr< Gtk::Action > > 	actions = get_actions();
  typedef std::deque< Glib::RefPtr< Gtk::Action > >::const_iterator const_iterator;
  typedef Glib::RefPtr< xNdrawAction> xnaction_type;

  for(const_iterator w = actions.begin(); w!= actions.end();w++){
    Glib::RefPtr< Gtk::Action > actref= *w;
    if(!(*w)){throw xn_Error("NULL action in list from gtk");}
    xnaction_type action =xnaction_type::cast_dynamic(actref);
    if(!action){
      throw xn_Error("failed to cast to action type");
    }
    Glib::ustring label = action->get_label();
    if(label == s_label){
      return action;
    }
  }
  std::ostringstream ostmp;
  ostmp << "no such label : '" << s_label;
  throw xn_Error(ostmp.str());
}

bool actiongroup::set_slot(Glib::RefPtr<xNdrawAction>& action_, sigc::slot<void>& slot_){
  Glib::RefPtr<xNdrawAction> oldaction = action_;
  remove(action_);
  action_ = xNdrawAction::create(oldaction->get_name(), oldaction->get_label());
  add(action_,slot_);
  return true;
}
Glib::RefPtr<Gtk::ActionGroup> MenuActions::get_actiongroup(){
  return Glib::RefPtr<Gtk::ActionGroup>((Gtk::ActionGroup*)this);
}

Glib::RefPtr<xNdrawAction> MenuActions::get_quitaction(){
  return action_file_quitter;
}

Glib::RefPtr<MenuActions> MenuActions::create(h_frame* parent_){
  Glib::RefPtr<MenuActions> newactions =Glib::RefPtr<MenuActions>(new MenuActions (parent_));
  newactions->reference();
  return  newactions;
}

Glib::RefPtr<ExportActions> ExportActions::create(h_frame* parent_){
  Glib::RefPtr<ExportActions> newexports =Glib::RefPtr<ExportActions>(new ExportActions( parent_));
  newexports->reference();
  return newexports;
}

ExportActions::ExportActions(h_frame* h):parent(h){
  action_export_clickpath = xNdrawAction::create("Export_clickpath" ,"Export clickpath");
  add(action_export_clickpath->get_gtkaction(), sigc::mem_fun(parent->get_pingbuffer(), &ping_buffered::save_history_as_datastream) );
  action_export_nodemap = xNdrawAction::create("Export_nodemap" ,"Export nodemap");
  add(action_export_nodemap->get_gtkaction(),sigc::mem_fun(parent, &h_frame::on_nodes_save));
  action_export_pngsave= xNdrawAction::create("Export_pngsave","Export png", "export a png file");
  add(action_export_pngsave->get_gtkaction(),sigc::mem_fun(*parent  , &h_frame::on_png_save));
  action_export_pngsave_bg= xNdrawAction::create("Export_pngsave_bg","Export png in bg (Warning:experimental)", "export a png file in a background thread\n(Warning:experimental)");
  add(action_export_pngsave_bg->get_gtkaction(),sigc::mem_fun(*parent  , &h_frame::on_bg_pngsave));
  action_export_svgsave= xNdrawAction::create("Export_svgsave", "Export svg(if available)");
  add(action_export_svgsave->get_gtkaction(),sigc::mem_fun(*parent,&h_frame::on_svg_file_export));
  action_export_svgsave_bg= xNdrawAction::create("Export_svgsave_bg","Export svg in bg (Warning:experimental)", "export a svg file in a background thread\n(Warning:experimental)");
  add(action_export_svgsave_bg->get_gtkaction(),sigc::mem_fun(*parent,&h_frame::on_bg_svgsave));
  action_export_pngbung= xNdrawAction::create("Export_pngbung","pngBUNG");
  add(action_export_pngbung->get_gtkaction(),sigc::mem_fun(*parent , &h_frame::on_png_bung));
  action_svg_native_convert= xNdrawAction::create("Svg_Native_Convert", "svg to native paths");
  add(action_svg_native_convert->get_gtkaction(),
      sigc::mem_fun(parent, &h_frame::on_svg_to_native_convert));
  action_import_nodemap = xNdrawAction::create("Import_nodemap" ,"Import nodemap");
  add(action_import_nodemap->get_gtkaction(),sigc::mem_fun(parent, &h_frame::on_nodes_read));
  action_import_clickpath = xNdrawAction::create("ImportClickpath" ,"Import svg paths");
  action_import_xndraw_clickpath = xNdrawAction::create("ImportXndrawClickpath" ,"Import xndraw paths");
  Gtk::AccelKey clickimportkey('i', Gdk::CONTROL_MASK,"<ui>/MenuBar/MenuFile/MenuImport/ImportClickpath");
  add(action_import_clickpath->get_gtkaction(),clickimportkey,
      sigc::mem_fun(parent, &h_frame::on_svg_fileget));
  add(action_import_xndraw_clickpath->get_gtkaction(), sigc::mem_fun(parent, &h_frame::on_native_path_import));
      action_import_png= xNdrawAction::create("Import_pngpattern", "Import png(background/fill)");
      add(action_import_png->get_gtkaction(),sigc::mem_fun(parent, &h_frame::on_png_file_import));
      }

    //break this group up: can ActionGroups contain ActionGroups?
    MenuActions::MenuActions(h_frame* h):parent(h){
    action_filemenu = xNdrawAction::create("MenuFile", "_File");
    action_file_newmenu = xNdrawAction::create("MenuNew", "_New");
    action_file_new_drawaddikt = xNdrawAction::create("New_Drawaddikt", "Drawaddikt");
    action_file_exportmenu= xNdrawAction::create("MenuExport", "_Export");
    action_file_importmenu= xNdrawAction::create("MenuImport", "_Import");
    action_file_quitter= xNdrawAction::create("Quit", Gtk::Stock::QUIT);
    action_editmenu= xNdrawAction::create("MenuEditoo" "_Feditfoo");
    ////
    action_edit_sys_copy= xNdrawAction::create("SysCopy", "_Copy");;
    action_edit_sys_cut= xNdrawAction::create("SysCut", "_Cut");
    action_edit_sys_paste= xNdrawAction::create("SysPaste", "_Paste");
    action_edit_sys_clear= xNdrawAction::create("SysClear", "Clear");
    ///
    action_edit_clear_path= xNdrawAction::create("Clear_Path", "Clear Path");
   

action_patternmenu = xNdrawAction::create("MenuPattern", "Pattern");
   
    action_choose_patternsubmenu= xNdrawAction::create("ChoosePattern", "choose pattern");
    action_choose_pattern_draw= xNdrawAction::create("ChoosePatternDraw","single");
    action_choose_pattern_drawset= xNdrawAction::create("ChoosePatternDrawSet", "multinode");
    action_choose_pattern_rosy= xNdrawAction::create("ChoosePatternRosy", "mosaic feature");

    ///
    action_pattern_flipped = xNdrawAction::create("PatternFlipped","Flipped");
    action_pattern_reversed = xNdrawAction::create("PatternReversed", "reversed");
    action_pattern_drawset_correct= xNdrawAction::create("PatternCorrect", "set pattern correct - IE no line under mouse -TODO: FIXME");
    action_pattern_skip_odd= xNdrawAction::create("PatternSkipOdd", "skip odd");

    Glib::ustring pattern_even_tip("skip drawing nodes with even/DOWN polarity-ie draw about half as many paths(a sparser pattern");
    //action_pattern_skip_even->set_tooltip(pattern_even_tip);
    action_pattern_skip_even= xNdrawAction::create("PatternSkipEven", 
						   "skip even", pattern_even_tip);
 
    submenu_pattern_select= xNdrawAction::create("Select", "select pattern");
 action_windowmenu= xNdrawAction::create("MenuWindow", "Window");
    action_window_view_paths= xNdrawAction::create("WindowViewPaths", "view paths");
    action_window_npathframe= xNdrawAction::create("WindowViewNPathFrame", "view npathframe");
    action_cairomenu = xNdrawAction::create("MenuCairo", "Cairo");
    action_cairo_fill = xNdrawAction::create("Cairo_Fill", "Fill");
    action_cairo_stroke= xNdrawAction::create("Cairo_Stroke", "Stroke");
    action_cairo_stroke_preserve= xNdrawAction::create("Cairo_StrokePreserve", "Stroke Preserve");
    action_cairo_fill_preserve= xNdrawAction::create("Cairo_FillPreserve", "Fill Preserve");
   action_helpmenu = xNdrawAction::create("MenuHelp", "_Help");
   action_help_aboutxndraw= xNdrawAction::create("Help_AboutDialog", "About");

    add(action_filemenu->get_gtkaction());
   add(action_file_newmenu->get_gtkaction());
    add(action_file_new_drawaddikt->get_gtkaction(), sigc::mem_fun(*parent, &h_frame::launch_drawaddikt));
    add(action_file_exportmenu->get_gtkaction());
    add(action_file_importmenu->get_gtkaction());
    add(action_file_quitter->get_gtkaction(), sigc::mem_fun(*parent, &h_frame::on_action_quit));
    add(action_editmenu->get_gtkaction());
    ///
    add(action_edit_sys_copy->get_gtkaction(), sigc::mem_fun(*parent, &h_frame::on_edit_sys_copy));
    add(action_edit_sys_cut->get_gtkaction(), sigc::mem_fun(*parent, &h_frame::on_edit_sys_cut));
    add(action_edit_sys_paste->get_gtkaction(), sigc::mem_fun(*parent, &h_frame::on_edit_sys_paste));
    add(action_edit_sys_clear->get_gtkaction(), sigc::mem_fun(parent->get_pingbuffer(), &ping_buffered::clear_cairo));

    ///
    add(action_edit_clear_path->get_gtkaction(),sigc::mem_fun(parent->get_pingbuffer(), &ping_buffered::clear_cairo));

    ///path combine test functuions- to remove later
 action_edit_rand_pathcombine_OR=xNdrawAction::create("pathcombine_OR","pathcombine OR");
    action_edit_rand_pathcombine_AND=xNdrawAction::create("pathcombine_AND","pathcombine AND");
action_edit_rand_pathcombine_NOTAND=xNdrawAction::create("pathcombine_NOTAND","pathcombine NOTAND");
action_edit_rand_pathcombine_DIFF=xNdrawAction::create("pathcombine_DIFF","pathcombine_DIFF");
 add(action_edit_rand_pathcombine_OR->get_gtkaction(), 
     sigc::mem_fun(parent->get_pingbuffer(),
		   &ping_window::do_rand_OR));
 add(action_edit_rand_pathcombine_AND->get_gtkaction(), 
     sigc::mem_fun(parent->get_pingbuffer(),
		   &ping_window::do_rand_AND));
 add(action_edit_rand_pathcombine_NOTAND->get_gtkaction(), 
     sigc::mem_fun(parent->get_pingbuffer(),
		   &ping_window::do_rand_NOTAND));
 add(action_edit_rand_pathcombine_DIFF->get_gtkaction(), 
     sigc::mem_fun(parent->get_pingbuffer(),
		   &ping_window::do_rand_DIFF));
 ////////////END of rand path combine
    add(action_patternmenu->get_gtkaction());



    add(action_choose_patternsubmenu->get_gtkaction());
    add(action_choose_pattern_draw->get_gtkaction());//needs slot
    add(action_choose_pattern_drawset->get_gtkaction(),
	sigc::mem_fun(*parent, &h_frame::on_reinit_drawset));
    add(action_choose_pattern_rosy->get_gtkaction(), 
	sigc::mem_fun(*parent  , &h_frame::on_init_rosydraw));
    add(action_pattern_flipped->get_gtkaction(),sigc::mem_fun(parent->get_pingbuffer()  , &ping_window::flip_inverted));
    add(action_pattern_reversed->get_gtkaction(),sigc::mem_fun(parent->get_pingbuffer()  , &ping_window::flip_reversed));
    add(action_pattern_drawset_correct->get_gtkaction(),sigc::mem_fun(parent->get_pingbuffer()  , &ping_window::flip_drawset_correct));
    add(action_pattern_skip_odd, sigc::mem_fun(parent->get_pingbuffer() , &ping_window::flip_skip_odd));
    add(submenu_pattern_select->get_gtkaction());
    add(action_pattern_skip_even, sigc::mem_fun(parent->get_pingbuffer() , &ping_window::flip_skip_even));
    add(action_windowmenu->get_gtkaction());
    add(action_window_view_paths, sigc::mem_fun(*parent, &h_frame::on_view_saved_paths));
    add(action_window_npathframe, sigc::mem_fun(*parent, &h_frame::on_pathframe_create));

    add(action_cairomenu->get_gtkaction());
    add(action_cairo_fill->get_gtkaction(), sigc::mem_fun(parent->get_pingbuffer()  , &ping_buffered::fill_history));
    add(action_cairo_stroke->get_gtkaction(), sigc::mem_fun(parent->get_pingbuffer()  , &ping_buffered::stroke_history));

    add(action_cairo_fill_preserve->get_gtkaction(), sigc::mem_fun(parent->get_pingbuffer()  , &ping_buffered::fill_preserve_history));
    add(action_cairo_stroke_preserve->get_gtkaction(), sigc::mem_fun(parent->get_pingbuffer()  , &ping_buffered::stroke_preserve_history));


  add(action_helpmenu->get_gtkaction());
     add(action_help_aboutxndraw->get_gtkaction(), 
	 sigc::mem_fun(*parent, &h_frame::on_show_about_dialog));
   
  }

  uiManager::uiManager(h_frame* h):parent(h){
    std::cerr << "  uiManager::uiManager(export_actiongroup = ExportActions::create(parent);"<< std::endl;
    export_actiongroup = ExportActions::create(parent);
    std::cerr << " menu_actiongroup= MenuActions::create(parent); "<< std::endl;
    menu_actiongroup= MenuActions::create(parent);
    std::cerr << " insert_action_group(export_actiongroup->get_actiongroup()); "<< std::endl;
    insert_action_group(export_actiongroup->get_actiongroup());
    std::cerr << " insert_action_group(menu_actiongroup->get_actiongroup()); "<< std::endl;
    insert_action_group(menu_actiongroup->get_actiongroup());
    //what filename location for this as real xmlfile?     

    Glib::ustring xndraw_ui_info =
      "<ui>"
      "  <menubar name='MenuBar'>"
      "    <menu action='MenuFile'>"
      "      <menu action='MenuNew'>"
      "        <menuitem action='New_Drawaddikt'/>"
      "      </menu>"
      "      <separator/>"
      "      <menu action='MenuExport'>"
      "        <menuitem action='Export_clickpath'/>"
      "        <menuitem action='Svg_Native_Convert'/>"
      "        <menuitem action='Export_nodemap'/>"
      "        <menuitem action='Export_pngsave'/>"
      "        <menuitem action='Export_svgsave'/>"
      "        <menuitem action='Export_pngbung'/>"
      "        <menuitem action='Export_pngsave_bg'/>"
      "        <menuitem action='Export_svgsave_bg'/>"
      "      </menu>"
      "      <menu action='MenuImport'>" 
      "        <menuitem action='Import_nodemap'/>"
      "        <menuitem action='ImportClickpath'/>"
      "        <menuitem action='ImportXndrawClickpath'/>"
      "        <menuitem action='Import_pngpattern'/>"
      "      </menu>"
      "      <separator/>"
      "      <menuitem action='Quit'/>"
      //"    </menu>"//end of file menu
      //"    <menu action='MenuEditoo'>"
      "      <separator/>"
      "      <menuitem action='SysCut'/>"
      "      <menuitem action='SysCopy'/>"
      "      <menuitem action='SysPaste'/>"
      "      <menuitem action='SysClear'/>"
      "      <separator/>"
      "      <menuitem action='Clear_Path'/>"
      "      <menuitem action='pathcombine_OR'/>"
      "      <menuitem action='pathcombine_AND'/>"
      "      <menuitem action='pathcombine_NOTAND'/>"
      "      <menuitem action='pathcombine_DIFF'/>"
      "      <separator/>"
      "    </menu>"
      "    <menu action='MenuPattern'>"
      "      <menu action='ChoosePattern'>"
      "        <menuitem action='ChoosePatternDraw'/>"
      "        <menuitem action='ChoosePatternDrawSet'/>"
      "        <menuitem action='ChoosePatternRosy'/>"
      "      </menu>"
      "      <menuitem action='PatternFlipped'/>"
      "      <menuitem action='PatternCorrect'/>"
      "      <menuitem action='PatternReversed'/>"
      "      <menuitem action='PatternSkipOdd'/>"
      "      <menuitem action='PatternSkipEven'/>"
      "    </menu>"
      "    <menu action='MenuWindow'>"
      "      <menuitem action='WindowViewPaths'/>"
      "      <menuitem action='WindowViewNPathFrame'/>"
      "    </menu>"
      "    <menu action='MenuCairo'>"
      "      <menuitem action='Cairo_Fill'/>"
      "      <menuitem action='Cairo_FillPreserve'/>"
      "      <menuitem action='Cairo_Stroke'/>"
      "      <menuitem action='Cairo_StrokePreserve'/>"
      "    </menu>"
      "    <menu action='MenuHelp'>"
      "      <menuitem action='Help_AboutDialog'/>"
      "    </menu>"
      "  </menubar>"
      "</ui>";
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try
      {  
	//UI_id = add_ui_from_file("xml/ui_info.xml");
	UI_id = add_ui_from_string(xndraw_ui_info);
      }
    catch(const Glib::Error& ex)
      {
	std::cerr << "building menus failed: " <<  ex.what();
      }
#else
    std::auto_ptr<Glib::Error> ex;
    UI_id = add_ui_from_string(xndraw_ui_info);
    if(ex.get())
      { 
	std::cerr << "building menus failed: " <<  ex->what();
      }
#endif //GLIBMM_EXCEPTIONS_ENABLED

  }

Glib::RefPtr<uiManager> uiManager::create(h_frame* parent_){
      Glib::RefPtr<uiManager> newman = Glib::RefPtr<uiManager>(new uiManager( parent_));
      if(!newman){
	throw xn_Error("uiManager NULL");
      }
      newman->reference();
	return newman;
    }

    Glib::RefPtr<ExportActions> uiManager::get_exportactions(){
return export_actiongroup;
    }

Glib::RefPtr<MenuActions>& uiManager::get_menuactions(){
return menu_actiongroup;
    }

  /*
FileDialogManager& wo_man = parent->get_dialog_manager();
wo_man.do_filedialog_slot(*parent, XNDRAW_PNG, "LOAD A BACKGROUND", 
                          SIGC::MEM_FUN(parent->get_buffd_gtk_surf()?, &ping_window::on_load_background), Gtk::FILE_CHOOSER_ACTION_OPEN);
    Background * bg = parent->buffd...get_bg();
    }
    return bg;
    }

  */
