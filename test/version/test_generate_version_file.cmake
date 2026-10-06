# Test of cmake/GenerateVersionFile.cmake, the script which generates the files
# with the version information when DSQSS is built
#
#   cmake -DDSQSS_CMAKE_DIR=... -DWORK_DIR=... [-DGIT_EXECUTABLE=...]
#         -P test_generate_version_file.cmake
#
# The cases of a git repository are skipped if GIT_EXECUTABLE is not given.
#
# The references in the templates are written as bracket arguments, which no
# version of CMake expands.

cmake_minimum_required(VERSION 3.2...3.20.2)

set(script "${DSQSS_CMAKE_DIR}/GenerateVersionFile.cmake")
set(source_dir "${WORK_DIR}/source")
set(template "${WORK_DIR}/template.in")
set(output "${WORK_DIR}/output")
# git given to the script; none until the cases of a git repository
set(script_git "")

# generate(<version> <variable for the exit status>)
function(generate version result)
  execute_process(
    COMMAND "${CMAKE_COMMAND}"
      "-DDSQSS_SOURCE_DIR=${source_dir}"
      "-DDSQSS_VERSION=${version}"
      "-DGIT_EXECUTABLE=${script_git}"
      "-DTEMPLATE=${template}"
      "-DOUTPUT=${output}"
      -P "${script}"
    RESULT_VARIABLE status
    OUTPUT_QUIET ERROR_QUIET)
  set(${result} "${status}" PARENT_SCOPE)
endfunction()

function(expect_success status what)
  if(NOT status EQUAL 0)
    message(FATAL_ERROR "${what}: the script failed")
  endif()
endfunction()

function(expect_output expected what)
  file(READ "${output}" content)
  if(NOT "${content}" STREQUAL "${expected}")
    message(FATAL_ERROR "${what}: the output is\n${content}\ninstead of\n${expected}")
  endif()
  if(EXISTS "${output}.new")
    message(FATAL_ERROR "${what}: ${output}.new is left")
  endif()
endfunction()

function(get_time var)
  file(TIMESTAMP "${output}" time "%Y%m%d%H%M%S" UTC)
  set(${var} "${time}" PARENT_SCOPE)
endfunction()

# a source tree which is neither a repository nor a tarball
file(REMOVE_RECURSE "${WORK_DIR}")
file(MAKE_DIRECTORY "${source_dir}")
file(WRITE "${template}" [[
version = "@DSQSS_VERSION@"
hash = "@DSQSS_GIT_HASH@"
string = "@DSQSS_VERSION_STRING@"
]])

# ---- the references are filled in ----

generate(1.2.3 status)
expect_success("${status}" "first generation")
expect_output(
  "version = \"1.2.3\"\nhash = \"unknown\"\nstring = \"v1.2.3 (unknown)\"\n"
  "first generation")
get_time(time_first)

# ---- the output is left untouched unless the content changes ----

generate(1.2.3 status)
expect_success("${status}" "generation without changes")
expect_output(
  "version = \"1.2.3\"\nhash = \"unknown\"\nstring = \"v1.2.3 (unknown)\"\n"
  "generation without changes")
get_time(time_unchanged)
if(NOT time_unchanged STREQUAL time_first)
  message(FATAL_ERROR "generation without changes: the output is written again")
endif()

# nor does the script wait for a second; three generations take less than
# two seconds without the waits, and three seconds or more with them.
# (string(TIMESTAMP) supports %s since CMake 3.6)
if(NOT CMAKE_VERSION VERSION_LESS 3.6)
  string(TIMESTAMP time_start "%s" UTC)
  foreach(i 1 2 3)
    generate(1.2.3 status)
    expect_success("${status}" "generation without changes")
  endforeach()
  string(TIMESTAMP time_end "%s" UTC)
  math(EXPR elapsed "${time_end} - ${time_start}")
  if(elapsed GREATER 1)
    message(FATAL_ERROR
      "generation without changes: three generations took ${elapsed} seconds; "
      "the script waits although the output does not change")
  endif()
endif()

# ---- the changed output gets a time stamp of a later second ----
# (otherwise make which compares the time stamps by the second does not
# compile the sources again)

generate(1.2.4 status)
expect_success("${status}" "generation with a new version")
expect_output(
  "version = \"1.2.4\"\nhash = \"unknown\"\nstring = \"v1.2.4 (unknown)\"\n"
  "generation with a new version")
get_time(time_changed)
if(NOT time_changed STRGREATER time_first)
  message(FATAL_ERROR
    "generation with a new version: the time stamp ${time_changed} is not "
    "later than ${time_first} by the second")
endif()

# ---- the commit hash of a tarball ----

file(WRITE "${source_dir}/.git_archival.txt"
  "0123456789abcdef0123456789abcdef01234567\n")
generate(1.2.4 status)
expect_success("${status}" "tarball")
expect_output(
  "version = \"1.2.4\"\nhash = \"01234567\"\nstring = \"v1.2.4 (01234567)\"\n"
  "tarball")

file(WRITE "${source_dir}/.git_archival.txt" [[
$Format:%H$
]])
generate(1.2.4 status)
expect_success("${status}" "tarball which is not made by git archive")
expect_output(
  "version = \"1.2.4\"\nhash = \"unknown\"\nstring = \"v1.2.4 (unknown)\"\n"
  "tarball which is not made by git archive")

# ---- a reference which the script does not fill in is an error ----
# (configure_file would fill it in with nothing)

file(WRITE "${template}" [[
version = "@DSQSS_VERSION@"
other = "@DSQSS_NOT_DEFINED@"
]])
generate(1.2.5 status)
if(status EQUAL 0)
  message(FATAL_ERROR "unknown reference: the script did not fail")
endif()
expect_output(
  "version = \"1.2.4\"\nhash = \"unknown\"\nstring = \"v1.2.4 (unknown)\"\n"
  "unknown reference")

# ---- an empty version number is an error ----

file(WRITE "${template}" [[
version = "@DSQSS_VERSION@"
hash = "@DSQSS_GIT_HASH@"
string = "@DSQSS_VERSION_STRING@"
]])
generate("" status)
if(status EQUAL 0)
  message(FATAL_ERROR "empty version number: the script did not fail")
endif()
expect_output(
  "version = \"1.2.4\"\nhash = \"unknown\"\nstring = \"v1.2.4 (unknown)\"\n"
  "empty version number")

# ---- the commit hash of a git repository ----

if(NOT GIT_EXECUTABLE)
  message(STATUS "git is not given; the cases of a git repository are skipped")
  message(STATUS "all the tests passed")
  return()
endif()

set(repository "${WORK_DIR}/repository")
set(script_git "${GIT_EXECUTABLE}")

# run_git(<directory> <arguments>...)
function(run_git dir)
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" -c user.name=test -c user.email=test@example.com
      -c commit.gpgsign=false ${ARGN}
    WORKING_DIRECTORY "${dir}"
    RESULT_VARIABLE status
    OUTPUT_QUIET ERROR_VARIABLE error)
  if(NOT status EQUAL 0)
    message(FATAL_ERROR "git ${ARGN} failed:\n${error}")
  endif()
endfunction()

# expect_hash(<hash> <what>)
function(expect_hash hash what)
  expect_output(
    "version = \"1.2.4\"\nhash = \"${hash}\"\nstring = \"v1.2.4 (${hash})\"\n"
    "${what}")
endfunction()

file(MAKE_DIRECTORY "${repository}")
file(WRITE "${repository}/tracked.txt" "tracked\n")
run_git("${repository}" init)
run_git("${repository}" add tracked.txt)
run_git("${repository}" commit -m test)
execute_process(
  COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
  WORKING_DIRECTORY "${repository}"
  OUTPUT_VARIABLE head
  OUTPUT_STRIP_TRAILING_WHITESPACE)
string(SUBSTRING "${head}" 0 8 head)

set(source_dir "${repository}")

generate(1.2.4 status)
expect_success("${status}" "repository")
expect_hash("${head}" "repository")

file(APPEND "${repository}/tracked.txt" "changed\n")
generate(1.2.4 status)
expect_success("${status}" "repository with a changed file")
expect_hash("${head}-dirty" "repository with a changed file")

file(WRITE "${repository}/tracked.txt" "tracked\n")
generate(1.2.4 status)
expect_success("${status}" "repository after the change is reverted")
expect_hash("${head}" "repository after the change is reverted")

# files which are not under the version control are not taken into account
file(WRITE "${repository}/untracked.txt" "untracked\n")
generate(1.2.4 status)
expect_success("${status}" "repository with an untracked file")
expect_hash("${head}" "repository with an untracked file")

# the repository is preferred to .git_archival.txt
file(WRITE "${repository}/.git_archival.txt"
  "0123456789abcdef0123456789abcdef01234567\n")
generate(1.2.4 status)
expect_success("${status}" "repository with .git_archival.txt")
expect_hash("${head}" "repository with .git_archival.txt")

# .git is a file in a worktree made by "git worktree add"
run_git("${repository}" worktree add --detach "${WORK_DIR}/worktree" HEAD)
set(source_dir "${WORK_DIR}/worktree")
generate(1.2.4 status)
expect_success("${status}" "worktree")
expect_hash("${head}" "worktree")

message(STATUS "all the tests passed")
