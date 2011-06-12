// -*- C++ -*-

/* Copyright (C)2006-2011 John O'Driscoll - biggerbumme@ yahoo.co.in
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "drawfunctors.h"

using namespace xNDRAW;

void draw_set_stroke_colour::operator()(Drawingbase* draw){
 draw->set_stroke_colour(r,g,b,a);
}

void draw_source_to_fill::operator()(Drawingbase* draw){
  draw->set_source_to_fill();  
}

void draw_source_to_stroke::operator()(Drawingbase* draw){
  draw->set_source_to_stroke();
}

void draw_save::operator()(Drawingbase* draw){
  draw->save();
}

void draw_set_fill_colour::operator()(Drawingbase* draw){
  draw->set_fill_colour(r,g,b,a);
}

void draw_restore::operator()(Drawingbase* draw){
  draw->restore();
}

void draw_insert_clickpath::operator()(Drawingbase* draw){
	draw->insert_path(path,relative,base);
	
      }

void draw_prepend_clickpath::operator()(Drawingbase* draw){
	draw->prepend_path(path,relative,base);
      }

void draw_append_clickpath::operator()(Drawingbase* draw){
	draw->append_path(path,relative,base);
      }

void draw_line_to_history::operator()(Drawingbase* draw){
      draw->stroke_history();
    }

void draw_stroke_history::operator()(Drawingbase* draw){
  draw->stroke_history();
}

void draw_stroke_preserve_history::operator()(Drawingbase* draw){
  draw->stroke_preserve_history();
}

void draw_fill_preserve_history::operator()(Drawingbase* draw){
  draw->fill_preserve_history();
}

void draw_fill_history::operator()(Drawingbase* draw){
      draw->fill_history();
    }

void draw_set_stroke_rgb::operator()(Drawingbase* draw){
	draw->set_stroke_colour(red, green, blue);
}


void draw_set_fill_rgb::operator()(Drawingbase* draw){
	draw->set_fill_colour(red, green, blue);
      
}

void draw_set_operator::operator()(Drawingbase* draw){
	draw->set_operator(op);
      }

void draw_set_linecap::operator()(Drawingbase* draw){
	draw->set_line_cap(linecap);
      }

void draw_set_fillrule::operator()(Drawingbase* draw){
	draw->set_fill_rule( fillrule);
      }

void draw_set_linejoin::operator()(Drawingbase* draw){
	draw->set_line_join( linejoin);
      }

void draw_clear_history::operator()(Drawingbase* draw){
      draw->clear_history();
    }

void draw_adjust_stroke_alpha::operator()(Drawingbase* draw){
	draw->adjust_stroke_alpha(alpha);
      }

void draw_set_linewidth::operator()(Drawingbase* draw){
	draw->set_line_width(linewidth);
      }

void draw_save_png::operator()(Drawingbase* draw){
	std::ostringstream outstring;
	outstring << filename << ++count << ".png";
	draw->get_target()->write_to_png(outstring.str());
      }

void draw_moveto_coords::operator()(Drawingbase* draw){
if(!pt){
    throw NCOORDS::xn_Error("NULL coords");
  }
#ifdef DEBUG
	draw->check_type(std::cout, "drawdeque::move_to");
#endif
	PRINT_STRING(draw->get_has_nodes());
	draw->move_to(pt);
      }

void draw_lineto_coords::operator()(Drawingbase* draw){
  if(!pt){
    throw NCOORDS::xn_Error("NULL coords");
  }
#ifdef DEBUG
	draw->check_type(std::cout, "drawdeque::line_to");
	draw->check_status();
#endif	
	draw->line_to(pt);
      }

void draw_setorigin_coords::operator()(Drawingbase* draw){
	draw->set_to_origin(pt);
      }

void draw_fill_functor::operator()(Drawingbase* draw){
      draw->save();
      draw->set_source_to_fill();
      draw->fill();
      draw->restore();
    }

void draw_fillpreserve_functor::operator()(Drawingbase* draw){
      draw->save();
      draw->set_source_to_fill();
      draw->fill_preserve();
      draw->restore();
    }

void draw_stroke_functor::operator()(Drawingbase* draw){
      draw->save();
      draw->set_source_to_stroke();
      draw->stroke();
      draw->restore();
    }

void draw_strokepreserve_functor::operator()(Drawingbase* draw){
      draw->save();
      draw->set_source_to_stroke();
      draw->stroke_preserve();
      draw->restore();
    }


void draw_set_baseno::operator()(Drawingbase* draw){
 draw->clear_history();
  //draw->paint_white();
  draw->save();
  draw->set_source_rgb(1,1,1);
  draw->paint();
  draw->restore();
      draw->set_baseno(newbase); 
    }


