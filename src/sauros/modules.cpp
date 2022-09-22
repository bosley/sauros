#include "modules.hpp"
#include "environment.hpp"
#include "modules/io/io.hpp"
#include "modules/math/math.hpp"

namespace sauros {

modules_c::modules_c() {
   _modules["io"] = new modules::io_c();
   _modules["math"] = new modules::math_c();
}

modules_c::~modules_c() {
   for (auto [name, module] : _modules) {
      if (module) {
         delete module;
      }
   }
}

bool modules_c::contains(const std::string module_name) {
   return (_modules.find(module_name) != _modules.end());
}

void modules_c::populate_environment(const std::string module_name, std::shared_ptr<environment_c> &env) {

   for(auto [name, cell] : _modules[module_name]->get_members()) {
      std::string member_name = module_name + "::" + name;
      env->set(member_name, cell);
   }
}

}