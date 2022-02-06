//
//  arglist.cpp
//  derivs
//
//  Created by Xin Li on 2/3/22.
//

#include "arglist.hpp"
#include <algorithm>
#include <sstream>
#include <cctype>

void to_lower_case(std::string& input)
{
    std::transform(input.begin(), input.end(), input.begin(),
                   [](unsigned char c){return std::tolower(c);});
}

std::string to_string(double number)
{
    std::ostringstream os;
    os << number;
    return os.str();
}


std::string to_string(unsigned long number)
{
    std::ostringstream os;
    os << number;
    return os.str();
}


xlw::CellMatrix extract_cells(xlw::CellMatrix& cells,
                              unsigned long row,
                              unsigned long column,
                              std::string error_id,
                              std::string name,
                              bool& non_numeric
                              )
{
    // read number of rows and columns in the first row
    if(!cells(row, column).IsANumber())
        throw(error_id + " " + name + "rows and columns expected.");
    if(cells.ColumnsInStructure() <= column + 1)
        throw(error_id + " " + name + "rows and columns expected. ");
    if(!cells(row, column+1).IsANumber())
        throw(error_id + " " + name + "rows and columns expected. ");
    unsigned long num_rows = cells(row, column);
    unsigned long num_cols = cells(row, column+1);
    // clear cells after reading
    cells(row, column).clear();
    cells(row, column+1).clear();
    
    xlw::CellMatrix result(num_rows, num_cols);
    if(num_rows + row + 1 > cells.RowsInStructure())
        throw(error_id + " " + name + "insufficient rows in structure");
    if(num_cols + column > cells.ColumnsInStructure())
        throw(error_id + " " + name + "insufficient columns in structure");
    // read in results from cells
    for(unsigned long i=0; i < num_rows; ++i)
        for(unsigned long j=0; j < num_cols; ++j){
            result(i, j) = cells(row + 1 + i, column + j);
            cells(row + 1 + i, column + j).clear();
            if(!result(i, j).IsANumber())
                non_numeric = true;
        }
    return result;
}

/*
 Expected input cells format:
 struct_name|
 data_name1|
 data1 (could be a matrix or another cells)
 data_name2|
 data2 (could be a matrix or another cells)
 ...
 */
ArgumentList::ArgumentList(xlw::CellMatrix cells, std::string error_id)
{
    xlw::impl::MJCellValue empty;
    unsigned long rows = cells.RowsInStructure();
    unsigned long columns = cells.ColumnsInStructure();
    
    // read structure name
    if(rows==0)
        throw(std::string("Argument List requires non empty cell matrix") + error_id);
    if(!cells(0,0).IsAString())
        throw(std::string("a structure name must be specified for argument list class ") + error_id);
    struct_name = cells(0, 0).StringValue();
    to_lower_case(struct_name);
    cells(0,0) = empty;
    
    for(unsigned long i=1; i<columns; ++i)
        if(!cells(0, i).IsEmpty())
            throw("An argument list should only have the structure name on the first line: " + struct_name + " " + error_id);
    error_id += " " + struct_name;
    
    // read in cell values
    for(unsigned long i=1; i<rows; ++i)
        for(unsigned long j=0; j<columns; ++j)
            if(cells(i,j).IsError())
                generate_throw("Error Cell passed in ", i, j);
    unsigned long row = 1UL;
    while(row < rows){
        unsigned long rows_down = 1;
        unsigned long column = 0;
        while(column < columns){
            if(cells(row, column).IsEmpty()){
                // check nothing else in row
                while(column < columns){
                    if(!cells(row, column).IsEmpty())
                        generate_throw("data or value where unexpected.", row, column);
                    ++column;
                }
            }else{
                // we have data
                if(!cells(row, column).IsAString())
                    generate_throw("data where name expected.", row, column);
                std::string name(cells(row, column).StringValue());
                to_lower_case(name);
                if(name=="")
                    generate_throw("empty name not permissible.", row, column);
                if(rows==row+1)
                    generate_throw("No space where data expected below name", row, column);
                cells(row, column).clear();
                xlw::CellValue* below_ptr = &cells(row+1, column);
                xlw::CellValue& cell_below = *below_ptr;
                if(cell_below.IsEmpty())
                    generate_throw("Data expected below name", row, column);
                if(cell_below.IsANumber()){
                    add(name, cell_below.NumericValue());
                    column++;
                    cell_below=empty;
                }else if(cell_below.IsBoolean()){
                    add(name, cell_below.BooleanValue());
                    column++;
                    cell_below=empty;
                }else{
                    // it is a string
                    std::string str_val(cell_below.StringValue());
                    to_lower_case(str_val);
                    if((str_val == "list")||(str_val=="matrix")||(str_val=="cells")){
                        bool non_numeric = false;
                        xlw::CellMatrix extracted(extract_cells(cells, row+2, column, error_id, name, non_numeric));
                        if(str_val == "list"){
                            ArgumentList value(extracted, error_id + ":" + name);
                            add_list(name, extracted);
                        }
                        if(str_val == "cells"){
                            add(name, extracted);
                        }
                        if(str_val == "matrix"){
                            if(non_numeric)
                                throw("Non numerical value in matrix argument: " + name + " " + error_id);
                            xlw::MyMatrix value(extracted.RowsInStructure(),
                                                extracted.ColumnsInStructure());
                            for(unsigned long i=0; i<extracted.RowsInStructure(); ++i)
                                for(unsigned long j=0; j<extracted.ColumnsInStructure(); ++j)
                                    value(i, j) = extracted(i, j);
                            add(name, value);
                            cell_below = empty;
                            rows_down = std::max(rows_down, extracted.RowsInStructure() + 2);
                            column += extracted.ColumnsInStructure();
                        }else{
                            // it is an array or boring string
                            if((str_val == "array")||(str_val == "vector")){
                                cell_below.clear();
                                if(row + 2 >= rows)
                                    throw(error_id + " data expected below array " + name);
                                unsigned long size = cells(row+2, column);
                                cells(row+2, column).clear();
                                if(row+2+size>=rows)
                                    throw(error_id + " more data expected below array " + name);
                                xlw::MyArray arr(size);
                                for(unsigned long i=0; i<size; ++i){
                                    arr[i] = cells(row+3+i, column);
                                    cells(row+3+i, column).clear();
                                }
                                add(name, arr);
                                rows_down = std::max(rows_down, size+2);
                                column += 1;
                            }else{
                                // a plain string
                                std::string val = cell_below.StringValue();
                                to_lower_case(val);
                                add(name, val);
                                column++;
                                cell_below=empty;
                            }
                        }
                    }
                }
            }
            
        }
        row += rows_down + 1;
    }
    // check all cells are used (empty)
    for(unsigned long i=0; i<rows; ++i)
        for(unsigned long j=0; j<columns; ++j)
            if(!cells(i,j).IsEmpty())
                generate_throw("extraneous data " + error_id, i, j);
}




// add data and register argument names
void ArgumentList::add(const std::string& arg_name, const std::string& value)
{
    ArgumentType type = string;
    // record arg_name -> type
    // arg_names.push_back(std::pair<std::string, ArgumentType>{arg_name, type});
    // record arg_name -> value
    str_args.insert(std::pair<std::string, std::string>{arg_name, value});
    // resigter name
    register_name(arg_name, type);
}

void ArgumentList::add(const std::string& arg_name, double value)
{
    ArgumentType type = number;
    // record arg_name -> type
    // arg_names.push_back(std::pair<std::string, ArgumentType>{arg_name, type});
    // record arg_name -> value
    double_args.insert(std::pair<std::string, double>{arg_name, value});
    // resigter name
    register_name(arg_name, type);
}

void ArgumentList::add(const std::string& arg_name, const xlw::MyArray& value)
{
    ArgumentType type = vector;
    // record arg_name -> type
    // arg_names.push_back(std::pair<std::string, ArgumentType>{arg_name, type});
    // record arg_name -> value
    array_args.insert(std::make_pair(arg_name, value));
    // resigter name
    register_name(arg_name, type);
}

void ArgumentList::add(const std::string& arg_name, const xlw::MyMatrix& value)
{
    ArgumentType type = matrix;
    // record arg_name -> type
    // arg_names.push_back(std::pair<std::string, ArgumentType>{arg_name, type});
    // record arg_name -> value
    matrix_args.insert(std::make_pair(arg_name, value));
    // resigter name
    register_name(arg_name, type);
}

void ArgumentList::add(const std::string& arg_name, bool value)
{
    ArgumentType type = boolean;
    // record arg_name -> type
    // arg_names.push_back(std::pair<std::string, ArgumentType>{arg_name, type});
    // record arg_name -> value
    bool_args.insert(std::pair<std::string, bool>{arg_name, value});
    // resigter name
    register_name(arg_name, type);
}

void ArgumentList::add(const std::string& arg_name, const xlw::CellMatrix& values)
{
    ArgumentType type = cells;
    // record arg_name -> type
    // arg_names.push_back(std::pair<std::string, ArgumentType>{arg_name, type});
    // record arg_name -> value
    cell_args.insert(std::make_pair(arg_name, values));
    // resigter name
    register_name(arg_name, type);
}

void ArgumentList::add_list(const std::string& arg_name, const xlw::CellMatrix& values)
{
    ArgumentType type = list;
    // record arg_name -> type
    // arg_names.push_back(std::pair<std::string, ArgumentType>{arg_name, type});
    // record arg_name -> value
    list_args.insert(std::make_pair(arg_name, values));
    // resigter name
    register_name(arg_name, type);
}

void ArgumentList::add(const std::string& arg_name, const ArgumentList& values)
{
    xlw::CellMatrix cell_values(values.all_data());
    add_list(arg_name, cell_values);
}

void ArgumentList::register_name(const std::string &arg_name, ArgumentType type)
{
    arg_names.push_back(std::make_pair(arg_name, type));
    if(!(names.insert(*arg_names.rbegin()).second)) // insert returns a pair (iterator, bool)
        throw("Same argument name used twice " + arg_name);
    args_used.insert(std::pair<std::string, bool>{arg_name, false});
}

void ArgumentList::use_arg_name(const std::string& arg_name)
{
    std::map<std::string, bool>::iterator it = args_used.find(arg_name);
    it->second = true;
}

// get argument values
std::string ArgumentList::get_str_arg_val(const std::string& arg_name_)
{
    std::string arg_name(arg_name_);
    to_lower_case(arg_name);
    std::map<std::string, std::string>::const_iterator it = str_args.find(arg_name);
    if(it == str_args.end())
        throw(struct_name + std::string(" unknown string argument asked for: ") + arg_name);
    use_arg_name(arg_name);
    return it->second;
}

unsigned long ArgumentList::get_ul_arg_val(const std::string& arg_name_)
{
    std::string arg_name(arg_name_);
    to_lower_case(arg_name);
    std::map<std::string, double>::const_iterator it = double_args.find(arg_name);
    if(it == double_args.end())
        throw(struct_name + std::string(" unknown unsigned long argument asked for: ") + arg_name);
    use_arg_name(arg_name);
    return static_cast<unsigned long>(it->second);
}

double ArgumentList::get_double_arg_val(const std::string& arg_name_)
{
    std::string arg_name(arg_name_);
    to_lower_case(arg_name);
    std::map<std::string, double>::const_iterator it = double_args.find(arg_name);
    if(it == double_args.end())
        throw(struct_name + std::string(" unknown double argument asked for: ") + arg_name);
    use_arg_name(arg_name);
    return it->second;
}

xlw::MyArray ArgumentList::get_array_arg_val(const std::string &arg_name_)
{
    std::string arg_name(arg_name_);
    to_lower_case(arg_name);
    std::map<std::string, xlw::MyArray>::const_iterator it = array_args.find(arg_name);
    if(it == array_args.end())
        throw(struct_name + std::string(" unknown array argument asked for : ") + arg_name);
    use_arg_name(arg_name);
    return it->second;
}

xlw::MyMatrix ArgumentList::get_matrix_arg_val(const std::string &arg_name_)
{
    std::string arg_name(arg_name_);
    to_lower_case(arg_name);
    std::map<std::string, xlw::MyMatrix>::const_iterator it = matrix_args.find(arg_name);
    if(it == matrix_args.end())
        throw(struct_name + std::string(" unknown matrix argument asked for : ") + arg_name);
    use_arg_name(arg_name);
    return it->second;
}

bool ArgumentList::get_bool_arg_val(const std::string &arg_name_)
{
    std::string arg_name(arg_name_);
    to_lower_case(arg_name);
    std::map<std::string, bool>::const_iterator it = bool_args.find(arg_name);
    if(it == bool_args.end())
        throw(struct_name + std::string(" unknown bool argument asked for : ") + arg_name);
    use_arg_name(arg_name);
    return it->second;
}

xlw::CellMatrix ArgumentList::get_cells_arg_val(const std::string &arg_name_)
{
    std::string arg_name(arg_name_);
    to_lower_case(arg_name);
    std::map<std::string, xlw::CellMatrix>::const_iterator it = cell_args.find(arg_name);
    if(it == cell_args.end())
        throw(struct_name + std::string(" unknown cells argument asked for : ") + arg_name);
    use_arg_name(arg_name);
    return it->second;
}

ArgumentList ArgumentList::get_arglist_arg_val(const std::string &arg_name_)
{
    std::string arg_name(arg_name_);
    to_lower_case(arg_name);
    std::map<std::string, xlw::CellMatrix>::const_iterator it = list_args.find(arg_name);
    if(it == list_args.end())
        throw(struct_name + std::string(" unknown arglist argument asked for : ") + arg_name);
    use_arg_name(arg_name);
    return ArgumentList(it->second, arg_name);
}

// check arguments
bool ArgumentList::is_arg_present(const std::string &arg_name_) const
{
    std::string arg_name(arg_name_);
    to_lower_case(arg_name);
    return names.find(arg_name) != names.end();
}

void ArgumentList::check_all_used(const std::string& error_id) const
{
    std::string unused_list;
    for(std::map<std::string, bool>::const_iterator it = args_used.begin();
        it != args_used.end(); ++it){
        if(!it->second)
            unused_list += it->first + std::string(", ");
    }
    if(unused_list != "")
        throw("Unused arguments in " + error_id + " " + struct_name + " " + unused_list);
}


void ArgumentList::generate_throw(std::string msg, unsigned long row, unsigned long column)
{
    throw(struct_name + " " + msg + " row:" + to_string(row) + "; column:" + to_string(column) + ".");
}

// get optional arguments
bool ArgumentList::get_if_present(const std::string &arg_name, unsigned long &arg_value)
{
    if(!is_arg_present(arg_name))
        return false;
    arg_value = get_ul_arg_val(arg_name);
    return true;
}

bool ArgumentList::get_if_present(const std::string &arg_name, double &arg_value)
{
    if(!is_arg_present(arg_name))
        return false;
    arg_value = get_double_arg_val(arg_name);
    return true;
}

bool ArgumentList::get_if_present(const std::string &arg_name, xlw::MyArray &arg_value)
{
    if(!is_arg_present(arg_name))
        return false;
    arg_value = get_array_arg_val(arg_name);
    return true;
}

bool ArgumentList::get_if_present(const std::string &arg_name, xlw::MyMatrix &arg_value)
{
    if(!is_arg_present(arg_name))
        return false;
    arg_value = get_matrix_arg_val(arg_name);
    return true;
}

bool ArgumentList::get_if_present(const std::string &arg_name, bool &arg_value)
{
    if(!is_arg_present(arg_name))
        return false;
    arg_value = get_bool_arg_val(arg_name);
    return true;
}

bool ArgumentList::get_if_present(const std::string &arg_name, xlw::CellMatrix &arg_value)
{
    if(!is_arg_present(arg_name))
        return false;
    arg_value = get_cells_arg_val(arg_name);
    return true;
}

bool ArgumentList::get_if_present(const std::string &arg_name, ArgumentList &arg_value)
{
    if(!is_arg_present(arg_name))
        return false;
    arg_value = get_arglist_arg_val(arg_name);
    return true;
}


