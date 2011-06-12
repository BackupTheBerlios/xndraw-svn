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


#ifndef X3DRAW_FMATRIX_H
#define X3DRAW_FMATRIX_H

#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif



#include <gtkmm.h>
#include <cairomm/context.h>
#include "guiutils.h"
#include "xndrawdata.h"
#include "dataview.h"

namespace xNDRAW{

  class Matrix_fucker: public XnWindowBase{
  public:
    Matrix_fucker(Cairo::Matrix in_matrix);
   
    virtual ~Matrix_fucker();

    typedef sigc::signal<void, int> baseno_reinit_signal;
    typedef sigc::signal<void, int, int> init_drawings_signal;
  protected:
    baseno_reinit_signal m_signal_baseno_reinit;
init_drawings_signal m_signal_init_drawings;
    sigc::signal<void, unsigned int> m_grid_symmetry_signal;
  public:
    sigc::signal<void, unsigned int> signal_grid_symmetry(){
return  m_grid_symmetry_signal;
    }
    baseno_reinit_signal signal_baseno_reinit();
    init_drawings_signal signal_init_drawings();
    
  protected:
    Cairo::Matrix mat;
    Gtk::VBox main_box;

    Gtk::Frame entry_frame;
    //Gtk::DrawingArea preview;
    Path_textframe* history_frame;
    Data_view datatree;

    Gtk::HBox m_matrix_box;
    Gtk::VBox b_box;
    Gtk::VBox m_matrix_a, m_matrix_b, m_matrix_c;
    Gtk::Adjustment xmove_adjust,ymove_adjust;
    Gtk::Adjustment xx_adjust,xy_adjust, yx_adjust,yy_adjust;
    
    Gtk::SpinButton xx, xy, yx, yy, x_move, y_move;

    Gtk::Button distort, interactive_distort, path_clean, paths_clean;


    Gtk::VBox Base_no_box;
    Gtk::HBox Basecontrols_box, grid_symmetry_box;
    Gtk::Adjustment Base_no_adjust, grid_symmetry_adjustment,
      init_drawings_minadjust, init_drawings_maxadjust;
    Gtk::SpinButton Base_number, grid_symmetry_button,
      init_drawings_min, init_drawings_max;
    Gtk::Button Base_no_OK, grid_symmetry_OK,flipflop_verbose_butt;
  public:

    Path_textframe* get_history_frame();
    const Path_textframe* get_history_frame()const ;
    Data_view& get_datatree();
    const Data_view& get_datatree()const;

    sigc::connection connect_path_clean(sigc::slot<void> slot);
    sigc::connection connect_paths_clean(sigc::slot<void> slot);
    //call when a point added to path etc
    void on_path_modified();
    void on_reset_path();
    void signal_grid_symmetry_changed();
    void treeview_set_draw(Drawingbase* d, sigc::signal<void> signal);

    Drawingbase* get_draw();

    void on_flipflop_verbose();
    bool update_pathframe();
    void on_base_no_OK();

    void set_pathframe(const NCOORDS::simple_click_history& p);
     
    void on_adjust_xx();
    void on_adjust_xy();
    void on_adjust_yx();
    void on_adjust_yy();
    void on_adjust_xmove();
    void on_adjust_ymove();

    void adjust(double xx_, double xy_, double yx_, double yy_, double xmov ,double ymov); 
    typedef sigc::signal<void, Cairo::Matrix&> transform_signal;
    transform_signal m_signal_transform;
    transform_signal signal_transform(){return m_signal_transform;}
 
    void on_distort(){
      signal_transform().emit(mat);
    }
    double get_xx(){
      return mat.xx;
    }

    double get_xy(){
      return mat.xy;
    }

    double get_yx(){
      return mat.yx;
    }

    double get_yy(){
      return mat.yy;
    }

    double get_xmove(){
      return mat.x0;
    }

    double get_ymove(){
      return mat.y0;
    }

  };

}//namespace xNDRAW

#endif /*X3DRAW_FMATRIX_H*/

