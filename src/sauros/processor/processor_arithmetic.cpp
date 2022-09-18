#include "processor.hpp"

namespace sauros {

cell_c processor_c::perform_arithmetic(std::string op,
                                       std::vector<cell_c> &cells,
                                       std::function<double(double, double)> fn,
                                       std::shared_ptr<environment_c> env,
                                       bool force_double) {

   if (cells.size() < 3) {
      throw runtime_exception_c("Expected a list size of at least 3 items",
                                cells[0].location);
   }

   double result = 0.0;
   bool store_as_double{false};

   auto first_cell_value = process_cell(cells[1], env);
   if (!first_cell_value.has_value()) {
      throw runtime_exception_c("Unknown operand given to '" + op + "'",
                                cells[1].location);
   }
   try {
      result = std::stod((*first_cell_value).data);
   } catch (const std::invalid_argument &) {
      throw runtime_exception_c("Invalid data type given for operand",
                                cells[0].location);
   } catch (const std::out_of_range &) {
      throw runtime_exception_c("Item caused out of range exception",
                                cells[0].location);
   }

   for (auto i = cells.begin() + 2; i != cells.end(); ++i) {

      auto cell_value = process_cell((*i), env);
      if (!cell_value) {
         throw runtime_exception_c("Unknown operand given to '" + op + "'",
                                   (*i).location);
      }

      if ((*cell_value).type == cell_type_e::DOUBLE) {
         store_as_double = true;
      } else if ((*cell_value).type != cell_type_e::INTEGER) {
         throw runtime_exception_c("Invalid type for operand", (*i).location);
      }

      try {
         result = fn(result, std::stod((*cell_value).data));
      } catch (const std::invalid_argument &) {
         throw runtime_exception_c("Invalid data type given for operand",
                                   (*i).location);
      } catch (const std::out_of_range &) {
         throw runtime_exception_c("Item caused out of range exception",
                                   (*i).location);
      }
   }

   if (force_double || store_as_double) {
      return cell_c(cell_type_e::DOUBLE, std::to_string(result),
                    cells[0].location);
   } else {
      return cell_c(cell_type_e::INTEGER,
                    std::to_string(static_cast<int64_t>(result)),
                    cells[0].location);
   }
}

} // namespace sauros