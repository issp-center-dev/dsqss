if [ $# -ne 1 ]; then
  echo "Usage: $0 <version>"
  exit 1
fi

version=$1

ROOT_DIR=`pwd`
if [ -z "$(grep 'project(DSQSS' $ROOT_DIR/CMakeLists.txt 2>/dev/null)" ]; then
  echo "ERROR: current directory is not the root directory of the DSQSS codes"
  exit 1
fi

if [ ! -d $ROOT_DIR/.git ]; then
  echo "ERROR: this is not a git repository"
  exit 1
fi

if [ -z "$(git archive -h 2>&1 | grep -e '--add-file')" ]; then
  echo "ERROR: git is too old (git archive --add-file is not available)"
  exit 1
fi

cd $ROOT_DIR
rm -rf build-doc
rm -f DSQSS_jp.pdf DSQSS_en.pdf
mkdir build-doc
cd build-doc
cmake -DDocument=ON ../
for lang in jp en; do
  make doc-${lang}-pdf
  cp doc/${lang}/pdf/DSQSS.pdf ../DSQSS_${lang}.pdf
done
cd $ROOT_DIR

for lang in jp en; do
  if [ ! -f DSQSS_${lang}.pdf ]; then
    echo "ERROR: failed to build the manual (DSQSS_${lang}.pdf)"
    exit 1
  fi
done


# The tarball is made from HEAD. The commit hash is filled in .git_archival.txt
# (export-subst in .gitattributes) for the version information of the programs
# built from the tarball.
if [ -n "$(git status --porcelain --untracked-files=no)" ]; then
  echo "WARNING: changes which are not committed are not included in the tarball"
fi

git archive \
  --format=tar.gz \
  --prefix=DSQSS-${version}/ \
  --add-file=DSQSS_jp.pdf \
  --add-file=DSQSS_en.pdf \
  -o DSQSS-${version}.tar.gz \
  HEAD
