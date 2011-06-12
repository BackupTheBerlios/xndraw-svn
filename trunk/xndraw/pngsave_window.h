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


#ifndef X3DRAW_PNGSAVEWINDOW_H
#define X3DRAW_PNGSAVEWINDOW_H
#include "xndrawexceptions.h"


#if HAVE_CONFIG_H
#include "config.h"
#endif
 
#include <gtkmm.h>
#include <gtkmm/filechooser.h>
#include <iostream>
#include <string>
#include "drawing.h"
#include <cairomm/context.h>
#include "x3draw.h"
#include "nodesmap.h"

//#include "hframe.h"
namespace xNDRAW{

 class FileDialogManager;

  class pngsave_window : public Gtk::Window
  {
  public:
    pngsave_window(Cairo::RefPtr<Cairo::Context> cairo,
FileDialogManager* dmanager);
    pngsave_window(Cairo::RefPtr<Cairo::Context> cairo, 
Glib::ustring foldername,
FileDialogManager* dmanager);
    virtual ~pngsave_window();
    typedef sigc::signal<void, Glib::ustring> current_folder_signal;
    typedef sigc::signal<void, std::string> filename_signal;
    typedef sigc::slot<void, std::string> filename_slot_type;
  protected:
    FileDialogManager* app_dialog_manager;

    current_folder_signal m_signal_current_folder;
    filename_signal m_signal_filename;
    filename_signal m_alt_signal_pngfilesave;
    filename_signal m_signal_pngfileopen;
    filename_signal m_signal_svgfilesave;
    filename_signal m_signal_svgpathsopen;
filename_signal m_signal_svg_projectpaths_save;
filename_signal m_signal_bg_svgfilesave;
filename_signal m_signal_bg_pngfilesave;
 protected:
    //Child widgets:
    Gtk::HBox biggerbox;
    Gtk::VBox bigbox;
    std::string filedefaultname;    
    Gtk::VButtonBox m_ButtonBox;
    Gtk::Button  m_Button_svgopen, m_Button_File, more_button1, 
      more_button2,projectpathsbutton;
    Gtk::Button  bg_pngsave,  bg_svgsave;
    Cairo::RefPtr<Cairo ::Context> cairo_context;
    std::string f_name;
    std::string folder;
    
  public:
    current_folder_signal signal_current_folder(){
      return m_signal_current_folder;
   }
    filename_signal signal_svg_projectpaths_save(){
      return m_signal_svg_projectpaths_save;
          }

    sigc::connection connect_svgpathssave(sigc::slot<void, std::string> slot){
      return m_signal_svg_projectpaths_save.connect(slot);
    }

    filename_signal signal_svgsave(){
      return  m_signal_svgfilesave;;
    }
    sigc::connection connect_svgsave(sigc::slot<void, std::string> slot){
      return  m_signal_svgfilesave.connect(slot);
    }
    sigc::connection connect_svgpathsopen(sigc::slot<void, std::string> slot){
      return  m_signal_svgpathsopen.connect(slot);
    }
    
    filename_signal signal_pngopen(){
      return m_signal_pngfileopen;
    }
    sigc::connection connect_pngopen(sigc::slot<void, std::string> slot){
      return  m_signal_pngfileopen.connect(slot);
    }
    filename_signal signal_altpngsave(){
      return m_alt_signal_pngfilesave;
    }
    sigc::connection connect_altpngsave(sigc::slot<void, std::string> slot){
      return  m_alt_signal_pngfilesave.connect(slot);
    }
    filename_signal signal_svgpathsopen(){
      return m_signal_svgpathsopen;
    }
    filename_signal signal_filename(){
      return m_signal_filename;
    }
    

    template<typename EMITTER,typename BINDARG>
    static std::string 
    do_filedialog_with_arg(Gtk::Window& parentwindow,
			   const std::string& dialogtitle,
			   EMITTER emitter,
			   BINDARG arg,
			   xnFileType filetype,
			   Gtk::FileChooserAction chooseraction=  Gtk::FILE_CHOOSER_ACTION_SAVE);

    template<typename EMITTER,typename BINDARG>
    static std::string 
    do_filedialog_with_arg(Gtk::Window& parentwindow,
			   const std::string& dialogtitle,
			   EMITTER emitter,
			   BINDARG arg,  
			   xnFileType filetype,
			   std::string& initfolder,
			   Gtk::FileChooserAction chooseraction=  Gtk::FILE_CHOOSER_ACTION_SAVE);

template <typename EMITTER, typename BINDARG>
static std::string  
run_fileopendialog_with_arg(Gtk::FileChooserDialog& dialog, 
			    EMITTER emitter,  BINDARG arg, xnFileType filetype);

template <typename EMITTER, typename BINDARG>
static std::string  
run_filesavedialog_with_arg(Gtk::FileChooserDialog& dialog,
			    EMITTER emitter,   BINDARG arg, 
			    xnFileType filetype);

   static void do_filedialog(Gtk::Window& parentwindow,
			      const std::string& dialogtitle,
			      filename_signal emitter,  
			      xnFileType filetype,
			      Gtk::FileChooserAction chooseraction=  Gtk::FILE_CHOOSER_ACTION_SAVE);

    static std::string do_filedialog(Gtk::Window& parentwindow,
			      const std::string& dialogtitle,
			      filename_signal emitter,  
			      xnFileType filetype,
			      std::string& initfolder,
			      Gtk::FileChooserAction chooseraction=  Gtk::FILE_CHOOSER_ACTION_SAVE);

    static std::string run_filesavedialog(Gtk::FileChooserDialog& dialog, 
				   filename_signal emitter,  
				   xnFileType filetype);
 static std::string run_fileopendialog(Gtk::FileChooserDialog& dialog, 
				   filename_signal emitter,  
				xnFileType filetype);//doesn't need filetype

 
    //Signal handlers
  public:
    static void add_filefilter(Gtk::FileChooserDialog& dialog, 
			       xnFileType filetype);

    static Gtk::FileChooserDialog& 
    create_save_dialog( Gtk::FileChooserDialog& dialog,
			xnFileType filetype, 
			Gtk::Window& parentwindow,
			std::string init_folder = std::string());

    static Gtk::FileChooserDialog&
    create_open_dialog(Gtk::FileChooserDialog& dialog,
		       xnFileType filetype, 
		       Gtk::Window& parentwindow,
			std::string init_folder = std::string());

    static bool handle_dialog(sigc::slot<void> successslot);

    virtual void on_button_file_clicked();
    
    void on_alt_pngfile_save();
    void on_svgfile_save();
    void on_svgfile_open();
    void on_pngfile_open();
    void on_svg_projectpaths_save();
    sigc::connection connect(Gtk::Button& button, sigc::slot<void> slot){
      return button.signal_clicked().connect(slot);
    }
    //utility
    void connect_buttons();
    void update_initfolder(Gtk::FileChooserDialog& dialog){
      folder=  dialog.get_current_folder();
    }
    static void add_png_filter(Gtk::FileChooserDialog& dialog);
    static void add_svg_filter(Gtk::FileChooserDialog& dialog);
    static void add_glob_all_filter(Gtk::FileChooserDialog& dialog);
    //CONTAINER : stl fwd iterator container of strings
static void add_some_filter(Gtk::FileChooserDialog& dialog,
				const std::string& name,
			    const std::string& pattern,
			    std::string mimetype= std::string());

    template <typename CONTAINER>
    static void add_some_filter(Gtk::FileChooserDialog& dialog,
				const std::string& name,
				const CONTAINER& globstrings,
				CONTAINER mimetypes=CONTAINER()){
      Gtk::FileFilter filter;
      filter.set_name(name.c_str());
      typedef typename CONTAINER::const_iterator const_iterator;
      if(!globstrings.size() && ! mimetypes.size()){
	throw xn_Error("no patterns or mimetypes provided to add_some_filter()");
      }
      for (const_iterator w = globstrings.begin(); w!= globstrings.end();w++){
	filter.add_pattern((*w).c_str());
      }
      for (const_iterator q = mimetypes.begin(); q!= mimetypes.end();q++){
	filter.add_pattern((*q).c_str());
      }
      dialog.add_filter(filter);
    }
    //std::string pngfile_save();
    //std::string pngfile_open();
  };

 class FileDialogManager{
    //TODO: all Gtk::FileChooserDialog action via this
    //H_FRAME can have as a member
    //until such time as there's a Gtk aware application class
  protected:
    std::map<std::string, std::string> directories;
  public:
   //TODO: for non-ascii lamguage
   //typedef std::map<std::string,Glib::ustring> map_type;
    typedef std::map<std::string, std::string> map_type;
    typedef map_type::iterator iterator;
    typedef map_type::const_iterator const_iterator;
   typedef pngsave_window::filename_signal filename_signal;
   typedef pngsave_window::filename_slot_type filename_slot_type;
    FileDialogManager();
    FileDialogManager(const std::map<std::string, std::string>& map);
    ~FileDialogManager();
  protected:
    static std::string s_import;
    static std::string s_export;
    std::string default_path;//empty by default
  public:

   template <typename EMITTER, typename BINDARG>
   std::string do_filedialog_with_arg(Gtk::Window& parentwindow,
				   xnFileType ftype, 
				   const std::string& dtitle,
				      EMITTER emitter,
				      BINDARG emitarg,
				      Gtk::FileChooserAction chooseraction){
std::string key = this->get_key(ftype, chooseraction);
  std::string sfolder = get_current_folder(key);
  std::string outfolder = pngsave_window::do_filedialog_with_arg(parentwindow,dtitle, emitter, emitarg,ftype, sfolder,chooseraction);
if(outfolder.size()){
    set_current_folder(key, outfolder);
  }
  return outfolder;
   }

   template<typename EMITTER,typename SLOT,typename BINDARG>
    std::string 
    do_filedialog_slot_with_arg(Gtk::Window& parentwindow,
				   xnFileType ftype, 
				   const std::string& dtitle,
				EMITTER dummysignal,
				SLOT slot,
				BINDARG emitarg,
				      Gtk::FileChooserAction chooseraction){
     //std::string key = this->get_key(ftype, chooseraction);
 sigc::connection conn = dummysignal.connect(slot);
 std::string outfolder = do_filedialog_with_arg(parentwindow, ftype,  dtitle, dummysignal,emitarg,chooseraction);
conn.disconnect();
  return outfolder;
}
    std::string do_filedialog(Gtk::Window& parentwindow,
				   xnFileType ftype, 
				   const std::string& dtitle,
				   filename_signal emitter,
				   Gtk::FileChooserAction chooseraction);

std::string do_filedialog_slot(Gtk::Window& parentwindow,
				   xnFileType ftype, 
				   const std::string& dtitle,
				   filename_slot_type slot,
				   Gtk::FileChooserAction chooseraction);

void set_folder(xnFileType ftype, 
		Gtk::FileChooserAction chooseraction, 
		const std::string& sfolder);
  
 
  protected:
    std::string get_key(xnFileType ftype, 
			Gtk::FileChooserAction chooseraction =Gtk::FILE_CHOOSER_ACTION_SAVE)const;

   std::string get_current_folder(const std::string& key)const;

    std::string get_default_path()const;

    void set_current_folder(const std::string& key, 
			    const std::string& dirpath);
  };



  std::string fix_file_extension(const std::string& filename, xnFileType tp);
  std::string fix_file_extension(const std::string& filename, 
				 std::string extension = ".png");


}// namespace xNDRAW

#endif //X3DRAW_PNGSAVEWINDOW_H

//template implementations
#include "pngsave_window.tcc"
