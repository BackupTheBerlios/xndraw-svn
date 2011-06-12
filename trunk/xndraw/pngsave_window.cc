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

#include "pngsave_window.h"
#include <iostream>

namespace xNDRAW{
  using namespace NCOORDS;
  std::string FileDialogManager::s_import("_IMPORT");
  std::string FileDialogManager::s_export("_EXPORT");

  FileDialogManager::FileDialogManager(){}
  FileDialogManager::FileDialogManager(const std::map<std::string, std::string>& map):  
    directories(map){}
  FileDialogManager::~FileDialogManager(){}
std::string 
FileDialogManager::do_filedialog_slot(Gtk::Window& parentwindow,
				   xnFileType ftype, 
				   const std::string& dtitle,
				   filename_slot_type slot,
				      Gtk::FileChooserAction chooseraction){
  filename_signal signal;
  sigc::connection conn = signal.connect(slot);
  std::string outfolder = do_filedialog(parentwindow, ftype,  dtitle,signal,
					chooseraction);
  //unnecessary - signal's destructor will do it, or make it irrelevent
    conn.disconnect();
  return outfolder;
}

  void FileDialogManager::set_folder(xnFileType ftype, 
				     Gtk::FileChooserAction chooseraction, 
				     const std::string& sfolder){
    std::string key= get_key(ftype, chooseraction);
    if(sfolder.size()){
      directories[key] = sfolder;
    }
    else{
      throw xn_Error("empty string");
    }
  }

 std::string 
 FileDialogManager::do_filedialog(Gtk::Window& parentwindow,
				       xnFileType ftype, 
				       const std::string& dtitle,
				       filename_signal callback,
				       Gtk::FileChooserAction chooseraction){
   std::string key = this->get_key(ftype, chooseraction);
  std::string sfolder = get_current_folder(key);
  std::string outfolder = pngsave_window::do_filedialog(parentwindow, dtitle, callback, ftype, sfolder,chooseraction);
  if(outfolder.size()){
    set_current_folder(key, outfolder);
  }
  return outfolder;
 }

  std::string  
  FileDialogManager::get_key(xnFileType ftype, 
			     Gtk::FileChooserAction chooseraction)const{
   std::string skey("unknown");
   std::string s_keytail, err;
   std::ostringstream ostmp;
   switch(chooseraction){
   case Gtk::FILE_CHOOSER_ACTION_SAVE:
     s_keytail = s_export;
     break;
case Gtk::FILE_CHOOSER_ACTION_OPEN:
     s_keytail = s_import;
     break;
   default:
     throw xn_Error("unsupported action");
   }
   switch(ftype){
   case XNDRAW_PNG:
     skey = "png";
     break;
   case XNDRAW_SVGRENDER:
     skey = "svg";
     break;
   case XNDRAW_SVGPATH:
     skey = "svgpath";
     break;
   case XNDRAW_PATH:
     skey = "xnd";
     break;
   case XNDRAW_GRID:
     skey = "nodes";
     break;
   case XNDRAW_PROJECT:
     skey = "xndraw project";
   default:
     ostmp << "filetype '"<< skey<< " : unsupported";
     err = ostmp.str();
     throw xn_Error(err);
   }
 skey += s_keytail;
 return skey;
 }

  std::string  FileDialogManager::get_current_folder(const std::string& key)const{
    const_iterator  iter= directories.find(key);
    if(iter !=directories.end()){
      return (*(iter)).second;
    }
    else{
      std::string def = get_default_path();
      //directories[key]=def;
      return def;
           }
  }

  std::string FileDialogManager::get_default_path()const{
    if(default_path.size()){return default_path;}
    return get_user_home();//or cwd, ~/Pictures and friends on other OS
  }

  void FileDialogManager::set_current_folder(const std::string& key, const std::string& dirpath){
    if(!dirpath.size()){
      throw xn_Error("path the empty string");
    }
    //check if valid path?
    directories[key]=dirpath;
  }



  pngsave_window::pngsave_window(Cairo::RefPtr<Cairo::Context> cairo ,
FileDialogManager* dmanager)
    :
    app_dialog_manager(dmanager),
    m_Button_svgopen("import paths from svg file"),
    m_Button_File("save png"),
    more_button1("alt save png"),
#if CAIRO_HAS_SVG_SURFACE
    more_button2("save svg"),
#else
    more_button2("save svg - disabled"),
#endif
    cairo_context(cairo),
    f_name(""),
    folder("")
  {
    set_title("save a png file,etc");
    std::cout << "in pngsave() constructor" << std::endl;
    add(biggerbox);
    biggerbox.pack_start(bigbox);
    bigbox.pack_start(m_ButtonBox);
    m_ButtonBox.pack_start(m_Button_File);
    m_ButtonBox.pack_start(m_Button_svgopen);
    m_ButtonBox.pack_start(more_button1);
    m_ButtonBox.pack_start(more_button2);

    connect_buttons(); 
    show_all_children();
  }
  pngsave_window::pngsave_window(Cairo::RefPtr<Cairo::Context> cairo , 
				 Glib::ustring current_folder,
				 FileDialogManager* dmanager)
    :
    app_dialog_manager(dmanager),
    m_Button_svgopen("import paths from SVG file"),
    m_Button_File("save png"),
    more_button1("alt savvee png"),
#if CAIRO_HAS_SVG_SURFACE
    more_button2("SAVE svg"),
#else
    more_button2("don't SAVE svg(not available)"),
#endif
    projectpathsbutton("save (project)paths to svg"),
    bg_pngsave("background save:png(testing)"),
    bg_svgsave("background save:svg(testing)"),
    cairo_context(cairo),
    f_name(""),
    folder(current_folder)
  {
    set_title("save a png file,etc");
    std::cout << "in pngsave() constructor" << std::endl;
    add(biggerbox);
    biggerbox.pack_start(bigbox);
    bigbox.pack_start(m_ButtonBox);
    m_ButtonBox.pack_start(m_Button_File);
 
    m_ButtonBox.pack_start(m_Button_svgopen);
  
    m_ButtonBox.pack_start(more_button1);
    m_ButtonBox.pack_start(more_button2);
    m_ButtonBox.pack_start(projectpathsbutton);
    m_ButtonBox.pack_start( bg_pngsave);
    m_ButtonBox.pack_start( bg_svgsave);
    connect_buttons();
    show_all_children();

  }
  pngsave_window::~pngsave_window(){}

  void pngsave_window::connect_buttons(){
    connect(m_Button_File, sigc::mem_fun(*this, &pngsave_window::on_button_file_clicked) );
    connect(m_Button_svgopen,sigc::mem_fun(*this,&pngsave_window::on_svgfile_open));
    more_button1.signal_clicked().connect( sigc::mem_fun(*this, &pngsave_window::on_alt_pngfile_save));
    more_button2.signal_clicked().connect( sigc::mem_fun(*this, &pngsave_window::on_svgfile_save));
    projectpathsbutton.signal_clicked().connect( sigc::mem_fun(*this, &pngsave_window::on_svg_projectpaths_save));
    //bg_pngsave.signal_clicked().connect( sigc::mem_fun(*this, &pngsave_window::on_bg_pngsave));
    //bg_svgsave.signal_clicked().connect( sigc::mem_fun(*this, &pngsave_window::on_bg_svgsave));


  }



  //void pngsave_window::on_coordset_folder_clicked(){}
  //void pngsave_window::on_coordset_file_clicked(){}

  void pngsave_window::on_button_file_clicked(){
    //APP_DIALOG_MANAGER HAS MUCH MORE USEFUL RECORD OF DIRECTORIES FOR VARIOUS TYPE OF FILE
    if(app_dialog_manager){
      app_dialog_manager->do_filedialog(*this, XNDRAW_PNG,"png FILENAME:PLEEEZ, write shit", signal_filename(), Gtk::FILE_CHOOSER_ACTION_SAVE);
    }
    else{
    Gtk::FileChooserDialog dialog("PLEEEZ, write shit", Gtk::FILE_CHOOSER_ACTION_SAVE);
    xnFileType tp=XNDRAW_PNG;
    create_save_dialog(dialog, tp, *this, folder);
    run_filesavedialog(dialog, signal_filename(), tp);
    folder = dialog.get_current_folder();
    }
  }

  void pngsave_window::add_png_filter(Gtk::FileChooserDialog& dialog){
    add_filefilter(dialog, XNDRAW_PNG);
  }

  std::string fix_file_extension(const std::string& filename, 
					 xnFileType tp){
    std::string extn = FileInfo::get_extension(tp);
    return  fix_file_extension(filename,extn);
  }

  std::string fix_file_extension(const std::string& filename, 
					 std::string extension ){
    if(filename.size()> extension.size()){
      std::string extn(filename.end() -4, filename.end());
      if(extn == extension){return filename;}
    }
    return filename + extension;
  }

  void pngsave_window::add_glob_all_filter(Gtk::FileChooserDialog& dialog){

    Gtk::FileFilter filter;
    filter.set_name("everything('*')");
    filter.add_pattern("*");
    dialog.add_filter(filter);
  }

  void pngsave_window::add_some_filter(Gtk::FileChooserDialog& dialog,
				       const std::string& name,
				       const std::string& globstring,
				       std::string mimetype){
    Gtk::FileFilter filter;
    filter.set_name(name.c_str());
     
    if(!globstring.size() && ! mimetype.size()){
      throw xn_Error("no pattern or mimetype provided to add_some_filter()");
    }
    if(globstring.size()){
      filter.add_pattern(globstring.c_str());
    }
    if(mimetype.size()){
      filter.add_mime_type(mimetype.c_str());
    }
    dialog.add_filter(filter);
  }

  void pngsave_window::add_filefilter(Gtk::FileChooserDialog& dialog, xnFileType filetype){
    Gtk::FileFilter filter;
    std::string mimetp = FileInfo::get_mimetype(filetype);
    std::vector<std::string> globs = FileInfo::get_globstrings(filetype);
    std::string nm = FileInfo::get_filetypename(filetype);
    filter.add_mime_type(mimetp.c_str());
    filter.set_name(nm.c_str());
    typedef std::vector<std::string>::const_iterator vector_iterator;
    for(vector_iterator w = globs.begin();w != globs.end();w++){
      filter.add_pattern((*w).c_str());
    }
    dialog.add_filter(filter);
  }

  void pngsave_window::add_svg_filter(Gtk::FileChooserDialog& dialog){
    add_filefilter(dialog, XNDRAW_SVGRENDER);
  }

  Gtk::FileChooserDialog& 
  pngsave_window::create_open_dialog( Gtk::FileChooserDialog& dialog,
				      xnFileType filetype, 
				      Gtk::Window& parentwindow,
				      std::string init_folder){
    dialog.set_transient_for(parentwindow);
    //dialog.set_create_folders(true);
    add_filefilter(dialog,filetype);
    add_glob_all_filter(dialog);
    if(init_folder.size()){
      dialog.set_current_folder(init_folder);
    }
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    return dialog;
  }

  Gtk::FileChooserDialog& 
  pngsave_window::create_save_dialog( Gtk::FileChooserDialog& dialog,
				      xnFileType filetype, 
				      Gtk::Window& parentwindow,
				      std::string init_folder){
    dialog.set_transient_for(parentwindow);
    dialog.set_create_folders(true);
    add_filefilter(dialog,filetype);
    add_glob_all_filter(dialog);
    if(init_folder.size()){
      dialog.set_current_folder(init_folder);
    }
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);
    return dialog;
  }

  std::string pngsave_window::run_filesavedialog(Gtk::FileChooserDialog& dialog, filename_signal emitter,  xnFileType filetype){
    //change to return the current directory(or the empty string? on failure
    int result = dialog.run();
    //Handle the response:
    std::string f_name;
    std::string cwd;
    switch(result){
    case Gtk::RESPONSE_OK:
      f_name = dialog.get_filename(); //use std::string
      f_name = fix_file_extension(f_name, filetype);
      std::cout << "run_filedialog(): dialog.get_filename(): " <<	f_name
		<<"\n dialog.get_current_folder(): " 
		<<  dialog.get_current_folder() << std::endl;
      cwd = dialog.get_current_folder();
      emitter.emit(f_name);
      std::cout << "File name dispatched: " <<  f_name << std::endl;
      break;
    case Gtk::RESPONSE_CANCEL:
      std::cout << "cancelled" << std::endl;
    default:
      break;
    }  
    return cwd;
  }

				    
  std::string pngsave_window::run_fileopendialog(Gtk::FileChooserDialog& dialog, filename_signal emitter,  xnFileType filetype){
    std::string outfolder;    
int result = dialog.run();
    //Handle the response:
    std::string f_name;
    switch(result){
    case Gtk::RESPONSE_OK:
      f_name = dialog.get_filename(); //use std::string
      outfolder = dialog.get_current_folder();
      std::cout << "run_fileopendialog(): dialog.get_filename(): " <<	f_name
		<<"\n dialog.get_current_folder(): " 
		<<  dialog.get_current_folder() << std::endl;
      emitter.emit(f_name);
      std::cout << "File name dispatched: " <<  f_name << std::endl;
      break;
    case Gtk::RESPONSE_CANCEL:
      std::cout << "cancelled" << std::endl;
    default:
      break;
    }  
    return outfolder;  
}



  std::string pngsave_window::do_filedialog(Gtk::Window& parentwindow,
				     const std::string& dialogtitle,
				     filename_signal emitter,  
				     xnFileType filetype,
				     std::string& initfolder,
				     Gtk::FileChooserAction chooseraction){
    Gtk::FileChooserDialog dialog(dialogtitle,  chooseraction);
    std::string outfolder;
    if( chooseraction==Gtk::FILE_CHOOSER_ACTION_SAVE){
      create_save_dialog(dialog, filetype,parentwindow, initfolder);
      outfolder = run_filesavedialog(dialog, emitter, filetype);
    }
    else{
      create_open_dialog(dialog, filetype,parentwindow, initfolder);
      outfolder = run_fileopendialog(dialog, emitter, filetype);
    }
    initfolder=dialog.get_current_folder();
    return outfolder;
  }

  void pngsave_window::do_filedialog(Gtk::Window& parentwindow,
				     const std::string& dialogtitle,
				     filename_signal emitter,  
				     xnFileType filetype,
				     Gtk::FileChooserAction chooseraction){
    Gtk::FileChooserDialog dialog(dialogtitle,  chooseraction);
    if( chooseraction==Gtk::FILE_CHOOSER_ACTION_SAVE){
      create_save_dialog(dialog, filetype,parentwindow);
      run_filesavedialog(dialog, emitter, filetype);
    }
    else{
      create_open_dialog(dialog, filetype,parentwindow);
      run_fileopendialog(dialog, emitter, filetype);
    }
     
  }

  void pngsave_window::on_alt_pngfile_save(){
    do_filedialog(*this, "save a png file" ,signal_altpngsave(), 
		  XNDRAW_PNG, folder, Gtk::FILE_CHOOSER_ACTION_SAVE);
  }

  void pngsave_window::on_svgfile_save(){
#if CAIRO_HAS_SVG_SURFACE
    if(app_dialog_manager){
      app_dialog_manager->do_filedialog(*this, XNDRAW_SVGPATH,"save an svg file", signal_svgsave(), Gtk::FILE_CHOOSER_ACTION_SAVE);
    }
    else{
      std::cerr <<"WARNING: app_dialog_manager: doing filesave dialog the old way" <<std::endl;
      Gtk::FileChooserDialog dialog("save an svg file", Gtk::FILE_CHOOSER_ACTION_SAVE);
      xnFileType tp=XNDRAW_SVGPATH;
      create_save_dialog(dialog,tp, *this, folder);
      run_filesavedialog(dialog,signal_svgsave(), tp);
      folder = dialog.get_current_folder();
    }
#endif
  }

  void pngsave_window::on_svg_projectpaths_save(){
#if CAIRO_HAS_SVG_SURFACE
if(app_dialog_manager){
  app_dialog_manager->do_filedialog(*this, XNDRAW_SVGPATH,"save source(ie input, not output)paths to svg file",signal_svg_projectpaths_save(),Gtk::FILE_CHOOSER_ACTION_SAVE);
 }
 else{
   std::cerr<<"WARNING: no app_dialog_manager - inconsistent filesave path setup"<<std::endl;
   Gtk::FileChooserDialog dialog("save source(ie input, not output)paths to svg file", Gtk::FILE_CHOOSER_ACTION_SAVE);
   xnFileType tp=XNDRAW_SVGPATH;
    create_save_dialog(dialog,tp, *this, folder);
    run_filesavedialog(dialog, signal_svg_projectpaths_save(), tp);
    folder = dialog.get_current_folder();
 }
#endif
  }
  /*
    void on_svg_projectpaths_save(){
    Gtk::FileChooserDialog dialog("save an svg file", Gtk::FILE_CHOOSER_ACTION_SAVE);
  
    xnFileType tp=XNDRAW_SVGPROJECT;
 
    create_save_dialog(dialog,tp, *this, folder);
    run_filedialog(dialog, signal_svg_projectpaths_save(), tp);
    folder = dialog.get_current_folder(); 
  */

  void pngsave_window::on_svgfile_open(){
#if CAIRO_HAS_SVG_SURFACE

#endif
#if CAIRO_HAS_SVG_SURFACE
if(app_dialog_manager){
  app_dialog_manager->do_filedialog(*this, XNDRAW_SVGPATH, "choose path to new svg file",m_signal_svgpathsopen, Gtk::FILE_CHOOSER_ACTION_OPEN);
 }
 else{
   std::cerr<< "WARNING: doing it 'the bad old way'\nthis will probably be made an error"<<std::endl;
    Gtk::FileChooserDialog opener ("choose path to new svg file", Gtk::FILE_CHOOSER_ACTION_OPEN);
    xnFileType tp =XNDRAW_SVGPATH;
    create_open_dialog(opener, tp, *this, folder);
    run_fileopendialog(opener,m_signal_svgpathsopen, tp);
    folder = opener.get_current_folder();
 }
#else
    std::cerr<<"no svg CAIRO_HAS_SVG_SURFACE ==0 or undefined(though this may not matter if python is installed)"<<std::endl;
    return
#endif
      }


  void pngsave_window::on_pngfile_open(){
if(app_dialog_manager){
  app_dialog_manager->do_filedialog(*this, XNDRAW_PNG,"open bg or pattern from png file", m_signal_pngfileopen, Gtk::FILE_CHOOSER_ACTION_OPEN);
 }
 else{
    Gtk::FileChooserDialog opener ("open bg or pattern from png file", 
				   Gtk::FILE_CHOOSER_ACTION_OPEN);
    xnFileType tp =XNDRAW_PNG;
    create_open_dialog(opener, tp, *this, folder);
    run_fileopendialog(opener, m_signal_pngfileopen, tp);
    folder = opener.get_current_folder();
 }
  }

}//namespace xNDRAW
