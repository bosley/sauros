#include "sys.hpp"

#include "sauros/processor/processor.hpp"
#include <chrono>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <thread>

namespace sauros {
namespace modules {

sys_c::sys_c() {

   _members_map["cls"] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
#if defined(__unix__) || defined(__unix) || defined(__linux__)
          int _ = system("clear");
#else
          system("cls");
#endif
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   _members_map["cmd"] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          if (cells.size() == 1) {
             throw processor_c::runtime_exception_c(
                 "system command expects at least one parameter",
                 cells[0]->location);
          }

          if (cells.size() == 2) {
             auto c = load(cells[1], env);
             if (c->type != cell_type_e::STRING) {
                throw processor_c::runtime_exception_c(
                    "system command must be of type: STRING",
                    cells[0]->location);
             }
             return std::make_shared<cell_c>(
                 cell_type_e::INTEGER, std::to_string(system(c->data.c_str())));
          }

          auto result = std::make_shared<cell_c>(cell_type_e::LIST);
          for (auto c = cells.begin() + 1; c < cells.end(); c++) {
             auto item = load(*c, env);
             if (item->type != cell_type_e::STRING) {
                throw processor_c::runtime_exception_c(
                    "system command must be of type: STRING", (*c)->location);
             }
             result->list.push_back(std::make_shared<cell_c>(
                 cell_type_e::INTEGER,
                 std::to_string(system(item->data.c_str()))));
          }

          return {result};
       });

   _members_map["sleep"] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          if (cells.size() != 2) {
             throw processor_c::runtime_exception_c(
                 "sleep function expects 1 parameter, but " +
                     std::to_string(cells.size() - 1) + " were given",
                 cells[0]->location);
          }

          auto item = load(cells[1], env);
          if (item->type != cell_type_e::DOUBLE &&
              item->type != cell_type_e::INTEGER) {
             throw processor_c::runtime_exception_c(
                 "sleep operation expects parameter to be a numerical type",
                 cells[1]->location);
          }

          std::this_thread::sleep_for(
              std::chrono::duration<double>(std::stoull(item->data)));
          return std::make_shared<cell_c>(CELL_TRUE);
       });
}

} // namespace modules
} // namespace sauros