# Version information of DSQSS
#
# The version number is set as DSQSS_VERSION in the top-level CMakeLists.txt,
# and nowhere else. The programs print it together with the commit hash, as
# "v2.1.0 (7b79e710)", by the --version option.
#
# The commit hash is the first 8 digits of
#   - the hash of HEAD, in a git repository
#   - the hash of the commit which the tarball was made from, in a tarball
#     (.git_archival.txt filled in by "git archive", which make_archive.sh and
#     GitHub make the tarballs by)
#   - "unknown", otherwise
# followed by "-dirty" if files under the version control have changes which
# are not committed in the repository, as "v2.1.0 (7b79e710-dirty)". Files
# which are not under the version control are not taken into account, as
# "git describe --dirty".

# dsqss_normalize_version(<variable>)
#
# removes the leading "v" of the version number (both 2.1.0 and v2.1.0 are
# accepted), and checks if it is available as a version of a python package
function(dsqss_normalize_version var)
  string(STRIP "${${var}}" version)
  string(REGEX REPLACE "^[vV]" "" version "${version}")

  set(pre "[-_.]?(a|b|c|rc|alpha|beta|pre|preview)[-_.]?[0-9]*")
  set(post "[-_.]?(post|rev|r)[-_.]?[0-9]*")
  set(dev "[-_.]?dev[-_.]?[0-9]*")
  if(NOT version MATCHES "^[0-9]+(\\.[0-9]+)*(${pre})?(${post})?(${dev})?$")
    message(FATAL_ERROR
      "DSQSS_VERSION \"${${var}}\" is not available as a version number.\n"
      "It has to be numbers separated by dots, optionally followed by a suffix "
      "of a pre-release, a post-release, or a development release, "
      "such as 2.1.0, 2.2-rc1, and 2.2-dev (PEP 440).")
  endif()
  set(${var} "${version}" PARENT_SCOPE)
endfunction()

# dsqss_get_git_hash(<source directory> <variable>)
function(dsqss_get_git_hash source_dir var)
  set(hash "")
  set(dirty "")
  if(EXISTS "${source_dir}/.git")
    if(NOT GIT_EXECUTABLE)
      find_package(Git QUIET)
    endif()
    if(GIT_EXECUTABLE)
      execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
        WORKING_DIRECTORY "${source_dir}"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE hash
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
      if(NOT result EQUAL 0)
        set(hash "")
      endif()

      execute_process(
        COMMAND "${GIT_EXECUTABLE}" status --porcelain --untracked-files=no
        WORKING_DIRECTORY "${source_dir}"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE changes
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
      if(result EQUAL 0 AND NOT "${changes}" STREQUAL "")
        set(dirty "-dirty")
      endif()
    endif()
  elseif(EXISTS "${source_dir}/.git_archival.txt")
    file(STRINGS "${source_dir}/.git_archival.txt" lines LIMIT_COUNT 1)
    string(STRIP "${lines}" hash)
    # "$Format:%H$" is left if it is not filled in
    if(NOT hash MATCHES "^[0-9a-f]+$")
      set(hash "")
    endif()
  endif()

  string(LENGTH "${hash}" length)
  if(length LESS 8)
    set(hash "unknown")
  else()
    string(SUBSTRING "${hash}" 0 8 hash)
    set(hash "${hash}${dirty}")
  endif()
  set(${var} "${hash}" PARENT_SCOPE)
endfunction()

# dsqss_generate_version_file(<source directory> <version> <template> <output>)
#
# fills in the references to DSQSS_VERSION, DSQSS_GIT_HASH, and
# DSQSS_VERSION_STRING of the template. The output is left untouched unless
# the content changes.
function(dsqss_generate_version_file source_dir version template output)
  # configure_file fills in a reference to a variable which is not set, or is
  # empty, with nothing, and does not complain. Stop here instead of building
  # programs which show an empty version.
  if("${version}" STREQUAL "")
    message(FATAL_ERROR "the version number is not given for ${output}")
  endif()
  file(READ "${template}" content)
  # (bracket arguments, which no version of CMake expands the references in)
  string(REGEX MATCHALL [=[@DSQSS_[A-Za-z0-9_]*@]=] references "${content}")
  foreach(reference ${references})
    if(NOT reference MATCHES
        [=[^@(DSQSS_VERSION|DSQSS_GIT_HASH|DSQSS_VERSION_STRING)@$]=])
      message(FATAL_ERROR
        "${reference} in ${template} is not a reference to the version information")
    endif()
  endforeach()

  set(DSQSS_VERSION "${version}")
  dsqss_get_git_hash("${source_dir}" DSQSS_GIT_HASH)
  set(DSQSS_VERSION_STRING "v${DSQSS_VERSION} (${DSQSS_GIT_HASH})")

  set(candidate "${output}.new")
  configure_file("${template}" "${candidate}" @ONLY)

  if(EXISTS "${output}")
    execute_process(
      COMMAND "${CMAKE_COMMAND}" -E compare_files "${candidate}" "${output}"
      RESULT_VARIABLE changed
      OUTPUT_QUIET ERROR_QUIET)
    if(changed)
      # make which compares the time stamps by the second (GNU Make 3.81 of
      # macOS, for example) does not compile the sources again if the output is
      # written within the same second as the object files of the last build,
      # and the programs keep the old version. Wait so that it gets a later one.
      execute_process(COMMAND "${CMAKE_COMMAND}" -E sleep 1)
      configure_file("${template}" "${output}" @ONLY)
    endif()
  else()
    configure_file("${template}" "${output}" @ONLY)
  endif()
  file(REMOVE "${candidate}")
endfunction()

# dsqss_version_file_command(<variable> <template> <output>)
#
# gives the arguments of COMMAND to generate the file when DSQSS is built, so
# that the commit hash follows the repository without running cmake again
function(dsqss_version_file_command var template output)
  set(${var}
    "${CMAKE_COMMAND}"
    "-DDSQSS_SOURCE_DIR=${CMAKE_SOURCE_DIR}"
    "-DDSQSS_VERSION=${DSQSS_VERSION}"
    "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}"
    "-DTEMPLATE=${template}"
    "-DOUTPUT=${output}"
    -P "${CMAKE_SOURCE_DIR}/cmake/GenerateVersionFile.cmake"
    PARENT_SCOPE)
endfunction()
