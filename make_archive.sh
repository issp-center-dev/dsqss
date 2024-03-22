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

res=0
type git-archive-all >/dev/null 2>&1 || res=1
if [ $res -eq 1 ]; then
  echo "ERROR: git-archive-all is not installed"
  exit 1
fi

cd $ROOT_DIR
rm -rf build-doc
mkdir build-doc
cd build-doc
cmake -DDocument=ON ../
for lang in jp en; do
  make doc-${lang}-pdf
  cp doc/${lang}/pdf/DSQSS.pdf ../DSQSS_${lang}.pdf
done
cd $ROOT_DIR


git submodule update -i -r
git-archive-all \
  --prefix=DSQSS-${version} \
  --extra=DSQSS_jp.pdf \
  --extra=DSQSS_en.pdf \
  DSQSS-${version}.tar.gz
