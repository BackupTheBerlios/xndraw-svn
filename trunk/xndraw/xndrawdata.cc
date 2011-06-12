
//$Id: exampletreemodel.cc,v 1.5 2005/02/28 10:14:24 murrayc Exp $ -*- c++ -*-

/* gtkmm example Copyright (C) 2002 gtkmm development team
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

//originally adapted from(and never really 'got')treemodel example in
//gtkmm docs


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

#include <iostream>
#include "xndrawdata.h"

using namespace xNDRAW;
using namespace NCOORDS;

Path_textframe::Path_textframe(const NCOORDS::simple_click_history& path_):
  path(&path_),
  timestamp(1),
  itercount(0),
  model(Gtk::TreeStore::create(columninfo)){
  model->set_column_types(columninfo);
  std::ostringstream os;
  os << "path (simple_click_history) at "<< path; /*address*/
  set_label(os.str());
  add(dataport);
  dataport.add(viewer);
  
  //Only show the scrollbars when they are necessary:
  dataport.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  
  viewer.set_model(model);
  viewer.append_column("point number", columninfo.ordinals);
  viewer.append_column("type", columninfo.clicks);
viewer.append_column("(  x  ,", columninfo.location_x);
viewer.append_column("  y   )", columninfo.location_x);
 viewer.append_column("state", columninfo.state);

  this->add_rows();
  show_all_children();
  }

void Path_textframe::add_rows(){
  itercount = 0;
for(path_iterator iter = path->begin(); iter != path->end(); iter++){
    Gtk::TreeModel::Row row = *(model->append());
    row[columninfo.ordinals] = ++itercount;
    std::ostringstream ostr;
    const std::pair<NCOORDS::coords, int>& point=  *iter;
    //std::cerr <<"segfault here :pairaddress : "<< &point;
const NCOORDS::coords & realpt = point.first;
//std::cerr <<"pointaddress : "<< &  realpt<<std::endl;
    ostr << *iter;
    row[columninfo.clicks] = ostr.str();
    
    int ptflags =(*iter).second;
    std::string ptstate = NCOORDS::clicktype_as_string(ptflags);
   
    row[columninfo.state] =ptstate;
    row[columninfo.location_x]= (*iter).first.x;
    row[columninfo.location_y]= (*iter).first.y;
 }
}

void Path_textframe::update(){
  Gtk::TreeNodeChildren chillun = model->children();
  if(path->size() != chillun.size()){
    model->clear();
    add_rows();
  }
}

Glib::RefPtr<Path_textframe> Path_textframe::create(const simple_click_history& history){
      Path_textframe* d = new  Path_textframe(history);
      return Glib::RefPtr<Path_textframe>(d);
    }
    

//*******************
// class xN_draw_data: don't use
//useless adaptation of demonstration class from gtkmm docs 
//(unless you want to show a list of rows of strings, 
//independent of any other data)
xN_draw_data::GlueItem::GlueItem(int row_number)
: m_row_number(row_number){}

int xN_draw_data::GlueItem::get_row_number() const{
  return m_row_number;
}

xN_draw_data::GlueList::GlueList(){}

xN_draw_data::GlueList::~GlueList(){
  for(type_listOfGlue::iterator iter = m_list.begin(); iter != m_list.end(); ++iter){
    xN_draw_data::GlueItem* pItem = *iter;
    delete pItem;
  }
}
     

xN_draw_data::xN_draw_data(unsigned int columns_count)
  : Glib::ObjectBase( typeid(NCOORDS::coords) ), //register a custom GType.
    Glib::Object(), //The custom GType is actually registered here.
    m_stamp(1), //When the model's stamp != the iterator's stamp then that iterator is invalid and should be ignored. Also, 0=invalid
    m_pGlueList(0){

  //this ctor just loads ( %rowno, %columnno )
  
  //the others should be their own model also
  //maybe a drawings model listing each cairo construct,with columns
  //for paths and maps,  the selected one displaying in its own view
  //m_draw_columns.names;
  //   m_draw_columns.rownumbers;
  //   m_draw_columns.drawings;
  //   m_draw_columns.simple_paths;
  //   m_draw_columns.coordmaps
  //need: get_drawings()


  const rowstring_list_type::size_type rows_count = 100;
  m_rows.resize(rows_count); //100 rows.
  for(unsigned int row_number = 0; row_number < rows_count; ++row_number){
    //Create the row:
    m_rows[row_number].resize(columns_count); // 10 cells for each row.
    for(unsigned int column_number = 0; column_number < columns_count; ++column_number){
      // Set the data in the row cells:
      // use existing data from some structure,instead of generating it here.
      char buffer[20]; //You could use a std::stringstream instead.
      g_snprintf(buffer, sizeof(buffer), "%d, %d", row_number, column_number);
      (m_rows[row_number])[column_number] = buffer; //all 10 columns the same type.
    }
  }
  //The Column info for with TreeView::append(), TreeModel::iterator[], etc.
  textcolumns.resize(columns_count);
  for(unsigned int column_number = 0; column_number < columns_count; ++column_number){
    m_column_record.add(textcolumns[column_number] );
  }
}

xN_draw_data::~xN_draw_data(){
  if(m_pGlueList){
    delete m_pGlueList;
  }
}

//static:
Glib::RefPtr<xN_draw_data> xN_draw_data::create(){
  return Glib::RefPtr<xN_draw_data>( new xN_draw_data );
}

Gtk::TreeModelFlags xN_draw_data::get_flags_vfunc() const{
   return Gtk::TreeModelFlags(0);
}

int xN_draw_data::get_n_columns_vfunc() const{
   return m_rows[0].size(); //column mumber same for all rows.
}

GType xN_draw_data::get_column_type_vfunc(int index) const{
  if(index <= (int)textcolumns.size())
    return textcolumns[index].type();
  else
    return 0;
}

void xN_draw_data::get_value_vfunc(const TreeModel::iterator& iter, int column, Glib::ValueBase& value) const{
  if(check_treeiter_validity(iter)){
    if(column <= (int)textcolumns.size()){
      //Get the correct ValueType from the Gtk::TreeModel::Column's type, so we don't have to repeat it here:
      TextColumn::ValueType value_specific;
      value_specific.init( TextColumn::ValueType::value_type() );  //TODO: Is there any way to avoid this step?
      rowstring_list_type::const_iterator dataRowIter = get_data_row_iter_from_tree_row_iter(iter);
      if(dataRowIter != m_rows.end()){
        const rowstring_type& dataRow = *dataRowIter;
        Glib::ustring result = dataRow[column];
        value_specific.set(result); //give The compiler correct type.
        value.init( Glib::Value< Glib::ustring >::value_type() ); 
//TODO: Is there any way to avoid this step? 
        value = value_specific;
      }
    }
  }
}

bool xN_draw_data::iter_next_vfunc(const iterator& iter, iterator& iter_next) const{ 
  if( check_treeiter_validity(iter) ){
    //initialize the iterator:
    iter_next = iterator();
    iter_next.set_stamp(m_stamp);
    //Get the current row:
    const GlueItem* pItem = (const GlueItem*)iter.gobj()->user_data;
    rowstring_list_type::size_type row_index = pItem->get_row_number();
    //Make the iter_next GtkTreeIter represent the next row:
    row_index++;
    if( row_index < m_rows.size() ){ 
      //Put the index of the next row in a GlueItem in iter_next:
      GlueItem* pItemNew = new GlueItem(row_index);
      iter_next.gobj()->user_data = (void*)pItemNew;
      remember_glue_item(pItemNew);
      return true; //success
    }
  }
  else{
    iter_next = iterator(); //Set as invalid, like the TreeModel docs say
  }
  return false; //There is no next row.
}

bool xN_draw_data::iter_children_vfunc(const iterator& parent, iterator& iter) const{
  return iter_nth_child_vfunc(parent, 0, iter);
}

bool xN_draw_data::iter_has_child_vfunc(const iterator& iter) const{
  return (iter_n_children_vfunc(iter) > 0);
}

int xN_draw_data::iter_n_children_vfunc(const iterator& iter) const{
  if(!check_treeiter_validity(iter)) { return 0; } 
  return 0; //There are no children
}

int xN_draw_data::iter_n_root_children_vfunc() const{
  return m_rows.size();
}

bool xN_draw_data::iter_nth_child_vfunc(const iterator& parent, int /* n */, iterator& iter) const{
  if(!check_treeiter_validity(parent)){
    iter = iterator(); 
    return false;
  }
  iter = iterator(); //Set as invalid, like the TreeModel docs say
  return false; //There are no children.
}

bool xN_draw_data::iter_nth_root_child_vfunc(int n, iterator& iter) const{
  if(n < (int)m_rows.size()){
    iter = iterator(); //clear the input parameter.
    iter.set_stamp(m_stamp);
    //Store the row_index in the GtkTreeIter:
    //See also iter_next_vfunc()
    unsigned row_index = n;
    //This will be deleted in the GlueList destructor, when old iterators are marked as invalid.
    GlueItem* pItem = new GlueItem(row_index);
    iter.gobj()->user_data = pItem;
    remember_glue_item(pItem);
    return true;
  }
  return false; //There are no children.  
}
  

bool xN_draw_data::iter_parent_vfunc(const iterator& child, iterator& iter) const{
  if(!check_treeiter_validity(child)){
    iter = iterator(); 
    return false;
  }

  iter = iterator(); //Set is as invalid, as the TreeModel documentation says that it should be.
  return false; //There are no children, so no parents.
}

Gtk::TreeModel::Path xN_draw_data::get_path_vfunc(const iterator& /* iter */) const{//whats this ?
   //TODO:
   return Path();
}

bool xN_draw_data::get_iter_vfunc(const Path& path, iterator& iter) const{
   unsigned sz = path.size();
   if(!sz){
     iter = iterator(); //Set as invalid, 
     return false;
   }
   if(sz > 1) {//There are no children.  {
     iter = iterator(); //Set is as invalid, 
     return false; 
   }
   //This is a new GtkTreeIter, so it needs the current stamp value.
   iter = iterator(); //clear the input parameter.
   iter.set_stamp(m_stamp);
   //Store the row_index in the GtkTreeIter:
   //TODO: Store a pointer to some more complex data type such as a typeListOfRows::iterator.
   unsigned row_index = path[0];
   GlueItem* pItem = new GlueItem(row_index);
   //Store the GlueItem in the GtkTreeIter.
   //This will be deleted in the GlueList destructor,
   //which will be called when the stamp value changes. 
   iter.gobj()->user_data = (void*)pItem;
   remember_glue_item(pItem);
   return true;
}

Gtk::TreeModelColumn< Glib::ustring >& xN_draw_data::get_model_column(int column){
  return textcolumns[column];
}

xN_draw_data::rowstring_list_type::iterator xN_draw_data::get_data_row_iter_from_tree_row_iter(const iterator& iter){
  //Don't call this on an invalid iter.
  const GlueItem* pItem = (const GlueItem*)iter.gobj()->user_data;
  rowstring_list_type::size_type row_index = pItem->get_row_number();
  if( row_index > m_rows.size() )
    return m_rows.end();
  else
    return m_rows.begin() + row_index; //TODO: Performance.
}

xN_draw_data::rowstring_list_type::const_iterator xN_draw_data::get_data_row_iter_from_tree_row_iter(const iterator& iter) const{
  //Don't call this on an invalid iter.
  const GlueItem* pItem = (const GlueItem*)iter.gobj()->user_data;
  rowstring_list_type::size_type row_index = pItem->get_row_number();
  if( row_index > m_rows.size() )
    return m_rows.end();
  else
    return m_rows.begin() + row_index; //TODO: Performance.
}

bool xN_draw_data::check_treeiter_validity(const iterator& iter) const{
  // Anything that modifies the model's structure should change its stamp,
  return m_stamp == iter.get_stamp();
}

bool xN_draw_data::iter_is_valid(const iterator& iter) const{
  if(!check_treeiter_validity(iter))
    return false;
  return Gtk::TreeModel::iter_is_valid(iter);
}

void xN_draw_data::remember_glue_item(GlueItem* item) const{
  if(!m_pGlueList){
    m_pGlueList = new GlueList();
  }
  m_pGlueList->m_list.push_back(item);
}


