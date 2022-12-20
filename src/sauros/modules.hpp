#ifndef SAUROS_AAAAMODULES_HPP
#define SAUROS_AAAAMODULES_HPP

#include "cell.hpp"
#include "modules/module_if.hpp"
#include <unordered_map>
#include <vector>

namespace sauros {

//! \brief An object that holds onto all of the modules
class modules_c {
 public:
   modules_c();
   ~modules_c();

   //! \brief Check if the modules object contains a module with a given name
   //! \param module_name The module name to check for
   //! \returns true iff the module exists in the module map
   bool contains(const std::string module_name);

   //! \brief Populate a given environment with the data from a
   //!        specified module
   //! \param module_name The name of the module to add data from
   //! \param env The environment to add data to
   //! \note This method does not ensure that the item exists in the map
   //!       so calling this without ensuring that the module exists with
   //!       the `contains` method can result in a crash
   void populate_environment(const std::string module_name,
                             std::shared_ptr<environment_c> &env);

 private:
   std::unordered_map<std::string, module_if *> _modules;
};

} // namespace sauros

#endif