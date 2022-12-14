#include "processor.hpp"

#include "sauros/profiler.hpp"
#include <iostream>
#include <sstream>

namespace sauros {

processor_c::processor_c() { populate_standard_builtins(); }

processor_c::~processor_c() {
   if (_sub_processor) {
      delete _sub_processor;
   }
}

void processor_c::cell_to_string(std::string &out, cell_ptr cell,
                                 std::shared_ptr<environment_c> env,
                                 bool show_space) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("processor_c::cell_to_string");
#endif
   switch (cell->type) {
   case cell_type_e::REAL:
      [[fallthrough]];
   case cell_type_e::STRING:
      [[fallthrough]];
   case cell_type_e::INTEGER:
      out += cell->data;
      if (show_space) {
         out += " ";
      }
      break;
   case cell_type_e::BOX_SYMBOL:
      [[fallthrough]];
   case cell_type_e::ENCODED_SYMBOL:
      [[fallthrough]];
   case cell_type_e::SYMBOL: {
      auto data = process_cell(cell, env);
      cell_to_string(out, data, env);
      break;
   }
   case cell_type_e::LIST: {

      out += std::string("[");

      for (auto &cell : cell->list) {
         cell_to_string(out, cell, env, true);
      }
      if (cell->list.size()) {
         out.pop_back();
      }
      out += std::string("]");
      if (show_space) {
         out += " ";
      }
      break;
   }
   case cell_type_e::LAMBDA: {
      out += "<lambda>";
      break;
   }
   case cell_type_e::BOX: {
      out += "<box>";
      break;
   }
   case cell_type_e::VARIANT: {
      out += "<variant>";
      break;
   }
   }
}

void processor_c::quote_cell(std::string &out, cell_ptr cell,
                             std::shared_ptr<environment_c> env) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("processor_c::quote_cell");
#endif
   switch (cell->type) {
   case cell_type_e::REAL:
      [[fallthrough]];
   case cell_type_e::INTEGER:
      [[fallthrough]];
   case cell_type_e::ENCODED_SYMBOL:
      [[fallthrough]];
   case cell_type_e::BOX_SYMBOL:
      [[fallthrough]];
   case cell_type_e::SYMBOL: {
      out += cell->data;
      out += " ";
      break;
   }
   case cell_type_e::STRING:
      out += "\"" + cell->data + "\" ";
      break;
   case cell_type_e::LIST: {

      out += std::string("[ ");

      for (auto &cell : cell->list) {
         quote_cell(out, cell, env);
      }
      out += std::string("] ");
      break;
   }
   case cell_type_e::LAMBDA: {

      auto &cells = cell->list;

      auto lambda_name = cells[0]->data;

      out += lambda_name + "[ ";

      auto target_lambda =
          env->find(cells[0]->data, cells[0])->get(cells[0]->data);

      for (auto param = target_lambda->list[0]->list.begin() + 1;
           param != target_lambda->list[0]->list.end(); ++param) {
         out += (*param)->data + " ";
      }

      out += "] ";

      out += std::string("[");

      for (auto &cell : target_lambda->list[1]->list) {
         quote_cell(out, cell, env);
      }
      out += std::string("] ");
      break;
   }
   case cell_type_e::BOX: {
      // TODO: Fill this in
      break;
   }
   case cell_type_e::VARIANT: {
      // TODO: Fill this in
      break;
   }
   }
}

cell_ptr processor_c::process_list(cells_t &cells,
                                   std::shared_ptr<environment_c> env) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("processor_c::process_list");
#endif
   if (_yield_cell) {
      return _yield_cell;
   }

   if (cells.empty()) {
      return std::make_shared<cell_c>(CELL_NIL);
   }

   auto suspect_cell = cells[0];

   switch (suspect_cell->type) {
   case cell_type_e::BOX_SYMBOL:
      [[fallthrough]];
   case cell_type_e::ENCODED_SYMBOL:
      [[fallthrough]];
   case cell_type_e::SYMBOL: {

      auto cell = process_cell(suspect_cell, env);
      // Check if its a lambda first
      if (cell->type == cell_type_e::LAMBDA) {
         return process_lambda(cell, cells, env);
      }

      // If its not then it might be a proc
      if (cell->proc) {
         return cell->proc(cells, env);
      }

      // Otherwise return whatever we got
      return cell;
   }

   case cell_type_e::LIST:
      return process_list(suspect_cell->list, env);

   case cell_type_e::REAL:
      [[fallthrough]];
   case cell_type_e::STRING:
      [[fallthrough]];
   case cell_type_e::INTEGER:
      return process_cell(suspect_cell, env);
      break;
   case cell_type_e::VARIANT:
      [[fallthrough]];
   case cell_type_e::LAMBDA:
      [[fallthrough]];
   case cell_type_e::BOX:
      break;

   default:
      break;
   }
   throw runtime_exception_c("Unknown cell type", cells[0]);
}

cell_ptr processor_c::process_cell(cell_ptr cell,
                                   std::shared_ptr<environment_c> env) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("processor_c::process_cell");
#endif

   if (_yield_cell) {
      return _yield_cell;
   }

   switch (cell->type) {
   case cell_type_e::BOX_SYMBOL: {
      // Each item up-to and not including the last item should be n box
      // the last member should be something within the box that we are
      // trying to access
      auto [r, s, e] = retrieve_box_data(cell, env);
      return r;
   }
   case cell_type_e::SYMBOL: {
      // If not built in maybe it is in the environment
      //
      auto env_with_data = env->find(cell->data, cell);
      auto r = env_with_data->get(cell->data);

      if (r->type == cell_type_e::BOX) {
         return clone_box(r);
      }
      return {r};
   }

   case cell_type_e::ENCODED_SYMBOL: {
      if (cell->builtin_encoding == BUILTIN_DEFAULT_VAL ||
          cell->builtin_encoding >= BUILTIN_ENTRY_COUNT) {
         throw runtime_exception_c("Invalid encoded symbol for : " + cell->data,
                                   cell);
      }

      // Direct access - no more mapping
      return _builtins[cell->builtin_encoding];
   }

   case cell_type_e::REAL:
      [[fallthrough]];
   case cell_type_e::STRING:
      [[fallthrough]];
   case cell_type_e::INTEGER:
      return cell;

   case cell_type_e::LIST:
      [[fallthrough]];
   case cell_type_e::LAMBDA: {
      return process_list(cell->list, env);
   case cell_type_e::BOX:
      [[fallthrough]];
   case cell_type_e::VARIANT:
      break;
   }

   default:
      break;
   }

   throw runtime_exception_c("internal error -> no processable cell", cell);
}

cell_ptr processor_c::process_lambda(cell_ptr cell, cells_t &cells,
                                     std::shared_ptr<environment_c> env) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("processor_c::process_lambda");
#endif
   cells_t exps;
   for (auto param = cells.begin() + 1; param != cells.end(); ++param) {
      exps.push_back(process_cell((*param), env));
   }

   if (cell->list[0]->list.size() != exps.size()) {
      throw runtime_exception_c(
          "Invalid number of paramters given to lambda: " + cells[0]->data +
              ". " + std::to_string(exps.size()) + " parameters given, but " +
              std::to_string(cell->list[0]->list.size()) + " were expected.",
          cells[0]);
   }

   // Create the lambda cell
   cell_ptr lambda_cell = std::make_shared<cell_c>();
   lambda_cell->data = cells[0]->data;
   lambda_cell->type = cell->type;
   lambda_cell->list = cell->list[1]->list;

   // Create the lambda env
   auto lambda_env = std::make_shared<environment_c>(
       environment_c(cell->list[0]->list, exps, env));

   if (!_sub_processor) {
      _sub_processor = new processor_c();
   }

   auto result = _sub_processor->process_cell(lambda_cell, lambda_env);
   _sub_processor->reset();
   return result;
}

void processor_c::reset() { _yield_cell = nullptr; }

cell_ptr processor_c::clone_box(cell_ptr cell) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("processor_c::clone_box");
#endif

   cell_c new_box(cell_type_e::BOX);
   new_box.box_env = std::shared_ptr<environment_c>(new environment_c());
   for (auto [key, value] : cell->box_env->get_map()) {
      new_box.box_env->set(key, value->clone());
   }
   return std::make_shared<cell_c>(new_box);
}

std::vector<std::string> processor_c::retrieve_accessors(cell_ptr &cell) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("processor_c::retrieve_accessors");
#endif
   std::vector<std::string> accessors;
   std::string accessor;
   std::stringstream source(cell->data);
   while (std::getline(source, accessor, '.')) {
      accessors.push_back(accessor);
   }
   if (accessors.size() <= 1) {
      throw runtime_exception_c("Malformed accessor", cell);
   }
   return accessors;
}

std::tuple<cell_ptr, std::string, std::shared_ptr<environment_c>>
processor_c::retrieve_box_data(cell_ptr &cell,
                               std::shared_ptr<environment_c> &env) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("processor_c::retrieve_box_data");
#endif

   auto accessors = retrieve_accessors(cell);

   cell_ptr result;
   std::shared_ptr<environment_c> moving_env = env;
   for (std::size_t i = 0; i < accessors.size(); i++) {

      // Get the item from the accessor
      auto containing_env = moving_env->find(accessors[i], cell);
      result = containing_env->get(accessors[i]);

      // Check if we need to move the environment "in" to the next box
      if (result->type == cell_type_e::BOX) {
         moving_env = result->box_env;
      }
   }
   return {result, accessors.back(), moving_env};
}

cell_ptr
processor_c::load_potential_variable(cell_ptr cell,
                                     std::shared_ptr<environment_c> env) {
#ifdef PROFILER_ENABLED
   profiler_c::get_profiler()->hit("processor_c::load_potential_variable");
#endif

   if (cell->type == cell_type_e::BOX_SYMBOL) {
      auto [r, s, e] = retrieve_box_data(cell, env);
      return r;
   }

   if (cell->type != cell_type_e::SYMBOL) {
      return process_cell(cell, env);
   }

   auto &variable_name = cell->data;
   auto containing_env = env->find(variable_name, cell);
   return containing_env->get(variable_name);
}

} // namespace sauros