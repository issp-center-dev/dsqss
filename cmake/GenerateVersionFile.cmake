# Script to generate a file with the version information when DSQSS is built
#
#   cmake -DDSQSS_SOURCE_DIR=... -DDSQSS_VERSION=... -DGIT_EXECUTABLE=...
#         -DTEMPLATE=... -DOUTPUT=... -P GenerateVersionFile.cmake

include("${CMAKE_CURRENT_LIST_DIR}/DsqssVersion.cmake")

dsqss_generate_version_file(
  "${DSQSS_SOURCE_DIR}" "${DSQSS_VERSION}" "${TEMPLATE}" "${OUTPUT}")
