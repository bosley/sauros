#include "fs.hpp"
#include <filesystem>
#include <fstream>
#include <string>

#include "sauros/processor/processor.hpp"

namespace sauros {
namespace modules {

fs_c::fs_c() {
   _members_map["cwd"] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          if (cells.size() != 1) {
             throw processor_c::runtime_exception_c(
                 "fs::cwd operation expects no parameters", cells[0]->location);
          }
          return std::make_shared<cell_c>(sauros::cell_type_e::STRING,
                                          std::filesystem::current_path());
       });

   _members_map["ls"] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          if (cells.size() != 1) {
             throw processor_c::runtime_exception_c(
                 "fs::ls operation expects no parameters", cells[0]->location);
          }

          cell_ptr result = std::make_shared<cell_c>(cell_type_e::LIST);

          for (const auto &entry : std::filesystem::directory_iterator(
                   std::filesystem::current_path())) {
             result->list.push_back(
                 std::make_shared<cell_c>(cell_type_e::STRING, entry.path()));
          }
          return result;
       });

   _members_map["chdir"] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          if (cells.size() != 2) {
             throw processor_c::runtime_exception_c(
                 "fs::chdir operation expects exactly 1 parameter",
                 cells[0]->location);
          }

          auto item = load(cells[1], env);
          if (item->type != cell_type_e::STRING) {
             throw processor_c::runtime_exception_c(
                 "fs::chdir operation expects parameter to be a string",
                 cells[1]->location);
          }

          std::filesystem::current_path(item->data);

          return std::make_shared<cell_c>(CELL_TRUE);
       });

   _members_map["is_file"] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          if (cells.size() != 2) {
             throw processor_c::runtime_exception_c(
                 "fs::is_file operation expects exactly 1 parameter",
                 cells[0]->location);
          }

          auto item = load(cells[1], env);
          if (item->type != cell_type_e::STRING) {
             throw processor_c::runtime_exception_c(
                 "fs::is_file operation expects parameter to be a string",
                 cells[1]->location);
          }

          if (std::filesystem::is_regular_file(item->data)) {
             return std::make_shared<cell_c>(CELL_TRUE);
          }
          return std::make_shared<cell_c>(CELL_FALSE);
       });

   _members_map["is_dir"] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          if (cells.size() != 2) {
             throw processor_c::runtime_exception_c(
                 "fs::is_dir operation expects exactly 1 parameter",
                 cells[0]->location);
          }

          auto item = load(cells[1], env);
          if (item->type != cell_type_e::STRING) {
             throw processor_c::runtime_exception_c(
                 "fs::is_dir operation expects parameter to be a string",
                 cells[1]->location);
          }

          if (std::filesystem::is_directory(item->data)) {
             return std::make_shared<cell_c>(CELL_TRUE);
          }
          return std::make_shared<cell_c>(CELL_FALSE);
       });

   _members_map["read"] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          if (cells.size() != 2) {
             throw processor_c::runtime_exception_c(
                 "fs::read_file operation expects exactly 1 parameter",
                 cells[0]->location);
          }

          auto item = load(cells[1], env);
          if (item->type != cell_type_e::STRING) {
             throw processor_c::runtime_exception_c(
                 "fs::read_file operation expects parameter to be a string",
                 cells[1]->location);
          }

          if (!std::filesystem::is_regular_file(item->data)) {
             return std::make_shared<cell_c>(CELL_FALSE);
          }

          std::fstream fs;
          fs.open(item->data, std::fstream::in);
          if (!fs.is_open()) {
             return std::make_shared<cell_c>(CELL_FALSE);
          }

          cell_ptr result = std::make_shared<cell_c>(cell_type_e::LIST);

          std::string line;
          while (std::getline(fs, line)) {
             result->list.push_back(
                 std::make_shared<cell_c>(cell_type_e::STRING, line));
          }
          return result;
       });

   _members_map["write"] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          if (cells.size() != 3) {
             throw processor_c::runtime_exception_c(
                 "fs::write operation expects exactly 2 parameter",
                 cells[0]->location);
          }

          auto item = load(cells[1], env);
          if (item->type != cell_type_e::STRING) {
             throw processor_c::runtime_exception_c(
                 "fs::write first parameter needs to be a string representing "
                 "a destination",
                 cells[1]->location);
          }

          auto source = load(cells[2], env);
          if (source->type == cell_type_e::LAMBDA ||
              source->type == cell_type_e::LIST) {
             throw processor_c::runtime_exception_c(
                 "fs::append source data must not be a list or lambda",
                 cells[2]->location);
          }

          std::ofstream os;
          os.open(item->data);

          os << source->data;

          os.close();
          return std::make_shared<cell_c>(CELL_TRUE);
       });

   _members_map["app"] = std::make_shared<cell_c>(
       [this](cells_t &cells, std::shared_ptr<environment_c> env) -> cell_ptr {
          if (cells.size() != 3) {
             throw processor_c::runtime_exception_c(
                 "fs::append operation expects exactly 2 parameter",
                 cells[0]->location);
          }

          auto item = load(cells[1], env);
          if (item->type != cell_type_e::STRING) {
             throw processor_c::runtime_exception_c(
                 "fs::append first parameter needs to be a string representing "
                 "a destination",
                 cells[1]->location);
          }

          auto source = load(cells[2], env);
          if (source->type == cell_type_e::LAMBDA ||
              source->type == cell_type_e::LIST) {
             throw processor_c::runtime_exception_c(
                 "fs::append source data must not be a list or lambda",
                 cells[2]->location);
          }

          std::ofstream os;
          os.open(item->data, std::ios::app);

          os << "\n" << source->data;

          os.close();
          return std::make_shared<cell_c>(CELL_TRUE);
       });
}

} // namespace modules
} // namespace sauros