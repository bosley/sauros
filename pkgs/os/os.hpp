#ifndef SAUROS_PKG_OS
#define SAUROS_PKG_OS

#include <cstdlib>
#include <sauros/sauros.hpp>

#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif

extern "C" {

/*
   Get the current working directory
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_cwd_(sauros::cells_t &cells,
             std::shared_ptr<sauros::environment_c> env);

/*
   List current directory
*/
API_EXPORT
extern sauros::cell_ptr _pkg_os_ls_(sauros::cells_t &cells,
                                    std::shared_ptr<sauros::environment_c> env);

/*
   Change working directory
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_chdir_(sauros::cells_t &cells,
               std::shared_ptr<sauros::environment_c> env);

/*
   Check if system is little endian
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_endian_(sauros::cells_t &cells,
                std::shared_ptr<sauros::environment_c> env);

/*
   Get the name of the os
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_os_name_(sauros::cells_t &cells,
                 std::shared_ptr<sauros::environment_c> env);

/*
   Check if an item is a file
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_is_file_(sauros::cells_t &cells,
                 std::shared_ptr<sauros::environment_c> env);

/*
   Check if an item is a directory
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_is_dir_(sauros::cells_t &cells,
                std::shared_ptr<sauros::environment_c> env);

/*
   Check if a path exists
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_exists_(sauros::cells_t &cells,
                std::shared_ptr<sauros::environment_c> env);

/*
   Create one or more dirs (string vs list of strings)
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_mkdir_(sauros::cells_t &cells,
               std::shared_ptr<sauros::environment_c> env);

/*
   Delete one or more files or directories  (string vs list of strings)
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_delete_(sauros::cells_t &cells,
                std::shared_ptr<sauros::environment_c> env);

/*
   Delete one or more files or directories recursively  (string vs list of
   strings)
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_delete_all_(sauros::cells_t &cells,
                    std::shared_ptr<sauros::environment_c> env);

/*
   Copy files or directories
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_copy_(sauros::cells_t &cells,
              std::shared_ptr<sauros::environment_c> env);

/*
   Append data to a file
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_file_append_(sauros::cells_t &cells,
                     std::shared_ptr<sauros::environment_c> env);

/*
   Append data to a file
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_file_write_(sauros::cells_t &cells,
                    std::shared_ptr<sauros::environment_c> env);

/*
   Read data from a file
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_file_read_(sauros::cells_t &cells,
                   std::shared_ptr<sauros::environment_c> env);

/*
   Clear the screen
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_clear_screen_(sauros::cells_t &cells,
                      std::shared_ptr<sauros::environment_c> env);

/*
   Get an environment variable
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_get_env_(sauros::cells_t &cells,
                 std::shared_ptr<sauros::environment_c> env);

/*
   Sleep ms
*/
API_EXPORT
extern sauros::cell_ptr
_pkg_os_sleep_ms_(sauros::cells_t &cells,
                  std::shared_ptr<sauros::environment_c> env);
}

#endif