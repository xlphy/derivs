//
//  arglist.hpp
//  derivs
//
//  Created by Xin Li on 2/3/22.
//

#ifndef arglist_hpp
#define arglist_hpp

/*Argument list class to encapsulate arguments used to construct objects or functions
 re-implement argument list class from xlw (ArgList header)
 */


#include "xlw/MyContainers.h"
#include <xlw/CellMatrix.h>
#include <map>
#include <string>
#include <vector>

void to_lower_case(std::string& input);

class ArgumentList{
public:
    ArgumentList(xlw::CellMatrix cells, std::string error_id);
    ArgumentList(std::string name):struct_name(name){}
    
    enum ArgumentType{string, number, vector, matrix, boolean, list, cells};
    
    std::string get_struct_name() const {return struct_name;}
    const std::vector<std::pair<std::string, ArgumentType>>& get_arg_names_types() const {return arg_names;}
    
    // argument data retrieved using a string key
    std::string get_str_arg_val(const std::string& arg_name);
    unsigned long get_ul_arg_val(const std::string& arg_name);
    double get_double_arg_val(const std::string& arg_name);
    xlw::MyArray get_array_arg_val(const std::string& arg_name);
    xlw::MyMatrix get_matrix_arg_val(const std::string& arg_name);
    bool get_bool_arg_val(const std::string& arg_name);
    xlw::CellMatrix get_cells_arg_val(const std::string& arg_name);
    ArgumentList get_arglist_arg_val(const std::string& arg_name);
    
    // bool indicates whether the argument was found, deal with optional arguments
    // arg_value get overridden if present
    bool get_if_present(const std::string& arg_name, unsigned long& arg_value);
    bool get_if_present(const std::string& arg_name, double& arg_value);
    bool get_if_present(const std::string& arg_name, xlw::MyArray& arg_value);
    bool get_if_present(const std::string& arg_name, xlw::MyMatrix& arg_value);
    bool get_if_present(const std::string& arg_name, bool& arg_value);
    bool get_if_present(const std::string& arg_name, xlw::CellMatrix& arg_value);
    bool get_if_present(const std::string& arg_name, ArgumentList& arg_value);
    
    bool is_arg_present(const std::string& arg_name) const;
    void check_all_used(const std::string& error_id) const; // all args needed to be used to avoid miss-spelled optional arguments
    
    xlw::CellMatrix all_data() const {//a stub will not be used
        return xlw::CellMatrix(0, 0); //convert argument list to CellMatrix
    };
    
    // data insertions
    void add(const std::string& arg_name, const std::string& value);
    void add(const std::string& arg_name, double value);
    void add(const std::string& arg_name, const xlw::MyArray& value);
    void add(const std::string& arg_name, const xlw::MyMatrix& value);
    void add(const std::string& arg_name, bool value);
    void add(const std::string& arg_name, const xlw::CellMatrix& values);
    void add(const std::string& arg_name, const ArgumentList& values);
    void add_list(const std::string& arg_name, const xlw::CellMatrix& values);
    
private:
    std::string struct_name;  //base class name
    // store argument data
    std::vector<std::pair<std::string, ArgumentType>> arg_names;
    std::map<std::string, double> double_args;
    std::map<std::string, xlw::MyArray> array_args;
    std::map<std::string, xlw::MyMatrix> matrix_args;
    std::map<std::string, std::string> str_args; // should contain "name"
    std::map<std::string, xlw::CellMatrix> list_args;
    std::map<std::string, xlw::CellMatrix> cell_args;
    std::map<std::string, bool> bool_args;
    std::map<std::string, ArgumentType> names;
    std::map<std::string, bool> args_used;
    
    void generate_throw(std::string msg, unsigned long row, unsigned long column);
    void use_arg_name(const std::string& arg_name);
    void register_name(const std::string& arg_name, ArgumentType type);
};



#endif /* arglist_hpp */
