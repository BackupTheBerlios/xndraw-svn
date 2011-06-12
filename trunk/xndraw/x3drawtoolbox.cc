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



#include "x3drawtoolbox.h"

namespace xNDRAW{
  using namespace NCOORDS;

  X3drawtoolbox::X3drawtoolbox(std::string window_title)
    :choos(*this),
     CLEAR ("CLEAR"), SOURCE("SOURCE"), OVER("OVER"), IN ("IN"), OUT ("OUT"),
     ATOP ("ATOP"), DEST ("DEST"), DEST_OVER ("DEST_OVER"), DEST_IN ("DEST_IN"), 
     DEST_OUT ("DEST_OUT"), DEST_ATOP ("DEST_ATOP"), XOR ("XOR"), ADD ("ADD"), 
     SATURATE ("SATURATE"),
     //TESTING -COMPOSITING OPERATORS FORVARIOS cairo_t in drawingbase
     CLEAR_visual("CLEAR_visual"), SOURCE_visual(" SOURCE_vis"), 
     OVER_visual("OVER_vis"), IN_visual("IN_vis"), OUT_visual("OUT_vis"), 
     ATOP_visual("ATOP_vis"), DEST_visual("DEST_vis"), DEST_OVER_visual("DEST_OVER_vis"), 
     DEST_IN_visual(" DEST_IN_vis"), DEST_OUT_visual("DEST_OUT_vis"),
     DEST_ATOP_visual("DEST_ATOP_vis"), XOR_visual("XOR_vis"), 
     ADD_visual("ADD_vis"), SATURATE_visual("SATURATE_vis"),
     CLEAR_buffer("CLEAR_buf"), SOURCE_buffer("SOURCE_buf"), 
     OVER_buffer("OVER_buf"), IN_buffer("IN_buf"), OUT_buffer("OUT_buf"), 
     ATOP_buffer("ATOP_bu"), DEST_buffer("DEST_buf"), 
     DEST_OVER_buffer(" DEST_OVER_buf"), DEST_IN_buffer("DEST_IN_buf"), 
     DEST_OUT_buffer("DEST_OUT_buf"), DEST_ATOP_buffer("DEST_ATOP_buf"), 
     XOR_buffer("XOR_buf"), ADD_buffer("ADD_buf"), 
     SATURATE_buffer(" SATURATE_buf"),
     MITER("MITER"),ROUND("ROUND"), BEVEL("BEVEL"),
   
     WINDING("WINDING"),EVEN_ODD("EVEN_ODD"),

     BUTT("BUTT"), ROUNDY("ROUNDY"),SQUARE("SQUARE")
    
  {
    //drawprog = &X3_draw::instance();
    set_title(window_title);
    std::cout << "in x3drawtoolbox() constructor" << std::endl;
  
    Gtk::RadioButton::Group group = OVER.get_group();
    OVER.set_active();
    CLEAR.set_group(group);
    SOURCE.set_group(group);
    IN.set_group(group);
    OUT.set_group(group);
    ATOP.set_group(group);
    DEST.set_group(group);
    DEST_OVER.set_group(group);
    DEST_IN.set_group(group);
    DEST_OUT.set_group(group);
    DEST_ATOP.set_group(group);
    XOR.set_group(group);
    ADD.set_group(group);
    SATURATE.set_group(group);
    //////

    Gtk::RadioButton::Group group_buffer = OVER_buffer.get_group();
    OVER_buffer.set_active();
    CLEAR_buffer.set_group(group_buffer);
    SOURCE_buffer.set_group(group_buffer);
    IN_buffer.set_group(group_buffer);
    OUT_buffer.set_group(group_buffer);
    ATOP_buffer.set_group(group_buffer);
    DEST_buffer.set_group(group_buffer);
    DEST_OVER_buffer.set_group(group_buffer);
    DEST_IN_buffer.set_group(group_buffer);
    DEST_OUT_buffer.set_group(group_buffer);
    DEST_ATOP_buffer.set_group(group_buffer);
    XOR_buffer.set_group(group_buffer);
    ADD_buffer.set_group(group_buffer);
    SATURATE_buffer.set_group(group_buffer);
    /////////
    Gtk::RadioButton::Group group_visual = OVER_visual.get_group();
    OVER_visual.set_active();
    CLEAR_visual.set_group(group_visual);
    SOURCE_visual.set_group(group_visual);
    IN_visual.set_group(group_visual);
    OUT_visual.set_group(group_visual);
    ATOP_visual.set_group(group_visual);
    DEST_visual.set_group(group_visual);
    DEST_OVER_visual.set_group(group_visual);
    DEST_IN_visual.set_group(group_visual);
    DEST_OUT_visual.set_group(group_visual);
    DEST_ATOP_visual.set_group(group_visual);
    XOR_visual.set_group(group_visual);
    ADD_visual.set_group(group_visual);
    SATURATE_visual.set_group(group_visual);
    /////
  
    Gtk::RadioButton::Group join_group = MITER.get_group();
    ROUND.set_group(join_group);
    BEVEL.set_group(join_group);
  
    Gtk::RadioButton::Group fillgroup = WINDING.get_group();
    EVEN_ODD.set_group(fillgroup);
  
    Gtk::RadioButton::Group capgroup = BUTT.get_group();
    ROUNDY.set_group(capgroup);
    SQUARE.set_group(capgroup);

    add(biggerbox);
    biggerbox.pack_start(bigbox);
    biggerbox.pack_start(op_ButtonBox,Gtk::PACK_SHRINK);
    biggerbox.pack_start(op_ButtonBox_visual,Gtk::PACK_SHRINK);
    biggerbox.pack_start(op_ButtonBox_buffer,Gtk::PACK_SHRINK);

    biggerbox.pack_start(choos,Gtk::PACK_SHRINK);
    bigbox.pack_start(join_box,Gtk::PACK_SHRINK);
    bigbox.pack_start(cap_box,Gtk::PACK_SHRINK);
    join_box.pack_start(*Gtk::manage(new Gtk::Label("set fill style", 0)),  Gtk::PACK_SHRINK);
    join_box.pack_start(WINDING);

    join_box.pack_start(EVEN_ODD);
    bigbox.pack_start(m_ButtonBox);
    join_box.pack_start(*Gtk::manage(new Gtk::Label("set line join", 0)),  Gtk::PACK_SHRINK);
    join_box.pack_start(MITER);
    join_box.pack_start(ROUND);
    join_box.pack_start(BEVEL);
    cap_box.pack_start( BUTT);
    cap_box.pack_start(ROUNDY);
    cap_box.pack_start(SQUARE, Gtk::PACK_SHRINK);
    op_ButtonBox.pack_start(OVER, Gtk::PACK_SHRINK);
    op_ButtonBox.pack_start(CLEAR, Gtk::PACK_SHRINK);
    op_ButtonBox.pack_start(SOURCE, Gtk::PACK_SHRINK);
    op_ButtonBox.pack_start(IN, Gtk::PACK_SHRINK);
    op_ButtonBox.pack_start(OUT, Gtk::PACK_SHRINK);
    op_ButtonBox.pack_start( ATOP, Gtk::PACK_SHRINK);
    op_ButtonBox.pack_start( DEST, Gtk::PACK_SHRINK);
    op_ButtonBox.pack_start(DEST_OVER, Gtk::PACK_SHRINK);
    op_ButtonBox.pack_start(DEST_IN, Gtk::PACK_SHRINK);
    op_ButtonBox.pack_start(DEST_OUT, Gtk::PACK_SHRINK);
    op_ButtonBox.pack_start(DEST_ATOP, Gtk::PACK_SHRINK);
    op_ButtonBox.pack_start(XOR, Gtk::PACK_SHRINK);
    op_ButtonBox.pack_start(ADD, Gtk::PACK_SHRINK);
    op_ButtonBox.pack_start(SATURATE, Gtk::PACK_SHRINK);
    op_ButtonBox.pack_start(*Gtk::manage(new Gtk::Label(" set the\ncompositing\noperator...", 0)),  Gtk::PACK_SHRINK);
    op_ButtonBox_buffer.pack_start(CLEAR_buffer, Gtk::PACK_SHRINK);
    op_ButtonBox_buffer.pack_start(SOURCE_buffer, Gtk::PACK_SHRINK);
    op_ButtonBox_buffer.pack_start(IN_buffer, Gtk::PACK_SHRINK);
    op_ButtonBox_buffer.pack_start(OUT_buffer, Gtk::PACK_SHRINK);
    op_ButtonBox_buffer.pack_start( ATOP_buffer, Gtk::PACK_SHRINK);
    op_ButtonBox_buffer.pack_start( DEST_buffer, Gtk::PACK_SHRINK);
    op_ButtonBox_buffer.pack_start(DEST_OVER_buffer, Gtk::PACK_SHRINK);
    op_ButtonBox_buffer.pack_start(DEST_IN_buffer, Gtk::PACK_SHRINK);
    op_ButtonBox_buffer.pack_start(DEST_OUT_buffer, Gtk::PACK_SHRINK);
    op_ButtonBox_buffer.pack_start(DEST_ATOP_buffer, Gtk::PACK_SHRINK);
    op_ButtonBox_buffer.pack_start(XOR_buffer, Gtk::PACK_SHRINK);
    op_ButtonBox_buffer.pack_start(ADD_buffer, Gtk::PACK_SHRINK);
    op_ButtonBox_buffer.pack_start(SATURATE_buffer, Gtk::PACK_SHRINK);
    ///
    op_ButtonBox_visual.pack_start(CLEAR_visual, Gtk::PACK_SHRINK);
    op_ButtonBox_visual.pack_start(SOURCE_visual, Gtk::PACK_SHRINK);
    op_ButtonBox_visual.pack_start(IN_visual, Gtk::PACK_SHRINK);
    op_ButtonBox_visual.pack_start(OUT_visual, Gtk::PACK_SHRINK);
    op_ButtonBox_visual.pack_start( ATOP_visual, Gtk::PACK_SHRINK);
    op_ButtonBox_visual.pack_start( DEST_visual, Gtk::PACK_SHRINK);
    op_ButtonBox_visual.pack_start(DEST_OVER_visual, Gtk::PACK_SHRINK);
    op_ButtonBox_visual.pack_start(DEST_IN_visual, Gtk::PACK_SHRINK);
    op_ButtonBox_visual.pack_start(DEST_OUT_visual, Gtk::PACK_SHRINK);
    op_ButtonBox_visual.pack_start(DEST_ATOP_visual, Gtk::PACK_SHRINK);
    op_ButtonBox_visual.pack_start(XOR_visual, Gtk::PACK_SHRINK);
    op_ButtonBox_visual.pack_start(ADD_visual, Gtk::PACK_SHRINK);
    op_ButtonBox_visual.pack_start(SATURATE_visual, Gtk::PACK_SHRINK);

    connect_internal_signals();
 
 
    show_all_children();
    std::cout << "leaving x3drawtoolbox CTor" << std::endl;
  }

  X3drawtoolbox::~X3drawtoolbox()
  {
  }

  void X3drawtoolbox::connect_internal_signals(){
    //compositing operators
    CLEAR.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_CLEAR)); 
    SOURCE.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_SOURCE));
    OVER.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_OVER));
    IN.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_IN));
    OUT.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_OUT));
    ATOP.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_ATOP));
    DEST.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_DEST));
    DEST_OVER.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_DEST_OVER));
    DEST_IN.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_DEST_IN));
    DEST_OUT.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_DEST_OUT));
    DEST_ATOP.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_DEST_ATOP));
    XOR.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_XOR));
    ADD.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_ADD));
    SATURATE.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_SATURATE));
 
    //////////////
    CLEAR_buffer.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_buffer_CLEAR)); 
    SOURCE_buffer.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_buffer_SOURCE));
    OVER_buffer.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_buffer_OVER));
    IN_buffer.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_buffer_IN));
    OUT_buffer.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_buffer_OUT));
    ATOP_buffer.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_buffer_ATOP));
    DEST_buffer.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_ATOP));
    DEST_buffer.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_buffer_DEST));
    DEST_OVER_buffer.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_buffer_DEST_OVER));
    DEST_IN_buffer.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_buffer_DEST_IN));
    DEST_OUT_buffer.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_buffer_DEST_OUT));
    DEST_ATOP_buffer.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_buffer_DEST_ATOP));
    XOR_buffer.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_buffer_XOR));
    ADD_buffer.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_buffer_ADD));
    SATURATE_buffer.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_buffer_SATURATE));

    //////////////

    CLEAR_visual.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_visual_CLEAR)); 
    SOURCE_visual.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_visual_SOURCE));
    OVER_visual.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_visual_OVER));
    IN_visual.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_visual_IN));
    OUT_visual.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_visual_OUT));
    ATOP_visual.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_visual_ATOP));
    DEST_visual.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_visual_DEST));
    DEST_OVER_visual.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_visual_DEST_OVER));
    DEST_IN_visual.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_visual_DEST_IN));
    DEST_OUT_visual.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_visual_DEST_OUT));
    DEST_ATOP_visual.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_visual_DEST_ATOP));
    XOR_visual.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_visual_XOR));
    ADD_visual.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_visual_ADD));
    SATURATE_visual.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_visual_SATURATE));
    //////////////
    //////////////
    //linejoin
    MITER.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_MITER));
    ROUND.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_ROUND));
    BEVEL.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_BEVEL));

    //fill type
    WINDING.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_WINDING));
    EVEN_ODD.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_EVEN_ODD));

    //linecap
    BUTT.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_BUTT));
    ROUNDY.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_ROUNDY));
    SQUARE.signal_clicked().connect(sigc::mem_fun(*this, &X3drawtoolbox::on_button_SQUARE));
    std::cout << "leaving x3drawtoolbox() constructor" << std::endl;
  }

  X3drawtoolbox::compositing_signal X3drawtoolbox::signal_operator(){
    return m_signal_compositing;
  }

  void X3drawtoolbox::on_button_CLEAR(){
    signal_operator().emit(Cairo::OPERATOR_CLEAR);
  }
  void X3drawtoolbox::on_button_OVER(){
    signal_operator().emit(Cairo::OPERATOR_OVER);
  }
  void X3drawtoolbox::on_button_SOURCE(){
    signal_operator().emit(Cairo::OPERATOR_SOURCE);
  }
  void X3drawtoolbox::on_button_IN(){
    signal_operator().emit(Cairo::OPERATOR_IN);
  }
  void X3drawtoolbox::on_button_DEST(){
    signal_operator().emit(Cairo::OPERATOR_DEST);
  }
  void X3drawtoolbox::on_button_DEST_ATOP(){
    signal_operator().emit(Cairo::OPERATOR_DEST_ATOP);
  }
  void X3drawtoolbox::on_button_DEST_OUT(){
    signal_operator().emit(Cairo::OPERATOR_DEST_OUT);
  }
  void X3drawtoolbox::on_button_XOR(){
    signal_operator().emit(Cairo::OPERATOR_XOR);
  }

  void X3drawtoolbox::on_button_ATOP(){
    signal_operator().emit(Cairo::OPERATOR_ATOP);
  }
  void X3drawtoolbox::on_button_OUT(){
    signal_operator().emit(Cairo::OPERATOR_OUT);
  }
  void X3drawtoolbox::on_button_DEST_OVER(){
    signal_operator().emit(Cairo::OPERATOR_DEST_OVER);
  }
  void X3drawtoolbox::on_button_DEST_IN(){
    signal_operator().emit(Cairo::OPERATOR_DEST_IN);
  }
  void X3drawtoolbox::on_button_ADD(){
    signal_operator().emit(Cairo::OPERATOR_ADD);
  }
  void X3drawtoolbox::on_button_SATURATE(){
    signal_operator().emit(Cairo::OPERATOR_SATURATE);
  }
  ///////////////////

  void X3drawtoolbox::on_button_buffer_CLEAR(){
    signal_operator_buffer().emit(Cairo::OPERATOR_CLEAR);
  }
  void X3drawtoolbox::on_button_buffer_OVER(){
    signal_operator_buffer().emit(Cairo::OPERATOR_OVER);
  }
  void X3drawtoolbox::on_button_buffer_SOURCE(){
    signal_operator_buffer().emit(Cairo::OPERATOR_SOURCE);
  }
  void X3drawtoolbox::on_button_buffer_IN(){
    signal_operator_buffer().emit(Cairo::OPERATOR_IN);
  }
  void X3drawtoolbox::on_button_buffer_DEST(){
    signal_operator_buffer().emit(Cairo::OPERATOR_DEST);
  }
  void X3drawtoolbox::on_button_buffer_DEST_ATOP(){
    signal_operator_buffer().emit(Cairo::OPERATOR_DEST_ATOP);
  }
  void X3drawtoolbox::on_button_buffer_DEST_OUT(){
    signal_operator_buffer().emit(Cairo::OPERATOR_DEST_OUT);
  }
  void X3drawtoolbox::on_button_buffer_XOR(){
    signal_operator_buffer().emit(Cairo::OPERATOR_XOR);
  }

  void X3drawtoolbox::on_button_buffer_ATOP(){
    signal_operator_buffer().emit(Cairo::OPERATOR_ATOP);
  }

  void X3drawtoolbox::on_button_buffer_OUT(){
    signal_operator_buffer().emit(Cairo::OPERATOR_OUT);
  }
  void X3drawtoolbox::on_button_buffer_DEST_OVER(){
    signal_operator_buffer().emit(Cairo::OPERATOR_DEST_OVER);
  }
  void X3drawtoolbox::on_button_buffer_DEST_IN(){
    signal_operator_buffer().emit(Cairo::OPERATOR_DEST_IN);
  }
  void X3drawtoolbox::on_button_buffer_ADD(){
    signal_operator_buffer().emit(Cairo::OPERATOR_ADD);
  }
  void X3drawtoolbox::on_button_buffer_SATURATE(){
    signal_operator_buffer().emit(Cairo::OPERATOR_SATURATE);
  }
  ///////////////////
  ///set operator on visual

  void X3drawtoolbox::on_button_visual_CLEAR(){
    signal_operator_visual().emit(Cairo::OPERATOR_CLEAR);
  }
  void X3drawtoolbox::on_button_visual_OVER(){
    signal_operator_visual().emit(Cairo::OPERATOR_OVER);
  }
  void X3drawtoolbox::on_button_visual_SOURCE(){
    signal_operator().emit(Cairo::OPERATOR_SOURCE);
  }
  void X3drawtoolbox::on_button_visual_IN(){
    signal_operator_visual().emit(Cairo::OPERATOR_IN);
  }
  void X3drawtoolbox::on_button_visual_DEST(){
    signal_operator_visual().emit(Cairo::OPERATOR_DEST);
  }
  void X3drawtoolbox::on_button_visual_DEST_ATOP(){
    signal_operator_visual().emit(Cairo::OPERATOR_DEST_ATOP);
  }
  void X3drawtoolbox::on_button_visual_DEST_OUT(){
    signal_operator_visual().emit(Cairo::OPERATOR_DEST_OUT);
  }
  void X3drawtoolbox::on_button_visual_XOR(){
    signal_operator_visual().emit(Cairo::OPERATOR_XOR);
  }
  void X3drawtoolbox::on_button_visual_ATOP(){
    signal_operator_visual().emit(Cairo::OPERATOR_ATOP);
  }
  void X3drawtoolbox::on_button_visual_OUT(){
    signal_operator_visual().emit(Cairo::OPERATOR_OUT);
  }
  void X3drawtoolbox::on_button_visual_DEST_OVER(){
    signal_operator_visual().emit(Cairo::OPERATOR_DEST_OVER);
  }
  void X3drawtoolbox::on_button_visual_DEST_IN(){
    signal_operator_visual().emit(Cairo::OPERATOR_DEST_IN);
  }
  void X3drawtoolbox::on_button_visual_ADD(){
    signal_operator_visual().emit(Cairo::OPERATOR_ADD);
  }
  void X3drawtoolbox::on_button_visual_SATURATE(){
    signal_operator_visual().emit(Cairo::OPERATOR_SATURATE);
  }

  ////////////////

  X3drawtoolbox::fill_rule_signal X3drawtoolbox::signal_fill_rule(){
    return m_signal_fill_rule;
  }
  X3drawtoolbox::line_join_signal X3drawtoolbox::signal_line_join(){
    return m_signal_line_join;
  }


  X3drawtoolbox::line_cap_signal X3drawtoolbox::signal_line_cap(){
    return m_signal_line_cap;
  }

  X3drawtoolbox::filename_signal X3drawtoolbox::signal_filename(){
    return m_signal_filename;
  }


  void X3drawtoolbox::on_button_MITER(){
    signal_line_join().emit(Cairo::LINE_JOIN_MITER);
  }

  void X3drawtoolbox::on_button_ROUND(){
    signal_line_join().emit(Cairo::LINE_JOIN_ROUND);
  }

  void X3drawtoolbox::on_button_BEVEL(){
    signal_line_join().emit(Cairo::LINE_JOIN_BEVEL);
  }
  void X3drawtoolbox::on_button_EVEN_ODD(){
    signal_fill_rule().emit(Cairo::FILL_RULE_EVEN_ODD);
  }

  void X3drawtoolbox::on_button_WINDING(){
    signal_fill_rule().emit(Cairo::FILL_RULE_WINDING);
  }




  void X3drawtoolbox::on_button_BUTT(){
    signal_line_cap().emit(Cairo::LINE_CAP_BUTT);
  }
  void X3drawtoolbox::on_button_ROUNDY(){
    signal_line_cap().emit(Cairo::LINE_CAP_ROUND);
  }
  void X3drawtoolbox::on_button_SQUARE(){
    signal_line_cap().emit(Cairo::LINE_CAP_SQUARE);
  }


  PointerToolBox::PointerToolBox(bool hbox):bbox(NULL){
    create_bbox(hbox);
    show_all_children();
  }

  PointerToolBox::PointerToolBox(const XndrawMap<int, std::string>& buttoninfo, 
				 bool hbox):bbox(NULL){
    create_bbox(hbox);
    for(XndrawMap<int, std::string>::const_iterator w = buttoninfo.begin(); w!= buttoninfo.end();w++){
      IconButton* butt = Gtk::manage(new IconButton((*w).second.c_str(), (*w).first));
      buttons.push_back(butt);//
      bbox->pack_start(*butt);
    }
  }

  PointerToolBox::PointerToolBox(const XndrawMap<int,  PointerToolBox::button_info_type >& buttoninfo, 
				 bool hbox):bbox(NULL){
    create_bbox(hbox);
    for(XndrawMap<int, std::pair<std::string, sigc::slot0<void> > >::const_iterator w = buttoninfo.begin(); w!= buttoninfo.end();w++){
      IconButton* butt = Gtk::manage(new IconButton((*w).second.first.c_str(), (*w).first));
      buttons.push_back(butt);
      butt->set_slot((*w).second.second);
      bbox->pack_start(*butt, Gtk::PACK_SHRINK);
    }
  }


  void PointerToolBox::create_bbox(bool hbox){
    if(hbox){
      bbox = Gtk::manage(new Gtk::HButtonBox());
    }
    else{
      bbox = Gtk::manage(new Gtk::VButtonBox());
    }
    if(!bbox){throw xn_Error("Buttonbox creation failed");}
    add(*bbox);
  }

  IconButton* PointerToolBox::get_button(int id)const{
    for (const_button_iterator w = buttons.begin(); w!= buttons.end();
	 w++){
      if(id == (*w)->get_ID()){return *w;}
    }
    return NULL;
  }

FloatingToolbox::FloatingToolbox(const XndrawMap<int, std::string>& buttoninfo, 
		    bool hbox):tools(buttoninfo,  hbox){
      setup();
      add(tools);
      show_all_children();
    }

  FloatingToolbox::FloatingToolbox(const XndrawMap<int,  FloatingToolbox::button_info_type >& buttoninfo, 
		bool hbox):tools(buttoninfo,  hbox){
   setup();
   add(tools);
   show_all_children();
    }

void FloatingToolbox::setup(){
      set_title("tools");
      set_type_hint(Gdk::WINDOW_TYPE_HINT_UTILITY );
      set_keep_above (true);
    }

}//namespace xNDRAW
