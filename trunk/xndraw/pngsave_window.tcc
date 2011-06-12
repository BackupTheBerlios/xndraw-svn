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


#ifndef XNDRAW_PNGSAVEWINDOW_TCC
#define XNDRAW_PNGSAVEWINDOW_TCC

namespace xNDRAW{

template <typename EMITTER, typename BINDARG>
std::string 
pngsave_window::run_filesavedialog_with_arg(Gtk::FileChooserDialog& dialog, 
					EMITTER emitter,  
					BINDARG arg, xnFileType filetype){
  std::string outfolder;
  int result = dialog.run();
  //Handle the response:
  std::string f_name;
  switch(result){
  case Gtk::RESPONSE_OK:
    f_name = dialog.get_filename(); //use std::string
    outfolder = dialog.get_current_folder();
    f_name = fix_file_extension(f_name, filetype);
    std::cout << "run_filedialog(): dialog.get_filename(): " <<	f_name
	      <<"\n dialog.get_current_folder(): " 
	      <<  dialog.get_current_folder() << std::endl;
    emitter.emit(f_name, arg);
    std::cout << "File name dispatched: " <<  f_name << std::endl;
    break;
  case Gtk::RESPONSE_CANCEL:
    std::cout << "cancelled" << std::endl;
  default:
    break;
  } 
  return outfolder; 
}


template <typename EMITTER, typename BINDARG>
std::string
pngsave_window::run_fileopendialog_with_arg(Gtk::FileChooserDialog& dialog, 
					    EMITTER emitter,  
					    BINDARG arg, xnFileType filetype){
  std::string outfolder;
  int result = dialog.run();
  //Handle the response:
  std::string f_name;
  switch(result){
  case Gtk::RESPONSE_OK:
    f_name = dialog.get_filename(); //use std::string
    outfolder= dialog.get_current_folder();
    std::cout << "run_fileopendialog(): dialog.get_filename(): " <<	f_name
	      <<"\n dialog.get_current_folder(): " 
	      <<  dialog.get_current_folder() << std::endl;
    emitter.emit(f_name, arg);
    std::cout << "File name dispatched: " <<  f_name << std::endl;
    break;
  case Gtk::RESPONSE_CANCEL:
    std::cout << "cancelled" << std::endl;
  default:
    break;
  } 
  return outfolder; 
}
  //use version which has an initdir arg
template <typename EMITTER, typename BINDARG>
std::string
pngsave_window::do_filedialog_with_arg(Gtk::Window& parentwindow,
			      const std::string& dialogtitle,
			      EMITTER emitter,  
				   BINDARG arg,
			      xnFileType filetype,
				   Gtk::FileChooserAction chooseraction){
  //std::string outfolder;
  std::string initer(".");
  return pngsave_window::do_filedialog_with_arg(parentwindow, dialogtitle, emitter,  arg,filetype, initer, chooseraction);
}

  template <typename EMITTER, typename BINDARG>
  std::string
 pngsave_window::do_filedialog_with_arg(Gtk::Window& parentwindow,
			      const std::string& dialogtitle,
			      EMITTER emitter,  
				   BINDARG arg,
			      xnFileType filetype,
			      std::string& initfolder,
			      Gtk::FileChooserAction chooseraction){
    std::string outfolder;
      Gtk::FileChooserDialog dialog(dialogtitle,  chooseraction);
      if( chooseraction==Gtk::FILE_CHOOSER_ACTION_SAVE){
	create_save_dialog(dialog, filetype,parentwindow, initfolder);
	outfolder = run_filesavedialog_with_arg(dialog, emitter, arg,filetype);
      }
      else{
	create_open_dialog(dialog, filetype,parentwindow, initfolder);
	outfolder = run_fileopendialog_with_arg(dialog, emitter, arg, filetype);
      }
      initfolder=dialog.get_current_folder();
      return outfolder;
    }




} //namespace xNDRAW
#endif
